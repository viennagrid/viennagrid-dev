/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <cstdlib>
#include <cmath>
#include <iostream>

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

#define ERROR_CHECK(err) do { if (err != VIENNAGRID_SUCCESS) { std::cout << "Return value invalid in line " << (long)__LINE__ << std::endl; return (int)err; } } while (0);

//
// |-----|-----------------|
// 0    3.14               42.0
//
int test_1d()
{
  viennagrid_error err;

  std::cout << "1D, line mesh ..."; std::cout.flush();
  viennagrid_mesh  my_mesh;
  err = viennagrid_mesh_create(&my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_mesh, 1); ERROR_CHECK(err);

  viennagrid_numeric coords0 = 0.0;    viennagrid_int vertex0;
  viennagrid_numeric coords1 = 3.1415; viennagrid_int vertex1;
  viennagrid_numeric coords2 = 42.0;   viennagrid_int vertex2;

  err = viennagrid_mesh_vertex_create(my_mesh, &coords0, &vertex0); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, &coords1, &vertex1); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, &coords2, &vertex2); ERROR_CHECK(err);

  viennagrid_int element0_vertices[2] = {vertex0, vertex1}; viennagrid_int element0;
  viennagrid_int element1_vertices[2] = {vertex1, vertex2}; viennagrid_int element1;

  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_LINE, 2, element0_vertices, &element0); ERROR_CHECK(err);
  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_LINE, 2, element1_vertices, &element1); ERROR_CHECK(err);

  viennagrid_numeric volume0;
  err = viennagrid_element_volume(my_mesh, 1, element0, &volume0); ERROR_CHECK(err);
  if (std::fabs(volume0 - coords1) > 1e-4)
  {
    std::cerr << "ERROR: 1D volume check failed for volume0: " << volume0 << " vs. " << coords1 << std::endl;
    return EXIT_FAILURE;
  }

  viennagrid_numeric volume1;
  err = viennagrid_element_volume(my_mesh, 1, element1, &volume1); ERROR_CHECK(err);
  if (std::fabs(volume1 - (coords2 - coords1)) > 1e-4)
  {
    std::cerr << "ERROR: 1D volume check failed for volume1: " << volume1 << " vs. " << (coords2 - coords1) << std::endl;
    return EXIT_FAILURE;
  }

  // Mesh volume:
  viennagrid_numeric volume_mesh;
  err = viennagrid_mesh_volume(my_mesh, &volume_mesh); ERROR_CHECK(err);
  if (std::fabs(volume_mesh - coords2) > 1e-4)
  {
    std::cerr << "ERROR: mesh volume check failed: " << volume_mesh << " vs. " << coords2 << std::endl;
    return EXIT_FAILURE;
  }

  err = viennagrid_mesh_release(my_mesh); ERROR_CHECK(err);

  std::cout << " PASSED" << std::endl;
  return EXIT_SUCCESS;
}

//
// 2.0 * --_
//     |     --_
//     |    _  * 1.0
//     | _/    |
//     *-------*
//   0.0      1.0
//
int test_2d_triangle()
{
  viennagrid_error err;

  std::cout << "2D, triangular mesh ..."; std::cout.flush();
  viennagrid_mesh  my_mesh;
  err = viennagrid_mesh_create(&my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_mesh, 2); ERROR_CHECK(err);

  viennagrid_numeric coords0[2] = {0.0, 0.0}; viennagrid_int vertex0;
  viennagrid_numeric coords1[2] = {1.0, 0.0}; viennagrid_int vertex1;
  viennagrid_numeric coords2[2] = {1.0, 1.0}; viennagrid_int vertex2;
  viennagrid_numeric coords3[2] = {0.0, 2.0}; viennagrid_int vertex3;

  err = viennagrid_mesh_vertex_create(my_mesh, coords0, &vertex0); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords1, &vertex1); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords2, &vertex2); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords3, &vertex3); ERROR_CHECK(err);

  viennagrid_int element0_vertices[3] = {vertex0, vertex1, vertex2}; viennagrid_int element0;
  viennagrid_int element1_vertices[3] = {vertex0, vertex2, vertex3}; viennagrid_int element1;

  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, element0_vertices, &element0); ERROR_CHECK(err);
  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, element1_vertices, &element1); ERROR_CHECK(err);

  viennagrid_numeric volume0;
  err = viennagrid_element_volume(my_mesh, 2, element0, &volume0); ERROR_CHECK(err);
  if (std::fabs(volume0 - 0.5) > 1e-4)
  {
    std::cerr << "ERROR: volume check failed for volume0: " << volume0 << " vs. " << 0.5 << std::endl;
    return EXIT_FAILURE;
  }

  viennagrid_numeric volume1;
  err = viennagrid_element_volume(my_mesh, 2, element1, &volume1); ERROR_CHECK(err);
  if (std::fabs(volume1 - 1.0) > 1e-4)
  {
    std::cerr << "ERROR: volume check failed for volume1: " << volume1 << " vs. " << 1.0 << std::endl;
    return EXIT_FAILURE;
  }

  // Mesh volume:
  viennagrid_numeric volume_mesh;
  err = viennagrid_mesh_volume(my_mesh, &volume_mesh); ERROR_CHECK(err);
  if (std::fabs(volume_mesh - 1.5) > 1e-4)
  {
    std::cerr << "ERROR: mesh volume check failed: " << volume_mesh << " vs. " << 1.5 << std::endl;
    return EXIT_FAILURE;
  }

  err = viennagrid_mesh_release(my_mesh); ERROR_CHECK(err);

  std::cout << " PASSED" << std::endl;
  return EXIT_SUCCESS;
}

