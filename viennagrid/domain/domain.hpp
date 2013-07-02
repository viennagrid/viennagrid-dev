#ifndef VIENNAGRID_DOMAIN_HPP
#define VIENNAGRID_DOMAIN_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/config/topology_config.hpp"
#include "viennagrid/domain/topology.hpp"
// #include "viennagrid/domain/metainfo.hpp"
#include "viennagrid/element/element_view.hpp"

namespace viennagrid
{
    
    namespace result_of
    {
        template<typename point_container_type>
        struct point_type
        {
//             typedef typename viennagrid::metainfo::result_of::value_type<point_container_type>::type type;
        };

        template<typename ConfigType>
        struct point_type< domain_t<ConfigType> >
        {
            typedef typename viennagrid::result_of::vertex< domain_t<ConfigType> >::type::appendix_type type;
        };
        
        template<typename ConfigType>
        struct point_type< const domain_t<ConfigType> >
        {
            typedef typename viennagrid::result_of::vertex< domain_t<ConfigType> >::type::appendix_type type;
        };
        
        template<typename EA, typename EB, typename EC, typename ED>
        struct point_type< element_t<EA, EB, EC, ED> >
        {
            typedef typename viennagrid::result_of::vertex< element_t<EA, EB, EC, ED> >::type::appendix_type type;
        };
        
        template<typename EA, typename EB, typename EC, typename ED>
        struct point_type< const element_t<EA, EB, EC, ED> >
        {
            typedef typename viennagrid::result_of::vertex< element_t<EA, EB, EC, ED> >::type::appendix_type type;
        };
    }
    
    

    
    template<typename ConfigType>
    typename result_of::point_type< domain_t<ConfigType> >::type & point(
            domain_t<ConfigType> & domain,
            typename result_of::vertex< domain_t<ConfigType> >::type & vertex
        )
    {
        return vertex.appendix();
    }
    
    template<typename ConfigType>
    typename result_of::point_type< domain_t<ConfigType> >::type const & point(
            domain_t<ConfigType> const & domain,
            typename result_of::vertex< domain_t<ConfigType> >::type const & vertex
        )
    {
        return vertex.appendix();
    }
    
    
    template<typename ConfigType>
    typename result_of::point_type< domain_t<ConfigType> >::type & point(
            domain_t<ConfigType> & domain,
            typename result_of::vertex_handle< domain_t<ConfigType> >::type vertex_handle
        )
    {
        return dereference_handle(domain, vertex_handle).appendix();
    }
    
