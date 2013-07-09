#ifndef VIENNAGRID_ELEMENT_CONFIG_HPP
#define VIENNAGRID_ELEMENT_CONFIG_HPP

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


#include "viennagrid/storage/view.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/config/config.hpp"
#include "viennagrid/topology/polygon.hpp"


/** @file element.hpp
    @brief Provides the main n-cell type
*/

namespace viennagrid
{
    
    namespace config
    {
        namespace result_of
        {       
            template<typename storage_layout_tag, long num>
            struct boundary_cell_container_tag
            {};
            
            template<long num>
            struct boundary_cell_container_tag<viennagrid::static_layout_tag, num>
            {
                typedef viennagrid::storage::static_array_tag<num> type;
            };
            
            template<long num>
            struct boundary_cell_container_tag<viennagrid::dynamic_layout_tag, num>
            {
                typedef viennagrid::storage::std_vector_tag type;
            };
            
            template<long num>
            struct boundary_cell_container_tag<viennagrid::dynamic_unique_layout_tag, num>
            {
                typedef viennagrid::storage::std_set_tag type;
            };
            
            
            
            //
            // Generates an element type
            //

            template<typename WrappedConfigType, typename element_tag>
            struct is_element_present
            {
                typedef typename query_config<typename WrappedConfigType::type, topology_config_tag>::type  TopologyConfig;
                
                static const bool value =
                    !viennagrid::meta::EQUAL<
                        typename viennagrid::meta::typemap::result_of::find<
                            TopologyConfig,
                            element_tag
                        >::type,
                        viennagrid::meta::not_found
                    >::value;
            };
            
            
            template<typename WrappedConfigType, typename element_tag>
            struct query_element_id_tag
            {
                typedef typename query_config<typename WrappedConfigType::type, topology_config_tag>::type  TopologyConfig;
                typedef typename query_config<TopologyConfig, element_tag>::type  ElementConfig;
                typedef typename query_config<ElementConfig, element_id_tag>::type  type;
            };
            
            template<typename WrappedConfigType, typename element_tag>
            struct query_element_container_tag
            {
                typedef typename query_config<typename WrappedConfigType::type, topology_config_tag>::type  TopologyConfig;
                typedef typename query_config<TopologyConfig, element_tag>::type  ElementConfig;
                typedef typename query_config<ElementConfig, element_container_tag>::type  type;
            };
            

            template<typename WrappedConfigType, typename element_tag, typename boundary_cell_tag>
            struct query_boundary_storage_layout
            {
                typedef typename query_config<typename WrappedConfigType::type, topology_config_tag>::type  TopologyConfig;
                typedef typename query_config<TopologyConfig, element_tag>::type  ElementConfig;
                typedef typename query_config<ElementConfig, element_boundary_storage_layout_tag>::type  BoundaryStorageLayoutConfig;
                typedef typename query_config<BoundaryStorageLayoutConfig, boundary_cell_tag>::type  type;
            };
            
            
            template<typename WrappedConfigType, typename element_tag>
            struct query_appendix_type
            {
                typedef typename query_config<typename WrappedConfigType::type, topology_config_tag>::type  TopologyConfig;
                typedef typename query_config<TopologyConfig, element_tag>::type  ElementConfig;
                typedef typename query_config<ElementConfig, element_appendix_type_tag>::type  type;
            };
            
            
            
            
            
            template<typename domain_config_, typename element_tag_, typename boundary_cell_tag_, bool is_present = is_element_present<domain_config_, boundary_cell_tag_>::value >
            struct has_boundary_cells_helper
            {
                static const bool value = 
                    !viennagrid::meta::EQUAL<
                        typename query_boundary_storage_layout<domain_config_, element_tag_, boundary_cell_tag_>::type,
                        viennagrid::no_handling_tag
                    >::value;
            };
            
            template<typename domain_config_, typename element_tag_, typename boundary_cell_tag_>
            struct has_boundary_cells_helper<domain_config_, element_tag_, boundary_cell_tag_, false>
            {
                static const bool value = false;
            };
            
            
            template<typename domain_config_, typename element_tag_, typename boundary_cell_tag_>
            struct has_boundary_cells
            {
                static const bool is_present = is_element_present<domain_config_, boundary_cell_tag_>::value;
                            
                static const bool value = has_boundary_cells_helper<domain_config_, element_tag_, boundary_cell_tag_, is_present>::value;
            };
            
            
            template<typename domain_config_, typename element_tag_, typename boundary_cell_tag_, bool is_present = is_element_present<domain_config_, boundary_cell_tag_>::value >
            struct has_orientation_helper
            {
                static const bool value = 
                    viennagrid::meta::EQUAL<
                        typename query_boundary_storage_layout<domain_config_, element_tag_, boundary_cell_tag_>::type,
                        viennagrid::full_handling_tag
                    >::value
                        ||
                    viennagrid::meta::EQUAL<
                        typename query_boundary_storage_layout<domain_config_, element_tag_, boundary_cell_tag_>::type,
                        viennagrid::full_lazy_handling_tag
                    >::value;
            };
            
            template<typename domain_config_, typename element_tag_, typename boundary_cell_tag_>
            struct has_orientation_helper<domain_config_, element_tag_, boundary_cell_tag_, false>
            {
                static const bool value = false;
            };
            
            
            template<typename domain_config_, typename element_tag_, typename boundary_cell_tag_>
            struct has_orientation
            {
                static const bool is_present = is_element_present<domain_config_, boundary_cell_tag_>::value;
                            
