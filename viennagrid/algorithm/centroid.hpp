#ifndef VIENNAGRID_ALGORITHM_CENTROID_HPP
#define VIENNAGRID_ALGORITHM_CENTROID_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

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
    typename viennagrid::result_of::point<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::triangle_tag)
    {
      //typedef typename ElementType::config_type             Config;
//       typedef typename ElementType::tag             ElementTag;
      typedef typename viennagrid::result_of::point<PointAccessorType>::type PointType;
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
    typename viennagrid::result_of::point<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }

    //
    // Note: This works for rectangles only, but not for general quadrilateral
    //
    /** @brief Implementation of the calculation of a centroid for a quadrilateral */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }

    //
    // Note: This works for cuboids only, but not for general hexahedra
    //
    /** @brief Implementation of the calculation of a centroid for a hexahedron */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::hexahedron_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    //a line can reuse the algorithm defined for a triangle
    /** @brief Implementation of the calculation of a centroid for a line (1-simplex) */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::simplex_tag<1>)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    /** @brief Implementation of the calculation of a centroid for a line (1-hypercube) */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::hypercube_tag<1>)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    //a point is degenerate and returns its location
    /** @brief Implementation of the calculation of a centroid for a point */
    template <typename PointAccessorType, typename ElementType>
    typename viennagrid::result_of::point<PointAccessorType>::type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::vertex_tag)
    {
      return accessor(cell);
    }



    /** @brief Implementation of the calculation of a centroid for a domain/segment */
    template <typename ElementTypeOrTag, typename DomainSegmentType, typename PointAccessorType>
    typename viennagrid::result_of::point<DomainSegmentType>::type
    centroid_domain(DomainSegmentType const & domain, PointAccessorType const point_accessor)
    {
      //typedef typename DomainSegmentType::config_type                                      ConfigType;
      //typedef typename ElementType::tag                                                CellTag;
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTag>::type ElementTag;

      typedef typename viennagrid::result_of::point<DomainSegmentType>::type                      PointType;
      typedef typename viennagrid::result_of::element<DomainSegmentType, vertex_tag>::type                         VertexType;
      typedef typename viennagrid::result_of::element<DomainSegmentType, line_tag>::type                         EdgeType;

      typedef typename viennagrid::result_of::const_element_range<DomainSegmentType,
                                                                ElementTag>::type  CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                         CellIterator;

      PointType result = 0;
      double volume = 0;

      CellRange cells = viennagrid::elements<ElementTag>(domain);
      for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        double vol_cell = viennagrid::volume( point_accessor, *cit );
        result += vol_cell * centroid( point_accessor, *cit);
        volume += vol_cell;
      }

      return result / volume;
    }

  } //namespace detail



  /** @brief The public interface function for the computation of a centroid of an element with explicit point accessor
   *
   * @param element    The element for which the centroid should be computed
   * @param accessor   The point accessor providing point information for geometric calculation
   */
  template <typename PointAccessorType, typename ElementType>
  typename viennagrid::result_of::point<PointAccessorType>::type
  centroid(PointAccessorType const accessor, ElementType const & element)
  {
    return detail::centroid( accessor, element, typename ElementType::tag());
  }

  /** @brief The public interface function for the computation of a centroid of an element
   *
   * @param element    The element for which the centroid should be computed
   */
  template <typename ElementTag, typename WrappedConfigType>
  typename viennagrid::result_of::point< element_t<ElementTag,WrappedConfigType> >::type
  centroid(element_t<ElementTag,WrappedConfigType> const & element)
  {
    return detail::centroid( default_point_accessor(element), element, ElementTag());
  }
  
  
  
  /** @brief The public interface function for the computation of a centroid of a domain with explicit point accessor.
   *
   * @tparam ElementTypeOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  domain               The domain which centroid is to be calculated
   * @param  accessor             The point accessor providing point information for geometric calculation
   */
  template<typename ElementTypeOrTagT, typename WrappedConfigType, typename PointAccessorType>
  typename viennagrid::result_of::point< domain_t<WrappedConfigType> >::type
  centroid(domain_t<WrappedConfigType> const & domain, PointAccessorType const point_accessor)
  {
    return detail::centroid_domain<ElementTypeOrTagT>(domain, point_accessor);
  }
  
  /** @brief The public interface function for the computation of a centroid of a domain with explicit point accessor. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  domain               The domain which centroid is to be calculated
   * @param  accessor             The point accessor providing point information for geometric calculation
   */
  template<typename WrappedConfigType, typename PointAccessorType>
  typename viennagrid::result_of::point< domain_t<WrappedConfigType> >::type
  centroid(domain_t<WrappedConfigType> const & domain, PointAccessorType const point_accessor)
  {
    typedef typename viennagrid::result_of::cell_tag< domain_t<WrappedConfigType> >::type CellTag;
    return detail::centroid_domain<CellTag>(domain, point_accessor);
  }

  
  /** @brief The public interface function for the computation of a centroid of a domain.
   *
   * @tparam ElementTypeOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  domain               The domain which centroid is to be calculated
   */
  template<typename ElementTypeOrTagT, typename WrappedConfigType>
  typename viennagrid::result_of::point< domain_t<WrappedConfigType> >::type
  centroid(domain_t<WrappedConfigType> const & domain)
  {
    return centroid<ElementTypeOrTagT>(domain, default_point_accessor(domain));
  }
  
  /** @brief The public interface function for the computation of a centroid of a domain. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  domain               The domain which centroid is to be calculated
   */
  template<typename WrappedConfigType>
  typename viennagrid::result_of::point< domain_t<WrappedConfigType> >::type
  centroid(domain_t<WrappedConfigType> const & domain)
  {
    typedef typename viennagrid::result_of::cell_tag< domain_t<WrappedConfigType> >::type CellTag;
    return centroid<CellTag>(domain, default_point_accessor(domain));
  }
  

  
  
  
  /** @brief The public interface function for the computation of a centroid of a segment with explicit point accessor.
   *
   * @tparam ElementTypeOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  segment              The segment which centroid is to be calculated
   * @param  accessor             The point accessor providing point information for geometric calculation
   */
  template<typename ElementTypeOrTagT, typename SegmentationT, typename PointAccessorType>
  typename viennagrid::result_of::point< segment_t<SegmentationT> >::type
  centroid(segment_t<SegmentationT> const & segment, PointAccessorType const point_accessor)
  {
    return detail::centroid_domain<ElementTypeOrTagT>(segment, point_accessor);
  }
  
  /** @brief The public interface function for the computation of a centroid of a segment with explicit point accessor. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  segment              The segment which centroid is to be calculated
   * @param  accessor             The point accessor providing point information for geometric calculation
   */
  template<typename SegmentationT, typename PointAccessorType>
  typename viennagrid::result_of::point< segment_t<SegmentationT> >::type
  centroid(segment_t<SegmentationT> const & segment, PointAccessorType const point_accessor)
  {
    typedef typename viennagrid::result_of::cell_tag< segment_t<SegmentationT> >::type CellTag;
    return detail::centroid_domain<CellTag>(segment, point_accessor);
  }

  
  /** @brief The public interface function for the computation of a centroid of a segment.
   *
   * @tparam ElementTypeOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  segment              The segment which centroid is to be calculated
   */
  template<typename ElementTypeOrTagT, typename SegmentationT>
  typename viennagrid::result_of::point< segment_t<SegmentationT> >::type
  centroid(segment_t<SegmentationT> const & segment)
  {
    return centroid<ElementTypeOrTagT>(segment, default_point_accessor(segment));
  }
  
  /** @brief The public interface function for the computation of a centroid of a segment. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  segment               The segment which centroid is to be calculated
   */
  template<typename SegmentationT>
  typename viennagrid::result_of::point< segment_t<SegmentationT> >::type
  centroid(segment_t<SegmentationT> const & segment)
  {
    typedef typename viennagrid::result_of::cell_tag< segment_t<SegmentationT> >::type CellTag;
    return centroid<CellTag>(segment, default_point_accessor(segment));
  }


} //namespace viennagrid
#endif
