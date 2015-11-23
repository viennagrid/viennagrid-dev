/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <vector>
#include <cmath>
#include <map>

#include "viennagrid/viennagrid.h"
#include "common.hpp"
#include "mesh.hpp"


struct viennagrid_copy_map_ : public reference_counted
{
  viennagrid_copy_map_() : src_mesh(NULL), dst_mesh(NULL), max_vertex_distance(-1), copy_region_information(true) {}
  viennagrid_copy_map_(viennagrid_mesh src_mesh_, viennagrid_mesh dst_mesh_, bool copy_region_information_ = true) : src_mesh(src_mesh_), dst_mesh(dst_mesh_), max_vertex_distance(-1), copy_region_information(copy_region_information_)
  {
    init();
  }
  viennagrid_copy_map_(viennagrid_mesh src_mesh_, viennagrid_mesh dst_mesh_, viennagrid_numeric max_vertex_distance_, bool copy_region_information_ = true) : src_mesh(src_mesh_), dst_mesh(dst_mesh_), max_vertex_distance(max_vertex_distance_), copy_region_information(copy_region_information_)
  {
    init();
  }

  ~viennagrid_copy_map_()
  {
    viennagrid_mesh_release(src_mesh);
    viennagrid_mesh_release(dst_mesh);
  }

  void init()
  {
    viennagrid_mesh_retain(src_mesh);
    viennagrid_mesh_retain(dst_mesh);

    viennagrid_dimension src_dimension;
    viennagrid_mesh_geometric_dimension_get(src_mesh, &src_dimension);

    viennagrid_dimension dst_dimension;
    viennagrid_mesh_geometric_dimension_get(dst_mesh, &dst_dimension);

    if (dst_dimension == VIENNAGRID_INVALID_DIMENSION)
      viennagrid_mesh_geometric_dimension_set(dst_mesh, src_dimension);
  }

  viennagrid_mesh src_mesh;
  viennagrid_mesh dst_mesh;
  viennagrid_numeric max_vertex_distance;
  bool copy_region_information;

  std::map<viennagrid_element_id, viennagrid_element_id> vertex_map;
};

typedef viennagrid_copy_map_ * viennagrid_copy_map;



