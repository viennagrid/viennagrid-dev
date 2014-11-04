#ifndef VIENNAGRID_ELEMENT_HPP
#define VIENNAGRID_ELEMENT_HPP

#include "viennagrid/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"

namespace viennagrid
{
  template<bool is_const>
  class base_element_t
  {
    typedef base_element_t<is_const> self_type;

    template<bool other_is_const>
    friend class base_element_t;

  public:

    typedef base_element_t<false> nonconst_element_type;
    typedef base_element_t<true> const_element_type;

    typedef viennagrid_index id_type;
    typedef element_tag_t tag_type;

    typedef typename result_of::const_nonconst<mesh_hierarchy_t, is_const>::type mesh_hierarchy_type;
    typedef typename result_of::const_nonconst<mesh_hierarchy_t, true>::type const_mesh_hierarchy_type;

    base_element_t() : mesh_hierarchy_(0), id_(-1) {}
    base_element_t(tag_type element_tag_in, mesh_hierarchy_type mesh_hierarchy_in, viennagrid_index id_in) : mesh_hierarchy_(mesh_hierarchy_in), id_(id_in), element_tag_(element_tag_in) {}

    template<bool other_is_const>
    base_element_t(base_element_t<other_is_const> element) : mesh_hierarchy_(element.mesh_hierarchy_), id_(element.id_), element_tag_(element.element_tag_) {}

    id_type id() const { return id_; }
    mesh_hierarchy_type mesh_hierarchy() { return mesh_hierarchy_; }
    const_mesh_hierarchy_type mesh_hierarchy() const { return mesh_hierarchy_; }

    tag_type tag() const { return element_tag_; }

  private:
    mesh_hierarchy_type mesh_hierarchy_;
    id_type id_;
    tag_type element_tag_;
  };


  inline bool operator<(element_t const & lhs, element_t const & rhs)
  { return lhs.id() < rhs.id(); }
  inline bool operator<(const_element_t const & lhs, element_t const & rhs)
  { return lhs.id() < rhs.id(); }
  inline bool operator<(element_t const & lhs, const_element_t const & rhs)
  { return lhs.id() < rhs.id(); }
  inline bool operator<(const_element_t const & lhs, const_element_t const & rhs)
  { return lhs.id() < rhs.id(); }




  template<bool is_const>
  std::ostream& operator << (std::ostream & os, base_element_t<is_const> const & element)
  {
    typedef typename viennagrid::result_of::const_vertex_range< base_element_t<is_const> >::type ConstVertexRangeType;
    ConstVertexRangeType vertices(element);

    std::cout << element.tag().name() << " [ id = " << element.id() << " vertex-ids: ";

    typename ConstVertexRangeType::iterator vit = vertices.begin();
    std::cout << (*vit++).id();

    for (; vit != vertices.end(); ++vit)
      std::cout << ", " << (*vit).id();

    std::cout << " ]";
    return os;
  }





}

#endif
