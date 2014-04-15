/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <iostream>

#include "viennagrid/meta/typemap.hpp"
#include "meta_common.hpp"



int main()
{
  std::cout << "size" << std::endl;
  test_value<
      viennagrid::detail::result_of::size< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type >::value,
      3
  >();



  std::cout << "at" << std::endl;
  test_type<
      viennagrid::detail::result_of::at< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 0 >::type,
      viennagrid::static_pair<int, unsigned int>
  >();

  test_type<
      viennagrid::detail::result_of::at< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 2 >::type,
      viennagrid::static_pair<short, unsigned short>
  >();

  test_type<
      viennagrid::detail::result_of::at< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, -10 >::type,
      viennagrid::out_of_range
  >();

  test_type<
      viennagrid::detail::result_of::at< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 10 >::type,
      viennagrid::out_of_range
  >();



  std::cout << "index_of" << std::endl;
  test_value<
      viennagrid::detail::result_of::index_of< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::value,
      0
  >();

  test_value<
      viennagrid::detail::result_of::index_of< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, char >::value,
      1
  >();

  test_value<
      viennagrid::detail::result_of::index_of< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, double >::value,
      -1
  >();



  std::cout << "find" << std::endl;
  test_type<
      viennagrid::detail::result_of::find< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::type::second,
      unsigned int
  >();

  test_type<
      viennagrid::detail::result_of::find< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, char >::type::second,
      unsigned char
  >();

  test_type<
      viennagrid::detail::result_of::find< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, short >::type::second,
      unsigned short
  >();

  test_type<
      viennagrid::detail::result_of::find< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, float >::type,
      viennagrid::not_found
  >();



  std::cout << "insert" << std::endl;
  test_type<
      viennagrid::detail::result_of::insert< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::static_pair<float, double> >::type,
      viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, double>::type
  >();

  test_type<
      viennagrid::detail::result_of::insert< viennagrid::null_type, viennagrid::static_pair<float, double> >::type,
      viennagrid::make_typemap<float, double>::type
  >();

  test_type<
      viennagrid::detail::result_of::insert< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::null_type >::type,
      viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
  >();

  // this should not work and generate a compiler error
//     test_type<
//         viennagrid::detail::result_of::insert< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::static_pair<int, double> >::type,
//         viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, double>::type
//     >();



  std::cout << "modify" << std::endl;
  test_type<
      viennagrid::detail::result_of::modify< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::static_pair<int, float> >::type,
      viennagrid::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
  >();

  // this should not work and generate a compiler error
//     test_type<
//         viennagrid::detail::result_of::modify< viennagrid::null_type, viennagrid::static_pair<int, float> >::type,
//         viennagrid::null_type
//     >();

  // this should not work and generate a compiler error
//     test_type<
//         viennagrid::detail::result_of::modify< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::static_pair<float, float> >::type,
//         viennagrid::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
//     >();



  std::cout << "insert_or_modify" << std::endl;
  test_type<
      viennagrid::detail::result_of::insert_or_modify< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::static_pair<int, float> >::type,
      viennagrid::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
  >();

  test_type<
      viennagrid::detail::result_of::insert_or_modify< viennagrid::null_type, viennagrid::static_pair<float, double> >::type,
      viennagrid::make_typemap<float, double>::type
  >();

  test_type<
      viennagrid::detail::result_of::insert_or_modify< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::null_type >::type,
      viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
  >();

  test_type<
      viennagrid::detail::result_of::insert_or_modify< viennagrid::null_type, viennagrid::static_pair<int, double> >::type,
      viennagrid::make_typemap<int, double>::type
  >();

  test_type<
      viennagrid::detail::result_of::insert_or_modify< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::static_pair<float, float> >::type,
      viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float>::type
  >();




  std::cout << "erase" << std::endl;
  test_type<
      viennagrid::detail::result_of::erase< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::type,
      viennagrid::make_typemap<char, unsigned char, short, unsigned short>::type
  >();

  // this should not work and generate a compiler error
//     test_type<
//         viennagrid::detail::result_of::erase< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, float >::type,
//         viennagrid::null_type
//     >();



  std::cout << "merge" << std::endl;
  test_type<
      viennagrid::detail::result_of::merge<
          viennagrid::null_type,
          viennagrid::make_typemap<float, float, double, double>::type
      >::type,
     viennagrid::make_typemap<float, float, double, double>::type
  >();

  test_type<
      viennagrid::detail::result_of::merge<
          viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
          viennagrid::null_type
      >::type,
      viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
  >();

  test_type<
      viennagrid::detail::result_of::merge<
          viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
          viennagrid::make_typemap<float, float, double, double>::type
      >::type,
      viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
  >();

  // this should not work and generate a compiler error
//     test_type<
//         viennagrid::detail::result_of::merge<
//             viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
//             viennagrid::make_typemap<int, float, double, double>::type
//         >::type,
//         viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
//     >();

  test_type<
      viennagrid::detail::result_of::merge_overwrite<
          viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
          viennagrid::make_typemap<float, float, double, double, int, bool>::type
      >::type,
      viennagrid::make_typemap<int, bool, char, unsigned char, short, unsigned short, float, float, double, double>::type
  >();

  test_type<
      viennagrid::detail::result_of::merge_ignore<
          viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
          viennagrid::make_typemap<float, float, double, double, int, bool>::type
      >::type,
      viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
  >();


  std::cout << "consistency test" << std::endl;
  test_type<
      viennagrid::detail::result_of::consistency< viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type >::type,
      viennagrid::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
  >();

  // this should not work and generate a compiler error
  // viennagrid::make_typemap<int, unsigned int, char, unsigned char, int, unsigned short>::type tmp;

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
