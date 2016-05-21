#include "refine.hpp"




viennagrid_error stable_line_is_longer(viennagrid_mesh mesh,
                                       viennagrid_element_id v1_1, viennagrid_element_id v1_2,
                                       viennagrid_element_id v2_1, viennagrid_element_id v2_2,
                                       viennagrid_bool * result)
{
  if (!result)
    return VIENNAGRID_SUCCESS;

  viennagrid_dimension geo_dim;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &geo_dim) );

//       typedef typename viennagrid::detail::result_of::value_type< VertexHandleT >::type VertexType;
//   typedef typename viennagrid::result_of::point< MeshT >::type PointType;
//   typedef typename viennagrid::result_of::coord< PointType >::type ScalarType;

//       const VertexType & v1_1 = viennagrid::dereference_handle( mesh, v1_1 );
//       const VertexType & v1_2 = viennagrid::dereference_handle( mesh, v1_2 );
//       const VertexType & v2_1 = viennagrid::dereference_handle( mesh, v2_1 );
//       const VertexType & v2_2 = viennagrid::dereference_handle( mesh, v2_2 );

  if (v1_2 < v1_1)
    std::swap(v1_1, v1_2);

  if (v2_2 < v2_1)
    std::swap(v2_1, v2_2);

//   viennagrid_element_id v1_smaller = (v1_1.id() < v1_2.id()) ? v1_1 : v1_2; //v1_1 carries smaller ID
//   viennagrid_element_id v1_larger = (v1_1.id() < v1_2.id()) ? v1_2 : v1_1; //v1_2 carries larger ID

//   viennagrid_element_id v2_smaller = (v2_1.id() < v2_2.id()) ? v2_1 : v2_2; //v2_1 carries smaller ID
//   viennagrid_element_id v2_larger = (v2_1.id() < v2_2.id()) ? v2_2 : v2_1; //v2_2 carries larger ID


  viennagrid_numeric line1;
  viennagrid_numeric line2;
  viennagrid_numeric * pt1, * pt2;
  std::vector<viennagrid_numeric> tmp(geo_dim);


  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, v1_1, &pt1) );
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, v1_2, &pt2) );
  RETURN_ON_ERROR( viennagrid_subtract(geo_dim, pt1, pt2, &tmp[0]) );
  RETURN_ON_ERROR( viennagrid_norm_2(geo_dim, &tmp[0], &line1) );

  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, v2_1, &pt1) );
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, v2_2, &pt2) );
  RETURN_ON_ERROR( viennagrid_subtract(geo_dim, pt1, pt2, &tmp[0]) );
  RETURN_ON_ERROR( viennagrid_norm_2(geo_dim, &tmp[0], &line2) );



//   ScalarType line1 = viennagrid::norm( viennagrid::get_point(mesh, v1_1) - viennagrid::get_point(mesh, v1_2) );
//   ScalarType line2 = viennagrid::norm( viennagrid::get_point(mesh, v2_1) - viennagrid::get_point(mesh, v2_2) );


  if (line1 > line2)
  {
    *result = VIENNAGRID_TRUE;
    return VIENNAGRID_SUCCESS;
  }
  else if (line1 >= line2) // use of == leads to floating-point comparison warning in Clang
  {
    //compare IDs:
    if (v1_1 > v2_1)
    {
      *result = VIENNAGRID_TRUE;
      return VIENNAGRID_SUCCESS;
    }
    else if (v1_1 == v2_1)
    {
      if (v1_2 > v2_2)
      {
        *result = VIENNAGRID_TRUE;
        return VIENNAGRID_SUCCESS;
      }
      else if (v1_2 == v2_2)
      {
        //identical lines are compared!
        *result = VIENNAGRID_FALSE;
        return VIENNAGRID_SUCCESS;
      }
    }
  }

  *result = VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}


viennagrid_error stable_line_is_longer(viennagrid_mesh mesh,
                                       viennagrid_element_id * vertex_ids,
                                       viennagrid_int i0, viennagrid_int i1,
                                       viennagrid_int i2, viennagrid_int i3,
                                       viennagrid_bool * result)
{
  return stable_line_is_longer(mesh, vertex_ids[i0], vertex_ids[i1], vertex_ids[i2], vertex_ids[i3], result);
}




