
#include <picoutil.h>

#ifndef PICOUTIL_STATIC_BYTES_SIZE
    #error PICOUTIL_STATIC_BYTES_SIZE not defined
#endif

#include <pico/stdlib.h>
#include <pico/sync.h>
#include <pico/bootrom.h>

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h> // memcpy, memset
#include <math.h>
#include <stdlib.h>

// TODO: Do we need to communicate with other core and request it to stop when performing memory operations? (Are the mutexes enough ?) (At least they should)
// Maybe create two pools for both cores and put them in .scratch_{x,y} sections, and create a third common pool for both cores
// (with options / parameters controlling source of each allocation)

#ifdef DECAY
    #undef DECAY
#endif
#define DECAY(ARR) (&(*(ARR)) + 0)

#ifdef IS_IN_CHUNK
    #undef IS_IN_CHUNK
#endif
#define IS_IN_CHUNK(HDR, PTR) ((uintptr_t)(PTR) >= (uintptr_t)((HDR)->chnk_limits[0]) && (uintptr_t)(PTR) < (uintptr_t)((HDR)->chnk_limits[1]))

#ifdef CHNK_SZ
    #undef CHNK_SZ
#endif
#define CHNK_SZ(HDR) ((uintptr_t)((HDR)->chnk_limits[1]) - (uintptr_t)((HDR)->chnk_limits[0]))

#ifdef DATA_ROOM
    #undef DATA_ROOM
#endif
#define DATA_ROOM(HDR) ((uintptr_t)((HDR)->chnk_limits[1]) - (uintptr_t)((HDR)->data_start))

#ifdef ENOUGH_ROOM
    #undef ENOUGH_ROOM
#endif
#define ENOUGH_ROOM(HDR, SIZE, ALIGN) (DATA_ROOM((HDR)) - align_ptr((HDR)->data_start, (ALIGN)).offset >= (SIZE))

#ifdef CAN_SPLIT
    #undef CAN_SPLIT
#endif
#define CAN_SPLIT(HDR, SIZE, ALIGN)                                         \
    (                                                                       \
        /* All data space */                                                \
        DATA_ROOM((HDR)) -                                                  \
        /* minus the space needed to satisfy the alignment requirement */   \
        align_ptr((HDR)->data_start, (ALIGN)).offset -                      \
        /* minus the space needed to align the next (new) header */         \
        align_ptr(                                                          \
            (void*)(                                                        \
                (uintptr_t)(HDR)->data_start +                              \
                align_ptr((HDR)->data_start, (ALIGN)).offset +              \
                (SIZE)                                                      \
            ),                                                              \
            alignof(memory_header_t)                                        \
        ).offset -                                                          \
        /* minus the space needed for the next (new) header */              \
        sizeof(memory_header_t) > (SIZE)                                    \
    )

#ifdef NEXT_CHNK
    #undef NEXT_CHNK
#endif
#define NEXT_CHNK(HDR) ((HDR)->chnk_limits[1])

#ifdef PREV_CHNK
    #undef PREV_CHNK
#endif
#define PREV_CHNK(HDR) ((HDR)->prev->chnk_limits[0])

#ifdef MEM_HDR_ALIGN
    #undef MEM_HDR_ALIGN
#endif
#define MEM_HDR_ALIGN(ADDR) (align_ptr((ADDR), alignof(memory_header_t)).offset + (ADDR))

typedef
    struct memory_header __aligned(alignof(max_align_t))
        memory_header_t;

struct memory_header
{
    byte_t* chnk_limits[2];
    memory_header_t* next;
    memory_header_t* prev;
    byte_t* data_start;
    bool free;
};

typedef struct memory_offset
{
    void* ptr;
    intptr_t offset;
} memory_offset_t;

alignas(alignof(memory_header_t))
static byte_t /* __in_flash("picoutil_static_bytes") */ __after_data("picoutil_static_bytes") picoutil_static_bytes[PICOUTIL_STATIC_BYTES_SIZE] = { 0 };
static bool picoutil_static_bytes_initialized = false;
static bool picoutil_static_allocator_safe = true;
auto_init_recursive_mutex(picoutil_static_bytes_mutex); // <=> static recursive_mutex_t picoutil_static_bytes_mutex; (but initialized)
static intptr_t first_hdr_off = 0;

