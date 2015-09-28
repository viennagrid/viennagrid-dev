#include "viennagrid/viennagrid.h"
#include "../common.hpp"

/* computes the surface of a particular element of the mesh */
viennagrid_error viennagrid_element_surface(viennagrid_mesh mesh,
                                            viennagrid_element_id element_id,
                                            viennagrid_numeric * surface)
{
  if (surface)
  {
    if (TOPODIM(element_id) < 1) // surface of vertex is undefined
      return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

    // Sum volumes of facets
    viennagrid_element_id *facet_ids_begin, *facet_ids_end;
    RETURN_ON_ERROR(  viennagrid_element_boundary_elements(mesh, element_id, TOPODIM(element_id) - 1, &facet_ids_begin, &facet_ids_end) );

    *surface = 0;
    viennagrid_numeric surface_contribution;
    for (viennagrid_element_id * it = facet_ids_begin; it != facet_ids_end; ++it)
    {
      RETURN_ON_ERROR( viennagrid_element_volume(mesh, *it, &surface_contribution) );
      *surface += surface_contribution;
    }
  }

  return VIENNAGRID_SUCCESS;
}

/* computes the surface of the mesh (given by the sum of all facets with only one connected cell) */
viennagrid_error viennagrid_mesh_surface(viennagrid_mesh mesh,
                                         viennagrid_numeric * surface)
{
  if (surface)
  {
    viennagrid_dimension cell_dim;
    RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(mesh, &cell_dim) );
    if (cell_dim < 1)
      return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

    //
    // Sum contributions from all facets on boundary
    //
    *surface = 0;
    viennagrid_element_id *facet_ids_begin, *facet_ids_end;
    RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, cell_dim-1, &facet_ids_begin, &facet_ids_end) );
    viennagrid_numeric surface_contribution;
    viennagrid_bool is_boundary;
    for (viennagrid_element_id *fit = facet_ids_begin; fit != facet_ids_end; ++fit)
    {
      RETURN_ON_ERROR( viennagrid_element_is_mesh_boundary(mesh, *fit, &is_boundary) );
      if (is_boundary == VIENNAGRID_TRUE)
      {
        RETURN_ON_ERROR( viennagrid_element_volume(mesh, *fit, &surface_contribution) );
        *surface += surface_contribution;
      }
    }
  }
  return VIENNAGRID_SUCCESS;
}


/* computes the surface of the region (given by the sum of all facets with only one connected cell) */
viennagrid_error viennagrid_region_surface(viennagrid_mesh mesh,
                                           viennagrid_region region,
                                           viennagrid_numeric * surface)
{
  if (surface)
  {
    viennagrid_dimension cell_dim;
    RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(mesh, &cell_dim) );
    if (cell_dim < 1)
      return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

    //
    // Sum contributions from all facets on boundary
    //
    *surface = 0;
    viennagrid_element_id *facet_ids_begin, *facet_ids_end;
    RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, cell_dim-1, &facet_ids_begin, &facet_ids_end) );
    viennagrid_numeric surface_contribution;
    viennagrid_bool is_boundary;
    viennagrid_bool is_in_region;
    for (viennagrid_element_id *fit = facet_ids_begin; fit != facet_ids_end; ++fit)
    {
      RETURN_ON_ERROR( viennagrid_region_contains_element(region, *fit, &is_in_region) );
      RETURN_ON_ERROR( viennagrid_element_is_mesh_boundary(mesh, *fit, &is_boundary) );
      if ((is_boundary == VIENNAGRID_TRUE) && (is_in_region == VIENNAGRID_TRUE))
      {
        RETURN_ON_ERROR( viennagrid_element_volume(mesh, *fit, &surface_contribution) );
        *surface += surface_contribution;
      }
    }
  }
  return VIENNAGRID_SUCCESS;
}


