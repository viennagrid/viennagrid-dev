#include "viennagrid/viennagrid.h"
#include "../common.hpp"
#include <cmath>


//
// spanned volume
//

/** @brief Returns the signed volume of the line spanned by two points (begin, end) */
viennagrid_error viennagrid_signed_spanned_volume_2(viennagrid_dimension dimension,
                                                    viennagrid_numeric const * p1,
                                                    viennagrid_numeric const * p2,
                                                    viennagrid_numeric       * result)
{
  if (dimension == 1)
    *result = p2[0] - p1[0];
  else if (dimension == 2)
  {
    viennagrid_numeric dx = p2[0] - p1[0];
    viennagrid_numeric dy = p2[1] - p1[1];

    *result = std::sqrt(dx*dx + dy*dy);
  }
  else if (dimension == 3)
  {
    viennagrid_numeric dx = p2[0] - p1[0];
    viennagrid_numeric dy = p2[1] - p1[1];
    viennagrid_numeric dz = p2[2] - p1[2];

    *result = std::sqrt(dx*dx + dy*dy + dz*dz);
  }

  return VIENNAGRID_SUCCESS;
}

/** @brief Returns the area of the triangle spanned by the three points p1, p2, p3 */
viennagrid_error viennagrid_spanned_volume_2(viennagrid_dimension dimension,
                                             viennagrid_numeric const * p1,
                                             viennagrid_numeric const * p2,
                                             viennagrid_numeric       * result)
{
  if (result)
  {
    RETURN_ON_ERROR( viennagrid_signed_spanned_volume_2(dimension, p1, p2, result) );
  }
  return VIENNAGRID_SUCCESS;
}



