#include "mesh_hierarchy.hpp"
















// void viennagrid_element_buffer::calculate_boundary_buffer(viennagrid_index boundary_topo_dim)
// {
// //   std::cout << "Calculate Boundary Buffer for dim " << topologic_dimension << " boundary dim = " << boundary_topo_dim << std::endl;
//   for (viennagrid_int i = 1; i != size(); ++i)
//   {
//     calculate_boundary_buffer(boundary_topo_dim, i);
//   }
//
//   boundary_indices[boundary_topo_dim].second = true;
// }


void viennagrid_element_buffer::calculate_boundary_buffer(viennagrid_index boundary_topo_dim, viennagrid_index element_id, viennagrid_mesh mesh)
{
  assert(topologic_dimension > boundary_topo_dim);
  assert(boundary_topo_dim > 0);

  viennagrid_element_tag tag = element_tag(element_id);

  viennagrid_index * vertex_indices = boundary_indices[0].first.begin(element_id);
  viennagrid_index * vertex_indices_end = boundary_indices[0].first.end(element_id);

//   viennagrid_index * vertex_indices = boundary_begin(0, element_id);
//   viennagrid_index * vertex_indices_end = boundary_end(0, element_id);
  viennagrid_int count = vertex_indices_end - vertex_indices;


  viennagrid_int index = 0;
  viennagrid_index * ptr = 0;

  ViennaGridBoundaryBufferType & bb = boundary_indices[boundary_topo_dim].first;


  switch (boundary_topo_dim)
  {
    case 0:
    {
      assert(false);
      break;
    }

    case 1:
    {
      switch (tag)
      {
        case VIENNAGRID_ELEMENT_TAG_TRIANGLE:
        {
          assert(count == 3);
          index = 0;
          ptr = bb.push_back( viennagrid_boundary_element_count_from_element_tag(tag, VIENNAGRID_ELEMENT_TAG_LINE) );

          for (viennagrid_int i = 0; i < 3; ++i)
            for (viennagrid_int j = i+1; j < 3; ++j)
              ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, i, j, mesh).first;

          break;
        }

        case VIENNAGRID_ELEMENT_TAG_QUADRILATERAL:
        {
          assert(count == 4);
          index = 0;
          ptr = bb.push_back( viennagrid_boundary_element_count_from_element_tag(tag, VIENNAGRID_ELEMENT_TAG_LINE) );

          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 1, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 2, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 1, 3, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 2, 3, mesh).first;

          break;
        }

        case VIENNAGRID_ELEMENT_TAG_POLYGON:
        {
          index = 0;
          ptr = bb.push_back(count);

          for (viennagrid_int i = 0; i < count-1; ++i)
            ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, i, i+1, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, count-1, 0, mesh).first;

          break;
        }

//         // TODO CHECK
//         case VIENNAGRID_ELEMENT_TAG_PLC:
//         {
//           index = 0;
//           ptr = boundary_buffer(boundary_topo_dim).push_back(count);
//
//           std::copy( vertex_indices, vertex_indices+count, ptr );
//
//           break;
//         }

        case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
        {
          assert(count == 4);
          index = 0;
          ptr = bb.push_back( viennagrid_boundary_element_count_from_element_tag(tag, VIENNAGRID_ELEMENT_TAG_LINE) );

          for (viennagrid_int i = 0; i < 4; ++i)
            for (viennagrid_int j = i+1; j < 4; ++j)
              ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, i, j, mesh).first;

          break;
        }

        case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
        {
          assert(count == 8);
          index = 0;

          ptr = bb.push_back( viennagrid_boundary_element_count_from_element_tag(tag, VIENNAGRID_ELEMENT_TAG_LINE) );
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 1, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 2, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 4, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 1, 3, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 1, 5, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 2, 3, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 2, 5, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 3, 7, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 4, 5, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 4, 6, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 5, 7, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 6, 7, mesh).first;

          break;
        }
      }

      break;
    } // end case 1

    case 2:
    {
      switch (tag)
      {
        case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
        {
          assert(count == 4);
          index = 0;
          ptr = bb.push_back( viennagrid_boundary_element_count_from_element_tag(tag, VIENNAGRID_ELEMENT_TAG_TRIANGLE) );

          for (viennagrid_int i = 0; i < 4; ++i)
            for (viennagrid_int j = i+1; j < 4; ++j)
              for (viennagrid_int k = j+1; k < 4; ++k)
                ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_TRIANGLE, vertex_indices, i, j, k, mesh).first;

          break;
        }

        case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
        {
          assert(count == 8);
          index = 0;
          ptr = bb.push_back( viennagrid_boundary_element_count_from_element_tag(tag, VIENNAGRID_ELEMENT_TAG_QUADRILATERAL) );

          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 0, 1, 2, 3, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 0, 1, 4, 5, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 0, 2, 4, 6, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 1, 3, 5, 7, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 2, 3, 6, 7, mesh).first;
          ptr[index++] = mesh_hierarchy->get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 4, 5, 6, 7, mesh).first;

          break;
        }
      }

      break;
    } // end case 2

    assert(false);
  }

}


