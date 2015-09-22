#ifndef VIENNAGRID_MESH_HPP
#define VIENNAGRID_MESH_HPP

#include <map>

#include "viennagrid/core/forwards.hpp"
#include "viennagrid/core/point.hpp"
#include "viennagrid/core/iterators.hpp"

namespace viennagrid
{

  template<bool is_const>
  class base_mesh
  {
  public:

    typedef typename result_of::const_nonconst<region, is_const>::type region_type;
    typedef typename result_of::const_nonconst<region, true>::type const_region_type;
    typedef viennagrid_region_id region_id_type;

    typedef point point_type;

    typedef typename result_of::const_nonconst<element, is_const>::type element_type;
    typedef typename result_of::const_nonconst<element, true>::type const_element_type;


    base_mesh(viennagrid_mesh internal_mesh_in) : internal_mesh_(internal_mesh_in)
    {
      retain();
    }

    base_mesh() : internal_mesh_(0)
    {
      THROW_ON_ERROR( viennagrid_mesh_create(&internal_mesh_) );
      THROW_ON_ERROR( viennagrid_mesh_property_set(internal_mesh_, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_SPARSE) );
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

    viennagrid_mesh internal() const { return const_cast<viennagrid_mesh>(internal_mesh_); }

    base_mesh<is_const> get_root()
    {
      viennagrid_mesh root;
      THROW_ON_ERROR( viennagrid_mesh_root_mesh_get(internal(), &root) );
      return base_mesh<is_const>(root);
    }

    const_mesh get_root() const
    {
      viennagrid_mesh root;
      THROW_ON_ERROR( viennagrid_mesh_root_mesh_get(internal(), &root) );
      return const_mesh(root);
    }

    bool is_root() const
    {
      viennagrid_bool value;
      THROW_ON_ERROR( viennagrid_mesh_is_root(internal(), &value) );
      return value == VIENNAGRID_TRUE;
    }

    bool operator==(base_mesh<is_const> const & rhs) const { return internal() == rhs.internal(); }
    bool operator!=(base_mesh<is_const> const & rhs) const { return !(*this == rhs); }

    std::size_t region_count() const;
    region_type get_or_create_region(region_id_type region_id) const;
    region_type create_region() const;
    const_region_type get_region(region_id_type region_id) const;
    bool region_exists(region_id_type region_id) const;

    region_type get_or_create_region(std::string const & name) const;
    const_region_type get_region(std::string const & name) const;
    bool region_exists(std::string const & name) const;


    void serialize(void ** buffer, viennagrid_int * size)
    {
      THROW_ON_ERROR( viennagrid_mesh_serialize(internal(), buffer, size) );
    }

    void deserialize(void * buffer, viennagrid_int size)
    {
      THROW_ON_ERROR( viennagrid_mesh_deserialize(buffer, size, internal()) );
    }

    void optimize_memory()
    {
      THROW_ON_ERROR( viennagrid_mesh_memory_optimize(internal()) );
    }

    long memory_size() const
    {
      long size;
      THROW_ON_ERROR( viennagrid_mesh_memory_size(internal(), &size) );
      return size;
    }



    std::string name() const
    {
      const char * name_;
      THROW_ON_ERROR( viennagrid_mesh_name_get(internal(), &name_) );
      return name_;
    }

    void set_name(std::string const & name_)
    {
      THROW_ON_ERROR( viennagrid_mesh_name_set(internal(), name_.c_str()) );
    }

  private:

    void retain() const
    {
      if (internal())
        THROW_ON_ERROR( viennagrid_mesh_retain(internal()) );
    }

    void release() const
    {
      if (internal())
        THROW_ON_ERROR( viennagrid_mesh_release(internal()) );
    }

    viennagrid_mesh internal_mesh_;
  };




  inline viennagrid_mesh internal_mesh(viennagrid_mesh mesh)
  {
    return mesh;
  }

  template<bool is_const>
  viennagrid_mesh internal_mesh( base_mesh<is_const> const & mesh )
  {
    return mesh.internal();
  }






  template<bool lhs_is_const, bool rhs_is_const>
  bool operator==(base_mesh<lhs_is_const> const & lhs, base_mesh<rhs_is_const> const & rhs)
  { return lhs.internal() == rhs.internal(); }

  template<bool lhs_is_const, bool rhs_is_const>
  bool operator!=(base_mesh<lhs_is_const> const & lhs, base_mesh<rhs_is_const> const & rhs)
  { return !(lhs == rhs); }



  template<bool is_const>
  viennagrid_int element_count(base_mesh<is_const> const & mesh, viennagrid_dimension topological_dimension)
  {
    viennagrid_int count;
    THROW_ON_ERROR( viennagrid_mesh_element_count( internal_mesh(mesh), topological_dimension, &count) );
    return count;
  }


