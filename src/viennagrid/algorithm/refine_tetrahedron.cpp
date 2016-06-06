#include "refine.hpp"
#include <cassert>



viennagrid_error stable_line_is_longer(viennagrid_mesh mesh,
                                       viennagrid_element_id v1_1, viennagrid_element_id v1_2,
                                       viennagrid_element_id v2_1, viennagrid_element_id v2_2,
                                       viennagrid_bool * result)
{
  if (!result)
    return VIENNAGRID_SUCCESS;

  viennagrid_dimension geo_dim;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &geo_dim) );

  if (v1_2 < v1_1)
    std::swap(v1_1, v1_2);

  if (v2_2 < v2_1)
    std::swap(v2_1, v2_2);

  viennagrid_numeric line1;
  viennagrid_numeric line2;
  viennagrid_numeric * pt1, * pt2;
  std::vector<viennagrid_numeric> tmp(geo_dim);


  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, v1_1, &pt1) );
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, v1_2, &pt2) );
  RETURN_ON_ERROR( viennagrid_point_subtract(geo_dim, pt1, pt2, &tmp[0]) );
  RETURN_ON_ERROR( viennagrid_point_norm_2(geo_dim, &tmp[0], &line1) );

  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, v2_1, &pt1) );
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, v2_2, &pt2) );
  RETURN_ON_ERROR( viennagrid_point_subtract(geo_dim, pt1, pt2, &tmp[0]) );
  RETURN_ON_ERROR( viennagrid_point_norm_2(geo_dim, &tmp[0], &line2) );

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



