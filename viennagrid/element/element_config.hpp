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


#include "viennagrid/element/element.hpp"

/** @file element.hpp
    @brief Provides the main n-cell type
*/

namespace viennagrid
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
        
        
        
        //
        // Generates an element type
        //
        template<typename config__, typename element_tag__>
        struct element;
        
        template<typename domain_config, typename element_tag>
        struct is_element_present
        {
            static const bool value =
                !viennameta::_equal<
                    typename viennameta::typemap::result_of::find<
                        domain_config,
                        element_tag
                    >::type,
                    viennameta::not_found
                >::value;
        };
        
        
        template<typename domain_config, typename element_tag>
        struct element_id_tag
        {
            typedef typename viennameta::typemap::result_of::find< domain_config, element_tag >::type::second element_config;
            typedef typename viennameta::typemap::result_of::find< element_config, viennagrid::element_id_tag >::type::second type;
        };
        
        template<typename domain_config, typename element_tag>
        struct element_container_tag
        {
            typedef typename viennameta::typemap::result_of::find< domain_config, element_tag >::type::second element_config;
            typedef typename viennameta::typemap::result_of::find< element_config, viennagrid::element_container_tag >::type::second type;
        };
        

        template<typename domain_config, typename element_tag, typename boundary_cell_tag>
        struct boundary_storage_layout
        {
            typedef typename viennameta::typemap::result_of::find< domain_config, element_tag >::type::second element_config;
            typedef typename viennameta::typemap::result_of::find< element_config, viennagrid::element_boundary_storage_layout_tag >::type::second element_storage_layout_config;
            typedef typename viennameta::typemap::result_of::find< element_storage_layout_config, boundary_cell_tag >::type::second boundary_cell_storage_handling;
            //typedef typename viennameta::typemap::result_of::find< element_storage_layout_config, boundary_cell_tag >::type::second boundary_cell_storage_layout;
            
            //typedef typename boundary_cell_storage_layout::first storage_tag;
            //typedef typename boundary_cell_storage_layout::second orientation_tag;
        };
        
        
        
        
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__, bool is_present = is_element_present<domain_config__, boundary_cell_tag__>::value >
        struct has_boundary_cells_helper
        {
            static const bool value = 
                !viennameta::_equal<
                    typename boundary_storage_layout<domain_config__, element_tag__, boundary_cell_tag__>::boundary_cell_storage_handling,
                    viennagrid::no_handling_tag
                >::value;
        };
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__>
        struct has_boundary_cells_helper<domain_config__, element_tag__, boundary_cell_tag__, false>
        {
            static const bool value = false;
        };
        
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__>
        struct has_boundary_cells
        {
            static const bool is_present = is_element_present<domain_config__, boundary_cell_tag__>::value;
                        
            static const bool value = has_boundary_cells_helper<domain_config__, element_tag__, boundary_cell_tag__, is_present>::value;
        };
        
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__, bool is_present = is_element_present<domain_config__, boundary_cell_tag__>::value >
        struct has_orientation_helper
        {
            static const bool value = 
                viennameta::_equal<
                    typename boundary_storage_layout<domain_config__, element_tag__, boundary_cell_tag__>::boundary_cell_storage_handling,
                    viennagrid::full_handling_tag
                >::value
                    ||
                viennameta::_equal<
                    typename boundary_storage_layout<domain_config__, element_tag__, boundary_cell_tag__>::boundary_cell_storage_handling,
                    viennagrid::full_lazy_handling_tag
                >::value;
        };
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__>
        struct has_orientation_helper<domain_config__, element_tag__, boundary_cell_tag__, false>
        {
            static const bool value = false;
        };
        
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__>
        struct has_orientation
        {
            static const bool is_present = is_element_present<domain_config__, boundary_cell_tag__>::value;
                        
            static const bool value = has_orientation_helper<domain_config__, element_tag__, boundary_cell_tag__, is_present>::value;
        };
        
        
        
        
        
        // TODO: handle possible other tags (lazy_handling_tag, ...)
        template<typename domain_config, typename element_tag, typename boundary_cell_tag, bool is_present>
        struct element_boundary_cell_container_helper
        {
            
            //
            // boundary cell view
            //
            
            typedef typename element<domain_config, boundary_cell_tag>::type boundary_cell_type;
            
            typedef typename viennagrid::storage::result_of::container<
                boundary_cell_type,                                         // the 'value_type', i.e. vertices     
                typename element_container_tag<domain_config, boundary_cell_tag>::type
            >::type boundary_cell_container;
            
            
            typedef typename boundary_cell_container_tag<
                    typename viennagrid::topology::boundary_cells< element_tag, boundary_cell_tag>::layout_tag,
                    viennagrid::topology::boundary_cells< element_tag, boundary_cell_tag>::num
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
                viennagrid::topology::boundary_cells<
                    boundary_cell_tag,
                    typename boundary_cell_tag::facet_tag
                >::num
            >::type permutator_type;
            
            typedef typename boundary_cell_container_tag<
                    typename viennagrid::topology::boundary_cells< element_tag, boundary_cell_tag>::layout_tag,
                    viennagrid::topology::boundary_cells< element_tag, boundary_cell_tag>::num
                >::type orientation_container_tag;
            
            
            typedef typename viennagrid::storage::result_of::container<permutator_type, container_tag>::type orientation_container_type;
            typedef viennagrid::element_orientation<orientation_container_type> facet_orientation_type;
           
            typedef typename 
                viennameta::_if<
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
        
        template<typename config__, typename element_tag__, typename boundary_cell_tag__>
        struct element_boundary_cell_container_helper<config__, element_tag__, boundary_cell_tag__, false>
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
        
        
        template<typename config__, typename element_tag__>
        struct element
        {
            typedef typename element_id_tag<config__, element_tag__>::type id_tag;
            
            typedef typename element_boundary_cell_container_typelist<config__, element_tag__, element_tag__>::type container_typelist;
            
            typedef viennagrid::element_t<element_tag__, container_typelist, id_tag> type;
        };

    }
        
        
        
        
    namespace result_of
    {
        template<typename domain_config, long dim>
        struct ncell_tag;
        
        template<typename element_tag, typename element_config, typename tail, long dim>
        struct ncell_tag< viennameta::typelist_t< viennameta::static_pair<element_tag, element_config>, tail >, dim >
        {
            typedef typename viennameta::_if<
                element_tag::dim == dim,
                element_tag,
                typename ncell_tag<tail, dim>::type
            >::type type;
        };
        
        template<long dim>
        struct ncell_tag<viennameta::null_type, dim>
        {
            typedef viennameta::null_type type;
        };
        

        template<typename domain_config, long dim>
        struct ncell
        {
            typedef typename element<
                domain_config,
                typename ncell_tag<domain_config, dim>::type
            >::type type;
        };
        
        
    }
        
        
        
    namespace result_of
    {
        //
        // generates a container for a specified element_tag for the domain container collection
        //
        template<typename config, typename element_tag>
        struct element_container
        {
            typedef typename element<config, element_tag>::type element_type;
            typedef typename viennagrid::storage::result_of::container<element_type, typename element_container_tag<config, element_tag>::type >::type type;
        };
        
        
        //
        // generates the container typelist for the domain container collection
        //
        template<typename config, typename cur_config = config>
        struct element_container_typelist;
        
        template<typename config, typename key, typename value, typename tail>
        struct element_container_typelist< config, viennameta::typelist_t< viennameta::static_pair<key, value>, tail > >
        {
            typedef viennameta::typelist_t<
                typename element_container<config, key>::type,
                typename element_container_typelist<config, tail>::type
            > type;
        };
        
        template<typename config>
        struct element_container_typelist<config, viennameta::null_type>
        {
            typedef viennameta::null_type type;
        };
    }
    
    namespace result_of
    {
        
        template<typename domain_config, typename element_tag>
        struct element_hook
        {
            typedef typename element_container<domain_config, element_tag>::type::hook_type type;
        };
        
    }
  
}


#endif
