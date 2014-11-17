#ifndef VIENNAGRID_ELEMENT_CREATION_HPP
#define VIENNAGRID_ELEMENT_CREATION_HPP

#include <vector>
#include "viennagrid/forwards.hpp"
#include "viennagrid/range.hpp"
#include "viennagrid/mesh/region.hpp"

#include "viennagrid/algorithm/detail/numeric.hpp"

namespace viennagrid
{

  inline void non_recursive_add_element_to_mesh(mesh_t m, element_t const & e)
  {
    viennagrid_element_add(m.internal(), e.tag().internal(), e.id());
  }

  inline void add_element_to_mesh(mesh_t & m, element_t const & e)
  {
    non_recursive_add_element_to_mesh(m, e);

    typedef viennagrid::result_of::element_range<element_t>::type ElementRangeType;

    for (element_tag_t et = element_tag_t::vertex(); et != m.unpack_element_tag(e.tag()); ++et)
    {
      ElementRangeType elements(e, et);
      for (ElementRangeType::iterator it = elements.begin(); it != elements.end(); ++it)
        non_recursive_add_element_to_mesh(m, *it);
    }
  }




  element_t make_vertex(mesh_hierarchy_t mesh_hierarchy,
                        viennagrid_numeric const * coords)
  {
    viennagrid_index id;
    viennagrid_vertex_create( mesh_hierarchy.internal(), coords, &id );
    return element_t(viennagrid::vertex_tag(), mesh_hierarchy, id);
  }

  element_t make_vertex(mesh_hierarchy_t mesh_hierarchy)
  {
    return make_vertex(mesh_hierarchy, 0);
  }

  element_t make_vertex(mesh_t mesh,
                        viennagrid_numeric const * coords)
  {
    element_t vertex = make_vertex(mesh.mesh_hierarchy(), coords);
    non_recursive_add_element_to_mesh(mesh, vertex);
    return vertex;
  }

  element_t make_vertex(mesh_t mesh)
  {
    return make_vertex(mesh, 0);
  }


  template<typename SomethingT>
  typename result_of::vertex<SomethingT>::type make_vertex(SomethingT something, point_t const & point)
  {
    assert( (viennagrid_int)point.size() == something.dimension() );
    return make_vertex(something, &point[0]);
  }


  template<typename SomethingT>
  typename result_of::vertex<SomethingT>::type make_vertex(SomethingT something, viennagrid_numeric x)
  {
    typename viennagrid::result_of::point<SomethingT>::type pt(1);
    pt[0] = x;
    return make_vertex(something, pt);
  }

  template<typename SomethingT>
  typename result_of::vertex<SomethingT>::type make_vertex(SomethingT something, viennagrid_numeric x, viennagrid_numeric y)
  {
    typename viennagrid::result_of::point<SomethingT>::type pt(2);
    pt[0] = x;
    pt[1] = y;
    return make_vertex(something, pt);
  }

  template<typename SomethingT>
  typename result_of::vertex<SomethingT>::type make_vertex(SomethingT something, viennagrid_numeric x, viennagrid_numeric y, viennagrid_numeric z)
  {
    typename viennagrid::result_of::point<SomethingT>::type pt(3);
    pt[0] = x;
    pt[1] = y;
    pt[2] = z;
    return make_vertex(something, pt);
  }




