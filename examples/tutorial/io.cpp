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

#ifdef _MSC_VER      //Visual Studio complains about the use of 'this' in member initialization, which is perfectly legal in our context
  #pragma warning( disable : 4355 )
#endif


#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/opendx_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"


//
// Shows how to read a domain from Netgen mesh files
//
template <typename DomainType>
void read_netgen(DomainType & domain)
{
  
  viennagrid::io::netgen_reader my_netgen_reader;
  #ifdef _MSC_VER      //Visual Studio builds in a subfolder
  std::string path = "../../examples/data/";
  #else
  std::string path = "../examples/data/";
  #endif
  std::string filename = path + "cube48.mesh";
  my_netgen_reader(domain, filename);
  
  //
  // Note that the Netgen format supports multiple segments, which will be automatically created by the reader
  //
  std::cout << "Number of segments in Netgen file: " << domain.segments().size() << std::endl;
  
  // do more stuff with the domain here.
}


//
// Write to IBM Vizualization DataExplorer (OpenDX, http://www.opendx.org/) file
//
template <typename DomainType>
void write_opendx(DomainType const & domain)
{
  // Instantiate writer object:
  viennagrid::io::opendx_writer<DomainType> my_dx_writer;
  
  // Add scalar vertex data: Note that only the first data is used.
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_dx_writer, "vtk_data", "data_double");
  
  // Add scalar cell data: Note that only the first data is used and that no other vertex data must be present!
  //viennagrid::io::add_scalar_data_on_cells<std::string, double>(my_dx_writer, "vtk_data", "cell_data_double");
  
  
  my_dx_writer(domain, "tutorial_io.out");
}


//
// Read the mesh from the VTK File format (XML)
//
template <typename DomainType>
void read_vtk(DomainType & domain)
{
  //
  // Step 1: Instantiate reader object
  //
  viennagrid::io::vtk_reader<DomainType>  reader;
  
  //-----------------------------------------------------
  
  //
  // Step 2: Specify where to write the vertex data. 
  //         By default, data is written as type double or std::vector<double> to ViennaData using a std::string that matches the name in the VTK file.
  //
  
  // Write scalar vertex data that matches the name 'data_double' to ViennaData as data of type double, using a key of type std::string and value "vtk_data":
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(reader, "vtk_data", "data_double");
  
  // Write vector-valued vertex data that matches the name 'data_point' to ViennaData as data of type std::vector<double>, using a key of type std::string and value "vtk_data":
  viennagrid::io::add_vector_data_on_vertices<std::string, std::vector<double> >(reader, "vtk_data", "data_point");
  
  // Write normal (vector-valued) vertex data that matches the name 'data_normal' to ViennaData as data of type std::vector<double>, using a key of type std::string and value "vtk_data_2":
  viennagrid::io::add_normal_data_on_vertices<std::string, std::vector<double> >(reader, "vtk_data_2", "data_normal");

  //-----------------------------------------------------
  
  //
  // Step 3: Specify where to write the cell data. 
  //         By default, data is written as type double or std::vector<double> to ViennaData using a std::string that matches the name in the VTK file.
  //

  // Write scalar cell data that matches the name 'data_double' to ViennaData as data of type double, using a key of type std::string and value "vtk_data":
  viennagrid::io::add_scalar_data_on_cells<std::string, double>(reader, "vtk_data", "data_double");
  
  // Write vector-valued cell data that matches the name 'data_point' to ViennaData as data of type std::vector<double>, using a key of type std::string and value "vtk_data":
  viennagrid::io::add_vector_data_on_cells<std::string, std::vector<double> >(reader, "vtk_data", "data_point");
  
  // Write normal (vector-valued) cell data that matches the name 'data_normal' to ViennaData as data of type std::vector<double>, using a key of type std::string and value "vtk_data_2":
  viennagrid::io::add_normal_data_on_cells<std::string, std::vector<double> >(reader, "vtk_data_2", "data_normal");

  
  //-----------------------------------------------------
  
  //
  // Step 4: Trigger filereader:
  //
  #ifdef _MSC_VER      //Visual Studio builds in a subfolder
  std::string path = "../../examples/data/";
  #else
  std::string path = "../examples/data/";
  #endif
  reader(domain, path + "tets_with_data_main.pvd");
  
  
  //
  // Step 5: Get and print all names read from vtk file(s):
  //
  
  std::cout << "--- Data read from VTK file: ---" << std::endl;
  std::cout << "* Scalar data on vertices: " << std::endl;
  for (size_t i=0; i<reader.get_scalar_data_on_vertices().size(); ++i)
    std::cout << "Segment " << reader.get_scalar_data_on_vertices()[i].first << ": "
              << reader.get_scalar_data_on_vertices()[i].second << std::endl;
              
  std::cout << "* Vector data on vertices: " << std::endl;
  for (size_t i=0; i<reader.get_vector_data_on_vertices().size(); ++i)
    std::cout << "Segment " << reader.get_vector_data_on_vertices()[i].first << ": " 
              << reader.get_vector_data_on_vertices()[i].second << std::endl;
              
  std::cout << "* Scalar data on cells: " << std::endl;
  for (size_t i=0; i<reader.get_scalar_data_on_cells().size(); ++i)
    std::cout << "Segment " << reader.get_scalar_data_on_cells()[i].first << ": " 
              << reader.get_scalar_data_on_cells()[i].second << std::endl;
              
  std::cout << "* Vector data on cells: " << std::endl;
  for (size_t i=0; i<reader.get_vector_data_on_cells().size(); ++i)
    std::cout << "Segment " << reader.get_vector_data_on_cells()[i].first << ": "
              << reader.get_vector_data_on_cells()[i].second << std::endl;
}