void viennagrid_element_buffer::calculate_boundary_buffers(viennagrid_index element_id, viennagrid_mesh mesh)
{
//   std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;

  for (viennagrid_dimension boundary_topo_dim = 1; boundary_topo_dim != topologic_dimension; ++boundary_topo_dim)
  {
//     if (boundary_indices[boundary_topo_dim].second)
    {
//       std::cout << "Creating boundary " << boundary_topo_dim << " for " << topologic_dimension << " (id=" << element_id << ")" << std::endl;
      calculate_boundary_buffer(boundary_topo_dim, element_id, mesh);
    }
  }
}







viennagrid_index viennagrid_element_buffer::make_element(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                         viennagrid_element_tag element_tag,
                                                         viennagrid_index * indices,
                                                         viennagrid_int index_count)
{
  viennagrid_index id = size();

  element_tags.push_back(element_tag);
  parents.push_back(-1);
  region_buffer.push_back(0);
  if (element_tag == VIENNAGRID_ELEMENT_TAG_PLC)
    mesh_hierarchy->make_plc();

  if (indices)
  {
    if (element_tag == VIENNAGRID_ELEMENT_TAG_PLC)
    {
      std::set<viennagrid_index> vertices;
      for (viennagrid_int i = 0; i < index_count; ++i)
      {
        viennagrid_index * vtx = mesh_hierarchy->boundary_begin(1, *(indices+i), 0);
        viennagrid_index * end = mesh_hierarchy->boundary_end(1, *(indices+i), 0);

//         element_buffer(VIENNAGRID_ELEMENT_TAG_LINE).boundary_indices_begin(VIENNAGRID_ELEMENT_TAG_VERTEX, *(indices+i));
//         viennagrid_index * end = mesh_hierarchy->element_buffer(VIENNAGRID_ELEMENT_TAG_LINE).boundary_indices_end(VIENNAGRID_ELEMENT_TAG_VERTEX, *(indices+i));

        for (; vtx != end; ++vtx)
          vertices.insert(*vtx);
      }

      viennagrid_index * ptr = boundary_buffer(VIENNAGRID_ELEMENT_TAG_VERTEX).push_back( vertices.size() );
      std::copy( vertices.begin(), vertices.end(), ptr );
    }
    else
    {
      viennagrid_index * ptr = boundary_buffer(VIENNAGRID_ELEMENT_TAG_VERTEX).push_back( index_count );
      std::copy( indices, indices+index_count, ptr );
    }


    element_key key(indices, index_count);
    if (key.front() >= element_map.size())
      element_map.resize( key.front()+1 );

    element_map[key.front()][key] = id;

//     element_map[key] = id;

//     element_key key(indices, index_count);
//     ElementMapType::iterator it = std::lower_bound( element_map.begin(), element_map.end(), key, IndicesComperator() );
//     element_map.insert(it, std::make_pair(key, id));
  }

  return id;
}






