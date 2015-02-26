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

    mesh_hierarchy_type mesh_hierarchy()
    {
      return mesh_hierarchy_type(internal_mesh_hierarchy());
    }

    const_mesh_hierarchy_type mesh_hierarchy() const
    {
      return const_mesh_hierarchy_type(internal_mesh_hierarchy());
    }

    viennagrid_mesh_hierarchy internal_mesh_hierarchy() const
    {
      viennagrid_mesh_hierarchy tmp;
      viennagrid_mesh_get_mesh_hierarchy(internal(), &tmp);
      return tmp;
    }

    mesh_t make_child()
    {
      viennagrid_mesh tmp;
      viennagrid_mesh_create( internal(), &tmp );
      return mesh_t(tmp);
    }

    viennagrid_mesh internal() const { return const_cast<viennagrid_mesh>(internal_mesh_); }

    bool is_root() const
    {
      viennagrid_mesh mesh;
      viennagrid_mesh_hierarchy_get_root( internal_mesh_hierarchy(), &mesh);
      return internal() == mesh;
    }

    bool operator==(base_mesh<is_const> const & rhs) const { return internal() == rhs.internal(); }
    bool operator!=(base_mesh<is_const> const & rhs) const { return !(*this == rhs); }

    std::size_t region_count() const;
    region_type get_make_region(region_id_type region_id) const;
    region_type make_region() const;
    const_region_type get_region(region_id_type region_id) const;
    bool region_exists(region_id_type region_id) const;

    region_type get_make_region(std::string const & name) const;
    const_region_type get_region(std::string const & name) const;
    bool region_exists(std::string const & name) const;

  private:

    void retain() const
    {
      if (internal())
        viennagrid_mesh_hierarchy_retain( internal_mesh_hierarchy() );
    }

    void release() const
    {
      if (internal())
        viennagrid_mesh_hierarchy_release( internal_mesh_hierarchy() );
    }

    viennagrid_mesh internal_mesh_;
  };




  template<bool is_const>
  base_mesh_hierarchy<is_const> mesh_hierarchy( base_mesh<is_const> const & mesh )
  {
    return mesh.mesh_hierarchy();
  }

  inline viennagrid_mesh_hierarchy internal_mesh_hierarchy(viennagrid_mesh mesh)
  {
    viennagrid_mesh_hierarchy tmp;
    viennagrid_mesh_get_mesh_hierarchy(mesh, &tmp);
    return tmp;
  }

  template<bool is_const>
  viennagrid_mesh_hierarchy internal_mesh_hierarchy( base_mesh<is_const> const & mesh )
  {
    return mesh.internal_mesh_hierarchy();
  }







  template<bool mesh_is_const, bool element_is_const>
  bool is_boundary( base_mesh<mesh_is_const> const & mesh, base_element<element_is_const> const & element );

  template<bool mesh_is_const, bool element_is_const>
  bool is_any_boundary(base_mesh<mesh_is_const> const & mesh, base_element<element_is_const> const & element);

  template<typename SomethingFirstT, typename SomethingSecondT, typename ElementT>
  bool is_interface( SomethingFirstT const & first, SomethingSecondT const & second, ElementT const & element )
  {
    return is_boundary(first, element) && is_boundary(second, element);
  }



  void non_recursive_add(mesh_t const & mesh, element_t const & element);
  void add(mesh_t const & mesh, element_t const & element);


}


#endif
