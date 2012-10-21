#include <iostream>
#include <typeinfo>

#include <vector>
#include <deque>


using std::cout;
using std::endl;


#include "viennagrid/storage/container.hpp"
#include "viennagrid/storage/view.hpp"

#include "viennagrid/storage/id.hpp"
#include "viennagrid/storage/algorithm.hpp"









struct id_int : public viennagrid::storage::id_handler<int>
{
    id_int(int value_) : value(value_) {}
    int value;
};


std::ostream & operator<<(std::ostream & os, std::vector<id_int>::iterator it)
{
    os << &* it;
    return os;
}

std::ostream & operator<<(std::ostream & os, std::deque<id_int>::iterator it)
{
    os << &* it;
    return os;
}


std::ostream & operator<<(std::ostream & os, const id_int & i)
{
    os << "[id=" << i.id() << "] " << i.value;
    return os;
}




template<typename container_tag>
void test()
{
    //typedef typename viennagrid::storage::result_of::container_from_tag< id_int, container_tag >::type my_container_type;
    typedef typename viennagrid::storage::result_of::hooked_container< id_int, container_tag >::type my_container_type;
    typedef typename my_container_type::hook_type hook_type;
    
    my_container_type my_container;
    id_int i0(10);
    i0.id(0);
    my_container.insert(i0);
    
    
    typedef typename viennagrid::storage::result_of::view<
                my_container_type,
                viennagrid::storage::hooked_container_tag<viennagrid::storage::std_deque_tag, viennagrid::storage::no_hook_tag>
            >::type my_view_type;
    my_view_type my_view;
    my_view.set_base_container(my_container);
    
    id_int i1(15);
    i1.id(1);
    my_view.insert_hook( my_container.insert(i1).first );
    
    
    typedef typename viennagrid::storage::result_of::view<
                my_view_type,
                viennagrid::storage::hooked_container_tag<viennagrid::storage::std_deque_tag, viennagrid::storage::no_hook_tag>
            >::type my_view_type2;
    my_view_type2 my_view2;
    my_view2.set_base_container(my_view);
    
    id_int i2(20);
    i2.id(2);
    my_view2.insert_hook( my_container.insert(i2).first );
    
    
    cout << typeid(my_container).name() << endl;
    cout << "my_container iteration ";
    std::copy( my_container.begin(), my_container.end(), std::ostream_iterator<id_int>(cout, " ") );
    cout << endl;
    cout << "my_container hook iteration ";
    for (typename my_container_type::hook_iterator it = my_container.hook_begin(); it != my_container.hook_end(); ++it)
        cout << *it << " ";
    cout << endl;
    cout << "my_container front/back " << my_container.front() << " " << my_container.back() << endl;
    cout << "my_container random access " << my_container[0] << " " << my_container[1] << endl;
    
    cout << endl;
    
    cout << typeid(my_view).name() << endl;
    cout << "my_view iteration ";
    std::copy( my_view.begin(), my_view.end(), std::ostream_iterator<id_int>(cout, " ") );
    cout << endl;

    cout << "my_view hook iteration ";
    for (typename my_view_type::hook_iterator it = my_view.hook_begin(); it != my_view.hook_end(); ++it)
        cout << *it << " ";
    cout << endl;
    cout << "my_view front/back " << my_view.front() << " " << my_view.back() << endl;
    cout << "my_view random access " << my_view[0] << endl;
    
    cout << endl;
    
    cout << typeid(my_view2).name() << endl;
    cout << "my_view2 iteration ";
    std::copy( my_view2.begin(), my_view2.end(), std::ostream_iterator<id_int>(cout, " ") );
    cout << endl;

    cout << "my_view2 hook iteration ";
    for (typename my_view_type::hook_iterator it = my_view2.hook_begin(); it != my_view2.hook_end(); ++it)
        cout << *it << " ";
    cout << endl;
    cout << "my_view2 front/back " << my_view2.front() << " " << my_view2.back() << endl;
    cout << "my_view2 random access " << my_view2[0] << endl;
    
    cout << endl;
    
    
    my_view_type my_view3;
    my_view3.set_base_container(my_container);
    viennagrid::storage::view::hook(my_container, my_view3);
    
    
    cout << "my_view3 iteration ";
    std::copy( my_view3.begin(), my_view3.end(), std::ostream_iterator<id_int>(cout, " ") );
    cout << endl;
    cout << "my_view3 hook iteration ";
    for (typename my_view_type::hook_iterator it = my_view3.hook_begin(); it != my_view3.hook_end(); ++it)
        cout << *it << " ";
    cout << endl;
    cout << "my_view3 front/back " << my_view3.front() << " " << my_view3.back() << endl;
    cout << "my_view3 random access " << my_view3[0] << endl;
    
    cout << endl;
    
    
    my_view3.clear();
    viennagrid::storage::view::hook(my_view, my_view3);
    
    
    cout << "clear and reference my_view" << endl;
    
    cout << endl;
    
    cout << "my_view3 iteration ";
    std::copy( my_view3.begin(), my_view3.end(), std::ostream_iterator<id_int>(cout, " ") );
    cout << endl;
    cout << "my_view3 hook iteration ";
    std::copy( my_view3.hook_begin(), my_view3.hook_end(), std::ostream_iterator<hook_type>(cout, " ") );
    //for (typename my_view_type::hook_iterator it = my_view3.hook_begin(); it != my_view3.hook_end(); ++it)
    //    cout << *it << " ";
    cout << endl;
    cout << "my_view3 front/back " << my_view3.front() << " " << my_view3.back() << endl;
    cout << "my_view3 random access " << my_view3[0] << endl;

    
}


int main()
{

    
    test< viennagrid::storage::hooked_container_tag< viennagrid::storage::std_vector_tag, viennagrid::storage::pointer_hook_tag > >();
    cout << endl 
         << " --------------------------- "
         << endl << endl;
    
    test< viennagrid::storage::hooked_container_tag< viennagrid::storage::std_vector_tag, viennagrid::storage::iterator_hook_tag > >();
    cout << endl 
         << " --------------------------- "
         << endl << endl;
    
    test< viennagrid::storage::hooked_container_tag< viennagrid::storage::std_vector_tag, viennagrid::storage::id_hook_tag<int> > >();
    cout << endl 
         << " --------------------------- "
         << endl << endl;

    test< viennagrid::storage::hooked_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::iterator_hook_tag > >();
    cout << endl 
         << " --------------------------- "
         << endl << endl;

    
    return 0;
}

