#include <iostream>
#include <typeinfo>

#include <map>

using std::cout;
using std::endl;


#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"

#include "viennagrid/storage/io.hpp"



struct count_size_functor
{
    int size;
    
    count_size_functor() : size(0) {}
    
    template<typename container_type>
    void operator() ( const container_type & c)
    {
        size += c.size();
    }  
};

struct output_functor
{
    template<typename container_type>
    void operator() ( const container_type & c)
    {
        cout << c.size() << endl;
    }  
};

struct insert_bullshit_functor
{
    template<typename container_type>
    void operator() ( container_type & c)
    {
        //viennagrid::storage::container::insert_helper<container_type>::insert_noreturn( c, typename container_type::value_type() );
        c.insert( typename container_type::value_type() );
    }  
};

struct view2_predicate
{
    template<typename type>
    bool operator() (type)
    {
        return true;
    }
    
    bool operator() (int i)
    {
        return i % 2 == 0;
    }
};
    


int main()
{
    typedef viennameta::make_typelist<char, int, float, double>::type config;
    
    
    typedef viennameta::make_typemap<   
                                        viennagrid::storage::default_tag,   viennagrid::storage::hooked_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::pointer_hook_tag>,
                                        int,                                viennagrid::storage::hooked_container_tag< viennagrid::storage::std_list_tag, viennagrid::storage::pointer_hook_tag>
                                    >::type container_config;
    typedef viennagrid::storage::result_of::container_collection<config, container_config>::type collection_type;
    collection_type collection;

    
    typedef viennagrid::storage::result_of::continuous_id_generator<config>::type id_generator_type;
    id_generator_type id_generator;
    
    
    typedef viennagrid::storage::result_of::physical_inserter< collection_type, id_generator_type& >::type inserter_type;
    inserter_type inserter(collection, id_generator);
    
    
    inserter( 'c' );
    inserter( 10 );
    inserter( 35 );
    inserter( 35.0 );
    inserter( 35.4f );
    
    cout << "collection" << endl;
    cout << collection << endl;

    
    
    typedef viennagrid::storage::result_of::container_collection<
        viennameta::make_typelist<char, int, double>::type,
        viennameta::make_typemap<   viennagrid::storage::default_tag,   viennagrid::storage::hooked_container_tag< viennagrid::storage::std_vector_tag, viennagrid::storage::no_hook_tag> ,
                                    int,                                viennagrid::storage::hooked_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::no_hook_tag>
                            >::type 
        >::type collection2_type;
    
    collection2_type collection2;
    
    
    

    viennagrid::storage::container_collection::copy(collection, collection2);
    
    cout << "collection2" << endl;
    cout << collection2 << endl;
    
    
    
    
    
    
    typedef viennameta::make_typemap<   
                                        viennagrid::storage::default_tag,       viennagrid::storage::hooked_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::no_hook_tag>,
                                        float,                                  viennagrid::storage::hooked_container_tag< viennagrid::storage::std_vector_tag, viennagrid::storage::no_hook_tag>
                                    >::type view_container_config;
    typedef viennagrid::storage::result_of::view_collection<collection_type, view_container_config>::type view_collection_type;
    view_collection_type view_collection;
    
    viennagrid::storage::container_collection::hook(collection, view_collection);
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;
    

    typedef viennagrid::storage::result_of::recursive_inserter<view_collection_type, inserter_type>::type view_inserter_type;
    view_inserter_type view_inserter(view_collection, inserter);

    
    view_inserter( 42 );
    
    cout << "After insert 42" << endl;
    
    cout << "collection" << endl;
    cout << collection << endl;
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;
    

    
    typedef viennameta::make_typemap<
                                        viennagrid::storage::default_tag,   viennagrid::storage::hooked_container_tag< viennagrid::storage::std_set_tag, viennagrid::storage::no_hook_tag>,
                                        float,                              viennagrid::storage::hooked_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::no_hook_tag>
                                    >::type view_container2_config;
    typedef viennagrid::storage::result_of::view_collection<view_collection_type, view_container_config>::type view_collection2_type;
    view_collection2_type view_collection2;

    viennagrid::storage::container_collection::hook_if(view_collection, view_collection2, view2_predicate());
    
    cout << "view_collection2" << endl;
    cout << view_collection2 << endl;
    
    
    
    
    
    typedef viennagrid::storage::result_of::recursive_inserter<view_collection2_type, view_inserter_type>::type view_inserter2_type;
    view_inserter2_type view_inserter2(view_collection2, view_inserter);


    view_inserter2( 20.0 );
    
    cout << "After insert 20.0" << endl;
    
    cout << "collection" << endl;
    cout << collection << endl;
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;
    
    cout << "view_collection2" << endl;
    cout << view_collection2 << endl;
    
    
    
    
    
    typedef viennagrid::storage::result_of::recursive_inserter<view_collection2_type, inserter_type>::type view_inserter3_type;
    view_inserter3_type view_inserter3(view_collection2, inserter);


    view_inserter3( 4.2f );
    
    cout << "After insert 4.2f" << endl;
    
    cout << "collection" << endl;
    cout << collection << endl;
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;
    
    cout << "view_collection2" << endl;
    cout << view_collection2 << endl;
    
    
    
    
    const collection_type & collection_ref = collection;

    count_size_functor csf;
    output_functor of;
    insert_bullshit_functor ibf;
    
    
    viennagrid::storage::collection::for_each( collection_ref, of );
    viennagrid::storage::collection::for_each<const collection_type, count_size_functor&>( collection_ref, csf );
    cout << "total size: " << csf.size << endl;
    
    viennagrid::storage::collection::for_each( collection, ibf );
    viennagrid::storage::collection::for_each( collection_ref, of );
    

    return 0;
}

