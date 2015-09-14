#ifndef VIENNAGRID_ELEMENT_HPP
#define VIENNAGRID_ELEMENT_HPP

#include "viennagrid/core/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"

namespace viennagrid
{


  class element_id
  {
  public:

    element_id() : id(-1) {}
    element_id(viennagrid_element_id id_) : id(id_) {}
    element_id(viennagrid_dimension topological_dimension_, viennagrid_element_id element_index_) : id( viennagrid_compose_element_id(topological_dimension_,element_index_) ) {}

    bool valid() const { return id; }

    viennagrid_element_id internal() const { return id; }
    operator viennagrid_element_id() { return internal(); }


    viennagrid_dimension topological_dimension() const { return viennagrid_topological_dimension_from_element_id(id); }
    viennagrid_element_id index() const { return viennagrid_index_from_element_id(id); }


    bool operator<(element_id rhs) const { return internal() < rhs.internal(); }

    element_id & operator++() { ++id; return *this; }
    element_id operator++(int) { viennagrid_element_id tmp(*this); ++(*this); return tmp; }

  private:
    viennagrid_element_id id;
  };


  inline std::ostream & operator<<(std::ostream & stream, element_id id)
  {
    stream << id.internal() << " (dim=" << (int)id.topological_dimension() << " index=" << id.index() << ")";
    return stream;
  }





  template<bool is_const>
  class base_element
  {
    typedef base_element<is_const> self_type;

    template<bool other_is_const>
    friend class base_element;

  public:

    typedef base_element<false> nonconst_element_type;
    typedef base_element<true> const_element_type;

    typedef element_id id_type;
    typedef viennagrid_dimension dimension_type;

    typedef typename result_of::const_nonconst<mesh, is_const>::type mesh_type;
    typedef typename result_of::const_nonconst<mesh, true>::type const_mesh_type;

    base_element() : mesh_(0), id_(-1) {}
    base_element(viennagrid_mesh mesh_in,
                 id_type id_in) :
                 mesh_(mesh_in),
                 id_(id_in) {}
    base_element(mesh_type const & mesh_in,
                 id_type id_in) :
                 mesh_(mesh_in.internal()),
                 id_(id_in) {}

    template<bool other_is_const>
    base_element(base_element<other_is_const> element) : mesh_(element.mesh_), id_(element.id_) {}

    id_type id() const { return id_; }
    bool valid() const { return id() >= 0; }

    mesh_type get_mesh()
    { return mesh_type(mesh_); }

    const_mesh_type get_mesh() const
    { return const_mesh_type(mesh_); }

    viennagrid_mesh internal_mesh() const { return const_cast<viennagrid_mesh>(mesh_); }

    void * get_aux() const
    {
      void * aux;
      viennagrid_element_aux_get(mesh_, id_, &aux);
      return aux;
    }

    void set_aux(void * aux)
    {
      viennagrid_element_aux_set(mesh_, id_, aux);
    }

    element_tag tag() const
    {
      viennagrid_element_type type;
      viennagrid_element_type_get( internal_mesh(), id(), &type);
      return element_tag(type);
    }

    bool is_vertex() const { return tag().is_vertex(); }
    bool is_line() const { return tag().is_line(); }
    bool is_edge() const { return tag().is_edge(); }
    bool is_triangle() const { return tag().is_triangle(); }
    bool is_quadrilateral() const { return tag().is_quadrilateral(); }
    bool is_polygon() const { return tag().is_polygon(); }
    bool is_tetrahedron() const { return tag().is_tetrahedron(); }
    bool is_hexahedron() const { return tag().is_hexahedron(); }

    bool is_simplex() const { return tag().is_simplex(); }

    dimension_type topologic_dimension() const { return id_.topological_dimension(); }

  private:

    viennagrid_mesh mesh_;
    id_type id_;
  };


  template<bool lhs_is_const, bool rhs_is_const>
  bool operator==(base_element<lhs_is_const> const & lhs, base_element<rhs_is_const> const & rhs)
  { return (lhs.tag() == rhs.tag()) && (lhs.id() == rhs.id()); }

  template<bool lhs_is_const, bool rhs_is_const>
  bool operator!=(base_element<lhs_is_const> const & lhs, base_element<rhs_is_const> const & rhs)
  { return !(lhs == rhs); }



  template<bool lhs_is_const, bool rhs_is_const>
  bool operator<(base_element<lhs_is_const> const & lhs, base_element<rhs_is_const> const & rhs)
  {
    if (lhs.tag() < rhs.tag())
      return true;
    if (rhs.tag() < lhs.tag())
      return false;

    return lhs.id() < rhs.id();
  }



  template<bool is_const>
  viennagrid_int topologic_dimension( base_element<is_const> const & element )
  { return element.topologic_dimension(); }



  template<bool is_const>
  viennagrid_mesh internal_mesh( base_element<is_const> const & element )
  {
    return element.internal_mesh();
  }


  template<bool is_const>
  base_element<is_const> parent(base_element<is_const> const & element)
  {
    viennagrid_int parent_id;
    viennagrid_element_parent_get(internal_mesh_hierarchy(element),
                                  viennagrid::topologic_dimension(element),
                                  element.id(),
                                  &parent_id);
    return base_element<is_const>(element.get_mesh_hierarchy(), viennagrid::topologic_dimension(element), parent_id);
  }

  template<bool is_const>
  void set_parent(base_element<false> element, base_element<is_const> const & parent)
  {
    viennagrid_element_parent_set(internal_mesh(element),
                                  viennagrid::topologic_dimension(element),
                                  element.id(),
                                  parent.id());
  }






  template<bool is_const>
  std::ostream& operator << (std::ostream & os, base_element<is_const> const & element)
  {
    if (!element.valid())
    {
      os << "Invalid Element";
      return os;
    }

    os << element.tag().name() << " [id = " << element.id() << "] ";

    if (element.tag().is_vertex())
    {
      os << viennagrid::get_point(element);
    }
    else
    {
      typedef typename viennagrid::result_of::const_element_range< base_element<is_const> >::type ConstVertexRangeType;
      ConstVertexRangeType vertices(element, 0);

      os <<  "{";
      typename ConstVertexRangeType::iterator vit = vertices.begin();
      os << *vit++;

      for (; vit != vertices.end(); ++vit)
        os << ", " << *vit;
      os << " }";
    }


    return os;
  }









}

#endif
