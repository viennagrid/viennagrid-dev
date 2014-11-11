#ifndef VIENNAGRID_MESH_IMPL_HPP
#define VIENNAGRID_MESH_IMPL_HPP

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/element/element.hpp"

namespace viennagrid
{

  template<bool is_const>
  typename base_mesh<is_const>::point_type base_mesh<is_const>::point( element_type vertex )
  {
    viennagrid_numeric * result;
    viennagrid_vertex_get(internal(), vertex.id(), (viennagrid_numeric**)&result);
    return point_type( mesh_hierarchy().dimension(), result );
  }

  template<bool is_const>
  typename base_mesh<is_const>::const_point_type base_mesh<is_const>::point(const_element_type vertex) const
  {
    viennagrid_numeric const * result;
    viennagrid_vertex_get(internal(), vertex.id(), const_cast<viennagrid_numeric **>(&result));
    return const_point_type( mesh_hierarchy().dimension(), result );
  }



  inline result_of::point<element_t>::type point(element_t vertex)
  {
    return vertex.mesh_hierarchy().root().point(vertex);
  }

  inline result_of::point<const_element_t>::type point(const_element_t vertex)
  {
    return vertex.mesh_hierarchy().root().point(vertex);
  }




  template<bool is_const>
  std::size_t base_mesh<is_const>::regions_count() const { return mesh_hierarchy().regions_count(); }

  template<bool is_const>
  typename base_mesh<is_const>::region_type base_mesh<is_const>::get_make_region(region_id_type region_id) { return region_type(*this, mesh_hierarchy().get_make_region(region_id)); }

  template<bool is_const>
  typename base_mesh<is_const>::region_type base_mesh<is_const>::make_region() { return region_type(*this, mesh_hierarchy().make_region()); }

  template<bool is_const>
  typename base_mesh<is_const>::const_region_type base_mesh<is_const>::get_region(region_id_type region_id) const { return region_type(*this, mesh_hierarchy().get_region(region_id)); }
}

#endif