viennagrid_error viennagrid_signed_spanned_volume_3(viennagrid_dimension dimension,
                                                    viennagrid_numeric const * p1,
                                                    viennagrid_numeric const * p2,
                                                    viennagrid_numeric const * p3,
                                                    viennagrid_numeric       * result)
{
  if (dimension == 2) //a triangle in the plane
  {
    if (result)
      *result = (  p1[0] * (p2[1] - p3[1])
                 + p2[0] * (p3[1] - p1[1])
                 + p3[0] * (p1[1] - p2[1]) ) / 2.0;
  }
  else if (dimension == 3) // triangle in 3d space
  {
    viennagrid_numeric v1[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
    viennagrid_numeric v2[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };

    // v3 = cross_prod(v1, v2);
    viennagrid_numeric v3[3];

    viennagrid_error err = viennagrid_cross_prod(v1, v2, v3);
    if (err != VIENNAGRID_SUCCESS)
      return err;

    if (result)
      *result = std::sqrt(v3[0] * v3[0] + v3[1] * v3[1] + v3[2] * v3[2]) / 2.0;
  }
  else
    return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  return VIENNAGRID_SUCCESS;
}

/** @brief Returns the area of the triangle spanned by the three points p1, p2, p3 */
viennagrid_error viennagrid_spanned_volume_3(viennagrid_dimension dimension,
                                             viennagrid_numeric const * p1,
                                             viennagrid_numeric const * p2,
                                             viennagrid_numeric const * p3,
                                             viennagrid_numeric       * result)
{
  if (result)
  {
    RETURN_ON_ERROR( viennagrid_signed_spanned_volume_3(dimension, p1, p2, p3, result) );
    *result = std::fabs(*result);
  }
  return VIENNAGRID_SUCCESS;
}


/** @brief Returns the area of the tetrahedron spanned by the three points p1, p2, p3 */
viennagrid_error viennagrid_signed_spanned_volume_4(viennagrid_dimension dimension,
                                                    viennagrid_numeric const * p1,
                                                    viennagrid_numeric const * p2,
                                                    viennagrid_numeric const * p3,
                                                    viennagrid_numeric const * p4,
                                                    viennagrid_numeric       * result)
{
  if (dimension == 3) // triangle in 3d space
  {
    viennagrid_numeric v1[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
    viennagrid_numeric v2[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
    viennagrid_numeric v3[3] = { p4[0] - p1[0], p4[1] - p1[1], p4[2] - p1[2] };

    // v3 = cross_prod(v1, v2);
    viennagrid_numeric v2v3[3];
    RETURN_ON_ERROR( viennagrid_cross_prod(v2, v3, v2v3) );

    if (result)
      *result = (v1[0] * v2v3[0] + v1[1] * v2v3[1] + v1[2] * v2v3[2]) / 6.0;
  }
  else
    return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  return VIENNAGRID_SUCCESS;
}


/** @brief Returns the area of the tetrahedron spanned by the three points p1, p2, p3 */
viennagrid_error viennagrid_spanned_volume_4(viennagrid_dimension dimension,
                                             viennagrid_numeric const * p1,
                                             viennagrid_numeric const * p2,
                                             viennagrid_numeric const * p3,
                                             viennagrid_numeric const * p4,
                                             viennagrid_numeric       * result)
{
  RETURN_ON_ERROR( viennagrid_signed_spanned_volume_4(dimension, p1, p2, p3, p4, result) );
  if (result)
    *result = std::fabs(*result);
  return VIENNAGRID_SUCCESS;
}




viennagrid_error viennagrid_signed_spanned_volume(viennagrid_dimension        dimension,
                                                  viennagrid_numeric const ** p_begin,
                                                  viennagrid_numeric const ** p_end,
                                                  viennagrid_numeric        * result)
{
  viennagrid_int point_count = p_end-p_begin;

  switch (point_count)
  {
    case 2:
      return viennagrid_signed_spanned_volume_2(dimension, *(p_begin+0), *(p_begin+1), result);
    case 3:
      return viennagrid_signed_spanned_volume_3(dimension, *(p_begin+0), *(p_begin+1), *(p_begin+2), result);
    case 4:
      return viennagrid_signed_spanned_volume_4(dimension, *(p_begin+0), *(p_begin+1), *(p_begin+2), *(p_begin+3), result);
  }

  return VIENNAGRID_ERROR_INVALID_ARGUMENTS;
}

viennagrid_error viennagrid_spanned_volume(viennagrid_dimension        dimension,
                                           viennagrid_numeric const ** p_begin,
                                           viennagrid_numeric const ** p_end,
                                           viennagrid_numeric        * result)
{
  viennagrid_int point_count = p_end-p_begin;

  switch (point_count)
  {
    case 2:
      return viennagrid_spanned_volume_2(dimension, *(p_begin+0), *(p_begin+1), result);
    case 3:
      return viennagrid_spanned_volume_3(dimension, *(p_begin+0), *(p_begin+1), *(p_begin+2), result);
    case 4:
      return viennagrid_spanned_volume_4(dimension, *(p_begin+0), *(p_begin+1), *(p_begin+2), *(p_begin+3), result);
  }

  return VIENNAGRID_ERROR_INVALID_ARGUMENTS;
}




//
// volume
//

/* computes the volume (area/length) of a particular element of the mesh */
viennagrid_error viennagrid_element_volume(viennagrid_mesh mesh,
                                           viennagrid_element_id element_id,
                                           viennagrid_numeric * volume)
{
  if (volume)
  {
    viennagrid_element_type element_type;
    RETURN_ON_ERROR( viennagrid_element_type_get(mesh, element_id, &element_type) );

    if (element_type == VIENNAGRID_ELEMENT_TYPE_VERTEX)
    {
      *volume = 1; // volume of 0-sphere
    }
    else
    {
      viennagrid_dimension dim;
      RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dim) );

      viennagrid_element_id *vertex_ids_begin, *vertex_ids_end;
      RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, element_id, 0, &vertex_ids_begin, &vertex_ids_end) );

      std::vector<viennagrid_numeric *> coords(vertex_ids_end - vertex_ids_begin);
      for (viennagrid_element_id *it = vertex_ids_begin; it<vertex_ids_end; ++it)
      {
        RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *it, &(coords[it - vertex_ids_begin])) );
      }


      if (element_type == VIENNAGRID_ELEMENT_TYPE_LINE || element_type == VIENNAGRID_ELEMENT_TYPE_EDGE)
      {
        RETURN_ON_ERROR( viennagrid_spanned_volume_2(dim, coords[0], coords[1], volume) );
      }
      else if (element_type == VIENNAGRID_ELEMENT_TYPE_TRIANGLE)
      {
        RETURN_ON_ERROR( viennagrid_spanned_volume_3(dim, coords[0], coords[1], coords[2], volume) );
      }
      else if (element_type == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL)
      {
        viennagrid_numeric volume_1;
        RETURN_ON_ERROR( viennagrid_spanned_volume_3(dim, coords[0], coords[1], coords[2], &volume_1) );
        RETURN_ON_ERROR( viennagrid_spanned_volume_3(dim, coords[2], coords[1], coords[3], volume) );
        *volume += volume_1;
      }
      else if (element_type == VIENNAGRID_ELEMENT_TYPE_POLYGON)
      {
        // decompose into triangles and sum signed contributions
        *volume = 0;
        viennagrid_numeric signed_volume_contribution;
        for (viennagrid_int i = 2; i < viennagrid_int(coords.size()); ++i)
        {
          RETURN_ON_ERROR( viennagrid_signed_spanned_volume_3(dim, coords[0], coords[i-1], coords[i], &signed_volume_contribution) );
          *volume += signed_volume_contribution;
        }
      }
      else if (element_type == VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON)
      {
        RETURN_ON_ERROR( viennagrid_spanned_volume_4(dim, coords[0], coords[1], coords[2], coords[3], volume) );
      }
      else if (element_type == VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON)
      {
        // decompose into tetrahedra and sum signed contributions
        *volume = 0;
        viennagrid_numeric signed_volume_contribution;
        RETURN_ON_ERROR( viennagrid_spanned_volume_4(dim, coords[0], coords[1], coords[3], coords[4], &signed_volume_contribution) );
        *volume += signed_volume_contribution;
        RETURN_ON_ERROR( viennagrid_spanned_volume_4(dim, coords[4], coords[1], coords[3], coords[7], &signed_volume_contribution) );
        *volume += signed_volume_contribution;
        RETURN_ON_ERROR( viennagrid_spanned_volume_4(dim, coords[4], coords[1], coords[7], coords[5], &signed_volume_contribution) );
        *volume += signed_volume_contribution;

        RETURN_ON_ERROR( viennagrid_spanned_volume_4(dim, coords[0], coords[3], coords[2], coords[4], &signed_volume_contribution) );
        *volume += signed_volume_contribution;
        RETURN_ON_ERROR( viennagrid_spanned_volume_4(dim, coords[2], coords[4], coords[3], coords[6], &signed_volume_contribution) );
        *volume += signed_volume_contribution;
        RETURN_ON_ERROR( viennagrid_spanned_volume_4(dim, coords[6], coords[4], coords[3], coords[7], &signed_volume_contribution) );
        *volume += signed_volume_contribution;
      }
      else
        return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;
    }
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the volume (area/length) of the mesh (given by the sum of all cells inside the mesh) */
viennagrid_error viennagrid_mesh_volume(viennagrid_mesh mesh,
                                        viennagrid_numeric * volume)
{
  if (volume)
  {
    viennagrid_dimension cell_dim;
    RETURN_ON_ERROR(  viennagrid_mesh_cell_dimension_get(mesh, &cell_dim) );
    if (cell_dim < 1)
      return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

    *volume = 0;
    viennagrid_numeric volume_contribution;
    viennagrid_element_id *cell_ids_begin, *cell_ids_end;
    RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, cell_dim, &cell_ids_begin, &cell_ids_end) );
    for (viennagrid_element_id *cit = cell_ids_begin; cit != cell_ids_end; ++cit)
    {
      RETURN_ON_ERROR( viennagrid_element_volume(mesh, *cit, &volume_contribution) );
      *volume += volume_contribution;
    }
  }

  return VIENNAGRID_SUCCESS;
}



/* computes the volume (area/length) of the region (given by the sum of all cells inside the mesh) */
viennagrid_error viennagrid_region_volume(viennagrid_mesh mesh,
                                          viennagrid_region region,
                                          viennagrid_numeric * volume)
{
  if (volume)
  {
    viennagrid_dimension cell_dim;
    RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(mesh, &cell_dim) );
    if (cell_dim < 1)
      return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

    *volume = 0;
    viennagrid_element_id *cell_ids_begin, *cell_ids_end;
    RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, cell_dim, &cell_ids_begin, &cell_ids_end) );
    viennagrid_numeric volume_contribution;
    viennagrid_bool is_in_region;
    for (viennagrid_element_id *cit = cell_ids_begin; cit != cell_ids_end; ++cit)
    {
      RETURN_ON_ERROR( viennagrid_region_contains_element(region, *cit, &is_in_region) );
      if (is_in_region == VIENNAGRID_TRUE)
      {
        RETURN_ON_ERROR( viennagrid_element_volume(mesh, *cit, &volume_contribution) );
        *volume += volume_contribution;
      }
    }
  }

  return VIENNAGRID_SUCCESS;
}