  template<typename SomethingT, typename NumericConfigT>
  typename result_of::vertex<SomethingT>::type make_unique_vertex(
        SomethingT & something,
        typename result_of::point<SomethingT>::type const & point,
        NumericConfigT nc)
  {
    typedef typename result_of::const_vertex_range<SomethingT>::type VertexRangeType;
    typedef typename result_of::iterator<VertexRangeType>::type VertexRangeIterator;

    if (nc > 0)
    {
      VertexRangeType vertices(something);
      for (VertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
      {
        if ( detail::is_equal_point(point, viennagrid::get_point(something, *vit), nc) )
          return *vit;
      }
    }

    return make_vertex(something, point);
  }

  template<typename SomethingT, typename NumericConfigT>
  typename result_of::vertex<SomethingT>::type make_unique_vertex(SomethingT something, viennagrid_numeric x, NumericConfigT nc)
  {
    typename viennagrid::result_of::point<SomethingT>::type pt(1);
    pt[1] = x;
    return make_unique_vertex(something, pt, nc);
  }

  template<typename SomethingT, typename NumericConfigT>
  typename result_of::vertex<SomethingT>::type make_unique_vertex(SomethingT something, viennagrid_numeric x, viennagrid_numeric y, NumericConfigT nc)
  {
    typename viennagrid::result_of::point<SomethingT>::type pt(2);
    pt[0] = x;
    pt[1] = y;
    return make_unique_vertex(something, pt, nc);
  }

  template<typename SomethingT, typename NumericConfigT>
  typename result_of::vertex<SomethingT>::type make_unique_vertex(SomethingT something, viennagrid_numeric x, viennagrid_numeric y, viennagrid_numeric z, NumericConfigT nc)
  {
    typename viennagrid::result_of::point<SomethingT>::type pt(3);
    pt[0] = x;
    pt[1] = y;
    pt[2] = z;
    return make_unique_vertex(something, pt, nc);
  }














  template<typename VertexHandleIteratorT>
  element_t make_element(mesh_hierarchy_t mesh_hierarchy,
                         element_tag_t tag,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    std::vector<viennagrid_index> internal_vertices_indices;
    for (; vertices_begin != vertices_end; ++vertices_begin)
      internal_vertices_indices.push_back( (*vertices_begin).id() );

    viennagrid_index id;
    viennagrid_element_create(mesh_hierarchy.internal(),
                              tag.internal(),
                              internal_vertices_indices.size(),
                              &internal_vertices_indices[0],
                              &id);

    return element_t(tag, mesh_hierarchy, id);
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element_t make_element(mesh_hierarchy_t mh,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    return make_element(mh, ElementTagT(), vertices_begin, vertices_end);
  }





  template<typename VertexHandleIteratorT>
  element_t make_element(mesh_t m,
                         element_tag_t tag,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    element_t e = make_element(m.mesh_hierarchy(), tag, vertices_begin, vertices_end);
    add_element_to_mesh(m, e);
    return e;
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element_t make_element(mesh_t mesh,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    return make_element(mesh, ElementTagT(), vertices_begin, vertices_end);
  }









  element_t make_vertex(mesh_region_t mr,
                        viennagrid_numeric const * coords)
  {
    element_t vertex = make_vertex(mr.mesh(), coords);
    add(mr, vertex);
    return vertex;
  }

  element_t make_vertex(mesh_region_t mr)
  {
    return make_vertex(mr, 0);
  }



  template<typename VertexHandleIteratorT>
  element_t make_element(mesh_region_t mr,
                         element_tag_t tag,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    element_t element = make_element(mr.mesh(), tag, vertices_begin, vertices_end);
    add(mr, element);
    return element;
  }

  template<typename ElementTagT, typename VertexHandleIteratorT>
  element_t make_element(mesh_region_t mr,
                         VertexHandleIteratorT vertices_begin,
                         VertexHandleIteratorT vertices_end)
  {
    return make_element(mr, ElementTagT(), vertices_begin, vertices_end);
  }







  template<typename VertexHandleIteratorT>
  element_t make_cell(mesh_hierarchy_t m,
                      VertexHandleIteratorT vertices_begin,
                      VertexHandleIteratorT vertices_end)
  {
    return make_element(m, m.cell_tag(), vertices_begin, vertices_end);
  }

  template<typename VertexHandleIteratorT>
  element_t make_cell(mesh_t m,
                      VertexHandleIteratorT vertices_begin,
                      VertexHandleIteratorT vertices_end)
  {
    return make_element(m, m.cell_tag(), vertices_begin, vertices_end);
  }

  template<typename VertexHandleIteratorT>
  element_t make_cell(mesh_region_t mr,
                      VertexHandleIteratorT vertices_begin,
                      VertexHandleIteratorT vertices_end)
  {
    return make_element(mr, mr.cell_tag(), vertices_begin, vertices_end);
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
