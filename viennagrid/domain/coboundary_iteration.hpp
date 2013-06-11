#ifndef VIENNAGRID_DOMAIN_COBOUNDARY_ITERATION_HPP
#define VIENNAGRID_DOMAIN_COBOUNDARY_ITERATION_HPP

#include "viennagrid/forwards.hpp"
#include "viennagrid/storage/forwards.hpp"
#include "viennagrid/domain/domain.hpp"
 
namespace viennagrid
{

    
    namespace result_of
    {
        
        
        template<typename domain_or_container_type, typename coboundary_type_or_tag, typename view_container_tag = viennagrid::storage::std_deque_tag>
        struct coboundary_container
        {
            typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
            typedef typename viennagrid::result_of::container_of_tag< typename viennagrid::result_of::container_collection<domain_or_container_type>::type, coboundary_tag >::type container_type;
            typedef typename viennagrid::storage::result_of::view<container_type, view_container_tag>::type type;
        };
        
        template<typename domain_or_container_type, typename coboundary_type_or_tag, typename view_container_tag = viennagrid::storage::std_deque_tag>
        struct const_coboundary_container
        {
            typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
            typedef typename viennagrid::result_of::container_of_tag< typename viennagrid::result_of::container_collection<domain_or_container_type>::type, coboundary_tag >::type container_type;
            typedef typename viennagrid::storage::result_of::view<const container_type, view_container_tag>::type type;
        };
        
        template<typename domain_or_container_type, typename coboundary_type_or_tag, typename view_container_tag>
        struct coboundary_container<const domain_or_container_type, coboundary_type_or_tag, view_container_tag>
        {
            typedef typename const_coboundary_container<domain_or_container_type, coboundary_type_or_tag, view_container_tag>::type type;
        };
        
        
        
        template<typename domain_or_container_type, typename coboundary_type_or_tag, typename view_container_tag = viennagrid::storage::std_deque_tag>
        struct coboundary_range
        {
            typedef viennagrid::storage::container_range_wrapper< typename coboundary_container<domain_or_container_type, coboundary_type_or_tag, view_container_tag>::type > type; 
        };
        
        template<typename domain_or_container_type, typename coboundary_type_or_tag, typename view_container_tag = viennagrid::storage::std_deque_tag>
        struct const_coboundary_range
        {
            typedef viennagrid::storage::container_range_wrapper< typename const_coboundary_container<domain_or_container_type, coboundary_type_or_tag, view_container_tag>::type > type; 
        };
        
        template<typename domain_or_container_type, typename coboundary_type_or_tag, typename view_container_tag>
        struct coboundary_range<const domain_or_container_type, coboundary_type_or_tag, view_container_tag>
        {
            typedef typename const_coboundary_range<domain_or_container_type, coboundary_type_or_tag, view_container_tag>::type type;
        };
        
    }
    
    
    
    
    template<typename coboundary_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::coboundary_container<domain_or_container_type, coboundary_type_or_tag>::type create_coboundary_container(domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::result_of::value_type< handle_type >::type element_type;
        typedef typename viennagrid::result_of::element_tag< element_type >::type element_tag;
        
        typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
        typedef typename viennagrid::result_of::element< domain_or_container_type, coboundary_tag >::type coboundary_type;
        
        typedef typename result_of::coboundary_container<domain_or_container_type, coboundary_type_or_tag>::type coboundary_container_type;
        
        coboundary_container_type coboundary_container;
        coboundary_container.set_base_container( viennagrid::storage::collection::get< coboundary_type >( viennagrid::container_collection(domain) ) );
        
        typedef typename viennagrid::result_of::element_range< domain_or_container_type, coboundary_tag >::type coboundary_range_type;
        typedef typename viennagrid::result_of::handle_iterator<coboundary_range_type>::type coboundary_handle_iterator;
        
        
        coboundary_range_type coboundary_range = viennagrid::elements<coboundary_tag>( domain );
        for (coboundary_handle_iterator chit = coboundary_range.handle_begin(); chit != coboundary_range.handle_end(); ++chit)
        {
            typedef typename viennagrid::result_of::element_range< coboundary_type, element_tag >::type element_on_coboundary_range_type;
            typedef typename viennagrid::result_of::handle_iterator< element_on_coboundary_range_type >::type handle_on_coboundary_iterator;
            
            coboundary_type & coboundary_element = viennagrid::dereference_handle(domain, *chit);
            
            element_on_coboundary_range_type element_on_coboundary_range = viennagrid::elements<element_tag>( coboundary_element );
            for (handle_on_coboundary_iterator ehcit = element_on_coboundary_range.handle_begin(); ehcit != element_on_coboundary_range.handle_end(); ++ehcit)
            {
                if ( *ehcit == handle )
                    coboundary_container.insert_handle( *chit );
            }   
        }

        return coboundary_container;
    }
    