//
// 2.0 * --_      _-- *
//     |     --_--    |
//     |       * 1.0  |
//     |       |      |
//     *-------*------*
//   0.0      1.0    2.0
//
int test_2d_quadrilateral()
{
  viennagrid_error err;

  std::cout << "2D, quadrilateral mesh ..."; std::cout.flush();
  viennagrid_mesh  my_mesh;
  err = viennagrid_mesh_create(&my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_mesh, 2); ERROR_CHECK(err);

  viennagrid_numeric coords0[2] = {0.0, 0.0}; viennagrid_int vertex0;
  viennagrid_numeric coords1[2] = {1.0, 0.0}; viennagrid_int vertex1;
  viennagrid_numeric coords2[2] = {2.0, 0.0}; viennagrid_int vertex2;
  viennagrid_numeric coords3[2] = {0.0, 2.0}; viennagrid_int vertex3;
  viennagrid_numeric coords4[2] = {1.0, 1.0}; viennagrid_int vertex4;
  viennagrid_numeric coords5[2] = {2.0, 2.0}; viennagrid_int vertex5;

  err = viennagrid_mesh_vertex_create(my_mesh, coords0, &vertex0); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords1, &vertex1); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords2, &vertex2); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords3, &vertex3); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords4, &vertex4); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords5, &vertex5); ERROR_CHECK(err);

  viennagrid_int element0_vertices[4] = {vertex0, vertex1, vertex3, vertex4}; viennagrid_int element0;
  viennagrid_int element1_vertices[4] = {vertex1, vertex2, vertex4, vertex5}; viennagrid_int element1;

  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, 4, element0_vertices, &element0); ERROR_CHECK(err);
  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL, 4, element1_vertices, &element1); ERROR_CHECK(err);

  viennagrid_numeric volume0;
  err = viennagrid_element_volume(my_mesh, 2, element0, &volume0); ERROR_CHECK(err);
  if (std::fabs(volume0 - 1.5) > 1e-4)
  {
    std::cerr << "ERROR: volume check failed for volume0: " << volume0 << " vs. " << 1.5 << std::endl;
    return EXIT_FAILURE;
  }

  viennagrid_numeric volume1;
  err = viennagrid_element_volume(my_mesh, 2, element1, &volume1); ERROR_CHECK(err);
  if (std::fabs(volume1 - 1.5) > 1e-4)
  {
    std::cerr << "ERROR: volume check failed for volume1: " << volume1 << " vs. " << 1.5 << std::endl;
    return EXIT_FAILURE;
  }

  // Mesh volume:
  viennagrid_numeric volume_mesh;
  err = viennagrid_mesh_volume(my_mesh, &volume_mesh); ERROR_CHECK(err);
  if (std::fabs(volume_mesh - 3) > 1e-4)
  {
    std::cerr << "ERROR: mesh volume check failed: " << volume_mesh << " vs. " << 3.0 << std::endl;
    return EXIT_FAILURE;
  }

  err = viennagrid_mesh_release(my_mesh); ERROR_CHECK(err);

  std::cout << " PASSED" << std::endl;
  return EXIT_SUCCESS;
}

