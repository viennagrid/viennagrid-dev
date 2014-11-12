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

#include "viennagrid/forwards.hpp"
#include "viennagrid/algorithm/volume.hpp"
// #include "viennagrid/algorithm/boundary.hpp"

/** @file viennagrid/algorithm/surface.hpp
    @brief Computes the surface of different cell types as well as meshs and segments
*/


namespace viennagrid
{
  namespace detail
  {
    /** @brief Implementation for the calculation of the surface of a mesh or segment */
    template <typename ElementTypeOrTag, typename MeshT>
    typename viennagrid::result_of::coord< MeshT >::type
    surface_meshsegment(MeshT const & mesh_obj)
    {
      typedef typename viennagrid::result_of::const_element_range<MeshT, ElementTypeOrTag>::type  ElementRange;
      typedef typename viennagrid::result_of::iterator<ElementRange>::type                        ElementIterator;

      typename viennagrid::result_of::coord<MeshT>::type result = 0;

      ElementRange facets(mesh_obj);
      for (ElementIterator fit = facets.begin();
                           fit != facets.end();
                         ++fit)
      {
        if (is_boundary(mesh_obj, *fit))
          result += viennagrid::volume(*fit);
      }
      return result;
    }
  } //namespace detail



  //
  // The public interface functions
  //
  /** @brief Returns the surface of a n-cell using the provided point accessor to obtain the spatial points from each vertex. */
  template <typename PointAccessorT, bool element_is_const>
  typename viennagrid::result_of::coord<
      typename viennagrid::result_of::point_accessor_point< PointAccessorT, base_element<element_is_const> >::type
  >::type
  surface(PointAccessorT const accessor, base_element<element_is_const> const & element)
  {
    typedef base_element<element_is_const> ElementType;
    typedef typename viennagrid::result_of::const_facet_range<ElementType>::type   ElementBoundaryRange;
    typedef typename viennagrid::result_of::iterator<ElementBoundaryRange>::type                                         ElementBoundaryIterator;

    typedef typename viennagrid::result_of::point_accessor_point< PointAccessorT, base_element<element_is_const> >::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type value_type;

    value_type result = 0;

    ElementBoundaryRange boundary = viennagrid::facets(element);
    for (ElementBoundaryIterator ebit = boundary.begin();
                                 ebit != boundary.end();
                               ++ebit)
    {
      result += viennagrid::volume(accessor, *ebit);
    }

    return result;
  }

  /** @brief Returns the surface of a n-cell using the default point accessor. */
  template <bool element_is_const>
  typename viennagrid::result_of::coord< base_element<element_is_const> >::type
  surface( base_element<element_is_const> const & element)
  {
    return surface( root_mesh_point_accessor(), element );
  }



  //special case: mesh
  /** @brief Returns the surface of a mesh with explicit element type/tag*/
  template <typename ElementTypeOrTag, bool mesh_is_const>
  typename viennagrid::result_of::coord< base_mesh<mesh_is_const> >::type
  surface(base_mesh<mesh_is_const> const & d)
  {
    return detail::surface_meshsegment<ElementTypeOrTag>(d);
  }

  /** @brief Returns the surface of a mesh*/
  template <bool mesh_is_const>
  typename viennagrid::result_of::coord< base_mesh<mesh_is_const> >::type
  surface(base_mesh<mesh_is_const> const & d)
  {
    typedef typename viennagrid::result_of::cell_tag< base_mesh<mesh_is_const> >::type CellTag;
    return detail::surface_meshsegment< typename viennagrid::result_of::facet_tag<CellTag>::type>(d);
  }

  /** @brief Returns the surface of a segment*/
  template<bool mesh_region_is_const>
  typename viennagrid::result_of::coord< base_mesh_region<mesh_region_is_const> >::type
  surface(base_mesh_region<mesh_region_is_const> const & segment)
  {
    typedef typename viennagrid::result_of::cell_tag< base_mesh_region<mesh_region_is_const> >::type CellTag;
    return detail::surface_meshsegment< typename viennagrid::result_of::facet_tag<CellTag>::type >(segment);
  }



