#ifndef VIENNAGRID_STORAGE_ALGORITHM_HPP
#define VIENNAGRID_STORAGE_ALGORITHM_HPP


namespace viennameta
{
    namespace typelist
    {
        template<typename typelist>
        struct for_each_impl {};
        
        template<typename head, typename tail>
        struct for_each_impl< viennameta::typelist_t<head, tail> >
        {
            template<typename functor>
            static void exec( functor & f)
            {
                f( tag<head>() );
                for_each_impl<tail>::exec(f);
            }
            
            template<typename functor>
            static void exec( const functor & f)
            {
                f( tag<head>() );
                for_each_impl<tail>::exec(f);
            }
        };
        
        template<>
        struct for_each_impl< viennameta::null_type >
        {
            template<typename functor> static void exec( functor & f) {}
            template<typename functor> static void exec( const functor & f) {}
        };

        
        template<typename typelist, typename functor>
        void for_each(functor & f)
        { for_each_impl<typelist>::exec(f); }
        
        template<typename typelist, typename functor>
        void for_each(const functor & f)
        { for_each_impl<typelist>::exec(f); }
    }
}

#endif
