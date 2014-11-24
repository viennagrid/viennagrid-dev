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
    @brief Computes the surface of different cell types as well as meshs and regions
*/


namespace viennagrid
{
  namespace detail
  {
    /** @brief Implementation for the calculation of the surface of a mesh or region */
    template<typename MeshT>
    typename viennagrid::result_of::coord< MeshT >::type
    surface_meshregion(MeshT const & mesh_obj, viennagrid_int facet_dimension)

//                        element_tag_t facet_tag, element_tag_t facet_end)
    {
      typedef typename viennagrid::result_of::const_element_range<MeshT>::type  ElementRange;
      typedef typename viennagrid::result_of::iterator<ElementRange>::type      ElementIterator;

      typename viennagrid::result_of::coord<MeshT>::type result = 0;

//       for (; facet_tag != facet_end; ++facet_tag)
//       {
        ElementRange facets(mesh_obj, facet_dimension);
        for (ElementIterator fit = facets.begin();
                            fit != facets.end();
                          ++fit)
        {
          if (is_boundary(mesh_obj, *fit))
            result += viennagrid::volume(*fit);
        }
//       }


      return result;
    }
  } //namespace detail



  //
  // The public interface functions
  //
  /** @brief Returns the surface of a n-cell using the provided point accessor to obtain the spatial points from each vertex. */
  template <typename PointAccessorT, bool element_is_const>
  typename viennagrid::result_of::coord< PointAccessorT, base_element<element_is_const> >::type
  surface(PointAccessorT const accessor, base_element<element_is_const> const & element)
  {
    typedef base_element<element_is_const> ElementType;
    typedef typename viennagrid::result_of::const_element_range<ElementType>::type   ElementBoundaryRange;
    typedef typename viennagrid::result_of::iterator<ElementBoundaryRange>::type                                         ElementBoundaryIterator;

    typedef typename viennagrid::result_of::point< PointAccessorT, base_element<element_is_const> >::type PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type value_type;

    value_type result = 0;

//     for (element_tag_t facet_tag = facet_tag_begin(element); facet_tag != facet_tag_end(element); ++facet_tag)
//     {
    ElementBoundaryRange boundary(element, viennagrid::facet_dimension(element));
    for (ElementBoundaryIterator ebit = boundary.begin();
                                ebit != boundary.end();
                              ++ebit)
    {
      result += viennagrid::volume(accessor, *ebit);
    }
//     }

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
    return detail::surface_meshregion<ElementTypeOrTag>(d);
  }

  /** @brief Returns the surface of a mesh*/
  template <bool mesh_is_const>
  typename viennagrid::result_of::coord< base_mesh<mesh_is_const> >::type
  surface(base_mesh<mesh_is_const> const & d)
  {
    return detail::surface_meshregion(d, viennagrid::facet_dimension(d));
  }

  /** @brief Returns the surface of a region*/
  template<bool mesh_region_is_const>
  typename viennagrid::result_of::coord< base_mesh_region<mesh_region_is_const> >::type
  surface(base_mesh_region<mesh_region_is_const> const & region)
  {
    return detail::surface_meshregion(region, viennagrid::facet_dimension(region));
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
