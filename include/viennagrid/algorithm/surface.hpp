#ifndef VIENNAGRID_ALGORITHM_SURFACE_HPP
#define VIENNAGRID_ALGORITHM_SURFACE_HPP

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

#include "viennagrid/core/forwards.hpp"
#include "viennagrid/algorithm/volume.hpp"

/** @file viennagrid/algorithm/surface.hpp
    @brief Computes the surface of different cell types as well as meshs and regions
*/


namespace viennagrid
{
  /** @brief Returns the surface of an element */
  template <bool element_is_const>
  viennagrid_numeric surface(base_element<element_is_const> const & cell)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_element_surface( cell.internal_mesh(), cell.id().internal(), &result) );
    return result;
  }

  /** @brief Returns the surface of a whole mesh */
  template<bool mesh_is_const>
  viennagrid_numeric surface(base_mesh<mesh_is_const> const & mesh_obj)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_mesh_surface( mesh_obj.internal(), &result) );
    return result;
  }




  /** @brief Returns a handle to the boundary element with the smallest volume */
  template<typename MeshT, typename ElementT>
  typename viennagrid::result_of::element<MeshT>::type
  smallest_boundary_volume( MeshT & mesh_obj, viennagrid_int boundary_topologic_dimension, ElementT & element )
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type NumericType;
    typedef typename viennagrid::result_of::element<ElementT>::type           BoundaryElementType;
    typedef typename viennagrid::result_of::element_range<ElementT>::type    BoundaryElementRangeType;
    typedef typename viennagrid::result_of::iterator<BoundaryElementRangeType>::type           BoundaryElementRangeIterator;

    BoundaryElementRangeType boundary_elements( element, boundary_topologic_dimension );
    BoundaryElementRangeIterator it = boundary_elements.begin();

    BoundaryElementType best = *it;
    NumericType best_volume = viennagrid::volume( mesh_obj, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      NumericType volume = viennagrid::volume( mesh_obj, *it );
      if (volume < best_volume)
      {
        best = *it;
        best_volume = volume;
      }
    }

    return best;
  }

  /** @brief Returns a const handle to the boundary element with the smallest volume */
  template<typename MeshT, typename ElementT>
  typename viennagrid::result_of::const_element<MeshT>::type
  smallest_boundary_volume( MeshT const & mesh_obj, viennagrid_int boundary_topologic_dimension, ElementT const & element )
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type NumericType;
    typedef typename viennagrid::result_of::const_element<ElementT>::type          BoundaryElementType;
    typedef typename viennagrid::result_of::const_element_range<ElementT>::type   BoundaryElementRangeType;
    typedef typename viennagrid::result_of::iterator<BoundaryElementRangeType>::type                BoundaryElementRangeIterator;

    BoundaryElementRangeType boundary_elements( element, boundary_topologic_dimension );
    BoundaryElementRangeIterator it = boundary_elements.begin();

    BoundaryElementType best_handle = *it;
    NumericType best_volume = viennagrid::volume( mesh_obj, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      NumericType volume = viennagrid::volume( mesh_obj, *it );
      if (volume < best_volume)
      {
        best_handle = *it;
        best_volume = volume;
      }
    }

    return best_handle;
  }

  /** @brief Returns a handle to the boundary element with the largest volume */
  template<typename MeshT, typename ElementT>
  typename viennagrid::result_of::element<MeshT>::type largest_boundary_volume( MeshT & mesh_obj, viennagrid_int boundary_topologic_dimension, ElementT & element )
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type NumericType;
    typedef typename viennagrid::result_of::element<ElementT>::type           BoundaryElementType;
    typedef typename viennagrid::result_of::element_range<ElementT>::type    BoundaryElementRangeType;
    typedef typename viennagrid::result_of::iterator<BoundaryElementRangeType>::type           BoundaryElementRangeIterator;

    BoundaryElementRangeType boundary_elements( element, boundary_topologic_dimension );
    BoundaryElementRangeIterator it = boundary_elements.begin();

    BoundaryElementType best_handle = *it;
    NumericType best_volume = viennagrid::volume( mesh_obj, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      NumericType volume = viennagrid::volume( mesh_obj, *it );
      if (volume > best_volume)
      {
        best_handle = *it;
        best_volume = volume;
      }
    }

    return best_handle;
  }

  /** @brief Returns a const handle to the boundary element with the largest volume */
  template<typename MeshT, typename ElementT>
  typename viennagrid::result_of::const_element<MeshT>::type largest_boundary_volume( MeshT const & mesh_obj, viennagrid_int boundary_topologic_dimension, ElementT const & element )
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type NumericType;
    typedef typename viennagrid::result_of::const_element<ElementT>::type            BoundaryElementType;
    typedef typename viennagrid::result_of::const_element_range<ElementT>::type     BoundaryElementRangeType;
    typedef typename viennagrid::result_of::iterator<BoundaryElementRangeType>::type                  BoundaryElementRangeIterator;

    BoundaryElementRangeType boundary_elements( element, boundary_topologic_dimension );
    BoundaryElementRangeIterator it = boundary_elements.begin();

    BoundaryElementType best_handle = *it;
    NumericType best_volume = viennagrid::volume( mesh_obj, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      NumericType volume = viennagrid::volume( mesh_obj, *it );
      if (volume > best_volume)
      {
        best_handle = *it;
        best_volume = volume;
      }
    }

    return best_handle;
  }


} //namespace viennagrid
#endif