//
// Write the mesh to VTK File format (XML)
//
template <typename DomainType>
void write_vtk(DomainType & domain)
{
  //
  // Step 1: Instantiate a writer object:
  //
  viennagrid::io::vtk_writer<DomainType> my_vtk_writer;
  
  //-----------------------------------------------------
  
  //
  // Step 2: Add vertex data to the writer. In the following, keys of type std::string and value "vtk_data" are used as the ViennaData accessor.
  //         The third function argument is the name of the quantity as it will appear in the VTK file.
  //
  
  // Write data of type data that is stored with ViennaData for keys of type std::string with value "vtk_data" and call it "data_double" in the VTK file:
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_vtk_writer, "vtk_data", "data_double");

  // Same as above, but with data of type long, which is then called "data_long" in the VTK file:
  viennagrid::io::add_scalar_data_on_vertices<std::string, long>(my_vtk_writer, "vtk_data", "data_long");
  
  // Vector-valued data of type std::vector<double> is written to the VTK file and named "data_vector". Data is accessed with ViennaData using a std::string "vtk_data" as key.
  viennagrid::io::add_vector_data_on_vertices<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_point");

  // Same as vector-data, but with the constraint that each vector has length 1
  viennagrid::io::add_normal_data_on_vertices<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_normal");
  
  //-----------------------------------------------------
  
  //
  // Step 3: Add cell data to the writer. Works in the same way as vertex data
  //
  
  // Write data of type double, using the std::string key "vtk_data". Name in VTK file is "data_double" (there is no name collision with vertex data here).
  viennagrid::io::add_scalar_data_on_cells<std::string, double>(my_vtk_writer, "vtk_data", "data_double");
  
  // Vector valued data on cells. Similar to vertex case
  viennagrid::io::add_vector_data_on_cells<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_vector");

  // Vector valued data on cells. Just like vector data, but with a normalization requirement to length 1.
  viennagrid::io::add_normal_data_on_cells<std::string, std::vector<double> >(my_vtk_writer, "vtk_data", "data_normal");
  
  //-----------------------------------------------------
  
  //
  // Final Step: Trigger the write process:
  //
  my_vtk_writer(domain, "tutorial_io.vtu");
   
}


template <typename DomainType>
void write_data(DomainType & domain)
{
  typedef typename DomainType::config_type         ConfigType;
  typedef typename ConfigType::cell_tag            CellTag;
  
  typedef typename viennagrid::result_of::ncell_range<DomainType, 0>::type           VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;

  typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::dim>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;
  

  //write x-component to each vertex
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {
    viennadata::access<std::string, double>("vtk_data")(*vit) = vit->point()[0];
  }
  
  //write x-component of circumcenter to each cell
  CellContainer cells = viennagrid::ncells(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    viennadata::access<std::string, double>("vtk_data")(*cit) = viennagrid::circumcenter(*cit)[0];
  }
}

int main()
{
  typedef viennagrid::config::tetrahedral_3d     ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type        Domain;
  typedef viennagrid::result_of::segment<ConfigType>::type       SegmentType;

  std::cout << "----------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: IO operations --" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
  std::cout << std::endl;
  
  Domain domain;

  //
  // Use-case 1: Read from Netgen mesh files
  //
  read_netgen(domain);
  
  //write a bit of data to domain:
  write_data(domain);
  
  //
  // Use-case 2: Write to OpenDX
  //
  write_opendx(domain);
  
  
  //
  // Use-case 3: Read VTK file(s)
  //
  Domain vtk_domain;
  read_vtk(vtk_domain);
  
  
  //
  // Use-case 4: Write VTK file(s)
  //
  write_vtk(vtk_domain);

  std::cout << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}