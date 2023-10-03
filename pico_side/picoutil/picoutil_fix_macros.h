#ifndef __always_inline
    #error "picoutil_fix_macros.h must be included after picoutil.h, after __restore_macro(__always_inline) and included files that make use of __always_inline"
#endif
__save_macro(__always_inline)
#undef __always_inline
#define __always_inline PICOUTIL_ALWAYS_INLINE
