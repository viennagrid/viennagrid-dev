#ifndef VIENNAMETA_UTILS_HPP
#define VIENNAMETA_UTILS_HPP


namespace viennameta
{
    template<typename type1, typename type2>
    struct _equal
    {
        static const bool value = false;
    };
    
    template<typename type>
    struct _equal<type,type>
    {
        static const bool value = true;
    };
    
    template<bool condition, typename type1, typename type2>
    struct _if
    {
        typedef type1 result;
    };
    
    template<typename type1, typename type2>
    struct _if<false, type1, type2>
    {
        typedef type2 result;
    };

    
    // an empty type
    class null_type {};
    
    class out_of_range {};
    class not_found {};
    
    
    // a static type pair
    template<typename first_, typename second_>
    struct static_pair
    {
        typedef first_ first;
        typedef second_ second;
    };
    
    
    // a tag wrapper
    template<typename foo>
    class tag {};
}


#endif