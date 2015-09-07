/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <stdlib.h>
#include <stdio.h>

#include "viennagrid/viennagrid.h"

/*
//    Let us construct the following input mesh using the C API (have a look at mesh_setup.cpp for the C++ API):
//
//    5---------4---------3
//    | \       | \       |
//    |   \     |   \     |    y
//    |     \   |     \   |     ^
//    |       \ |       \ |     |
//    0---------1---------2     *--> x
//
//     Region 1 | Region 2
*/

#define ERROR_CHECK(err) do { if (err != VIENNAGRID_SUCCESS) { printf("Return value invalid in line %ld \n", (long)__LINE__); return (int)err; } } while (0);

int print_cells_in_region(viennagrid_mesh_hierarchy my_mesh_hierarchy, viennagrid_mesh my_mesh, viennagrid_region_id region_id)
{
  viennagrid_error     err;
  viennagrid_int       *element_ptr_begin, *element_ptr_end;
  viennagrid_region_id *regions_begin, *regions_end;
  viennagrid_int       *vertices_begin, *vertices_end;
  viennagrid_int       *it;
  viennagrid_region_id *it2;
  viennagrid_int       *vertex_it;

  err = viennagrid_mesh_elements_get(my_mesh, 2, &element_ptr_begin, &element_ptr_end); ERROR_CHECK(err);

  for (it = element_ptr_begin; it != element_ptr_end; ++it)
  {
    err = viennagrid_element_regions_get(my_mesh_hierarchy, 2, *it, &regions_begin, &regions_end); ERROR_CHECK(err);
    for (it2 = regions_begin; it2 != regions_end; ++it2)
      if (*it2 == region_id)
      {
        printf("--- Element %ld ---\n", (long)*it);
        /* print vertices: */
        viennagrid_element_boundary_elements(my_mesh_hierarchy, 2, *it, 0, &vertices_begin, &vertices_end); ERROR_CHECK(err);
        for (vertex_it = vertices_begin; vertex_it != vertices_end; ++vertex_it)
        {
          viennagrid_numeric *vertex_coords;
          err = viennagrid_mesh_hierarchy_vertex_coords_get(my_mesh_hierarchy, *vertex_it, &vertex_coords); ERROR_CHECK(err);
          printf(" %ld: %g %g \n", (long)*vertex_it, vertex_coords[0], vertex_coords[1]);
        }
      }
  }

  return VIENNAGRID_SUCCESS;
}

int main()
{
  viennagrid_error          err;
  viennagrid_mesh_hierarchy my_mesh_hierarchy;
  viennagrid_mesh           my_mesh;
  viennagrid_region         my_region0, my_region1;
  viennagrid_region_id      my_region_id0, my_region_id1;
  viennagrid_int            *element_ptr_begin, *element_ptr_end;

  viennagrid_numeric coords0[2] = {0, 0}; viennagrid_int vertex0;
  viennagrid_numeric coords1[2] = {1, 0}; viennagrid_int vertex1;
  viennagrid_numeric coords2[2] = {2, 0}; viennagrid_int vertex2;

  viennagrid_numeric coords3[2] = {2, 1}; viennagrid_int vertex3;
  viennagrid_numeric coords4[2] = {1, 1}; viennagrid_int vertex4;
  viennagrid_numeric coords5[2] = {0, 1}; viennagrid_int vertex5;

  viennagrid_int element0_vertices[3]; viennagrid_int element0;
  viennagrid_int element1_vertices[3]; viennagrid_int element1;
  viennagrid_int element2_vertices[3]; viennagrid_int element2;
  viennagrid_int element3_vertices[3]; viennagrid_int element3;

  /*
   * Define the necessary types:
   */

  printf("------------------------------------------------------------\n");
  printf("-- ViennaGrid tutorial: Setup of a mesh with two segments --\n");
  printf("------------------------------------------------------------\n");

  /*
   * Step 1: Instantiate the mesh hierarchy, extract the root mesh, and create 2 regions:
   */
  err = viennagrid_mesh_hierarchy_create(&my_mesh_hierarchy); ERROR_CHECK(err);
  err = viennagrid_mesh_hierarchy_geometric_dimension_set(my_mesh_hierarchy, 2); ERROR_CHECK(err);

  err = viennagrid_mesh_hierarchy_root_mesh_get(my_mesh_hierarchy, &my_mesh); ERROR_CHECK(err);

  err = viennagrid_mesh_hierarchy_region_create(my_mesh_hierarchy, &my_region0); ERROR_CHECK(err);
  err = viennagrid_mesh_hierarchy_region_create(my_mesh_hierarchy, &my_region1); ERROR_CHECK(err);

  err = viennagrid_region_id_get(my_region0, &my_region_id0); ERROR_CHECK(err);
  err = viennagrid_region_id_get(my_region1, &my_region_id1); ERROR_CHECK(err);

  /*
   * Step 2: Add vertices to the mesh.
   *         Note that vertices with IDs are enumerated in the order they are pushed to the mesh.
   */
  err = viennagrid_mesh_vertex_create(my_mesh, coords0, &vertex0); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords1, &vertex1); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords2, &vertex2); ERROR_CHECK(err);

  err = viennagrid_mesh_vertex_create(my_mesh, coords3, &vertex3); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords4, &vertex4); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords5, &vertex5); ERROR_CHECK(err);

  /*
   * Step 3: Fill the two regions with cells.
   *         To do so, each cell must be linked with the defining vertex handles from the mesh
   */

  element0_vertices[0] = vertex0; element0_vertices[1] = vertex1; element0_vertices[2] = vertex5;
  element1_vertices[0] = vertex1; element1_vertices[1] = vertex4; element1_vertices[2] = vertex5;
  element2_vertices[0] = vertex1; element2_vertices[1] = vertex2; element2_vertices[2] = vertex4;
  element3_vertices[0] = vertex2; element3_vertices[1] = vertex3; element3_vertices[2] = vertex4;

  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, element0_vertices, &element0); ERROR_CHECK(err);
  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, element1_vertices, &element1); ERROR_CHECK(err);
  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, element2_vertices, &element2); ERROR_CHECK(err);
  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, element3_vertices, &element3); ERROR_CHECK(err);

  err = viennagrid_region_element_add(my_region0, 2, element0); ERROR_CHECK(err);
  err = viennagrid_region_element_add(my_region0, 2, element1); ERROR_CHECK(err);

  err = viennagrid_region_element_add(my_region1, 2, element2); ERROR_CHECK(err);
  err = viennagrid_region_element_add(my_region1, 2, element3); ERROR_CHECK(err);

  err = viennagrid_mesh_elements_get(my_mesh, 2, &element_ptr_begin, &element_ptr_end); ERROR_CHECK(err);

  printf("Number of triangles in mesh: %ld \n", (long)(element_ptr_end - element_ptr_begin));

  /*
   * That's it. The mesh consisting of two segments is now set up.
   * If no segments are required, one can also directly write viennagrid::make_triangle(mesh, ...);
   */

  /*
   * Step 4: Output the cells for each segment:
   */

  printf("\nCells in region 0:\n");
  print_cells_in_region(my_mesh_hierarchy, my_mesh, my_region_id0);

  printf("\nCells in region 1:\n");
  print_cells_in_region(my_mesh_hierarchy, my_mesh, my_region_id1);

  err = viennagrid_mesh_hierarchy_release(my_mesh_hierarchy); ERROR_CHECK(err);


  printf("\n");
  printf("-------------------------------------------------\n");
  printf(" \\o/    Tutorial finished successfully!    \\o/ \n");
  printf("-------------------------------------------------\n");

  return EXIT_SUCCESS;
}
