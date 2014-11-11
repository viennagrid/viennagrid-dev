#ifndef VIENNAGRID_ELEMENT_CREATION_HPP
#define VIENNAGRID_ELEMENT_CREATION_HPP

#include <vector>
#include "viennagrid/forwards.hpp"
#include "viennagrid/range.hpp"
#include "viennagrid/mesh/region.hpp"

namespace viennagrid
{

  inline void non_recursive_add_element_to_mesh(mesh m, element const & e)
  {
    viennagrid_element_add(m.internal_mesh(), e.tag().internal_element_tag(), e.id());
  }

  inline void add_element_to_mesh(mesh & m, element const & e)
  {
    non_recursive_add_element_to_mesh(m, e);

    typedef viennagrid::result_of::element_range<element>::type ElementRangeType;

    for (element_tag et = element_tag::vertex(); et != m.unpack_element_tag(e.tag()); ++et)
    {
      ElementRangeType elements(e, et);
      for (ElementRangeType::iterator it = elements.begin(); it != elements.end(); ++it)
        non_recursive_add_element_to_mesh(m, *it);
    }
  }




  element make_vertex(mesh_hierarchy mesh_hierarchy,
                      viennagrid_numeric const * coords)
  {
    viennagrid_index id;
    viennagrid_vertex_create( mesh_hierarchy.internal_mesh_hierarchy(), coords, &id );
    return element(viennagrid::element_tag::vertex(), mesh_hierarchy, id);
  }

  element make_vertex(mesh m,
                      viennagrid_numeric const * coords)
  {
    element vertex = make_vertex(m.get_mesh_hierarchy(), coords);
    non_recursive_add_element_to_mesh(m, vertex);
    return vertex;
  }


  template<typename SomethingT>
  element make_vertex(SomethingT something, viennagrid_numeric x)
  {
    return make_vertex(something, &x);
  }

  template<typename SomethingT>
  element make_vertex(SomethingT something, viennagrid_numeric x, viennagrid_numeric y)
  {
    viennagrid_numeric pt[2];
    pt[0] = x;
    pt[1] = y;
    return make_vertex(something, pt);
  }

  template<typename SomethingT>
  element make_vertex(SomethingT something, viennagrid_numeric x, viennagrid_numeric y, viennagrid_numeric z)
  {
    viennagrid_numeric pt[3];
    pt[0] = x;
    pt[1] = y;
    pt[2] = z;
    return make_vertex(something, pt);
  }




  template<typename VertexHandleIteratorT>
  element make_element(mesh_hierarchy mesh_hierarchy,
                       element_tag tag,
                       VertexHandleIteratorT vertices_begin,
                       VertexHandleIteratorT vertices_end)
  {
    std::vector<viennagrid_index> internal_vertices_indices;
    for (; vertices_begin != vertices_end; ++vertices_begin)
      internal_vertices_indices.push_back( (*vertices_begin).id() );

    viennagrid_index id;
    viennagrid_element_create(mesh_hierarchy.internal_mesh_hierarchy(),
                              tag.internal_element_tag(),
                              internal_vertices_indices.size(),
                              &internal_vertices_indices[0],
                              &id);

    return element(tag, mesh_hierarchy, id);
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element make_element(mesh_hierarchy mh,
                       VertexHandleIteratorT vertices_begin,
                       VertexHandleIteratorT vertices_end)
  {
    return make_element(mh, ElementTagT(), vertices_begin, vertices_end);
  }





  template<typename VertexHandleIteratorT>
  element make_element(mesh m,
                       element_tag tag,
                       VertexHandleIteratorT vertices_begin,
                       VertexHandleIteratorT vertices_end)
  {
    element e = make_element(m.get_mesh_hierarchy(), tag, vertices_begin, vertices_end);
    add_element_to_mesh(m, e);
    return e;
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element make_element(mesh mesh,
                       VertexHandleIteratorT vertices_begin,
                       VertexHandleIteratorT vertices_end)
  {
    return make_element(mesh, ElementTagT(), vertices_begin, vertices_end);
  }







  template<typename VertexHandleIteratorT>
  element make_element(mesh_region mr,
                       element_tag tag,
                       VertexHandleIteratorT vertices_begin,
                       VertexHandleIteratorT vertices_end)
  {
    element e = make_element(mr.get_mesh(), tag, vertices_begin, vertices_end);
    add(mr, e);
    return e;
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element make_element(mesh_region mr,
                       VertexHandleIteratorT vertices_begin,
                       VertexHandleIteratorT vertices_end)
  {
    return make_element(mr, ElementTagT(), vertices_begin, vertices_end);
  }







  template<typename VertexHandleIteratorT>
  element make_cell(mesh m,
                    VertexHandleIteratorT vertices_begin,
                    VertexHandleIteratorT vertices_end)
  {
    return make_element(m, m.cell_tag(), vertices_begin, vertices_end);
  }


  template<typename SomethingT>
  element make_line(SomethingT something, element v0, element v1)
  {
    element v[2];
    v[0] = v0;
    v[1] = v1;
    return make_element<line_tag>(something, v, v+2);
  }

  template<typename SomethingT>
  element make_triangle(SomethingT something, element v0, element v1, element v2)
  {
    element v[3];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    return make_element<triangle_tag>(something, v, v+3);
  }

  template<typename SomethingT>
  element make_quadrilateral(SomethingT something, element v0, element v1, element v2, element v3)
  {
    element v[3];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;
    return make_element<quadrilateral_tag>(something, v, v+4);
  }

  template<typename SomethingT>
  element make_tetrahedron(SomethingT something, element v0, element v1, element v2, element v3)
  {
    element v[4];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;
    return make_element<tetrahedron_tag>(something, v, v+4);
  }


}

#endif
