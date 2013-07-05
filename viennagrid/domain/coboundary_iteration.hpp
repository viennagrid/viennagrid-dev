#ifndef VIENNAGRID_DOMAIN_COBOUNDARY_ITERATION_HPP
#define VIENNAGRID_DOMAIN_COBOUNDARY_ITERATION_HPP

#include "viennagrid/forwards.hpp"
#include "viennagrid/storage/forwards.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/accessor.hpp"
 
namespace viennagrid
{

    namespace result_of
    {
        template<typename domain_type, typename element_type_or_tag, typename coboundary_type_or_tag>
        struct coboundary_view
        {
            typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
            typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
            
            typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    coboundary_collection_tag
                  >::type,
                viennagrid::meta::static_pair<element_tag, coboundary_tag>
              >::type::container_type::value_type type;
        };
        
        
        template<typename domain_type, typename element_type_or_tag, typename coboundary_type_or_tag>
        struct coboundary_range
        {
            typedef viennagrid::storage::container_range_wrapper< typename coboundary_view<domain_type, element_type_or_tag, coboundary_type_or_tag>::type > type; 
        };
        
        template<typename domain_type, typename element_type_or_tag, typename coboundary_type_or_tag>
        struct const_coboundary_range
        {
            typedef viennagrid::storage::container_range_wrapper< const typename coboundary_view<domain_type, element_type_or_tag, coboundary_type_or_tag>::type > type; 
        };
        
