#ifndef VIENNAGRID_TOPOLOGY_SIMPLEX_HPP
#define VIENNAGRID_TOPOLOGY_SIMPLEX_HPP

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
#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
// #include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/algorithm/norm.hpp"

/** @file topology/simplex.hpp
    @brief Provides the topological definition of an arbitrary simplex element
*/

namespace viennagrid
{
  namespace meta  //TODO: agree on handling meta stuff
  {
    template <long n, long k>
    struct n_over_k
    {
      enum { value = n_over_k<n-1, k-1>::value + n_over_k<n-1, k>::value };
    };
    
    template <long n>
    struct n_over_k<n, 0>
    {
      enum { value = 1 };
    };
    
    template <long k>
    struct n_over_k<0, k>
    {
      enum { value = 0 };
    };
    
    template <>
    struct n_over_k<0, 0>
    {
      enum { value = 1 };
    };
    
  }
  

  /** @brief Topological description of an n-simplex.*/
  template <long n>
  struct simplex_tag
  {
    typedef simplex_tag<n-1> facet_tag;
      
    enum{ dim = n };
    static std::string name() 
    {
      std::stringstream ss;
      ss << n << "-simplex";
      return ss.str();
    }
  };

  
  namespace topology
  {
    /** @brief Topological description of the boundary k-cells an n-simplex */
//     template <long n, long k>
//     struct bndcells<simplex_tag<n>, k>
//     {
//       typedef simplex_tag<k>             tag;
// 
//       typedef static_layout_tag     layout_tag;
//       enum{ num = meta::n_over_k<n+1, k+1>::value };
//     };
    
    template <long n, long k>
    struct boundary_cells<simplex_tag<n>, simplex_tag<k> >
    {
      //typedef simplex_tag<k>             tag;

      typedef static_layout_tag     layout_tag;
      enum{ num = meta::n_over_k<n+1, k+1>::value };
    };

  
    ///////////////////////////////// Generator for boundary cell elements ///////////////////////////////////
    
    
    template<long n, typename bnd_cell_type>
    struct bndcell_generator<simplex_tag<n>, simplex_tag<1>, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & element, inserter_type & inserter)
        {
            bnd_cell_type bnd_cell( inserter.get_physical_container_collection() );
            
            int index = 0;
            for (int i = 0; i < boundary_cells<simplex_tag<n>, vertex_tag >::num; ++i)
                for (int j = i+1; j < boundary_cells<simplex_tag<n>, vertex_tag >::num; ++j)
                {
                    bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(i), 0 );
                    bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(j), 1 );
                    
//                     std::cout << "## " << i << " " << j << std::endl;
//                     std::cout << "## bnd cell generation for " << n << "-simplex: " << element.container( dimension_tag<0>() ).handle_at(i) << " " << element.container( dimension_tag<0>() ).handle_at(j) << std::endl;
                    
                    element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
                }
        }
    };
    
    
    template<long n, typename bnd_cell_type>
    struct bndcell_generator<simplex_tag<n>, simplex_tag<2>, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & element, inserter_type & inserter)
        {
            bnd_cell_type bnd_cell( inserter.get_physical_container_collection() );
            
            int index = 0;
            for (int i = 0; i < boundary_cells<simplex_tag<n>, vertex_tag >::num; ++i)
                for (int j = i+1; j < boundary_cells<simplex_tag<n>, vertex_tag >::num; ++j)
                    for (int k = j+1; k < boundary_cells<simplex_tag<n>, vertex_tag >::num; ++k)
                    {
                        bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(i), 0 );
                        bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(j), 1 );
                        bnd_cell.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(k), 2 );
                        
                        element.set_bnd_cell( bnd_cell, inserter(bnd_cell), index++ );
                    }
        }
    };

    
    // similarly for higher topological dimensions...
    
  } //topology
  
    
}

#endif

