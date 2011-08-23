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

#include <stdlib.h>


//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/point.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"

//compare a and b, taking round-off into account
bool fuzzy_equal(double a, double b)
{
  //check for equality:
  if (a == b)
    return true;
  
  //relative check:
  if ( std::fabs(a - b) / std::max(std::fabs(a), std::fabs(b)) < 1e-10)
    return true;
  
  //absolute check: (near 0)
  if (std::fabs(a) < 1e-10 && std::fabs(b) < 1e-10)
    return true;
  
  std::cerr << "[FAILED]" << std::endl;
  std::cerr << "Error in fuzzy_equal: a = " << a << ", b = " << b << std::endl;
  return false;
}

template <typename PointType>
bool fuzzy_equal(PointType const & a, PointType const & b)
{
  for (std::size_t i=0; i<a.size(); ++i)
  {
    if (!fuzzy_equal(a[i], b[i]))
      return false;
  }
  
  return true;
}

template <typename PointType, typename PointType2>
bool fuzzy_equal(PointType const & a, PointType2 const & b)
{
  bool ret = fuzzy_equal(a, viennagrid::coordinate_converter<PointType2, PointType>()(b));
  if (!ret)
    std::cerr << "Vectors: " << a << "; " << b << std::endl;
  return ret;
}




void test_1d()
{
  typedef viennagrid::point<double, 1, viennagrid::cartesian_cs>   CartesianPoint;
  
  CartesianPoint c0(0);
  CartesianPoint c1(0.5);
  CartesianPoint c2(1);
  
  //Print points
  std::cout << c0 << std::endl;
  std::cout << c1 << std::endl;
  std::cout << c2 << std::endl;
  
  //Basic operations:
  std::cout << "Basic operations: ";
  if ( !fuzzy_equal((c0 + c1)[0], 0.5) || !fuzzy_equal((c0 + c2)[0], 1.0) || !fuzzy_equal((c1 + c2)[0], 1.5))
    exit(EXIT_FAILURE);
  if ( !fuzzy_equal((c0 - c1)[0], -0.5) || !fuzzy_equal((c0 - c2)[0], -1.0) || !fuzzy_equal((c1 - c2)[0], -0.5))
    exit(EXIT_FAILURE);
  if ( !fuzzy_equal((2.0f*c0 + 3.0*c1)[0], 1.5) || !fuzzy_equal((2.0*c0 - 3.0f*c2)[0], -3.0) || !fuzzy_equal((2.0f * c1 - 3.0f * c2)[0], -2.0))
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;
  
  
  std::cout << "Inner product: ";
  if ( !fuzzy_equal(viennagrid::inner_prod(c0, c1), 0.0)
       || !fuzzy_equal(viennagrid::inner_prod(c0, c2), 0.0)
       || !fuzzy_equal(viennagrid::inner_prod(c1, c2), 0.5) )
    exit(EXIT_FAILURE);
  if ( !fuzzy_equal(viennagrid::inner_prod(2.0*c0, c1), 0.0)
       || !fuzzy_equal(viennagrid::inner_prod(c0, 2.0*c2), 0.0)
       || !fuzzy_equal(viennagrid::inner_prod(c0 + c1, c0 + c2), 0.5) )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;
  
  std::cout << "Norm: ";
  if ( !fuzzy_equal(viennagrid::norm(c0), 0.0)
       || !fuzzy_equal(viennagrid::norm(c1), 0.5)
       || !fuzzy_equal(viennagrid::norm(c2), 1.0) )
    exit(EXIT_FAILURE);
  if ( !fuzzy_equal(viennagrid::norm(2.0*c0), 0.0)
       || !fuzzy_equal(viennagrid::norm(2.0*c2), 2.0)
       || !fuzzy_equal(viennagrid::norm(c1 + c2), 1.5) )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;
}

