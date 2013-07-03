#ifndef VIENNAGRID_STORAGE_HANDLE_HPP
#define VIENNAGRID_STORAGE_HANDLE_HPP

#include <iterator>
#include <vector>
#include <map>
#include "viennagrid/meta/typemap.hpp"
#include "viennagrid/storage/id.hpp"
#include "viennagrid/storage/forwards.hpp"


namespace viennagrid
{
    namespace storage
    {
        

        namespace handle
        {
            
            namespace result_of
            {
            
                template<typename base_container_type, typename handle_tag>
                struct handle_type
                {};
                
                template<typename base_container_type>
                struct handle_type<base_container_type, no_handle_tag>
                {
                    typedef viennagrid::meta::null_type type;
                };
                
                template<typename base_container_type>
                struct handle_type<base_container_type, pointer_handle_tag>
                {
                    typedef typename base_container_type::pointer type;
                };
                
                template<typename base_container_type>
                struct handle_type<base_container_type, iterator_handle_tag>
                {
                    typedef typename base_container_type::iterator type;
                };
                
                template<typename base_container_type>
                struct handle_type<base_container_type, id_handle_tag>
                {
                    typedef typename base_container_type::value_type::id_type type;
                };
                
                
                
                
                template<typename base_container_type, typename handle_tag>
                struct const_handle_type
                {};
                
                template<typename base_container_type>
                struct const_handle_type<base_container_type, no_handle_tag>
                {
                    typedef viennagrid::meta::null_type type;
                };
                
                template<typename base_container_type>
                struct const_handle_type<base_container_type, pointer_handle_tag>
                {
                    typedef typename base_container_type::const_pointer type;
                };
                
                template<typename base_container_type>
                struct const_handle_type<base_container_type, iterator_handle_tag>
                {
                    typedef typename base_container_type::const_iterator type;
                };
                
                template<typename base_container_type>
                struct const_handle_type<base_container_type, id_handle_tag>
                {
                    typedef typename base_container_type::value_type::const_id_type type;
                };
                
                
                
                
                // default = iterator
                template<typename handle_type>
                struct value_type
                {
                    typedef typename viennagrid::meta::IF<
                        viennagrid::meta::is_const_iterator<handle_type>::value,
                        const typename handle_type::value_type,
                        typename handle_type::value_type
                    >::type type;
                };
                
                // pointer
                template<typename value_type_>
                struct value_type< value_type_ * >
                {
                    typedef value_type_ type;
                };
                
                template<typename value_type_>
                struct value_type< const value_type_ * >
                {
                    typedef const value_type_ type;
                };
                
                // id
                template<typename value_type_, typename base_id_type_>
                struct value_type< smart_id_t<value_type_, base_id_type_> >
                {
                    typedef value_type_ type;
                };
                
                template<typename value_type_, typename base_id_type_>
                struct value_type< smart_id_t<const value_type_, base_id_type_> >
                {
                    typedef const value_type_ type;
                };
                
                
                
                
                // default = iterator
                template<typename handle_type>
                struct handle_tag
                {
                    typedef iterator_handle_tag type;
                };
                
                // no handle
                template<>
                struct handle_tag<viennagrid::meta::null_type>
                {
                    typedef no_handle_tag type;
                };
                
                // pointer
                template<typename value_type>
                struct handle_tag<value_type *>
                {
                    typedef pointer_handle_tag type;
                };
                
                // id
                template<typename value_type_, typename base_id_type_>
                struct handle_tag< smart_id_t<value_type_, base_id_type_> >
                {
                    typedef id_handle_tag type;
                };
            
            }
            
          
            

            // Pointer handle
            template<typename container_type, typename handle_type>
            typename result_of::value_type<handle_type>::type & dereference_handle( container_type & container, handle_type handle, pointer_handle_tag )
            { return *handle; }
            
            template<typename container_type, typename handle_type>
            typename result_of::value_type<handle_type>::type const & dereference_handle( container_type const & container, handle_type handle, pointer_handle_tag )
            { return *handle; }

            
            // Iterator handle
            template<typename container_type, typename handle_type>
            typename result_of::value_type<handle_type>::type & dereference_handle( container_type & container, handle_type handle, iterator_handle_tag )
            { return *handle; }
            
