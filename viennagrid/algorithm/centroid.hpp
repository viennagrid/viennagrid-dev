#ifndef VIENNAGRID_ALGORITHM_CENTROID_HPP
#define VIENNAGRID_ALGORITHM_CENTROID_HPP

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

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/forwards.h"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/volume.hpp"    //for domain/segment centroid

/** @file centroid.hpp
    @brief Computes the centroid (center of mass) for different cell types
*/


namespace viennagrid
{
  namespace detail
  {
    //
    // Calculation of centroid
    //
    /** @brief Implementation of the calculation of a centroid for a triangle */
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::triangle_tag)
    {
      typedef typename ElementType::config_type             Config;
      typedef typename ElementType::tag             ElementTag;
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
      
      p0 /= viennagrid::topology::bndcells<ElementTag, 0>::num;
      
      return p0;
    }

    //tetrahedron can reuse the algorithm defined for a triangle
    /** @brief Implementation of the calculation of a centroid for a tetrahedron */
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //
    // Note: This works for rectangles only, but not for general quadrilateral
    //
    /** @brief Implementation of the calculation of a centroid for a quadrilateral */
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //
    // Note: This works for cuboids only, but not for general hexahedra
    //
    /** @brief Implementation of the calculation of a centroid for a hexahedron */
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::hexahedron_tag)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }


    //a line can reuse the algorithm defined for a triangle
    /** @brief Implementation of the calculation of a centroid for a line (1-simplex) */
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::simplex_tag<1>)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    /** @brief Implementation of the calculation of a centroid for a line (1-hypercube) */
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::hypercube_tag<1>)
    {
      return centroid(cell, viennagrid::triangle_tag());
    }

    //a point is degenerate and returns its location
    /** @brief Implementation of the calculation of a centroid for a point */
    template <typename ElementType>
    typename viennagrid::result_of::point<typename ElementType::config_type>::type
    centroid(ElementType const & cell, viennagrid::point_tag)
    {
      return cell.point();
    }




    template <typename DomainSegmentType>
    typename viennagrid::result_of::point<typename DomainSegmentType::config_type>::type
    centroid_domseg(DomainSegmentType const & domseg)
    {
      typedef typename DomainSegmentType::config_type                                      ConfigType;
      typedef typename ConfigType::cell_tag                                                CellTag;
      
      typedef typename viennagrid::result_of::point<ConfigType>::type                      PointType;
      typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type                   VertexType;
      typedef typename viennagrid::result_of::ncell<ConfigType, 1>::type                   EdgeType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainSegmentType,
                                                                CellTag::dim>::type  CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                         CellIterator;
      
      PointType result = 0;
      double volume = 0;
      
      CellRange cells = viennagrid::ncells(domseg);
      for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        double vol_cell = viennagrid::volume(*cit);
        result += vol_cell * centroid(*cit);
        volume += vol_cell;
      }
      
      return result / volume;
    }
    
  } //namespace detail


  /** @brief The public interface function for the computation of a centroid
   * 
   * @param cell    The n-cell for which the centroid should be computed
   */
  template <typename CellType>
  typename viennagrid::result_of::point<typename CellType::config_type>::type
  centroid(CellType const & cell)
  {
    return detail::centroid(cell, typename CellType::tag());
  }

  /** @brief The public interface function for the computation of the centroid of a domain
   * 
   * @param domain    The domain object
   */
  template <typename ConfigType>
  typename viennagrid::result_of::point<ConfigType>::type
  centroid(domain_t<ConfigType> const & domain)
  {
    return detail::centroid_domseg(domain);
  }

  /** @brief The public interface function for the computation of the centroid of a segment
   * 
   * @param segment    The segment object
   */
  template <typename ConfigType>
  typename viennagrid::result_of::point<ConfigType>::type
  centroid(segment_t<ConfigType> const & segment)
  {
    return detail::centroid_domseg(segment);
  }

} //namespace viennagrid
#endif
