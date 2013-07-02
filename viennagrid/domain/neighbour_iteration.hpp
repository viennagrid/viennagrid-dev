#ifndef VIENNAGRID_DOMAIN_NEIGHBOUR_ITERATION_HPP
#define VIENNAGRID_DOMAIN_NEIGHBOUR_ITERATION_HPP

#include "viennagrid/domain/coboundary_iteration.hpp"






namespace viennagrid
{

    
    namespace result_of
    {
        template<typename domain_type, typename element_type_or_tag, typename connector_element_type_or_tag>
        struct neighbour_view
        {
            typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
            typedef typename viennagrid::result_of::element_tag< connector_element_type_or_tag >::type connector_element_tag;
            
            typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    neighbour_collection_tag
                  >::type,
                viennameta::static_pair<element_tag, connector_element_tag>
              >::type::container_type::value_type type;
        };
        
        
        template<typename domain_type, typename element_type_or_tag, typename connector_element_type_or_tag>
        struct neighbour_range
        {
            typedef viennagrid::storage::container_range_wrapper< typename neighbour_view<domain_type, element_type_or_tag, connector_element_type_or_tag>::type > type; 
        };
        
        template<typename domain_type, typename element_type_or_tag, typename connector_element_type_or_tag>
        struct const_neighbour_range
        {
            typedef viennagrid::storage::container_range_wrapper< const typename neighbour_view<domain_type, element_type_or_tag, connector_element_type_or_tag>::type > type; 
        };
        
        template<typename domain_type, typename element_type_or_tag, typename connector_element_type_or_tag>
        struct neighbour_range<const domain_type, element_type_or_tag, connector_element_type_or_tag>
        {
            typedef typename const_neighbour_range<domain_type, element_type_or_tag, connector_element_type_or_tag>::type type;
        };
        
    }
    
    
    
    
    template<typename element_type_or_tag, typename connector_element_type_or_tag, typename domain_type, typename neigbour_accessor_type>
    void create_neighbour_information(domain_type & domain, neigbour_accessor_type accessor)
    {
        std::cout << "Recalculating neighbour" << std::endl;
        
        typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< connector_element_type_or_tag >::type connector_element_tag;
        
        typedef typename viennagrid::result_of::element< domain_type, element_type_or_tag >::type element_type;
        
        typedef typename viennagrid::result_of::element_range< domain_type, element_type_or_tag >::type element_range_type;
        typedef typename viennagrid::result_of::iterator< element_range_type >::type element_range_iterator;
        
        element_range_type elements = viennagrid::elements(domain);        
        accessor.resize( elements.size() );
        
        
        for ( element_range_iterator it = elements.begin(); it != elements.end(); ++it )
        {
            accessor( *it ).clear();
            accessor( *it ).set_base_container( viennagrid::storage::collection::get< element_type >( element_collection(domain) ) );
        }
        
        
        typedef typename viennagrid::result_of::element_range< domain_type, connector_element_tag >::type connector_element_range_type;
        typedef typename viennagrid::result_of::iterator< connector_element_range_type >::type connector_element_range_iterator;
        
        connector_element_range_type connector_elements = viennagrid::elements(domain);
        for ( connector_element_range_iterator it = connector_elements.begin(); it != connector_elements.end(); ++it )
        {
            typedef typename viennagrid::result_of::coboundary_range< domain_type, connector_element_tag, element_tag >::type element_on_connector_element_range_type;
            typedef typename viennagrid::result_of::iterator< element_on_connector_element_range_type >::type element_on_connector_element_range_iterator;
            
            element_on_connector_element_range_type coboundary_range = viennagrid::coboundary_elements<connector_element_tag, element_tag>( domain, it.handle() );
            if (coboundary_range.empty())
                continue;
            
            element_on_connector_element_range_iterator jt1 = coboundary_range.begin(); ++jt1;
            for (; jt1 != coboundary_range.end(); ++jt1)
            {
                for (element_on_connector_element_range_iterator jt0 = coboundary_range.begin(); jt0 != jt1; ++jt0)
                {
                    typedef typename result_of::neighbour_view<domain_type, element_type_or_tag, connector_element_type_or_tag>::type view_type;
                    view_type & view = accessor( *jt0 );
                    
                    typename view_type::iterator kt = view.begin();
                    for (; kt != view.end(); ++kt)
                        if ( kt->id() == jt1->id() )
                            break;
                    
                    if (kt == view.end())
                    {
                        accessor( *jt0 ).insert_handle( jt1.handle() );
                        accessor( *jt1 ).insert_handle( jt0.handle() );
                    }
                }
            }
        }
    }
    
    
    
