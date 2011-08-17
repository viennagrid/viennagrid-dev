/* =======================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
       ViennaSHE - The Vienna Spherical Harmonics Expansion Simulator
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Markus Bina                        bina@iue.tuwien.ac.at

   license:    To be discussed, see file LICENSE in the ViennaSHE base directory
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
    typename viennagrid::result_of::point_type<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::triangle_tag)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename ElementType::element_tag             ElementTag;
      typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell_type<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_container<ElementType, 0>::type         VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type            VertexOnCellIterator;
      
      PointType p0(0.0, 0.0);
      
      VertexOnCellContainer vertices = viennagrid::ncells<0>(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
      {
        p0 += vocit->getPoint();
      }
      
      p0 /= viennagrid::traits::subcell_desc<ElementTag, 0>::num_elements;
      
      return p0;
    }

    //tetrahedron can reuse the algorithm defined for a triangle
    template <typename ElementType>
    typename viennagrid::result_of::point_type<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //
    // TODO: This works for rectangles only, but not for general quadrilateral
    //
    template <typename ElementType>
    typename viennagrid::result_of::point_type<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //
    // TODO: This works for cuboids only, but not for general hexahedra
    //
    template <typename ElementType>
    typename viennagrid::result_of::point_type<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::hexahedron_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }


    //a line can reuse the algorithm defined for a triangle
    template <typename ElementType>
    typename viennagrid::result_of::point_type<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::line_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //
    // The public interface function
    //
    template <typename CellType>
    typename viennagrid::result_of::point_type<typename CellType::config_type>::type
    centroid(CellType const & cell)
    {
      return centroid(cell, typename CellType::element_tag());
    }
    
} //namespace viennashe
#endif
