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

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/serialization.hpp"

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  // setup a mesh
  // 
  typedef viennagrid::triangular_2d_mesh                           MeshType;
  typedef viennagrid::io::mesh_serializer<MeshType>              MeshSerializer;
//   typedef boost::shared_ptr<MeshType>                              MeshSP;

  // setup a shared pointer on a mesh
  //
  MeshType          mesh;
  
  // create a serializer object and load it with the mesh pointer
  //
  MeshSerializer  mesh_serial;
  mesh_serial.load( mesh );

  // access the mesh object by calling the functor
  //
  viennagrid::io::netgen_reader my_reader;
  my_reader(mesh_serial(), "../../examples/data/square32.mesh");
  
  // the meshserial object is now a Boost Serialization conform serializable
  // object, for example, one can ..
  
  // .. use boost archive to dump it to a text file
  {
  std::ofstream ofs("meshfile.txt");
  boost::archive::text_oarchive oa(ofs);
  oa << mesh_serial;
  }

  // .. use boost archive to dump it to std::cout
  {
  boost::archive::text_oarchive oa(std::cout);
  oa << mesh_serial;
  }

  // .. transmit it via boost::mpi's send
  // note: this example is not active, as it would require linking 
  // against the (boost)mpi libraries and the execution via mpirun ...
  /*
  if (world.rank() == 0) world.send(1, MESH_TAG, mesh_serial);
  else                   world.recv(0, MESH_TAG, mesh_serial);     
  */
  
  // the actual viennagrid mesh object can be accessed via the functor interface
  viennagrid::io::vtk_writer<MeshType>  vtk;         
  vtk(mesh_serial(), //extract the mesh from the serializer
      "serialized_mesh");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}