    template<typename element_type_or_tag, typename connector_element_type_or_tag, typename domain_type>
    void create_neighbour_information(domain_type & domain)
    {
        typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< connector_element_type_or_tag >::type connector_element_tag;
        typedef typename viennagrid::result_of::element< domain_type, element_type_or_tag >::type element_type;
        
        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    neighbour_collection_tag
                >::type,
                viennameta::static_pair<element_tag, connector_element_tag>
                >::type neighbour_container_wrapper_type;
        neighbour_container_wrapper_type & neighbour_container_wrapper = neighbour_collection<element_tag, connector_element_tag>( domain );
        
        viennagrid::accessor::dense_container_accessor_t< typename neighbour_container_wrapper_type::container_type, element_type > accessor( neighbour_container_wrapper.container );
        create_neighbour_information<element_type_or_tag, connector_element_type_or_tag>( domain, accessor );
        
        neighbour_container_wrapper.change_counter = domain.change_counter();
    }
    
    
    
    
    template<typename element_type_or_tag, typename connector_element_type_or_tag, typename neigbour_accessor_type, typename ElementTag, typename WrappedConfigType>
    viennagrid::storage::container_range_wrapper<typename neigbour_accessor_type::value_type>
    neighbour_elements(neigbour_accessor_type accessor, element_t<ElementTag, WrappedConfigType> & element)
    {
        typedef viennagrid::storage::container_range_wrapper<typename neigbour_accessor_type::value_type> range_type;
        return range_type( accessor( element ) );
    }
    
    template<typename element_type_or_tag, typename connector_element_type_or_tag, typename neigbour_accessor_type, typename ElementTag, typename WrappedConfigType>
    viennagrid::storage::container_range_wrapper<const typename neigbour_accessor_type::value_type>
    neighbour_elements(neigbour_accessor_type const accessor, element_t<ElementTag, WrappedConfigType> const & element)
    {
        typedef viennagrid::storage::container_range_wrapper<const typename neigbour_accessor_type::value_type> range_type;
        return range_type( accessor( element ) );
    }
    
    
    template<typename element_type_or_tag, typename connector_element_type_or_tag, typename neigbour_accessor_type, typename WrappedConfigType, typename element_or_handle_type>
    viennagrid::storage::container_range_wrapper<typename neigbour_accessor_type::value_type>
    neighbour_elements(domain_t<WrappedConfigType> & domain, neigbour_accessor_type accessor, element_or_handle_type & hendl)
    {
        return neighbour_elements<element_type_or_tag, connector_element_type_or_tag>( accessor, viennagrid::dereference_handle(domain, hendl) );
    }
    
    template<typename element_type_or_tag, typename connector_element_type_or_tag, typename neigbour_accessor_type, typename WrappedConfigType, typename element_or_handle_type>
    viennagrid::storage::container_range_wrapper<const typename neigbour_accessor_type::value_type>
    neighbour_elements(domain_t<WrappedConfigType> const & domain, neigbour_accessor_type const accessor, element_or_handle_type const & hendl)
    {
        return neighbour_elements<element_type_or_tag, connector_element_type_or_tag>( accessor, viennagrid::dereference_handle(domain, hendl) );
    }
    
    
    
    
    template<typename element_type_or_tag, typename connector_element_type_or_tag, typename WrappedConfigType, typename element_or_handle_type>
    typename result_of::neighbour_range<domain_t<WrappedConfigType>, element_type_or_tag, connector_element_type_or_tag>::type
    neighbour_elements(domain_t<WrappedConfigType> & domain, element_or_handle_type const & hendl)
    {
        typedef domain_t<WrappedConfigType> domain_type;
        typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< connector_element_type_or_tag >::type connector_element_tag;
        typedef typename viennagrid::result_of::element< domain_type, element_type_or_tag >::type element_type;
                
        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    neighbour_collection_tag
                >::type,
                viennameta::static_pair<element_tag, connector_element_tag>
                >::type neighbour_container_wrapper_type;
        neighbour_container_wrapper_type & neighbour_container_wrapper = neighbour_collection<element_tag, connector_element_tag>( domain );
        
        if ( neighbour_container_wrapper.change_counter != domain.change_counter() )
            create_neighbour_information<element_type_or_tag, connector_element_type_or_tag>(domain);
        
        return neighbour_elements<element_type_or_tag, connector_element_type_or_tag>( accessor::dense_container_accessor<element_type>(neighbour_container_wrapper.container), viennagrid::dereference_handle(domain, hendl) );
    }
    
    
    template<typename element_type_or_tag, typename connector_element_type_or_tag, typename WrappedConfigType, typename element_or_handle_type>
    typename result_of::const_neighbour_range<domain_t<WrappedConfigType>, element_type_or_tag, connector_element_type_or_tag>::type
    neighbour_elements(domain_t<WrappedConfigType> const & domain, element_or_handle_type const & hendl)
    {
        typedef domain_t<WrappedConfigType> domain_type;
        typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< connector_element_type_or_tag >::type connector_element_tag;
        typedef typename viennagrid::result_of::element< domain_type, element_type_or_tag >::type element_type;
                
        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    neighbour_collection_tag
                >::type,
                viennameta::static_pair<element_tag, connector_element_tag>
                >::type neighbour_container_wrapper_type;
        neighbour_container_wrapper_type const & neighbour_container_wrapper = neighbour_collection<element_tag, connector_element_tag>( domain );
        
        if ( neighbour_container_wrapper.change_counter != domain.change_counter() )
            create_neighbour_information<element_type_or_tag, connector_element_type_or_tag>( const_cast<domain_type&>(domain) );

        
        return neighbour_elements<element_type_or_tag, connector_element_type_or_tag>( accessor::dense_container_accessor<element_type>(neighbour_container_wrapper.container), viennagrid::dereference_handle(domain, hendl) );
    }
    

}


#endif
