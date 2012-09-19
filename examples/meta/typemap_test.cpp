#include <iostream>

#include "viennagrid/meta/typemap.hpp"
#include "tests.hpp"



int main()
{
    std::cout << "size" << std::endl;
    test_value<
        viennameta::typemap::result_of::size< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short) >::value,
        3
    >();
    
    
    
    std::cout << "at" << std::endl;
    test_type<
        viennameta::typemap::result_of::at< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), 0 >::type,
        viennameta::static_pair<int, unsigned int>
    >();
    
    test_type<
        viennameta::typemap::result_of::at< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), 2 >::type,
        viennameta::static_pair<short, unsigned short>
    >();

    test_type<
        viennameta::typemap::result_of::at< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), -10 >::type,
        viennameta::out_of_range
    >();

    test_type<
        viennameta::typemap::result_of::at< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), 10 >::type,
        viennameta::out_of_range
    >();
    
    
    
    std::cout << "index_of" << std::endl;
    test_value<
        viennameta::typemap::result_of::index_of< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), int >::value,
        0
    >();
    
    test_value<
        viennameta::typemap::result_of::index_of< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), char >::value,
        1
    >();
    
    test_value<
        viennameta::typemap::result_of::index_of< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), double >::value,
        -1
    >();    
    
    
    
    std::cout << "find" << std::endl;
    test_type<
        viennameta::typemap::result_of::find< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), int >::type::second,
        unsigned int
    >();

    test_type<
        viennameta::typemap::result_of::find< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), char >::type::second,
        unsigned char
    >();
    
    test_type<
        viennameta::typemap::result_of::find< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), short >::type::second,
        unsigned short
    >();
    
    test_type<
        viennameta::typemap::result_of::find< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), float >::type,
        viennameta::not_found
    >();

    
    
    std::cout << "insert" << std::endl;
    test_type<
        viennameta::typemap::result_of::insert< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), viennameta::static_pair<float, double> >::type,
        VIENNAMETA_MAKE_TYPEMAP_4( int, unsigned int, char, unsigned char, short, unsigned short, float, double)
    >();

    test_type<
        viennameta::typemap::result_of::insert< viennameta::null_type, viennameta::static_pair<float, double> >::type,
        VIENNAMETA_MAKE_TYPEMAP_1(float, double)
    >();

    test_type<
        viennameta::typemap::result_of::insert< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), viennameta::null_type >::type,
        VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short)
    >();
    
    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::insert< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), viennameta::static_pair<int, double> >::type,
//         VIENNAMETA_MAKE_TYPEMAP_4( int, unsigned int, char, unsigned char, short, unsigned short, float, double)
//     >();
    
    
    
    std::cout << "modify" << std::endl;
    test_type<
        viennameta::typemap::result_of::modify< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), viennameta::static_pair<int, float> >::type,
        VIENNAMETA_MAKE_TYPEMAP_3( int, float, char, unsigned char, short, unsigned short)
    >();
    
    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::modify< viennameta::null_type, int, float >::type,
//         viennameta::null_type
//     >();
    
    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::modify< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), float, float >::type,
//         VIENNAMETA_MAKE_TYPEMAP_3( int, float, char, unsigned char, short, unsigned short)
//     >();


    
    std::cout << "insert_or_modify" << std::endl;
    test_type<
        viennameta::typemap::result_of::insert_or_modify< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), viennameta::static_pair<int, float> >::type,
        VIENNAMETA_MAKE_TYPEMAP_3( int, float, char, unsigned char, short, unsigned short)
    >();
    
    test_type<
        viennameta::typemap::result_of::insert_or_modify< viennameta::null_type, viennameta::static_pair<float, double> >::type,
        VIENNAMETA_MAKE_TYPEMAP_1(float, double)
    >();

    test_type<
        viennameta::typemap::result_of::insert_or_modify< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), viennameta::null_type >::type,
        VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short)
    >();   
    
    test_type<
        viennameta::typemap::result_of::insert_or_modify< viennameta::null_type, viennameta::static_pair<int, double> >::type,
        VIENNAMETA_MAKE_TYPEMAP_1( int, double )
    >();
    
    test_type<
        viennameta::typemap::result_of::insert_or_modify< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), viennameta::static_pair<float, float> >::type,
        VIENNAMETA_MAKE_TYPEMAP_4( int, unsigned int, char, unsigned char, short, unsigned short, float, float )
    >();
    
    
    
    
    std::cout << "erase" << std::endl;
    test_type<
        viennameta::typemap::result_of::erase< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), int >::type,
        VIENNAMETA_MAKE_TYPEMAP_2(char, unsigned char, short, unsigned short)
    >();
    
    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::erase< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short), float >::type,
//         viennameta::null_type
//     >();
    
    
    
    std::cout << "merge" << std::endl;
    test_type<
        viennameta::typemap::result_of::merge<
            viennameta::null_type,
            VIENNAMETA_MAKE_TYPEMAP_2( float, float, double, double)
        >::type,
       VIENNAMETA_MAKE_TYPEMAP_2( float, float, double, double)
    >();
    
    test_type<
        viennameta::typemap::result_of::merge<
            VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short),
            viennameta::null_type
        >::type,
        VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short)
    >();
    
    test_type<
        viennameta::typemap::result_of::merge<
            VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short),
            VIENNAMETA_MAKE_TYPEMAP_2( float, float, double, double)
        >::type,
        VIENNAMETA_MAKE_TYPEMAP_5( int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double )
    >();

    // this should not work and generate a compiler error
//     test_type<
//         viennameta::typemap::result_of::merge<
//             VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short),
//             VIENNAMETA_MAKE_TYPEMAP_2( int, float, double, double)
//         >::type,
//         VIENNAMETA_MAKE_TYPEMAP_5( int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double )
//     >();

    test_type<
        viennameta::typemap::result_of::merge_overwrite<
            VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short),
            VIENNAMETA_MAKE_TYPEMAP_3( float, float, double, double, int, bool )
        >::type,
        VIENNAMETA_MAKE_TYPEMAP_5( int, bool, char, unsigned char, short, unsigned short, float, float, double, double )
    >();
    
    test_type<
        viennameta::typemap::result_of::merge_ignore<
            VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short),
            VIENNAMETA_MAKE_TYPEMAP_3( float, float, double, double, int, bool )
        >::type,
        VIENNAMETA_MAKE_TYPEMAP_5( int, unsigned int, char, unsigned char, short, unsigned short, float, float, double, double )
    >();
    
    
    std::cout << "consistency test" << std::endl;
    test_type<
        viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short) >::type,
        VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short)
    >();
    
    // this should not work and generate a compiler error
    //VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, int, unsigned short) tmp;
   
    return 0;
}