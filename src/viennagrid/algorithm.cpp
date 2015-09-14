#ifndef VIENNAGRID_ALGORITHM_SPANNED_VOLUME_HPP
#define VIENNAGRID_ALGORITHM_SPANNED_VOLUME_HPP

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

#include "viennagrid/viennagrid.h"
#include "common.hpp"

/** @file viennagrid/algorithm/spanned_volume.hpp
    @brief Computes the volume of n-simplices spanned by points
*/


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
viennagrid_error viennagrid_inner_prod(viennagrid_int dimension,
                                       viennagrid_numeric const * v1,
                                       viennagrid_numeric const * v2,
                                       viennagrid_numeric       * result)
{
  if (result)
  {
    *result = 0;
    for (viennagrid_int i = 0; i < dimension; ++i)
      *result += v1[i] * v2[i];
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the l2-norm of a vector: result = ||v1||_2 */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_norm(viennagrid_int dimension,
                                                           viennagrid_numeric const * v1,
                                                           viennagrid_numeric       * result)
{
  if (result)
  {
    *result = 0;
    for (viennagrid_int i = 0; i < dimension; ++i)
      *result += v1[i] * v1[i];
    *result = std::sqrt(*result);
  }

  return VIENNAGRID_SUCCESS;
}



//
// spanned volume
//

/** @brief Returns the signed volume of the line spanned by two points (begin, end) */
viennagrid_error viennagrid_spanned_volume_2(viennagrid_int dimension,
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


static viennagrid_error signed_spanned_volume_3(viennagrid_int dimension,
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
viennagrid_error viennagrid_spanned_volume_3(viennagrid_int dimension,
                                             viennagrid_numeric const * p1,
                                             viennagrid_numeric const * p2,
                                             viennagrid_numeric const * p3,
                                             viennagrid_numeric       * result)
{
  if (result)
  {
    viennagrid_error err = signed_spanned_volume_3(dimension, p1, p2, p3, result);
    *result = std::fabs(*result);
    return err;
  }
  return VIENNAGRID_SUCCESS;
}


/** @brief Returns the area of the tetrahedron spanned by the three points p1, p2, p3 */
static viennagrid_error signed_spanned_volume_4(viennagrid_int dimension,
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
    viennagrid_error err = viennagrid_cross_prod(v2, v3, v2v3);
    if (err != VIENNAGRID_SUCCESS)
      return err;

    if (result)
      *result = (v1[0] * v2v3[0] + v1[1] * v2v3[1] + v1[2] * v2v3[2]) / 6.0;
  }
  else
    return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  return VIENNAGRID_SUCCESS;
}


/** @brief Returns the area of the tetrahedron spanned by the three points p1, p2, p3 */
viennagrid_error viennagrid_spanned_volume_4(viennagrid_int dimension,
                                             viennagrid_numeric const * p1,
                                             viennagrid_numeric const * p2,
                                             viennagrid_numeric const * p3,
                                             viennagrid_numeric const * p4,
                                             viennagrid_numeric       * result)
{
  viennagrid_error err = signed_spanned_volume_4(dimension, p1, p2, p3, p4, result);
  if (result)
    *result = std::fabs(*result);
  return err;
}


//
// volume
//

/* computes the volume (area/length) of a particular element of the mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_volume(viennagrid_mesh mesh,
                                                                     viennagrid_element_id element_id,
                                                                     viennagrid_numeric * volume)
{
  if (volume)
  {
    viennagrid_element_type element_type;
    viennagrid_int err = viennagrid_element_type_get(mesh, element_id, &element_type);
    if (err != VIENNAGRID_SUCCESS)
      return err;

    viennagrid_dimension dim;
    err = viennagrid_mesh_geometric_dimension_get(mesh, &dim);
    if (err != VIENNAGRID_SUCCESS)
      return err;

    viennagrid_int *vertex_ids_begin, *vertex_ids_end;
    err = viennagrid_element_boundary_elements(mesh, element_id, 0, &vertex_ids_begin, &vertex_ids_end);
    if (err != VIENNAGRID_SUCCESS)
      return err;

    std::vector<viennagrid_numeric *> coords(vertex_ids_end - vertex_ids_begin);
    for (viennagrid_int *it = vertex_ids_begin; it<vertex_ids_end; ++it)
    {
      err = viennagrid_mesh_vertex_coords_get(mesh, *it, &(coords[it - vertex_ids_begin]));
      if (err != VIENNAGRID_SUCCESS)
        return err;
    }

    if (element_type == VIENNAGRID_ELEMENT_TYPE_VERTEX)
    {
      *volume = 1; // volume of 0-sphere
    }
    else if (element_type == VIENNAGRID_ELEMENT_TYPE_LINE || element_type == VIENNAGRID_ELEMENT_TYPE_EDGE)
    {
      err = viennagrid_spanned_volume_2(dim, coords[0], coords[1], volume); if (err != VIENNAGRID_SUCCESS) return err;
    }
    else if (element_type == VIENNAGRID_ELEMENT_TYPE_TRIANGLE)
    {
      err = viennagrid_spanned_volume_3(dim, coords[0], coords[1], coords[2], volume); if (err != VIENNAGRID_SUCCESS) return err;
    }
    else if (element_type == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL)
    {
      viennagrid_numeric volume_1;
      err = viennagrid_spanned_volume_3(dim, coords[0], coords[1], coords[2], &volume_1); if (err != VIENNAGRID_SUCCESS) return err;
      err = viennagrid_spanned_volume_3(dim, coords[2], coords[1], coords[3], volume);    if (err != VIENNAGRID_SUCCESS) return err;
      *volume += volume_1;
    }
    else if (element_type == VIENNAGRID_ELEMENT_TYPE_POLYGON)
    {
      // decompose into triangles and sum signed contributions
      *volume = 0;
      viennagrid_numeric signed_volume_contribution;
      for (viennagrid_int i = 2; i < viennagrid_int(coords.size()); ++i)
      {
        err = signed_spanned_volume_3(dim, coords[0], coords[i-1], coords[i], &signed_volume_contribution); if (err != VIENNAGRID_SUCCESS) return err;
        *volume += signed_volume_contribution;
      }
    }
    else if (element_type == VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON)
    {
      err = viennagrid_spanned_volume_4(dim, coords[0], coords[1], coords[2], coords[3], volume); if (err != VIENNAGRID_SUCCESS) return err;
    }
    else if (element_type == VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON)
    {
      // decompose into tetrahedra and sum signed contributions
      *volume = 0;
      viennagrid_numeric signed_volume_contribution;
      err = viennagrid_spanned_volume_4(dim, coords[0], coords[1], coords[3], coords[4], &signed_volume_contribution); if (err != VIENNAGRID_SUCCESS) return err;
      *volume += signed_volume_contribution;
      err = viennagrid_spanned_volume_4(dim, coords[4], coords[1], coords[3], coords[7], &signed_volume_contribution); if (err != VIENNAGRID_SUCCESS) return err;
      *volume += signed_volume_contribution;
      err = viennagrid_spanned_volume_4(dim, coords[4], coords[1], coords[7], coords[5], &signed_volume_contribution); if (err != VIENNAGRID_SUCCESS) return err;
      *volume += signed_volume_contribution;

      err = viennagrid_spanned_volume_4(dim, coords[0], coords[3], coords[2], coords[4], &signed_volume_contribution); if (err != VIENNAGRID_SUCCESS) return err;
      *volume += signed_volume_contribution;
      err = viennagrid_spanned_volume_4(dim, coords[2], coords[4], coords[3], coords[6], &signed_volume_contribution); if (err != VIENNAGRID_SUCCESS) return err;
      *volume += signed_volume_contribution;
      err = viennagrid_spanned_volume_4(dim, coords[6], coords[4], coords[3], coords[7], &signed_volume_contribution); if (err != VIENNAGRID_SUCCESS) return err;
      *volume += signed_volume_contribution;
    }
    else
      return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the surface of a particular element of the mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_surface(viennagrid_mesh mesh,
                                                                      viennagrid_element_id element_id,
                                                                      viennagrid_numeric * surface)
{
  if (surface)
  {
    if (TOPODIM(element_id) < 1) // surface of vertex is undefined
      return VIENNAGRID_INVALID_TOPOLOGIC_DIMENSION;

    // Sum volumes of facets
    viennagrid_int *facet_ids_begin, *facet_ids_end;
    viennagrid_error err = viennagrid_element_boundary_elements(mesh, element_id, TOPODIM(element_id) - 1, &facet_ids_begin, &facet_ids_end); if (err != VIENNAGRID_SUCCESS) return err;

    *surface = 0;
    viennagrid_numeric surface_contribution;
    for (viennagrid_int * it = facet_ids_begin; it != facet_ids_end; ++it)
    {
      err = viennagrid_element_volume(mesh, *it, &surface_contribution); if (err != VIENNAGRID_SUCCESS) return err;
      *surface += surface_contribution;
    }
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the volume (area/length) of the mesh (given by the sum of all cells inside the mesh) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_volume(viennagrid_mesh mesh,
                                                                  viennagrid_numeric * volume)
{
  if (volume)
  {
    viennagrid_dimension cell_dim;
    viennagrid_error err = viennagrid_mesh_cell_dimension_get(mesh, &cell_dim); if (err != VIENNAGRID_SUCCESS) return err;
    if (cell_dim < 1)
      return VIENNAGRID_INVALID_TOPOLOGIC_DIMENSION;

    *volume = 0;
    viennagrid_numeric volume_contribution;
    viennagrid_int *cell_ids_begin, *cell_ids_end;
    err = viennagrid_mesh_elements_get(mesh, cell_dim, &cell_ids_begin, &cell_ids_end); if (err != VIENNAGRID_SUCCESS) return err;
    for (viennagrid_int *cit = cell_ids_begin; cit != cell_ids_end; ++cit)
    {
      err = viennagrid_element_volume(mesh, *cit, &volume_contribution); if (err != VIENNAGRID_SUCCESS) return err;
      *volume += volume_contribution;
    }
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the surface of the mesh (given by the sum of all facets with only one connected cell) */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_mesh_surface(viennagrid_mesh mesh,
                                                                   viennagrid_numeric * surface)
{
  if (surface)
  {
    viennagrid_dimension cell_dim;
    viennagrid_error err = viennagrid_mesh_cell_dimension_get(mesh, &cell_dim); if (err != VIENNAGRID_SUCCESS) return err;
    if (cell_dim < 1)
      return VIENNAGRID_INVALID_TOPOLOGIC_DIMENSION;

    //
    // Step 1: Find all facets on the boundary (do not assume that co-boundary information has been set up)
    //
    viennagrid_int *facet_ids_begin, *facet_ids_end;
    err = viennagrid_mesh_elements_get(mesh, cell_dim - 1, &facet_ids_begin, &facet_ids_end); if (err != VIENNAGRID_SUCCESS) return err;

    std::vector<bool> facet_on_boundary(facet_ids_end - facet_ids_begin, false);

    viennagrid_int *cell_ids_begin, *cell_ids_end;
    err = viennagrid_mesh_elements_get(mesh, cell_dim, &cell_ids_begin, &cell_ids_end); if (err != VIENNAGRID_SUCCESS) return err;
    for (viennagrid_int *cit = cell_ids_begin; cit != cell_ids_end; ++cit)
    {
      viennagrid_int *facets_on_cell_begin, *facets_on_cell_end;
      err = viennagrid_element_boundary_elements(mesh, *cit, cell_dim - 1, &facets_on_cell_begin, &facets_on_cell_end); if (err != VIENNAGRID_SUCCESS) return err;

      for (viennagrid_int *fit = facets_on_cell_begin; fit != facets_on_cell_end; ++fit)
        facet_on_boundary[*fit] = !facet_on_boundary[*fit]; // facets belonging to only one cell will flip: false -> true. Others will double-flip: false -> true -> false
    }

    //
    // Step 2: Sum contributions from all facets on boundary
    //
    *surface = 0;
    viennagrid_numeric surface_contribution;
    for (viennagrid_int *fit = facet_ids_begin; fit != facet_ids_end; ++fit)
    {
      if (facet_on_boundary[*fit])
      {
        err = viennagrid_element_volume(mesh, *fit, &surface_contribution); if (err != VIENNAGRID_SUCCESS) return err;
        *surface += surface_contribution;
      }
    }
  }
  return VIENNAGRID_SUCCESS;
}


/* computes the centroid of a particular element of the mesh */
VIENNAGRID_DYNAMIC_EXPORT viennagrid_error viennagrid_element_centroid(viennagrid_mesh mesh,
                                                                       viennagrid_element_id element_id,
                                                                       viennagrid_numeric * coords)
{
  if (coords)
  {
    viennagrid_element_type element_type;
    viennagrid_int err = viennagrid_element_type_get(mesh, element_id, &element_type);
    if (err != VIENNAGRID_SUCCESS)
      return err;

    viennagrid_dimension dim;
    err = viennagrid_mesh_geometric_dimension_get(mesh, &dim);
    if (err != VIENNAGRID_SUCCESS)
      return err;

    viennagrid_int *vertex_ids_begin, *vertex_ids_end;
    err = viennagrid_mesh_elements_get(mesh, 0, &vertex_ids_begin, &vertex_ids_end);
    if (err != VIENNAGRID_SUCCESS)
      return err;

    std::vector<viennagrid_numeric *> point_coords(vertex_ids_end - vertex_ids_begin);
    for (viennagrid_int *it = vertex_ids_begin; it<vertex_ids_end; ++it)
    {
      err = viennagrid_mesh_vertex_coords_get(mesh, *it, &(point_coords[it - vertex_ids_begin]));
      if (err != VIENNAGRID_SUCCESS)
        return err;
    }

    if (element_type == VIENNAGRID_ELEMENT_TYPE_VERTEX)
    {
      for (viennagrid_dimension d = 0; d<dim; ++d)
        coords[+d] = point_coords[0][+d];
    }
    else if (   (element_type == VIENNAGRID_ELEMENT_TYPE_LINE)        || (element_type == VIENNAGRID_ELEMENT_TYPE_EDGE)
             || (element_type == VIENNAGRID_ELEMENT_TYPE_TRIANGLE)    || (element_type == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL)
             || (element_type == VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON) || (element_type == VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON) )
    {
      for (viennagrid_dimension d = 0; d<dim; ++d)
        coords[+d] = 0;

      // centroid is arithmetic mean of point coordinates
      for (std::size_t i=0; i<point_coords.size(); ++i)
        for (viennagrid_dimension d = 0; d<dim; ++d)
          coords[+d] += point_coords[i][+d];

      for (viennagrid_dimension d = 0; d<dim; ++d)
        coords[+d] /= viennagrid_numeric(point_coords.size());
    }
    //else if (element_type == VIENNAGRID_ELEMENT_TYPE_POLYGON)
    //{
    //  TODO!
    //}
    else
      return VIENNAGRID_ERROR_INVALID_ELEMENT_TYPE;
  }

  return VIENNAGRID_SUCCESS;
}

#endif