viennagrid_error viennagrid_copy_map_create(viennagrid_mesh src_mesh, viennagrid_mesh dst_mesh, viennagrid_copy_map * copy_map)
{
  if (!src_mesh || !dst_mesh) return VIENNAGRID_ERROR_INVALID_MESH;

  viennagrid_dimension src_dimension;
  viennagrid_dimension dst_dimension;

  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(src_mesh, &src_dimension) );
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(dst_mesh, &dst_dimension) );

  if ((dst_dimension != VIENNAGRID_INVALID_DIMENSION) && (dst_dimension != src_dimension)) return VIENNAGRID_ERROR_GEOMETRIC_DIMENSION_MISMATCH;

  if (copy_map)
    *copy_map = new viennagrid_copy_map_(src_mesh, dst_mesh);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_copy_map_retain(viennagrid_copy_map copy_map)
{
  retain(copy_map);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_copy_map_release(viennagrid_copy_map copy_map)
{
  release(copy_map);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_copy_map_copy_region_information_set(viennagrid_copy_map copy_map, viennagrid_bool copy)
{
  copy_map->copy_region_information = (copy == VIENNAGRID_TRUE);
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_copy_map_copy_region_information_get(viennagrid_copy_map copy_map, viennagrid_bool * copy)
{
  if (copy)
    *copy = copy_map->copy_region_information ? VIENNAGRID_TRUE : VIENNAGRID_FALSE;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_copy_map_max_vertex_distance_set(viennagrid_copy_map copy_map, viennagrid_numeric max_vertex_distance)
{
  copy_map->max_vertex_distance = max_vertex_distance;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_copy_map_max_vertex_distance_get(viennagrid_copy_map copy_map, viennagrid_numeric * max_vertex_distance)
{
  if (max_vertex_distance)
    *max_vertex_distance = copy_map->max_vertex_distance;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_copy_map_element_copy(viennagrid_copy_map copy_map,
                                                  viennagrid_element_id source_element_id,
                                                  viennagrid_element_id * destination_element_id)
{
  viennagrid_element_type element_type;
  viennagrid_element_type_get(copy_map->src_mesh, source_element_id, &element_type);

  if (element_type == VIENNAGRID_ELEMENT_TYPE_VERTEX)
  {
    std::map<viennagrid_element_id, viennagrid_element_id>::const_iterator it = copy_map->vertex_map.find(source_element_id);
    if (it != copy_map->vertex_map.end())
    {
      *destination_element_id = (*it).second;
      return VIENNAGRID_SUCCESS;
    }

    viennagrid_element_id * vertices_begin;
    viennagrid_element_id * vertices_end;
    viennagrid_mesh_elements_get(copy_map->dst_mesh, 0, &vertices_begin, &vertices_end);

    viennagrid_numeric * source_vertex_coords;
    viennagrid_mesh_vertex_coords_get(copy_map->src_mesh, source_element_id, &source_vertex_coords);

    if ((copy_map->max_vertex_distance > 0) && (vertices_end-vertices_begin > 0))
    {
      viennagrid_numeric * vertex_coords;
      viennagrid_mesh_vertex_coords_pointer(copy_map->dst_mesh, &vertex_coords);

      viennagrid_dimension dimension;
      viennagrid_mesh_geometric_dimension_get(copy_map->dst_mesh, &dimension);


      for (viennagrid_element_id * vit = vertices_begin; vit != vertices_end; ++vit)
      {
        viennagrid_numeric distance;
        viennagrid_distance_2(dimension, source_vertex_coords, vertex_coords+dimension*(*vit), &distance);

        if ( distance < copy_map->max_vertex_distance )
        {
          *destination_element_id = *vit;
          return VIENNAGRID_SUCCESS;
        }
      }
    }

    viennagrid_element_id new_vertex_id;
    viennagrid_mesh_vertex_create(copy_map->dst_mesh, source_vertex_coords, &new_vertex_id);
    copy_map->vertex_map[source_element_id] = new_vertex_id;

    if (copy_map->copy_region_information)
      viennagrid_element_copy_region_information(copy_map->src_mesh, source_element_id, copy_map->dst_mesh, new_vertex_id);

    if (destination_element_id)
      *destination_element_id = new_vertex_id;
  }
  else
  {
    viennagrid_element_id * vertices_begin;
    viennagrid_element_id * vertices_end;
    viennagrid_element_boundary_elements(copy_map->src_mesh, source_element_id, 0, &vertices_begin, &vertices_end);

    std::vector<viennagrid_element_id> new_vertices( vertices_end-vertices_begin );
    for (viennagrid_element_id * vit = vertices_begin; vit != vertices_end; ++vit)
      viennagrid_copy_map_element_copy(copy_map, *vit, &new_vertices[vit-vertices_begin]);

    viennagrid_element_id new_element_id;
    viennagrid_mesh_element_create(copy_map->dst_mesh, element_type, new_vertices.size(), &new_vertices[0], &new_element_id);

    if (copy_map->copy_region_information)
      viennagrid_element_copy_region_information(copy_map->src_mesh, source_element_id, copy_map->dst_mesh, new_element_id);

    if (destination_element_id)
      *destination_element_id = new_element_id;
  }



  return VIENNAGRID_SUCCESS;
}












viennagrid_error viennagrid_copy(viennagrid_dimension       dimension,
                                 viennagrid_numeric const * v,
                                 viennagrid_numeric       * result)
{
  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = v[+i];
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_add(viennagrid_dimension dimension,
                                viennagrid_numeric const * v1,
                                viennagrid_numeric const * v2,
                                viennagrid_numeric       * result)
{
  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = v1[+i] + v2[+i];
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_subtract(viennagrid_dimension dimension,
                                     viennagrid_numeric const * v1,
                                     viennagrid_numeric const * v2,
                                     viennagrid_numeric       * result)
{
  for (viennagrid_dimension i = 0; i != dimension; ++i)
    result[+i] = v1[+i] - v2[+i];
  return VIENNAGRID_SUCCESS;
}




/* computes the cross-product of two vectors: result = v1 x v2  (3d only) */
viennagrid_error viennagrid_cross_prod(viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the inner product of two vectors: result = (v1, v2) */
viennagrid_error viennagrid_inner_prod(viennagrid_dimension dimension,
                                       viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    *result = 0;
    for (viennagrid_dimension i = 0; i < dimension; ++i)
      *result += v1[+i] * v2[+i];
  }

  return VIENNAGRID_SUCCESS;
}


/* computes the l1-norm of a vector: result = ||v1||_1 */
viennagrid_error viennagrid_norm_1(viennagrid_dimension dimension,
                                   viennagrid_numeric const * v1,
                                   viennagrid_numeric       * result)
{
  if (result)
  {
    *result = 0;
    for (viennagrid_dimension i = 0; i < dimension; ++i)
      *result +=  std::abs(v1[+i]);
  }

  return VIENNAGRID_SUCCESS;
}


/* computes the l2-norm of a vector: result = ||v1||_2 */
viennagrid_error viennagrid_norm_2(viennagrid_dimension dimension,
                                   viennagrid_numeric const * v1,
                                   viennagrid_numeric       * result)
{
  if (result)
  {
    *result = 0;
    for (viennagrid_dimension i = 0; i < dimension; ++i)
      *result += v1[+i] * v1[+i];
    *result = std::sqrt(*result);
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the l2-norm of a vector: result = ||v1||_2 */
viennagrid_error viennagrid_norm_inf(viennagrid_dimension dimension,
                                     viennagrid_numeric const * v1,
                                     viennagrid_numeric       * result)
{
  if (result && dimension > 0)
  {
    *result = std::abs(v1[0]);
    for (viennagrid_dimension i = 1; i < dimension; ++i)
      *result = std::max( *result, std::abs(v1[+i]) );
  }

  return VIENNAGRID_SUCCESS;
}



/* computes the l1-distance of two vectors: result = ||v1-v2||_1 */
viennagrid_error viennagrid_distance_1(viennagrid_dimension dimension,
                                       viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    std::vector<viennagrid_numeric> v1mv2(dimension);
    viennagrid_subtract(dimension, v1, v2, &v1mv2[0]);
    viennagrid_norm_1(dimension, &v1mv2[0], result);
  }

  return VIENNAGRID_SUCCESS;
}


/* computes the l2-distance of two vectors: result = ||v1-v2||_2 */
viennagrid_error viennagrid_distance_2(viennagrid_dimension dimension,
                                       viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    std::vector<viennagrid_numeric> v1mv2(dimension);
    viennagrid_subtract(dimension, v1, v2, &v1mv2[0]);
    viennagrid_norm_2(dimension, &v1mv2[0], result);
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the inf-distance of two vectors: result = ||v1-v2||_inf */
viennagrid_error viennagrid_distance_inf(viennagrid_dimension dimension,
                                         viennagrid_numeric const * v1,
                                         viennagrid_numeric const * v2,
                                         viennagrid_numeric       * result)
{
  if (result && dimension > 0)
  {
    std::vector<viennagrid_numeric> v1mv2(dimension);
    viennagrid_subtract(dimension, v1, v2, &v1mv2[0]);
    viennagrid_norm_inf(dimension, &v1mv2[0], result);
  }

  return VIENNAGRID_SUCCESS;
}
















viennagrid_error viennagrid_mesh_extract_boundary(viennagrid_mesh volume_mesh,
                                                  viennagrid_mesh boundary_mesh,
                                                  viennagrid_dimension boundary_dimension)
{
  if (!volume_mesh)                                             return VIENNAGRID_ERROR_INVALID_MESH;
  if (!boundary_dimension)                                      return VIENNAGRID_ERROR_INVALID_MESH;
  if (!volume_mesh->valid_sparse_dimension(boundary_dimension)) return VIENNAGRID_ERROR_MESH_HAS_SPARSE_BOUNDARY_STORAGE_LAYOUT;

  viennagrid_dimension dimension;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(volume_mesh, &dimension) );

  RETURN_ON_ERROR( viennagrid_mesh_clear(boundary_mesh) );
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_set(boundary_mesh, dimension) );

  viennagrid_dimension cell_dimension;
  RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(volume_mesh, &cell_dimension) );

  if ((boundary_dimension < 0) || (boundary_dimension >= cell_dimension)) return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;


  viennagrid_copy_map copy_map;
  RETURN_ON_ERROR( viennagrid_copy_map_create(volume_mesh, boundary_mesh, &copy_map) );


  viennagrid_element_id * hull_elements_begin;
  viennagrid_element_id * hull_elements_end;
  RETURN_ON_ERROR( viennagrid_mesh_elements_get(volume_mesh, boundary_dimension, &hull_elements_begin, &hull_elements_end) );

  for (viennagrid_element_id * eit = hull_elements_begin; eit != hull_elements_end; ++eit)
  {
    viennagrid_bool is_boundary;
    RETURN_ON_ERROR( viennagrid_element_is_any_boundary(volume_mesh, *eit, &is_boundary) );

    if (is_boundary == VIENNAGRID_TRUE)
    {
      viennagrid_element_id new_element_id;
      RETURN_ON_ERROR( viennagrid_copy_map_element_copy(copy_map, *eit, &new_element_id) );
    }
  }

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_affine_transform(viennagrid_mesh mesh,
                                                  viennagrid_dimension destination_dimension,
                                                  viennagrid_numeric const * matrix,
                                                  viennagrid_numeric const * translation)
{
  if (!mesh)                        return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root())             return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;
  if ((destination_dimension <= 0)) return VIENNAGRID_ERROR_INVALID_GEOMETRIC_DIMENSION;

  if (!matrix && !translation)
    return VIENNAGRID_SUCCESS;

  viennagrid_dimension dimension;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dimension) );

  viennagrid_numeric * src_coords;
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_pointer(mesh, &src_coords) );

  viennagrid_int vertex_count;
  RETURN_ON_ERROR( viennagrid_mesh_element_count(mesh, 0, &vertex_count) );

  std::vector<viennagrid_numeric> dst_coords( vertex_count * destination_dimension );

  for (viennagrid_int index = 0; index != vertex_count; ++index)
  {
    viennagrid_numeric * src = src_coords + dimension*index;
    viennagrid_numeric * dst = &dst_coords[0] + destination_dimension*index;

    for (viennagrid_dimension d = 0; d != destination_dimension; ++d)
    {
      if (matrix)
        viennagrid_inner_prod(dimension, src, matrix+dimension*d, dst+d);
      else
        dst[+d] = 0;

      if (translation)
        dst[+d] += translation[+d];
    }
  }

  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_set(mesh, destination_dimension) );
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_pointer(mesh, &src_coords) );
  memcpy(src_coords, &dst_coords[0], sizeof(viennagrid_numeric) * dst_coords.size());

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_mesh_scale(viennagrid_mesh mesh,
                                       viennagrid_numeric factor,
                                       viennagrid_numeric const * scaling_center)
{
  if (!mesh)                        return VIENNAGRID_ERROR_INVALID_MESH;
  if (!mesh->is_root())             return VIENNAGRID_ERROR_MESH_IS_NOT_ROOT;

  viennagrid_dimension dimension;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dimension) );

  viennagrid_numeric * coords;
  RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_pointer(mesh, &coords) );

  viennagrid_int vertex_count;
  RETURN_ON_ERROR( viennagrid_mesh_element_count(mesh, 0, &vertex_count) );

  if (scaling_center)
  {
    std::vector<viennagrid_numeric> translate(dimension);
    for (viennagrid_dimension d = 0; d != dimension; ++d)
      translate[+d] = scaling_center[+d]*(1-factor);

    for (viennagrid_int index = 0; index != vertex_count; ++index)
    {
      for (viennagrid_dimension d = 0; d != dimension; ++d)
      {
        coords[dimension*index+d] = coords[dimension*index+d]*factor + translate[+d];
      }
    }
  }
  else
  {
    for (viennagrid_int index = 0; index != vertex_count*dimension; ++index)
      coords[index] *= factor;
  }

  return VIENNAGRID_SUCCESS;
}










