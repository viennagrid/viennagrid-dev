/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Josef Weinbub                    weinbub@iue.tuwien.ac.at
               Karl Rupp                           rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
   
   This is a test file for the ViennaGrid Library IO-System
======================================================================= */

#include "viennagrid/forwards.h"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/tdr_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

int main(int argc, char *argv[])
{
   std::string filename = argv[1];

   typedef viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type    domain_type;
   domain_type domain;

   viennagrid::io::tdr_reader    reader(filename);
   reader(domain);

   viennagrid::io::export_vtk(domain, "output");

   return 0;
}
