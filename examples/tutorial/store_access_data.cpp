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

#ifdef _MSC_VER      //Visual Studio complains about potentially dangerous things, which are perfectly legal in our context
  #pragma warning( disable : 4355 )
#endif

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"


struct my_key {};

namespace viennadata
{
  namespace config
  {
    //
    // The following specialization sets a compile-time key dispatch for the 'MyKey' type
    //
    template <>
    struct key_dispatch<my_key>
    {
      typedef type_key_dispatch_tag    tag;
    };
  }
}




template <typename DomainType>
void exec_for_domain(DomainType const & domain,
                     std::string const & outfile)
{
  //
  // Get all the types needed later on:
  //
  typedef typename DomainType::config_type               ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type              VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type   CellType;

  typedef typename viennagrid::result_of::const_ncell_range<DomainType, 0>::type         VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type          VertexIterator;
      
  typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::dim>::type    CellRange;
  typedef typename viennagrid::result_of::iterator<CellRange>::type                      CellIterator;

  
  //
  // First example: Iterate over vertices and store data on them: 
  //
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {
    //
    // Using a key of type std::string and value "my_data", we store data of type 'double' on the vertex:
    //
    viennadata::access<std::string, double>("my_data")(*vit) = vit->point()[0];

    //
    // Using a type-based key dispatch with the class 'my_key' defined at the beginning, we do the same:
    //
    viennadata::access<my_key, double>()(*vit) = vit->point()[0];
    
    //
    // We store a vector quantity using PointType for that purpose. std::vector<double> can also be used.
    // As key we use the long '42'. 
    //
    viennadata::access<long, PointType>(42)(*vit) = vit->point();
    
  }


  //
  // Second example: Iterate over
  //
  CellRange cells = viennagrid::ncells(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                   ++cit)
  {
    //
    // Store the x-component of the centroid of the cell on the cell.
    // A std::string "my_data" is used as key to access the quantity.
    //
    viennadata::access<std::string, double>("my_data")(*cit) = viennagrid::centroid(*cit)[0];
    
    //
    // Store the centroid on the cell. Note that the same key is used as before,
    // but the value type has changed from 'double' to 'PointType'. 
    // The x-component stored before is therefore not overwritten.
    //
    viennadata::access<std::string, PointType>("my_data")(*cit) = viennagrid::centroid(*cit);

    //
    // Store the centroid on the cell again. Do not compute it again, but rather get the previously stored data:
    //
    viennadata::access<my_key, PointType>()(*cit) = viennadata::access<std::string, PointType>("my_data")(*cit);
  }

  //
  // In the same way data is stored on edges, faces etc.
  //

  //
  // Write the data to VTK:
  //
  viennagrid::io::vtk_writer<DomainType> my_vtk_writer;
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_vtk_writer, "my_data", "my_data");
  viennagrid::io::add_scalar_data_on_vertices<my_key,      double>(my_vtk_writer, my_key(), "my_key");
  viennagrid::io::add_vector_data_on_vertices<long,     PointType>(my_vtk_writer, 42, "42");
  
  viennagrid::io::add_scalar_data_on_cells<std::string,    double>(my_vtk_writer, "my_data", "my_data");
  viennagrid::io::add_vector_data_on_cells<std::string, PointType>(my_vtk_writer, "my_data", "my_data");
  viennagrid::io::add_vector_data_on_cells<my_key,      PointType>(my_vtk_writer, my_key(), "my_key");
  
  my_vtk_writer(domain, outfile + ".vtu");
   
}


int main()
{
  typedef viennagrid::result_of::domain<viennagrid::config::triangular_2d>::type        Domain2d;
  typedef viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type       Domain3d;
  
  std::cout << "----------------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Storing and accessing data on n-cells --" << std::endl;
  std::cout << "----------------------------------------------------------------" << std::endl;
  std::cout << std::endl;
  
  std::string path = "../examples/data/";
  
  //Stage 1: Read from Netgen files, write to VTK
  viennagrid::io::netgen_reader my_netgen_reader;
  
  //
  // Run tutorial for a 2D domain:
  //  
  /*
  Domain2d domain_2d;
  
  try
  {
    my_netgen_reader(domain_2d, path + "square32.mesh");
  }
  catch (std::exception const & ex)
  {
     std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }
  exec_for_domain(domain_2d, "stored_data_2d");
  std::cout << "Two-dimensional domain attached with data and written to stored_data_2d_main.pvd" << std::endl;
  std::cout << std::endl;*/
  
  
  //
  // Run tutorial for a 3D domain:
  //  
  Domain3d domain_3d;
  
  try
  {
    my_netgen_reader(domain_3d, path + "cube48.mesh");
  }
  catch (std::exception const & ex)
  {
     std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }
  exec_for_domain(domain_3d, "stored_data_3d");
  std::cout << "Three-dimensional domain attached with data and written to stored_data_3d_main.pvd" << std::endl;
  std::cout << std::endl;
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  return EXIT_SUCCESS;
}
