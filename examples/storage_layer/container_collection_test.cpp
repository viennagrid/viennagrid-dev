#include <iostream>
#include <typeinfo>

#include <map>

using std::cout;
using std::endl;


#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"

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
        viennagrid::storage::container::insert( c, typename container_type::value_type() );
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
    typedef VIENNAMETA_MAKE_TYPELIST_4(char, int, float, double) config;
    typedef VIENNAMETA_MAKE_TYPEMAP_2( viennagrid::storage::default_tag, viennagrid::storage::std_deque_tag,
                                       int, viennagrid::storage::std_deque_tag ) container_config;
        
    typedef viennagrid::storage::result_of::container_collection<config, container_config>::type collection_type;
    collection_type collection;
                                       
    
    typedef viennagrid::storage::result_of::recursive_inserter<collection_type>::type inserter_type;
    inserter_type inserter(collection);
    
    inserter.insert( 'c' );
    inserter.insert( 10 );
    inserter.insert( 35 );
    inserter.insert( 35.0 );
    inserter.insert( 35.4f );
    
    cout << "collection" << endl;
    cout << collection << endl;

    
    
    typedef viennagrid::storage::result_of::container_collection<
        VIENNAMETA_MAKE_TYPELIST_3(char, int, double),
        VIENNAMETA_MAKE_TYPEMAP_2( viennagrid::storage::default_tag, viennagrid::storage::std_vector_tag,
                                       int, viennagrid::storage::std_deque_tag )
    >::type collection2_type;
    
    collection2_type collection2;
    
    
    

    viennagrid::storage::container_collection::copy(collection, collection2);
    
    cout << "collection2" << endl;
    cout << collection2 << endl;
    
    
    
    
    
    
    typedef collection_type::container_typelist view_config;
    typedef VIENNAMETA_MAKE_TYPEMAP_2( viennagrid::storage::default_tag, viennagrid::storage::view_tag<viennagrid::storage::std_deque_tag>,
                                       float, viennagrid::storage::view_tag<viennagrid::storage::std_vector_tag> ) view_container_config;
                                       
    typedef viennagrid::storage::result_of::container_collection<view_config, view_container_config>::type view_collection_type;
    view_collection_type view_collection;
    
    viennagrid::storage::container_collection::reference(collection, view_collection);
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;
    
    
    
    
    
    
    typedef viennagrid::storage::result_of::recursive_inserter<view_collection_type, inserter_type>::type view_inserter_type;
    view_inserter_type view_inserter(view_collection, inserter);

    
    view_inserter.insert( 42 );
    
    cout << "After insert 42" << endl;
    
    cout << "collection" << endl;
    cout << collection << endl;
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;
    
    
    typedef view_collection_type::container_typelist view_config2;
    typedef VIENNAMETA_MAKE_TYPEMAP_2( viennagrid::storage::default_tag, viennagrid::storage::view_tag<viennagrid::storage::std_set_tag>,
                                       float, viennagrid::storage::view_tag<viennagrid::storage::std_deque_tag> ) view_container2_config;
                                       
    typedef viennagrid::storage::result_of::container_collection<view_config2, view_container2_config>::type view_collection2_type;
    view_collection2_type view_collection2;
    
    viennagrid::storage::container_collection::reference_if(view_collection, view_collection2, view2_predicate());
    
    cout << "view_collection2" << endl;
    cout << view_collection2 << endl;
    
    
    
    
    
    typedef viennagrid::storage::result_of::recursive_inserter<view_collection2_type, view_inserter_type>::type view_inserter2_type;
    view_inserter2_type view_inserter2(view_collection2, view_inserter);


    view_inserter2.insert( 20.0 );
    
    cout << "After insert 20.0" << endl;
    
    cout << "collection" << endl;
    cout << collection << endl;
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;
    
    cout << "view_collection2" << endl;
    cout << view_collection2 << endl;
    
    
    
    
    
    typedef viennagrid::storage::result_of::recursive_inserter<view_collection2_type, inserter_type>::type view_inserter3_type;
    view_inserter3_type view_inserter3(view_collection2, inserter);


    view_inserter3.insert( 4.2f );
    
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

