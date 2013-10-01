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
  #pragma warning( disable : 4355 )     //use of this in member initializer list
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/mesh.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"


struct my_key {};

//
// Configure ViennaData to use the n-cell IDs for storing the data:
// (Mind the VIENNAGRID prefix here!)
//
VIENNAGRID_ENABLE_ALL_NCELL_ID_FOR_DATA(viennagrid::config::triangular_2d)
VIENNAGRID_GLOBAL_ENABLE_NCELL_ID_FOR_DATA(viennagrid::point_tag)
VIENNAGRID_ENABLE_NCELL_ID_FOR_DATA(viennagrid::config::triangular_2d, viennagrid::point_tag)   //resolve template ambiguity from the two cases above

// for demonstration purposes, enable the use of ncell IDs globally for all configuration an n-cell types:
VIENNAGRID_GLOBAL_ENABLE_ALL_NCELL_ID_FOR_DATA()
 

//
// Now as the use of IDs is enabled, enable dense storage for keys of type my_key and double data:
//
VIENNADATA_ENABLE_DENSE_DATA_STORAGE_FOR_KEY_DATA(my_key, double)



//
// Enable type-based dispatch for my_key:
//
VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(my_key)




template <typename MeshType>
void exec_for_mesh(MeshType & mesh,
                     std::string const & infile,
                     std::string const & outfile)
{
  //
  // Get all the types needed later on:
  //
  typedef typename MeshType::config_type               ConfigType;
  typedef typename ConfigType::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::point<ConfigType>::type                 PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type              VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type   CellType;

  typedef typename viennagrid::result_of::const_ncell_range<MeshType, 0>::type         VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type          VertexIterator;
      
  typedef typename viennagrid::result_of::const_ncell_range<MeshType, CellTag::dim>::type    CellRange;
  typedef typename viennagrid::result_of::iterator<CellRange>::type                      CellIterator;

  //
  // First, read the mesh from a Netgen file
  //
  try
  {
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(mesh, infile);
  }
  catch (std::exception const & ex)
  {
     std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }
  
  //
  // First example: Iterate over vertices and store data on them: 
  //
  VertexContainer vertices = viennagrid::ncells<0>(mesh);
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
  // Second example: Iterate over cells and store some data:
  //
  CellRange cells = viennagrid::ncells(mesh);
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
  // Write the data to VTK file (can be visualized with e.g. Paraview)
  // More information on the VTK writer can be found in examples/tutorial/io.cpp
  //
  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  
  // Vertex data
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_vtk_writer, "my_data", "my_data");
  viennagrid::io::add_scalar_data_on_vertices<my_key,      double>(my_vtk_writer, my_key(), "my_key");
  viennagrid::io::add_vector_data_on_vertices<long,     PointType>(my_vtk_writer, 42, "42"); //Note that z-component is automatically padded with zeros for the 2d case.
  
  // Cell data (note that Paraview crashes if the same name for vertex and scalar data is used)
  // Note that z-component of vector-valued data is automatically padded with zeros for the 2d case.
  viennagrid::io::add_scalar_data_on_cells<std::string,    double>(my_vtk_writer, "my_data", "my_scalar_data");
  viennagrid::io::add_vector_data_on_cells<std::string, PointType>(my_vtk_writer, "my_data", "my_vector_data");
  viennagrid::io::add_vector_data_on_cells<my_key,      PointType>(my_vtk_writer, my_key(), "my_key");
  
  my_vtk_writer(mesh, outfile);
   
}


int main()
{
  typedef viennagrid::result_of::mesh<viennagrid::config::triangular_2d>::type        Mesh2d;
  typedef viennagrid::result_of::mesh<viennagrid::config::tetrahedral_3d>::type       Mesh3d;
  
  std::cout << "----------------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Storing and accessing data on n-cells --" << std::endl;
  std::cout << "----------------------------------------------------------------" << std::endl;
  std::cout << std::endl;
  
  std::string path = "../examples/data/";
  
  // ---------------------------------------------------------------
  
  //
  // Run tutorial for a 2D mesh:
  //  
  Mesh2d mesh_2d;
  
  // store some data on the mesh and write the result to VTK files:
  exec_for_mesh(mesh_2d, path + "square32.mesh", "stored_data_2d");
  
  std::cout << "Two-dimensional mesh attached with data and written to stored_data_2d_main.pvd" << std::endl;
  std::cout << std::endl;
  
  // ---------------------------------------------------------------
  
  //
  // Run tutorial for a 3D mesh. Note the use of the same code as for the 2d case!
  //  
  Mesh3d mesh_3d;
  
  // store some data on the mesh and write the result to VTK files:
  exec_for_mesh(mesh_3d, path + "cube48.mesh", "stored_data_3d");
  
  std::cout << "Three-dimensional mesh attached with data and written to stored_data_3d_main.pvd" << std::endl;
  std::cout << std::endl;

  // ---------------------------------------------------------------
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  return EXIT_SUCCESS;
}
