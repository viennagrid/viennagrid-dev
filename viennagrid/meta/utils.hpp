#ifndef VIENNAGRID_META_UTILS_HPP
#define VIENNAGRID_META_UTILS_HPP

#include <iterator>


namespace viennagrid
{
  namespace meta
  {
    // basic operations
    template<typename type1, typename type2>
    struct EQUAL
    {
        static const bool value = false;
    };
    
    template<typename type>
    struct EQUAL<type,type>
    {
        static const bool value = true;
    };
    
    template<typename type1, typename type2>
    struct NOT_EQUAL
    {
        static const bool value = !EQUAL<type1, type2>::value;
    };
    
    template<bool condition, typename type1, typename type2>
    struct IF
    {
        typedef type1 type;
    };
    
    template<typename type1, typename type2>
    struct IF<false, type1, type2>
    {
        typedef type2 type;
    };
    
    template<typename _type>
    struct IDENTITY
    {
        typedef _type type;
    };
    
    
    // error generator
    template<bool error>
    struct STATIC_ASSERT
    {
        typedef void type;
    };
    
    template<>
    struct STATIC_ASSERT<false>
    {};


    
    // an empty type
    class null_type {};
    
    // some special types
    class out_of_range {};
    class not_found {};
    
    
    // a static type pair
    template<typename first_, typename second_>
    struct static_pair
    {
        typedef first_ first;
        typedef second_ second;
    };
    
    namespace result_of
    {
        template<typename pair>
        struct first
        {
            typedef not_found type;
        };
        
        template<typename first_, typename second_>
        struct first< static_pair<first_, second_> >
        {
            typedef first_ type;
        };
        
        template<typename pair>
        struct second
        {
            typedef not_found type;
        };
        
        template<typename first_, typename second_>
        struct second< static_pair<first_, second_> >
        {
            typedef second_ type;
        };
    }
    
    
    // a tag wrapper
    template<typename foo>
    class tag {};
    
    
    // a true functor
    struct true_predicate
    {
        template<typename type>
        bool operator()(const type &)
        {
            return true;
        }
    };
    
    
    
    
    
    
    template<typename some_type>
    struct remove_const
    {
        typedef some_type type;
    };
    
    template<typename some_type>
    struct remove_const<const some_type>
    {
        typedef some_type type;
    };
    
    
    
    // http://stackoverflow.com/questions/5423246/how-to-detect-if-a-type-is-an-iterator-or-const-iterator
    template<typename T>
    struct is_const_pointer { static const bool value = false; };

    template<typename T>
    struct is_const_pointer<const T*> { static const bool value = true; };

    template <typename TIterator>
    struct is_const_iterator
    {
        typedef typename std::iterator_traits<TIterator>::pointer pointer;
        static const bool value = is_const_pointer<pointer>::value;
    };
    
  } // namespace meta
} // namespace viennagrid


#endif
