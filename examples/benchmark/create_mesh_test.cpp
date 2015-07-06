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

#include "common.hpp"


int main()
{
  int cell_count = 10000;


  {
    viennagrid::mesh mesh;
    make_aabb_triangles_C(mesh, cell_count);
    viennagrid::io::vtk_writer<viennagrid::mesh> writer;
    writer(mesh, "triangles");
  }

  {
    viennagrid::mesh mesh;
    make_aabb_tetrahedrons_C(mesh, cell_count);
    viennagrid::io::vtk_writer<viennagrid::mesh> writer;
    writer(mesh, "tetrahedrons");
  }

  return EXIT_SUCCESS;
}
