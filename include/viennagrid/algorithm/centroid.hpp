#ifndef VIENNAGRID_ALGORITHM_CENTROID_HPP
#define VIENNAGRID_ALGORITHM_CENTROID_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
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

#include "viennagrid/core.hpp"
// #include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/volume.hpp"    //for mesh/region centroid
#include "viennagrid/point_accessor.hpp"

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
    template <typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point< PointAccessorT, ElementT >::type
    centroid(PointAccessorT const accessor, ElementT const & cell)
    {
      typedef typename viennagrid::result_of::point< PointAccessorT, ElementT >::type PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type    CoordType;

      typedef typename viennagrid::result_of::const_vertex_range<ElementT>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type            VertexOnCellIterator;

      PointType p0( viennagrid::geometric_dimension(cell) );

      VertexOnCellRange vertices(cell);
      for (VertexOnCellIterator vocit = vertices.begin();
           vocit != vertices.end();
           ++vocit)
        p0 += accessor(*vocit);

      p0 /= static_cast<CoordType>(vertices.size());

      return p0;
    }

//     //tetrahedron can reuse the algorithm defined for a triangle
//     /** @brief Implementation of the calculation of a centroid for a tetrahedron */
//     template <typename PointAccessorT, typename ElementT>
//     typename PointAccessorT::value_type
//     centroid(PointAccessorT const accessor, ElementT const & cell, viennagrid::tetrahedron_tag)
//     {
//       return centroid(accessor, cell, viennagrid::triangle_tag());
//     }
//
//     //
//     // Note: This works for rectangles only, but not for general quadrilateral
//     //
//     /** @brief Implementation of the calculation of a centroid for a quadrilateral */
//     template <typename PointAccessorT, typename ElementT>
//     typename PointAccessorT::value_type
//     centroid(PointAccessorT const accessor, ElementT const & cell, viennagrid::quadrilateral_tag)
//     {
//       return centroid(accessor, cell, viennagrid::triangle_tag());
//     }
//
//     //
//     // Note: This works for cuboids only, but not for general hexahedra
//     //
//     /** @brief Implementation of the calculation of a centroid for a hexahedron */
//     template <typename PointAccessorT, typename ElementT>
//     typename PointAccessorT::value_type
//     centroid(PointAccessorT const accessor, ElementT const & cell, viennagrid::hexahedron_tag)
//     {
//       return centroid(accessor, cell, viennagrid::triangle_tag());
//     }
//
//
//     //a line can reuse the algorithm defined for a triangle
//     /** @brief Implementation of the calculation of a centroid for a line (1-simplex) */
//     template <typename PointAccessorT, typename ElementT>
//     typename PointAccessorT::value_type
//     centroid(PointAccessorT const accessor, ElementT const & cell, viennagrid::simplex_tag<1>)
//     {
//       return centroid(accessor, cell, viennagrid::triangle_tag());
//     }
//
//
//     /** @brief Implementation of the calculation of a centroid for a line (1-hypercube) */
//     template <typename PointAccessorT, typename ElementT>
//     typename PointAccessorT::value_type
//     centroid(PointAccessorT const accessor, ElementT const & cell, viennagrid::hypercube_tag<1>)
//     {
//       return centroid(accessor, cell, viennagrid::triangle_tag());
//     }
//
//
//     //a point is degenerate and returns its location
//     /** @brief Implementation of the calculation of a centroid for a point */
//     template <typename PointAccessorT, typename ElementT>
//     typename PointAccessorT::value_type
//     centroid(PointAccessorT const accessor, ElementT const & cell, viennagrid::vertex_tag)
//     {
//       return accessor(cell);
//     }



    /** @brief Implementation of the calculation of a centroid for a mesh/region */
    template<typename MeshSegmentHandleType, typename PointAccessorT>
    typename viennagrid::result_of::point< PointAccessorT, typename viennagrid::result_of::element<MeshSegmentHandleType>::type >::type
    centroid_mesh(PointAccessorT const point_accessor, MeshSegmentHandleType const & mesh_obj,
                  viennagrid_int topologic_dimension)
    {
      typedef typename viennagrid::result_of::point<MeshSegmentHandleType>::type          PointType;
      typedef typename viennagrid::result_of::const_element_range<MeshSegmentHandleType>::type         CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                   CellIterator;

      PointType result( viennagrid::geometric_dimension(mesh_obj) );
      double volume = 0;

//       for (; cell_tag != cell_end; ++cell_tag)
      {
        CellRange cells(mesh_obj, topologic_dimension);
        for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
        {
          double vol_cell = viennagrid::volume( point_accessor, *cit );
          result += vol_cell * centroid( point_accessor, *cit);
          volume += vol_cell;
        }
      }

      return result / volume;
    }

  } //namespace detail



  /** @brief The public interface function for the computation of a centroid of an element with explicit point accessor
   *
   * @param element    The element for which the centroid should be computed
   * @param accessor   The point accessor providing point information for geometric calculation
   */
  template <typename PointAccessorT, bool element_is_const>
  typename viennagrid::result_of::point< PointAccessorT, base_element<element_is_const> >::type
  centroid(PointAccessorT const accessor, base_element<element_is_const> const & element)
  {
    return detail::centroid( accessor, element );
  }

  /** @brief The public interface function for the computation of a centroid of an element
   *
   * @param element    The element for which the centroid should be computed
   */
  template <bool element_is_const>
  typename viennagrid::result_of::point< base_element<element_is_const> >::type
  centroid(base_element<element_is_const> const & element)
  {
    return centroid( root_mesh_point_accessor(), element );
  }



  /** @brief The public interface function for the computation of a centroid of a mesh with explicit point accessor.
   *
   * @tparam ElementTOrTagT       The element type/tag of the elements for which the centroid is calculcated
   * @param  mesh_obj             The mesh which centroid is to be calculated
   * @param  point_accessor       The point accessor providing point information for geometric calculation
   */
  template<typename ElementTOrTagT, typename PointAccessorT, bool mesh_is_const>
  typename viennagrid::result_of::point< base_mesh<mesh_is_const> >::type
  centroid(PointAccessorT const point_accessor, base_mesh<mesh_is_const> const & mesh_obj)
  {
    return detail::centroid_mesh<ElementTOrTagT>(point_accessor, mesh_obj);
  }

  /** @brief The public interface function for the computation of a centroid of a mesh with explicit point accessor. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  mesh_obj           The mesh which centroid is to be calculated
   * @param  point_accessor     The point accessor providing point information for geometric calculation
   */
  template<typename PointAccessorT, bool mesh_is_const>
  typename viennagrid::result_of::point< base_mesh<mesh_is_const> >::type
  centroid(PointAccessorT const point_accessor, base_mesh<mesh_is_const> const & mesh_obj)
  {
    return detail::centroid_mesh(point_accessor, mesh_obj, viennagrid::cell_dimension(mesh_obj));
  }


  /** @brief The public interface function for the computation of a centroid of a mesh.
   *
   * @tparam ElementTOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  mesh_obj          The mesh which centroid is to be calculated
   */
  template<typename ElementTOrTagT, bool mesh_is_const>
  typename viennagrid::result_of::point< base_mesh<mesh_is_const> >::type
  centroid(const_mesh_t const & mesh_obj)
  {
    return centroid<ElementTOrTagT>(point_accessor(mesh_obj), mesh_obj);
  }

  /** @brief The public interface function for the computation of a centroid of a mesh. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  mesh_obj          The mesh which centroid is to be calculated
   */
  template<bool mesh_is_const>
  typename viennagrid::result_of::point< base_mesh<mesh_is_const> >::type
  centroid(base_mesh<mesh_is_const> const & mesh_obj)
  {
    return centroid(point_accessor(mesh_obj), mesh_obj,
                    cell_tag_begin(mesh_obj), cell_tag_end(mesh_obj));
  }


  /** @brief The public interface function for the computation of a centroid of a region with explicit point accessor.
   *
   * @tparam ElementTOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  region              The region which centroid is to be calculated
   * @param  point_accessor       The point accessor providing point information for geometric calculation
   */
  template<typename ElementTOrTagT, typename PointAccessorT, bool mesh_region_is_const>
  typename viennagrid::result_of::point< base_mesh_region<mesh_region_is_const> >::type
  centroid(PointAccessorT const point_accessor, base_mesh_region<mesh_region_is_const> const & region)
  {
    return detail::centroid_mesh<ElementTOrTagT>(point_accessor, region);
  }

  /** @brief The public interface function for the computation of a centroid of a region with explicit point accessor. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  region              The region which centroid is to be calculated
   * @param  point_accessor       The point accessor providing point information for geometric calculation
   */
  template<typename PointAccessorT, bool mesh_region_is_const>
  typename viennagrid::result_of::point< base_mesh_region<mesh_region_is_const> >::type
  centroid(PointAccessorT const point_accessor, base_mesh_region<mesh_region_is_const> const & region)
  {
    return detail::centroid_mesh(point_accessor, region,
                                 cell_tag_begin(region), cell_tag_end(region));
  }


  /** @brief The public interface function for the computation of a centroid of a region.
   *
   * @tparam ElementTOrTagT    The element type/tag of the elements for which the centroid is calculcated
   * @param  region              The region which centroid is to be calculated
   */
  template<typename ElementTOrTagT, bool mesh_region_is_const>
  typename viennagrid::result_of::point< base_mesh_region<mesh_region_is_const> >::type
  centroid(base_mesh_region<mesh_region_is_const> const & region)
  {
    return centroid<ElementTOrTagT>(default_point_accessor(region), region);
  }

  /** @brief The public interface function for the computation of a centroid of a region. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  region               The region which centroid is to be calculated
   */
  template<bool mesh_region_is_const>
  typename viennagrid::result_of::point< base_mesh_region<mesh_region_is_const> >::type
  centroid(base_mesh_region<mesh_region_is_const> const & region)
  {
    return centroid(default_point_accessor(region), region,
                    cell_tag_begin(region), cell_tag_end(region));
  }
} //namespace viennagrid
#endif
