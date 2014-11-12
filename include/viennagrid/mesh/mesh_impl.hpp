#ifndef VIENNAGRID_MESH_IMPL_HPP
#define VIENNAGRID_MESH_IMPL_HPP

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/element/element.hpp"

namespace viennagrid
{

//   template<bool is_const>
//   typename base_mesh<is_const>::point_type base_mesh<is_const>::point( element_type vertex )
//   {
//     viennagrid_numeric * result;
//     viennagrid_vertex_get(internal(), vertex.id(), (viennagrid_numeric**)&result);
//     return point_type( mesh_hierarchy().dimension(), result );
//   }

  template<bool is_const>
  template<bool element_is_const>
  typename base_mesh<is_const>::point_type base_mesh<is_const>::get_point(base_element<element_is_const> const & vertex) const
  {
    point_type result( mesh_hierarchy().dimension() );
    viennagrid_numeric const * tmp;
    viennagrid_vertex_get(internal(), vertex.id(), const_cast<viennagrid_numeric **>(&tmp));
    std::copy(tmp, tmp+result.size(), result.begin());
    return result;
  }

  template<bool is_const>
  void base_mesh<is_const>::set_point(element_type vertex, point_type const & point)
  {
    viennagrid_numeric * tmp;
    viennagrid_vertex_get(internal(), vertex.id(), &tmp);
    std::copy(point.begin(), point.end(), tmp);
  }



  inline void set_point(base_mesh<false> m,
                        base_element<false> const & vertex,
                        result_of::point< base_mesh<false> >::type const & point)
  { return m.set_point(vertex, point); }



  template<bool element_is_const>
  typename result_of::point< base_element<element_is_const> >::type get_point(base_element<element_is_const> const & vertex)
  { return vertex.mesh_hierarchy().root().get_point(vertex); }

  void set_point(element_t vertex, result_of::point<element_t>::type const & point)
  { vertex.mesh_hierarchy().root().set_point(vertex, point); }

//   inline result_of::point<const_element_t>::type point(const_element_t vertex)
//   {
//     return vertex.mesh_hierarchy().root().point(vertex);
//   }




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
