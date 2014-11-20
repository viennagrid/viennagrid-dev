#ifndef VIENNAGRID_ELEMENT_HPP
#define VIENNAGRID_ELEMENT_HPP

#include "viennagrid/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"

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

    typedef viennagrid_index id_type;
    typedef element_tag_t tag_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy_t, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy_t, true>::type const_mesh_hierarchy_type;

    base_element() : mesh_hierarchy_(0), id_(-1) {}
    base_element(mesh_hierarchy_type mesh_hierarchy_in,
                 viennagrid_int topologic_dimension_in,
                 viennagrid_index id_in) :
                 mesh_hierarchy_(mesh_hierarchy_in),
                 topologic_dimension_(topologic_dimension_in),
                 id_(id_in) {}

    template<bool other_is_const>
    base_element(base_element<other_is_const> element) : mesh_hierarchy_(element.mesh_hierarchy_), topologic_dimension_(element.topologic_dimension_), id_(element.id_) {}

    id_type id() const { return id_; }
    mesh_hierarchy_type mesh_hierarchy() { return mesh_hierarchy_; }
    const_mesh_hierarchy_type mesh_hierarchy() const { return mesh_hierarchy_; }

    tag_type tag() const
    {
      viennagrid_element_tag tag;
      viennagrid_element_get_tag( mesh_hierarchy().internal(), topologic_dimension(), id(), &tag);
      return tag_type(tag);
    }
    viennagrid_int topologic_dimension() const { return topologic_dimension_; }

  private:
    mesh_hierarchy_type mesh_hierarchy_;
    viennagrid_int topologic_dimension_;
    id_type id_;
  };


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
  base_mesh_hierarchy<is_const> mesh_hierarchy( base_element<is_const> element )
  { return element.mesh_hierarchy(); }






  template<bool is_const>
  std::ostream& operator << (std::ostream & os, base_element<is_const> const & element)
  {
    std::cout << element.tag().name() << " [id = " << element.id() << "] ";

    if (element.tag().is_vertex())
    {
      std::cout << viennagrid::get_point(element);
    }
    else if (element.tag().is_plc())
    {
      typedef typename viennagrid::result_of::const_element_range< base_element<is_const> >::type ConstLineRangeType;
      ConstLineRangeType lines(element, 1);

      std::cout <<  "{";
      typename ConstLineRangeType::iterator lit = lines.begin();
      std::cout << *lit++;

      for (; lit != lines.end(); ++lit)
        std::cout << ", " << *lit;
      std::cout << " }";
    }
    else
    {
      typedef typename viennagrid::result_of::const_element_range< base_element<is_const> >::type ConstVertexRangeType;
      ConstVertexRangeType vertices(element, 0);

      std::cout <<  "{";
      typename ConstVertexRangeType::iterator vit = vertices.begin();
      std::cout << *vit++;

      for (; vit != vertices.end(); ++vit)
        std::cout << ", " << *vit;
      std::cout << " }";
    }


    return os;
  }









}

#endif
