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

#ifndef VIENNAGRID_ALGORITHM_CIRCUMCENTER_HPP
#define VIENNAGRID_ALGORITHM_CIRCUMCENTER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/forwards.h"
#include "viennagrid/algorithm/spanned_volume.hpp"

#include "viennadata/api.hpp"

/** @file voronoi.hpp
    @brief Computes the Voronoi information for a Delaunay triangulation.
*/


namespace viennagrid
{
    
    //
    // Calculation of circumcenter for a line
    //
    template <typename ElementType>
    typename viennagrid::result_of::point_type<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, viennagrid::line_tag)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
      
      typedef typename viennagrid::result_of::const_ncell_container<ElementType, 0>::type         VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type            VertexOnCellIterator;

      VertexOnCellContainer vertices = viennagrid::ncells<0>(cell);
      VertexOnCellIterator vocit = vertices.begin();
      
      PointType const & A = vocit->getPoint(); ++vocit;
      PointType const & B = vocit->getPoint(); 
      
      PointType ret = A + B;
      ret /= 2.0;
      
      return ret;
    }
  
  
    //
    // Calculation of circumcenter taken from Wikipedia (better reference required...)
    //
    template <typename ElementType>
    typename viennagrid::result_of::point_type<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, viennagrid::triangle_tag)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell_type<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_container<ElementType, 0>::type         VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type            VertexOnCellIterator;

      VertexOnCellContainer vertices = viennagrid::ncells<0>(cell);
      VertexOnCellIterator vocit = vertices.begin();
      
      PointType const & A = vocit->getPoint(); ++vocit;
      PointType const & B = vocit->getPoint(); ++vocit;
      PointType const & C = vocit->getPoint();
      
      PointType circ_cent;
      
      double D = 2.0 * (   A[0] * (B[1] - C[1])
                         + B[0] * (C[1] - A[1])
                         + C[0] * (A[1] - B[1]) );
      
      double x =  (   (A[1] * A[1] + A[0] * A[0]) * (B[1] - C[1])
                    + (B[1] * B[1] + B[0] * B[0]) * (C[1] - A[1])
                    + (C[1] * C[1] + C[0] * C[0]) * (A[1] - B[1])  ) / D;
      
      double y =  (   (A[1] * A[1] + A[0] * A[0]) * (C[0] - B[0])
                    + (B[1] * B[1] + B[0] * B[0]) * (A[0] - C[0])
                    + (C[1] * C[1] + C[0] * C[0]) * (B[0] - A[0])  ) / D;
      
      circ_cent[0] = x;              
      circ_cent[1] = y;
      
      return circ_cent;
    }


    //
    // TODO: This works for rectangles only, but not for general quadrilateral
    //
    template <typename CellType>
    typename viennagrid::result_of::point_type<typename CellType::config_type>::type
    circumcenter(CellType const & cell, viennagrid::quadrilateral_tag)
    {
      typedef typename CellType::config_type             Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell_type<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 0>::type         VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type      VertexOnCellIterator;

      PointType p0(0.0, 0.0);
      
      VertexOnCellContainer vertices = viennagrid::ncells<0>(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
      {
        p0 += vocit->getPoint();
      }
      
      p0 /= viennagrid::traits::subcell_desc<CellTag, 0>::num_elements;
      
      return p0;
    }


    template <typename CellType>
    typename viennagrid::result_of::point_type<typename CellType::config_type>::type
    circumcenter(CellType const & cell)
    {
      return circumcenter(cell, typename CellType::element_tag());
    }
    
} //namespace viennashe
#endif
