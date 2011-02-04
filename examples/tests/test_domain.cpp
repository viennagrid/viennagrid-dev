/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
   
   This is a test file for the ViennaGrid Library IO-System
======================================================================= */

#include "viennagrid/domain.hpp"


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  typedef viennagrid::domain<viennagrid::config::u22>       domain_22u_type;
  typedef viennagrid::domain<viennagrid::config::u32>       domain_32u_type;
  typedef viennagrid::domain<viennagrid::config::u33>       domain_33u_type;

  std::cout << "* instantiating 22u domain.. " << std::endl;
  domain_22u_type domain_22u;
  std::cout << "* instantiating 32u domain.. " << std::endl;
  domain_32u_type domain_32u;
  std::cout << "* instantiating 33u domain.. " << std::endl;
  domain_33u_type domain_33u;

  return EXIT_SUCCESS;
}
