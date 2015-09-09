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
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"


int main()
{
  //
  // Define the necessary types:
  //

  typedef viennagrid::mesh                                        MeshType;


  void * buffer;
  viennagrid_int size;


  {
    viennagrid::io::vtk_reader<MeshType> reader;
    MeshType mesh;
    reader(mesh, "../data/tets_with_data_main.pvd");

    mesh.serialize( &buffer, &size );
  }

  {
    MeshType mesh;
    mesh.deserialize( buffer, size );

    viennagrid::io::vtk_writer<MeshType> writer;
    writer(mesh, "deserialized");
  }

  viennagrid_delete(&buffer);



  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
