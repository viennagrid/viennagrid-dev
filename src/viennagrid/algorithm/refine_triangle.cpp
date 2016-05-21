#include "refine.hpp"

viennagrid_error edge_refine_triangle(viennagrid_mesh         mesh,
                                      viennagrid_bool       * edge_refinement_tags,
                                      viennagrid_element_id * vertex_on_edge_ids,
                                      viennagrid_element_id * old_to_new_vertex_map,
                                      viennagrid_element_id   cell_id,
                                      viennagrid_mesh         output_mesh)
{
  viennagrid_int num_edges_to_refine = 0;
  viennagrid_element_id new_vertices[3];

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

  if (num_edges_to_refine == 0)
  {
    new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
    new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
    new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];

    RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );
  }
  else if (num_edges_to_refine == 1) // note: for (hopefully) better maintainability and better understanding each case is coded explicitly
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])])
    {
      /*
      *           2
      *         /   \
      *        /     \
      *       0 - X - 1
      */
      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])])
    {
      /*
      *            2
      *          /   \
      *         X     \
      *        /       \
      *       0 - - - - 1
      */
      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])])
    {
      /*
      *            2
      *          /   \
      *         /     X
      *        /       \
      *       0 - - - - 1
      */
      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );
    }
  }
  else if (num_edges_to_refine == 2) // note: for (hopefully) better maintainability and better understanding each case is coded explicitly
  {
    viennagrid_element_id v0_id = -1;
    viennagrid_element_id v1_id = -1;
    viennagrid_element_id v2_id = -1;

    viennagrid_element_id vX1_id = -1;
    viennagrid_element_id vX2_id = -1;

    /*
    * assign indices that the triangle locally looks like:
    *
    *             2
    *           /  \
    *          /    \
    *        X2      \
    *        /        \
    *       0 -- X1 -- 1
    */

    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] && edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])])
    {
      /*
       * vertex index 0 -> 0, 1 -> 1, 2 -> 2
       * vX1 = edge 0
       * vX2 = edge 1
       */

      v0_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      v1_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      v2_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];

      vX1_id = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      vX2_id = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] && edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])])
    {
      /*
       * vertex index 1 -> 0, 2 -> 1, 0 -> 2
       * vX1 = edge 2
       * vX2 = edge 0
       */

      v0_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      v1_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      v2_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];

      vX1_id = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      vX2_id = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] && edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])])
    {
      /*
       * vertex index 2 -> 0, 0 -> 1, 1 -> 2
       * vX1 = edge 1
       * vX2 = edge 2
       */

      v0_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      v1_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      v2_id = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];

      vX1_id = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      vX2_id = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    }


    viennagrid_numeric d_X1_2;
    viennagrid_numeric d_X2_1;

    RETURN_ON_ERROR( viennagrid_distance_2(dim, coords+vX1_id*dim, coords+v2_id*dim, &d_X1_2) );
    RETURN_ON_ERROR( viennagrid_distance_2(dim, coords+vX2_id*dim, coords+v1_id*dim, &d_X2_1) );

    new_vertices[0] = v0_id;
    new_vertices[1] = vX1_id;
    new_vertices[2] = vX2_id;
    RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

    if (d_X1_2 < d_X2_1)
    {
      new_vertices[0] = vX2_id;
      new_vertices[1] = vX1_id;
      new_vertices[2] = v2_id;
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

      new_vertices[0] = vX1_id;
      new_vertices[1] = v1_id;
      new_vertices[2] = v2_id;
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );
    }
    else
    {
      new_vertices[0] = vX2_id;
      new_vertices[1] = vX1_id;
      new_vertices[2] = v1_id;
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

      new_vertices[0] = vX2_id;
      new_vertices[1] = v1_id;
      new_vertices[2] = v2_id;
      RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );
    }
  }
  else // refine each edge
  {
    /*
    *            2
    *          /   \
    *         X     X
    *        /       \
    *       0 -- X -- 1
    */
    new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
    new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
    new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create_with_regions(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, regions_begin, regions_end, NULL) );
  }

  return VIENNAGRID_SUCCESS;
}
