#ifndef VIENNAGRID_ALGORITHM_EXTRACT_BOUNDARY_HPP
#define VIENNAGRID_ALGORITHM_EXTRACT_BOUNDARY_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/mesh_operations.hpp"

/** @file viennagrid/algorithm/extract_boundary.hpp
    @brief Extraction of a hull/boundary of a mesh.
*/

namespace viennagrid
{
  /** @brief Extracts the hull of mesh using viennagrid::boundary, e.g. the triangular hull of a tetrahedral mesh.
   *
   * @tparam HullTypeOrTagT                The type or tag of the hull element
   * @param volume_mesh                    The input mesh
   * @param hull_mesh                      The output hull mesh
   */
  template<bool mesh_is_const>
  void extract_boundary(viennagrid::base_mesh<mesh_is_const> const & volume_mesh,
                        viennagrid::mesh_t const & hull_mesh,
                        viennagrid_dimension hull_dimension)
  {
    viennagrid::clear(hull_mesh);

    typedef viennagrid::base_mesh<mesh_is_const> VolumeMeshType;
    typedef viennagrid::mesh_t HullMeshType;

    typedef typename viennagrid::result_of::const_element_range<VolumeMeshType>::type    HullRangeType;
    typedef typename viennagrid::result_of::iterator<HullRangeType>::type                                   HullRangeIterator;

    typedef typename viennagrid::result_of::element<VolumeMeshType>::type    VolumeElement;
    typedef typename viennagrid::result_of::element<HullMeshType>::type      HullElement;

    viennagrid::result_of::element_copy_map<>::type element_map(hull_mesh);

    HullRangeType hull_elements( volume_mesh, hull_dimension );
    for (HullRangeIterator hit = hull_elements.begin(); hit != hull_elements.end(); ++hit)
    {
      if ( viennagrid::is_any_boundary( volume_mesh, *hit ) )
      {
        HullElement hull_element = element_map( *hit );

        typedef typename viennagrid::result_of::region_range<VolumeMeshType, VolumeElement>::type RegionRangeType;
        typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

        RegionRangeType regions(volume_mesh, *hit);
        for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
        {
          viennagrid::add( hull_mesh.get_make_region((*rit).id()), hull_element );
        }
      }
    }
  }

  /** @brief Extracts the hull of mesh using viennagrid::boundary, e.g. the triangular hull of a tetrahedral mesh. The facet type is used as the hull element type.
   *
   * @param volume_mesh                    The input mesh
   * @param hull_mesh                      The output hull mesh
   */
  template<bool mesh_is_const>
  void extract_boundary(viennagrid::base_mesh<mesh_is_const> const & volume_mesh,
                        viennagrid::mesh_t const & hull_mesh)
  {
    extract_boundary(volume_mesh, hull_mesh, viennagrid::facet_dimension(volume_mesh));
  }

}

#endif
