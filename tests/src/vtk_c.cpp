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
#include <cstdio>
#include <vector>
#include <cmath>
#include <string>
#include <iostream>

#include "viennagrid/viennagrid.h"

#define ERROR_CHECK(err) do { if (err != VIENNAGRID_SUCCESS) { printf("Return value %ld invalid in line %ld \n", (long)err, (long)__LINE__); return (int)err; } } while (0);


int test(std::string infile, std::string outfile, std::string outfile2)
{
  viennagrid_error err;

  viennagrid_mesh           my_mesh;

  // create mesh object
  err = viennagrid_mesh_create(&my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_mesh, 2); ERROR_CHECK(err);

  // read mesh from file:
  viennagrid_mesh_io my_mesh_io;
  err = viennagrid_mesh_io_create(&my_mesh_io); ERROR_CHECK(err);
  err = viennagrid_mesh_io_mesh_set(my_mesh_io, my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_io_read(my_mesh_io, infile.c_str()); ERROR_CHECK(err);
  err = viennagrid_mesh_io_release(my_mesh_io); ERROR_CHECK(err);

  //
  // Create various quantity fields:
  //
  viennagrid_quantity_field scalar_vertex_field, vector_vertex_field, scalar_cell_field, vector_cell_field;
  err = viennagrid_quantity_field_create(&scalar_vertex_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_init(scalar_vertex_field, 0, VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC, 1, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE); ERROR_CHECK(err);
  err = viennagrid_quantity_field_name_set(scalar_vertex_field, "scalar_vertex_field"); ERROR_CHECK(err);

  err = viennagrid_quantity_field_create(&vector_vertex_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_init(vector_vertex_field, 0, VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC, 3, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE); ERROR_CHECK(err);
  err = viennagrid_quantity_field_name_set(vector_vertex_field, "vector_vertex_field"); ERROR_CHECK(err);

  std::vector<viennagrid_numeric> centroid(3);
  viennagrid_int *vertices_begin, *vertices_end;
  err = viennagrid_mesh_elements_get(my_mesh, 0, &vertices_begin, &vertices_end); ERROR_CHECK(err);
  for (viennagrid_int *vit  = vertices_begin;
                       vit != vertices_end;
                     ++vit)
  {
    err = viennagrid_element_centroid(my_mesh, *vit, &(centroid[0])); ERROR_CHECK(err);
    err = viennagrid_quantity_field_value_set(scalar_vertex_field, *vit, &(centroid[0])); ERROR_CHECK(err);
    err = viennagrid_quantity_field_value_set(vector_vertex_field, *vit, &(centroid[0])); ERROR_CHECK(err);
  }


  err = viennagrid_quantity_field_create(&scalar_cell_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_init(scalar_cell_field, 2, VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC, 1, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE); ERROR_CHECK(err);
  err = viennagrid_quantity_field_name_set(scalar_cell_field, "scalar_cell_field"); ERROR_CHECK(err);

  err = viennagrid_quantity_field_create(&vector_cell_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_init(vector_cell_field, 2, VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC, 3, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE); ERROR_CHECK(err);
  err = viennagrid_quantity_field_name_set(vector_cell_field, "vector_cell_field"); ERROR_CHECK(err);

  viennagrid_int *cells_begin, *cells_end;
  err = viennagrid_mesh_elements_get(my_mesh, 2, &cells_begin, &cells_end); ERROR_CHECK(err);
  for (viennagrid_int *cit  = cells_begin;
                       cit != cells_end;
                     ++cit)
  {
    err = viennagrid_element_centroid(my_mesh, *cit, &(centroid[0])); ERROR_CHECK(err);
    err = viennagrid_quantity_field_value_set(scalar_cell_field, *cit, &(centroid[0])); ERROR_CHECK(err);
    err = viennagrid_quantity_field_value_set(vector_cell_field, *cit, &(centroid[0])); ERROR_CHECK(err);
  }

  //
  // First write to VTK:
  //
  std::cout << "Writing VTK mesh..." << std::endl;
  viennagrid_mesh_io mesh_io;
  err = viennagrid_mesh_io_create(&mesh_io); ERROR_CHECK(err);

  err = viennagrid_mesh_io_mesh_set(mesh_io, my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_io_quantity_field_set(mesh_io, scalar_vertex_field); ERROR_CHECK(err);
  err = viennagrid_mesh_io_quantity_field_set(mesh_io, vector_vertex_field); ERROR_CHECK(err);
  err = viennagrid_mesh_io_quantity_field_set(mesh_io, scalar_cell_field); ERROR_CHECK(err);
  err = viennagrid_mesh_io_quantity_field_set(mesh_io, vector_cell_field); ERROR_CHECK(err);

  err = viennagrid_mesh_io_write_with_filetype(mesh_io, outfile.c_str(), VIENNAGRID_FILETYPE_VTK_MESH); ERROR_CHECK(err);
  err = viennagrid_mesh_io_release(mesh_io); ERROR_CHECK(err);


  //
  // Read mesh written to VTK:
  //
  std::cout << "Reading VTK mesh..." << std::endl;
  err = viennagrid_mesh_io_create(&mesh_io); ERROR_CHECK(err);

  viennagrid_mesh           my_mesh_2;
  err = viennagrid_mesh_create(&my_mesh_2); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_mesh_2, 2); ERROR_CHECK(err);

  err = viennagrid_mesh_io_mesh_set(mesh_io, my_mesh_2); ERROR_CHECK(err);

  err = viennagrid_mesh_io_read_with_filetype(mesh_io, (outfile + ".vtu").c_str(), VIENNAGRID_FILETYPE_VTK_MESH); ERROR_CHECK(err);

  viennagrid_int num_fields;
  err = viennagrid_mesh_io_quantity_field_count(mesh_io, &num_fields); ERROR_CHECK(err);

  std::vector<viennagrid_quantity_field> input_fields(static_cast<std::size_t>(num_fields)); ERROR_CHECK(err);
  for (viennagrid_int i=0; i<num_fields; ++i)
  {
    err = viennagrid_mesh_io_quantity_field_get_by_index(mesh_io, i, &(input_fields.at(i))); ERROR_CHECK(err);
    err = viennagrid_quantity_field_retain(input_fields.at(i)); ERROR_CHECK(err);  // Note: mesh_io will be released
  }
  err = viennagrid_mesh_io_release(mesh_io); ERROR_CHECK(err);


  //
  // Write again:
  //
  std::cout << "Writing VTK mesh..." << std::endl;
  err = viennagrid_mesh_io_create(&mesh_io); ERROR_CHECK(err);

  err = viennagrid_mesh_io_mesh_set(mesh_io, my_mesh_2); ERROR_CHECK(err);
  for (viennagrid_int i=0; i<num_fields; ++i)
  {
    err = viennagrid_mesh_io_quantity_field_set(mesh_io, input_fields[i]); ERROR_CHECK(err);
  }
  err = viennagrid_mesh_io_write_with_filetype(mesh_io, outfile2.c_str(), VIENNAGRID_FILETYPE_VTK_MESH); ERROR_CHECK(err);
  err = viennagrid_mesh_io_release(mesh_io); ERROR_CHECK(err);


  //
  // Clean up:
  //
  err = viennagrid_quantity_field_release(scalar_vertex_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_release(vector_vertex_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_release(scalar_cell_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_release(vector_cell_field); ERROR_CHECK(err);

  for (viennagrid_int i=0; i<num_fields; ++i)
  {
    err = viennagrid_quantity_field_release(input_fields.at(i)); ERROR_CHECK(err);
  }

  err = viennagrid_mesh_release(my_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_release(my_mesh_2); ERROR_CHECK(err);

  return EXIT_SUCCESS;
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::string path = "../examples/data/";

  std::string infile = path + "sshape2d.mesh";
  std::string outfile  = "./out_sshape2d";
  std::string outfile2 = "./out_sshape2d_2";

  viennagrid_error err;
  err = test(infile, outfile, outfile2); ERROR_CHECK(err);

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
