/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
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

#ifndef VIENNAGRID_ALGORITHM_CENTROID_HPP
#define VIENNAGRID_ALGORITHM_CENTROID_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/forwards.h"
#include "viennagrid/topology/all.hpp"

/** @file centroid.hpp
    @brief Computes the centroid (center of mass) for different cell types
*/


namespace viennagrid
{
    
    //
    // Calculation of circumcenter taken from Wikipedia (better reference required...)
    //
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::triangle_tag)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename ElementType::element_tag             ElementTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type            VertexOnCellIterator;
      
      PointType p0(0.0, 0.0);
      
      VertexOnCellRange vertices = viennagrid::ncells<0>(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
      {
        p0 += vocit->point();
      }
      
      p0 /= viennagrid::topology::subcell_desc<ElementTag, 0>::num_elements;
      
      return p0;
    }

    //tetrahedron can reuse the algorithm defined for a triangle
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //
    // TODO: This works for rectangles only, but not for general quadrilateral
    //
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //
    // TODO: This works for cuboids only, but not for general hexahedra
    //
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::hexahedron_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }


    //a line can reuse the algorithm defined for a triangle
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::line_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //
    // The public interface function
    //
    template <typename CellType>
    typename viennagrid::result_of::point<typename CellType::config_type>::type
    centroid(CellType const & cell)
    {
      return centroid(cell, typename CellType::element_tag());
    }
    
} //namespace viennashe
#endif
