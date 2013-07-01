#ifndef VIENNAGRID_EXAMPLES_META_HPP
#define VIENNAGRID_EXAMPLES_META_HPP

#include <iostream>
#include <cstdlib>
#include <typeinfo>

template<typename type1, typename type2>
void test_type()
{
    
    bool is_the_same = viennameta::EQUAL<type1, type2>::value;
        
    std::cout << " " << std::boolalpha << is_the_same << std::endl;
    
    if (!is_the_same)
    {
        std::cout << "type1:" << std::endl;
        std::cout << typeid(type1).name() << std::endl;
        std::cout << "type2:" << std::endl;
        std::cout << typeid(type2).name() << std::endl;
        exit(-1);
    }
    
}


template<int value1, int value2>
void test_value()
{
    
    bool is_the_same = value1 == value2;
        
    std::cout << " " << std::boolalpha << is_the_same << std::endl;
    
    if (!is_the_same)
    {
        std::cout << "value1 = " << value1 << std::endl;
        std::cout << "value2 = " << value2 << std::endl;
        exit(-1);
    }
    
}

#endif
