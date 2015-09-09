/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/viennagrid.hpp"

#include "common.hpp"
#include "bench.hpp"
#include "iteration.hpp"


int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cout << "Usage: memory_benchmark <TRIANGLE_COUNT>" << std::endl;
    return EXIT_FAILURE;
  }



  typedef viennagrid::mesh MeshType;

  std::cout << "sizeof(viennagrid_bool) = " << sizeof(viennagrid_bool) << std::endl;
  std::cout << "sizeof(viennagrid_int) = " << sizeof(viennagrid_int) << std::endl;
  std::cout << "sizeof(viennagrid_error) = " << sizeof(viennagrid_error) << std::endl;
  std::cout << "sizeof(viennagrid_numeric) = " << sizeof(viennagrid_numeric) << std::endl;
  std::cout << "sizeof(viennagrid_element_type) = " << sizeof(viennagrid_element_type) << std::endl;
  std::cout << "sizeof(viennagrid_dimension) = " << sizeof(viennagrid_dimension) << std::endl;
  std::cout << "sizeof(std::vector<viennagrid_region>) = " << sizeof(std::vector<viennagrid_region>) << std::endl;

  std::cout << "sizeof(viennagrid_mesh) = " << sizeof(viennagrid_mesh) << std::endl;
  std::cout << "sizeof(viennagrid_region) = " << sizeof(viennagrid_region) << std::endl;
  std::cout << "sizeof(viennagrid_plc) = " << sizeof(viennagrid_plc) << std::endl;
  std::cout << "sizeof(viennagrid_quantity_field) = " << sizeof(viennagrid_quantity_field) << std::endl;
  std::cout << "sizeof(viennagrid_mesh_io) = " << sizeof(viennagrid_mesh_io) << std::endl;


  std::cout << std::endl << std::endl << std::endl;

  {
    MeshType mesh;

    batch_make_aabb_triangles_region_C(mesh, atoi(argv[1]), 4, false);
  //   batch_make_aabb_tetrahedrons_region_C(mesh, atoi(argv[1]), 4);


    int total_size = mesh.memory_size();
    std::cout << "memory =         " << total_size << std::endl;

    int vertex_count = viennagrid::elements(mesh, 0).size();
    int cell_count = viennagrid::elements(mesh, viennagrid::cell_dimension(mesh)).size();

    std::cout << "Vertex count:    " << vertex_count << std::endl;
    std::cout << "Triangle count:  " << cell_count << std::endl;
    std::cout << "size per cell:   " << (double)total_size/cell_count << std::endl;
  }

  std::cout << std::endl << std::endl << std::endl;


  {
    MeshType mesh;

    batch_make_aabb_triangles_region_C(mesh, atoi(argv[1]), 4, true);
  //   batch_make_aabb_tetrahedrons_region_C(mesh, atoi(argv[1]), 4);


    int total_size = mesh.memory_size();
    std::cout << "memory =         " << total_size << std::endl;

    int vertex_count = viennagrid::elements(mesh, 0).size();
    int cell_count = viennagrid::elements(mesh, viennagrid::cell_dimension(mesh)).size();

    std::cout << "Vertex count:    " << vertex_count << std::endl;
    std::cout << "Triangle count:  " << cell_count << std::endl;
    std::cout << "size per cell:   " << (double)total_size/cell_count << std::endl;
  }

  return EXIT_SUCCESS;
}
