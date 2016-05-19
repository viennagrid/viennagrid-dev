#include "viennagrid/viennagrid.h"
#include "../mesh.hpp"
#include "../common.hpp"
#include <cmath>


static viennagrid_error edge_refine_triangle(viennagrid_mesh         mesh,
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

  viennagrid_element_id new_cell_id;
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

  viennagrid_region_id *regions_begin, *regions_end;
  RETURN_ON_ERROR( viennagrid_element_regions_get(mesh, cell_id, &regions_begin, &regions_end); );

  viennagrid_region cell_region = 0;
  if (regions_begin && regions_end && (regions_end-regions_begin != 0))
  {
    RETURN_ON_ERROR( viennagrid_mesh_region_get_or_create(output_mesh, *regions_begin, &cell_region) );
  }

  viennagrid_element_id new_cell_id;

  if (num_edges_to_refine == 0)
  {
    new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
    new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
    new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];

    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
    for (std::size_t i = 0; i != regions.size(); ++i)
      RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );
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
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );
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
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );
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
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );
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
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
    for (std::size_t i = 0; i != regions.size(); ++i)
      RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

    if (d_X1_2 < d_X2_1)
    {
      new_vertices[0] = vX2_id;
      new_vertices[1] = vX1_id;
      new_vertices[2] = v2_id;
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

      new_vertices[0] = vX1_id;
      new_vertices[1] = v1_id;
      new_vertices[2] = v2_id;
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );
    }
    else
    {
      new_vertices[0] = vX2_id;
      new_vertices[1] = vX1_id;
      new_vertices[2] = v1_id;
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

      new_vertices[0] = vX2_id;
      new_vertices[1] = v1_id;
      new_vertices[2] = v2_id;
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
      for (std::size_t i = 0; i != regions.size(); ++i)
        RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );
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
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
    for (std::size_t i = 0; i != regions.size(); ++i)
      RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
    new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
    for (std::size_t i = 0; i != regions.size(); ++i)
      RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
    for (std::size_t i = 0; i != regions.size(); ++i)
      RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, &new_cell_id) );
    for (std::size_t i = 0; i != regions.size(); ++i)
      RETURN_ON_ERROR( viennagrid_region_element_add(regions[i], new_cell_id); );
  }

  return VIENNAGRID_SUCCESS;
}



//
///////////// Top-level refinement routine
//