viennagrid_error edge_refine_tetrahedron_0(viennagrid_bool       * /*edge_refinement_tags*/,
                                           viennagrid_element_id * /*vertex_on_edge_ids*/,
                                           viennagrid_element_id * old_to_new_vertex_map,
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


viennagrid_error edge_refine_tetrahedron_1(viennagrid_bool       * edge_refinement_tags,
                                           viennagrid_element_id * vertex_on_edge_ids,
                                           viennagrid_element_id * old_to_new_vertex_map,
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



  if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] == VIENNAGRID_TRUE)
  {
    ordered_vertices[0] = vertices[0];
    ordered_vertices[1] = vertices[1];
    ordered_vertices[2] = vertices[2];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] == VIENNAGRID_TRUE)
  {
    ordered_vertices[0] = vertices[2];
    ordered_vertices[1] = vertices[0];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_TRUE)
  {
    ordered_vertices[0] = vertices[0];
    ordered_vertices[1] = vertices[3];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[2];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
  {
    ordered_vertices[0] = vertices[1];
    ordered_vertices[1] = vertices[2];
    ordered_vertices[2] = vertices[0];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
  {
    ordered_vertices[0] = vertices[3];
    ordered_vertices[1] = vertices[1];
    ordered_vertices[2] = vertices[0];
    ordered_vertices[3] = vertices[2];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
  {
    ordered_vertices[0] = vertices[3];
    ordered_vertices[1] = vertices[2];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[0];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
  }
  else
  {
    assert(false && "Logic error: No edge for refinement found!");
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

viennagrid_error edge_refine_tetrahedron_2(viennagrid_bool       * edge_refinement_tags,
                                           viennagrid_element_id * vertex_on_edge_ids,
                                           viennagrid_element_id * old_to_new_vertex_map,
                                           viennagrid_element_id * vertices_begin,
                                           viennagrid_element_id * edges_begin,
                                           viennagrid_region *     regions_begin,
                                           viennagrid_region *     regions_end,
                                           viennagrid_mesh         output_mesh)
{
  viennagrid_element_id vertices[4];
  viennagrid_element_id ordered_vertices[6];

  vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
  vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
  vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
  vertices[3] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[3])];

  if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[1];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[3];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[0];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];

      edge_refine_tetrahedron_2_2(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[1];
      ordered_vertices[1] = vertices[2];
      ordered_vertices[2] = vertices[0];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];


      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

      edge_refine_tetrahedron_2_2(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[2];
      ordered_vertices[2] = vertices[3];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];

      edge_refine_tetrahedron_2_2(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[3];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[3];
      ordered_vertices[2] = vertices[0];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[3];
      ordered_vertices[3] = vertices[0];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[2];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[0];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[1];
      ordered_vertices[1] = vertices[3];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[0];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];

      edge_refine_tetrahedron_2_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else
  {
    assert(false && "Logic error: No edge for refinement found!");
  }

  return VIENNAGRID_SUCCESS;
}




viennagrid_error edge_refine_tetrahedron_3_1(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 6, regions_begin, regions_end) );

  // Strategy: The two longest edges of the common vertex are split 'twice',
  //           i.e. two new edges start from the center of the two longest edges

  viennagrid_bool tmp;
  RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 1, 1, 2, &tmp) );

  if (tmp == VIENNAGRID_TRUE)
  {
    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 1, 2, &tmp) );
    if (tmp == VIENNAGRID_TRUE)
    {
      RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 0, 1, &tmp) );
      if (tmp == VIENNAGRID_TRUE)
      {
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 6, 2, 3, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 2, 6, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 2, 6, regions_begin, regions_end) );
      }
      else  //split edge 01 again, introduce line 43
      {
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 2, 3, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 3, 4, 2, 6, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 2, 6, regions_begin, regions_end) );
      }
    }
    else //split edge 12 again, introduce lines 43 and 53
    {
      RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 0, 1, &tmp) );
      if (tmp == VIENNAGRID_TRUE)
      {
        assert(false && "diag13_len > diag01_len impossible!");
      }
      else  //split edge 01 again, introduce line 43
      {
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 2, 3, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 2, 3, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 6, 5, 3, regions_begin, regions_end) );
      }
    }
  }
  else //split edge 12, introduce line 05
  {
    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 1, 2, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 13 again, introduce line 62
    {
      RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 0, 1, &tmp) );
      if (tmp == VIENNAGRID_TRUE) //split edge 13 again, introduce line 60
      {
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 6, 5, 2, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 6, 2, 3, regions_begin, regions_end) );
      }
      else  //split edge 01 again, introduce line 43
      {
        assert(false && "diag13_len > diag01_len impossible!");
      }
    }
    else //split edge 12 again, introduce line 53
    {
      RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 0, 1, &tmp) );
      if (tmp == VIENNAGRID_TRUE) //split edge 13 again, introduce line 60
      {
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 6, 5, 3, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 2, 3, regions_begin, regions_end) );
      }
      else  //split edge 01 again, introduce line 43
      {
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 3, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 3, 6, regions_begin, regions_end) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 2, 3, regions_begin, regions_end) );
      }
    }
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error edge_refine_tetrahedron_3_2(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 6, 3, regions_begin, regions_end) );
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 6, 3, regions_begin, regions_end) );
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 3, regions_begin, regions_end) );
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 5, 2, 3, regions_begin, regions_end) );

  return VIENNAGRID_SUCCESS;
}

