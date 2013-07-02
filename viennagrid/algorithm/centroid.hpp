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

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/volume.hpp"    //for domain/segment centroid
#include "viennagrid/domain/accessor.hpp"

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
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point_type<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::triangle_tag)
    {
      //typedef typename ElementType::config_type             Config;
//       typedef typename ElementType::tag             ElementTag;
      typedef typename viennagrid::result_of::point_type<PointAccessorType>::type PointType;
//       typedef typename viennagrid::result_of::element<DomainType, vertex_tag>::type                         VertexType;
//       typedef typename viennagrid::result_of::element<DomainType, line_tag>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_element_range<ElementType, vertex_tag>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type            VertexOnCellIterator;
      
      PointType p0(0.0, 0.0);
      
      VertexOnCellRange vertices = viennagrid::elements<vertex_tag>(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
      {
        //p0 += vocit->point();
        p0 += accessor(*vocit);
      }
      
      p0 /= vertices.size();
      
      return p0;
    }

    //tetrahedron can reuse the algorithm defined for a triangle
    /** @brief Implementation of the calculation of a centroid for a tetrahedron */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point_type<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }

    //
    // Note: This works for rectangles only, but not for general quadrilateral
    //
    /** @brief Implementation of the calculation of a centroid for a quadrilateral */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point_type<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }

    //
    // Note: This works for cuboids only, but not for general hexahedra
    //
    /** @brief Implementation of the calculation of a centroid for a hexahedron */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point_type<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::hexahedron_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    //a line can reuse the algorithm defined for a triangle
    /** @brief Implementation of the calculation of a centroid for a line (1-simplex) */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point_type<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::simplex_tag<1>)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    /** @brief Implementation of the calculation of a centroid for a line (1-hypercube) */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point_type<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::hypercube_tag<1>)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    //a point is degenerate and returns its location
    /** @brief Implementation of the calculation of a centroid for a point */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point_type<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::vertex_tag)
    {
      return accessor(cell);
    }




    template <typename ElementTypeOrTag, typename DomainType>
    typename viennagrid::result_of::point_type<DomainType>::type
    centroid_domain(DomainType const & domain)
    {
      //typedef typename DomainSegmentType::config_type                                      ConfigType;
      //typedef typename ElementType::tag                                                CellTag;
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTag>::type ElementTag;
      
      typedef typename viennagrid::result_of::point_type<DomainType>::type                      PointType;
      typedef typename viennagrid::result_of::element<DomainType, vertex_tag>::type                         VertexType;
      typedef typename viennagrid::result_of::element<DomainType, line_tag>::type                         EdgeType;
      
      typedef typename viennagrid::result_of::const_element_range<DomainType,
                                                                ElementTag>::type  CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                         CellIterator;
      
      PointType result = 0;
      double volume = 0;
      
      CellRange cells = viennagrid::elements<ElementTag>(domain);
      for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        double vol_cell = viennagrid::volume(domain, *cit);
        result += vol_cell * centroid( accessor::default_point_accessor(domain), *cit);
        volume += vol_cell;
      }
      
      return result / volume;
    }
    
  } //namespace detail


  /** @brief The public interface function for the computation of a centroid
   * 
   * @param cell    The n-cell for which the centroid should be computed
   */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point_type<PointAccessorType>::type
  centroid(PointAccessorType const accessor, ElementType const & cell)
  {
    return detail::centroid( accessor, cell, typename ElementType::tag());
  }
  
    template <typename ElementTag, typename WrappedConfigType>
    typename viennagrid::result_of::point_type< element_t<ElementTag,WrappedConfigType> >::type
  centroid(element_t<ElementTag,WrappedConfigType> const & cell)
  {
    return detail::centroid( accessor::default_point_accessor(cell), cell, ElementTag());
  }
  
  template<typename ElementTypeOrTag, typename WrappedConfigType>
  typename viennagrid::result_of::coord_type<
        domain_t<WrappedConfigType>
  >::type
  centroid(domain_t<WrappedConfigType> const & domain)
  {
      return detail::centroid_domain<ElementTypeOrTag>(domain);
  }


} //namespace viennagrid
#endif