__always_inline __wur
static inline bool check_alignment_requirements(void* ptr, size_t alignment)
{
    return MODu32((uintptr_t)ptr, alignment) == 0;
}

__always_inline __wur
static inline memory_offset_t align_ptr(void* ptr, size_t alignment)
{
    void* ptr_cpy = ptr;
    if (MODu32(alignment, 2) == 0)
    {
        void* aligned_ptr = (void*)(((uintptr_t)ptr + alignment - 1) & ~(alignment - 1));
        assert((uintptr_t)aligned_ptr >= (uintptr_t)ptr);
        return (memory_offset_t){aligned_ptr, (intptr_t)aligned_ptr - (intptr_t)ptr};
    }
    else
    {
        while (!check_alignment_requirements(ptr, alignment))
            ptr = (void*)((uintptr_t)ptr + 1);
        return (memory_offset_t){ptr, (intptr_t)ptr - (intptr_t)ptr_cpy};
    }
    __unreachable();
    return (memory_offset_t){NULL, 0};
}

bool picoutil_static_allocator_set_safe(bool safe)
{
    bool old = picoutil_static_allocator_safe;
    picoutil_static_allocator_safe = safe;
    return old;
}

bool picoutil_static_allocator_is_safe(void)
{
    return picoutil_static_allocator_safe;
}

void picoutil_static_allocator_init(bool safe)
{
    picoutil_static_allocator_safe = safe;
    if (!recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        recursive_mutex_init(&picoutil_static_bytes_mutex);
    // Add the first `memory_header_t` to the static bytes
    if (picoutil_static_bytes_initialized)
        return;
    memory_offset_t offset = align_ptr(DECAY(picoutil_static_bytes), alignof(memory_header_t));
    assert(offset.ptr != NULL);
    if (offset.ptr == NULL)
        picoutil_log(LOG_WARNING, "Failed to align pointer");
    assert(offset.offset == 0); // Because we aligned it
    if (offset.offset != 0)
        picoutil_log(LOG_WARNING, "Misaligned static bytes (wrong offset)");
    assert(check_alignment_requirements(offset.ptr, alignof(memory_header_t)));
    if (!check_alignment_requirements(offset.ptr, alignof(memory_header_t)))
        picoutil_log(LOG_WARNING, "Misaligned static bytes (wrong alignment)");
#if 0
    memory_header_t hdr = {
        .chnk_limits = { *(&picoutil_static_bytes), *(&picoutil_static_bytes) + PICOUTIL_STATIC_BYTES_SIZE },
        .next = NULL,
        .prev = NULL,
        .data_start = offset.ptr + sizeof(memory_header_t),
        .free = true
    };
    memcpy(offset.ptr, &hdr, sizeof(memory_header_t));
#else
    volatile memory_header_t* volatile hdr = (memory_header_t*)(byte_t*)offset.ptr;
    (*hdr).chnk_limits[0] = DECAY(picoutil_static_bytes);
    (*hdr).chnk_limits[1] = DECAY(picoutil_static_bytes) + PICOUTIL_STATIC_BYTES_SIZE;
    (*hdr).next = NULL;
    (*hdr).prev = NULL;
    (*hdr).data_start = offset.ptr + sizeof(memory_header_t);
    (*hdr).free = true;
#endif
    first_hdr_off = offset.offset;
    picoutil_static_bytes_initialized = true;
}

__malloc __wur __mallocsize(1) __allocalign(2)
void* __time_critical_func(picoutil_static_alloc_aligned)(size_t size, size_t requested_align)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return NULL;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    memory_header_t* hdr = (memory_header_t*)(picoutil_static_bytes + first_hdr_off);
    while (hdr)
    {
        if (hdr->free && ENOUGH_ROOM(hdr, size, requested_align))
        {
            if (CAN_SPLIT(hdr, size, requested_align))
            {
                memory_header_t new_hdr = {
                    .chnk_limits = { (byte_t*)((uintptr_t)hdr->data_start + align_ptr(hdr->data_start, requested_align).offset + size), hdr->chnk_limits[1] },
                    .next = hdr->next,
                    .prev = hdr,
                    .data_start = 
                        (byte_t*)((uintptr_t)hdr->data_start + align_ptr(hdr->data_start, requested_align).offset + size) + align_ptr((byte_t*)((uintptr_t)hdr->data_start + align_ptr(hdr->data_start, requested_align).offset + size), alignof(memory_header_t)).offset + sizeof(memory_header_t),
                    .free = true
                };
                hdr->chnk_limits[1] = (byte_t*)((uintptr_t)hdr->data_start + align_ptr(hdr->data_start, requested_align).offset + size);
                hdr->next = (memory_header_t*)(new_hdr.chnk_limits[0] + align_ptr(new_hdr.chnk_limits[0], alignof(memory_header_t)).offset);
                assert(hdr->next != NULL && (uintptr_t) hdr->next % alignof(memory_header_t) == 0);
                memcpy(hdr->next, &new_hdr, sizeof(memory_header_t));
            }
            hdr->free = false;
            hdr->data_start += align_ptr(hdr->data_start, requested_align).offset;
            recursive_mutex_exit(&picoutil_static_bytes_mutex);
            picoutil_log(LOG_DEBUG, "Allocated %zu bytes aligned to %zu at 0x%p", size, requested_align, hdr->data_start);
            return hdr->data_start;
        }
        hdr = hdr->next;
    }
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
    picoutil_log(LOG_WARNING, "Failed to allocate %zu bytes aligned to %zu (not enough room)", size, requested_align);
    return NULL;
}

