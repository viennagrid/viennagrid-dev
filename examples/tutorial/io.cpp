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

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
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
  std::string filename = "../examples/data/cube48.mesh";
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
  viennagrid::io::opendx_writer my_dx_writer;
  my_dx_writer(domain, "tutorial_io.odx");
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
  std::string filename = "../examples/data/multi_segment_tet_main.pvd";
  reader(domain, filename);
  
  
  //
  // Step 5: Get and print all names read:
  //
  //TODO: Output all data names read from file
}




//
// Write the mesh to VTK File format (XML)
//
template <typename DomainType>
void write_vtk(DomainType const & domain)
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


int main()
{
  typedef viennagrid::config::tetrahedral_3d     ConfigType;
  typedef viennagrid::result_of::domain<ConfigType>::type        Domain;
  typedef viennagrid::result_of::segment<ConfigType>::type       SegmentType;

  std::cout << " ---------------------------------------- " << std::endl;
  std::cout << " -- ViennaGrid tutorial: IO operations -- " << std::endl;
  std::cout << " ---------------------------------------- " << std::endl;
  std::cout << std::endl;
  
  Domain domain;

  //
  // Use-case 1: Read from Netgen mesh files
  //
  read_netgen(domain);
  
  
  //
  // Use-case 2: Write to OpenDX
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
