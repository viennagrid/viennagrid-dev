/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */
//#define VIENNAGRID_DEBUG_IO

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/opendx_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"


void print(std::vector<std::string> const & vec)
{
  for (std::size_t i=0; i<vec.size(); ++i)
    std::cout << vec[i] << std::endl;
}

template <typename ConfigType, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile, std::string const & outfile, bool run_check = false)
{

  typedef typename viennagrid::result_of::domain<ConfigType>::type        Domain;
  typedef typename viennagrid::result_of::segment<ConfigType>::type       SegmentType;
  typedef typename ConfigType::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::topology_level>::type   CellType;

  typedef typename viennagrid::result_of::ncell_range<Domain, 0>::type   VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
  typedef typename viennagrid::result_of::ncell_range<SegmentType, 0>::type       SegmentVertexContainer;
  typedef typename viennagrid::result_of::iterator<SegmentVertexContainer>::type  SegmentVertexIterator;
      
  typedef typename viennagrid::result_of::ncell_range<Domain, 1>::type   EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::topology_level-1>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::topology_level>::type     CellRange;
  typedef typename viennagrid::result_of::iterator<CellRange>::type                              CellIterator;

  Domain domain;
  
  try
  {
    my_reader(domain, infile);
  }
  catch (std::exception const & ex)
  {
     std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }

  
  //write some dummy data:
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {
    if (run_check)
    {
      double data_double = viennadata::access<std::string, double>("vtk_data_2")(*vit);
      long data_long = viennadata::access<std::string, long>("vtk_data_2")(*vit);
      std::vector<double> data_point = viennadata::access<std::string, std::vector<double> >("vtk_data_2")(*vit);
      
      assert( fabs(data_double - vit->point()[0]) < 1e-4 && "Vertex check failed: data_double!");
      assert( (data_long == vit->id()) && "Vertex check failed: data_long!");
      assert( fabs(data_point[0] - (*vit)[0]) < 1e-4 
             && fabs(data_point[1] - (*vit)[1]) < 1e-4
             && "Vertex check failed: data_point!");
    }
    else
    {
      viennadata::access<std::string, double>("vtk_data")(*vit) = vit->point()[0];
      viennadata::access<std::string, long>("vtk_data")(*vit) = vit->id();
      
      viennadata::access<std::string, std::vector<double> >("vtk_data")(*vit).resize(3);
      viennadata::access<std::string, std::vector<double> >("vtk_data")(*vit)[0] =(*vit)[0];
      viennadata::access<std::string, std::vector<double> >("vtk_data")(*vit)[1] = (*vit)[1];
    }
  }

  CellRange cells = viennagrid::ncells(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                   ++cit)
  {
    if (run_check)
    {
      double data_double = viennadata::access<std::string, double>("vtk_data_2")(*cit);
      long data_long = viennadata::access<std::string, long>("vtk_data_2")(*cit);
      std::vector<double> data_point = viennadata::access<std::string, std::vector<double> >("vtk_data_2")(*cit);
      
      assert( fabs(data_double - viennagrid::centroid(*cit)[0]) < 1e-4 && "Cell check failed: data_double!");
      assert( (data_long == cit->id()) && "Cell check failed: data_long!");
      assert( fabs(data_point[0] - viennagrid::centroid(*cit)[0]) < 1e-4 
             && fabs(data_point[1] - viennagrid::centroid(*cit)[1]) < 1e-4
             && "Cell check failed: data_point!");
    }
    else
    {
      viennadata::access<std::string, double>("vtk_data")(*cit) = viennagrid::centroid(*cit)[0];
      viennadata::access<std::string, long>("vtk_data")(*cit) = cit->id();
      
      viennadata::access<std::string, std::vector<double> >("vtk_data")(*cit).resize(3);
      viennadata::access<std::string, std::vector<double> >("vtk_data")(*cit)[0] = viennagrid::centroid(*cit)[0];
      viennadata::access<std::string, std::vector<double> >("vtk_data")(*cit)[1] = viennagrid::centroid(*cit)[1];
    }
  }

  //test writers:
  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_vtk_writer, "vtk_data", "data_double");
  viennagrid::io::add_scalar_data_on_vertices<std::string, long>(my_vtk_writer, "vtk_data", "data_long");
  viennagrid::io::add_vector_data_on_vertices<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_point");
  viennagrid::io::add_scalar_data_on_vertices<std::string, long>(my_vtk_writer, "vtk_data", "data_dummy");
  viennagrid::io::add_vector_data_on_vertices<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_dummy2");
  
  viennagrid::io::add_scalar_data_on_cells<std::string, double>(my_vtk_writer, "vtk_data", "data_double");
  viennagrid::io::add_scalar_data_on_cells<std::string, long>(my_vtk_writer, "vtk_data", "data_long");
  viennagrid::io::add_vector_data_on_cells<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_point");
  viennagrid::io::add_scalar_data_on_cells<std::string, long>(my_vtk_writer, "vtk_data", "data_dummy");
  viennagrid::io::add_vector_data_on_cells<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_dummy2");
  
  my_vtk_writer(domain, outfile + ".vtu");
   
  viennagrid::io::opendx_writer my_dx_writer;
  my_dx_writer(domain, outfile + ".odx");
}


