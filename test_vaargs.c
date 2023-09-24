#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define __unused __attribute__((__unused__))

struct stuff
{
    int a;
    int b;
    int c;
};

struct stuff1
{
    int a;
    int b;
    int c;

    size_t d;
};

struct stuff2
{
    int a;
    int b;
    int c;

    size_t d;
};

__attribute__((__sentinel__))
bool va_func(int number, ...)
{
    struct stuff opt_stuff1 = { 0 };
    struct stuff opt_stuff2 = { 0 };
    void* opt_stuff1_ptr = NULL;
    void* opt_stuff2_ptr = NULL;
    va_list args;
    va_start(args, number);
    if ((opt_stuff1_ptr = va_arg(args, void*)) != NULL)
    {
        opt_stuff1 = *(struct stuff*)opt_stuff1_ptr;
    }
    else
        goto no_more_opt;
    if ((opt_stuff2_ptr = va_arg(args, void*)) != NULL)
    {
        opt_stuff2 = *(struct stuff*)opt_stuff2_ptr;
    }
    else
        goto no_more_opt;
no_more_opt:
    va_end(args);
    printf("number: %d\n", number);
    if (opt_stuff1_ptr != NULL)
        printf("opt_stuff1: %d %d %d\n", opt_stuff1.a, opt_stuff1.b, opt_stuff1.c);
    if (opt_stuff2_ptr != NULL)
        printf("opt_stuff2: %d %d %d\n", opt_stuff2.a, opt_stuff2.b, opt_stuff2.c);
    return true;
}

bool va_func2(int number, ...)
{
    struct { __unused char _[(sizeof(struct stuff1) > sizeof(struct stuff2) ? sizeof(struct stuff1) : sizeof(struct stuff2))]; } null_stuff = { 0 };
    struct { __unused char _[(sizeof(struct stuff1) > sizeof(struct stuff2) ? sizeof(struct stuff1) : sizeof(struct stuff2))]; } opt = { 0 };
    struct stuff1 opt_stuff1 = { 0 };
    struct stuff2 opt_stuff2 = { 0 };
    va_list args;
    va_start(args, number);
    opt = va_arg(args, typeof(opt));
    if (memcmp(&opt, &null_stuff, sizeof(struct stuff1)) == 0)
        goto no_more_opt;
    else
        memcpy(&opt_stuff1, &opt, sizeof(struct stuff1));
    printf("hi\n");
    memset(&opt, 0, sizeof(opt));
    opt = va_arg(args, typeof(opt));
    if (memcmp(&opt, &null_stuff, sizeof(struct stuff1)) == 0)
        goto no_more_opt;
    else
        memcpy(&opt_stuff2, &opt, sizeof(struct stuff2));
no_more_opt:
    va_end(args);
    printf("number: %d\n", number);

        printf("opt_stuff1: %d %d %d\n", opt_stuff1.a, opt_stuff1.b, opt_stuff1.c);
        printf("opt_stuff2: %d %d %d\n", opt_stuff2.a, opt_stuff2.b, opt_stuff2.c);
    return true;
}

int main(int argc, char const *argv[])
{
    struct stuff stuff1 = { 1, 2, 3 };
    struct stuff stuff2 = { 4, 5, 6 };
    
    va_func(1, NULL);
    va_func(2, &stuff1, NULL);
    va_func(3, &stuff1, &stuff2, NULL);
    puts("");
    
    struct stuff1 stuff1_1 = { 1, 2, 3, 4 };
    struct stuff2 stuff2_1 = { 4, 5, 6 };
    va_func2(1, (struct { __unused char _[(sizeof(struct stuff1) > sizeof(struct stuff2) ? sizeof(struct stuff1) : sizeof(struct stuff2))]; }) { 0 });
    va_func2(2, stuff1_1, (struct { __unused char _[(sizeof(struct stuff1) > sizeof(struct stuff2) ? sizeof(struct stuff1) : sizeof(struct stuff2))]; }) { 0 });
    va_func2(3, stuff1_1, stuff2_1, (struct { __unused char _[(sizeof(struct stuff1) > sizeof(struct stuff2) ? sizeof(struct stuff1) : sizeof(struct stuff2))]; }) { 0 });

    return 0;
}