                static const bool value = has_orientation_helper<domain_config_, element_tag_, boundary_cell_tag_, is_present>::value;
            };
            
            
            
            
            
            // TODO: handle possible other tags (lazy_handling_tag, ...)
            template<typename domain_config, typename element_tag, typename boundary_cell_tag, bool is_present>
            struct element_boundary_cell_container_helper
            {
                
                //
                // boundary cell view
                //
                typedef element_t<boundary_cell_tag, domain_config> boundary_cell_type;
                
                typedef typename viennagrid::storage::result_of::container<
                    boundary_cell_type,                                         // the 'value_type', i.e. vertices     
                    typename query_element_container_tag<domain_config, boundary_cell_tag>::type
                >::type boundary_cell_container;
                
                
                typedef typename boundary_cell_container_tag<
                        typename viennagrid::boundary_elements< element_tag, boundary_cell_tag>::layout_tag,
                        viennagrid::boundary_elements< element_tag, boundary_cell_tag>::num
                    >::type container_tag;
                

                typedef typename viennagrid::storage::result_of::view<
                        boundary_cell_container,
                        container_tag
                    >::type
                boundary_cell_view;
                
                
                
                //
                // Orientation
                //
                
                typedef typename viennagrid::result_of::permutator_type<
                    viennagrid::boundary_elements<
                        boundary_cell_tag,
                        typename boundary_cell_tag::facet_tag
                    >::num
                >::type permutator_type;
                
                typedef typename boundary_cell_container_tag<
                        typename viennagrid::boundary_elements< boundary_cell_tag, typename boundary_cell_tag::facet_tag>::layout_tag,
                        viennagrid::boundary_elements< boundary_cell_tag, typename boundary_cell_tag::facet_tag>::num
                    >::type orientation_container_tag;
                
                
                typedef typename viennagrid::storage::result_of::container<permutator_type, orientation_container_tag>::type orientation_container_type;
                typedef viennagrid::element_orientation<orientation_container_type> facet_orientation_type;
            
                typedef typename 
                    viennagrid::meta::IF<
                        has_orientation< domain_config, element_tag, boundary_cell_tag>::value,
                        typename viennagrid::storage::result_of::container< facet_orientation_type, container_tag >::type,
                        viennagrid::meta::null_type
                    >::type facet_orientation_container_type;
                
                    
                //
                // final container pair
                //
                    
                typedef viennagrid::meta::static_pair<
                                boundary_cell_view,
                                facet_orientation_container_type
                            > type;
            };
            
            template<typename domain_config, typename element_tag, bool is_present>
            struct element_boundary_cell_container_helper<domain_config, element_tag, vertex_tag, is_present>
            {
                
                typedef vertex_tag boundary_cell_tag;
                
                //
                // boundary cell view
                //
                
                typedef element_t<boundary_cell_tag, domain_config> boundary_cell_type;
                
                typedef typename viennagrid::storage::result_of::container<
                    boundary_cell_type,                                         // the 'value_type', i.e. vertices     
                    typename query_element_container_tag<domain_config, boundary_cell_tag>::type
                >::type boundary_cell_container;
                
                
                typedef typename boundary_cell_container_tag<
                        typename viennagrid::boundary_elements< element_tag, boundary_cell_tag>::layout_tag,
                        viennagrid::boundary_elements< element_tag, boundary_cell_tag>::num
                    >::type container_tag;
                

                typedef typename viennagrid::storage::result_of::view<
                        boundary_cell_container,
                        container_tag
                    >::type
                boundary_cell_view;
                
                
                
                //
                // Orientation
                //
            
                typedef viennagrid::meta::null_type facet_orientation_container_type;
                
                    
                //
                // final container pair
                //
                    
                typedef viennagrid::meta::static_pair<
                                boundary_cell_view,
                                facet_orientation_container_type
                            > type;
            };
            
            
            
            
            
            
            template<typename config_, typename element_tag_, typename boundary_cell_tag_>
            struct element_boundary_cell_container_helper<config_, element_tag_, boundary_cell_tag_, false>
            {
                typedef viennagrid::meta::null_type type;
            };
            
            
            template<typename config, typename element_tag, typename boundary_cell_tag>
            struct element_boundary_cell_container
            {
                static const bool is_present = has_boundary_cells<config, element_tag, boundary_cell_tag>::value;
                
                typedef typename element_boundary_cell_container_helper<config, element_tag, boundary_cell_tag, is_present>::type type;
            };
            
            
            
            template<typename config, typename element_tag, typename boundary_cell_tag>
            struct element_boundary_cell_container_typelist
            {
                typedef typename boundary_cell_tag::facet_tag facet_tag;
                
                typedef typename element_boundary_cell_container<config, element_tag, facet_tag>::type boundary_cell_layer_containers;
                typedef typename viennagrid::meta::typelist::result_of::push_back<
                    typename element_boundary_cell_container_typelist< config, element_tag, facet_tag >::type,
                    boundary_cell_layer_containers
                >::type type;
            };
            
            template<typename config, typename element_tag>
            struct element_boundary_cell_container_typelist<config, element_tag, viennagrid::vertex_tag>
            {
                typedef viennagrid::meta::null_type type;
            };          
        }
            
            
            
            
    }

}


#endif
