#ifndef VIENNAGRID_ELEMENT_CREATION_HPP
#define VIENNAGRID_ELEMENT_CREATION_HPP

#include <vector>
#include "viennagridpp/forwards.hpp"
#include "viennagridpp/range.hpp"
#include "viennagridpp/mesh/region.hpp"

#include "viennagridpp/algorithm/detail/numeric.hpp"

namespace viennagrid
{
  inline element make_vertex(viennagrid_mesh mesh,
                             point const & p)
  {
    viennagrid_mesh_hierarchy mesh_hierarchy;
    viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);

    if ( viennagrid::geometric_dimension(mesh_hierarchy) <= 0 && !p.empty() )
      viennagrid_mesh_hierarchy_geometric_dimension_set( mesh_hierarchy, p.size() );

    viennagrid_int id;
    viennagrid_mesh_vertex_create( mesh, &p[0], &id );
    return element(mesh_hierarchy, 0, id);
  }


  inline element make_vertex(mesh const & m,
                             point const & p)
  {
    return make_vertex( m.internal(), p );
  }

  inline element make_vertex(mesh const & m)
  {
    return make_vertex( m.internal(), point() );
  }


  inline element make_vertex(mesh_region const & mr, point const & point)
  {
    element vertex = make_vertex(mr.internal_mesh(), point);
    add(mr, vertex);
    return vertex;
  }

  inline element make_vertex(mesh_region const & mr)
  {
    element vertex = make_vertex(mr.internal_mesh(), point());
    add(mr, vertex);
    return vertex;
  }





  template<typename SomethingT, typename NumericConfigT>
  typename result_of::element<SomethingT>::type make_unique_vertex(
        SomethingT something,
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
        if ( detail::is_equal(nc, point, viennagrid::get_point(something, *vit)) )
          return *vit;
      }
    }

    return make_vertex(something, point);
  }









  template<typename ElementIteratorT>
  element make_element(viennagrid_mesh mesh,
                       element_tag et,
                       ElementIteratorT elements_begin,
                       ElementIteratorT elements_end)
  {
    viennagrid_mesh_hierarchy mesh_hierarchy;
    viennagrid_mesh_mesh_hierarchy_get(mesh, &mesh_hierarchy);

    std::vector<viennagrid_int> internal_vertices_indices;

    for (; elements_begin != elements_end; ++elements_begin)
    {
      internal_vertices_indices.push_back( (*elements_begin).id() );
    }

    viennagrid_int id;
    viennagrid_mesh_element_create(mesh,
                                   et.internal(),
                                   internal_vertices_indices.size(),
                                   &internal_vertices_indices[0],
                                   &id);

    return element(mesh_hierarchy, et.topologic_dimension(), id);
  }




  template<typename ElementIteratorT>
  element make_element(mesh const & m,
                       element_tag et,
                       ElementIteratorT elements_begin,
                       ElementIteratorT elements_end)
  {
    return make_element(m.internal(), et, elements_begin, elements_end);
  }

  template<typename ElementTagT, typename ElementIteratorT>
  element make_element(mesh const & m,
                       ElementIteratorT elements_begin,
                       ElementIteratorT elements_end)
  {
    return make_element(m.internal(), ElementTagT(), elements_begin, elements_end);
  }





  template<typename ElementIteratorT>
  element make_element(mesh_region const & mr,
                       element_tag et,
                       ElementIteratorT elements_begin,
                       ElementIteratorT elements_end)
  {
    element element = make_element(mr.internal_mesh(), et, elements_begin, elements_end);
    add(mr, element);
    return element;
  }

  template<typename ElementTagT, typename ElementIteratorT>
  element make_element(mesh_region const & mr,
                       ElementIteratorT elements_begin,
                       ElementIteratorT elements_end)
  {
    element element = make_element(mr.internal_mesh(), ElementTagT(), elements_begin, elements_end);
    add(mr, element);
    return element;
  }






  template<typename ElementIteratorT, typename IntersectionIteratorT>
  element make_refined_element(mesh const & m,
                               element parent,
                               element_tag et,
                               ElementIteratorT elements_begin,
                               ElementIteratorT elements_end,
                               IntersectionIteratorT intersects_begin,
                               IntersectionIteratorT intersects_end)
  {
    std::vector<viennagrid_int> internal_vertices_indices;

    for (; elements_begin != elements_end; ++elements_begin)
    {
      internal_vertices_indices.push_back( (*elements_begin).id() );
    }

    std::vector<viennagrid_int> vertex_indices;
    std::vector<viennagrid_dimension> intersects_dimensions;
    std::vector<viennagrid_int> intersects_indices;

    for (; intersects_begin != intersects_end; ++intersects_begin)
    {
      vertex_indices.push_back( (intersects_begin->first).id() );
      intersects_dimensions.push_back( viennagrid::topologic_dimension(intersects_begin->second) );
      intersects_indices.push_back( (intersects_begin->second).id() );
    }


    viennagrid_int id;
    viennagrid_mesh_refined_element_create(m.internal(),
                                           viennagrid::topologic_dimension(parent),
                                           parent.id(),
                                           et.internal(),
                                           internal_vertices_indices.size(),
                                           &internal_vertices_indices[0],
                                           vertex_indices.size(),
                                           &vertex_indices[0],
                                           &intersects_indices[0], &intersects_dimensions[0],
                                           &id);

    return element(m.get_mesh_hierarchy(), et.topologic_dimension(), id);
  }







  template<typename SomethingT>
  element make_line(SomethingT const & something, element v0, element v1)
  {
    element v[2];
    v[0] = v0;
    v[1] = v1;
    return make_element<line_tag>(something, v, v+2);
  }

  template<typename SomethingT>
  element make_triangle(SomethingT const & something, element v0, element v1, element v2)
  {
    element v[3];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    return make_element<triangle_tag>(something, v, v+3);
  }

  template<typename SomethingT>
  element make_quadrilateral(SomethingT const & something, element v0, element v1, element v2, element v3)
  {
    element v[4];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;
    return make_element<quadrilateral_tag>(something, v, v+4);
  }

  template<typename SomethingT>
  element make_tetrahedron(SomethingT const & something, element v0, element v1, element v2, element v3)
  {
    element v[4];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;
    return make_element<tetrahedron_tag>(something, v, v+4);
  }

  template<typename SomethingT>
  element make_hexahedron(SomethingT const & something, element v0, element v1, element v2, element v3,
                                                        element v4, element v5, element v6, element v7)
  {
    element v[8];
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;
    v[4] = v4;
    v[5] = v5;
    v[6] = v6;
    v[7] = v7;
    return make_element<hexahedron_tag>(something, v, v+8);
  }

}

#endif
