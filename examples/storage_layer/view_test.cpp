#include <algorithm>
#include <iostream>
#include <iterator>

#include <vector>
#include <deque>


using std::cout;
using std::endl;



#include "viennagrid/storage/view.hpp"



struct is_even
{
    template<typename type>
    bool operator()(type t) { return t % 2 == 0; } 
};

int main()
{
    std::deque<int> container;
    
    container.push_back(5);
    container.push_back(10);
    container.push_back(15);
    container.push_back(20);
    
    cout << "Creating container" << endl;
    cout << "container        : ";
    std::copy( container.begin(), container.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n\n";
    
    typedef viennagrid::storage::result_of::view< std::deque<int>, viennagrid::storage::std_deque_tag >::type my_view_type;
    my_view_type my_view(container);
    
    cout << "Creating all view of container" << endl;
    cout << "container        : ";
    std::copy( container.begin(), container.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n";
    cout << "my_view          : ";
    std::copy( my_view.begin(), my_view.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n\n";

    
    my_view.insert( 25 );
    
    cout << "Pushing one element to all view" << endl;
    cout << "container        : ";
    std::copy( container.begin(), container.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n";
    cout << "my_view          : ";
    std::copy( my_view.begin(), my_view.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n\n";
    
    
    my_view.erase( my_view.begin() );
    
    cout << "Erasing one element from all view" << endl;
    cout << "container        : ";
    std::copy( container.begin(), container.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n";
    cout << "my_view          : ";
    std::copy( my_view.begin(), my_view.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n\n";
    
    
    typedef viennagrid::storage::result_of::view< my_view_type, viennagrid::storage::std_vector_tag >::type my_view_view_type;
    my_view_view_type my_view_view(my_view, is_even());
    
    cout << "Creating view of all view (only even elements)" << endl;
    cout << "container        : ";
    std::copy( container.begin(), container.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n";
    cout << "my_view          : ";
    std::copy( my_view.begin(), my_view.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n";
    cout << "my_view_view     : ";
    std::copy( my_view_view.begin(), my_view_view.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n\n";
    
    
    my_view_view.insert(42);
    
    cout << "Pushing one element to the view of all view" << endl;
    cout << "container        : ";
    std::copy( container.begin(), container.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n";
    cout << "my_view          : ";
    std::copy( my_view.begin(), my_view.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n";
    cout << "my_view_view     : ";
    std::copy( my_view_view.begin(), my_view_view.end(), std::ostream_iterator<int>(cout, " ") );
    cout << "\n\n";

    
 	return 0;
}
