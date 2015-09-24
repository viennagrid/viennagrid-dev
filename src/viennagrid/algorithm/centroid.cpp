#include "viennagrid/viennagrid.h"
#include "../common.hpp"

/* computes the centroid of a particular element of the mesh */
viennagrid_error viennagrid_element_centroid(viennagrid_mesh mesh,
                                             viennagrid_element_id element_id,
                                             viennagrid_numeric * coords)
{
  if (coords)
  {
    viennagrid_element_type element_type;
    RETURN_ON_ERROR( viennagrid_element_type_get(mesh, element_id, &element_type) );

    viennagrid_dimension dim;
    RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dim) );

    if (element_type == VIENNAGRID_ELEMENT_TYPE_VERTEX)
    {
      viennagrid_numeric * point_coords;
      RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, element_id, &point_coords) );

      for (viennagrid_dimension d = 0; d<dim; ++d)
        coords[+d] = point_coords[+d];
    }
    else if (   (element_type == VIENNAGRID_ELEMENT_TYPE_LINE)        || (element_type == VIENNAGRID_ELEMENT_TYPE_EDGE)
             || (element_type == VIENNAGRID_ELEMENT_TYPE_TRIANGLE)    || (element_type == VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL)
             || (element_type == VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON) || (element_type == VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON) )
    {
      viennagrid_element_id *vertex_ids_begin, *vertex_ids_end;
      RETURN_ON_ERROR( viennagrid_element_boundary_elements(mesh, element_id, 0, &vertex_ids_begin, &vertex_ids_end) );

      std::vector<viennagrid_numeric *> point_coords(vertex_ids_end - vertex_ids_begin);
      for (viennagrid_element_id *it = vertex_ids_begin; it<vertex_ids_end; ++it)
      {
        RETURN_ON_ERROR( viennagrid_mesh_vertex_coords_get(mesh, *it, &(point_coords[it - vertex_ids_begin])) );
      }

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



viennagrid_error viennagrid_mesh_centroid(viennagrid_mesh mesh,
                                          viennagrid_numeric * coords)
{
  if (coords)
  {
    viennagrid_dimension dim;
    RETURN_ON_ERROR( viennagrid_mesh_geometric_dimension_get(mesh, &dim) );
    if (dim <= 0)
      return VIENNAGRID_ERROR_INVALID_TOPOLOGICAL_DIMENSION;

    viennagrid_dimension cell_dimension;
    RETURN_ON_ERROR( viennagrid_mesh_cell_dimension_get(mesh, &cell_dimension) );

    viennagrid_element_id * cells_begin;
    viennagrid_element_id * cells_end;
    RETURN_ON_ERROR( viennagrid_mesh_elements_get(mesh, cell_dimension, &cells_begin, &cells_end) );

    for (viennagrid_dimension d = 0; d<dim; ++d)
      coords[+d] = 0;
    viennagrid_numeric volume = 0;

    for (viennagrid_element_id * cit = cells_begin; cit != cells_end; ++cit)
    {
      std::vector<viennagrid_numeric> current_centroid(dim, 0);
      viennagrid_numeric current_volume;

      RETURN_ON_ERROR( viennagrid_element_centroid(mesh, *cit, &current_centroid[0]) );
      RETURN_ON_ERROR( viennagrid_element_volume(mesh, *cit, &current_volume) );

      volume += current_volume;
      for (viennagrid_dimension d = 0; d<dim; ++d)
        coords[+d] += current_centroid[+d];
    }

    for (viennagrid_dimension d = 0; d<dim; ++d)
      coords[+d] /= volume;
  }

  return VIENNAGRID_SUCCESS;
}
