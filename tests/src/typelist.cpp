/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <iostream>

#include "viennagrid/meta/typelist.hpp"
#include "meta_common.hpp"

int main()
{
  // size
  std::cout << "size" << std::endl;

  test_value<
      //viennagrid::meta::result_of::size< VIENNAMETA_MAKE_TYPELIST_5(int, int, int, int, int) >::value,
      viennagrid::meta::result_of::size< viennagrid::make_typelist<int, int, int, int, int>::type >::value,
      5
  >();


  // at
  std::cout << "at" << std::endl;

  test_type<
      viennagrid::meta::result_of::at< viennagrid::make_typelist<int, char, float, double, unsigned short>::type, 2 >::type,
      float
  >();

  test_type<
      viennagrid::meta::result_of::at< viennagrid::make_typelist<int, char, float, double, unsigned short>::type, 0 >::type,
      int
  >();

  test_type<
      viennagrid::meta::result_of::at< viennagrid::make_typelist<int, char, float, double, unsigned short>::type, 8 >::type,
      viennagrid::out_of_range
  >();

  test_type<
      viennagrid::meta::result_of::at< viennagrid::make_typelist<int, char, float, double, unsigned short>::type, -1000 >::type,
      viennagrid::out_of_range
  >();


  // index_of
  std::cout << "index_of" << std::endl;

  test_value<
      viennagrid::meta::result_of::index_of< viennagrid::make_typelist<int, char, float, double, unsigned short>::type, float >::value,
      2
  >();

  test_value<
      viennagrid::meta::result_of::index_of< viennagrid::make_typelist<int, char, float, double, unsigned short>::type, unsigned short >::value,
      4
  >();

  test_value<
      viennagrid::meta::result_of::index_of< viennagrid::make_typelist<int, char, float, double, unsigned short>::type, unsigned char >::value,
      -1
  >();



  // push_back
  std::cout << "push_back" << std::endl;

  test_type<
      viennagrid::meta::result_of::push_back< viennagrid::make_typelist<int>::type, float >::type,
      viennagrid::make_typelist<int, float>::type
  >();

  test_type<
      viennagrid::meta::result_of::push_back< viennagrid::make_typelist<int>::type, viennagrid::null_type >::type,
      viennagrid::make_typelist<int>::type
  >();

  test_type<
      viennagrid::meta::result_of::push_back< viennagrid::null_type, int >::type,
      viennagrid::make_typelist<int>::type
  >();


  // push_back_list
  std::cout << "push_back_list" << std::endl;

  test_type<
      viennagrid::meta::result_of::push_back_list< viennagrid::make_typelist<int, float>::type, viennagrid::make_typelist<char, double>::type >::type,
      viennagrid::make_typelist<int, float, char, double>::type
  >();

  test_type<
      viennagrid::meta::result_of::push_back_list< viennagrid::null_type, viennagrid::make_typelist<char, double>::type >::type,
      viennagrid::make_typelist<char, double>::type
  >();

  test_type<
      viennagrid::meta::result_of::push_back_list< viennagrid::make_typelist<int, float>::type, viennagrid::null_type >::type,
      viennagrid::make_typelist<int, float>::type
  >();

// not valid, should give a compiler error!
//     test<
//         viennagrid::meta::result_of::push_back_list< int, viennagrid::null_type >::type,
//         viennagrid::make_typelist<int>::type
//     >();



  // erase_at
  std::cout << "erase_at" << std::endl;

  test_type<
      viennagrid::meta::result_of::erase_at< viennagrid::make_typelist<int, float, int>::type, 0 >::type,
      viennagrid::make_typelist<float, int>::type
  >();

  test_type<
      viennagrid::meta::result_of::erase_at< viennagrid::make_typelist<int, float, int>::type, 1 >::type,
      viennagrid::make_typelist<int, int>::type
  >();

  test_type<
      viennagrid::meta::result_of::erase_at< viennagrid::make_typelist<int, float, int>::type, 8 >::type,
      viennagrid::make_typelist<int, float, int>::type
  >();


  // erase
  std::cout << "erase" << std::endl;

  test_type<
      viennagrid::meta::result_of::erase< viennagrid::make_typelist<int, float, int>::type, int >::type,
      viennagrid::make_typelist<float, int>::type
  >();

  test_type<
      viennagrid::meta::result_of::erase< viennagrid::make_typelist<int, float, int>::type, float >::type,
      viennagrid::make_typelist<int, int>::type
  >();

  test_type<
      viennagrid::meta::result_of::erase< viennagrid::make_typelist<int, float, int>::type, double >::type,
      viennagrid::make_typelist<int, float, int>::type
  >();


  // erase_all
  std::cout << "erase_all" << std::endl;

  test_type<
      viennagrid::meta::result_of::erase_all< viennagrid::make_typelist<int, float, int>::type, int >::type,
      viennagrid::make_typelist<float>::type
  >();

  test_type<
      viennagrid::meta::result_of::erase_all< viennagrid::make_typelist<int, float, int>::type, float >::type,
      viennagrid::make_typelist<int, int>::type
  >();

  test_type<
      viennagrid::meta::result_of::erase_all< viennagrid::make_typelist<int, float, int>::type, double >::type,
      viennagrid::make_typelist<int, float, int>::type
  >();



  // no_duplicates
  std::cout << "no_duplicates" << std::endl;

  test_type<
      viennagrid::meta::result_of::no_duplicates<viennagrid::make_typelist<int, char, float, double, unsigned short, int, float, float>::type>::type,
      viennagrid::make_typelist<int, char, float, double, unsigned short>::type
  >();

  test_type<
      viennagrid::meta::result_of::no_duplicates< viennagrid::make_typelist<int, char, float, double, unsigned short>::type >::type,
      viennagrid::make_typelist<int, char, float, double, unsigned short>::type
  >();




  // replace_at
  std::cout << "replace_at" << std::endl;

  test_type<
      viennagrid::meta::result_of::replace_at< viennagrid::make_typelist<int, float, int>::type, 0, double >::type,
      viennagrid::make_typelist<double, float, int>::type
  >();

  test_type<
      viennagrid::meta::result_of::replace_at< viennagrid::make_typelist<int, float, int>::type, 5, int >::type,
     viennagrid::make_typelist<int, float, int>::type
  >();





  // replace
  std::cout << "replace" << std::endl;

  test_type<
      viennagrid::meta::result_of::replace<viennagrid::make_typelist<int, float, int>::type, int, double >::type,
      viennagrid::make_typelist<double, float, int>::type
  >();

  test_type<
      viennagrid::meta::result_of::replace< viennagrid::make_typelist<int, float, int>::type, double, int >::type,
      viennagrid::make_typelist<int, float, int>::type
  >();


  // replace_all
  std::cout << "replace_all" << std::endl;

  test_type<
      viennagrid::meta::result_of::replace_all< viennagrid::make_typelist<int, float, int>::type, int, double >::type,
      viennagrid::make_typelist<double, float, double>::type
  >();

  test_type<
      viennagrid::meta::result_of::replace_all< viennagrid::make_typelist<int, float, int>::type, double, int >::type,
     viennagrid::make_typelist<int, float, int>::type
  >();




  // intersection
  std::cout << "replace_all" << std::endl;

  test_type<
      viennagrid::meta::result_of::intersection<
          viennagrid::make_typelist<int, float, int>::type,
          viennagrid::make_typelist<int, float, int>::type
      >::type,
      viennagrid::make_typelist<int, float, int>::type
  >();

  test_type<
      viennagrid::meta::result_of::intersection<
          viennagrid::make_typelist<int, float, double>::type,
          viennagrid::make_typelist<double, char, short>::type
      >::type,
      viennagrid::make_typelist<double>::type
  >();

  test_type<
      viennagrid::meta::result_of::intersection<
          viennagrid::make_typelist<int, float, int>::type,
          viennagrid::make_typelist<double, char, short>::type
      >::type,
      viennagrid::null_type
  >();

  test_type<
      viennagrid::meta::result_of::intersection<
          viennagrid::make_typelist<int, float, int>::type,
          viennagrid::null_type
      >::type,
      viennagrid::null_type
  >();

  test_type<
      viennagrid::meta::result_of::intersection<
          viennagrid::null_type,
          viennagrid::make_typelist<double, char, short>::type
      >::type,
      viennagrid::null_type
  >();

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