viennagrid_error viennagrid_mesh_simplexify(viennagrid_mesh src_mesh,
                                            viennagrid_mesh dst_mesh)
{
  viennagrid_dimension dimension;
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(src_mesh, &dimension) );

  RETURN_ON_ERROR( viennagrid_mesh_clear(dst_mesh) );
  RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_set(dst_mesh, dimension) );

  viennagrid_dimension cell_dimension;
  RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(src_mesh, &cell_dimension) );


  viennagrid_copy_map copy_map;
  RETURN_ON_ERROR( viennagrid_copy_map_create(src_mesh, dst_mesh, &copy_map) );


  viennagrid_element_id * cells_begin;
  viennagrid_element_id * cells_end;
  RETURN_ON_ERROR( viennagrid_mesh_elements_get(src_mesh, cell_dimension, &cells_begin, &cells_end) );

  for (viennagrid_element_id * cit = cells_begin; cit != cells_end; ++cit)
  {
    viennagrid_element_type element_type;
    RETURN_ON_ERROR( viennagrid_element_type_get(src_mesh, *cit, &element_type) );

    if ((element_type != VIENNAGRID_ELEMENT_TYPE_TRIANGLE) && (element_type != VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL))
      return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;

    if (viennagrid_is_simplex(element_type) == VIENNAGRID_TRUE)
    {
      viennagrid_element_id new_element_id;
      RETURN_ON_ERROR( viennagrid_copy_map_element_copy(copy_map, *cit, &new_element_id) );
    }
    else
    {
      if (element_type == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL)
      {
        viennagrid_element_id * vertices_begin;
        viennagrid_element_id * vertices_end;
        RETURN_ON_ERROR( viennagrid_element_boundary_elements(src_mesh, *cit, 0, &vertices_begin, &vertices_end) );

        viennagrid_element_id new_vertices_ids[4];
        RETURN_ON_ERROR( viennagrid_copy_map_element_copy(copy_map, *vertices_begin, new_vertices_ids) );
        RETURN_ON_ERROR( viennagrid_copy_map_element_copy(copy_map, *(vertices_begin+1), new_vertices_ids+1) );
        RETURN_ON_ERROR( viennagrid_copy_map_element_copy(copy_map, *(vertices_begin+2), new_vertices_ids+2) );
        RETURN_ON_ERROR( viennagrid_copy_map_element_copy(copy_map, *(vertices_begin+3), new_vertices_ids+3) );

        viennagrid_element_id triangle_ids[2];
        RETURN_ON_ERROR( viennagrid_mesh_element_create(dst_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices_ids, triangle_ids) );
        RETURN_ON_ERROR( viennagrid_mesh_element_create(dst_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, new_vertices_ids+1, triangle_ids+1) );
      }
    }
  }

  return VIENNAGRID_SUCCESS;
}





