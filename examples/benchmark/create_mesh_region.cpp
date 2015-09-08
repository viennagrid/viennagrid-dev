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
#include "viennagrid/io/vtk_writer.hpp"

#include "common.hpp"
#include "bench.hpp"


int main()
{
  int bench_count = 10;
  int cell_count = 1000000;
  int region_count = 4;


  std::cout << "C triangles" << std::endl;
  bench_creation(cell_count, bench_count, boost::bind(make_aabb_triangles_region_C, _1, _2, region_count, false) );
  std::cout << std::endl;

  std::cout << "CPP triangles" << std::endl;
  bench_creation(cell_count, bench_count, boost::bind(make_aabb_triangles_region_CPP, _1, _2, region_count, false) );
  std::cout << std::endl;

  std::cout << "C tetrahedrons" << std::endl;
  bench_creation(cell_count, bench_count, boost::bind(make_aabb_tetrahedrons_region_C, _1, _2, region_count, false) );
  std::cout << std::endl;

  std::cout << "CPP tetrahedrons" << std::endl;
  bench_creation(cell_count, bench_count, boost::bind(make_aabb_tetrahedrons_region_CPP, _1, _2, region_count, false) );
  std::cout << std::endl;


  return EXIT_SUCCESS;
}
