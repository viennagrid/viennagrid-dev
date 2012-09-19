#ifndef VIENNAMETA_TYPEMAP_MACROS_HPP
#define VIENNAMETA_TYPEMAP_MACROS_HPP






#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_1(K1, V1) ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, ::viennameta::null_type>

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_2(K1, V1, K2, V2) ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_1(K2, V2) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_3(K1, V1, K2, V2, K3, V3) ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_2(K2, V2, K3, V3) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_4(K1, V1, K2, V2, K3, V3, K4, V4) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_3(K2, V2, K3, V3, K4, V4) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_5(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_4(K2, V2, K3, V3, K4, V4, K5, V5) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_6(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_5(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_7(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_6(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_8(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_7(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_9(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_8(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_10(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_9(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10) >
    
#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_11(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_10(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_12(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_11(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_13(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_12(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_14(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_13(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14) >

#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_15(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_14(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15) >
        
#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_16(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_15(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16) >
        
#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_17(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_16(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17) >
        
#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_18(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_17(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18) >
        
#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_19(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18, K19, V19) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_18(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18, K19, V19) >
        
#define VIENNAMETA_MAKE_TYPEMAP_UNSAFE_20(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18, K19, V19, K20, V20) \
    ::viennameta::typelist_t< ::viennameta::static_pair<K1,V1>, VIENNAMETA_MAKE_TYPEMAP_UNSAFE_19(K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
        K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18, K19, V19, K20, V20) >

        
        
        
        
        
#define VIENNAMETA_MAKE_TYPEMAP_1(K1, V1) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_1(K1, V1) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_2(K1, V1, K2, V2) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_2(K1, V1, K2, V2) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_3(K1, V1, K2, V2, K3, V3) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_3(K1, V1, K2, V2, K3, V3) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_4(K1, V1, K2, V2, K3, V3, K4, V4) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_4(K1, V1, K2, V2, K3, V3, K4, V4) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_5(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_5(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_6(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_6(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_7(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_7(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_8(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_8(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_9(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_9(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_10(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_10(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10) >::type
    
#define VIENNAMETA_MAKE_TYPEMAP_11(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_11(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11) >::type

#define VIENNAMETA_MAKE_TYPEMAP_12(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_12(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12) >::type
        
#define VIENNAMETA_MAKE_TYPEMAP_13(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_13(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13) >::type

#define VIENNAMETA_MAKE_TYPEMAP_14(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_14(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14) >::type
        
#define VIENNAMETA_MAKE_TYPEMAP_15(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_15(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15) >::type
        
#define VIENNAMETA_MAKE_TYPEMAP_16(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_16(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16) >::type
        
#define VIENNAMETA_MAKE_TYPEMAP_17(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_17(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17) >::type
        
#define VIENNAMETA_MAKE_TYPEMAP_18(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_18(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18) >::type

#define VIENNAMETA_MAKE_TYPEMAP_19(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18, K19, V19) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_19(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18, K19, V19) >::type

#define VIENNAMETA_MAKE_TYPEMAP_20(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18, K19, V19, K20, V20) \
    ::viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_UNSAFE_20(K1, V1, K2, V2, K3, V3, K4, V4, K5, V5, K6, V6, K7, V7, K8, V8, K9, V9, K10, V10, \
                                  K11, V11, K12, V12, K13, V13, K14, V14, K15, V15, K16, V16, K17, V17, K18, V18, K19, V19, K20, V20) >::type
                                                                                    
                                                                                    
                                                                                    
#endif