  /** @brief Returns a handle to the boundary element with the smallest volume */
  template<typename BoundaryElementTTag, typename MeshT, typename ElementT>
  typename viennagrid::result_of::element<MeshT, BoundaryElementTTag>::type
  smallest_boundary_volume( MeshT & mesh_obj, ElementT & element )
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type numeric_type;
    typedef typename viennagrid::result_of::element<ElementT, BoundaryElementTTag>::type           boundary_element_handle;
    typedef typename viennagrid::result_of::element_range<ElementT, BoundaryElementTTag>::type    boundary_element_range_type;
    typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type           boundary_element_range_iterator;

    boundary_element_range_type boundary_elements( element );
    boundary_element_range_iterator it = boundary_elements.begin();

    boundary_element_handle best_handle = it.handle();
    numeric_type best_volume = viennagrid::volume( mesh_obj, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      numeric_type volume = viennagrid::volume( mesh_obj, *it );
      if (volume < best_volume)
      {
        best_handle = it.handle();
        best_volume = volume;
      }
    }

    return best_handle;
  }

  /** @brief Returns a const handle to the boundary element with the smallest volume */
  template<typename BoundaryElementTTag, typename MeshT, typename ElementT>
  typename viennagrid::result_of::const_element<MeshT, BoundaryElementTTag>::type
  smallest_boundary_volume( MeshT const & mesh_obj, ElementT const & element )
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type numeric_type;
    typedef typename viennagrid::result_of::const_element<ElementT, BoundaryElementTTag>::type          boundary_element_handle;
    typedef typename viennagrid::result_of::const_element_range<ElementT, BoundaryElementTTag>::type   boundary_element_range_type;
    typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type                boundary_element_range_iterator;

    boundary_element_range_type boundary_elements( element );
    boundary_element_range_iterator it = boundary_elements.begin();

    boundary_element_handle best_handle = it.handle();
    numeric_type best_volume = viennagrid::volume( mesh_obj, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      numeric_type volume = viennagrid::volume( mesh_obj, *it );
      if (volume < best_volume)
      {
        best_handle = it.handle();
        best_volume = volume;
      }
    }

    return best_handle;
  }

  /** @brief Returns a handle to the boundary element with the largest volume */
  template<typename BoundaryElementTTag, typename MeshT, typename ElementT>
  typename viennagrid::result_of::element<MeshT, BoundaryElementTTag>::type largest_boundary_volume( MeshT & mesh_obj, ElementT & element )
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type numeric_type;
    typedef typename viennagrid::result_of::element<ElementT, BoundaryElementTTag>::type           boundary_element_handle;
    typedef typename viennagrid::result_of::element_range<ElementT, BoundaryElementTTag>::type    boundary_element_range_type;
    typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type           boundary_element_range_iterator;

    boundary_element_range_type boundary_elements( element );
    boundary_element_range_iterator it = boundary_elements.begin();

    boundary_element_handle best_handle = it.handle();
    numeric_type best_volume = viennagrid::volume( mesh_obj, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      numeric_type volume = viennagrid::volume( mesh_obj, *it );
      if (volume > best_volume)
      {
        best_handle = it.handle();
        best_volume = volume;
      }
    }

    return best_handle;
  }

  /** @brief Returns a const handle to the boundary element with the largest volume */
  template<typename BoundaryElementTTag, typename MeshT, typename ElementT>
  typename viennagrid::result_of::const_element<MeshT, BoundaryElementTTag>::type largest_boundary_volume( MeshT const & mesh_obj, ElementT const & element )
  {
    typedef typename viennagrid::result_of::coord<MeshT>::type numeric_type;
    typedef typename viennagrid::result_of::const_element<ElementT, BoundaryElementTTag>::type            boundary_element_handle;
    typedef typename viennagrid::result_of::const_element_range<ElementT, BoundaryElementTTag>::type     boundary_element_range_type;
    typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type                  boundary_element_range_iterator;

    boundary_element_range_type boundary_elements( element );
    boundary_element_range_iterator it = boundary_elements.begin();

    boundary_element_handle best_handle = it.handle();
    numeric_type best_volume = viennagrid::volume( mesh_obj, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      numeric_type volume = viennagrid::volume( mesh_obj, *it );
      if (volume > best_volume)
      {
        best_handle = it.handle();
        best_volume = volume;
      }
    }

    return best_handle;
  }


} //namespace viennagrid
#endif
