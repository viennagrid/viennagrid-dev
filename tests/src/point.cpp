/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include <stdlib.h>


//***********************************************
// Define the input-file format
//***********************************************

#include "viennagridpp/core.hpp"
#include "viennagridpp/algorithm/spanned_volume.hpp"
#include "viennagridpp/algorithm/inner_prod.hpp"
#include "viennagridpp/algorithm/cross_prod.hpp"

//compare a and b, taking round-off into account
inline bool fuzzy_equal(double a, double b)
{
  //check for equality:
  if (a <= b && a >= b)
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

bool fuzzy_equal(viennagrid::point const & a, viennagrid::point const & b)
{
  for (std::size_t i=0; i<a.size(); ++i)
  {
    if (!fuzzy_equal(a[i], b[i]))
      return false;
  }

  return true;
}

// template <typename PointType, typename PointType2>
// bool fuzzy_equal(PointType const & a, PointType2 const & b)
// {
//   bool ret = fuzzy_equal(viennagrid::to_cartesian(a),
//                          viennagrid::to_cartesian(b)
//                         );
//   if (!ret)
//     std::cerr << "Vectors: " << a << "; " << b << std::endl;
//   return ret;
// }


template<typename PointType1, typename PointType2>
void test_operations(PointType1 const & c0, PointType1 const & c1, PointType1 const & c2, PointType1 const & c3,
                     PointType2 const & p0, PointType2 const & p1, PointType2 const & p2, PointType2 const & p3)
{

  std::cout << "Basic operations: ";
  if ( !fuzzy_equal(c0+c1, p0+p1)
       || !fuzzy_equal(c1+c2, p1+p2)
       || !fuzzy_equal(c2-c1, p2-p1)
      || !fuzzy_equal(c3 + 2.0 * c1, p3 + 2.0 * p1)
     )
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

  std::cout << "Testing inner products: ";
  if ( !fuzzy_equal(viennagrid::inner_prod(c1, c0), viennagrid::inner_prod(p1, p0))
       || !fuzzy_equal(viennagrid::inner_prod(c2, c1), viennagrid::inner_prod(p2, p1))
       || !fuzzy_equal(viennagrid::inner_prod(c3, c0), viennagrid::inner_prod(p3, p0))
       || !fuzzy_equal(viennagrid::inner_prod(c1, c3), viennagrid::inner_prod(p1, p3))
     )
    exit(EXIT_FAILURE);
  if ( !fuzzy_equal(viennagrid::inner_prod(c1, p0), viennagrid::inner_prod(c1, p0))
       || !fuzzy_equal(viennagrid::inner_prod(c2, p1), viennagrid::inner_prod(c2, p1))
       || !fuzzy_equal(viennagrid::inner_prod(c3, p0), viennagrid::inner_prod(c3, p0))
       || !fuzzy_equal(viennagrid::inner_prod(c1, p3), viennagrid::inner_prod(c1, p3)) )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;



  std::cout << "Testing cross products: ";
  if ( !fuzzy_equal(viennagrid::cross_prod(c1, c0), viennagrid::cross_prod(p1, p0))
       || !fuzzy_equal(viennagrid::cross_prod(c2, c1), viennagrid::cross_prod(p2, p1))
       || !fuzzy_equal(viennagrid::cross_prod(c3, c0), viennagrid::cross_prod(p3, p0))
       || !fuzzy_equal(viennagrid::cross_prod(c1, c3), viennagrid::cross_prod(p1, p3))
     )
    exit(EXIT_FAILURE);
  if ( !fuzzy_equal(viennagrid::cross_prod(c1, p0), viennagrid::cross_prod(c1, p0))
       || !fuzzy_equal(viennagrid::cross_prod(c2, p1), viennagrid::cross_prod(c2, p1))
       || !fuzzy_equal(viennagrid::cross_prod(c3, p0), viennagrid::cross_prod(c3, p0))
       || !fuzzy_equal(viennagrid::cross_prod(c1, p3), viennagrid::cross_prod(c1, p3)) )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;

  std::cout << "Testing norms: ";
  if (    !fuzzy_equal(viennagrid::norm(c0, viennagrid::one_norm_tag()), viennagrid::norm(p0, viennagrid::one_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c1, viennagrid::one_norm_tag()), viennagrid::norm(p1, viennagrid::one_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c2, viennagrid::one_norm_tag()), viennagrid::norm(p2, viennagrid::one_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c3, viennagrid::one_norm_tag()), viennagrid::norm(p3, viennagrid::one_norm_tag()))
     )
    exit(EXIT_FAILURE);
  if (    !fuzzy_equal(viennagrid::norm(c0, viennagrid::two_norm_tag()), viennagrid::norm(p0, viennagrid::two_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c1, viennagrid::two_norm_tag()), viennagrid::norm(p1, viennagrid::two_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c2, viennagrid::two_norm_tag()), viennagrid::norm(p2, viennagrid::two_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c3, viennagrid::two_norm_tag()), viennagrid::norm(p3, viennagrid::two_norm_tag()))
     )
    exit(EXIT_FAILURE);
  if (    !fuzzy_equal(viennagrid::norm(c0, viennagrid::inf_norm_tag()), viennagrid::norm(p0, viennagrid::inf_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c1, viennagrid::inf_norm_tag()), viennagrid::norm(p1, viennagrid::inf_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c2, viennagrid::inf_norm_tag()), viennagrid::norm(p2, viennagrid::inf_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c3, viennagrid::inf_norm_tag()), viennagrid::norm(p3, viennagrid::inf_norm_tag()))
     )
    exit(EXIT_FAILURE);
  if (    !fuzzy_equal(viennagrid::norm(3.1415*c3, viennagrid::two_norm_tag()), viennagrid::norm(3.1415*p3, viennagrid::two_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c1,        viennagrid::one_norm_tag()), viennagrid::norm(p1,        viennagrid::one_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c2+p1,     viennagrid::two_norm_tag()), viennagrid::norm(p2+c1,     viennagrid::two_norm_tag()))
       || !fuzzy_equal(viennagrid::norm(c3-c2,     viennagrid::inf_norm_tag()), viennagrid::norm(p3-p2,     viennagrid::inf_norm_tag()))
     )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;



  std::cout << "Testing spanned volume: ";
  if ( !fuzzy_equal(viennagrid::spanned_volume(c0, c1), viennagrid::spanned_volume(p0, p1))
       || !fuzzy_equal(viennagrid::spanned_volume(c1, c2), viennagrid::spanned_volume(p1, p2))
       || !fuzzy_equal(viennagrid::spanned_volume(c2, p3), viennagrid::spanned_volume(p2, c3))
       || !fuzzy_equal(viennagrid::spanned_volume(c3, p0), viennagrid::spanned_volume(p3, c0))
     )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;
}



inline void test_1d()
{
  typedef viennagrid::point PointType;

  PointType c0 = viennagrid::make_point(0);
  PointType c1 = viennagrid::make_point(0.5);
  PointType c2 = viennagrid::make_point(1);

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
//   typedef viennagrid::spatial_point<double, CSystem1>   PointType1;
//   typedef viennagrid::spatial_point<double, CSystem2>   PointType2;
  typedef viennagrid::point PointType;

  PointType c0 = viennagrid::make_point(0, 0);
  PointType c1 = viennagrid::make_point(1, 0);
  PointType c2 = viennagrid::make_point(0, 1);
  PointType c3 = viennagrid::make_point(1, 1);

  PointType p0 = c0;
  PointType p1;
  p1 = c1;
  PointType p2(c2);
  PointType p3 = viennagrid::make_point(1, 1);
  p3 = c3;

  //convert to cartesian:
  PointType c4 = p0;
  PointType c5;
  c5 = p1;
  PointType c6(p2);
  PointType c7 = viennagrid::make_point(1, 1);
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

  std::cout << "Testing operations... " << std::endl;
  test_operations(c0, c1, c2, c3,
                  p0, p1, p2, p3);

  std::cout << "Testing spanned volume (part 2): ";
  if ( !fuzzy_equal(viennagrid::spanned_volume(c0, c1, c2), viennagrid::spanned_volume(p0, p1, p2))
       || !fuzzy_equal(viennagrid::spanned_volume(c1, c2, p3), viennagrid::spanned_volume(p1, p2, c3))
       || !fuzzy_equal(viennagrid::spanned_volume(c2, p3, p0), viennagrid::spanned_volume(p2, c3, c0))
       || !fuzzy_equal(viennagrid::spanned_volume(p3, c0, p1), viennagrid::spanned_volume(c3, p0, c1))
     )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;

}

void test_3d()
{
  typedef viennagrid::point PointType;

  PointType c0 = viennagrid::make_point(0, 0, 0);
  PointType c1 = viennagrid::make_point(1, 0, 1);
  PointType c2 = viennagrid::make_point(0, 1, 0);
  PointType c3 = viennagrid::make_point(1, 1, 1);

  PointType p0 = c0;
  PointType p1;
  p1 = c1;
  PointType p2(c2);
  PointType p3 = viennagrid::make_point(1, 1, 0);
  p3 = c3;

  //convert to cartesian:
  PointType c4 = p0;
  PointType c5;
  c5 = p1;
  PointType c6(p2);
  PointType c7 = viennagrid::make_point(1, 1);
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

  test_operations(c0, c1, c2, c3,
                  p0, p1, p2, p3);

  std::cout << "Testing spanned volume (part 2): ";
  if ( !fuzzy_equal(viennagrid::spanned_volume(c0, c1, c2), viennagrid::spanned_volume(p0, p1, p2))
       || !fuzzy_equal(viennagrid::spanned_volume(c1, c2, p3), viennagrid::spanned_volume(p1, p2, c3))
       || !fuzzy_equal(viennagrid::spanned_volume(c2, p3, p0), viennagrid::spanned_volume(p2, c3, c0))
       || !fuzzy_equal(viennagrid::spanned_volume(p3, c0, p1), viennagrid::spanned_volume(c3, p0, c1))
     )
    exit(EXIT_FAILURE);
  if ( !fuzzy_equal(viennagrid::spanned_volume(c0, c1, c2, c3), viennagrid::spanned_volume(p0, p1, p2, p3))
       || !fuzzy_equal(viennagrid::spanned_volume(c0, c1, c2, p3), viennagrid::spanned_volume(p0, p1, p2, c3))
       || !fuzzy_equal(viennagrid::spanned_volume(c0, c1, p2, p3), viennagrid::spanned_volume(p0, p1, c2, c3))
       || !fuzzy_equal(viennagrid::spanned_volume(c0, p1, c2, c3), viennagrid::spanned_volume(p0, c1, p2, p3))
     )
    exit(EXIT_FAILURE);
  std::cout << "[PASSED]" << std::endl;


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
