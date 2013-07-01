#include <iostream>

#include "viennagrid/meta/typelist.hpp"
#include "tests.hpp"

int main()
{
    // size
    std::cout << "size" << std::endl;
    
    test_value<
        //viennameta::typelist::result_of::size< VIENNAMETA_MAKE_TYPELIST_5(int, int, int, int, int) >::value,
        viennameta::typelist::result_of::size< viennameta::make_typelist<int, int, int, int, int>::type >::value,
        5
    >();

    
    // at
    std::cout << "at" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::at< viennameta::make_typelist<int, char, float, double, unsigned short>::type, 2 >::type,
        float
    >();
    
    test_type<
        viennameta::typelist::result_of::at< viennameta::make_typelist<int, char, float, double, unsigned short>::type, 0 >::type,
        int
    >();

    test_type<
        viennameta::typelist::result_of::at< viennameta::make_typelist<int, char, float, double, unsigned short>::type, 8 >::type,
        viennameta::out_of_range
    >();
    
    test_type<
        viennameta::typelist::result_of::at< viennameta::make_typelist<int, char, float, double, unsigned short>::type, -1000 >::type,
        viennameta::out_of_range
    >();
    
    
    // index_of
    std::cout << "index_of" << std::endl;
    
    test_value<
        viennameta::typelist::result_of::index_of< viennameta::make_typelist<int, char, float, double, unsigned short>::type, float >::value,
        2
    >();

    test_value<
        viennameta::typelist::result_of::index_of< viennameta::make_typelist<int, char, float, double, unsigned short>::type, unsigned short >::value,
        4
    >();
    
    test_value<
        viennameta::typelist::result_of::index_of< viennameta::make_typelist<int, char, float, double, unsigned short>::type, unsigned char >::value,
        -1
    >();


    
    // push_back
    std::cout << "push_back" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::push_back< viennameta::make_typelist<int>::type, float >::type,
        viennameta::make_typelist<int, float>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::push_back< viennameta::make_typelist<int>::type, viennameta::null_type >::type,
        viennameta::make_typelist<int>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::push_back< viennameta::null_type, int >::type,
        viennameta::make_typelist<int>::type
    >();
    
    
    // push_back_list
    std::cout << "push_back_list" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::push_back_list< viennameta::make_typelist<int, float>::type, viennameta::make_typelist<char, double>::type >::type,
        viennameta::make_typelist<int, float, char, double>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::push_back_list< viennameta::null_type, viennameta::make_typelist<char, double>::type >::type,
        viennameta::make_typelist<char, double>::type
    >();

    test_type<
        viennameta::typelist::result_of::push_back_list< viennameta::make_typelist<int, float>::type, viennameta::null_type >::type,
        viennameta::make_typelist<int, float>::type
    >();

// not valid, should give a compiler error!
//     test<
//         viennameta::typelist::result_of::push_back_list< int, viennameta::null_type >::type,
//         viennameta::make_typelist<int>::type
//     >();
    
    
    
    // erase_at
    std::cout << "erase_at" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::erase_at< viennameta::make_typelist<int, float, int>::type, 0 >::type,
        viennameta::make_typelist<float, int>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::erase_at< viennameta::make_typelist<int, float, int>::type, 1 >::type,
        viennameta::make_typelist<int, int>::type
    >();

    test_type<
        viennameta::typelist::result_of::erase_at< viennameta::make_typelist<int, float, int>::type, 8 >::type,
        viennameta::make_typelist<int, float, int>::type
    >();
    
    
    // erase
    std::cout << "erase" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::erase< viennameta::make_typelist<int, float, int>::type, int >::type,
        viennameta::make_typelist<float, int>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::erase< viennameta::make_typelist<int, float, int>::type, float >::type,
        viennameta::make_typelist<int, int>::type
    >();

    test_type<
        viennameta::typelist::result_of::erase< viennameta::make_typelist<int, float, int>::type, double >::type,
        viennameta::make_typelist<int, float, int>::type
    >();
    
    
    // erase_all
    std::cout << "erase_all" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::erase_all< viennameta::make_typelist<int, float, int>::type, int >::type,
        viennameta::make_typelist<float>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::erase_all< viennameta::make_typelist<int, float, int>::type, float >::type,
        viennameta::make_typelist<int, int>::type
    >();

    test_type<
        viennameta::typelist::result_of::erase_all< viennameta::make_typelist<int, float, int>::type, double >::type,
        viennameta::make_typelist<int, float, int>::type
    >();
    
    
    
    // no_duplicates
    std::cout << "no_duplicates" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::no_duplicates<viennameta::make_typelist<int, char, float, double, unsigned short, int, float, float>::type>::type,
        viennameta::make_typelist<int, char, float, double, unsigned short>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::no_duplicates< viennameta::make_typelist<int, char, float, double, unsigned short>::type >::type,
        viennameta::make_typelist<int, char, float, double, unsigned short>::type
    >();
    
    
    
    
    // replace_at
    std::cout << "replace_at" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::replace_at< viennameta::make_typelist<int, float, int>::type, 0, double >::type,
        viennameta::make_typelist<double, float, int>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::replace_at< viennameta::make_typelist<int, float, int>::type, 5, int >::type,
       viennameta::make_typelist<int, float, int>::type
    >();

    
    
    
    
    // replace
    std::cout << "replace" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::replace<viennameta::make_typelist<int, float, int>::type, int, double >::type,
        viennameta::make_typelist<double, float, int>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::replace< viennameta::make_typelist<int, float, int>::type, double, int >::type,
        viennameta::make_typelist<int, float, int>::type
    >();


    // replace_all
    std::cout << "replace_all" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::replace_all< viennameta::make_typelist<int, float, int>::type, int, double >::type,
        viennameta::make_typelist<double, float, double>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::replace_all< viennameta::make_typelist<int, float, int>::type, double, int >::type,
       viennameta::make_typelist<int, float, int>::type
    >();
    
    
    
    
    // intersection
    std::cout << "replace_all" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::intersection<
            viennameta::make_typelist<int, float, int>::type,
            viennameta::make_typelist<int, float, int>::type
        >::type,
        viennameta::make_typelist<int, float, int>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::intersection<
            viennameta::make_typelist<int, float, double>::type,
            viennameta::make_typelist<double, char, short>::type
        >::type,
        viennameta::make_typelist<double>::type
    >();
    
    test_type<
        viennameta::typelist::result_of::intersection<
            viennameta::make_typelist<int, float, int>::type,
            viennameta::make_typelist<double, char, short>::type
        >::type,
        viennameta::null_type
    >();

    test_type<
        viennameta::typelist::result_of::intersection<
            viennameta::make_typelist<int, float, int>::type,
            viennameta::null_type
        >::type,
        viennameta::null_type
    >();
    
    test_type<
        viennameta::typelist::result_of::intersection<
            viennameta::null_type,
            viennameta::make_typelist<double, char, short>::type
        >::type,
        viennameta::null_type
    >();
    
    return 0;
}
