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
#include "viennagrid/config/others.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/serialization.hpp"

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  // setup a domain
  // 
  typedef viennagrid::config::triangular_2d                          Config;  
  typedef viennagrid::result_of::domain<Config>::type                Domain;  
  typedef viennagrid::io::domain_serializer<Domain>                  DomainSerializer;
  typedef boost::shared_ptr<Domain>                                  DomainSP;

  // setup a shared pointer on a domain
  //
  DomainSP          domainsp(new Domain);
  
  // create a serializer object and load it with the domain pointer
  //
  DomainSerializer  domain_serial;
  domain_serial.load( domainsp );

  // access the domain object by calling the functor
  //
  viennagrid::io::netgen_reader my_reader;
  my_reader(domain_serial(), "../../examples/data/square32.mesh");
  
  // the domainserial object is now a Boost Serialization conform serializable
  // object, for example, one can ..
  
  // .. use boost archive to dump it to a text file
  {
  std::ofstream ofs("domainfile.txt");
  boost::archive::text_oarchive oa(ofs);
  oa << domain_serial;
  }

  // .. use boost archive to dump it to std::cout
  {
  boost::archive::text_oarchive oa(std::cout);
  oa << domain_serial;
  }

  // .. trasmit it via boost::mpi's send
  // note: this example is not active, as it would require linking 
  // against the (boost)mpi libraries and the execution via mpirun ...
  /*
  if (world.rank() == 0) world.send(1, DOMAIN_TAG, domain_serial);
  else                   world.recv(0, DOMAIN_TAG, domain_serial);     
  */
  
  // the actual viennagrid domain object can be accessed via the functor
  viennagrid::io::vtk_writer<Domain>  vtk;         
  vtk(domain_serial(), "serialized_domain");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}