int main()
{
  typedef viennagrid::result_of::domain<viennagrid::config::triangular_2d>::type        Domain2d;
  typedef viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type       Domain3d;
  
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  //Stage 1: Read from Netgen files, write to VTK
  viennagrid::io::netgen_reader my_netgen_reader;
  
  //test<viennagrid::config::line_1d>(my_netgen_reader, path + "line8.mesh", "io_1d");
  std::cout << "*********** Reading from Netgen, 2d ***********" << std::endl;
  test<viennagrid::config::triangular_2d>(my_netgen_reader, path + "square32.mesh", "io_2d");
  std::cout << "*********** Reading from Netgen, 3d ***********" << std::endl;
  test<viennagrid::config::tetrahedral_3d>(my_netgen_reader, path + "cube48.mesh", "io_3d");

  //Stage 2: Read VTK files, write to VTK
  //test<viennagrid::config::line_1d>("io_1d", "io_1d_2");
  
  viennagrid::io::vtk_reader<Domain2d>  vtk_reader_2d;
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(vtk_reader_2d, "vtk_data_2", "data_double");
  viennagrid::io::add_scalar_data_on_vertices<std::string, long>(vtk_reader_2d, "vtk_data_2", "data_long");
  viennagrid::io::add_vector_data_on_vertices<std::string, std::vector<double> >(vtk_reader_2d, "vtk_data_2", "data_point");

  viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_reader_2d, "vtk_data_2", "data_double");
  viennagrid::io::add_scalar_data_on_cells<std::string, long>(vtk_reader_2d, "vtk_data_2", "data_long");
  viennagrid::io::add_vector_data_on_cells<std::string, std::vector<double> >(vtk_reader_2d, "vtk_data_2", "data_point");
  
  std::cout << "*********** Reading from VTK, 2d ***********" << std::endl;
  test<viennagrid::config::triangular_2d>(vtk_reader_2d, "io_2d.vtu_main.pvd", "io_2d_2", true);
  
  std::cout << "-- Scalar vertex quantities: --" << std::endl;
  print(vtk_reader_2d.scalar_vertex_data_names(0));
  std::cout << "-- Vector vertex quantities: --" << std::endl;
  print(vtk_reader_2d.vector_vertex_data_names(0));
  std::cout << "-- Scalar cell quantities: --" << std::endl;
  print(vtk_reader_2d.scalar_cell_data_names(0));
  std::cout << "-- Vector cell quantities: --" << std::endl;
  print(vtk_reader_2d.vector_cell_data_names(0));
  
  assert( (vtk_reader_2d.scalar_vertex_data_names(0).size() == 3) && "Not all data parsed!");
  assert( (vtk_reader_2d.vector_vertex_data_names(0).size() == 2) && "Not all data parsed!");
  assert( (vtk_reader_2d.scalar_cell_data_names(0).size() == 3) && "Not all data parsed!");
  assert( (vtk_reader_2d.vector_cell_data_names(0).size() == 2) && "Not all data parsed!");

  
  viennagrid::io::vtk_reader<Domain3d>  vtk_reader_3d;
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(vtk_reader_3d, "vtk_data_2", "data_double");
  viennagrid::io::add_scalar_data_on_vertices<std::string, long>(vtk_reader_3d, "vtk_data_2", "data_long");
  viennagrid::io::add_vector_data_on_vertices<std::string, std::vector<double> >(vtk_reader_3d, "vtk_data_2", "data_point");

  viennagrid::io::add_scalar_data_on_cells<std::string, double>(vtk_reader_3d, "vtk_data_2", "data_double");
  viennagrid::io::add_scalar_data_on_cells<std::string, long>(vtk_reader_3d, "vtk_data_2", "data_long");
  viennagrid::io::add_vector_data_on_cells<std::string, std::vector<double> >(vtk_reader_3d, "vtk_data_2", "data_point");
  
  std::cout << "*********** Reading from VTK, 3d ***********" << std::endl;
  test<viennagrid::config::tetrahedral_3d>(vtk_reader_3d, "io_3d.vtu_main.pvd", "io_3d_2", true);
  
  std::cout << "-- Scalar vertex quantities: --" << std::endl;
  print(vtk_reader_3d.scalar_vertex_data_names(0));
  std::cout << "-- Vector vertex quantities: --" << std::endl;
  print(vtk_reader_3d.vector_vertex_data_names(0));
  std::cout << "-- Scalar cell quantities: --" << std::endl;
  print(vtk_reader_3d.scalar_cell_data_names(0));
  std::cout << "-- Vector cell quantities: --" << std::endl;
  print(vtk_reader_3d.vector_cell_data_names(0));
  
  assert( (vtk_reader_3d.scalar_vertex_data_names(0).size() == 3) && "Not all data parsed!");
  assert( (vtk_reader_3d.vector_vertex_data_names(0).size() == 2) && "Not all data parsed!");
  assert( (vtk_reader_3d.scalar_cell_data_names(0).size() == 3) && "Not all data parsed!");
  assert( (vtk_reader_3d.vector_cell_data_names(0).size() == 2) && "Not all data parsed!");
  
  //test<viennagrid::config::tetrahedral_3d>(vtk_reader_3d, "multi-segment_main.pvd", "io_3d_2");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