/** refine a mesh according to edge-based refinement tags and new vertex locations */
viennagrid_error viennagrid_mesh_refine_edges(viennagrid_mesh      mesh,
                                              viennagrid_bool    * edge_refinement_tags,
                                              viennagrid_numeric * vertex_locations,
                                              viennagrid_mesh      output_mesh)
{
  viennagrid_mesh root_in, root_out;
  RETURN_ON_ERROR( viennagrid_mesh_root_mesh_get(       mesh, &root_in) );
  RETURN_ON_ERROR( viennagrid_mesh_root_mesh_get(output_mesh, &root_out) );
  if (root_in == root_out)
    return VIENNAGRID_ERROR_ROOT_MESHES_NEED_TO_BE_DIFFERENT;

  viennagrid_dimension dim;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dim) );
  std::vector<viennagrid_numeric> new_vertex_coords(dim);

  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_set(output_mesh, dim) );

  //
  // Step 1: Copy existing vertices over to new mesh
  //
  viennagrid_element_id *vertices_begin, *vertices_end;
  RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, 0, &vertices_begin, &vertices_end); );
  std::vector<viennagrid_element_id> old_vertex_to_new_vertex_id(vertices_end - vertices_begin);

  for (viennagrid_element_id *vit  = vertices_begin;
                              vit != vertices_end;
                            ++vit)
  {
    viennagrid_numeric *coords_ptr;
    RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *vit, &coords_ptr) );
    RETURN_ON_ERROR( viennagrid_mesh_vertex_create(output_mesh, coords_ptr, &(old_vertex_to_new_vertex_id[viennagrid_index_from_element_id(*vit)])) );
  }

  //
  // Step 2: Add new vertices from tagged edges
  //
  viennagrid_element_id *edges_begin, *edges_end;
  RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, 1, &edges_begin, &edges_end); );

  std::vector<viennagrid_element_id> new_vertices(edges_end - edges_begin);

  for (viennagrid_element_id *eit  = edges_begin;
                              eit != edges_end;
                            ++eit)
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(*eit)])
    {
      if (vertex_locations) // new vertex is not in center of edge
      {
        RETURN_ON_ERROR( viennagrid_mesh_vertex_create(output_mesh,
                                                       vertex_locations + dim*viennagrid_index_from_element_id(*eit),
                                                       &(new_vertices[viennagrid_index_from_element_id(*eit)])) );
      }
      else
      {
        RETURN_ON_ERROR( viennagrid_element_centroid(mesh, *eit, &(new_vertex_coords[0])) );
        RETURN_ON_ERROR( viennagrid_mesh_vertex_create(output_mesh, &(new_vertex_coords[0]), &(new_vertices[viennagrid_index_from_element_id(*eit)])) );
      }
    }
  }

  //
  // Step 3: Add cells to refined mesh (with parent-child information)
  //
  viennagrid_element_id *cells_begin, *cells_end;
  viennagrid_dimension cell_dim;
  RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(mesh, &cell_dim) );
  if (cell_dim < 1)
    return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

  RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, cell_dim, &cells_begin, &cells_end) );
  for (viennagrid_element_id *cit  = cells_begin;
                              cit != cells_end;
                            ++cit)
  {
    viennagrid_element_type element_type;
    RETURN_ON_ERROR( viennagrid_element_type_get(mesh, *cit, &element_type) );

    switch (element_type)
    {
    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      RETURN_ON_ERROR( edge_refine_triangle(mesh,
                                            edge_refinement_tags,
                                            &(new_vertices[0]),
                                            &(old_vertex_to_new_vertex_id[0]),
                                            *cit,
                                            output_mesh) ); break;
    default:
      return VIENNAGRID_ERROR_UNSUPPORTED_ELEMENT_TYPE;
    }
  }


  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_mesh_refine_create_uniformly_edge_flags(viennagrid_mesh         mesh,
                                                                    viennagrid_int          elements_to_refine_count,
                                                                    viennagrid_element_id * elements_to_refine,
                                                                    viennagrid_bool *       edge_refinement_tags)
{
  if (edge_refinement_tags)
  {
    for (viennagrid_element_id * eit = elements_to_refine; eit != elements_to_refine+elements_to_refine_count; ++eit)
    {
      viennagrid_dimension topo_dim = TOPODIM(*eit);

      if (topo_dim == 0)
      {
        // TODO return error?
      }
      else if (topo_dim == 1)
      {
        edge_refinement_tags[ INDEX(*eit) ] = VIENNAGRID_TRUE;
      }
      else
      {
        viennagrid_element_id * edges_begin;
        viennagrid_element_id * edges_end;

        RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, *eit, 1, &edges_begin, &edges_end) );
        for (viennagrid_element_id * it = edges_begin; it != edges_end; ++it)
          edge_refinement_tags[ INDEX(*it) ] = VIENNAGRID_TRUE;
      }
    }
  }

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_refine_ensure_longest_edge(viennagrid_mesh   mesh,
                                                            viennagrid_bool * edge_refinement_tags)
{
  if (edge_refinement_tags)
  {
    viennagrid_dimension cell_dimension;
    RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(mesh, &cell_dimension) );


    bool something_changed = true;

    while (something_changed)
    {
      something_changed = false;

      viennagrid_element_id * cells_begin;
      viennagrid_element_id * cells_end;
      RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, cell_dimension, &cells_begin, &cells_end) );
      for (viennagrid_element_id * cit = cells_begin; cit != cells_end; ++cit)
      {
        viennagrid_element_id * edges_begin;
        viennagrid_element_id * edges_end;
        RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, *cit, 1, &edges_begin, &edges_end) );
        viennagrid_element_id * eit = edges_begin;
        for (; eit != edges_end; ++eit)
        {
          if (edge_refinement_tags[INDEX(*eit)] == VIENNAGRID_TRUE)
            break;
        }

        if (eit != edges_end)
        {
          viennagrid_element_id longest_edge_id = -1;
          viennagrid_numeric longest_edge_len = 0;

          for (eit = edges_begin; eit != edges_end; ++eit)
          {
            viennagrid_numeric edge_len;
            RETURN_ON_ERROR( viennagrid_element_volume(mesh, *eit, &edge_len) );
            if (edge_len > longest_edge_len)
            {
              longest_edge_len = edge_len;
              longest_edge_id = *eit;
            }
          }

          if (edge_refinement_tags[INDEX(longest_edge_id)] == VIENNAGRID_FALSE)
          {
            edge_refinement_tags[INDEX(longest_edge_id)] = VIENNAGRID_TRUE;
            something_changed = true;
          }
        }
      }
    }

  }

  return VIENNAGRID_SUCCESS;
}





