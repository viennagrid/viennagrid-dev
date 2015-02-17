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

#include "viennagrid/core.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"


int main()
{
  //
  // Define the necessary types:
  //

  typedef viennagrid::mesh_hierarchy_t MeshHierarchyType;
  typedef viennagrid::result_of::mesh<MeshHierarchyType>::type MeshType;

  viennagrid_serialized_mesh_hierarchy serialized_mesh;
  viennagrid_serialized_mesh_hierarchy_make(&serialized_mesh);

  {
    viennagrid::io::vtk_reader<MeshType> reader;
    MeshHierarchyType mesh_hierarchy;
    MeshType mesh = mesh_hierarchy.root();
    reader(mesh, "../data/tets_with_data_main.pvd");

    mesh_hierarchy.serialize(serialized_mesh);
  }

  {
    MeshHierarchyType mesh_hierarchy_2;
    mesh_hierarchy_2.deserialize(serialized_mesh);
    MeshType mesh_2 = mesh_hierarchy_2.root();

    viennagrid::io::vtk_writer<MeshType> writer;
    writer(mesh_2, "deserialized");
  }


  viennagrid_serialized_mesh_hierarchy_delete(serialized_mesh);


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