    template<typename coboundary_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::const_coboundary_container<domain_or_container_type, coboundary_type_or_tag>::type create_coboundary_container(const domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::result_of::value_type< handle_type >::type element_type;
        typedef typename viennagrid::result_of::element_tag< element_type >::type element_tag;
        
        typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
        typedef typename viennagrid::result_of::element< domain_or_container_type, coboundary_tag >::type coboundary_type;
        
        typedef typename result_of::const_coboundary_container<domain_or_container_type, coboundary_type_or_tag>::type const_coboundary_container_type;
        
        const_coboundary_container_type coboundary_container;
        coboundary_container.set_base_container( viennagrid::storage::collection::get< coboundary_type >( viennagrid::container_collection(domain) ) );
        
        typedef typename viennagrid::result_of::const_element_range< domain_or_container_type, coboundary_tag >::type coboundary_range_type;
        typedef typename viennagrid::result_of::const_handle_iterator<coboundary_range_type>::type coboundary_handle_iterator;
        
        
        coboundary_range_type coboundary_range = viennagrid::elements<coboundary_tag>( domain );
        for (coboundary_handle_iterator chit = coboundary_range.handle_begin(); chit != coboundary_range.handle_end(); ++chit)
        {
            typedef typename viennagrid::result_of::const_element_range< coboundary_type, element_tag >::type element_on_coboundary_range_type;
            typedef typename viennagrid::result_of::const_handle_iterator< element_on_coboundary_range_type >::type handle_on_coboundary_iterator;
            
            const coboundary_type & coboundary_element = viennagrid::dereference_handle(domain, *chit);
            
            element_on_coboundary_range_type element_on_coboundary_range = viennagrid::elements<element_tag>( coboundary_element );
            for (handle_on_coboundary_iterator ehcit = element_on_coboundary_range.handle_begin(); ehcit != element_on_coboundary_range.handle_end(); ++ehcit)
            {
                if ( *ehcit == handle )
                    coboundary_container.insert_handle( *chit );
            }   
        }

        return coboundary_container;
    }
    
    
    template<typename coboundary_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::coboundary_range<domain_or_container_type, coboundary_type_or_tag>::type coboundary_elements(domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type element_type;
        typedef typename result_of::element_tag<coboundary_type_or_tag>::type coboundary_tag;
        typedef typename result_of::coboundary_container<domain_or_container_type, coboundary_type_or_tag>::type container_type;
        typedef typename result_of::coboundary_range<domain_or_container_type, coboundary_type_or_tag>::type range_type;
        
        typedef viennagrid::coboundary_key<domain_or_container_type, coboundary_tag> key_type;
        key_type key(domain);
        
        element_type & element = viennagrid::dereference_handle(domain, handle);
        container_type * container = viennadata::find<key_type, container_type>(key)(element);
//        cout << " coboundary_elements handle=" << handle << " " << container << endl;
        
        if (container)
        {
//             cout << "Using existing co-boundary container" << endl;
            return range_type( *container );
        }
        else
        {
//             cout << "Creating co-boundary container" << endl;
            viennadata::access<key_type, container_type>(key)(element) = create_coboundary_container<coboundary_type_or_tag>(domain, handle);
            return range_type( viennadata::access<key_type, container_type>(key)(element) );
        }
    }
    
