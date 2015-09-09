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

#include "viennagrid/viennagrid.hpp"
#include "viennagrid/algorithm/volume.hpp"    //for mesh/region centroid

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
        p0 += accessor.get(*vocit);

      p0 /= static_cast<CoordType>(vertices.size());

      return p0;
    }



    /** @brief Implementation of the calculation of a centroid for a mesh/region */
    template<typename MeshSegmentHandleType, typename PointAccessorT>
    typename viennagrid::result_of::point< PointAccessorT, typename viennagrid::result_of::element<MeshSegmentHandleType>::type >::type
    centroid_mesh(PointAccessorT const point_accessor, MeshSegmentHandleType const & mesh_obj,
                  viennagrid_int topologic_dimension)
    {
      typedef typename viennagrid::result_of::point<MeshSegmentHandleType>::type                    PointType;
      typedef typename viennagrid::result_of::const_element_range<MeshSegmentHandleType>::type      CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                             CellIterator;

      PointType result( viennagrid::geometric_dimension(mesh_obj) );
      double volume = 0;

      CellRange cells(mesh_obj, topologic_dimension);
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
  template <typename PointAccessorT, bool element_is_const>
  typename viennagrid::result_of::point< PointAccessorT, base_element<element_is_const> >::type centroid(PointAccessorT const accessor,
                                                                                                         base_element<element_is_const> const & element)
  {
    return detail::centroid( accessor, element );
  }

  /** @brief The public interface function for the computation of a centroid of an element
   *
   * @param element    The element for which the centroid should be computed
   */
  template <bool element_is_const>
  typename viennagrid::result_of::point< base_element<element_is_const> >::type centroid(base_element<element_is_const> const & element)
  {
    return centroid( root_mesh_point_accessor(), element );
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

  /** @brief The public interface function for the computation of a centroid of a mesh. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  mesh_obj          The mesh which centroid is to be calculated
   */
  template<bool mesh_is_const>
  typename viennagrid::result_of::point< base_mesh<mesh_is_const> >::type centroid(base_mesh<mesh_is_const> const & mesh_obj)
  {
    return detail::centroid_mesh(mesh_point_accessor(mesh_obj), mesh_obj, viennagrid::cell_dimension(mesh_obj));
  }

  /** @brief The public interface function for the computation of a centroid of a region with explicit point accessor. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  region              The region which centroid is to be calculated
   * @param  point_accessor       The point accessor providing point information for geometric calculation
   */
  template<typename PointAccessorT, bool mesh_region_is_const>
  typename viennagrid::result_of::point< base_region<mesh_region_is_const> >::type
  centroid(PointAccessorT const point_accessor, base_region<mesh_region_is_const> const & region)
  {
    return detail::centroid_mesh(point_accessor, region, viennagrid::cell_dimension(region));
  }

  /** @brief The public interface function for the computation of a centroid of a region. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  region               The region which centroid is to be calculated
   */
  template<bool mesh_region_is_const>
  typename viennagrid::result_of::point< base_region<mesh_region_is_const> >::type
  centroid(base_region<mesh_region_is_const> const & region)
  {
    return detail::centroid_mesh(mesh_point_accessor(region), region, viennagrid::cell_dimension(region));
  }
} //namespace viennagrid
#endif
