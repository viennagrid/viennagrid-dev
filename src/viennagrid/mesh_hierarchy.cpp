#include "mesh_hierarchy.hpp"

void viennagrid_element_buffer::reserve_boundary(viennagrid_int element_id)
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


void viennagrid_element_buffer::make_boundary(viennagrid_int element_id, viennagrid_mesh mesh)
{
  viennagrid_element_type type = element_type(element_id);

  viennagrid_int * vertex_ids = boundary_ids[0].begin(element_id);
  viennagrid_int * vertex_end = boundary_ids[0].end(element_id);

  viennagrid_int count = vertex_end - vertex_ids;

  switch (type)
  {
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
    case VIENNAGRID_ELEMENT_TYPE_LINE:
    {
      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    {
      assert(count == 3);
      viennagrid_int * line_ids = boundary_begin(1, element_id);

      line_ids[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ids[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ids[2] = mesh_hierarchy->get_make_line(vertex_ids, 1, 2, mesh);

      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
    {
      viennagrid_int * line_ids = boundary_begin(1, element_id);

      line_ids[0] = mesh_hierarchy->get_make_line(vertex_ids, 0, 1, mesh);
      line_ids[1] = mesh_hierarchy->get_make_line(vertex_ids, 0, 2, mesh);
      line_ids[2] = mesh_hierarchy->get_make_line(vertex_ids, 1, 3, mesh);
      line_ids[3] = mesh_hierarchy->get_make_line(vertex_ids, 2, 3, mesh);

      break;
    }

    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    {
      viennagrid_int * line_ids = boundary_begin(1, element_id);
      viennagrid_int * triangle_ids = boundary_begin(2, element_id);

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
      viennagrid_int * line_ptr = boundary_begin(1, element_id);
      viennagrid_int * quad_ptr = boundary_begin(2, element_id);


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





viennagrid_int viennagrid_element_buffer::make_element(viennagrid_mesh_hierarchy /*mesh_hierarchy*/,
                                                         viennagrid_element_type element_type,
                                                         viennagrid_int * vertex_ids,
                                                         viennagrid_int vertex_count,
                                                         bool reserve_boundary)
{
  viennagrid_int id = size();

  element_types.push_back(element_type);
  parents.push_back(-1);
  element_flags.push_back(0);
  region_buffer.push_back( std::vector<viennagrid_region>() );


  viennagrid_int * ptr = 0;

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

  element_key key(vertex_ids, vertex_count);
  if (key.front() >= static_cast<viennagrid_int>(element_map.size()))
    element_map.resize( key.front()+1 );

  element_map[key.front()][key] = id;

  return id;
}







viennagrid_error viennagrid_mesh_hierarchy_::set_boundary_layout(viennagrid_int boundary_layout_in)
{
  if ((boundary_layout_in != VIENNAGRID_BOUNDARY_LAYOUT_FULL) && (boundary_layout_in != VIENNAGRID_BOUNDARY_LAYOUT_SPARSE))
    return VIENNAGRID_ERROR_UNSUPPORTED_BOUNDARY_LAYOUT;

  if ((boundary_layout() == VIENNAGRID_BOUNDARY_LAYOUT_SPARSE) &&
      (boundary_layout_in == VIENNAGRID_BOUNDARY_LAYOUT_FULL))
  {
    if (cell_dimension() > 0)
    {
      viennagrid_element_buffer & cell_buffer = element_buffer( cell_dimension() );

      for (viennagrid_int cell_id = 0; cell_id != cell_buffer.size(); ++cell_id)
      {
        cell_buffer.reserve_boundary(cell_id);
        cell_buffer.make_boundary(cell_id, root());

        viennagrid_region * regions_begin = cell_buffer.regions_begin(cell_id);
        viennagrid_region * regions_end = cell_buffer.regions_end(cell_id);

        for (viennagrid_region * region_it = regions_begin; region_it != regions_end; ++region_it)
        {
          for (viennagrid_dimension boundary_dim = 1; boundary_dim != cell_dimension(); ++boundary_dim)
          {
            viennagrid_int * boundary_begin = cell_buffer.boundary_begin(boundary_dim, cell_id);
            viennagrid_int * boundary_end = cell_buffer.boundary_end(boundary_dim, cell_id);

            for (viennagrid_int * boundary_it = boundary_begin; boundary_it != boundary_end; ++boundary_it)
              element_buffer(boundary_dim).add_to_region(*boundary_it, *region_it);
          }
        }
      }

      boundary_layout_ = boundary_layout_in;
    }
  }


  // switching from full layout to sparse, only allowed when only one mesh (root mesh) is in the hierarchy
  if ((boundary_layout() == VIENNAGRID_BOUNDARY_LAYOUT_FULL) &&
      (boundary_layout_in == VIENNAGRID_BOUNDARY_LAYOUT_SPARSE) &&
      (mesh_count() == 1))
  {
    if (cell_dimension() > 0)
    {
      for (viennagrid_dimension i = 1; i != cell_dimension(); ++i)
      {
        element_buffer(i).clear(this);
      }

      element_buffer( cell_dimension() ).clear_boundary();
    }

//     std::cout << "Switching boundary layout of " << this << ": FULL -> SPARSE" << std::endl;

    boundary_layout_ = boundary_layout_in;
  }

  return VIENNAGRID_SUCCESS;
}







std::pair<viennagrid_int, bool> viennagrid_mesh_hierarchy_::get_make_element(viennagrid_element_type element_type,
                                                                             viennagrid_int * vertex_ids,
                                                                             viennagrid_int vertex_count,
                                                                             viennagrid_mesh mesh,
                                                                             bool make_boundary)
{
  viennagrid_int element_topologic_dimension = viennagrid_topological_dimension(element_type);
  viennagrid_int id = element_buffer(element_topologic_dimension).get_element(vertex_ids, vertex_count);
  if (id != -1)
    return std::make_pair(id, false);

  increment_change_counter();

  id = element_buffer(element_topologic_dimension).make_element(this, element_type, vertex_ids, vertex_count, full_boundary_layout());
  cell_dimension_ = std::max( cell_dimension_, element_topologic_dimension );
  ++element_counts[element_type];

  if (mesh)
    mesh->add_element(element_topologic_dimension, id);

  if ( full_boundary_layout() && make_boundary )
  {
    element_buffer(element_topologic_dimension).make_boundary(id, mesh);
  }

  return std::make_pair(id, true);
}