__malloc __wur __mallocsize(1)
void* __time_critical_func(picoutil_static_alloc)(size_t size)
{
    return picoutil_static_alloc_aligned(size, alignof(max_align_t));
}

__malloc __wur __callocsize(1, 2) __allocalign(3)
void* __time_critical_func(picoutil_static_calloc_aligned)(size_t count, size_t size, size_t requested_align)
{
    void* ptr = picoutil_static_alloc_aligned(size * count, requested_align);
    if (ptr == NULL)
        return NULL;
    picoutil_static_allocator_safe ? picoutil_memset_explicit(ptr, 0, size * count) : memset(ptr, 0, size * count);
    return ptr;
}

__malloc __wur __callocsize(1, 2)
void* __time_critical_func(picoutil_static_calloc)(size_t count, size_t size)
{
    return picoutil_static_calloc_aligned(size, count, alignof(max_align_t));
}

// Merges all adjacent free chunks
static inline void merge_free_chunks(void)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    memory_header_t* hdr = (memory_header_t*)(picoutil_static_bytes + first_hdr_off);
    while (hdr)
    {
        if (hdr->free)
        {
            memory_header_t* next_hdr = hdr->next;
#if 0
            if (next_hdr != NULL && next_hdr->free)
            {
                hdr->chnk_limits[1] = next_hdr->chnk_limits[1];
                hdr->next = next_hdr->next;
                if (hdr->next != NULL)
                    hdr->next->prev = hdr;
                if (picoutil_static_allocator_safe)
                    picoutil_memset_explicit(next_hdr, 0, sizeof(memory_header_t));
            }
#else
            while (next_hdr != NULL && next_hdr->free)
            {
                hdr->chnk_limits[1] = next_hdr->chnk_limits[1];
                hdr->next = next_hdr->next;
                if (hdr->next != NULL)
                    hdr->next->prev = hdr;
                if (picoutil_static_allocator_safe)
                    picoutil_memset_explicit(next_hdr, 0, sizeof(memory_header_t));
                next_hdr = hdr->next;
            }
#endif
        }
        hdr = hdr->next;
    }
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
}

static void destroy_null_chunks(void)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    memory_header_t* hdr = (memory_header_t*)(picoutil_static_bytes + first_hdr_off);
    while (hdr)
    {
        if (hdr->chnk_limits[0] == hdr->chnk_limits[1])
        {
            if (hdr->prev != NULL)
                hdr->prev->next = hdr->next;
            if (hdr->next != NULL)
                hdr->next->prev = hdr->prev;
        }
        hdr = hdr->next;
    }
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
}