    template<typename coboundary_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::coboundary_range<domain_or_container_type, coboundary_type_or_tag>::type create_coboundary_elements(domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type element_type;
        typedef typename result_of::element_tag<coboundary_type_or_tag>::type coboundary_tag;
        typedef typename result_of::coboundary_container<domain_or_container_type, coboundary_type_or_tag>::type container_type;
        typedef typename result_of::coboundary_range<domain_or_container_type, coboundary_type_or_tag>::type range_type;
        
        typedef viennagrid::coboundary_key<domain_or_container_type, coboundary_tag> key_type;
        key_type key(domain);
        
        element_type & element = viennagrid::dereference_handle(domain, handle);
//         container_type * container = viennadata::find<key_type, container_type>(key)(element);
//        cout << " coboundary_elements handle=" << handle << " " << container << endl;
        

        viennadata::access<key_type, container_type>(key)(element) = create_coboundary_container<coboundary_type_or_tag>(domain, handle);
        return range_type( viennadata::access<key_type, container_type>(key)(element) );
    }
    
    
    template<typename coboundary_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::const_coboundary_range<domain_or_container_type, coboundary_type_or_tag>::type coboundary_elements(const domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type element_type;
        typedef typename result_of::element_tag<coboundary_type_or_tag>::type coboundary_tag;
        typedef typename result_of::const_coboundary_container<domain_or_container_type, coboundary_type_or_tag>::type container_type;
        typedef typename result_of::const_coboundary_range<domain_or_container_type, coboundary_type_or_tag>::type range_type;
        
        typedef viennagrid::coboundary_key<domain_or_container_type, coboundary_tag> key_type;
        key_type key(domain);
        
        const element_type & element = viennagrid::dereference_handle(domain, handle);
        container_type * container = viennadata::find<key_type, container_type>(key)(element);
//        cout << " coboundary_elements handle=" << handle << " " << container << endl;
        
        if (container)
        {
//             cout << "Using existing co-boundary container" << endl;
            return range_type( *container );
        }
        else
        {
//             cout << "Creating co-boundary container" << endl;
            viennadata::access<key_type, container_type>(key)(element) = create_coboundary_container<coboundary_type_or_tag>(domain, handle);
            return range_type( viennadata::access<key_type, container_type>(key)(element) );
        }
    }
    
