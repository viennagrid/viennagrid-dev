#ifndef VIENNAGRID_TOPOLOGY_POLYGON_HPP
#define VIENNAGRID_TOPOLOGY_POLYGON_HPP

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

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/simplex.hpp"

/** @file polygon.hpp
    @brief Provides the topological definition of a polygon
*/

namespace viennagrid
{

  struct polygon_tag
  {
    typedef simplex_tag<1> facet_tag;
      
    enum{ dim = 2 };
    static std::string name() { return "Polygon"; }
  };

    struct hole_polygon_tag
    {
        typedef simplex_tag<1> facet_tag;
        
        enum{ dim = 2 };
        static std::string name() { return "Hole Polygon"; }
    };
  
  
  namespace topology
  {

    //Line:
    /** @brief Topological description of the 0-cells of a polygon */
    template <>
    struct boundary_cells<polygon_tag, vertex_tag>
    {
      typedef dynamic_layout_tag     layout_tag;
      enum{ num = -1 };
    };
    
    template <>
    struct boundary_cells<polygon_tag, line_tag>
    {
      typedef dynamic_layout_tag     layout_tag;
      enum{ num = -1 };
    };
    
    
    
    template <>
    struct boundary_cells<hole_polygon_tag, vertex_tag>
    {
      typedef dynamic_layout_tag     layout_tag;
      enum{ num = -1 };
    };
    
    template <>
    struct boundary_cells<hole_polygon_tag, line_tag>
    {
      typedef dynamic_layout_tag     layout_tag;
      enum{ num = -1 };
    };

    
    
    template<typename bnd_cell_type>
    struct bndcell_generator<polygon_tag, simplex_tag<1>, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & element, inserter_type & inserter)
        {
            bnd_cell_type bnd_cell( inserter.get_physical_container_collection() );
            
            int index = 0;
            for (int i = 0; i < element.container( dimension_tag<0>() ).size()-1; ++i)
            {
                bnd_cell.container(dimension_tag<0>()).set_hook( element.container( dimension_tag<0>() ).hook_at(i), 0 );
                bnd_cell.container(dimension_tag<0>()).set_hook( element.container( dimension_tag<0>() ).hook_at(i+1), 1 ); 
                element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            }
                
                
            bnd_cell.container(dimension_tag<0>()).set_hook( element.container( dimension_tag<0>() ).hook_at( element.container( dimension_tag<0>() ).size()-1 ), 0 );
            bnd_cell.container(dimension_tag<0>()).set_hook( element.container( dimension_tag<0>() ).hook_at(0), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
        }
    };

    template<typename bnd_cell_type>
    struct bndcell_generator<hole_polygon_tag, simplex_tag<1>, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & element, inserter_type & inserter)
        {
            bndcell_generator< polygon_tag, simplex_tag<1>, bnd_cell_type >::create_bnd_cells(element, inserter);
        }
    };

    

  } //topology
}

#endif

