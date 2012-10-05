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

template<typename _id_type>
struct id_element
{
public:
    typedef _id_type id_type;
    
    void id(id_type _id) { id_ = _id; }
    id_type id() const { return id_; }

private:
    id_type id_;
};


template<typename _numeric_type>
struct vertex : public id_element<int>
{
    typedef _numeric_type numeric_type;
    typedef VIENNAMETA_MAKE_TYPELIST_1( vertex<numeric_type> ) required_types;    
    
    enum { dim = 2 };
    
    vertex(numeric_type _x, numeric_type _y) : x(_x), y(_y) {}
        
    template<typename inserter_type>
    void insert_callback( inserter_type & inserter, bool inserted )
    {}
    
    numeric_type x;
    numeric_type y;
};

template <typename numeric_type>
std::ostream & operator<<(std::ostream & os, const vertex<numeric_type> & v)
{
    os << "[Vertex] (" << v.x << "," << v.y << ")";    
    return os;
}


template<typename _vertex_type>
struct line : public id_element<int>
{
    typedef _vertex_type vertex_type;
    typedef typename viennameta::typelist::result_of::push_back<typename vertex_type::required_types, line<vertex_type> >::type required_types;
    
    template<typename inserter_type>
    void insert_callback( inserter_type & inserter, bool inserted )
    {}
    
    vertex_type * vertices[2];
};

template <typename vertex_type>
std::ostream & operator<<(std::ostream & os, const line<vertex_type> & l)
{
    os << "[Line] " << *l.vertices[0] << " " << *l.vertices[1];
    return os;
}

template<typename _vertex_type>
struct triangle : public id_element<int>
{
    typedef _vertex_type vertex_type;
    typedef line<vertex_type> line_type;
    
    typedef typename viennameta::typelist::result_of::push_back<typename line_type::required_types, triangle<vertex_type> >::type required_types;
    
    template<typename inserter_type>
    void insert_callback( inserter_type & inserter, bool inserted )
    {
        line_type l;
        
        l.vertices[0] = vertices[0];
        l.vertices[1] = vertices[1];
        lines[0] = &* inserter(l).first;
        
        l.vertices[0] = vertices[1];
        l.vertices[1] = vertices[2];
        lines[1] = &* inserter(l).first;

        l.vertices[0] = vertices[2];
        l.vertices[1] = vertices[0];
        lines[2] = &* inserter(l).first;
    }
    
    line_type * lines[3];
    vertex_type * vertices[3];
};

template <typename vertex_type>
std::ostream & operator<<(std::ostream & os, const triangle<vertex_type> & t)
{
    os << "[Triangle] \n";
    os << "  " << *t.vertices[0] << "\n";
    os << "  " << *t.vertices[1] << "\n";
    os << "  " << *t.vertices[2] << "\n";
    os << "  " << *t.lines[0] << "\n";
    os << "  " << *t.lines[1] << "\n";
    os << "  " << *t.lines[2] << "\n";
    
    return os;
}
    


int main()
{
    typedef vertex<double> vertex_type;
    typedef line<vertex_type> line_type;
    typedef triangle<vertex_type> triangle_type;
    
    typedef viennagrid::storage::result_of::container_collection<
        triangle<vertex_type>::required_types,
        viennagrid::storage::container_collection::default_container_config
    >::type collection_type;
    
    collection_type collection;
    
    
    typedef viennagrid::storage::result_of::continuous_id_generator_layer< triangle<vertex_type>::required_types >::type id_generator_type;
    id_generator_type id_generator;
    
    
    typedef viennagrid::storage::result_of::physical_inserter<
        collection_type,
        viennagrid::storage::inserter::pointer_reference_config, 
        id_generator_type
    >::type inserter_type;
    inserter_type inserter(collection, id_generator);
    
    
    triangle_type t;
    
    t.vertices[0] = &* inserter( vertex_type(0.0, 0.0) ).first;
    t.vertices[1] = &* inserter( vertex_type(1.0, 0.0) ).first;
    t.vertices[2] = &* inserter( vertex_type(0.0, 1.0) ).first;
    
    inserter( t );
    
    cout << "collection" << endl;
    cout << collection << endl;
    
    
    
    typedef viennagrid::storage::result_of::container_collection<
        viennameta::typelist::result_of::erase_at<viennagrid::storage::container_collection::result_of::container_typelist<collection_type>::type, 1>::type,
        viennagrid::storage::view::default_view_config
    >::type view_collection_type;
    
    view_collection_type view_collection;
    viennagrid::storage::container_collection::reference(collection, view_collection);
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;

    
    typedef viennagrid::storage::result_of::recursive_inserter<view_collection_type, inserter_type>::type view_inserter_type;
    view_inserter_type view_inserter(view_collection, inserter);
    
    t.vertices[0] = &* view_inserter( vertex_type(5.0, 0.0) ).first;
    t.vertices[1] = &* view_inserter( vertex_type(7.0, 5.0) ).first;
    t.vertices[2] = &* view_inserter( vertex_type(0.0, 7.0) ).first;
    
    view_inserter( t );
    
    cout << "After Inser" << endl;
    
    cout << "collection" << endl;
    cout << collection << endl;
    
    cout << "view_collection" << endl;
    cout << view_collection << endl;
    

    return 0;
}