            template<typename container_type, typename handle_type>
            typename result_of::value_type<handle_type>::type const & dereference_handle( container_type const & container, handle_type handle, iterator_handle_tag )
            { return *handle; }
            
            
            // ID handle
            template<typename container_type, typename handle_type>
            typename result_of::value_type<handle_type>::type & dereference_handle( container_type & container, handle_type handle, id_handle_tag )
            {
                typedef typename result_of::value_type<handle_type>::type value_type;
                typedef typename storage::result_of::id_type<value_type>::type id_type;
                
                return *std::find_if(
                    container.begin(),
                    container.end(),
                    id_compare<id_type>(handle)
                );
            }
            
            template<typename container_type, typename handle_type>
            typename result_of::value_type<handle_type>::type const & dereference_handle( container_type const & container, handle_type handle, id_handle_tag )
            {
                typedef typename result_of::value_type<handle_type>::type value_type;
                typedef typename storage::result_of::id_type<value_type>::type id_type;
                
                return *std::find_if(
                    container.begin(),
                    container.end(),
                    id_compare<id_type>(handle)
                );
            }
            
            
//             // generic
//             template<typename container_type, typename handle_type>
//             typename result_of::value_type<handle_type>::type & dereference_handle( container_type & container, handle_type handle )
//             {
//                 return dereference_handle(container, handle, typename result_of::handle_tag<handle_type>::type());
//             }
//             
//             template<typename container_type, typename handle_type>
//             typename result_of::value_type<handle_type>::type const & dereference_handle( container_type const & container, handle_type handle )
//             {
//                 return dereference_handle(container, handle, typename result_of::handle_tag<handle_type>::type());
//             }
            
            
            
            
            template<typename container_type, typename value_type, typename handle_tag>
            struct handle_helper;

            
            template<typename container_type, typename value_type>
            struct handle_helper<container_type, value_type, no_handle_tag>
            {
                static typename result_of::handle_type<container_type, no_handle_tag>::type handle( container_type & container, value_type & value )
                { return typename result_of::handle_type<container_type, no_handle_tag>::type(); }
                
                static typename result_of::const_handle_type<container_type, no_handle_tag>::type handle( container_type const & container, value_type const & value )
                { return typename result_of::handle_type<container_type, no_handle_tag>::type(); }
            };
            
            template<typename container_type, typename value_type>
            struct handle_helper<container_type, value_type, iterator_handle_tag>
            {
                static typename result_of::handle_type<container_type, iterator_handle_tag>::type handle( container_type & container, value_type & value )
                {
                    for (typename container_type::iterator it = container.begin(); it != container.end(); ++it)
                        if ( &(*it) == &value ) return it;
                    return container.end();
                }
                            
                static typename result_of::const_handle_type<container_type, iterator_handle_tag>::type handle( container_type const & container, value_type const & value )
                {
                    for (typename container_type::const_iterator it = container.begin(); it != container.end(); ++it)
                        if ( &(*it) == &value ) return it;
                    return container.end();
                }
            };
            
            template<typename container_type, typename value_type>
            struct handle_helper<container_type, value_type, pointer_handle_tag>
            {
                static typename result_of::handle_type<container_type, pointer_handle_tag>::type handle( container_type & container, value_type & value )
                { return &value; }
                
                static typename result_of::const_handle_type<container_type, pointer_handle_tag>::type handle( container_type const & container, value_type const & value )
                { return &value; }
            };
            
            template<typename container_type, typename value_type>
            struct handle_helper<container_type, value_type, id_handle_tag>
            {
                static typename result_of::handle_type<container_type, id_handle_tag>::type handle( container_type & container, value_type & value )
                { return value.id(); }
                
                static typename result_of::const_handle_type<container_type, id_handle_tag>::type handle( container_type const & container, value_type const & value )
                { return value.id(); }
            };
            
            
            
            template<typename container_type, typename value_type, typename handle_tag>
            typename result_of::handle_type<container_type, handle_tag>::type handle( container_type & container, value_type & value, handle_tag )
            { return handle_helper<container_type, value_type, handle_tag>::handle( container,value ); }
            
            template<typename container_type, typename value_type, typename handle_tag>
            typename result_of::const_handle_type<container_type, handle_tag>::type handle( container_type const & container, value_type const & value, handle_tag )
            { return handle_helper<container_type, value_type, handle_tag>::handle( container,value ); }
            
            
            
        }
        
    }
}

#endif