    template<typename coboundary_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::const_coboundary_range<domain_or_container_type, coboundary_type_or_tag>::type create_coboundary_elements(const domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type element_type;
        typedef typename result_of::element_tag<coboundary_type_or_tag>::type coboundary_tag;
        typedef typename result_of::const_coboundary_container<domain_or_container_type, coboundary_type_or_tag>::type container_type;
        typedef typename result_of::const_coboundary_range<domain_or_container_type, coboundary_type_or_tag>::type range_type;
        
        typedef viennagrid::coboundary_key<domain_or_container_type, coboundary_tag> key_type;
        key_type key(domain);
        
        const element_type & element = viennagrid::dereference_handle(domain, handle);
//         container_type * container = viennadata::find<key_type, container_type>(key)(element);

        viennadata::access<key_type, container_type>(key)(element) = create_coboundary_container<coboundary_type_or_tag>(domain, handle);
        return range_type( viennadata::access<key_type, container_type>(key)(element) );
    }
    
    
    
//     template<typename element_type_or_tag, typename coboundary_type_or_tag, typename domain_type>
//     void create_coboundary_information( domain_type & domain )
//     {
//         typedef typename viennagrid::result_of::element_tag<coboundary_type_or_tag>::type coboundary_tag;
//         typedef typename viennagrid::result_of::element< domain_type, coboundary_type_or_tag >::type coboundary_element_type;
//         typedef typename viennagrid::result_of::element_range< domain_type, coboundary_type_or_tag >::type coboundary_range_type;
//         typedef typename viennagrid::result_of::iterator< coboundary_range_type >::type coboundary_range_iterator;
//         
//         typedef typename result_of::coboundary_container<domain_type, coboundary_type_or_tag>::type coboundary_container_type;
//         
//         typedef viennagrid::coboundary_key<domain_type, coboundary_tag> key_type;
//         key_type key(domain);
//         
//         coboundary_range_type coboundary_elements = viennagrid::elements( domain );
//         for (coboundary_range_iterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
//         {
//             typedef typename viennagrid::result_of::element_range< coboundary_element_type, element_type_or_tag >::type element_on_coboundary_range_type;
//             typedef typename viennagrid::result_of::iterator< element_on_coboundary_range_type >::type element_on_coboundary_range_iterator;
//             
//             element_on_coboundary_range_type elements_on_coboundary = viennagrid::elements( *it );
//             for (element_on_coboundary_range_iterator jt = elements_on_coboundary.begin(); jt != elements_on_coboundary.end(); ++jt)
//             {
//                 coboundary_container_type * container = viennadata::find<key_type, coboundary_container_type>(key)( *jt );
//                 if (container)
//                     container->insert_handle( it.handle() );
//                 else    
//                 {
//                     coboundary_container_type coboundary_container;
//                     coboundary_container.set_base_container( viennagrid::storage::collection::get< coboundary_element_type >( viennagrid::container_collection(domain) ) );
//                     viennadata::access<key_type, coboundary_container_type>(key)(*jt) = coboundary_container;
//                 }
//             }
//         }
//     }
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename domain_type>
    void create_coboundary_information( domain_type const & domain )
    {
        typedef typename viennagrid::result_of::element_tag<coboundary_type_or_tag>::type coboundary_tag;
        typedef typename viennagrid::result_of::element< domain_type, coboundary_type_or_tag >::type coboundary_element_type;
        typedef typename viennagrid::result_of::const_element_range< domain_type, coboundary_type_or_tag >::type coboundary_range_type;
        typedef typename viennagrid::result_of::iterator< coboundary_range_type >::type coboundary_range_iterator;
        
        typedef typename result_of::const_coboundary_container<domain_type, coboundary_type_or_tag>::type coboundary_container_type;
        
        typedef viennagrid::coboundary_key<domain_type, coboundary_tag> key_type;
        key_type key(domain);
        
        coboundary_range_type coboundary_elements = viennagrid::elements( domain );
        for (coboundary_range_iterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
        {
            typedef typename viennagrid::result_of::const_element_range< coboundary_element_type, element_type_or_tag >::type element_on_coboundary_range_type;
            typedef typename viennagrid::result_of::iterator< element_on_coboundary_range_type >::type element_on_coboundary_range_iterator;
            
            element_on_coboundary_range_type elements_on_coboundary = viennagrid::elements( *it );
            for (element_on_coboundary_range_iterator jt = elements_on_coboundary.begin(); jt != elements_on_coboundary.end(); ++jt)
            {
                coboundary_container_type * container = viennadata::find<key_type, coboundary_container_type>(key)( *jt );
                if (container)
                    container->insert_handle( it.handle() );
                else    
                {
                    coboundary_container_type coboundary_container;
                    coboundary_container.set_base_container( viennagrid::storage::collection::get< coboundary_element_type >( viennagrid::container_collection(domain) ) );
                    coboundary_container.insert_handle( it.handle() );
                    viennadata::access<key_type, coboundary_container_type>(key)(*jt) = coboundary_container;
                }
            }
        }
    }
    
}

#endif