std::pair<viennagrid_index, bool> viennagrid_mesh_hierarchy_::get_make_element(viennagrid_element_tag element_tag,
                                                              viennagrid_index * indices,
                                                              viennagrid_dimension * /*topo_dims*/,
                                                              viennagrid_int count,
                                                              viennagrid_mesh mesh)
{
  viennagrid_int element_topologic_dimension = viennagrid_topological_dimension(element_tag);
  viennagrid_index id = element_buffer(element_topologic_dimension).get_element(indices, count);
  if (id != -1)
    return std::make_pair(id, false);

//   std::cout << "Make element " << viennagrid_element_tag_string(element_tag) << " at " << element_topologic_dimension << std::endl;


  increment_change_counter();

  id = element_buffer(element_topologic_dimension).make_element(this, element_tag, indices, count);
  cell_dimension_ = std::max( cell_dimension_, element_topologic_dimension );
  ++element_counts[element_tag];

  if (mesh)
    mesh->add_element(element_topologic_dimension, id);

  viennagrid_int index = 0;
  viennagrid_index * ptr = 0;


  element_buffer(element_topologic_dimension).calculate_boundary_buffers(id, mesh);

  if (element_tag == VIENNAGRID_ELEMENT_TAG_PLC)
  {
    index = 0;
    ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_PLC, VIENNAGRID_ELEMENT_TAG_LINE, count);

    std::copy( indices, indices+count, ptr );
  }

  return std::make_pair(id, true);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//   switch (element_tag)
//   {
//     case VIENNAGRID_ELEMENT_TAG_VERTEX:
//       return id;
//
//     case VIENNAGRID_ELEMENT_TAG_LINE:
//       return id;
//
//     case VIENNAGRID_ELEMENT_TAG_TRIANGLE:
//     {
//       index = 0;
//       ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TRIANGLE, VIENNAGRID_ELEMENT_TAG_LINE);
//       for (viennagrid_int i = 0; i < 3; ++i)
//         for (viennagrid_int j = i+1; j < 3; ++j)
//           ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, i, j);
//
//       return id;
//     }
//
//     case VIENNAGRID_ELEMENT_TAG_QUADRILATERAL:
//     {
//       index = 0;
//       ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, VIENNAGRID_ELEMENT_TAG_LINE);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 1);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 2);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 1, 3);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 2, 3);
//
//       return id;
//     }
//
//     case VIENNAGRID_ELEMENT_TAG_POLYGON:
//     {
//       index = 0;
//       ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_POLYGON, VIENNAGRID_ELEMENT_TAG_LINE, count);
//       for (viennagrid_int i = 0; i < count-1; ++i)
//         ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, i, i+1);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, count-1, 0);
//
//       return id;
//     }
//
//     case VIENNAGRID_ELEMENT_TAG_PLC:
//     {
//
//
// //       ptr[index++] = indices
//
// //       for (viennagrid_int i = 0; i < count-1; ++i)
// //         ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, i, i+1);
// //
// //       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, count-1, 0);
//     }
//
//     case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
//     {
//       index = 0;
//       ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, VIENNAGRID_ELEMENT_TAG_LINE);
//       for (viennagrid_int i = 0; i < 4; ++i)
//         for (viennagrid_int j = i+1; j < 4; ++j)
//           ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, i, j);
//
//       index = 0;
//       ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, VIENNAGRID_ELEMENT_TAG_TRIANGLE);
//       for (viennagrid_int i = 0; i < 4; ++i)
//         for (viennagrid_int j = i+1; j < 4; ++j)
//           for (viennagrid_int k = j+1; k < 4; ++k)
//             ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_TRIANGLE, indices, i, j, k);
//
//       return id;
//     }
//
//     case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
//     {
//       index = 0;
//       ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_HEXAHEDRON, VIENNAGRID_ELEMENT_TAG_LINE);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 1);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 2);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 4);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 1, 3);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 1, 5);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 2, 3);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 2, 5);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 3, 7);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 4, 5);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 4, 6);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 5, 7);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 6, 7);
//
//
//       index = 0;
//       ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_HEXAHEDRON, VIENNAGRID_ELEMENT_TAG_QUADRILATERAL);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 0, 1, 2, 3);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 0, 1, 4, 5);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 0, 2, 4, 6);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 1, 3, 5, 7);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 2, 3, 6, 7);
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 4, 5, 6, 7);
//
//       return id;
//     }
//
//     default:
//       assert(false);
//   }

//   return std::make_pair-1;
}
