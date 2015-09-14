#include "element_buffer.hpp"
#include "mesh_hierarchy.hpp"

void viennagrid_element_buffer::reserve_boundary(viennagrid_element_id element_id)
{
  viennagrid_element_type type = element_type(element_id);

  switch (type)
  {
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
    case VIENNAGRID_ELEMENT_TYPE_LINE:
    {
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    {
      boundary_ids[1].push_back(3);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
    {
      boundary_ids[1].push_back(4);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    {
      boundary_ids[1].push_back(6);
      boundary_ids[2].push_back(4);
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
    {
      boundary_ids[1].push_back(12);
      boundary_ids[2].push_back(6);
      break;
    }

    default:
      assert(false);
  }
}


void viennagrid_element_buffer::make_boundary(viennagrid_element_id element_id, viennagrid_mesh mesh)
{
  viennagrid_element_type type = element_type(element_id);

  viennagrid_element_id * vertex_ids = boundary_ids[0].begin(INDEX(element_id));

  switch (type)
  {
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
    case VIENNAGRID_ELEMENT_TYPE_LINE:
    {
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    {
      assert(boundary_ids[0].end(INDEX(element_id)) - vertex_ids == 3);
      viennagrid_element_id * line_ids = boundary_begin(element_id, 1);

      line_ids[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ids[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ids[2] = mesh_hierarchy->get_make_line(vertex_ids, 1, 2, mesh);

      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
    {
      assert(boundary_ids[0].end(INDEX(element_id)) - vertex_ids == 4);
      viennagrid_element_id * line_ids = boundary_begin(element_id, 1);

      line_ids[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ids[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ids[2] = mesh_hierarchy->get_make_line(vertex_ids, 1, 3, mesh);
      line_ids[3] = mesh_hierarchy->get_make_line(vertex_ids, 2, 3, mesh);

      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    {
      assert(boundary_ids[0].end(INDEX(element_id)) - vertex_ids == 4);
      viennagrid_element_id * line_ids = boundary_begin(element_id, 1);
      viennagrid_element_id * triangle_ids = boundary_begin(element_id, 2);

      line_ids[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ids[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ids[2] = mesh_hierarchy->get_make_line(vertex_ids, 0, 3, mesh);
      line_ids[3] = mesh_hierarchy->get_make_line(vertex_ids, 1, 2, mesh);
      line_ids[4] = mesh_hierarchy->get_make_line(vertex_ids, 1, 3, mesh);
      line_ids[5] = mesh_hierarchy->get_make_line(vertex_ids, 2, 3, mesh);

      triangle_ids[0] = mesh_hierarchy->get_make_triangle(vertex_ids, 0, 1, 2, line_ids, 0, 1, 3, mesh);
      triangle_ids[1] = mesh_hierarchy->get_make_triangle(vertex_ids, 0, 1, 3, line_ids, 0, 2, 4, mesh);
      triangle_ids[2] = mesh_hierarchy->get_make_triangle(vertex_ids, 0, 2, 3, line_ids, 1, 3, 5, mesh);
      triangle_ids[3] = mesh_hierarchy->get_make_triangle(vertex_ids, 1, 2, 3, line_ids, 3, 4, 5, mesh);

      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
    {
      assert(boundary_ids[0].end(INDEX(element_id)) - vertex_ids == 8);
      viennagrid_element_id * line_ptr = boundary_begin(element_id, 1);
      viennagrid_element_id * quad_ptr = boundary_begin(element_id, 2);


      line_ptr[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ptr[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ptr[2] = mesh_hierarchy->get_make_line(vertex_ids, 0, 4, mesh);
      line_ptr[3] = mesh_hierarchy->get_make_line(vertex_ids, 1, 3, mesh);
      line_ptr[4] = mesh_hierarchy->get_make_line(vertex_ids, 1, 5, mesh);
      line_ptr[5] = mesh_hierarchy->get_make_line(vertex_ids, 2, 3, mesh);
      line_ptr[6] = mesh_hierarchy->get_make_line(vertex_ids, 2, 5, mesh);
      line_ptr[7] = mesh_hierarchy->get_make_line(vertex_ids, 3, 7, mesh);
      line_ptr[8] = mesh_hierarchy->get_make_line(vertex_ids, 4, 5, mesh);
      line_ptr[9] = mesh_hierarchy->get_make_line(vertex_ids, 4, 6, mesh);
      line_ptr[10] = mesh_hierarchy->get_make_line(vertex_ids, 5, 7, mesh);
      line_ptr[11] = mesh_hierarchy->get_make_line(vertex_ids, 6, 7, mesh);

      quad_ptr[0] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 0, 1, 2, 3, mesh).first;
      quad_ptr[1] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 0, 1, 4, 5, mesh).first;
      quad_ptr[2] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 0, 2, 4, 6, mesh).first;
      quad_ptr[3] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 1, 3, 5, 7, mesh).first;
      quad_ptr[4] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 2, 3, 6, 7, mesh).first;
      quad_ptr[5] = mesh_hierarchy->get_make_element_4(VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, vertex_ids, 4, 5, 6, 7, mesh).first;

      break;
    }

    default:
      assert(false);
  }
}












viennagrid_element_id viennagrid_element_buffer::make_element(viennagrid_element_type element_type,
                                                              viennagrid_element_id * vertex_ids,
                                                              viennagrid_int vertex_count,
                                                              bool reserve_boundary)
{
  viennagrid_dimension topologic_dimension = viennagrid_topological_dimension(element_type);
  viennagrid_element_id id = ID(topologic_dimension, size());
  ++element_count;

  if (topologic_dimension != 0)
    element_types.push_back(element_type);

  viennagrid_element_id * ptr = 0;

  switch (element_type)
  {
    // topo dim == 0
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
      return id;

    // topo dim == 1
    case VIENNAGRID_ELEMENT_TYPE_LINE:

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
    {
      assert((vertex_count == -1) || (vertex_count == viennagrid_boundary_element_count(element_type, 0)));
      ptr = boundary_buffer(0).push_back( vertex_count );
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
    {
      ptr = boundary_buffer(0).push_back(vertex_count);
      break;
    }

    default:
      assert(false);
  }


  if (reserve_boundary)
    this->reserve_boundary(id);

  std::copy( vertex_ids, vertex_ids+vertex_count, ptr );


  viennagrid_element_id * min_vertex_id = std::min_element(vertex_ids, vertex_ids+vertex_count);
  assert( min_vertex_id != vertex_ids+vertex_count );

  if (INDEX(*min_vertex_id) >= (viennagrid_int)element_map.size())
    element_map.resize(INDEX(*min_vertex_id)+1);

  element_map[INDEX(*min_vertex_id)].push_back(id);

  return id;
}


viennagrid_element_id viennagrid_element_buffer::make_elements(viennagrid_int element_count_,
                                                               viennagrid_element_type * element_types_,
                                                               viennagrid_int * element_vertex_index_offsets_,
                                                               viennagrid_element_id * element_vertex_indices_)
{
  viennagrid_dimension topologic_dimension = viennagrid_topological_dimension( element_types_[0] );

  viennagrid_element_id first_id = ID(topologic_dimension, size());
  element_count += element_count_;

  element_types.resize( element_count_ );
  std::copy( element_types_, element_types_+element_count_, element_types.begin()+first_id );

  boundary_buffer(0).push_back(element_count_, element_vertex_index_offsets_, element_vertex_indices_);

  for (viennagrid_int i = 0; i != element_count_; ++i)
  {
    viennagrid_element_id * min_vertex_id = std::min_element(element_vertex_indices_ + element_vertex_index_offsets_[i],
                                                             element_vertex_indices_ + element_vertex_index_offsets_[i+1]);
    assert( *min_vertex_id != element_vertex_index_offsets_[i+1] );

    if ( INDEX(*min_vertex_id) >= (viennagrid_int)element_map.size() )
      element_map.resize(INDEX(*min_vertex_id)+1);

    element_map[INDEX(*min_vertex_id)].push_back(first_id+i);
  }

  return first_id;
}


viennagrid_element_id viennagrid_element_buffer::make_vertices(viennagrid_int vertex_count)
{
  assert( topological_dimension == 0 );
  viennagrid_int first_id = ID(0, size());
  element_count += vertex_count;
  return first_id;
}