static void reorder_chunks(void)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    memory_header_t* hdr = (memory_header_t*)(picoutil_static_bytes + first_hdr_off);
    while (hdr)
    {
        if (hdr->prev != NULL && hdr->prev->chnk_limits[1] > hdr->chnk_limits[0])
        {
            memory_header_t* prev = hdr->prev;
            hdr->prev = prev->prev;
            prev->prev = hdr;
            prev->next = hdr->next;
            hdr->next = prev;
            if (hdr->prev != NULL)
                hdr->prev->next = hdr;
            if (prev->next != NULL)
                prev->next->prev = prev;
        }
        hdr = hdr->next;
    }
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
}

// TODO: Check if given pointer to free is in chunk rather than equal to data_start
void __time_critical_func(picoutil_static_free)(void* ptr)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    memory_header_t* hdr = (memory_header_t*)(picoutil_static_bytes + first_hdr_off);
    while (hdr)
    {
        if (hdr->data_start == ptr || IS_IN_CHUNK(hdr, ptr))
        {
            hdr->free = true;
            hdr->data_start = hdr->chnk_limits[0] + align_ptr(hdr->chnk_limits[0], alignof(memory_header_t)).offset + sizeof(memory_header_t);
            // if `picoutil_static_allocator_safe` is true, we need to clear the memory
            if (picoutil_static_allocator_safe)
                picoutil_memset_explicit(hdr->data_start, 0, DATA_ROOM(hdr));
            //destroy_null_chunks();
            //reorder_chunks();
            merge_free_chunks();
            recursive_mutex_exit(&picoutil_static_bytes_mutex);
            return;
        }
        hdr = hdr->next;
    }
    merge_free_chunks();
    if (picoutil_static_allocator_safe)
        picoutil_log(LOG_FATAL, "Failed to free pointer 0x%p (not found)", ptr);
    else
        picoutil_log(LOG_ERROR, "Failed to free pointer 0x%p (not found)", ptr);
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
}

void picoutil_static_allocator_dump_hdrs(void)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
    {
        printf("Allocator not initialized\n");
        return;
    }
    else
        printf("Allocator initialized\n");
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    memory_header_t* hdr = (memory_header_t*)(picoutil_static_bytes + first_hdr_off);
    while (hdr)
    {
        printf("Header at 0x%p (static bytes starting at 0x%p):\n", hdr, DECAY(picoutil_static_bytes));
        printf("\tfree: %s", hdr->free ? "true" : "false");
        printf("\tnext: 0x%p", hdr->next);
        printf("\tprev: 0x%p\n", hdr->prev);
        printf("\tchnk_limits: [0x%p, 0x%p]", hdr->chnk_limits[0], hdr->chnk_limits[1]);
        printf("\tchunk size: %zu\n", CHNK_SZ(hdr));
        printf("\tdata_start: 0x%p (sizeof(memory_header_t) == %zu)", hdr->data_start, sizeof(memory_header_t));
        printf("\tdata room: %zu\n", DATA_ROOM(hdr));
        hdr = hdr->next;
    }
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
}

void picoutil_static_allocator_memdump(uintptr_t range[2])
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
    {
        printf("Allocator not initialized\n");
        return;
    }
    else
        printf("Allocator initialized\n");
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    uintptr_t curr = range[0];
    while (curr < range[1])
    {
        byte_t b = picoutil_static_bytes[curr];
        printf("%02x ", b);
        curr++;
        if (curr % 16 == 0)
            printf("\n");
    }
    printf("\n");
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
}

uintptr_t picoutil_static_bytes_get_start_addr(void)
{
    return (uintptr_t)DECAY(picoutil_static_bytes);
}

uintptr_t picoutil_static_bytes_get_end_addr(void)
{
    return (uintptr_t)DECAY(picoutil_static_bytes) + PICOUTIL_STATIC_BYTES_SIZE;
}