        template<typename domain_type, typename element_type_or_tag, typename coboundary_type_or_tag>
        struct coboundary_range<const domain_type, element_type_or_tag, coboundary_type_or_tag>
        {
            typedef typename const_coboundary_range<domain_type, element_type_or_tag, coboundary_type_or_tag>::type type;
        };
        
    }
    
    
    
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename domain_type, typename coboundary_accessor_type>
    void create_coboundary_information(domain_type & domain, coboundary_accessor_type accessor)
    {
        typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
        
        typedef typename viennagrid::result_of::element< domain_type, coboundary_type_or_tag >::type coboundary_type;
        
        typedef typename viennagrid::result_of::element_range< domain_type, element_type_or_tag >::type element_range_type;
        typedef typename viennagrid::result_of::iterator< element_range_type >::type element_range_iterator;
        
        element_range_type elements = viennagrid::elements(domain);
        
        accessor.resize( elements.size() );
        
        
        for ( element_range_iterator it = elements.begin(); it != elements.end(); ++it )
        {
            accessor.access( *it ).clear();
            accessor.access( *it ).set_base_container( viennagrid::storage::collection::get< coboundary_type >( element_collection(domain) ) );
        }

        
        typedef typename viennagrid::result_of::element_range< domain_type, coboundary_type_or_tag >::type coboundary_element_range_type;
        typedef typename viennagrid::result_of::iterator< coboundary_element_range_type >::type coboundary_element_range_iterator;
        
        coboundary_element_range_type coboundary_elements = viennagrid::elements(domain);
        for (coboundary_element_range_iterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
        {
            typedef typename viennagrid::result_of::element_range< coboundary_type, element_tag >::type element_on_coboundary_element_range_type;
            typedef typename viennagrid::result_of::iterator< element_on_coboundary_element_range_type >::type element_on_coboundary_element_range_iterator;
            
            element_on_coboundary_element_range_type elements_on_coboundary_element = viennagrid::elements( *it );
            for (element_on_coboundary_element_range_iterator jt = elements_on_coboundary_element.begin(); jt != elements_on_coboundary_element.end(); ++jt)
                accessor.access( *jt ).insert_handle( it.handle() );
        }
    }
    
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename domain_type>
    void create_coboundary_information(domain_type & domain)
    {
        typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
        typedef typename viennagrid::result_of::element< domain_type, element_type_or_tag >::type element_type;
        
        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    coboundary_collection_tag
                >::type,
                viennagrid::meta::static_pair<element_tag, coboundary_tag>
                >::type coboundary_container_wrapper_type;
        
        coboundary_container_wrapper_type & coboundary_container_wrapper = coboundary_collection<element_tag, coboundary_tag>( domain );//viennagrid::storage::collection::get< viennagrid::meta::static_pair<element_tag, coboundary_tag> > ( domain.coboundary_collection() );
        
        viennagrid::accessor::dense_container_accessor_t< typename coboundary_container_wrapper_type::container_type, element_type > accessor( coboundary_container_wrapper.container );
        create_coboundary_information<element_type_or_tag, coboundary_type_or_tag>( domain, accessor );
        
        update_change_counter( domain, coboundary_container_wrapper.change_counter );
    }
    
    
    
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename coboundary_accessor_type, typename ElementTag, typename WrappedConfigType>
    viennagrid::storage::container_range_wrapper<typename coboundary_accessor_type::value_type> 
    coboundary_elements(coboundary_accessor_type accessor, element_t<ElementTag, WrappedConfigType> & element)
    {
        typedef viennagrid::storage::container_range_wrapper<typename coboundary_accessor_type::value_type> range_type;
        return range_type( accessor( element ) );
    }
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename coboundary_accessor_type, typename ElementTag, typename WrappedConfigType>
    viennagrid::storage::container_range_wrapper<const typename coboundary_accessor_type::value_type> 
    coboundary_elements(coboundary_accessor_type const accessor, element_t<ElementTag, WrappedConfigType> const & element)
    {
        typedef viennagrid::storage::container_range_wrapper<const typename coboundary_accessor_type::value_type> range_type;
        return range_type( accessor( element ) );
    }
    
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename coboundary_accessor_type, typename WrappedConfigType, typename element_or_handle_type>
    viennagrid::storage::container_range_wrapper<typename coboundary_accessor_type::value_type> 
    coboundary_elements(domain_t<WrappedConfigType> & domain, coboundary_accessor_type accessor, element_or_handle_type & hendl)
    {
        return coboundary_elements<element_type_or_tag, coboundary_type_or_tag>( accessor, viennagrid::dereference_handle(domain, hendl) );
    }
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename coboundary_accessor_type, typename WrappedConfigType, typename element_or_handle_type>
    viennagrid::storage::container_range_wrapper<const typename coboundary_accessor_type::value_type> 
    coboundary_elements(domain_t<WrappedConfigType> const & domain, coboundary_accessor_type const accessor, element_or_handle_type const & hendl)
    {
        return coboundary_elements<element_type_or_tag, coboundary_type_or_tag>( accessor, viennagrid::dereference_handle(domain, hendl) );
    }
    

    
    
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename WrappedConfigType, typename element_or_handle_type>
    typename result_of::coboundary_range<domain_t<WrappedConfigType>, element_type_or_tag, coboundary_type_or_tag>::type 
    coboundary_elements(domain_t<WrappedConfigType> & domain, element_or_handle_type const & hendl)
    {
        typedef domain_t<WrappedConfigType> domain_type;
        typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
        typedef typename viennagrid::result_of::element< domain_type, element_type_or_tag >::type element_type;
        
        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    coboundary_collection_tag
                >::type,
                viennagrid::meta::static_pair<element_tag, coboundary_tag>
                >::type coboundary_container_wrapper_type;
        coboundary_container_wrapper_type & coboundary_container_wrapper = coboundary_collection<element_tag, coboundary_tag>( domain );
        
        if ( is_obsolete(domain, coboundary_container_wrapper.change_counter) )
            create_coboundary_information<element_type_or_tag, coboundary_type_or_tag>(domain);

        return coboundary_elements<element_type_or_tag, coboundary_type_or_tag>( accessor::dense_container_accessor<element_type>(coboundary_container_wrapper.container), viennagrid::dereference_handle(domain, hendl) );
        
    }
    
    
    template<typename element_type_or_tag, typename coboundary_type_or_tag, typename WrappedConfigType, typename element_or_handle_type>
    typename result_of::const_coboundary_range<domain_t<WrappedConfigType>, element_type_or_tag, coboundary_type_or_tag>::type
    coboundary_elements(domain_t<WrappedConfigType> const & domain, element_or_handle_type const & hendl)
    {
        typedef domain_t<WrappedConfigType> domain_type;
        typedef typename viennagrid::result_of::element_tag< element_type_or_tag >::type element_tag;
        typedef typename viennagrid::result_of::element_tag< coboundary_type_or_tag >::type coboundary_tag;
        typedef typename viennagrid::result_of::element< domain_type, element_type_or_tag >::type element_type;
                
        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    coboundary_collection_tag
                >::type,
                viennagrid::meta::static_pair<element_tag, coboundary_tag>
                >::type coboundary_container_wrapper_type;
        coboundary_container_wrapper_type const & coboundary_container_wrapper = coboundary_collection<element_tag, coboundary_tag>( domain );
        
        if ( is_obsolete(domain, coboundary_container_wrapper.change_counter) )
            create_coboundary_information<element_type_or_tag, coboundary_type_or_tag>( const_cast<domain_type&>(domain) );

        return coboundary_elements<element_type_or_tag, coboundary_type_or_tag>( accessor::dense_container_accessor<element_type>(coboundary_container_wrapper.container), viennagrid::dereference_handle(domain, hendl) );
    }

}

#endif
