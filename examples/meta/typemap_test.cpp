#include <typeinfo>
#include <iostream>
#include <cstdlib>

#include "viennagrid/meta/typemap.hpp"




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

    std::cout << "consistency test" << std::endl;
    test_type<
        viennameta::typemap::result_of::consistency< VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short) >::type,
        VIENNAMETA_MAKE_TYPEMAP_3( int, unsigned int, char, unsigned char, short, unsigned short)
    >();
    
   
    return 0;
}