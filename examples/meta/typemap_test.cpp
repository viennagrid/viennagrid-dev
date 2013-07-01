#include <iostream>

#include "viennagrid/meta/typemap.hpp"
#include "tests.hpp"



int main()
{
    std::cout << "size" << std::endl;
    test_value<
        viennameta::typemap::result_of::size< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type >::value,
        3
    >();
    
    
    
    std::cout << "at" << std::endl;
    test_type<
        viennameta::typemap::result_of::at< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 0 >::type,
        viennameta::static_pair<int, unsigned int>
    >();
    
    test_type<
        viennameta::typemap::result_of::at< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 2 >::type,
        viennameta::static_pair<short, unsigned short>
    >();

    test_type<
        viennameta::typemap::result_of::at< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, -10 >::type,
        viennameta::out_of_range
    >();

    test_type<
        viennameta::typemap::result_of::at< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, 10 >::type,
        viennameta::out_of_range
    >();
    
    
    
    std::cout << "index_of" << std::endl;
    test_value<
        viennameta::typemap::result_of::index_of< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::value,
        0
    >();
    
    test_value<
        viennameta::typemap::result_of::index_of< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, char >::value,
        1
    >();
    
    test_value<
        viennameta::typemap::result_of::index_of< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, double >::value,
        -1
    >();    
    
    
    
    std::cout << "find" << std::endl;
    test_type<
        viennameta::typemap::result_of::find< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::type::second,
        unsigned int
    >();

    test_type<
        viennameta::typemap::result_of::find< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, char >::type::second,
        unsigned char
    >();
    
    test_type<
        viennameta::typemap::result_of::find< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, short >::type::second,
        unsigned short
    >();
    
    test_type<
        viennameta::typemap::result_of::find< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, float >::type,
        viennameta::not_found
    >();

    
    
    std::cout << "insert" << std::endl;
    test_type<
        viennameta::typemap::result_of::insert< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennameta::static_pair<float, double> >::type,
        viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, double>::type
    >();

    test_type<
        viennameta::typemap::result_of::insert< viennameta::null_type, viennameta::static_pair<float, double> >::type,
        viennameta::make_typemap<float, double>::type
    >();

    test_type<
        viennameta::typemap::result_of::insert< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennameta::null_type >::type,
        viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
    >();
    
    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::insert< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennameta::static_pair<int, double> >::type,
//         viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, double>::type
//     >();
    
    
    
    std::cout << "modify" << std::endl;
    test_type<
        viennameta::typemap::result_of::modify< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennameta::static_pair<int, float> >::type,
        viennameta::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
    >();
    
    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::modify< viennameta::null_type, viennameta::static_pair<int, float> >::type,
//         viennameta::null_type
//     >();
    
    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::modify< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennameta::static_pair<float, float> >::type,
//         viennameta::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
//     >();


    
    std::cout << "insert_or_modify" << std::endl;
    test_type<
        viennameta::typemap::result_of::insert_or_modify< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennameta::static_pair<int, float> >::type,
        viennameta::make_typemap<int, float, char, unsigned char, short, unsigned short>::type
    >();
    
    test_type<
        viennameta::typemap::result_of::insert_or_modify< viennameta::null_type, viennameta::static_pair<float, double> >::type,
        viennameta::make_typemap<float, double>::type
    >();

    test_type<
        viennameta::typemap::result_of::insert_or_modify< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennameta::null_type >::type,
        viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
    >();   
    
    test_type<
        viennameta::typemap::result_of::insert_or_modify< viennameta::null_type, viennameta::static_pair<int, double> >::type,
        viennameta::make_typemap<int, double>::type
    >();
    
    test_type<
        viennameta::typemap::result_of::insert_or_modify< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, viennameta::static_pair<float, float> >::type,
        viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float>::type
    >();
    
    
    
    
    std::cout << "erase" << std::endl;
    test_type<
        viennameta::typemap::result_of::erase< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, int >::type,
        viennameta::make_typemap<char, unsigned char, short, unsigned short>::type
    >();
    
    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::erase< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type, float >::type,
//         viennameta::null_type
//     >();
    
    
    
    std::cout << "merge" << std::endl;
    test_type<
        viennameta::typemap::result_of::merge<
            viennameta::null_type,
            viennameta::make_typemap<float, float, double, double>::type
        >::type,
       viennameta::make_typemap<float, float, double, double>::type
    >();
    
    test_type<
        viennameta::typemap::result_of::merge<
            viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
            viennameta::null_type
        >::type,
        viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
    >();
    
    test_type<
        viennameta::typemap::result_of::merge<
            viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
            viennameta::make_typemap<float, float, double, double>::type
        >::type,
        viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
    >();

    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::merge<
//             viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
//             viennameta::make_typemap<int, float, double, double>::type
//         >::type,
//         viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
//     >();

    test_type<
        viennameta::typemap::result_of::merge_overwrite<
            viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
            viennameta::make_typemap<float, float, double, double, int, bool>::type
        >::type,
        viennameta::make_typemap<int, bool, char, unsigned char, short, unsigned short, float, float, double, double>::type
    >();
    
    test_type<
        viennameta::typemap::result_of::merge_ignore<
            viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type,
            viennameta::make_typemap<float, float, double, double, int, bool>::type
        >::type,
        viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double>::type
    >();
    
    
    std::cout << "consistency test" << std::endl;
    test_type<
        viennameta::typemap::result_of::consistency< viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type >::type,
        viennameta::make_typemap<int, unsigned int, char, unsigned char, short, unsigned short>::type
    >();
    
    // this should not work and generate a compiler error
    // viennameta::make_typemap<int, unsigned int, char, unsigned char, int, unsigned short>::type tmp;
   
    return 0;
}
