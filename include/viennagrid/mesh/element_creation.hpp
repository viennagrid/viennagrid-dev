#ifndef VIENNAGRID_ELEMENT_CREATION_HPP
#define VIENNAGRID_ELEMENT_CREATION_HPP

#include <vector>
#include "viennagrid/forwards.hpp"
#include "viennagrid/range.hpp"
#include "viennagrid/mesh/region.hpp"

namespace viennagrid
{

  inline void non_recursive_add_element_to_mesh(mesh_t mesh, element_t const & element)
  {
    viennagrid_element_add(mesh.internal_mesh(), element.tag().internal_element_tag(), element.id());
  }

  inline void add_element_to_mesh(mesh_t & mesh, element_t const & element)
  {
    non_recursive_add_element_to_mesh(mesh, element);

    typedef viennagrid::result_of::element_range<element_t>::type ElementRangeType;

    for (element_tag_t et = element_tag_t::vertex(); et != element.tag(); ++et)
    {
      ElementRangeType elements(element, et);
      for (ElementRangeType::iterator it = elements.begin(); it != elements.end(); ++it)
        non_recursive_add_element_to_mesh(mesh, *it);
    }
  }




  element_t make_vertex(mesh_hierarchy_t mesh_hierarchy,
                        viennagrid_numeric const * coords)
  {
    viennagrid_index id;
    viennagrid_vertex_make( mesh_hierarchy.internal_mesh_hierarchy(), coords, &id );
//     mesh_hierarchy.increment_change_counter();
    return element_t(viennagrid::element_tag_t::vertex(), mesh_hierarchy, id);
  }

  element_t make_vertex(mesh_t mesh,
                        viennagrid_numeric const * coords)
  {
    element_t vertex = make_vertex(mesh.mesh_hierarchy(), coords);
    non_recursive_add_element_to_mesh(mesh, vertex);
    return vertex;
  }


  template<typename SomethingT>
  element_t make_vertex(SomethingT something, viennagrid_numeric x)
  {
    return make_vertex(something, &x);
  }

  template<typename SomethingT>
  element_t make_vertex(SomethingT something, viennagrid_numeric x, viennagrid_numeric y)
  {
    viennagrid_numeric pt[2];
    pt[0] = x;
    pt[1] = y;
    return make_vertex(something, pt);
  }

  template<typename SomethingT>
  element_t make_vertex(SomethingT something, viennagrid_numeric x, viennagrid_numeric y, viennagrid_numeric z)
  {
    viennagrid_numeric pt[3];
    pt[0] = x;
    pt[1] = y;
    pt[2] = z;
    return make_vertex(something, pt);
  }




  template<typename VertexHandleIteratorT>
  element_t make_element(mesh_hierarchy_t mesh_hierarchy,
                         element_tag_t element_tag,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    std::vector<viennagrid_index> internal_vertices_indices;
    for (; vertices_begin != vertices_end; ++vertices_begin)
      internal_vertices_indices.push_back( (*vertices_begin).id() );

    viennagrid_index id;
    viennagrid_element_make(mesh_hierarchy.internal_mesh_hierarchy(),
                            element_tag.internal_element_tag(),
                            internal_vertices_indices.size(),
                            &internal_vertices_indices[0],
                            &id);

    return element_t(element_tag, mesh_hierarchy, id);
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element_t make_element(mesh_hierarchy_t mesh_hierarchy,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    return make_element(mesh_hierarchy, ElementTagT(), vertices_begin, vertices_end);
  }





  template<typename VertexHandleIteratorT>
  element_t make_element(mesh_t mesh,
                         element_tag_t element_tag,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    element_t element = make_element(mesh.mesh_hierarchy(), element_tag, vertices_begin, vertices_end);
    add_element_to_mesh(mesh, element);
    return element;
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element_t make_element(mesh_t mesh,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    return make_element(mesh, ElementTagT(), vertices_begin, vertices_end);
  }







  template<typename VertexHandleIteratorT>
  element_t make_element(mesh_region_t mesh_region,
                         element_tag_t element_tag,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    element_t element = make_element(mesh_region.mesh(), element_tag, vertices_begin, vertices_end);
    add(mesh_region, element);
    return element;
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element_t make_element(mesh_region_t mesh_region,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    return make_element(mesh_region, ElementTagT(), vertices_begin, vertices_end);
  }







  template<typename VertexHandleIteratorT>
  element_t make_cell(mesh_t mesh,
                      VertexHandleIteratorT vertices_begin,
                      VertexHandleIteratorT vertices_end)
  {
    return make_element(mesh, mesh.mesh_hierarchy().cell_tag(), vertices_begin, vertices_end);
  }


  template<typename SomethingT>
  element_t make_line(SomethingT something, element_t v0, element_t v1)
  {
    element_t v[2];
    v[0] = v0;
    v[1] = v1;
    return make_element<line_tag>(something, v, v+2);
  }

  template<typename SomethingT>
  element_t make_triangle(SomethingT something, element_t v0, element_t v1, element_t v2)
  {
    element_t v[3];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    return make_element<triangle_tag>(something, v, v+3);
  }

  template<typename SomethingT>
  element_t make_quadrilateral(SomethingT something, element_t v0, element_t v1, element_t v2, element_t v3)
  {
    element_t v[3];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;
    return make_element<quadrilateral_tag>(something, v, v+4);
  }

  template<typename SomethingT>
  element_t make_tetrahedron(SomethingT something, element_t v0, element_t v1, element_t v2, element_t v3)
  {
    element_t v[4];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;
    return make_element<tetrahedron_tag>(something, v, v+4);
  }


}

#endif
