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
  class base_mesh
  {
    template<bool other_is_const>
    friend class base_mesh;

    template<bool other_is_const>
    friend class base_mesh_hierarchy;

  public:

    typedef element_tag element_tag_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy, true>::type const_mesh_hierarchy_type;

    typedef typename result_of::const_nonconst<mesh_region, is_const>::type region_type;
    typedef typename result_of::const_nonconst<mesh_region, true>::type const_region_type;
    typedef typename mesh_hierarchy_type::region_id_type region_id_type;

    typedef typename result_of::const_nonconst<point_proxy, is_const>::type point_type;
    typedef typename result_of::const_nonconst<point_proxy, true>::type const_point_type;

    typedef typename result_of::const_nonconst<element, is_const>::type element_type;
    typedef typename result_of::const_nonconst<element, true>::type const_element_type;



    base_mesh(viennagrid_int dimension_, element_tag_type cell_tag_) : internal_mesh_(0)
    {
      base_mesh_hierarchy<is_const> mesh_hierarchy(dimension_, cell_tag_);
      *this = mesh_hierarchy.get_root();
      retain();
    }

    template<bool other_is_const>
    base_mesh(base_mesh<other_is_const> const & mesh) : internal_mesh_(mesh.internal_mesh_)
    {
      retain();
    }

    base_mesh(base_mesh<is_const> const & mesh) : internal_mesh_(mesh.internal_mesh_)
    {
      retain();
    }

    base_mesh<is_const> & operator=( base_mesh<is_const> const & mh )
    {
      release();
      internal_mesh_ = mh.internal_mesh_;
      retain();
      return *this;
    }

    ~base_mesh()
    {
      release();
    }



    viennagrid_int dimension() const { return get_mesh_hierarchy().dimension(); }
    element_tag_type cell_tag() const { return get_mesh_hierarchy().cell_tag(); }
    element_tag_type facet_tag() const { return get_mesh_hierarchy().facet_tag(); }

    point_type point(element_type vertex);
    const_point_type point(const_element_type vertex) const;

    mesh_hierarchy_type get_mesh_hierarchy()
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_mesh_get_mesh_hierarchy(internal_mesh(), &tmp);
      return mesh_hierarchy_type(tmp);
    }

    const_mesh_hierarchy_type get_mesh_hierarchy() const
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_mesh_get_mesh_hierarchy(internal_mesh(), &tmp);
      return const_mesh_hierarchy_type(tmp);
    }

    viennagrid_mesh internal_mesh() const { return const_cast<viennagrid_mesh>(internal_mesh_); }

    std::size_t regions_count() const;
    region_type get_make_region(region_id_type region_id);
    region_type make_region();
    const_region_type get_region(region_id_type region_id) const;

    element_tag_type unpack_element_tag(element_tag_type et) { return get_mesh_hierarchy().unpack_element_tag(et); }
  private:
    base_mesh(viennagrid_mesh internal_mesh_in) : internal_mesh_(internal_mesh_in) {}

    void retain() const
    {
      if (internal_mesh())
        get_mesh_hierarchy().retain();
    }

    void release() const
    {
      if (internal_mesh())
        get_mesh_hierarchy().release();
    }

    viennagrid_mesh internal_mesh_;
  };




  template<typename ElementT>
  typename result_of::point<ElementT>::type point(mesh & m, ElementT vertex)
  {
    return m.point(vertex);
  }

  template<typename ElementT>
  typename result_of::const_point<ElementT>::type point(mesh const & m, ElementT vertex)
  {
    return m.point(vertex);
  }




  template<bool element_is_const, bool mesh_is_const>
  bool is_boundary( base_mesh<mesh_is_const> m, base_element<element_is_const> e )
  {
    viennagrid_bool result;
    viennagrid_is_boundary_mesh(m.internal_mesh(), e.tag().internal_element_tag(), e.id(), &result);
    return result == VIENNAGRID_TRUE;
  }



}


#endif
