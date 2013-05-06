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
#include "viennagrid/element/element.hpp"
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

            template<typename domain_config, typename element_tag>
            struct is_element_present
            {
                typedef typename query_config<domain_config, topology_config_tag>::type topology_config;
                
                static const bool value =
                    !viennameta::EQUAL<
                        typename viennameta::typemap::result_of::find<
                            topology_config,
                            element_tag
                        >::type,
                        viennameta::not_found
                    >::value;
            };
            
            
            template<typename domain_config, typename element_tag>
            struct query_element_id_tag
            {
                typedef typename query_config<domain_config, topology_config_tag>::type topology_config;
                
                typedef typename query_config<topology_config, element_tag>::type element_config;
                
//                 typedef typename viennameta::typemap::result_of::find< topology_config, element_tag >::type::second element_config;
                typedef typename query_config<element_config, element_id_tag>::type type;
            };
            
            template<typename domain_config, typename element_tag>
            struct query_element_container_tag
            {
                typedef typename query_config<domain_config, topology_config_tag>::type topology_config;
                
                typedef typename query_config<topology_config, element_tag>::type element_config;
//                 typedef typename viennameta::typemap::result_of::find< topology_config, element_tag >::type::second element_config;
                typedef typename query_config<element_config, element_container_tag>::type type;
            };
            

            template<typename domain_config, typename element_tag, typename boundary_cell_tag>
            struct query_boundary_storage_layout
            {
                typedef typename query_config<domain_config, topology_config_tag>::type topology_config;
                
                typedef typename query_config<topology_config, element_tag>::type element_config;
                
//                 typedef typename viennameta::typemap::result_of::find< topology_config, element_tag >::type::second element_config;
                typedef typename viennameta::typemap::result_of::find< element_config, element_boundary_storage_layout_tag >::type::second element_storage_layout_config;
                typedef typename viennameta::typemap::result_of::find< element_storage_layout_config, boundary_cell_tag >::type::second boundary_cell_storage_handling;
                //typedef typename viennameta::typemap::result_of::find< element_storage_layout_config, boundary_cell_tag >::type::second boundary_cell_storage_layout;
                
                //typedef typename boundary_cell_storage_layout::first storage_tag;
                //typedef typename boundary_cell_storage_layout::second orientation_tag;
            };
            
            
            
            
            
            template<typename domain_config_, typename element_tag_, typename boundary_cell_tag_, bool is_present = is_element_present<domain_config_, boundary_cell_tag_>::value >
            struct has_boundary_cells_helper
            {
                static const bool value = 
                    !viennameta::EQUAL<
                        typename query_boundary_storage_layout<domain_config_, element_tag_, boundary_cell_tag_>::boundary_cell_storage_handling,
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
                    viennameta::EQUAL<
                        typename query_boundary_storage_layout<domain_config_, element_tag_, boundary_cell_tag_>::boundary_cell_storage_handling,
                        viennagrid::full_handling_tag
                    >::value
                        ||
                    viennameta::EQUAL<
                        typename query_boundary_storage_layout<domain_config_, element_tag_, boundary_cell_tag_>::boundary_cell_storage_handling,
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
                
                typedef typename viennagrid::result_of::element<domain_config, boundary_cell_tag>::type boundary_cell_type;
                
                typedef typename viennagrid::storage::result_of::container<
                    boundary_cell_type,                                         // the 'value_type', i.e. vertices     
                    typename query_element_container_tag<domain_config, boundary_cell_tag>::type
                >::type boundary_cell_container;
                
                
                typedef typename boundary_cell_container_tag<
                        typename viennagrid::element_topology::boundary_cells< element_tag, boundary_cell_tag>::layout_tag,
                        viennagrid::element_topology::boundary_cells< element_tag, boundary_cell_tag>::num
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
                    viennagrid::element_topology::boundary_cells<
                        boundary_cell_tag,
                        typename boundary_cell_tag::facet_tag
                    >::num
                >::type permutator_type;
                
                typedef typename boundary_cell_container_tag<
                        typename viennagrid::element_topology::boundary_cells< boundary_cell_tag, typename boundary_cell_tag::facet_tag>::layout_tag,
                        viennagrid::element_topology::boundary_cells< boundary_cell_tag, typename boundary_cell_tag::facet_tag>::num
                    >::type orientation_container_tag;
                
                
                typedef typename viennagrid::storage::result_of::container<permutator_type, orientation_container_tag>::type orientation_container_type;
                typedef viennagrid::element_orientation<orientation_container_type> facet_orientation_type;
            
                typedef typename 
                    viennameta::IF<
                        has_orientation< domain_config, element_tag, boundary_cell_tag>::value,
                        typename viennagrid::storage::result_of::container< facet_orientation_type, container_tag >::type,
                        viennameta::null_type
                    >::type facet_orientation_container_type;
                
                    
                //
                // final container pair
                //
                    
                typedef viennameta::static_pair<
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
                
                typedef typename viennagrid::result_of::element<domain_config, boundary_cell_tag>::type boundary_cell_type;
                
                typedef typename viennagrid::storage::result_of::container<
                    boundary_cell_type,                                         // the 'value_type', i.e. vertices     
                    typename query_element_container_tag<domain_config, boundary_cell_tag>::type
                >::type boundary_cell_container;
                
                
                typedef typename boundary_cell_container_tag<
                        typename viennagrid::element_topology::boundary_cells< element_tag, boundary_cell_tag>::layout_tag,
                        viennagrid::element_topology::boundary_cells< element_tag, boundary_cell_tag>::num
                    >::type container_tag;
                

                typedef typename viennagrid::storage::result_of::view<
                        boundary_cell_container,
                        container_tag
                    >::type
                boundary_cell_view;
                
                
                
                //
                // Orientation
                //
            
                typedef viennameta::null_type facet_orientation_container_type;
                
                    
                //
                // final container pair
                //
                    
                typedef viennameta::static_pair<
                                boundary_cell_view,
                                facet_orientation_container_type
                            > type;
            };
            
            
            
            
            
            
            template<typename config_, typename element_tag_, typename boundary_cell_tag_>
            struct element_boundary_cell_container_helper<config_, element_tag_, boundary_cell_tag_, false>
            {
                typedef viennameta::null_type type;
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
                typedef typename viennameta::typelist::result_of::push_back<
                    typename element_boundary_cell_container_typelist< config, element_tag, facet_tag >::type,
                    boundary_cell_layer_containers
                >::type type;
            };
            
            template<typename config, typename element_tag>
            struct element_boundary_cell_container_typelist<config, element_tag, viennagrid::vertex_tag>
            {
                typedef viennameta::null_type type;
            };
            
            
            
            template<typename domain_config, typename element_tag_>
            struct element_from_config_impl;
            
            template<typename config_element_tag, typename config_element_config, typename config_tail, typename element_tag_>
            struct element_from_config_impl< viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail >, element_tag_ >
            {
                typedef viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail > domain_config;
                typedef element_tag_ element_tag;
                
                typedef typename query_element_id_tag<domain_config, element_tag>::type id_tag;
                
                typedef typename element_boundary_cell_container_typelist<domain_config, element_tag, element_tag>::type container_typelist;
                
                typedef viennagrid::element_t<element_tag, container_typelist, id_tag> type;
            };        
            
        }
            
            
            
            
        namespace result_of
        {
    //         template<typename domain_config, long dim>
    //         struct ncell_tag;
    //         
    //         template<typename element_tag, typename element_config, typename tail, long dim>
    //         struct ncell_tag< viennameta::typelist_t< viennameta::static_pair<element_tag, element_config>, tail >, dim >
    //         {
    //             typedef typename viennameta::IF<
    //                 element_tag::dim == dim,
    //                 element_tag,
    //                 typename ncell_tag<tail, dim>::type
    //             >::type type;
    //         };
    //         
    //         template<long dim>
    //         struct ncell_tag<viennameta::null_type, dim>
    //         {
    //             typedef viennameta::null_type type;
    //         };
    //         
    // 
    //         template<typename head, typename tail, long dim>
    //         struct ncell< viennameta::typelist_t<head, tail>, dim >
    //         {
    //             typedef viennameta::typelist_t<head, tail> domain_config;
    //             typedef typename element<
    //                 domain_config,
    //                 typename ncell_tag<domain_config, dim>::type
    //             >::type type;
    //         };
            
            
        }
            
            
            
        namespace result_of
        {

        }  
    
    }
    
    
    
    
    namespace result_of
    {
        template<typename config_element_tag, typename config_element_config, typename config_tail, typename element_tag_>
        struct element< viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail >, element_tag_ >
        {
            typedef viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail > domain_config;
//             typedef typename config::result_of::query_config<domain_config, config::topology_config_tag>::type topology_config;
            typedef typename viennagrid::config::result_of::element_from_config_impl< domain_config, element_tag_ >::type type;
        };        
    }
}


#endif
