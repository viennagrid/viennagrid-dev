#ifndef VIENNAGRID_MESH_HPP
#define VIENNAGRID_MESH_HPP

#include <map>

#include "viennagrid/forwards.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/iterators.hpp"

#include "viennagrid/mesh/mesh_hierarchy.hpp"

namespace viennagrid
{



  template<bool is_const>
  class base_mesh_t
  {
    template<bool other_is_const>
    friend class base_mesh_t;

    template<bool other_is_const>
    friend class base_mesh_hierarchy_t;


    typedef typename result_of::const_nonconst<viennagrid_mesh_hierarchy_handle, is_const>::type internal_mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<viennagrid_mesh_hierarchy_handle, true>::type const_internal_mesh_hierarchy_type;

  public:

    template<bool other_is_const>
    base_mesh_t(base_mesh_t<other_is_const> const & mesh) : internal_mesh_(mesh.internal_mesh_) {}

    typedef element_tag_t element_tag_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy_t, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy_t, true>::type const_mesh_hierarchy_type;

    typedef typename result_of::const_nonconst<mesh_region_t, is_const>::type region_type;
    typedef typename result_of::const_nonconst<mesh_region_t, true>::type const_region_type;
//     typedef typename mesh_hierarchy_type::region_type region_type;
//     typedef typename mesh_hierarchy_type::const_region_type const_region_type;
    typedef typename mesh_hierarchy_type::region_id_type region_id_type;

    typedef typename result_of::const_nonconst<point_proxy_t, is_const>::type point_type;
    typedef typename result_of::const_nonconst<point_proxy_t, true>::type const_point_type;

    typedef typename result_of::const_nonconst<element_t, is_const>::type element_type;
    typedef typename result_of::const_nonconst<element_t, true>::type const_element_type;


    viennagrid_int dimension() const;
    element_tag_type cell_tag() const;
    element_tag_type facet_tag() const;

    point_type point(element_type vertex);
    const_point_type point(const_element_type vertex) const;

    mesh_hierarchy_type mesh_hierarchy();
    const_mesh_hierarchy_type mesh_hierarchy() const;

    viennagrid_mesh_handle internal_mesh() { return internal_mesh_; }
    viennagrid_mesh_const_handle internal_mesh() const { return internal_mesh_; }

    std::size_t regions_count() const;// { return mesh_hierarchy().regions_count(); }
    region_type get_make_region(region_id_type region_id);// { return mesh_region_type(*this, mesh_hierarchy().get_make_region(region_id)); }
    region_type make_region();// { return mesh_hierarchy().make_region(); }
    const_region_type get_region(region_id_type region_id) const;// { return mesh_region_type(*this, mesh_hierarchy().get_region(region_id)); }

    element_tag_type unpack_element_tag(element_tag_type et);
  private:
    base_mesh_t(viennagrid_mesh_handle internal_mesh_in) : internal_mesh_(internal_mesh_in) {}

    viennagrid_mesh_handle internal_mesh_;
  };












  template<bool is_const>
  inline viennagrid_int base_mesh_t<is_const>::dimension() const { return mesh_hierarchy().dimension(); }

  template<bool is_const>
  inline typename base_mesh_t<is_const>::element_tag_type base_mesh_t<is_const>::cell_tag() const { return mesh_hierarchy().cell_tag(); }

  template<bool is_const>
  inline typename base_mesh_t<is_const>::element_tag_type base_mesh_t<is_const>::facet_tag() const { return mesh_hierarchy().facet_tag(); }


  template<>
  inline base_mesh_t<true>::mesh_hierarchy_type base_mesh_t<true>::mesh_hierarchy()
  {
    internal_mesh_hierarchy_type tmp;
    viennagrid_mesh_get_mesh_hierarchy_const(internal_mesh(), &tmp);
    return mesh_hierarchy_type(tmp);
  }

  template<>
  inline base_mesh_t<false>::mesh_hierarchy_type base_mesh_t<false>::mesh_hierarchy()
  {
    internal_mesh_hierarchy_type tmp;
    viennagrid_mesh_get_mesh_hierarchy(internal_mesh(), &tmp);
    return mesh_hierarchy_type(tmp);
  }

  template<bool is_const>
  typename base_mesh_t<is_const>::const_mesh_hierarchy_type base_mesh_t<is_const>::mesh_hierarchy() const
  {
    const_internal_mesh_hierarchy_type tmp;
    viennagrid_mesh_get_mesh_hierarchy_const(internal_mesh(), &tmp);
    return const_mesh_hierarchy_type(tmp);
  }




  template<bool is_const>
  inline typename base_mesh_t<is_const>::element_tag_type base_mesh_t<is_const>::unpack_element_tag(element_tag_type et)
  { return mesh_hierarchy().unpack_element_tag(et); }




  template<typename ElementT>
  typename result_of::point<ElementT>::type point(mesh_t & mesh, ElementT vertex)
  {
    return mesh.point(vertex);
  }

  template<typename ElementT>
  typename result_of::const_point<ElementT>::type point(mesh_t const & mesh, ElementT vertex)
  {
    return mesh.point(vertex);
  }




  template<bool element_is_const, bool mesh_is_const>
  bool is_boundary( base_element_t<element_is_const> element, base_mesh_t<mesh_is_const> mesh )
  {
    viennagrid_bool result;
    viennagrid_is_boundary_mesh(mesh.internal_mesh(), element.tag().internal_element_tag(), element.id(), &result);
    return result == VIENNAGRID_TRUE;
  }



}


#endif
