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

#include "viennagrid/meta/typemap.hpp"
#include "meta_common.hpp"



int main()
{
    std::cout << "size" << std::endl;
    test_value<
        viennagrid::meta::typemap::result_of::size< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type >::value,
        3
    >();



    std::cout << "at" << std::endl;
    test_type<
        viennagrid::meta::typemap::result_of::at< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 0 >::type,
        viennagrid::meta::static_pair<int, unsigned int>
    >();

    test_type<
        viennagrid::meta::typemap::result_of::at< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 2 >::type,
        viennagrid::meta::static_pair<short, unsigned short>
    >();

    test_type<
        viennagrid::meta::typemap::result_of::at< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, -10 >::type,
        viennagrid::meta::out_of_range
    >();

    test_type<
        viennagrid::meta::typemap::result_of::at< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 10 >::type,
        viennagrid::meta::out_of_range
    >();



    std::cout << "index_of" << std::endl;
    test_value<
        viennagrid::meta::typemap::result_of::index_of< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::value,
        0
    >();

    test_value<
        viennagrid::meta::typemap::result_of::index_of< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, char >::value,
        1
    >();

    test_value<
        viennagrid::meta::typemap::result_of::index_of< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, double >::value,
        -1
    >();



    std::cout << "find" << std::endl;
    test_type<
        viennagrid::meta::typemap::result_of::find< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::type::second,
        unsigned int
    >();

    test_type<
        viennagrid::meta::typemap::result_of::find< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, char >::type::second,
        unsigned char
    >();

    test_type<
        viennagrid::meta::typemap::result_of::find< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, short >::type::second,
        unsigned short
    >();

    test_type<
        viennagrid::meta::typemap::result_of::find< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, float >::type,
        viennagrid::meta::not_found
    >();



    std::cout << "insert" << std::endl;
    test_type<
        viennagrid::meta::typemap::result_of::insert< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::meta::static_pair<float, double> >::type,
        viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, double>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::insert< viennagrid::meta::null_type, viennagrid::meta::static_pair<float, double> >::type,
        viennagrid::meta::make_typemap<float, double>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::insert< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::meta::null_type >::type,
        viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
    >();

    // this should not work and generate a compiler error
//     test_type<
//         viennagrid::meta::typemap::result_of::insert< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::meta::static_pair<int, double> >::type,
//         viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, double>::type
//     >();



    std::cout << "modify" << std::endl;
    test_type<
        viennagrid::meta::typemap::result_of::modify< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::meta::static_pair<int, float> >::type,
        viennagrid::meta::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
    >();

    // this should not work and generate a compiler error
//     test_type<
//         viennagrid::meta::typemap::result_of::modify< viennagrid::meta::null_type, viennagrid::meta::static_pair<int, float> >::type,
//         viennagrid::meta::null_type
//     >();

    // this should not work and generate a compiler error
//     test_type<
//         viennagrid::meta::typemap::result_of::modify< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::meta::static_pair<float, float> >::type,
//         viennagrid::meta::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
//     >();



    std::cout << "insert_or_modify" << std::endl;
    test_type<
        viennagrid::meta::typemap::result_of::insert_or_modify< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::meta::static_pair<int, float> >::type,
        viennagrid::meta::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::insert_or_modify< viennagrid::meta::null_type, viennagrid::meta::static_pair<float, double> >::type,
        viennagrid::meta::make_typemap<float, double>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::insert_or_modify< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::meta::null_type >::type,
        viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::insert_or_modify< viennagrid::meta::null_type, viennagrid::meta::static_pair<int, double> >::type,
        viennagrid::meta::make_typemap<int, double>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::insert_or_modify< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennagrid::meta::static_pair<float, float> >::type,
        viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float>::type
    >();




    std::cout << "erase" << std::endl;
    test_type<
        viennagrid::meta::typemap::result_of::erase< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::type,
        viennagrid::meta::make_typemap<char, unsigned char, short, unsigned short>::type
    >();

    // this should not work and generate a compiler error
//     test_type<
//         viennagrid::meta::typemap::result_of::erase< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, float >::type,
//         viennagrid::meta::null_type
//     >();



    std::cout << "merge" << std::endl;
    test_type<
        viennagrid::meta::typemap::result_of::merge<
            viennagrid::meta::null_type,
            viennagrid::meta::make_typemap<float, float, double, double>::type
        >::type,
       viennagrid::meta::make_typemap<float, float, double, double>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::merge<
            viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
            viennagrid::meta::null_type
        >::type,
        viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::merge<
            viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
            viennagrid::meta::make_typemap<float, float, double, double>::type
        >::type,
        viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
    >();

    // this should not work and generate a compiler error
//     test_type<
//         viennagrid::meta::typemap::result_of::merge<
//             viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
//             viennagrid::meta::make_typemap<int, float, double, double>::type
//         >::type,
//         viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
//     >();

    test_type<
        viennagrid::meta::typemap::result_of::merge_overwrite<
            viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
            viennagrid::meta::make_typemap<float, float, double, double, int, bool>::type
        >::type,
        viennagrid::meta::make_typemap<int, bool, char, unsigned char, short, unsigned short, float, float, double, double>::type
    >();

    test_type<
        viennagrid::meta::typemap::result_of::merge_ignore<
            viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
            viennagrid::meta::make_typemap<float, float, double, double, int, bool>::type
        >::type,
        viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
    >();


    std::cout << "consistency test" << std::endl;
    test_type<
        viennagrid::meta::typemap::result_of::consistency< viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type >::type,
        viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
    >();

    // this should not work and generate a compiler error
    // viennagrid::meta::make_typemap<int, unsigned int, char, unsigned char, int, unsigned short>::type tmp;

    std::cout << "*******************************" << std::endl;
    std::cout << "* Test finished successfully! *" << std::endl;
    std::cout << "*******************************" << std::endl;
    return EXIT_SUCCESS;
}