//
//            * (2.4, 2.6, 3.1)
//(1.1,3,1)/ / | \   (common tip)
//        * /  |  * (2.1, 3.0, 1.0)
//        |/   | /
//        *----* (2.0, 2.0, 1.0)
//  (1,2,1)
int test_3d_tetrahedron()
{
  viennagrid_error err;

  std::cout << "3D, tetrahedral mesh ..."; std::cout.flush();
  viennagrid_mesh  my_mesh;
  err = viennagrid_mesh_create(&my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_mesh, 3); ERROR_CHECK(err);

  viennagrid_numeric coords0[3] = {1.0, 2.0, 1.0}; viennagrid_int vertex0;
  viennagrid_numeric coords1[3] = {2.0, 2.0, 1.0}; viennagrid_int vertex1;
  viennagrid_numeric coords2[3] = {2.1, 3.0, 1.0}; viennagrid_int vertex2;
  viennagrid_numeric coords3[3] = {1.1, 3.0, 1.0}; viennagrid_int vertex3;
  viennagrid_numeric coords4[3] = {2.4, 2.6, 3.1}; viennagrid_int vertex4;

  err = viennagrid_mesh_vertex_create(my_mesh, coords0, &vertex0); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords1, &vertex1); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords2, &vertex2); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords3, &vertex3); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords4, &vertex4); ERROR_CHECK(err);

  viennagrid_int element0_vertices[4] = {vertex0, vertex1, vertex2, vertex4}; viennagrid_int element0;
  viennagrid_int element1_vertices[4] = {vertex0, vertex2, vertex3, vertex4}; viennagrid_int element1;

  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, element0_vertices, &element0); ERROR_CHECK(err);
  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, element1_vertices, &element1); ERROR_CHECK(err);

  viennagrid_numeric ref_volume = 0.5 * 2.1 / 3.0;

  viennagrid_numeric volume0;
  err = viennagrid_element_volume(my_mesh, 3, element0, &volume0); ERROR_CHECK(err);
  if (std::fabs(volume0 - ref_volume) > 1e-4)
  {
    std::cerr << "ERROR: volume check failed for volume0: " << volume0 << " vs. " << ref_volume << std::endl;
    return EXIT_FAILURE;
  }

  viennagrid_numeric volume1;
  err = viennagrid_element_volume(my_mesh, 3, element1, &volume1); ERROR_CHECK(err);
  if (std::fabs(volume1 - ref_volume) > 1e-4)
  {
    std::cerr << "ERROR: volume check failed for volume1: " << volume1 << " vs. " << ref_volume << std::endl;
    return EXIT_FAILURE;
  }

  // Mesh volume:
  viennagrid_numeric volume_mesh;
  err = viennagrid_mesh_volume(my_mesh, &volume_mesh); ERROR_CHECK(err);
  if (std::fabs(volume_mesh - 2 * ref_volume) > 1e-4)
  {
    std::cerr << "ERROR: mesh volume check failed: " << volume_mesh << " vs. " << 2 * ref_volume << std::endl;
    return EXIT_FAILURE;
  }

  err = viennagrid_mesh_release(my_mesh); ERROR_CHECK(err);

  std::cout << " PASSED" << std::endl;
  return EXIT_SUCCESS;
}

