#ifndef VIENNAGRID_ALGORITHM_CIRCUMCENTER_HPP
#define VIENNAGRID_ALGORITHM_CIRCUMCENTER_HPP

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

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/forwards.h"
#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

#include "viennadata/api.hpp"

/** @file voronoi.hpp
    @brief Computes the Voronoi information for a Delaunay triangulation.
*/

namespace viennagrid
{
  namespace detail
  {
    template <typename ElementType, typename ElementTag, typename DimensionTag>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, ElementTag const &, DimensionTag const &)
    {
      typedef typename ElementType::ERROR_COMPUTATION_OF_CIRCUMCENTER_NOT_IMPLEMENTED   error_type;
      return typename viennagrid::result_of::point<typename ElementType::config_type>::type();
    }
    
    
    //a point is degenerate and returns its location
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, viennagrid::point_tag)
    {
      return cell.point();
    }
    
    //
    // Calculation of circumcenter for a line
    //
    template <typename ElementType, typename DimensionTag>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, viennagrid::simplex_tag<1>, DimensionTag)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type         VertexOnCellRange;

      VertexOnCellRange vertices = viennagrid::ncells<0>(cell);
      
      PointType const & A = vertices[0].point();
      PointType const & B = vertices[1].point();
      
      PointType ret = A + B;
      ret /= 2.0;
      
      return ret;
    }

    template <typename ElementType, typename DimensionTag>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, viennagrid::hypercube_tag<1>, DimensionTag)
    {
      return circumcenter(cell, viennagrid::simplex_tag<1>(), DimensionTag());
    }
  
    //
    // Calculation of circumcenter taken from Wikipedia (better reference required...)
    //
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, viennagrid::triangle_tag, viennagrid::dimension_tag<2>)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type         VertexOnCellRange;

      VertexOnCellRange vertices = viennagrid::ncells<0>(cell);
      
      PointType const & A = vertices[0].point();
      PointType const & B = vertices[1].point();
      PointType const & C = vertices[2].point();
      
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
    // TODO: This works for rectangles only, but not for general quadrilaterals
    //
    template <typename CellType>
    typename viennagrid::result_of::point<typename CellType::config_type>::type
    circumcenter(CellType const & cell, viennagrid::quadrilateral_tag, viennagrid::dimension_tag<2>)
    {
      typedef typename CellType::config_type             Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type      VertexOnCellIterator;

      PointType p0(0.0, 0.0);
      
      VertexOnCellRange vertices = viennagrid::ncells<0>(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
      {
        p0 += vocit->point();
      }
      
      p0 /= viennagrid::topology::bndcells<CellTag, 0>::num;
      
      return p0;
    }



    //
    // Calculation of circumcenter taken from Wikipedia (better reference required...)
    //
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, viennagrid::triangle_tag, viennagrid::dimension_tag<3>)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type         VertexOnCellRange;

      VertexOnCellRange vertices = viennagrid::ncells<0>(cell);
      
      PointType const & A = vertices[0].point();
      PointType const & B = vertices[1].point();
      PointType const & C = vertices[2].point();
      
      double denominator = 2.0 * viennagrid::inner_prod(viennagrid::cross_prod(A-B, B-C), viennagrid::cross_prod(A-B, B-C));
      
      double alpha = viennagrid::inner_prod(B - C, B - C) * viennagrid::inner_prod(A - B, A - C);
      double beta  = viennagrid::inner_prod(A - C, A - C) * viennagrid::inner_prod(B - A, B - C);
      double gamma = viennagrid::inner_prod(A - B, A - B) * viennagrid::inner_prod(C - A, C - B);
      
      
      PointType circ_cent = A * (alpha/denominator) + B * (beta/denominator) + C * (gamma/denominator);
      
      return circ_cent;
    }


    //
    // Note: This works for rectangles only, but not for general quadrilaterals
    //
    template <typename CellType>
    typename viennagrid::result_of::point<typename CellType::config_type>::type
    circumcenter(CellType const & cell, viennagrid::quadrilateral_tag, viennagrid::dimension_tag<3>)
    {
      typedef typename CellType::config_type             Config;
      typedef typename CellType::tag             ElementTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type      VertexOnCellIterator;

      PointType p0(0.0, 0.0, 0.0);
      
      VertexOnCellRange vertices = viennagrid::ncells<0>(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
      {
        p0 += vocit->point();
      }
      
      p0 /= viennagrid::topology::bndcells<ElementTag, 0>::num;
      
      return p0;
    }


    //
    // Calculation of circumcenter taken from Wikipedia (better reference required...)
    //
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    circumcenter(ElementType const & cell, viennagrid::tetrahedron_tag, viennagrid::dimension_tag<3>)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_range<ElementType, 0>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type            VertexOnCellIterator;

      VertexOnCellRange vertices = viennagrid::ncells<0>(cell);
      VertexOnCellIterator vocit = vertices.begin();
      
      PointType const & O = vocit->point(); ++vocit;
      PointType const & A = vocit->point() - O; ++vocit;
      PointType const & B = vocit->point() - O; ++vocit;
      PointType const & C = vocit->point() - O;
      
      PointType circ_cent = (cross_prod(A, B) * viennagrid::inner_prod(C, C)
                             + cross_prod(C, A) * viennagrid::inner_prod(B, B)
                             + cross_prod(B, C) * viennagrid::inner_prod(A, A)
                             ) / (viennagrid::inner_prod(A, viennagrid::cross_prod(B, C)) * 2.0);
      if (viennagrid::inner_prod(A, viennagrid::cross_prod(B, C)) == 0)
      {
        std::cout << "Singularity in circum center calculation!" << std::endl;
        std::cout << "A: " << A << std::endl;
        std::cout << "B: " << B << std::endl;
        std::cout << "C: " << C << std::endl;
        std::cout << "B x C: " << viennagrid::cross_prod(B, C) << std::endl;
        exit(0);
      }
      return circ_cent + O;
    }

    //
    // Note: This works for rectangles only, but not for general quadrilaterals
    //
    template <typename CellType>
    typename viennagrid::result_of::point<typename CellType::config_type>::type
    circumcenter(CellType const & cell, viennagrid::hexahedron_tag, viennagrid::dimension_tag<3>)
    {
      typedef typename CellType::config_type             Config;
      typedef typename Config::cell_tag                  CellTag;
      typedef typename viennagrid::result_of::point<Config>::type                            PointType;
      typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
      typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type      VertexOnCellIterator;

      PointType p0(0.0, 0.0);
      
      VertexOnCellRange vertices = viennagrid::ncells<0>(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
      {
        p0 += vocit->point();
      }
      
      p0 /= viennagrid::topology::bndcells<CellTag, 0>::num;
      
      return p0;
    }

  } //namespace detail


  template <typename CellType>
  typename viennagrid::result_of::point<typename CellType::config_type>::type
  circumcenter(CellType const & cell)
  {
    return detail::circumcenter(cell,
                        typename CellType::tag(),
                        viennagrid::dimension_tag<CellType::config_type::coordinate_system_tag::dim>());
  }
    
} //namespace viennagrid
#endif