  template<bool is_const>
  viennagrid_int element_count(base_mesh<is_const> const & mesh, element_tag et)
  {
    viennagrid_int count;
    THROW_ON_ERROR( viennagrid_mesh_element_count_by_type( internal_mesh(mesh), et.internal(), &count) );
    return count;
  }

  template<bool is_const>
  viennagrid_int vertex_count(base_mesh<is_const> const & mesh)
  { return element_count( mesh, viennagrid::vertex_tag() ); }

  template<bool is_const>
  viennagrid_int line_count(base_mesh<is_const> const & mesh)
  { return element_count( mesh, viennagrid::line_tag() ); }

  template<bool is_const>
  viennagrid_int edge_count(base_mesh<is_const> const & mesh)
  { return element_count( mesh, viennagrid::edge_tag() ); }

  template<bool is_const>
  viennagrid_int triangle_count(base_mesh<is_const> const & mesh)
  { return element_count( mesh, viennagrid::triangle_tag() ); }

  template<bool is_const>
  viennagrid_int quadrilateral_count(base_mesh<is_const> const & mesh)
  { return element_count( mesh, viennagrid::quadrilateral_tag() ); }

  template<bool is_const>
  viennagrid_int polygon_count(base_mesh<is_const> const & mesh)
  { return element_count( mesh, viennagrid::polygon_tag() ); }

  template<bool is_const>
  viennagrid_int tetrahedron_count(base_mesh<is_const> const & mesh)
  { return element_count( mesh, viennagrid::tetrahedron_tag() ); }

  template<bool is_const>
  viennagrid_int hexahedron_count(base_mesh<is_const> const & mesh)
  { return element_count( mesh, viennagrid::hexahedron_tag() ); }






  inline void clear(viennagrid_mesh mesh)
  {
    THROW_ON_ERROR( viennagrid_mesh_clear(mesh) );
  }

  template<typename SomethingT>
  void clear(SomethingT const & something)
  {
    clear( internal_mesh(something) );
  }


  inline viennagrid_dimension geometric_dimension(viennagrid_mesh mesh)
  {
    viennagrid_dimension tmp;
    THROW_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &tmp) );
    return tmp;
  }

  template<typename SomethingT>
  viennagrid_dimension geometric_dimension(SomethingT const & something)
  {
    return geometric_dimension( internal_mesh(something) );
  }



  inline viennagrid_dimension topologic_dimension(viennagrid_mesh mesh)
  {
    viennagrid_dimension tmp;
    THROW_ON_ERROR( viennagrid_mesh_cell_dimension_get(mesh, &tmp) );
    return tmp;
  }


  template<typename SomethingT>
  viennagrid_dimension topologic_dimension( SomethingT const & something )
  {
    return topologic_dimension(internal_mesh(something));
  }



  template<typename SomethingT>
  viennagrid_dimension cell_dimension( SomethingT const & something )
  {
    return topologic_dimension(internal_mesh(something));
  }

  template<typename SomethingT>
  viennagrid_dimension facet_dimension( SomethingT const & something )
  {
    return topologic_dimension(internal_mesh(something)) - 1;
  }




  template<bool element_is_const>
  point get_point(viennagrid_mesh mesh, base_element<element_is_const> const & vertex);

  template<typename SomethingT, bool element_is_const>
  point get_point(SomethingT const & something, base_element<element_is_const> const & vertex)
  {
    return get_point(internal_mesh(something), vertex);
  }

  template<bool element_is_const>
  point get_point(base_element<element_is_const> const & vertex);

  template<bool element_is_const>
  point get_point(base_element<element_is_const> const & element, viennagrid_int index);

  void set_point(viennagrid_mesh mesh, base_element<false> const & vertex, point const & p);

  template<typename SomethingT>
  void set_point(SomethingT const & something, base_element<false> const & vertex, point const & p)
  {
    set_point(internal_mesh(something), vertex, p);
  }

  void set_point(base_element<false> const & vertex, point const & p);








  template<bool mesh_is_const, bool element_is_const>
  bool is_boundary( base_mesh<mesh_is_const> const & mesh, base_element<element_is_const> const & element );

  template<bool element_is_const>
  bool is_any_boundary(base_element<element_is_const> const & element);

  template<typename SomethingFirstT, typename SomethingSecondT, typename ElementT>
  bool is_interface( SomethingFirstT const & first, SomethingSecondT const & second, ElementT const & element )
  {
    return is_boundary(first, element) && is_boundary(second, element);
  }

}


#endif
