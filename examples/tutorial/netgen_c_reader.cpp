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

#include "viennagridpp/core.hpp"
#include "viennagridpp/io/vtk_writer.hpp"
#include "viennagridpp/algorithm/refine.hpp"


int main()
{
  //
  // Define the necessary types:
  //

  typedef viennagrid::mesh_hierarchy                              MeshHierarchyType;
  typedef viennagrid::result_of::mesh<MeshHierarchyType>::type    MeshType;


  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Setup of a mesh with two segments -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the mesh and the segmentation and create 2 segments:
  //
  MeshType mesh;

  viennagrid_mesh_io mesh_io;
  viennagrid_mesh_io_create(&mesh_io);
  viennagrid_mesh_io_mesh_set(mesh_io, mesh.internal());

  viennagrid_mesh_io_read( mesh_io, "../data/cube3072.mesh" );

  viennagrid_mesh_io_release(mesh_io);

  viennagrid::io::vtk_writer<MeshType> writer;
  writer(mesh, "netgen_c_reader");


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
