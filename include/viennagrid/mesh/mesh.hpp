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
    friend class base_mesh_hierarchy;

  public:

    typedef element_tag_t element_tag_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy_t, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy_t, true>::type const_mesh_hierarchy_type;

    typedef typename result_of::const_nonconst<mesh_region_t, is_const>::type region_type;
    typedef typename result_of::const_nonconst<mesh_region_t, true>::type const_region_type;
    typedef typename mesh_hierarchy_type::region_id_type region_id_type;

    typedef point_t point_type;

    typedef typename result_of::const_nonconst<element_t, is_const>::type element_type;
    typedef typename result_of::const_nonconst<element_t, true>::type const_element_type;


    base_mesh(viennagrid_mesh internal_mesh_in) : internal_mesh_(internal_mesh_in)
    {
      retain();
    }

    base_mesh() : internal_mesh_(0)
    {
      base_mesh_hierarchy<is_const> mesh_hierarchy;
      *this = mesh_hierarchy.root();
    }

    template<bool other_is_const>
    base_mesh(base_mesh<other_is_const> const & mesh) : internal_mesh_(mesh.internal())
    {
      retain();
    }

    base_mesh(base_mesh<is_const> const & mesh) : internal_mesh_(mesh.internal())
    {
      retain();
    }

    base_mesh<is_const> & operator=( base_mesh<is_const> const & mh )
    {
      release();
      internal_mesh_ = mh.internal();
      retain();
      return *this;
    }

    ~base_mesh()
    {
      release();
    }

//     element_tag_type cell_tag() const { return mesh_hierarchy().cell_tag(); }
// //     element_tag_type facet_tag() const { return mesh_hierarchy().facet_tag(); }

    mesh_hierarchy_type mesh_hierarchy()
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_mesh_get_mesh_hierarchy(internal(), &tmp);
      return mesh_hierarchy_type(tmp);
    }

    const_mesh_hierarchy_type mesh_hierarchy() const
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_mesh_get_mesh_hierarchy(internal(), &tmp);
      return const_mesh_hierarchy_type(tmp);
    }

    viennagrid_mesh internal() const { return const_cast<viennagrid_mesh>(internal_mesh_); }

    std::size_t region_count() const;
    region_type get_make_region(region_id_type region_id);
    region_type make_region();
    const_region_type get_region(region_id_type region_id) const;

    region_type get_make_region(std::string const & name);
    const_region_type get_region(std::string const & name) const;

//     element_tag_type unpack_element_tag(element_tag_type et) { return mesh_hierarchy().unpack_element_tag(et); }
  private:

    void retain() const
    {
      if (internal())
        mesh_hierarchy().retain();
    }

    void release() const
    {
      if (internal())
        mesh_hierarchy().release();
    }

    viennagrid_mesh internal_mesh_;
  };




  template<bool is_const>
  base_mesh_hierarchy<is_const> mesh_hierarchy( base_mesh<is_const> mesh )
  {
    return mesh.mesh_hierarchy();
  }






  template<bool mesh_is_const, bool element_is_const>
  typename result_of::point< base_element<element_is_const> >::type get_point(base_mesh<mesh_is_const> m, base_element<element_is_const> vertex);
  inline void set_point(base_mesh<false> m, base_element<false> const & vertex, result_of::point< base_mesh<false> >::type const & point);


  template<bool element_is_const>
  typename result_of::point< base_element<element_is_const> >::type get_point(base_element<element_is_const> const & vertex);
  inline void set_point(element_t vertex, result_of::point<element_t>::type const & point);




  template<bool element_is_const, bool mesh_is_const>
  bool is_boundary( base_mesh<mesh_is_const> m, base_element<element_is_const> e )
  {
    viennagrid_bool result;
    viennagrid_is_boundary_mesh(m.internal(), e.tag().internal(), e.id(), &result);
    return result == VIENNAGRID_TRUE;
  }

  template<typename SomethingFirstT, typename SomethingSecondT, typename ElementT>
  bool is_interface( SomethingFirstT first, SomethingSecondT second, ElementT element )
  {
    return is_boundary(first, element) && is_boundary(second, element);
  }


}


#endif