viennagrid_error viennagrid_mesh_element_create_refinement_tetrahedron(viennagrid_mesh mesh,
                                                                       viennagrid_element_id * vertex_ids,
                                                                       viennagrid_int i0, viennagrid_int i1,
                                                                       viennagrid_int i2, viennagrid_int i3,
                                                                       viennagrid_region * regions_begin,
                                                                       viennagrid_region * regions_end)
{
  viennagrid_element_id vertices[4];

  vertices[0] = vertex_ids[i0];
  vertices[1] = vertex_ids[i1];
  vertices[2] = vertex_ids[i2];
  vertices[3] = vertex_ids[i3];

  return viennagrid_mesh_element_create_with_regions(mesh,
                                                     VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON,
                                                     4,
                                                     vertices,
                                                     regions_begin,
                                                     regions_end,
                                                     NULL);
}



viennagrid_error edge_refine_tetrahedron_0(viennagrid_mesh         /*mesh*/,
                                           viennagrid_bool       * /*edge_refinement_tags*/,
                                           viennagrid_element_id * /*vertex_on_edge_ids*/,
                                           viennagrid_element_id * old_to_new_vertex_map,
                                           viennagrid_element_id   /*cell_id*/,
                                           viennagrid_element_id * vertices_begin,
                                           viennagrid_element_id * /*edges_begin*/,
                                           viennagrid_region *     regions_begin,
                                           viennagrid_region *     regions_end,
                                           viennagrid_mesh         output_mesh)
{
  viennagrid_element_id new_vertices[4];

  new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
  new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
  new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
  new_vertices[3] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[3])];

  RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, new_vertices, regions_begin, regions_end, NULL) );

  return VIENNAGRID_SUCCESS;
}


viennagrid_error edge_refine_tetrahedron_1(viennagrid_mesh         /*mesh*/,
                                           viennagrid_bool       * edge_refinement_tags,
                                           viennagrid_element_id * vertex_on_edge_ids,
                                           viennagrid_element_id * old_to_new_vertex_map,
                                           viennagrid_element_id   /*cell_id*/,
                                           viennagrid_element_id * vertices_begin,
                                           viennagrid_element_id * edges_begin,
                                           viennagrid_region *     regions_begin,
                                           viennagrid_region *     regions_end,
                                           viennagrid_mesh         output_mesh)
{
  viennagrid_element_id vertices[4];
  viennagrid_element_id ordered_vertices[5];

  vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
  vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
  vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
  vertices[3] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[3])];



  if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])])
  {
    ordered_vertices[0] = vertices[0];
    ordered_vertices[1] = vertices[1];
    ordered_vertices[2] = vertices[2];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])])
  {
    ordered_vertices[0] = vertices[2];
    ordered_vertices[1] = vertices[0];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])])
  {
    ordered_vertices[0] = vertices[0];
    ordered_vertices[1] = vertices[3];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[2];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])])
  {
    ordered_vertices[0] = vertices[1];
    ordered_vertices[1] = vertices[2];
    ordered_vertices[2] = vertices[0];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])])
  {
    ordered_vertices[0] = vertices[3];
    ordered_vertices[1] = vertices[1];
    ordered_vertices[2] = vertices[0];
    ordered_vertices[3] = vertices[2];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])])
  {
    ordered_vertices[0] = vertices[3];
    ordered_vertices[1] = vertices[2];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[0];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
  }
  else
  {
    // TODO: Error! This cannot be!
  }

  //cell containing vertex 0:
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, ordered_vertices, 0, 4, 2, 3, regions_begin, regions_end) );

  //cell without vertex 0:
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, ordered_vertices, 4, 1, 2, 3, regions_begin, regions_end) );

  return VIENNAGRID_SUCCESS;
}





viennagrid_error edge_refine_tetrahedron_2_1(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 3, regions_begin, regions_end) );

  viennagrid_bool tmp;
  RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 1, 2, 1, &tmp) );

  if (tmp == VIENNAGRID_TRUE)
  {
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 2, 3, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 2, 3, regions_begin, regions_end) );
  }
  else //split edge 12, introduce line 05
  {
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 3, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 2, 3, regions_begin, regions_end) );
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error edge_refine_tetrahedron_2_2(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 2, 5, regions_begin, regions_end) );
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 3, regions_begin, regions_end) );
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 2, 5, regions_begin, regions_end) );
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 3, regions_begin, regions_end) );

  return VIENNAGRID_SUCCESS;
}







