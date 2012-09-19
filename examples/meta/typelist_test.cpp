#include <iostream>
#include <cstdlib>

#include "viennagrid/meta/typelist.hpp"




template<typename type1, typename type2>
void test_type()
{
    
    bool is_the_same = viennameta::_equal<type1, type2>::value;
        
    std::cout << " " << std::boolalpha << is_the_same << std::endl;
    
    if (!is_the_same) exit(-1);
    
}


template<int value1, int value2>
void test_value()
{
    
    bool is_the_same = value1 == value2;
        
    std::cout << " " << std::boolalpha << is_the_same << std::endl;
    
    if (!is_the_same) exit(-1);
    
}


int main()
{
    // size
    std::cout << "size" << std::endl;
    
    test_value<
        viennameta::typelist::result_of::size< VIENNAMETA_MAKE_TYPELIST_5(int, int, int, int, int) >::value,
        5
    >();

    
    // at
    std::cout << "at" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::at< VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short), 2 >::type,
        float
    >();
    
    test_type<
        viennameta::typelist::result_of::at< VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short), 0 >::type,
        int
    >();

    test_type<
        viennameta::typelist::result_of::at< VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short), 8 >::type,
        viennameta::out_of_range
    >();
    
    test_type<
        viennameta::typelist::result_of::at< VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short), -1000 >::type,
        viennameta::out_of_range
    >();
    
    
    // find
    std::cout << "find" << std::endl;
    
    test_value<
        viennameta::typelist::result_of::find< VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short), float >::value,
        2
    >();

    test_value<
        viennameta::typelist::result_of::find< VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short), unsigned short >::value,
        4
    >();
    
    test_value<
        viennameta::typelist::result_of::find< VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short), unsigned char >::value,
        -1
    >();


    
    // push_back
    std::cout << "push_back" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::push_back< VIENNAMETA_MAKE_TYPELIST_1(int), float >::type,
        VIENNAMETA_MAKE_TYPELIST_2(int, float)
    >();
    
    test_type<
        viennameta::typelist::result_of::push_back< VIENNAMETA_MAKE_TYPELIST_1(int), viennameta::null_type >::type,
        VIENNAMETA_MAKE_TYPELIST_1(int)
    >();
    
    test_type<
        viennameta::typelist::result_of::push_back< viennameta::null_type, int >::type,
        VIENNAMETA_MAKE_TYPELIST_1(int)
    >();
    
    
    // push_back_list
    std::cout << "push_back_list" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::push_back_list< VIENNAMETA_MAKE_TYPELIST_2(int,float), VIENNAMETA_MAKE_TYPELIST_2(char,double) >::type,
        VIENNAMETA_MAKE_TYPELIST_4(int, float, char, double)
    >();
    
    test_type<
        viennameta::typelist::result_of::push_back_list< viennameta::null_type, VIENNAMETA_MAKE_TYPELIST_2(char,double) >::type,
        VIENNAMETA_MAKE_TYPELIST_2(char, double)
    >();

    test_type<
        viennameta::typelist::result_of::push_back_list< VIENNAMETA_MAKE_TYPELIST_2(int,float), viennameta::null_type >::type,
        VIENNAMETA_MAKE_TYPELIST_2(int, float)
    >();

// not valid, should give a compiler error!
//     test<
//         viennameta::typelist::result_of::push_back_list< int, viennameta::null_type >::type,
//         VIENNAMETA_MAKE_TYPELIST_1(int)
//     >();
    
    
    
    // erase_at
    std::cout << "erase_at" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::erase_at< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), 0 >::type,
        VIENNAMETA_MAKE_TYPELIST_2(float, int)
    >();
    
    test_type<
        viennameta::typelist::result_of::erase_at< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), 1 >::type,
        VIENNAMETA_MAKE_TYPELIST_2(int, int)
    >();

    test_type<
        viennameta::typelist::result_of::erase_at< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), 8 >::type,
        VIENNAMETA_MAKE_TYPELIST_3(int, float, int)
    >();
    
    
    // erase
    std::cout << "erase" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::erase< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), int >::type,
        VIENNAMETA_MAKE_TYPELIST_2(float, int)
    >();
    
    test_type<
        viennameta::typelist::result_of::erase< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), float >::type,
        VIENNAMETA_MAKE_TYPELIST_2(int, int)
    >();

    test_type<
        viennameta::typelist::result_of::erase< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), double >::type,
        VIENNAMETA_MAKE_TYPELIST_3(int, float, int)
    >();
    
    
    // erase_all
    std::cout << "erase_all" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::erase_all< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), int >::type,
        VIENNAMETA_MAKE_TYPELIST_1(float)
    >();
    
    test_type<
        viennameta::typelist::result_of::erase_all< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), float >::type,
        VIENNAMETA_MAKE_TYPELIST_2(int, int)
    >();

    test_type<
        viennameta::typelist::result_of::erase_all< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), double >::type,
        VIENNAMETA_MAKE_TYPELIST_3(int, float, int)
    >();
    
    
    
    // no_duplicates
    std::cout << "no_duplicates" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::no_duplicates< VIENNAMETA_MAKE_TYPELIST_8(int, char, float, double, unsigned short, int, float, float) >::type,
        VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short)
    >();
    
    test_type<
        viennameta::typelist::result_of::no_duplicates< VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short) >::type,
        VIENNAMETA_MAKE_TYPELIST_5(int, char, float, double, unsigned short)
    >();
    
    
    
    
    // replace_at
    std::cout << "replace_at" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::replace_at< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), 0, double >::type,
        VIENNAMETA_MAKE_TYPELIST_3(double, float, int)
    >();
    
    test_type<
        viennameta::typelist::result_of::replace_at< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), 5, int >::type,
       VIENNAMETA_MAKE_TYPELIST_3(int,float,int)
    >();

    
    
    
    
    // replace
    std::cout << "replace" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::replace< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), int, double >::type,
        VIENNAMETA_MAKE_TYPELIST_3(double, float, int)
    >();
    
    test_type<
        viennameta::typelist::result_of::replace< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), double, int >::type,
       VIENNAMETA_MAKE_TYPELIST_3(int,float,int)
    >();


    // replace_all
    std::cout << "replace_all" << std::endl;
    
    test_type<
        viennameta::typelist::result_of::replace_all< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), int, double >::type,
        VIENNAMETA_MAKE_TYPELIST_3(double, float, double)
    >();
    
    test_type<
        viennameta::typelist::result_of::replace_all< VIENNAMETA_MAKE_TYPELIST_3(int,float,int), double, int >::type,
       VIENNAMETA_MAKE_TYPELIST_3(int,float,int)
    >();
    

    return 0;
}