#ifndef VIENNAGRID_MESH_IMPL_HPP
#define VIENNAGRID_MESH_IMPL_HPP

#include "viennagrid/range.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/element/element.hpp"

namespace viennagrid
{

  template<bool mesh_is_const, bool element_is_const>
  typename result_of::point< base_element<element_is_const> >::type get_point(base_mesh<mesh_is_const> m, base_element<element_is_const> vertex)
  { return get_point(m.mesh_hierarchy(), vertex); }


  inline void set_point(base_mesh<false> m,
                        base_element<false> const & vertex,
                        result_of::point< base_mesh<false> >::type const & point)
  { set_point( m.mesh_hierarchy(), vertex, point ); }



  template<bool element_is_const>
  typename result_of::point< base_element<element_is_const> >::type get_point(base_element<element_is_const> const & vertex)
  {
    return get_point( vertex.mesh_hierarchy(), vertex );
  }

  void set_point(element_t vertex, result_of::point<element_t>::type const & point)
  {
    set_point( vertex.mesh_hierarchy(), vertex, point );
  }


  template<bool is_const>
  std::size_t base_mesh<is_const>::region_count() const
  { return mesh_hierarchy().region_count(); }

  template<bool is_const>
  typename base_mesh<is_const>::region_type base_mesh<is_const>::get_make_region(region_id_type region_id)
  { return region_type(*this, mesh_hierarchy().get_make_region(region_id)); }

  template<bool is_const>
  typename base_mesh<is_const>::region_type base_mesh<is_const>::make_region()
  { return region_type(*this, mesh_hierarchy().make_region()); }

  template<bool is_const>
  typename base_mesh<is_const>::const_region_type base_mesh<is_const>::get_region(region_id_type region_id) const
  { return region_type(*this, mesh_hierarchy().get_region(region_id)); }


  template<bool is_const>
  typename base_mesh<is_const>::region_type base_mesh<is_const>::get_make_region(std::string const & name)
  { return region_type(*this, mesh_hierarchy().get_make_region(name)); }

  template<bool is_const>
  typename base_mesh<is_const>::const_region_type base_mesh<is_const>::get_region(std::string const & name) const
  { return region_type(*this, mesh_hierarchy().get_region(name)); }




  template<bool element_is_const, bool mesh_is_const>
  bool is_boundary( base_mesh<mesh_is_const> mesh, base_element<element_is_const> element )
  {
    viennagrid_bool result;
    viennagrid_is_boundary_mesh(mesh.internal(),
                                viennagrid::topologic_dimension(element),
                                element.id(),
                                &result);
    return result == VIENNAGRID_TRUE;
  }




  inline void non_recursive_add(mesh_t mesh, element_t const & element)
  {
    viennagrid_element_add(mesh.internal(),
                           viennagrid::topologic_dimension(element),
                           element.id());
  }

  inline void add(mesh_t & mesh, element_t const & element)
  {
    non_recursive_add(mesh, element);

    for (viennagrid_int i = 0; i < viennagrid::topologic_dimension(element); ++i)
    {
      typedef viennagrid::result_of::element_range<element_t>::type ElementRangeType;
      ElementRangeType elements(element, i);
      for (ElementRangeType::iterator it = elements.begin(); it != elements.end(); ++it)
        non_recursive_add(mesh, *it);
    }
  }



}

#endif
