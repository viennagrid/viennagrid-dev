#ifndef VIENNAGRID_BENCHMARK_BENCH_HPP
#define VIENNAGRID_BENCHMARK_BENCH_HPP


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
void bench_creation(int cell_count_end, int bench_count, F f)
{
  viennautils::Timer timer;

  for (int cell_count = 100; cell_count <= cell_count_end; cell_count *= 10)
  {
    double time = 0.0;
    int actual_cell_count = 0;
    for (int i = 0; i != bench_count; ++i)
    {
      viennagrid::mesh_t mesh;
      timer.start();
      f(mesh, cell_count);
      time += timer.get();
      actual_cell_count = viennagrid::cells(mesh).size();
    }

    time /= bench_count;

    std::cout << "cc = " << actual_cell_count << " t = " << time << "  t/cc = " << time/actual_cell_count << std::endl;
  }
}


template<typename MCF, typename F>
void bench_iteration(int cell_count_end, int bench_count, MCF mcf, F f)
{
  viennautils::Timer timer;

  for (int cell_count = 100; cell_count <= cell_count_end; cell_count *= 10)
  {
    viennagrid::mesh_t mesh;
    mcf(mesh, cell_count);

    double time = 0.0;
    int actual_cell_count = 0;
    for (int i = 0; i != bench_count; ++i)
    {
      timer.start();
      f(mesh);
      time += timer.get();
      actual_cell_count = viennagrid::cells(mesh).size();
    }

    time /= bench_count;

    std::cout << "cc = " << actual_cell_count << " t = " << time << "  t/cc = " << time/actual_cell_count << std::endl;
  }
}


#endif