    template<typename ConfigType>
    typename result_of::point_type< domain_t<ConfigType> >::type const & point(
            domain_t<ConfigType> const & domain,
            typename result_of::const_vertex_handle< domain_t<ConfigType> >::type vertex_handle
        )
    {
        return dereference_handle(domain, vertex_handle).appendix();
    }


    
    
//     template<typename config_type, typename topology_type, typename metainfo_collection_type, typename element_tag, typename boundary_cell_container_typelist, typename id_type, typename appendix_type>
//     std::pair<
//                 typename viennagrid::storage::result_of::container_of<
//                     typename result_of::container_collection< domain_t<config_type, topology_type, metainfo_collection_type> >::type,
//                     viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type, appendix_type>
//                 >::type::handle_type,
//                 bool
//             >
//         push_element( domain_t<config_type, topology_type, metainfo_collection_type> & domain, const viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type, appendix_type> & element)
//     {
//         metainfo::increment_size< viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type, appendix_type> >(metainfo_collection(domain));
//         return inserter(domain)(element);
//     }
//     
//     template<typename config_type, typename topology_type, typename metainfo_collection_type, typename element_type>
//     std::pair<
//                 typename viennagrid::storage::result_of::container_of<
//                     typename result_of::container_collection< domain_t<config_type, topology_type, metainfo_collection_type> >::type,
//                     element_type
//                 >::type::handle_type,
//                 bool
//             >
//         push_element( domain_t<config_type, topology_type, metainfo_collection_type> & domain, const element_type & element)
//     {
//         metainfo::increment_size< element_type >(metainfo_collection(domain));
//         return inserter(domain)(element);
//     }   
//     
//     template<typename config_type, typename topology_type, typename metainfo_collection_type, typename element_type>
//     std::pair<
//                 typename viennagrid::storage::result_of::container_of<
//                     typename result_of::container_collection< domain_t<config_type, topology_type, metainfo_collection_type> >::type,
//                     element_type
//                 >::type::handle_type,
//                 bool
//             >
//         push_element_noid( domain_t<config_type, topology_type, metainfo_collection_type> & domain, const element_type & element)
//     {
//         metainfo::increment_size< element_type >(metainfo_collection(domain));
//         return inserter(domain).template insert<false, true>(element);
//     }
    
    
    
    
    
//     namespace result_of
//     {
//         template<typename domain_config>
//         struct domain
//         {
//             typedef typename viennagrid::result_of::topology<
//                     domain_config
//                 >::type topology_type;
//                 
//              typedef typename viennagrid::config::result_of::query_config<domain_config, viennagrid::config::vector_type_tag>::type vector_type;
//                 
//             typedef typename viennagrid::storage::collection_t<
//                     typename viennagrid::result_of::metainfo_container_typemap<
//                         typename viennagrid::config::result_of::query_config< domain_config, viennagrid::config::metainfo_typelist_tag >::type,
//                         typename viennagrid::config::result_of::query_config< domain_config, viennagrid::config::metainfo_container_config_tag >::type,
//                         domain_config
//                     >::type
//                 > metainfo_collection_type;  
//             
//             typedef domain_t<domain_config, topology_type, metainfo_collection_type> type;
//         };
//         
//         
//         template<
//             typename domain_type,
//             typename element_typelist = 
//                 typename storage::container_collection::result_of::value_typelist<
//                     typename container_collection<domain_type>::type
//                 >::type,
//             typename container_config = 
//                 storage::default_container_config>
//         struct domain_view
//         {
//             typedef typename viennagrid::result_of::topologic_view<typename domain_type::topology_type, element_typelist>::type topologic_view_type;
//             typedef typename domain_type::metainfo_collection_type metainfo_collection_type;
//             typedef typename viennagrid::domain_t<typename domain_type::config_type, topologic_view_type, metainfo_collection_type*> type;
//         };
//         
//         template<
//             typename config_type, typename topology_type, typename metainfo_collection_type,
//             typename element_typelist,
//             typename container_config>
//         struct domain_view< domain_t<config_type, topology_type, metainfo_collection_type*>, element_typelist, container_config >
//         {
//             typedef domain_t<config_type, topology_type, metainfo_collection_type*> domain_type;
//             typedef typename viennagrid::result_of::topologic_view<typename domain_type::topology_type, element_typelist>::type topologic_view_type;
//             typedef typename viennagrid::domain_t<typename domain_type::config_type, topologic_view_type, metainfo_collection_type*> type;
//         };
//     }
    
    

//     template<typename config_type, typename topology_type, typename metainfo_collection_type>
//     struct create_view_helper< domain_t<config_type, topology_type, metainfo_collection_type> >
//     {
//         typedef domain_t<config_type, topology_type, metainfo_collection_type> geomatric_view_type;
//         
//         template<typename domain_type>
//         static geomatric_view_type create( domain_type & domain )
//         {
//             typedef typename geomatric_view_type::topology_type topologic_view_type;
//             return geomatric_view_type(
//                 viennagrid::create_view<topologic_view_type>( viennagrid::topology(domain) ),
//                 domain.get_metainfo_collection()
//             );
//         }
//     };
    
    
    
    
    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::iterator< typename viennagrid::result_of::element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find_by_id(domain_type & domain, id_type id)
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::iterator<RangeType>::type RangeIterator;
        
        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.begin(); it != range.end(); ++it)
        {
            if ( viennagrid::dereference_handle(domain, it.handle()).id() == id )
                return it;
        }
        
        return range.end();
    }
    
    template<typename domain_type, typename id_type>
    typename viennagrid::result_of::const_iterator< typename viennagrid::result_of::const_element_range<domain_type, typename id_type::value_type::tag>::type >::type
            find_by_id(const domain_type & domain, id_type id)
    {
        typedef typename id_type::value_type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::const_element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::const_iterator<RangeType>::type RangeIterator;
        
        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.begin(); it != range.end(); ++it)
        {
            if ( viennagrid::dereference_handle(domain, it.handle()).id() == id )
                return it;
        }
        
        return range.end();
    }

    template<typename domain_type, typename handle_type>
    typename viennagrid::result_of::iterator< typename viennagrid::result_of::element_range<domain_type, typename storage::handle::result_of::value_type<handle_type>::type >::type >::type
            find_by_handle(domain_type & domain, handle_type handle)
    {
        typedef typename storage::handle::result_of::value_type<handle_type>::type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::iterator<RangeType>::type RangeIterator;
        
        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.begin(); it != range.end(); ++it)
        {
            if ( it.handle() == handle )
                return it;
        }
        
        return range.end();
    }
    
    template<typename domain_type, typename handle_type>
    typename viennagrid::result_of::const_iterator< typename viennagrid::result_of::const_element_range<domain_type, typename storage::handle::result_of::value_type<handle_type>::type >::type  >::type
            find_by_handle(const domain_type & domain, handle_type handle)
    {
        typedef typename storage::handle::result_of::value_type<handle_type>::type element_type;
        typedef typename element_type::tag element_tag;
        typedef typename viennagrid::result_of::const_element_range<domain_type, element_tag>::type RangeType;
        typedef typename viennagrid::result_of::const_iterator<RangeType>::type RangeIterator;
        
        RangeType range = viennagrid::elements<element_tag>(domain);
        for (RangeIterator it = range.begin(); it != range.end(); ++it)
        {
            if ( it.handle() == handle )
                return it;
        }
        
        return range.end();
    }
    
}

#endif
