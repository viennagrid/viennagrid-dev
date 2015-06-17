#ifndef VIENNAGRID_ELEMENT_HPP
#define VIENNAGRID_ELEMENT_HPP

#include "viennagridpp/forwards.hpp"
#include "viennagridpp/mesh/mesh.hpp"

namespace viennagrid
{
  template<bool is_const>
  class base_element
  {
    typedef base_element<is_const> self_type;

    template<bool other_is_const>
    friend class base_element;

  public:

    typedef base_element<false> nonconst_element_type;
    typedef base_element<true> const_element_type;

    typedef viennagrid_int id_type;
    typedef element_tag_t tag_type;
    typedef viennagrid_dimension dimension_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy_t, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy_t, true>::type const_mesh_hierarchy_type;

    base_element() : mesh_hierarchy_(0), id_(-1) {}
    base_element(viennagrid_mesh_hierarchy mesh_hierarchy_in,
                 dimension_type topologic_dimension_in,
                 id_type id_in) :
                 mesh_hierarchy_(mesh_hierarchy_in),
                 topologic_dimension_(topologic_dimension_in),
                 id_(id_in) {}
    base_element(mesh_hierarchy_type const & mesh_hierarchy_in,
                 dimension_type topologic_dimension_in,
                 id_type id_in) :
                 mesh_hierarchy_(mesh_hierarchy_in.internal()),
                 topologic_dimension_(topologic_dimension_in),
                 id_(id_in) {}

    template<bool other_is_const>
    base_element(base_element<other_is_const> element) : mesh_hierarchy_(element.mesh_hierarchy_), topologic_dimension_(element.topologic_dimension_), id_(element.id_) {}

    id_type id() const { return id_; }
    bool valid() const { return id() >= 0; }

    mesh_hierarchy_type mesh_hierarchy()
    { return mesh_hierarchy_type(mesh_hierarchy_); }

    const_mesh_hierarchy_type mesh_hierarchy() const
    { return const_mesh_hierarchy_type(mesh_hierarchy_); }

    viennagrid_mesh_hierarchy internal_mesh_hierarchy() const { return const_cast<viennagrid_mesh_hierarchy>(mesh_hierarchy_); }

    tag_type tag() const
    {
      viennagrid_element_tag tag;
      viennagrid_element_tag_get( internal_mesh_hierarchy(), topologic_dimension(), id(), &tag);
      return tag_type(tag);
    }
    dimension_type topologic_dimension() const { return topologic_dimension_; }

  private:

    viennagrid_mesh_hierarchy mesh_hierarchy_;
    dimension_type topologic_dimension_;
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
  base_mesh_hierarchy<is_const> mesh_hierarchy( base_element<is_const> const & element )
  { return element.mesh_hierarchy(); }

  template<bool is_const>
  viennagrid_mesh_hierarchy internal_mesh_hierarchy( base_element<is_const> const & element )
  {
    return element.internal_mesh_hierarchy();
  }


  template<bool is_const>
  base_element<is_const> parent(base_element<is_const> const & element)
  {
    viennagrid_int parent_id;
    viennagrid_element_parent_get(internal_mesh_hierarchy(element),
                                  viennagrid::topologic_dimension(element),
                                  element.id(),
                                  &parent_id);
    return base_element<is_const>(element.mesh_hierarchy(), viennagrid::topologic_dimension(element), parent_id);
  }

  template<bool is_const>
  void set_parent(base_element<false> element, base_element<is_const> const & parent)
  {
    viennagrid_element_parent_set(internal_mesh_hierarchy(element),
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
