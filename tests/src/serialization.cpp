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

  Domain domain;
  viennagrid::io::netgen_reader my_reader;
  my_reader(domain, "../../examples/data/square32.mesh");
  
  // define a serialization object and pass the domain to the constructor 
  //
  viennagrid::io::domain_serializer<Domain>  domainserial(domain);
  
  // the domainserial object is now a Boost Serialization conform serializable
  // object, for example, one can ..
  
  // .. use boost archive to dump it to a text file
  {
  std::ofstream ofs("domainfile.txt");
  boost::archive::text_oarchive oa(ofs);
  oa << domainserial;
  }

  // .. use boost archive to dump it to std::cout
  {
  boost::archive::text_oarchive oa(std::cout);
  oa << domainserial;
  }

  // .. trasmit it via boost::mpi's send
  // note: this example is not active, as it would require linking 
  // against the (boost)mpi libraries and the execution via mpirun ...
  /*
  if (world.rank() == 0) world.send(1, DOMAIN_TAG, domainserial);
  else                   world.recv(0, DOMAIN_TAG, domainserial);     
  */
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}

