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

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/mphtxt_writer.hpp"


int main()
{
  typedef viennagrid::mesh< viennagrid::config::tetrahedral_3d >     Mesh;
  typedef viennagrid::result_of::segmentation<Mesh>::type              Segmentation;

  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Convert to MPHTXT --" << std::endl;
  std::cout << "--------------------------------------------" << std::endl;
  std::cout << std::endl;

  Mesh mesh;
  Segmentation segmentation(mesh);


  viennagrid::io::vtk_reader<Mesh, Segmentation> reader;
  reader(mesh, segmentation, "../data/tets_with_data_main.pvd");


  viennagrid::io::mphtxt_writer writer;
  writer(mesh, segmentation, "output.mphtxt");


  std::cout << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
