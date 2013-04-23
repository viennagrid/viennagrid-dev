#include <iostream>
#include <typeinfo>

#include <map>

using std::cout;
using std::endl;


#include "viennagrid/forwards.hpp"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"


#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/element/element_key.hpp"
#include "viennagrid/point.hpp"



    


int main()
{
    //
    // First define the type of handle to use:
    //
    
    //typedef viennagrid::storage::pointer_handle_tag handle_tag;
    //typedef viennagrid::storage::iterator_handle_tag handle_tag;
    typedef viennagrid::storage::id_handle_tag handle_tag;
    
    //
    // Get the vertex type:
    //
    typedef viennagrid::element_t<  viennagrid::vertex_tag,   // element tag
                                    viennameta::null_type,    // boundary k-cells container (a vertex does not have boundary cells, hence 'null_type'
                                    viennagrid::storage::smart_id_tag<int>                       // type 'int' is used as ID
                                > vertex_type;
    
    //
    // Instantiate a few vertices (note that they are now free of any geometry information)
    //
    vertex_type v0;
    vertex_type v1;
    vertex_type v2;
    vertex_type v3;
    

                                                
                                                
    ///////////////////// Next step: Instantiate a line type ////////////////////////////////////                                            
    
    //
    // Create a container (type) for vertices. A handleed_container is basically a usual container which in addition provides handles
    //
    typedef viennagrid::storage::result_of::container<
            vertex_type,                                         // the 'value_type', i.e. vertices     
            viennagrid::storage::handleed_container_tag<           
                viennagrid::storage::std_deque_tag,              // use a std::deque as container
                handle_tag                                         // with the respective handles
            >
        >::type vertex_container_type;
    
        
    //
    // Create a view on the vertex container with two elements, thus referring to a line later on.
    //
    typedef viennameta::static_pair<                   
        viennagrid::storage::result_of::view<
            vertex_container_type,
            viennagrid::storage::static_array_tag<2>
        >::type,
        viennameta::null_type                                // no orientation container (only important for FEM-stuff)
    > line_vertex_containers;
    
    //
    // Provide the list of boundary k-cells. For a line, this is only the list of boundary vertices.
    // 
    // For technical reasons, all boundary containers of an element need to be wrapped in a typelist.
    // A line only has vertices as boundary k-cells, thus the typelist consists of a single container element only:
    //
    typedef viennameta::make_typelist< line_vertex_containers >::type line_containers_typelist;
    
    // 
    // Finally, we can instantiate the line type by passing the respective tag, the typelist of boundary k-cells, and the ID-type (int):
    //
    typedef viennagrid::element_t<  viennagrid::line_tag,           // element tag (as in ViennaGrid 1.0.x)
                                    line_containers_typelist,       // list of boundary containers
                                    viennagrid::storage::smart_id_tag<int>                             // ID type
                                    > line_type;
    
    typedef viennagrid::storage::result_of::container<
            line_type,
            viennagrid::storage::handleed_container_tag<
                //viennagrid::storage::std_deque_tag,
                viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag >,
                handle_tag
            >
        >::type line_container_type;

//     line_type line;
//     line.container( viennagrid::dimension_tag<0>() ).set_handle(&v0, 0);
//     line.container( viennagrid::dimension_tag<0>() ).set_handle(&v1, 1);
    
    
//     typedef viennagrid::viennagrid_ng::result_of::container_of_dimension<line_type, 0>::type vertices_of_line_range_type;
//     vertices_of_line_range_type range = viennagrid::viennagrid_ng::ncells<0>(line);   
//     std::copy( range.begin(), range.end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    

        
        
        
    ///////////////////// Next step: Instantiate a triangle type ////////////////////////////////////                                            
    
    //
    // First we define the container for vertices of a triangle (just handles to the vertex_container_type)
    //
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            vertex_container_type,
            viennagrid::storage::static_array_tag<3>
        >::type,
        viennameta::null_type                                // no orientation container (only important for FEM-stuff)
    > triangle_vertex_containers;
    
    //
    // Then we define the container for lines of a triangle (just handles to the line_container_type)
    //
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            line_container_type,
            viennagrid::storage::static_array_tag<3>
        >::type,
        viennameta::null_type                                // no orientation container (only important for FEM-stuff)
    > triangle_line_containers;
    
    
    //
    // Now all boundary k-cell-container of a triangle are given by concatenating the two vertex and line containers defined above
    //
    typedef viennameta::make_typelist< triangle_vertex_containers, triangle_line_containers >::type triangle_containers_typelist;
    
    //
    // Instantiate the triangle type as above: Provide the tag, the boundary container list, and the ID type
    //
    typedef viennagrid::element_t< viennagrid::triangle_tag, triangle_containers_typelist, viennagrid::storage::smart_id_tag<int> > triangle_type;
    
    //
    // Instantiate a handleed container of triangles using std::deque as the underlying container base. Hooks are again of the type defined at the beginning.
    //
    typedef viennagrid::storage::result_of::container<
            triangle_type,
            viennagrid::storage::handleed_container_tag<
                //viennagrid::storage::std_deque_tag,
                viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag >,
                handle_tag
            >
        >::type triangle_container_type;
    

        
        
        
    ///////////////////// Next step: Instantiate a tetrahedron type ////////////////////////////////////                                            
    
    
    //
    // Define the vertices of a tetrahedron to be a view of static size 4 on the vertex_container defined above
    //
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            vertex_container_type,
            viennagrid::storage::static_array_tag<4>
        >::type,
        viennameta::null_type                                // no orientation container (only important for FEM-stuff)
    > tetrahedron_vertex_containers;
    
    //
    // Define the lines of a tetrahedron to be a view of static size 6 on the line_container defined above
    //
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            line_container_type,
            viennagrid::storage::static_array_tag<6>
        >::type,
        viennameta::null_type                                // no orientation container (only important for FEM-stuff)
    > tetrahedron_line_containers;
    
    //
    // Define the facets of a tetrahedron to be a view of static size 4 on the triangle_container defined above
    //
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            triangle_container_type,
            viennagrid::storage::static_array_tag<4>
        >::type,
        viennameta::null_type                                // no orientation container (only important for FEM-stuff)
    > tetrahedron_triangle_containers;
    
    //
    // Now glue all the boundary k-cell container together into a single typelist:
    //
    typedef viennameta::make_typelist< tetrahedron_vertex_containers, tetrahedron_line_containers, tetrahedron_triangle_containers >::type tetrahedron_containers_typelist;  // use this for 'all in'
//    typedef viennameta::make_typelist< tetrahedron_vertex_containers, tetrahedron_line_containers >::type tetrahedron_containers_typelist;             // use this to skip triangles
    
    //
    // Now create the tetrahedron type as usual: Provide the tag, the boundary containers, and the ID type:
    //
    typedef viennagrid::element_t< viennagrid::tetrahedron_tag, tetrahedron_containers_typelist, viennagrid::storage::smart_id_tag<int> > tetrahedron_type;
    
    //
    // Instantiate a container of tetrahedra. A std::deque is used for the storage, and the usual handle type defined at the beginning is used
    //
    typedef viennagrid::storage::result_of::container<
            tetrahedron_type,
            viennagrid::storage::handleed_container_tag<
                viennagrid::storage::std_deque_tag,
                handle_tag
            >
        >::type tetrahedron_container_type;
        
        
    ///////////////////// Final step: Instantiate a domain consisting of the vertex, line, triangle, and tetrahedron types and containers defined above ////////////////////////////////////                                            
    
        
        
    //
    // Generate a domain as a collection of vertices, lines, (triangles), and tetrahedra. 
    //
    typedef viennagrid::storage::result_of::collection< 
        viennameta::make_typemap<
            vertex_type, vertex_container_type,
            line_type, line_container_type,
            triangle_type, triangle_container_type,
            tetrahedron_type, tetrahedron_container_type
        >::type   //including triangles
//        viennameta::make_typemap< vertex_type, vertex_container_type, line_type, line_container_type, tetrahedron_type, tetrahedron_container_type >::type                              //without triangles
    >::type domain_type;
    domain_type domain;
    
    //
    // for pushing n-cells to a domain, each of them needs to be equipped with an ID. For that purpose, a suitable ID generator is deduced here:
    //
    typedef viennagrid::storage::result_of::continuous_id_generator_config< viennagrid::storage::container_collection::result_of::value_typelist<domain_type>::type, viennagrid::storage::smart_id_tag<int> >::type id_generator_config;
    typedef viennagrid::storage::result_of::continuous_id_generator< id_generator_config >::type id_generator_type;
    
    id_generator_type id_generator;
    
    //
    // n-cells are inserted into the domain via dedicated inserter objects. Unless we wish to push an existing element to a view, a physical inserter is the way to go.
    //
    typedef viennagrid::storage::result_of::physical_inserter<domain_type, id_generator_type&>::type inserter_type;

    inserter_type inserter(domain, id_generator);
    
    //
    // Push the four vertices to the domain:
    //
    inserter(v0);
    inserter(v1);
    inserter(v2);
    inserter(v3);
    
    

    //
    // Alright, now lets create a tetrahedron and link it with the four vertices.
    //
    tetrahedron_type tet(domain);
    tet.container( viennagrid::dimension_tag<0>() ).set_handle( viennagrid::storage::collection::get<vertex_type>(domain).handle_at(0), 0);
    tet.container( viennagrid::dimension_tag<0>() ).set_handle( viennagrid::storage::collection::get<vertex_type>(domain).handle_at(1), 1);
    tet.container( viennagrid::dimension_tag<0>() ).set_handle( viennagrid::storage::collection::get<vertex_type>(domain).handle_at(2), 2);
    tet.container( viennagrid::dimension_tag<0>() ).set_handle( viennagrid::storage::collection::get<vertex_type>(domain).handle_at(3), 3);
    
    inserter(tet);
    

    //
    // Now lets print all the different elements in our domain:
    //
    std::copy( viennagrid::storage::collection::get<vertex_type>(domain).begin(), viennagrid::storage::collection::get<vertex_type>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::collection::get<line_type>(domain).begin(), viennagrid::storage::collection::get<line_type>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::collection::get<triangle_type>(domain).begin(), viennagrid::storage::collection::get<triangle_type>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::collection::get<tetrahedron_type>(domain).begin(), viennagrid::storage::collection::get<tetrahedron_type>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    

    return 0;
}


