#ifndef VIENNAGRID_MESH_HIERARCHY_IMPL_HPP
#define VIENNAGRID_MESH_HIERARCHY_IMPL_HPP

#include "viennagrid/mesh/mesh_hierarchy.hpp"
#include "viennagrid/mesh/mesh.hpp"

namespace viennagrid
{
  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::mesh_type base_mesh_hierarchy<is_const>::get_root()
  {
    viennagrid_mesh internal_root;
    viennagrid_mesh_hierarchy_get_root(internal_mesh_hierarchy_, &internal_root);
    return mesh_type(internal_root);
  }

  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::region_type base_mesh_hierarchy<is_const>::get_make_region(region_id_type region_id)
  {
    viennagrid_region region;
    viennagrid_region_get_create( internal_mesh_hierarchy(), region_id, &region );
    return region_type(region);
  }

  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::region_type base_mesh_hierarchy<is_const>::make_region()
  {
    viennagrid_region region;
    viennagrid_region_create( internal_mesh_hierarchy(), &region );
    return region_type(region);
  }

  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::const_region_type base_mesh_hierarchy<is_const>::get_region(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_region_get( internal_mesh_hierarchy(), region_id, &region );
    return const_region_type(region);
  }
}


#endif