//
// cross-section (x,y), (x,z) view:
//
//  *           *
//  |  \     /  |
//  |     *     |
//  |     O     |    (symmetric with respect to origin)
//  |     *     |
//  |  /     \  |
//  *           *
//
//
// (attempt of a) 3D view:
//
//        *                        ---- *
//      /    \              --- /     / |
//    /          \       /          /   |
//  * -- __           *           /     |
//  |       -- __    /      __--*       |
//  |              *  ---/      |       *
//  |              |            |     /
//  |       ___ -- * __         |   /
//  | ___---            ----___ | /
//  *                           *
//
int test_3d_hexahedron()
{
  viennagrid_error err;

  std::cout << "3D, hexahedral mesh ..."; std::cout.flush();
  viennagrid_mesh  my_mesh;
  err = viennagrid_mesh_create(&my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_mesh, 3); ERROR_CHECK(err);

  viennagrid_numeric coords0[3] = {-1.0, -2.0, -2.0}; viennagrid_int vertex0;
  viennagrid_numeric coords1[3] = { 0.0, -1.0, -1.0}; viennagrid_int vertex1;
  viennagrid_numeric coords2[3] = { 1.0, -2.0, -2.0}; viennagrid_int vertex2;

  viennagrid_numeric coords3[3] = {-1.0, 2.0, -2.0}; viennagrid_int vertex3;
  viennagrid_numeric coords4[3] = { 0.0, 1.0, -1.0}; viennagrid_int vertex4;
  viennagrid_numeric coords5[3] = { 1.0, 2.0, -2.0}; viennagrid_int vertex5;

  viennagrid_numeric coords6[3] = {-1.0, -2.0, 2.0}; viennagrid_int vertex6;
  viennagrid_numeric coords7[3] = { 0.0, -1.0, 1.0}; viennagrid_int vertex7;
  viennagrid_numeric coords8[3] = { 1.0, -2.0, 2.0}; viennagrid_int vertex8;

  viennagrid_numeric coords9 [3] = {-1.0, 2.0, 2.0}; viennagrid_int vertex9;
  viennagrid_numeric coords10[3] = { 0.0, 1.0, 1.0}; viennagrid_int vertex10;
  viennagrid_numeric coords11[3] = { 1.0, 2.0, 2.0}; viennagrid_int vertex11;

  err = viennagrid_mesh_vertex_create(my_mesh, coords0, &vertex0); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords1, &vertex1); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords2, &vertex2); ERROR_CHECK(err);

  err = viennagrid_mesh_vertex_create(my_mesh, coords3, &vertex3); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords4, &vertex4); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords5, &vertex5); ERROR_CHECK(err);

  err = viennagrid_mesh_vertex_create(my_mesh, coords6, &vertex6); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords7, &vertex7); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords8, &vertex8); ERROR_CHECK(err);

  err = viennagrid_mesh_vertex_create(my_mesh, coords9,  &vertex9);  ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords10, &vertex10); ERROR_CHECK(err);
  err = viennagrid_mesh_vertex_create(my_mesh, coords11, &vertex11); ERROR_CHECK(err);

  viennagrid_int element0_vertices[8] = {vertex0, vertex1, vertex3, vertex4, vertex6, vertex7, vertex9, vertex10}; viennagrid_int element0;
  viennagrid_int element1_vertices[8] = {vertex1, vertex2, vertex4, vertex5, vertex7, vertex8, vertex10, vertex11}; viennagrid_int element1;

  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON, 8, element0_vertices, &element0); ERROR_CHECK(err);
  err = viennagrid_mesh_element_create(my_mesh, VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON, 8, element1_vertices, &element1); ERROR_CHECK(err);

  viennagrid_numeric ref_volume = 28.0 / 3.0;

  viennagrid_numeric volume0;
  err = viennagrid_element_volume(my_mesh, 3, element0, &volume0); ERROR_CHECK(err);
  if (std::fabs(volume0 - ref_volume) > 1e-4)
  {
    std::cerr << "ERROR: volume check failed for volume0: " << volume0 << " vs. " << ref_volume << std::endl;
    return EXIT_FAILURE;
  }

  viennagrid_numeric volume1;
  err = viennagrid_element_volume(my_mesh, 3, element1, &volume1); ERROR_CHECK(err);
  if (std::fabs(volume1 - ref_volume) > 1e-4)
  {
    std::cerr << "ERROR: volume check failed for volume1: " << volume1 << " vs. " << ref_volume << std::endl;
    return EXIT_FAILURE;
  }

  // Mesh volume:
  viennagrid_numeric volume_mesh;
  err = viennagrid_mesh_volume(my_mesh, &volume_mesh); ERROR_CHECK(err);
  if (std::fabs(volume_mesh - 2 * ref_volume) > 1e-4)
  {
    std::cerr << "ERROR: mesh volume check failed: " << volume_mesh << " vs. " << 2 * ref_volume << std::endl;
    return EXIT_FAILURE;
  }

  err = viennagrid_mesh_release(my_mesh); ERROR_CHECK(err);

  std::cout << " PASSED" << std::endl;
  return EXIT_SUCCESS;
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  // 1d
  if (test_1d() != EXIT_SUCCESS)
    return EXIT_FAILURE;

  // 2d
  if (test_2d_triangle() != EXIT_SUCCESS)
    return EXIT_FAILURE;

  if (test_2d_quadrilateral() != EXIT_SUCCESS)
    return EXIT_FAILURE;

  // 3d
  if (test_3d_tetrahedron() != EXIT_SUCCESS)
    return EXIT_FAILURE;

  if (test_3d_hexahedron() != EXIT_SUCCESS)
    return EXIT_FAILURE;

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