void test_2d()
{
  typedef viennagrid::point<double, 2, viennagrid::cartesian_cs>   CartesianPoint;
  typedef viennagrid::point<double, 2, viennagrid::polar_cs>       PolarPoint;
  
  CartesianPoint c0(0, 0);
  CartesianPoint c1(1, 0);
  CartesianPoint c2(0, 1);
  CartesianPoint c3(1, 1);

  PolarPoint p0 = c0;
  PolarPoint p1;
  p1 = c1;
  PolarPoint p2(c2);
  PolarPoint p3(1, 1);
  p3 = c3;
  
  //convert to cartesian:
  CartesianPoint c4 = p0;
  CartesianPoint c5;
  c5 = p1;
  CartesianPoint c6(p2);
  CartesianPoint c7(1, 1);
  c7 = p3;
  
  std::cout << "Printing points: " << std::endl;
  std::cout << "c0: " << c0 << std::endl;
  std::cout << "c1: " << c1 << std::endl;
  std::cout << "c2: " << c2 << std::endl;
  std::cout << "c3: " << c3 << std::endl;
  std::cout << "c4: " << c4 << std::endl;
  std::cout << "c5: " << c5 << std::endl;
  std::cout << "c6: " << c6 << std::endl;
  std::cout << "c7: " << c7 << std::endl;
  std::cout << "p0: " << p0 << std::endl;
  std::cout << "p1: " << p1 << std::endl;
  std::cout << "p2: " << p2 << std::endl;
  std::cout << "p3: " << p3 << std::endl;

  if ( !fuzzy_equal(c0, c4)
       || !fuzzy_equal(c1, c5)
       || !fuzzy_equal(c2, c6)
       || !fuzzy_equal(c3, c7) )
    exit(EXIT_FAILURE);

  if ( !fuzzy_equal(p0, c4)
       || !fuzzy_equal(p1, c5)
       || !fuzzy_equal(p2, c6)
       || !fuzzy_equal(p3, c7) )
    exit(EXIT_FAILURE);

  if ( !fuzzy_equal(c0, p0)
       || !fuzzy_equal(c1, p1)
       || !fuzzy_equal(c2, p2)
       || !fuzzy_equal(c3, p3) )
    exit(EXIT_FAILURE);
  
  std::cout << std::endl;
  std::cout << "Basic operations: " << std::endl;
  if ( !fuzzy_equal(c0+c1, p0+p1)
       || !fuzzy_equal(c1+c2, p1+p2)
       || !fuzzy_equal(c2-c1, p2-p1)
       || !fuzzy_equal(c3 + 2.0 * c1, p3 + 2.0 * p1) )
    exit(EXIT_FAILURE);
  
  if ( !fuzzy_equal(p0+p1, c0+c1)
       || !fuzzy_equal(p1+p2, c1+c2)
       || !fuzzy_equal(p2-p1, c2-c1)
       || !fuzzy_equal(p3 + 2.0 * p1, c3 + 2.0 * c1) )
    exit(EXIT_FAILURE);
  if ( !fuzzy_equal(p0+c1, p0+c1)
       || !fuzzy_equal(p1+c2, p1+c2)
       || !fuzzy_equal(p2-c1, p2-c1)
       || !fuzzy_equal(p3 + 2.0 * c1, p3 + 2.0 * c1) )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;
  
  std::cout << "Testing inner products: " << std::endl;  
  std::cout << c1 << " vs " << p1 << std::endl;
  std::cout << viennagrid::to_cartesian(c3) << " vs " << viennagrid::to_cartesian(p3) << std::endl;
  if ( !fuzzy_equal(viennagrid::inner_prod(c1, c0), viennagrid::inner_prod(p1, p0))
       || !fuzzy_equal(viennagrid::inner_prod(c2, c1), viennagrid::inner_prod(p2, p1))
       || !fuzzy_equal(viennagrid::inner_prod(c3, c0), viennagrid::inner_prod(p3, p0))
       || !fuzzy_equal(viennagrid::inner_prod(c1, c3), viennagrid::inner_prod(p1, p3)) 
     )
    exit(EXIT_FAILURE);
  std::cout << "Passed first part" << std::endl;
  if ( !fuzzy_equal(viennagrid::inner_prod(c1, p0), viennagrid::inner_prod(c1, p0))
       || !fuzzy_equal(viennagrid::inner_prod(c2, p1), viennagrid::inner_prod(c2, p1))
       || !fuzzy_equal(viennagrid::inner_prod(c3, p0), viennagrid::inner_prod(c3, p0))
       || !fuzzy_equal(viennagrid::inner_prod(c1, p3), viennagrid::inner_prod(c1, p3)) )
    exit(EXIT_FAILURE);
  
  std::cout << "[PASSED]" << std::endl;
  
  
  
  std::cout << std::endl;
  std::cout << "c1 x c2 = " << viennagrid::cross_prod(c1, c2) << std::endl;
  std::cout << "c2 x c1 = " << viennagrid::cross_prod(c2, c1) << std::endl;
  std::cout << "c2 x c3 = " << viennagrid::cross_prod(c2, c3) << std::endl;
  std::cout << "c3 x c2 = " << viennagrid::cross_prod(c3, c2) << std::endl;
  std::cout << std::endl;
  std::cout << "||c0|| = " << viennagrid::norm(c0) << std::endl;
  std::cout << "||c1|| = " << viennagrid::norm(c1) << std::endl;
  std::cout << "||c2|| = " << viennagrid::norm(c2) << std::endl;
  std::cout << "||c3|| = " << viennagrid::norm(c3) << std::endl;
  std::cout << "||c1 - c0|| = " << viennagrid::norm(c1 - c0) << std::endl;
  std::cout << "||c2 - c0|| = " << viennagrid::norm(c2 - c0) << std::endl;
  std::cout << "||c3 - c0|| = " << viennagrid::norm(c3 - c0) << std::endl;
  
  
  std::cout << "Spanned Volume: " << std::endl;
  std::cout << "vol(c1, c2) = " << viennagrid::spanned_volume(c1, c2) << std::endl;
  std::cout << "vol(c2, c1) = " << viennagrid::spanned_volume(c2, c1) << std::endl;
  std::cout << "vol(c0, c1, c2) = " << viennagrid::spanned_volume(c0, c1, c2) << std::endl;
  std::cout << "vol(c1, c2, c3) = " << viennagrid::spanned_volume(c1, c2, c3) << std::endl;
  
}


void test_3d()
{
  typedef viennagrid::point<double, 3, viennagrid::cartesian_cs>    CartesianPoint;
  typedef viennagrid::point<double, 3, viennagrid::spherical_cs>    SphericalPoint;
  typedef viennagrid::point<double, 3, viennagrid::cylindrical_cs>  CylindricalPoint;
  
  
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::cout << std::endl;
  std::cout << " --- Testing one dimension: --- " << std::endl;
  test_1d();
  std::cout << std::endl;
  std::cout << " --- Testing two dimensions: --- " << std::endl;
  test_2d();
  std::cout << std::endl;
  std::cout << " --- Testing three dimensions: --- " << std::endl;
  test_3d();
  std::cout << std::endl;
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