viennagrid_error edge_refine_tetrahedron(viennagrid_mesh         mesh,
                                         viennagrid_bool       * edge_refinement_tags,
                                         viennagrid_element_id * vertex_on_edge_ids,
                                         viennagrid_element_id * old_to_new_vertex_map,
                                         viennagrid_element_id   cell_id,
                                         viennagrid_mesh         output_mesh)
{
  viennagrid_int num_edges_to_refine = 0;

  viennagrid_dimension dim;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(output_mesh, &dim) );

  viennagrid_numeric * coords;
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_pointer(output_mesh, &coords) );

  viennagrid_element_id *edges_begin, *edges_end;
  RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, cell_id, 1, &edges_begin, &edges_end); );
  for (viennagrid_element_id *eit  = edges_begin;
                              eit != edges_end;
                            ++eit)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(*eit)])
      ++num_edges_to_refine;
  }

  viennagrid_element_id *vertices_begin, *vertices_end;
  RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, cell_id, 0, &vertices_begin, &vertices_end); );
  (void)vertices_end; //suppress unused variable warning

  viennagrid_region_id * cell_region_ids_start;
  viennagrid_region_id * cell_region_ids_end;
  viennagrid_element_regions_get(mesh, cell_id, &cell_region_ids_start, &cell_region_ids_end);

  std::vector<viennagrid_region> regions;
  for (viennagrid_region_id * rit = cell_region_ids_start; rit != cell_region_ids_end; ++rit)
  {
    viennagrid_region tmp;
    viennagrid_mesh_region_get_or_create(output_mesh, *rit, &tmp);
    regions.push_back(tmp);
  }

  viennagrid_region * regions_begin = regions.empty() ? &regions[0] : NULL;
  viennagrid_region * regions_end = regions_begin ? regions_begin + regions.size() : NULL;



  switch (num_edges_to_refine)
  {
    case 0:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_0(mesh,
                                                 edge_refinement_tags,
                                                 vertex_on_edge_ids,
                                                 old_to_new_vertex_map,
                                                 cell_id,
                                                 vertices_begin, edges_begin,
                                                 regions_begin, regions_end,
                                                 output_mesh) );
      break;
    }
    case 1:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_1(mesh,
                                                 edge_refinement_tags,
                                                 vertex_on_edge_ids,
                                                 old_to_new_vertex_map,
                                                 cell_id,
                                                 vertices_begin, edges_begin,
                                                 regions_begin, regions_end,
                                                 output_mesh) );
      break;
    }
//     case 2:
//     {
//       edge_refine_tetrahedron_2(mesh,
//                                 edge_refinement_tags,
//                                 vertex_on_edge_ids,
//                                 old_to_new_vertex_map,
//                                 cell_id,
//                                 regions_begin,
//                                 regions_end,
//                                 output_mesh);
//       break;
//     }
//     case 3:
//     {
//       edge_refine_tetrahedron_3(mesh,
//                                 edge_refinement_tags,
//                                 vertex_on_edge_ids,
//                                 old_to_new_vertex_map,
//                                 cell_id,
//                                 regions_begin,
//                                 regions_end,
//                                 output_mesh);
//       break;
//     }
//     case 4:
//     {
//       edge_refine_tetrahedron_4(mesh,
//                                 edge_refinement_tags,
//                                 vertex_on_edge_ids,
//                                 old_to_new_vertex_map,
//                                 cell_id,
//                                 regions_begin,
//                                 regions_end,
//                                 output_mesh);
//       break;
//     }
//     case 5:
//     {
//       edge_refine_tetrahedron_5(mesh,
//                                 edge_refinement_tags,
//                                 vertex_on_edge_ids,
//                                 old_to_new_vertex_map,
//                                 cell_id,
//                                 regions_begin,
//                                 regions_end,
//                                 output_mesh);
//       break;
//     }
//     case 6:
//     {
//       edge_refine_tetrahedron_6(mesh,
//                                 edge_refinement_tags,
//                                 vertex_on_edge_ids,
//                                 old_to_new_vertex_map,
//                                 cell_id,
//                                 regions_begin,
//                                 regions_end,
//                                 output_mesh);
//       break;
//     }

    default: //nothing to do...
            break;
  }

  return VIENNAGRID_SUCCESS;
}
