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
  int cell_count = 100;
  int region_count = 4;


//   {
//     viennagrid::mesh mesh;
//     make_aabb_triangles_C(mesh, cell_count);
//     viennagrid::io::vtk_writer<viennagrid::mesh> writer;
//     writer(mesh, "triangles");
//   }
//
//   {
//     viennagrid::mesh mesh;
//     make_aabb_tetrahedrons_C(mesh, cell_count);
//     viennagrid::io::vtk_writer<viennagrid::mesh> writer;
//     writer(mesh, "tetrahedrons");
//   }
//
//
//
//   {
//     viennagrid::mesh mesh;
//     make_aabb_triangles_region_C(mesh, cell_count, region_count);
//     viennagrid::io::vtk_writer<viennagrid::mesh> writer;
//     writer(mesh, "triangles_region");
//   }
//
//   {
//     viennagrid::mesh mesh;
//     make_aabb_tetrahedrons_region_C(mesh, cell_count, region_count);
//     viennagrid::io::vtk_writer<viennagrid::mesh> writer;
//     writer(mesh, "tetrahedrons_region");
//   }



  {
    viennagrid::mesh mesh;
    batch_make_aabb_triangles_region_C(mesh, cell_count, region_count, false);

    typedef viennagrid::result_of::const_element_range<viennagrid::mesh>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    viennagrid::io::vtk_writer<viennagrid::mesh> writer;
    writer(mesh, "batch_triangles_region");
  }

  return EXIT_SUCCESS;
}
