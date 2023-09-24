#ifndef PICOUTIL_H
    #error "Include picoutil.h instead of picoutil_opt_buf.h"
#endif

#ifdef OPT_BUF
    #undef OPT_BUF
#endif
#define OPT_BUF(...) OPT_BUF_PRIM(__VA_ARGS__)

#ifdef OPT_BUF_PRIM
    #undef OPT_BUF_PRIM
#endif
#define OPT_BUF_PRIM(...) PP_CAT(OPT_BUF_, CHAOS_PP_VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__)

#ifdef OPT_BUF_1
    #undef OPT_BUF_1
#endif
#define OPT_BUF_1(TYPE1) struct { __unused char _[sizeof(TYPE1)]; }

#ifdef OPT_BUF_2
    #undef OPT_BUF_2
#endif
#define OPT_BUF_2(TYPE1, TYPE2) struct { __unused char _[sizeof(TYPE1) > sizeof(TYPE2) ? sizeof(TYPE1) : sizeof(TYPE2)]; }

#ifdef OPT_BUF_3
    #undef OPT_BUF_3
#endif
#define OPT_BUF_3(TYPE1, TYPE2, TYPE3) struct { __unused char _[sizeof(TYPE1) > sizeof(TYPE2) ? (sizeof(TYPE1) > sizeof(TYPE3) ? sizeof(TYPE1) : sizeof(TYPE3)) : (sizeof(TYPE2) > sizeof(TYPE3) ? sizeof(TYPE2) : sizeof(TYPE3))]; }

#ifdef OPT_BUF_4
    #undef OPT_BUF_4
#endif
#define OPT_BUF_4(TYPE1, TYPE2, TYPE3, TYPE4) struct { __unused char _[sizeof(OPT_BUF_3(TYPE1, TYPE2, TYPE3)) > sizeof(TYPE4) ? sizeof(OPT_BUF_3(TYPE1, TYPE2, TYPE3)) : sizeof(TYPE4)]; }

#ifdef OPT_BUF_5
    #undef OPT_BUF_5
#endif
#define OPT_BUF_5(TYPE1, TYPE2, TYPE3, TYPE4, TYPE5) struct { __unused char _[sizeof(OPT_BUF_4(TYPE1, TYPE2, TYPE3, TYPE4)) > sizeof(TYPE5) ? sizeof(OPT_BUF_4(TYPE1, TYPE2, TYPE3, TYPE4)) : sizeof(TYPE5)]; }

#ifdef OPT_BUF_6
    #undef OPT_BUF_6
#endif
#define OPT_BUF_6(TYPE1, TYPE2, TYPE3, TYPE4, TYPE5, TYPE6) struct { __unused char _[sizeof(OPT_BUF_5(TYPE1, TYPE2, TYPE3, TYPE4, TYPE5)) > sizeof(TYPE6) ? sizeof(OPT_BUF_5(TYPE1, TYPE2, TYPE3, TYPE4, TYPE5)) : sizeof(TYPE6)]; }

#ifdef OPT_BUF_7
    #undef OPT_BUF_7
#endif
#define OPT_BUF_7(TYPE1, TYPE2, TYPE3, TYPE4, TYPE5, TYPE6, TYPE7) struct { __unused char _[sizeof(OPT_BUF_6(TYPE1, TYPE2, TYPE3, TYPE4, TYPE5, TYPE6)) > sizeof(TYPE7) ? sizeof(OPT_BUF_6(TYPE1, TYPE2, TYPE3, TYPE4, TYPE5, TYPE6)) : sizeof(TYPE7)]; }
