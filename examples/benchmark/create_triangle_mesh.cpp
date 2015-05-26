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
#include "boost/bind.hpp"


template<typename F>
double bench(int bench_count, F f)
{
  viennautils::Timer timer;

  double time = 0.0;
  for (int i = 0; i != bench_count; ++i)
  {
    timer.start();
    f();
    time += timer.get();
  }

  return time / bench_count;
}

template<typename F>
double bench(int bench_count, int cell_count, F f)
{
  viennagrid::mesh_t mesh;
  return bench(bench_count, boost::bind(f, mesh, cell_count) );
}



typedef void (*BENCH_FUNC)(viennagrid::mesh_t const &, int);


int main()
{
  int bench_count = 10;
  int cell_count = 100000;

//   std::cout << "make triangles C: " << bench(bench_count, cell_count, (BENCH_FUNC)make_aabb_triangles_C) << std::endl;
//   std::cout << "make triangles CPP: " << bench(bench_count, cell_count, (BENCH_FUNC)make_aabb_triangles_CPP) << std::endl;
  std::cout << "make tetrahedrons C: " << bench(bench_count, cell_count, (BENCH_FUNC)make_aabb_tetrahedrons_C) << std::endl;
  std::cout << "make tetrahedrons CPP: " << bench(bench_count, cell_count, (BENCH_FUNC)make_aabb_tetrahedrons_CPP) << std::endl;


//   bench_triangle_mesh_creation_C(10, cell_count);
//   bench_triangle_mesh_creation_CPP(10, cell_count);


//   {
//     viennagrid::mesh_t mesh;
//
//     int N = std::pow(cell_count/6, 1.0/3.0)+0.5;
//     viennautils::Timer timer;
//     timer.start();
//     make_aabb_tetrahedrons_C(mesh.internal(), -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, N+1, N+1, N+1);
//     double t = timer.get();
//     std::cout << "Making mesh with " << N*N*N*6 << " tetrahedrons took " << t << "sec" << std::endl;
//
//     viennagrid::io::vtk_writer<viennagrid::mesh_t> writer;
//     writer(mesh, "output");
//   }



  return EXIT_SUCCESS;
}
