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

#include <stdlib.h>

//***********************************************
// Define the dimension
//***********************************************
//#define THREEDIM
#define TWODIM

//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/point.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"


template <viennagrid::dim_type dim>
void testPointsAdvanced()
{
  typedef viennagrid::point<double, dim, viennagrid::cartesian_cs>   PointType;
  
  PointType p0(0, 0, 0);
  PointType p1(1, 0, 0);
  PointType p2(0, 1, 0);
  PointType p3(0, 0, 1);
  
}



template <viennagrid::dim_type dim>
void testPoints()
{
  typedef viennagrid::point<double, dim, viennagrid::cartesian_cs>   PointType;
  
  PointType p0(0, 0, 0);
  PointType p1(1, 0, 0);
  PointType p2(0, 1, 0);
  PointType p3(0, 0, 1);
  
  std::cout << "Printing points: " << std::endl;
  std::cout << "p0: " << p0;
  std::cout << "p1: " << p1;
  std::cout << "p2: " << p2;
  std::cout << "p3: " << p3;

  std::cout << std::endl;
  std::cout << "Basic operations: " << std::endl;
  std::cout << "p0 + p1 = " << p0 + p1;
  std::cout << "p0 - p2 = " << p0 - p2;
  std::cout << "p0 + p3 * 4 = " << p0 + p1 * 4.0;
  std::cout << std::endl;
  std::cout << "<p1,p0> = " << viennagrid::inner_prod(p1, p0) << std::endl;
  std::cout << "<p1,p1> = " << viennagrid::inner_prod(p1, p1) << std::endl;
  std::cout << "<p1,p2> = " << viennagrid::inner_prod(p1, p2) << std::endl;
  std::cout << "<p1,p3> = " << viennagrid::inner_prod(p1, p3) << std::endl;
  std::cout << "<p2,p2> = " << viennagrid::inner_prod(p2, p2) << std::endl;
  std::cout << std::endl;
  std::cout << "p1 x p2 = " << viennagrid::cross_prod(p1, p2) << std::endl;
  std::cout << "p2 x p1 = " << viennagrid::cross_prod(p2, p1) << std::endl;
  std::cout << "p2 x p3 = " << viennagrid::cross_prod(p2, p3) << std::endl;
  std::cout << "p3 x p2 = " << viennagrid::cross_prod(p3, p2) << std::endl;
  std::cout << std::endl;
  std::cout << "||p0|| = " << viennagrid::norm(p0) << std::endl;
  std::cout << "||p1|| = " << viennagrid::norm(p1) << std::endl;
  std::cout << "||p2|| = " << viennagrid::norm(p2) << std::endl;
  std::cout << "||p3|| = " << viennagrid::norm(p3) << std::endl;
  std::cout << "||p1 - p0|| = " << viennagrid::norm(p1 - p0) << std::endl;
  std::cout << "||p2 - p0|| = " << viennagrid::norm(p2 - p0) << std::endl;
  std::cout << "||p3 - p0|| = " << viennagrid::norm(p3 - p0) << std::endl;
  
  
  std::cout << "Spanned Volume: " << std::endl;
  std::cout << "vol(p1, p2) = " << viennagrid::spanned_volume(p1, p2) << std::endl;
  std::cout << "vol(p2, p1) = " << viennagrid::spanned_volume(p2, p1) << std::endl;
  std::cout << "vol(p0, p1, p2) = " << viennagrid::spanned_volume(p0, p1, p2) << std::endl;
  std::cout << "vol(p1, p2, p3) = " << viennagrid::spanned_volume(p1, p2, p3) << std::endl;
  
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  //std::cout << "Testing one dimension:" << std::endl;
  //testPoints<1>();
  std::cout << std::endl;
  std::cout << " --- Testing two dimensions: --- " << std::endl;
  testPoints<2>();
  std::cout << std::endl;
  std::cout << " --- Testing three dimensions: --- " << std::endl;
  testPoints<3>();
  
  std::cout << std::endl;
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