viennagrid_error edge_refine_tetrahedron_3_3(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{
  // Strategy: The two longest edges of the common vertex are split 'twice',
  //           i.e. two new edges start from the center of the two longest edges
  //if (diag01_len > diag03_len) //split edge 01 again, introduce line 43

  viennagrid_bool tmp;
  RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 1, 0, 3, &tmp) );

  if (tmp == VIENNAGRID_TRUE) //split edge 01 again, introduce line 43
  {
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 3, regions_begin, regions_end) );

    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 1, 1, 2, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 01 again, introduce line 42
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 2, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 2, 3, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 2, 3, regions_begin, regions_end) );
    }
    else //split edge 12 again, introduce line 50
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 2, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 5, 3, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 5, 2, 3, regions_begin, regions_end) );
    }
  }
  else  //split edge 03 again, introduce line 61
  {
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 6, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 1, 5, 3, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 5, 2, 3, regions_begin, regions_end) );

    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 1, 1, 2, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 01 again, introduce line 42
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 2, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 5, 2, regions_begin, regions_end) );
    }
    else //split edge 12 again, introduce line 50
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 2, 6, regions_begin, regions_end) );
    }
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error edge_refine_tetrahedron_3_4(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{

  // Strategy: The two longest edges of the common vertex are split 'twice',
  //           i.e. two new edges start from the center of the two longest edges

  viennagrid_bool tmp;
  RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 1, 1, 2, &tmp) );

  if (tmp == VIENNAGRID_TRUE) //split edge 01 again, introduce line 42
  {
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 2, 6, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 6, 3, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 2, 6, regions_begin, regions_end) );

    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 2, 2, 3, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 12 again, introduce line 53
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 3, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 6, 3, regions_begin, regions_end) );
    }
    else //split edge 23 again, introduce line 61
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 6, 3, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 6, regions_begin, regions_end) );
    }
  }
  else //split edge 12, introduce line 50
  {
    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 2, 2, 3, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 12 again, introduce line 53
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 3, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 6, 3, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 2, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 3, regions_begin, regions_end) );
    }
    else //split edge 23 again, introduce line 61
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 6, 3, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 2, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 5, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 6, 3, regions_begin, regions_end) );
    }
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error edge_refine_tetrahedron_3(viennagrid_bool       * edge_refinement_tags,
                                           viennagrid_element_id * vertex_on_edge_ids,
                                           viennagrid_element_id * old_to_new_vertex_map,
                                           viennagrid_element_id * vertices_begin,
                                           viennagrid_element_id * edges_begin,
                                           viennagrid_region *     regions_begin,
                                           viennagrid_region *     regions_end,
                                           viennagrid_mesh         output_mesh)
{
  viennagrid_element_id vertices[4];
  viennagrid_element_id ordered_vertices[7];

  vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
  vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
  vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
  vertices[3] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[3])];

  //with e0
  if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
        edge_refine_tetrahedron_3_1(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
        edge_refine_tetrahedron_3_2(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
        edge_refine_tetrahedron_3_4(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        edge_refine_tetrahedron_3_3(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[1];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[3];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
        edge_refine_tetrahedron_3_3(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
        edge_refine_tetrahedron_3_2(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        edge_refine_tetrahedron_3_4(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
        edge_refine_tetrahedron_3_1(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        edge_refine_tetrahedron_3_4(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[0];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        edge_refine_tetrahedron_3_3(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else //no second edge
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
        edge_refine_tetrahedron_3_4(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
        edge_refine_tetrahedron_3_3(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        edge_refine_tetrahedron_3_2(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[1];
      ordered_vertices[1] = vertices[2];
      ordered_vertices[2] = vertices[0];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
        edge_refine_tetrahedron_3_3(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        edge_refine_tetrahedron_3_1(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
    {
      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        //make edges 4 and 5 the references
        ordered_vertices[0] = vertices[1];
        ordered_vertices[1] = vertices[3];
        ordered_vertices[2] = vertices[2];
        ordered_vertices[3] = vertices[0];
        ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
        ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];

        edge_refine_tetrahedron_3_4(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else //no second edge
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
    {
      //NOTE: edges 2 and 3 don't have a common vertex, therefore 'base facet' is chosen depending on the third edge

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
      {
        // take edges 2 and 4 as reference
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[3];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[2];
        ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
        ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];

        edge_refine_tetrahedron_3_4(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        // take edges 5 and 3 as reference
        ordered_vertices[0] = vertices[3];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[0];
        ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];

        edge_refine_tetrahedron_3_3(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[3];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        edge_refine_tetrahedron_3_1(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else //no second edge
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_TRUE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_TRUE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[3];
      ordered_vertices[3] = vertices[0];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

      if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_TRUE)
      {
        ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
        edge_refine_tetrahedron_3_2(output_mesh, ordered_vertices, regions_begin, regions_end);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!");
      }
    }
    else //no second edge
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else //no first edge found
  {
    assert(false && "Logic error: No edge for refinement found!");
  }

  return VIENNAGRID_SUCCESS;
}




viennagrid_error edge_refine_tetrahedron_4_1(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{
  viennagrid_bool tmp;
  RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 3, 1, 3, &tmp) );

  if (tmp == VIENNAGRID_TRUE) //split edge 03, introduce line 71
  {
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 7, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 5, 6, 3, regions_begin, regions_end) );

    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 2, 3, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 13, introduce line 52
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 1, 4, 5, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 5, 4, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 2, 4, 5, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 2, 6, regions_begin, regions_end) );
    }
    else //split edge 23, introduce line 61
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 1, 6, 5, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 1, 4, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 2, 4, 6, regions_begin, regions_end) );
    }
  }
  else //split edge 13, introduce line 50
  {
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 5, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 5, 4, 7, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 5, 6, 3, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 5, 4, 6, regions_begin, regions_end) );

    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 2, 3, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 13 again, introduce line 52
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 2, 4, 5, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 5, 2, 6, regions_begin, regions_end) );
    }
    else //split edge 23, introduce line 61
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 1, 4, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 1, 2, 6, regions_begin, regions_end) );
    }
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error edge_refine_tetrahedron_4_2(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{
  viennagrid_bool tmp;
  RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 3, 1, 3, &tmp) );

  if (tmp == VIENNAGRID_TRUE) //split edge 03, introduce line 61
  {
    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 1, 2, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 13, introduce line 72
    {
      RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 0, 3, &tmp) );
      if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 53
      {
        RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
        if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 1, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 2, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 5, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 2, 5, 3, regions_begin, regions_end) );
        }
        else //split edge 12, introduce line 40
        {
          assert( false && "Logic error: diag02 < diag12 not possible here!");
        }
      }
      else //split edge 03, introduce line 62
      {
        RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
        if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 6, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 4, 6, 2, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 2, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 2, 3, regions_begin, regions_end) );
        }
        else //split edge 12, introduce line 40
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 1, 4, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 4, 2, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 5, 2, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 2, 3, regions_begin, regions_end) );
        }
      }
    }
    else //split edge 12, introduce line 43
    {
      RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 0, 3, &tmp) );
      if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 53
      {
        RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
        if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 1, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 4, 2, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 7, 4, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 5, 3, regions_begin, regions_end) );
        }
        else //split edge 12, introduce line 40
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 1, 4, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 4, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 5, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 4, 2, 3, regions_begin, regions_end) );
        }
      }
      else //split edge 03, introduce line 62
      {
        RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
        if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 6, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 4, 6, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 2, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 2, 3, regions_begin, regions_end) );
        }
        else //split edge 12, introduce line 40
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 1, 4, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 4, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 4, 2, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 2, 3, regions_begin, regions_end) );
        }
      }
    }
  }
  else //split edge 13, introduce line 70
  {
    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 1, 3, 1, 2, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 13, introduce line 72
    {
      RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 0, 3, &tmp) );
      if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 53
      {
        RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
        if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 4, 2, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 7, 2, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 5, 3, regions_begin, regions_end) );
        }
        else //split edge 12, introduce line 40
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 5, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 4, 2, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 7, 2, 3, regions_begin, regions_end) );
        }
      }
      else //split edge 03, introduce line 62
      {
        RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
        if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 2, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 2, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 2, 6, 3, regions_begin, regions_end) );
        }
        else //split edge 12, introduce line 40
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 4, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 4, 2, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 5, 2, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 2, 3, regions_begin, regions_end) );
        }
      }
    }
    else //split edge 12, introduce line 43
    {
      RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 0, 3, &tmp) );
      if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 53
      {
        RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
        if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 5, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 4, 5, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 2, 5, 3, regions_begin, regions_end) );
        }
        else //split edge 12, introduce line 40
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 5, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 4, 5, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 4, 2, 5, 3, regions_begin, regions_end) );
        }
      }
      else //split edge 03, introduce line 62
      {
        RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
        if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
        {
          //we have diag12_len > diag13_len > diag03_len > diag02_len alreday, hence this case is bogus!
          assert( false && "Logic error: diag02 > diag12 not possible here!");
        }
        else //split edge 12, introduce line 40
        {
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 7, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 4, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 4, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 2, 3, regions_begin, regions_end) );
          RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 4, 2, 6, regions_begin, regions_end) );
        }
      }
    }
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error edge_refine_tetrahedron_4(viennagrid_bool       * edge_refinement_tags,
                                           viennagrid_element_id * vertex_on_edge_ids,
                                           viennagrid_element_id * old_to_new_vertex_map,
                                           viennagrid_element_id * vertices_begin,
                                           viennagrid_element_id * edges_begin,
                                           viennagrid_region *     regions_begin,
                                           viennagrid_region *     regions_end,
                                           viennagrid_mesh         output_mesh)
{
  viennagrid_element_id vertices[4];
  viennagrid_element_id ordered_vertices[8];

  vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
  vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
  vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
  vertices[3] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[3])];



  //with e0
  if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] == VIENNAGRID_FALSE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[1];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[3];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[0];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

      edge_refine_tetrahedron_4_2(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] == VIENNAGRID_FALSE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[1];
      ordered_vertices[1] = vertices[2];
      ordered_vertices[2] = vertices[0];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[3];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];

      edge_refine_tetrahedron_4_2(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[2];
      ordered_vertices[2] = vertices[3];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_FALSE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[0];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];

      edge_refine_tetrahedron_4_2(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[0];
      ordered_vertices[1] = vertices[3];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[2];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[3];
      ordered_vertices[2] = vertices[0];
      ordered_vertices[3] = vertices[1];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_FALSE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[2];
      ordered_vertices[1] = vertices[1];
      ordered_vertices[2] = vertices[3];
      ordered_vertices[3] = vertices[0];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[3];
      ordered_vertices[1] = vertices[2];
      ordered_vertices[2] = vertices[1];
      ordered_vertices[3] = vertices[0];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_FALSE)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_FALSE)
    {
      ordered_vertices[0] = vertices[1];
      ordered_vertices[1] = vertices[3];
      ordered_vertices[2] = vertices[2];
      ordered_vertices[3] = vertices[0];
      ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
      ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];

      edge_refine_tetrahedron_4_1(output_mesh, ordered_vertices, regions_begin, regions_end);
    }
    else
    {
      assert(false && "Logic error: No edge for refinement found!");
    }
  }
  else
  {
    assert(false && "Logic error: No edge for refinement found!");
  }

  return VIENNAGRID_SUCCESS;
}




