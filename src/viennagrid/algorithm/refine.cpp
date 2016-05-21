#include "viennagrid/viennagrid.h"
#include "../mesh.hpp"
#include "../common.hpp"
#include "refine.hpp"
#include <cmath>




viennagrid_error viennagrid_mesh_element_create_with_regions(viennagrid_mesh mesh,
                                                             viennagrid_element_type element_type,
                                                             viennagrid_int vertex_count,
                                                             viennagrid_element_id * vertex_ids,
                                                             viennagrid_region * regions_begin,
                                                             viennagrid_region * regions_end,
                                                             viennagrid_element_id * element_id)
{
  viennagrid_element_id tmp;
  RETURN_ON_ERROR( viennagrid_mesh_element_create(mesh, element_type, vertex_count, vertex_ids, &tmp) );

  if (regions_begin && regions_end)
  {
    for (viennagrid_region * rit = regions_begin; rit != regions_end; ++rit)
      RETURN_ON_ERROR( viennagrid_region_element_add(*rit, tmp); );
  }

  if (element_id)
    *element_id = tmp;

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
    viennagrid_numeric local_tolerance = tolerance * tmp[0];

    viennagrid_numeric smaller = (qd < pd) ? qd : pd;
    viennagrid_numeric larger = (qd < pd) ? pd : qd;

    if (smaller < -local_tolerance && larger > local_tolerance)
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