viennagrid_error viennagrid_mesh_refine_create_hyperplane_edge_flags(viennagrid_mesh      mesh,
                                                                     viennagrid_numeric * hyperplane_point,
                                                                     viennagrid_numeric * hyperplane_normal,
                                                                     viennagrid_bool    * edge_refinement_tags,
                                                                     viennagrid_numeric * vertex_locations,
                                                                     viennagrid_numeric   tolerance)
{
  viennagrid_dimension dim;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dim) );

  viennagrid_element_id * edges_begin;
  viennagrid_element_id * edges_end;
  RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, 1, &edges_begin, &edges_end) );

  for (viennagrid_element_id * eit = edges_begin; eit != edges_end; ++eit)
  {
    viennagrid_element_id * vertices_begin;
    viennagrid_element_id * vertices_end;
    RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, *eit, 0, &vertices_begin, &vertices_end) );

    viennagrid_numeric * pt0;
    viennagrid_numeric * pt1;

    RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *(vertices_begin+0), &pt0) );
    RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *(vertices_begin+1), &pt1) );

    std::vector<viennagrid_numeric> tmp(dim);

    viennagrid_numeric qd;
    viennagrid_numeric pd;

    RETURN_ON_ERROR( viennagrid_subtract(dim, pt0, hyperplane_point, &tmp[0]) );
    RETURN_ON_ERROR( viennagrid_inner_prod(dim, hyperplane_normal, &tmp[0], &qd) );

    RETURN_ON_ERROR( viennagrid_subtract(dim, pt1, hyperplane_point, &tmp[0]) );
    RETURN_ON_ERROR( viennagrid_inner_prod(dim, hyperplane_normal, &tmp[0], &pd) );

    RETURN_ON_ERROR( viennagrid_element_volume(mesh, *eit, &tmp[0]) );
    tolerance *= tmp[0];

    viennagrid_numeric smaller = (qd < pd) ? qd : pd;
    viennagrid_numeric larger = (qd < pd) ? pd : qd;

    if (smaller < -tolerance && larger > tolerance)
    {
      edge_refinement_tags[INDEX(*eit)] = VIENNAGRID_TRUE;

      RETURN_ON_ERROR( viennagrid_subtract(dim, pt1, pt0, &tmp[0]) );
      RETURN_ON_ERROR( viennagrid_prod(dim, &tmp[0], pd/(pd-qd), &tmp[0]) );
      RETURN_ON_ERROR( viennagrid_subtract(dim, pt1, &tmp[0], &tmp[0]) );

      std::copy( tmp.begin(), tmp.end(), vertex_locations+INDEX(*eit)*dim );
    }
    else
      edge_refinement_tags[INDEX(*eit)] = VIENNAGRID_FALSE;
  }

  return VIENNAGRID_SUCCESS;
}


