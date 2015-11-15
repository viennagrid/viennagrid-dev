#include "viennagrid/viennagrid.h"
#include "../mesh.hpp"
#include "../common.hpp"
#include <cmath>


static viennagrid_error refine_triangle(viennagrid_mesh mesh,
                                        viennagrid_bool       * edge_refinement_tags,
                                        viennagrid_element_id * vertex_on_edge_ids,
                                        viennagrid_element_id * old_to_new_vertex_map,
                                        viennagrid_element_id cell_id,
                                        viennagrid_mesh output_mesh)
{
  viennagrid_int num_edges_to_refine = 0;
  viennagrid_element_id new_vertices[3];

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

  if (num_edges_to_refine == 0)
  {
    new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
    new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
    new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];

    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );
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
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );
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
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );
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
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );
    }
  }
  else if (num_edges_to_refine == 2) // note: for (hopefully) better maintainability and better understanding each case is coded explicitly
  {
    if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] && edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])])
    {
      /*
      *            2
      *          /   \
      *         X     \
      *        /       \
      *       0 -- X -- 1
      */
      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      // TODO: Use stability criterion, e.g. split longer edge? This should be optional.
      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[0])] && edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])])
    {
      /*
      *            2
      *          /   \
      *         /     X
      *        /       \
      *       0 -- X -- 1
      */
      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      // TODO: Use stability criterion, e.g. split longer edge? This should be optional.
      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
      new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );
    }
    else if (edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[1])] && edge_refinement_tags[viennagrid_index_from_element_id(edges_begin[2])])
    {
      /*
      *            2
      *          /   \
      *         X     X
      *        /       \
      *       0 - - - - 1
      */
      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
      new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      // TODO: Use stability criterion, e.g. split longer edge? This should be optional.
      new_vertices[0] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[0])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

      new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
      new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
      new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
      RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );
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
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    new_vertices[1] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[1])];
    new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[0])];
    new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    new_vertices[2] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );

    new_vertices[0] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[1])];
    new_vertices[1] = vertex_on_edge_ids[viennagrid_index_from_element_id(edges_begin[2])];
    new_vertices[2] = old_to_new_vertex_map[viennagrid_index_from_element_id(vertices_begin[2])];
    RETURN_ON_ERROR( viennagrid_mesh_element_create(output_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices, NULL) );
  }

  return VIENNAGRID_SUCCESS;
}



//
///////////// Top-level refinement routine
//

/** refine a mesh according to edge-based refinement tags and new vertex locations */
viennagrid_error viennagrid_mesh_refine(viennagrid_mesh mesh,
                                        viennagrid_bool    * edge_refinement_tags,
                                        viennagrid_numeric * vertex_locations,
                                        viennagrid_mesh output_mesh)
{
  viennagrid_mesh root_in, root_out;
  RETURN_ON_ERROR( viennagrid_mesh_root_mesh_get(       mesh, &root_in) );
  RETURN_ON_ERROR( viennagrid_mesh_root_mesh_get(output_mesh, &root_out) );
  if (root_in == root_out)
    return VIENNAGRID_ERROR_ROOT_MESHES_NEED_TO_BE_DIFFERENT;

  viennagrid_dimension dim;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dim) );
  std::vector<viennagrid_numeric> new_vertex_coords(dim);

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
    if (vertex_locations) // new vertex is not in center of edge
    {
      // TODO: Implement
      return VIENNAGRID_ERROR_UNSPECIFIED_ERROR;
    }
    else
    {
      if (edge_refinement_tags[viennagrid_index_from_element_id(*eit)])
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
      RETURN_ON_ERROR( refine_triangle(mesh,
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