void __time_critical_func(picoutil_static_free_all)(void)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    // Just clear every single byte
    if (picoutil_static_allocator_safe)
        picoutil_memset_explicit(picoutil_static_bytes, 0, PICOUTIL_STATIC_BYTES_SIZE);
    // Add the first `memory_header_t` to the static bytes
    memory_offset_t offset = align_ptr(DECAY(picoutil_static_bytes), alignof(memory_header_t));
    assert(offset.ptr != NULL);
    assert(offset.offset == 0); // Because we aligned it
    assert(check_alignment_requirements(offset.ptr, alignof(memory_header_t)));
    memory_header_t* hdr = (memory_header_t*)offset.ptr;
    (*hdr).chnk_limits[0] = DECAY(picoutil_static_bytes);
    (*hdr).chnk_limits[1] = DECAY(picoutil_static_bytes) + PICOUTIL_STATIC_BYTES_SIZE;
    (*hdr).next = NULL;
    (*hdr).prev = NULL;
    (*hdr).data_start = offset.ptr + sizeof(memory_header_t);
    (*hdr).free = true;
    first_hdr_off = offset.offset;
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
}

struct data_pointers 
{
    byte_t** ptrs;
    size_t count;
};

// Gets only used pointers
static inline struct data_pointers get_all_datptrs(void)
{
    struct data_pointers datptrs = { NULL, 0 };
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return datptrs;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    memory_header_t* hdr = (memory_header_t*)(picoutil_static_bytes + first_hdr_off);
    while (hdr)
    {
        if (!hdr->free)
        {
            datptrs.ptrs = realloc(datptrs.ptrs, (datptrs.count + 1) * sizeof(byte_t*));
            datptrs.ptrs[datptrs.count] = hdr->data_start;
            datptrs.count++;
        }
        hdr = hdr->next;
    }
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
    return datptrs;
}

void __time_critical_func(picoutil_static_free_all_except)(void** ptr, size_t count)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    struct data_pointers datptrs = get_all_datptrs();
    if (datptrs.count == 0 || datptrs.ptrs == NULL)
    {
        recursive_mutex_exit(&picoutil_static_bytes_mutex);
        return;
    }
    for (size_t i = 0; i < datptrs.count; i++)
    {
        bool found = false;
        for (size_t j = 0; j < count; j++)
        {
            if (datptrs.ptrs[i] == ptr[j])
            {
                found = true;
                break;
            }
        }
        if (!found)
            picoutil_static_free(datptrs.ptrs[i]);
    }
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
    free(datptrs.ptrs);
}

static memory_header_t* search_containing_chunk(void* ptr)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return NULL;
    recursive_mutex_enter_blocking(&picoutil_static_bytes_mutex);
    memory_header_t* hdr = (memory_header_t*)(picoutil_static_bytes + first_hdr_off);
    while (hdr)
    {
        if (IS_IN_CHUNK(hdr, ptr))
        {
            recursive_mutex_exit(&picoutil_static_bytes_mutex);
            return hdr;
        }
        hdr = hdr->next;
    }
    recursive_mutex_exit(&picoutil_static_bytes_mutex);
    return NULL;
}

// Reallocate previously allocated memory, but this time with a specified alignment
__wur
void* __time_critical_func(picoutil_static_realloc_aligned)(void* ptr, size_t size, size_t requested_align)
{
    if (!picoutil_static_bytes_initialized || !recursive_mutex_is_initialized(&picoutil_static_bytes_mutex))
        return NULL;
    memory_header_t* hdr = search_containing_chunk(ptr);
    if (hdr == NULL)
        return NULL;
    void* new_ptr = picoutil_static_alloc_aligned(size, requested_align);
    if (new_ptr == NULL)
        return NULL;
    // Copy the data to a new location
    memcpy(new_ptr, ptr, size);
    // Free the old location
    picoutil_static_free(ptr);
    // Return the new location
    return new_ptr;
}

__wur
void* __time_critical_func(picoutil_static_realloc)(void* ptr, size_t size)
{
    return picoutil_static_realloc_aligned(ptr, size, alignof(max_align_t));
}