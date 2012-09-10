#include <iostream>

#include <vector>
#include <deque>


using std::cout;
using std::endl;


#include "viennagrid/Typelist.h"
#include "viennagrid/collection.hpp"


struct is_even
{
    template<typename type>
    bool operator()(type t) { return t % 2 == 0; } 
};

int main()
{
    typedef LOKI_TYPELIST_3(std::vector<int>, std::deque<float>, std::deque<int>) config;

    viennagrid::collection::result_of<config>::type collection;
    
    viennagrid::collection::container_of<viennagrid::collection::result_of<config>::type, int>::type & c = viennagrid::collection::get<int>( collection );
    c.push_back(10);
    
    cout << viennagrid::collection::get<float>( collection ).size() << endl;
    cout << viennagrid::collection::get<int>( collection ).size() << endl;

    
 	return 0;
}
