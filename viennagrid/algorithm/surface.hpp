#ifndef VIENNAGRID_ALGORITHM_SURFACE_HPP
#define VIENNAGRID_ALGORITHM_SURFACE_HPP

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
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/boundary.hpp"

/** @file viennagrid/algorithm/surface.hpp
    @brief Computes the surface of different cell types as well as meshs and segments
*/


namespace viennagrid
{
  namespace detail
  {
    /** @brief Implementation for the calculation of the surface of a mesh or segment */
    template <typename ElementTypeOrTag, typename MeshType>
    typename viennagrid::result_of::coord< MeshType >::type
    surface_meshsegment(MeshType const & mesh)
    {
      typedef typename viennagrid::result_of::const_element_range<MeshType, ElementTypeOrTag>::type  ElementRange;
      typedef typename viennagrid::result_of::iterator<ElementRange>::type                           ElementIterator;

      typename viennagrid::result_of::coord<MeshType>::type result = 0;

      ElementRange facets = viennagrid::elements(mesh);
      for (ElementIterator fit = facets.begin();
                         fit != facets.end();
                       ++fit)
      {
        if (is_boundary(mesh, *fit))
          result += viennagrid::volume(*fit);
      }
      return result;
    }
  } //namespace detail



  //
  // The public interface functions
  //
  /** @brief Returns the surface of a n-cell */
  template <typename PointAccessorType, typename ElementTag, typename WrappedConfigType>
  typename viennagrid::result_of::coord< typename PointAccessorType::value_type >::type
  surface(PointAccessorType const accessor, element_t<ElementTag, WrappedConfigType> const & element)
  {
    typedef typename viennagrid::result_of::const_facet_range< element_t<ElementTag, WrappedConfigType> >::type   ElementBoundaryRange;
    typedef typename viennagrid::result_of::iterator<ElementBoundaryRange>::type                                  ElementBoundaryIterator;

    typedef typename viennagrid::result_of::coord< typename PointAccessorType::value_type >::type value_type;

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

  template < typename ElementTag, typename WrappedConfigType>
  typename viennagrid::result_of::coord< element_t<ElementTag, WrappedConfigType> >::type
  surface( element_t<ElementTag, WrappedConfigType> const & element)
  {
    return surface( default_point_accessor(element), element );
  }



  //special case: mesh
  /** @brief Returns the surface of a mesh with explicit element type/tag*/
  template <typename ElementTypeOrTag, typename WrappedConfigType>
  typename viennagrid::result_of::coord< mesh_t<WrappedConfigType> >::type
  surface(mesh_t<WrappedConfigType> const & d)
  {
    return detail::surface_meshsegment<ElementTypeOrTag>(d);
  }

  /** @brief Returns the surface of a mesh*/
  template <typename WrappedConfigType>
  typename viennagrid::result_of::coord< mesh_t<WrappedConfigType> >::type
  surface(mesh_t<WrappedConfigType> const & d)
  {
    typedef typename viennagrid::result_of::cell_tag< mesh_t<WrappedConfigType> >::type CellTag;
    return detail::surface_meshsegment< typename viennagrid::result_of::facet_tag<CellTag>::type>(d);
  }

  /** @brief Returns the surface of a segment*/
  template<typename SegmentationType>
  typename viennagrid::result_of::coord< typename SegmentationType::mesh_type >::type
  surface(segment_handle_t<SegmentationType> const & segment)
  {
    typedef typename viennagrid::result_of::cell_tag< typename SegmentationType::mesh_type >::type CellTag;
    return detail::surface_meshsegment< typename viennagrid::result_of::facet_tag<CellTag>::type >(segment);
  }



  /** @brief Returns a handle to the boundary element with the smallest volume */
  template<typename boundary_element_type_or_tag, typename mesh_type, typename element_type>
  typename viennagrid::result_of::handle<mesh_type, boundary_element_type_or_tag>::type
  smallest_boundary_volume( mesh_type & mesh, element_type & element )
  {
    typedef typename viennagrid::result_of::coord<mesh_type>::type numeric_type;
    typedef typename viennagrid::result_of::handle<element_type, boundary_element_type_or_tag>::type           boundary_element_handle;
    typedef typename viennagrid::result_of::element_range<element_type, boundary_element_type_or_tag>::type    boundary_element_range_type;
    typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type                        boundary_element_range_iterator;

    boundary_element_range_type boundary_elements = viennagrid::elements( element );
    boundary_element_range_iterator it = boundary_elements.begin();

    boundary_element_handle best_handle = it.handle();
    numeric_type best_volume = viennagrid::volume( mesh, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      numeric_type volume = viennagrid::volume( mesh, *it );
      if (volume < best_volume)
      {
        best_handle = it.handle();
        best_volume = volume;
      }
    }

    return best_handle;
  }

  /** @brief Returns a const handle to the boundary element with the smallest volume */
  template<typename boundary_element_type_or_tag, typename mesh_type, typename element_type>
  typename viennagrid::result_of::const_handle<mesh_type, boundary_element_type_or_tag>::type
  smallest_boundary_volume( mesh_type const & mesh, element_type const & element )
  {
    typedef typename viennagrid::result_of::coord<mesh_type>::type numeric_type;
    typedef typename viennagrid::result_of::const_handle<element_type, boundary_element_type_or_tag>::type          boundary_element_handle;
    typedef typename viennagrid::result_of::const_element_range<element_type, boundary_element_type_or_tag>::type   boundary_element_range_type;
    typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type                             boundary_element_range_iterator;

    boundary_element_range_type boundary_elements = viennagrid::elements( element );
    boundary_element_range_iterator it = boundary_elements.begin();

    boundary_element_handle best_handle = it.handle();
    numeric_type best_volume = viennagrid::volume( mesh, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      numeric_type volume = viennagrid::volume( mesh, *it );
      if (volume < best_volume)
      {
        best_handle = it.handle();
        best_volume = volume;
      }
    }

    return best_handle;
  }

  /** @brief Returns a handle to the boundary element with the largest volume */
  template<typename boundary_element_type_or_tag, typename mesh_type, typename element_type>
  typename viennagrid::result_of::handle<mesh_type, boundary_element_type_or_tag>::type largest_boundary_volume( mesh_type & mesh, element_type & element )
  {
    typedef typename viennagrid::result_of::coord<mesh_type>::type numeric_type;
    typedef typename viennagrid::result_of::handle<element_type, boundary_element_type_or_tag>::type           boundary_element_handle;
    typedef typename viennagrid::result_of::element_range<element_type, boundary_element_type_or_tag>::type    boundary_element_range_type;
    typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type                        boundary_element_range_iterator;

    boundary_element_range_type boundary_elements = viennagrid::elements( element );
    boundary_element_range_iterator it = boundary_elements.begin();

    boundary_element_handle best_handle = it.handle();
    numeric_type best_volume = viennagrid::volume( mesh, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      numeric_type volume = viennagrid::volume( mesh, *it );
      if (volume > best_volume)
      {
        best_handle = it.handle();
        best_volume = volume;
      }
    }

    return best_handle;
  }

  /** @brief Returns a const handle to the boundary element with the largest volume */
  template<typename boundary_element_type_or_tag, typename mesh_type, typename element_type>
  typename viennagrid::result_of::const_handle<mesh_type, boundary_element_type_or_tag>::type largest_boundary_volume( mesh_type const & mesh, element_type const & element )
  {
    typedef typename viennagrid::result_of::coord<mesh_type>::type numeric_type;
    typedef typename viennagrid::result_of::const_handle<element_type, boundary_element_type_or_tag>::type            boundary_element_handle;
    typedef typename viennagrid::result_of::const_element_range<element_type, boundary_element_type_or_tag>::type     boundary_element_range_type;
    typedef typename viennagrid::result_of::iterator<boundary_element_range_type>::type                               boundary_element_range_iterator;

    boundary_element_range_type boundary_elements = viennagrid::elements( element );
    boundary_element_range_iterator it = boundary_elements.begin();

    boundary_element_handle best_handle = it.handle();
    numeric_type best_volume = viennagrid::volume( mesh, *it );

    for (; it != boundary_elements.end(); ++it)
    {
      numeric_type volume = viennagrid::volume( mesh, *it );
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
