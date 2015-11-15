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
#include <vector>
#include <cmath>

#include "refinement-common.hpp"

int tag_and_refine_mesh(viennagrid_mesh input_mesh, viennagrid_mesh & output_mesh)
{
  viennagrid_error err;

  // compute volume and surface of the input mesh
  viennagrid_numeric input_mesh_volume;
  err = viennagrid_mesh_volume(input_mesh, &input_mesh_volume); ERROR_CHECK(err);
  viennagrid_numeric input_mesh_surface;
  err = viennagrid_mesh_surface(input_mesh, &input_mesh_surface); ERROR_CHECK(err);

  std::cout << "Volume of input mesh: " << input_mesh_volume << std::endl;
  std::cout << "Surface of input mesh: " << input_mesh_surface << std::endl;

  // Tag some edges in the middle of the structure for refinement
  viennagrid_int num_edges;
  err = viennagrid_mesh_element_count(input_mesh, VIENNAGRID_ELEMENT_TYPE_LINE, &num_edges); ERROR_CHECK(err);
  std::vector<viennagrid_bool> edge_refinement_tags(num_edges);

  //Testing: Tag some edges with centroid at x \in [2,3]:
  viennagrid_dimension geo_dim;
  err = viennagrid_mesh_geometric_dimension_get(input_mesh, &geo_dim); ERROR_CHECK(err);
  std::vector<viennagrid_numeric> centroid(geo_dim);

  viennagrid_element_id *edges_begin, *edges_end;
  err = viennagrid_mesh_elements_get(input_mesh, 1, &edges_begin, &edges_end); ERROR_CHECK(err);
  for (viennagrid_element_id *eit  = edges_begin;
                              eit != edges_end;
                            ++eit)
  {
    err = viennagrid_element_centroid(input_mesh, *eit, &(centroid[0])); ERROR_CHECK(err);

    if (   (centroid[0] >= 2.0)
        && (centroid[0] <= 3.0)
        && (centroid[1] >= 0.0)
        && (centroid[1] <= 1.0))
    {
      edge_refinement_tags[viennagrid_index_from_element_id(*eit)] = true;
    }
  }

  // refine the mesh:
  err = viennagrid_mesh_refine(input_mesh, &(edge_refinement_tags[0]), NULL, output_mesh); ERROR_CHECK(err);

  // compute volume and surface of refined mesh:
  viennagrid_numeric output_mesh_volume;
  err = viennagrid_mesh_volume(output_mesh, &output_mesh_volume); ERROR_CHECK(err);
  viennagrid_numeric output_mesh_surface;
  err = viennagrid_mesh_surface(output_mesh, &output_mesh_surface); ERROR_CHECK(err);

  // check volume and surface of refined mesh:
  if ( std::fabs(output_mesh_volume - input_mesh_volume) / input_mesh_volume > 1e-3 )
  {
    std::cerr << "Error in check: Mesh volumes mismatch!" << std::endl;
    std::cerr << "Input mesh volume: " << input_mesh_volume << std::endl;
    std::cerr << "Output mesh volume: " << output_mesh_volume << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "Volume check passed!" << std::endl;

  if ( std::fabs(output_mesh_surface - input_mesh_surface) / input_mesh_surface > 1e-3 )
  {
    std::cerr << "Error in check: Mesh surfaces mismatch!" << std::endl;
    std::cerr << "Input mesh surface: " << input_mesh_surface << std::endl;
    std::cerr << "Output mesh surface: " << output_mesh_surface << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "Surface check passed!" << std::endl;

  return EXIT_SUCCESS;
}


int test(std::string & infile, std::string & outfile)
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

  viennagrid_mesh my_refined_mesh;
  err = viennagrid_mesh_create(&my_refined_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_refined_mesh, 2); ERROR_CHECK(err);
  tag_and_refine_mesh(my_mesh, my_refined_mesh);

  viennagrid_mesh my_twice_refined_mesh;
  err = viennagrid_mesh_create(&my_twice_refined_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_geometric_dimension_set(my_twice_refined_mesh, 2); ERROR_CHECK(err);
  tag_and_refine_mesh(my_refined_mesh, my_twice_refined_mesh);

  //test writers:
  std::cout << "Writing finest mesh..." << std::endl;
  viennagrid_mesh_io mesh_io;
  err = viennagrid_mesh_io_create(&mesh_io); ERROR_CHECK(err);

  viennagrid_quantity_field scalar_vertex_field, vector_vertex_field, scalar_cell_field, vector_cell_field;
  err = viennagrid_quantity_field_create(&scalar_vertex_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_init(scalar_vertex_field, 0, VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC, 1, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE); ERROR_CHECK(err);
  err = viennagrid_quantity_field_name_set(scalar_vertex_field, "scalar_vertex_field"); ERROR_CHECK(err);

  err = viennagrid_quantity_field_create(&vector_vertex_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_init(vector_vertex_field, 0, VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC, 3, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE); ERROR_CHECK(err);
  err = viennagrid_quantity_field_name_set(vector_vertex_field, "vector_vertex_field"); ERROR_CHECK(err);

  std::vector<viennagrid_numeric> centroid(3);
  viennagrid_int *vertices_begin, *vertices_end;
  err = viennagrid_mesh_elements_get(my_twice_refined_mesh, 0, &vertices_begin, &vertices_end); ERROR_CHECK(err);
  for (viennagrid_int *vit  = vertices_begin;
                       vit != vertices_end;
                     ++vit)
  {
    err = viennagrid_element_centroid(my_twice_refined_mesh, *vit, &(centroid[0])); ERROR_CHECK(err);
    //std::cout << "Centroid: " << centroid[0] << " " << centroid[1] << " " << centroid[2] << std::endl;
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
  err = viennagrid_mesh_elements_get(my_twice_refined_mesh, 2, &cells_begin, &cells_end); ERROR_CHECK(err);
  for (viennagrid_int *cit  = cells_begin;
                       cit != cells_end;
                     ++cit)
  {
    err = viennagrid_element_centroid(my_twice_refined_mesh, *cit, &(centroid[0])); ERROR_CHECK(err);
    err = viennagrid_quantity_field_value_set(scalar_cell_field, *cit, &(centroid[0])); ERROR_CHECK(err);
    err = viennagrid_quantity_field_value_set(vector_cell_field, *cit, &(centroid[0])); ERROR_CHECK(err);
  }

  err = viennagrid_mesh_io_mesh_set(mesh_io, my_twice_refined_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_io_quantity_field_set(mesh_io, scalar_vertex_field); ERROR_CHECK(err);
  err = viennagrid_mesh_io_quantity_field_set(mesh_io, vector_vertex_field); ERROR_CHECK(err);
  err = viennagrid_mesh_io_quantity_field_set(mesh_io, scalar_cell_field); ERROR_CHECK(err);
  err = viennagrid_mesh_io_quantity_field_set(mesh_io, vector_cell_field); ERROR_CHECK(err);

  err = viennagrid_mesh_io_write_with_filetype(mesh_io, outfile.c_str(), VIENNAGRID_FILETYPE_VTK_MESH); ERROR_CHECK(err);

  //err = viennagrid_mesh_io_mesh_set(mesh_io, my_refined_mesh); ERROR_CHECK(err);
  //err = viennagrid_mesh_io_write(mesh_io, (outfile + "_refined.vtu").c_str()); ERROR_CHECK(err);

  //err = viennagrid_mesh_io_mesh_set(mesh_io, my_twice_refined_mesh); ERROR_CHECK(err);
  //err = viennagrid_mesh_io_write(mesh_io, (outfile + "_double_refined.vtu").c_str()); ERROR_CHECK(err);
  err = viennagrid_mesh_io_release(mesh_io); ERROR_CHECK(err);

  err = viennagrid_quantity_field_release(scalar_vertex_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_release(vector_vertex_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_release(scalar_cell_field); ERROR_CHECK(err);
  err = viennagrid_quantity_field_release(vector_cell_field); ERROR_CHECK(err);

  err = viennagrid_mesh_release(my_twice_refined_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_release(my_refined_mesh); ERROR_CHECK(err);
  err = viennagrid_mesh_release(my_mesh); ERROR_CHECK(err);

  return EXIT_SUCCESS;
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::string path = "../examples/data/";

  std::string infile = path + "sshape2d.mesh";
  std::string outfile = "out_sshape2d"; // without ending

  viennagrid_error err;
  err = test(infile, outfile); ERROR_CHECK(err);

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
