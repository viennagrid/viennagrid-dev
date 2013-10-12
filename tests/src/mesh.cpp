/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
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


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << " ------ Part 1: Simplex meshs ------- " << std::endl;

  std::cout << "* instantiating simplex 10u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::vertex_1d>::type      mesh_simplex_10u;
  std::cout << "* instantiating simplex 20u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::vertex_2d>::type      mesh_simplex_20u;
  std::cout << "* instantiating simplex 30u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::vertex_3d>::type      mesh_simplex_30u;
  std::cout << "* instantiating simplex 11u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::line_1d>::type        mesh_simplex_11u;
  std::cout << "* instantiating simplex 21u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::line_2d>::type        mesh_simplex_21u;
  std::cout << "* instantiating simplex 31u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::line_3d>::type        mesh_simplex_31u;
  std::cout << "* instantiating simplex 22u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::triangular_2d>::type  mesh_simplex_22u;
  std::cout << "* instantiating simplex 32u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::triangular_3d>::type  mesh_simplex_32u;
  std::cout << "* instantiating simplex 33u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::tetrahedral_3d>::type mesh_simplex_33u;


  std::cout << " ------ Part 2: Non-simplex meshs ------- " << std::endl;

  std::cout << "* instantiating quadrilateral 22u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::quadrilateral_2d>::type mesh_22u;
  std::cout << "* instantiating quadrilateral 32u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::quadrilateral_3d>::type mesh_32u;
  std::cout << "* instantiating hexagonal 33u mesh.. " << std::endl;
  viennagrid::result_of::mesh<viennagrid::config::hexahedral_3d>::type mesh_33u;

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
