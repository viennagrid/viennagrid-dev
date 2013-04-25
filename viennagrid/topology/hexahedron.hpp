#ifndef VIENNAGRID_CONFIG_HEXAHEDRON_HPP
#define VIENNAGRID_CONFIG_HEXAHEDRON_HPP

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
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/quadrilateral.hpp"

/** @file hexahedron.hpp
    @brief Provides the topological definition of a hexahedron
*/

namespace viennagrid
{

  /** @brief A hypercube of dimension 3, aka. hexahedron */
  template <>
  struct hypercube_tag<3>
  {
    typedef hypercube_tag<2> facet_tag;
      
    enum{ dim = 3 };
    static std::string name() { return "Hexahedron"; }
  };
  

  namespace element_topology
  {

    template <>
    struct boundary_cells<hypercube_tag<3>, hypercube_tag<2> >
    {
      //typedef simplex_tag<k>             tag;

      typedef static_layout_tag     layout_tag;
      enum{ num = 6 };
    };
    
    template <>
    struct boundary_cells<hypercube_tag<3>, simplex_tag<1> >
    {
      //typedef simplex_tag<k>             tag;

      typedef static_layout_tag     layout_tag;
      enum{ num = 12 };
    };
    
    template <>
    struct boundary_cells<hypercube_tag<3>, simplex_tag<0> >
    {
      //typedef simplex_tag<k>             tag;

      typedef static_layout_tag     layout_tag;
      enum{ num = 8 };
    };
    
    
    
//         //fill edges according to reference orientation (use monospaced font for Full-HD ascii-art):
//         //
//         //              e11
//         //    v6-----------------v7
//         // e9  /|            e10/|
//         //    / |   e8      v5 / |
//         // v4------------------  |e7
//         //   |  |e6           |  |
//         // e2|  |______e5_____|__|
//         //   |  /v2         e4| /  v3
//         //   | /e1            |/ e3
//         // v0 ----------------  v1
//         //           e0 
//         //
    
    template<typename bnd_cell_type>
    struct bndcell_generator<hypercube_tag<3>, simplex_tag<1>, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & element, inserter_type & inserter)
        {
            bnd_cell_type bnd_cell( inserter.get_physical_container_collection() );
            int index = 0;
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(0), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(1), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );

            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(0), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(2), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(0), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(4), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(1), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(3), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(1), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(5), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(2), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(3), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(2), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(5), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(3), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(7), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(4), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(5), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );

            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(4), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(6), 1 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(5), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(7), 1 );        
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(6), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(7), 1 );        
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
        }
    };
    
    
    
    
//         //fill edges according to reference orientation (use monospaced font for Full-HD ascii-art):
//         //
//         //              
//         //    v6-----------------v7
//         //     /|      f5       /|
//         //    / |           v5 / |
//         // v4------------------  |
//         //   |f2|  (f4-back)  |f3|
//         //   |  |_____________|__|
//         //   |  /v2   f1      | /  v3
//         //   | /              |/ 
//         // v0 ----------------  v1
//         //       (f0-bottom)
//         //
//         // orientation is such that facet normals point out of the cell
    
    template<typename bnd_cell_type>
    struct bndcell_generator<hypercube_tag<3>, hypercube_tag<2>, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & element, inserter_type & inserter)
        {
            bnd_cell_type bnd_cell( inserter.get_physical_container_collection() );
            int index = 0;
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(0), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(1), 1 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(2), 2 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(3), 3 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );

            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(0), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(1), 1 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(4), 2 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(5), 3 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(0), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(2), 1 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(4), 2 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(6), 3 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(1), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(3), 1 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(5), 2 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(7), 3 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(2), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(3), 1 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(6), 2 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(7), 3 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
            
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(4), 0 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(5), 1 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(6), 2 );
            bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(7), 3 );
            element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
        }
    };
        
  } //topology
}

#endif

