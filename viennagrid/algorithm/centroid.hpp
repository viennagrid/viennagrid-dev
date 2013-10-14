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
#include "viennagrid/algorithm/volume.hpp"    //for mesh/segment centroid
#include "viennagrid/mesh/accessor.hpp"

/** @file viennagrid/algorithm/centroid.hpp
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
    typename PointAccessorType::value_type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::triangle_tag)
    {
      //typedef typename ElementType::config_type             Config;
//       typedef typename ElementType::tag             ElementTag;
      typedef typename PointAccessorType::value_type PointType;
//       typedef typename viennagrid::result_of::element<MeshType, vertex_tag>::type                         VertexType;
//       typedef typename viennagrid::result_of::element<MeshType, line_tag>::type                         EdgeType;

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
    typename PointAccessorType::value_type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::tetrahedron_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }

    //
    // Note: This works for rectangles only, but not for general quadrilateral
    //
    /** @brief Implementation of the calculation of a centroid for a quadrilateral */
    template <typename PointAccessorType, typename ElementType>
    typename PointAccessorType::value_type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::quadrilateral_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }

    //
    // Note: This works for cuboids only, but not for general hexahedra
    //
    /** @brief Implementation of the calculation of a centroid for a hexahedron */
    template <typename PointAccessorType, typename ElementType>
    typename PointAccessorType::value_type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::hexahedron_tag)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    //a line can reuse the algorithm defined for a triangle
    /** @brief Implementation of the calculation of a centroid for a line (1-simplex) */
    template <typename PointAccessorType, typename ElementType>
    typename PointAccessorType::value_type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::simplex_tag<1>)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    /** @brief Implementation of the calculation of a centroid for a line (1-hypercube) */
    template <typename PointAccessorType, typename ElementType>
    typename PointAccessorType::value_type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::hypercube_tag<1>)
    {
      return centroid(accessor, cell, viennagrid::triangle_tag());
    }


    //a point is degenerate and returns its location
    /** @brief Implementation of the calculation of a centroid for a point */
    template <typename PointAccessorType, typename ElementType>
    typename PointAccessorType::value_type
    centroid(PointAccessorType const accessor, ElementType const & cell, viennagrid::vertex_tag)
    {
      return accessor(cell);
    }



    /** @brief Implementation of the calculation of a centroid for a mesh/segment */
    template <typename ElementTypeOrTag, typename MeshSegmentHandleType, typename PointAccessorType>
    typename viennagrid::result_of::point<MeshSegmentHandleType>::type
    centroid_mesh(MeshSegmentHandleType const & mesh_obj, PointAccessorType const point_accessor)
    {
      //typedef typename MeshSegmentHandleType::config_type                                      ConfigType;
      //typedef typename ElementType::tag                                                CellTag;
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTag>::type ElementTag;

      typedef typename viennagrid::result_of::point<MeshSegmentHandleType>::type                      PointType;

      typedef typename viennagrid::result_of::const_element_range<MeshSegmentHandleType,
                                                                ElementTag>::type  CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                         CellIterator;

      PointType result = 0;
      double volume = 0;

      CellRange cells = viennagrid::elements<ElementTag>(mesh_obj);
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
  typename PointAccessorType::value_type
  centroid(PointAccessorType const accessor, ElementType const & element)
  {
    return detail::centroid( accessor, element, typename ElementType::tag());
  }

  /** @brief The public interface function for the computation of a centroid of an element
   *
   * @param element    The element for which the centroid should be computed
   */
  template <typename ElementTag, typename WrappedConfigType>
  typename viennagrid::result_of::point< viennagrid::element<ElementTag,WrappedConfigType> >::type
  centroid(viennagrid::element<ElementTag,WrappedConfigType> const & element)
  {
    return detail::centroid( default_point_accessor(element), element, ElementTag());
  }



  /** @brief The public interface function for the computation of a centroid of a mesh with explicit point accessor.
   *
   * @tparam ElementTypeOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  mesh               The mesh which centroid is to be calculated
   * @param  accessor             The point accessor providing point information for geometric calculation
   */
  template<typename ElementTypeOrTagT, typename WrappedConfigType, typename PointAccessorType>
  typename viennagrid::result_of::point< mesh<WrappedConfigType> >::type
  centroid(mesh<WrappedConfigType> const & mesh_obj, PointAccessorType const point_accessor)
  {
    return detail::centroid_mesh<ElementTypeOrTagT>(mesh_obj, point_accessor);
  }

  /** @brief The public interface function for the computation of a centroid of a mesh with explicit point accessor. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  mesh               The mesh which centroid is to be calculated
   * @param  accessor             The point accessor providing point information for geometric calculation
   */
  template<typename WrappedConfigType, typename PointAccessorType>
  typename viennagrid::result_of::point< mesh<WrappedConfigType> >::type
  centroid(mesh<WrappedConfigType> const & mesh_obj, PointAccessorType const point_accessor)
  {
    typedef typename viennagrid::result_of::cell_tag< mesh<WrappedConfigType> >::type CellTag;
    return detail::centroid_mesh<CellTag>(mesh_obj, point_accessor);
  }


  /** @brief The public interface function for the computation of a centroid of a mesh.
   *
   * @tparam ElementTypeOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  mesh               The mesh which centroid is to be calculated
   */
  template<typename ElementTypeOrTagT, typename WrappedConfigType>
  typename viennagrid::result_of::point< mesh<WrappedConfigType> >::type
  centroid(mesh<WrappedConfigType> const & mesh_obj)
  {
    return centroid<ElementTypeOrTagT>(mesh_obj, default_point_accessor(mesh_obj));
  }

  /** @brief The public interface function for the computation of a centroid of a mesh. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  mesh               The mesh which centroid is to be calculated
   */
  template<typename WrappedConfigType>
  typename viennagrid::result_of::point< mesh<WrappedConfigType> >::type
  centroid(mesh<WrappedConfigType> const & mesh_obj)
  {
    typedef typename viennagrid::result_of::cell_tag< mesh<WrappedConfigType> >::type CellTag;
    return centroid<CellTag>(mesh_obj, default_point_accessor(mesh_obj));
  }





  /** @brief The public interface function for the computation of a centroid of a segment with explicit point accessor.
   *
   * @tparam ElementTypeOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  segment              The segment which centroid is to be calculated
   * @param  accessor             The point accessor providing point information for geometric calculation
   */
  template<typename ElementTypeOrTagT, typename SegmentationT, typename PointAccessorType>
  typename viennagrid::result_of::point< segment_handle_t<SegmentationT> >::type
  centroid(segment_handle_t<SegmentationT> const & segment, PointAccessorType const point_accessor)
  {
    return detail::centroid_mesh<ElementTypeOrTagT>(segment, point_accessor);
  }

  /** @brief The public interface function for the computation of a centroid of a segment with explicit point accessor. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  segment              The segment which centroid is to be calculated
   * @param  accessor             The point accessor providing point information for geometric calculation
   */
  template<typename SegmentationT, typename PointAccessorType>
  typename viennagrid::result_of::point< segment_handle_t<SegmentationT> >::type
  centroid(segment_handle_t<SegmentationT> const & segment, PointAccessorType const point_accessor)
  {
    typedef typename viennagrid::result_of::cell_tag< segment_handle_t<SegmentationT> >::type CellTag;
    return detail::centroid_mesh<CellTag>(segment, point_accessor);
  }


  /** @brief The public interface function for the computation of a centroid of a segment.
   *
   * @tparam ElementTypeOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  segment              The segment which centroid is to be calculated
   */
  template<typename ElementTypeOrTagT, typename SegmentationT>
  typename viennagrid::result_of::point< segment_handle_t<SegmentationT> >::type
  centroid(segment_handle_t<SegmentationT> const & segment)
  {
    return centroid<ElementTypeOrTagT>(segment, default_point_accessor(segment));
  }

  /** @brief The public interface function for the computation of a centroid of a segment. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  segment               The segment which centroid is to be calculated
   */
  template<typename SegmentationT>
  typename viennagrid::result_of::point< segment_handle_t<SegmentationT> >::type
  centroid(segment_handle_t<SegmentationT> const & segment)
  {
    typedef typename viennagrid::result_of::cell_tag< segment_handle_t<SegmentationT> >::type CellTag;
    return centroid<CellTag>(segment, default_point_accessor(segment));
  }


} //namespace viennagrid
#endif