viennagrid_error edge_refine_tetrahedron_5_1(viennagrid_mesh output_mesh,
                                             viennagrid_element_id * vertex_ids,
                                             viennagrid_region * regions_begin,
                                             viennagrid_region * regions_end)
{
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 8, 3, regions_begin, regions_end) );
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 5, 4, 2, 8, regions_begin, regions_end) );

  viennagrid_bool tmp;
  RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 3, 1, 3, &tmp) );

  if (tmp == VIENNAGRID_TRUE) //split edge 03, introduce line 61
  {
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 5, 8, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 4, 8, regions_begin, regions_end) );
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 6, 7, regions_begin, regions_end) );

    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 6, regions_begin, regions_end) );
    }
    else //split edge 12, introduce line 40
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
    }
  }
  else  //split edge 13, introduce line 70
  {
    RETURN_ON_ERROR( stable_line_is_longer(output_mesh, vertex_ids, 0, 2, 1, 2, &tmp) );
    if (tmp == VIENNAGRID_TRUE) //split edge 02, introduce line 51
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 5, 7, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 5, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 1, 4, 5, 7, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 7, 4, 5, 8, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 5, 8, regions_begin, regions_end) );
    }
    else //split edge 12, introduce line 40
    {
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 1, 4, 7, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 7, 4, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 0, 4, 5, 6, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 4, 5, 8, regions_begin, regions_end) );
      RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertex_ids, 6, 7, 4, 8, regions_begin, regions_end) );
    }
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error edge_refine_tetrahedron_5(viennagrid_bool       * edge_refinement_tags,
                                           viennagrid_element_id * vertex_on_edge_ids,
                                           viennagrid_element_id * old_to_new_vertex_map,
                                           viennagrid_element_id * vertices_begin,
                                           viennagrid_element_id * edges_begin,
                                           viennagrid_region *     regions_begin,
                                           viennagrid_region *     regions_end,
                                           viennagrid_mesh         output_mesh)
{
  viennagrid_element_id vertices[4];
  viennagrid_element_id ordered_vertices[9];

  vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
  vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
  vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
  vertices[3] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[3])];


  if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] == VIENNAGRID_FALSE)
  {
    ordered_vertices[0] = vertices[0];
    ordered_vertices[1] = vertices[1];
    ordered_vertices[2] = vertices[2];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
    ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
    ordered_vertices[8] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];

    edge_refine_tetrahedron_5_1(output_mesh, ordered_vertices, regions_begin, regions_end);
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] == VIENNAGRID_FALSE)
  {
    ordered_vertices[0] = vertices[2];
    ordered_vertices[1] = vertices[0];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
    ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
    ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    ordered_vertices[8] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];

    edge_refine_tetrahedron_5_1(output_mesh, ordered_vertices, regions_begin, regions_end);
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])] == VIENNAGRID_FALSE)
  {
    ordered_vertices[0] = vertices[0];
    ordered_vertices[1] = vertices[3];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[2];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
    ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
    ordered_vertices[8] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];

    edge_refine_tetrahedron_5_1(output_mesh, ordered_vertices, regions_begin, regions_end);
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[3])] == VIENNAGRID_FALSE)
  {
    ordered_vertices[0] = vertices[1];
    ordered_vertices[1] = vertices[2];
    ordered_vertices[2] = vertices[0];
    ordered_vertices[3] = vertices[3];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
    ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
    ordered_vertices[8] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];

    edge_refine_tetrahedron_5_1(output_mesh, ordered_vertices, regions_begin, regions_end);
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[4])] == VIENNAGRID_FALSE)
  {
    ordered_vertices[0] = vertices[1];
    ordered_vertices[1] = vertices[3];
    ordered_vertices[2] = vertices[2];
    ordered_vertices[3] = vertices[0];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];
    ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
    ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    ordered_vertices[8] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];

    edge_refine_tetrahedron_5_1(output_mesh, ordered_vertices, regions_begin, regions_end);
  }
  else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[5])] == VIENNAGRID_FALSE)
  {
    ordered_vertices[0] = vertices[3];
    ordered_vertices[1] = vertices[2];
    ordered_vertices[2] = vertices[1];
    ordered_vertices[3] = vertices[0];
    ordered_vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
    ordered_vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
    ordered_vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    ordered_vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    ordered_vertices[8] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];

    edge_refine_tetrahedron_5_1(output_mesh, ordered_vertices, regions_begin, regions_end);
  }
  else
  {
    assert(false && "Logic error: No edge for refinement found!");
  }

  return VIENNAGRID_SUCCESS;
}




