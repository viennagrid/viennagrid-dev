#ifndef VIENNAGRID_DOMAIN_NEIGHBOUR_ITERATION_HPP
#define VIENNAGRID_DOMAIN_NEIGHBOUR_ITERATION_HPP

#include "viennagrid/domain/coboundary_iteration.hpp"

namespace viennagrid
{
    
    
    namespace result_of
    {
        
        template<typename domain_or_container_type, typename element_type_or_tag, typename view_container_tag = viennagrid::storage::std_deque_tag>
        struct neighbour_container
        {
            typedef typename coboundary_container<domain_or_container_type, element_type_or_tag, view_container_tag>::type type;
        };
        
        template<typename domain_or_container_type, typename element_type_or_tag, typename view_container_tag = viennagrid::storage::std_deque_tag>
        struct const_neighbour_container
        {
            typedef typename const_coboundary_container<domain_or_container_type, element_type_or_tag, view_container_tag>::type type;
        };
        
        
        template<typename domain_or_container_type, typename element_type_or_tag, typename view_container_tag = viennagrid::storage::std_deque_tag>
        struct neighbour_range
        {
            typedef typename coboundary_range<domain_or_container_type, element_type_or_tag, view_container_tag>::type type;
        };
        
        template<typename domain_or_container_type, typename element_type_or_tag, typename view_container_tag = viennagrid::storage::std_deque_tag>
        struct const_neighbour_range
        {
            typedef typename const_coboundary_range<domain_or_container_type, element_type_or_tag, view_container_tag>::type type;
        };
        
    }
    
    
    template<typename connector_boundary_element_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::neighbour_container<domain_or_container_type, typename viennagrid::storage::handle::value_type< handle_type >::type>::type
        create_neighbour_container(domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::value_type< handle_type >::type element_type;
        typedef typename viennagrid::result_of::element_tag< element_type >::type element_tag;
                
        typedef typename result_of::neighbour_container<domain_or_container_type, element_tag>::type neighbour_container_type;
        
        neighbour_container_type neighbour_container;
        neighbour_container.set_base_container( viennagrid::storage::collection::get< element_type >( viennagrid::container_collection(domain) ) );
        
        
        typedef typename result_of::element_tag<connector_boundary_element_type_or_tag>::type connector_boundary_element_tag;
        typedef typename result_of::element<domain_or_container_type, connector_boundary_element_tag>::type connector_boundary_element_type;
        
        typedef typename viennagrid::result_of::element_range< element_type, connector_boundary_element_tag >::type facet_on_element_range_type;
        typedef typename viennagrid::result_of::handle_iterator<facet_on_element_range_type>::type facet_on_handle_iterator;
        
        element_type & element = viennagrid::dereference_handle( domain, handle );
        
        facet_on_element_range_type facet_on_element_range = viennagrid::elements<connector_boundary_element_tag>( element );
        for (facet_on_handle_iterator feit = facet_on_element_range.handle_begin(); feit != facet_on_element_range.handle_end(); ++feit)
        {
            typedef typename viennagrid::result_of::coboundary_range<domain_or_container_type, element_type>::type element_on_facet_coboundary_range_type;
            typedef typename viennagrid::result_of::handle_iterator<element_on_facet_coboundary_range_type>::type element_on_facet_coboundary_iterator;
            
            element_on_facet_coboundary_range_type element_on_facet_coboundary_range = viennagrid::coboundary_elements<element_tag>(domain, *feit);
            for (element_on_facet_coboundary_iterator efcit = element_on_facet_coboundary_range.handle_begin(); efcit != element_on_facet_coboundary_range.handle_end(); ++efcit)
            {
                if (*efcit != handle)
                {
                    typename neighbour_container_type::handle_iterator ncit = neighbour_container.begin();
                    for (; ncit != neighbour_container.handle_end(); ++ncit)
                    {
                        if ( *ncit == *efcit )
                            break;
                    }
                    if (ncit == neighbour_container.handle_end())
                        neighbour_container.insert_handle( *efcit );
                }
                    
            }
        }

        return neighbour_container;
    }
    
    
    template<typename connector_boundary_element_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::const_neighbour_container<domain_or_container_type, typename viennagrid::storage::handle::value_type< handle_type >::type>::type
        create_neighbour_container(const domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::value_type< handle_type >::type element_type;
        typedef typename viennagrid::result_of::element_tag< element_type >::type element_tag;
                
        typedef typename result_of::const_neighbour_container<domain_or_container_type, element_tag>::type neighbour_container_type;
        
        neighbour_container_type neighbour_container;
        neighbour_container.set_base_container( viennagrid::storage::collection::get< element_type >( viennagrid::container_collection(domain) ) );
        
        
        typedef typename result_of::element_tag<connector_boundary_element_type_or_tag>::type connector_boundary_element_tag;
        typedef typename result_of::element<domain_or_container_type, connector_boundary_element_tag>::type connector_boundary_element_type;
        
        typedef typename viennagrid::result_of::const_element_range< element_type, connector_boundary_element_tag >::type facet_on_element_range_type;
        typedef typename viennagrid::result_of::const_handle_iterator<facet_on_element_range_type>::type facet_on_handle_iterator;
        
        const element_type & element = viennagrid::dereference_handle( domain, handle );
        
        facet_on_element_range_type facet_on_element_range = viennagrid::elements<connector_boundary_element_tag>( element );
        for (facet_on_handle_iterator feit = facet_on_element_range.handle_begin(); feit != facet_on_element_range.handle_end(); ++feit)
        {
            typedef typename viennagrid::result_of::const_coboundary_range<domain_or_container_type, element_type>::type element_on_facet_coboundary_range_type;
            typedef typename viennagrid::result_of::const_handle_iterator<element_on_facet_coboundary_range_type>::type element_on_facet_coboundary_iterator;
            
            element_on_facet_coboundary_range_type element_on_facet_coboundary_range = viennagrid::coboundary_elements<element_tag>(domain, *feit);
            for (element_on_facet_coboundary_iterator efcit = element_on_facet_coboundary_range.handle_begin(); efcit != element_on_facet_coboundary_range.handle_end(); ++efcit)
            {
                if (*efcit != handle)
                {
                    typename neighbour_container_type::const_handle_iterator ncit = neighbour_container.begin();
                    for (; ncit != neighbour_container.handle_end(); ++ncit)
                    {
                        if ( *ncit == *efcit )
                            break;
                    }
                    if (ncit == neighbour_container.handle_end())
                        neighbour_container.insert_handle( *efcit );
                }
                    
            }
        }

        return neighbour_container;
    }
    
    
    
    
    
    
    template<typename connector_boundary_element_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::neighbour_range<domain_or_container_type, typename viennagrid::storage::handle::value_type< handle_type >::type >::type
        neighbour_elements(domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::value_type<handle_type>::type element_type;
        typedef typename result_of::element_tag<connector_boundary_element_type_or_tag>::type connector_boundary_element_tag;
        //typedef typename result_of::element_tag<coboundary_type_or_tag>::type coboundary_tag;
        typedef typename result_of::neighbour_container<domain_or_container_type, element_type>::type container_type;
        typedef typename result_of::neighbour_range<domain_or_container_type, element_type>::type range_type;
        
        typedef viennagrid::neighbour_key<domain_or_container_type, connector_boundary_element_tag> key_type;
        key_type key(domain);
        
        element_type & element = viennagrid::dereference_handle(domain, handle);
        container_type * container = viennadata::find<key_type, container_type>(key)(element);
//        cout << " coboundary_elements handle=" << handle << " " << container << endl;
        
        if (container)
        {
//             cout << "Using existing neighbour container" << endl;
            return range_type( *container );
        }
        else
        {
//             cout << "Creating neighbour container" << endl;
            viennadata::access<key_type, container_type>(key)(element) = create_neighbour_container<connector_boundary_element_tag>(domain, handle);
            return range_type( viennadata::access<key_type, container_type>(key)(element) );
        }
    }
    
    
    template<typename connector_boundary_element_type_or_tag, typename domain_or_container_type, typename handle_type>
    typename result_of::const_neighbour_range<domain_or_container_type, typename viennagrid::storage::handle::value_type< handle_type >::type >::type
        neighbour_elements(const domain_or_container_type & domain, handle_type handle)
    {
        typedef typename viennagrid::storage::handle::value_type<handle_type>::type element_type;
        typedef typename result_of::element_tag<connector_boundary_element_type_or_tag>::type connector_boundary_element_tag;
        //typedef typename result_of::element_tag<coboundary_type_or_tag>::type coboundary_tag;
        typedef typename result_of::const_coboundary_container<domain_or_container_type, element_type>::type container_type;
        typedef typename result_of::const_coboundary_range<domain_or_container_type, element_type>::type range_type;
        
        typedef viennagrid::coboundary_key<domain_or_container_type, connector_boundary_element_tag> key_type;
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
            viennadata::access<key_type, container_type>(key)(element) = create_neighbour_container<connector_boundary_element_tag>(domain, handle);
            return range_type( viennadata::access<key_type, container_type>(key)(element) );
        }
    }
}

#endif