viennagrid_error edge_refine_tetrahedron_6(viennagrid_bool       * /*edge_refinement_tags*/,
                                           viennagrid_element_id * vertex_on_edge_ids,
                                           viennagrid_element_id * old_to_new_vertex_map,
                                           viennagrid_element_id * vertices_begin,
                                           viennagrid_element_id * edges_begin,
                                           viennagrid_region *     regions_begin,
                                           viennagrid_region *     regions_end,
                                           viennagrid_mesh         output_mesh)
{
  viennagrid_dimension geo_dim;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(output_mesh, &geo_dim) );

  viennagrid_element_id vertices[10];

  vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
  vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
  vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
  vertices[3] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[3])];


  vertices[4] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
  vertices[5] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
  vertices[6] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
  vertices[7] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[3])];
  vertices[8] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[4])];
  vertices[9] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[5])];


  //
  // Step 2: Add new cells to new mesh:
  //

  //0-4-5-6:
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 0, 4, 5, 6, regions_begin, regions_end) );

  //1-7-4-8:
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 1, 7, 4, 8, regions_begin, regions_end) );

  //2-5-7-9:
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 2, 5, 7, 9, regions_begin, regions_end) );

  //3-8-6-9:
  RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 3, 8, 6, 9, regions_begin, regions_end) );


  viennagrid_numeric diag58, diag67, diag49;
  viennagrid_numeric *pt0, *pt1;

  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(output_mesh, vertices[5], &pt0) );
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(output_mesh, vertices[8], &pt1) );
  RETURN_ON_ERROR( viennagrid_point_distance_2(geo_dim, pt0, pt1, &diag58) );

  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(output_mesh, vertices[6], &pt0) );
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(output_mesh, vertices[7], &pt1) );
  RETURN_ON_ERROR( viennagrid_point_distance_2(geo_dim, pt0, pt1, &diag67) );

  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(output_mesh, vertices[4], &pt0) );
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(output_mesh, vertices[9], &pt1) );
  RETURN_ON_ERROR( viennagrid_point_distance_2(geo_dim, pt0, pt1, &diag49) );


  if ( (diag58 <= diag67) && (diag58 <= diag49) )  //diag58 is shortest: keep it, split others
  {
    //4-8-5-6:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 4, 8, 5, 6, regions_begin, regions_end) );

    //4-8-7-5:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 4, 8, 7, 5, regions_begin, regions_end) );

    //7-8-9-5:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 7, 8, 9, 5, regions_begin, regions_end) );

    //8-6-9-5:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 8, 6, 9, 5, regions_begin, regions_end) );
  }
  else if ( (diag67 <= diag58) && (diag67 <= diag49) ) //diag67 is shortest: keep it, split others
  {
    //4-7-6-8:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 4, 7, 6, 8, regions_begin, regions_end) );

    //4-7-5-6:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 4, 7, 5, 6, regions_begin, regions_end) );

    //7-9-6-8:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 7, 9, 6, 8, regions_begin, regions_end) );

    //7-9-5-6:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 7, 9, 5, 6, regions_begin, regions_end) );
  }
  else //keep shortest diagonal diag49
  {
    //4-9-6-8:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 4, 9, 6, 8, regions_begin, regions_end) );

    //4-9-5-6:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 4, 9, 5, 6, regions_begin, regions_end) );

    //4-7-9-8:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 4, 7, 9, 8, regions_begin, regions_end) );

    //4-7-5-9:
    RETURN_ON_ERROR( viennagrid_mesh_element_create_refinement_tetrahedron(output_mesh, vertices, 4, 7, 5, 99, regions_begin, regions_end) );
  }

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

  viennagrid_region * regions_begin = regions.empty() ? NULL : &regions[0];
  viennagrid_region * regions_end = regions_begin ? regions_begin + regions.size() : NULL;



  switch (num_edges_to_refine)
  {
    case 0:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_0(edge_refinement_tags, vertex_on_edge_ids, old_to_new_vertex_map,
                                                 vertices_begin, edges_begin, regions_begin, regions_end, output_mesh) );
      break;
    }
    case 1:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_1(edge_refinement_tags, vertex_on_edge_ids, old_to_new_vertex_map,
                                                 vertices_begin, edges_begin, regions_begin, regions_end, output_mesh) );
      break;
    }
    case 2:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_2(edge_refinement_tags, vertex_on_edge_ids, old_to_new_vertex_map,
                                                 vertices_begin, edges_begin, regions_begin, regions_end, output_mesh) );
      break;
    }
    case 3:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_3(edge_refinement_tags, vertex_on_edge_ids, old_to_new_vertex_map,
                                                 vertices_begin, edges_begin, regions_begin, regions_end, output_mesh) );
      break;
    }
    case 4:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_4(edge_refinement_tags, vertex_on_edge_ids, old_to_new_vertex_map,
                                                 vertices_begin, edges_begin, regions_begin, regions_end, output_mesh) );
      break;
    }
    case 5:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_5(edge_refinement_tags, vertex_on_edge_ids, old_to_new_vertex_map,
                                                 vertices_begin, edges_begin, regions_begin, regions_end, output_mesh) );
      break;
    }
    case 6:
    {
      RETURN_ON_ERROR( edge_refine_tetrahedron_6(edge_refinement_tags, vertex_on_edge_ids, old_to_new_vertex_map,
                                                 vertices_begin, edges_begin, regions_begin, regions_end, output_mesh) );
      break;
    }

    default: //nothing to do...
            break;
  }

  return VIENNAGRID_SUCCESS;
}
