#include <iostream>
#include <typeinfo>

#include <map>
#include <boost/concept_check.hpp>

using std::cout;
using std::endl;


#include "viennagrid/forwards.hpp"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/storage/range.hpp"


#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"

#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/topology_config.hpp"

#include "viennagrid/point.hpp"

#include "viennagrid/domain/topology.hpp"
#include "viennagrid/domain/element_creation.hpp"

//
// First define the type of handle to use:
//

class my_domain_config
{
  private:
    //typedef viennagrid::storage::pointer_handle_tag handle_tag;
    //typedef viennagrid::storage::iterator_handle_tag handle_tag;
    typedef viennagrid::storage::id_handle_tag          handle_tag;
    
  public:
    
    typedef viennagrid::config::result_of::full_topology_config<viennagrid::tetrahedron_tag, handle_tag>::type    type;
};


int main()
{
    
    //
    // typedefing and setting up the topological domain
    //
    
    typedef viennagrid::domain_t<my_domain_config> domain_type;
    domain_type domain;
    
    //typedef domain_type::foo_bar_type bla;    //enjoy short compiler error message now :-)
    
    //
    // typedefs for the element types
    //    
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type      vertex_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::vertex_tag>::type       vertex_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type        line_type;
    typedef viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type    triangle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::tetrahedron_tag>::type  tetrahedron_handle_type;
    

    //
    // Adding a tetrahedron
    //

    // creates four vertices within the domain, vh is short vor vertex handle
    vertex_handle_type vh0 = viennagrid::make_vertex( domain );
    vertex_handle_type vh1 = viennagrid::make_vertex( domain );
    vertex_handle_type vh2 = viennagrid::make_vertex( domain );
    vertex_handle_type vh3 = viennagrid::make_vertex( domain );
    
    // creates a handle buffer for the vertex handles of the tetdrahedron
    //std::vector<vertex_handle_type> handles(4);
    viennagrid::storage::static_array<vertex_handle_type, 4> handles;
    handles[0] = vh0; handles[1] = vh1; handles[2] = vh2; handles[3] = vh3;
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( domain, handles.begin(), handles.end() );

    
    //
    // display the domain content
    //
    cout << "  USING elements<tag>()" << endl;
    
    cout << "All vertices of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::vertex_tag>(domain).begin(), viennagrid::elements<viennagrid::vertex_tag>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    cout << "All lines of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::line_tag>(domain).begin(), viennagrid::elements<viennagrid::line_tag>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    cout << "All triangles of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::triangle_tag>(domain).begin(), viennagrid::elements<viennagrid::triangle_tag>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetraherons of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    
    cout << "  USING elements<type>()" << endl;
    
    cout << "All vertices of the domain" << endl;
    std::copy( viennagrid::elements<vertex_type>(domain).begin(), viennagrid::elements<vertex_type>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    cout << "All lines of the domain" << endl;
    std::copy( viennagrid::elements<line_type>(domain).begin(), viennagrid::elements<line_type>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    cout << "All triangles of the domain" << endl;
    std::copy( viennagrid::elements<triangle_type>(domain).begin(), viennagrid::elements<triangle_type>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetraherons of the domain" << endl;
    std::copy( viennagrid::elements<tetrahedron_type>(domain).begin(), viennagrid::elements<tetrahedron_type>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    
    //
    // doing some boundary cell iteration
    //
    
    const tetrahedron_type & test_tet = *viennagrid::elements<viennagrid::tetrahedron_tag>(domain).begin();
    const triangle_type & test_tri = *viennagrid::elements<viennagrid::triangle_tag>(domain).begin();
    
    
    typedef viennagrid::result_of::const_element_range<tetrahedron_type, viennagrid::triangle_tag>::type tetrahedron_triangle_range_2;
    typedef viennagrid::result_of::const_iterator<tetrahedron_triangle_range_2>::type tetrahedron_triangle_iterator_2;
    
    cout << "All triangles of the first tetdrahedron in the domain" << endl;
    tetrahedron_triangle_range_2 tri_range_2 = viennagrid::elements<viennagrid::triangle_tag>(test_tet);
    for (tetrahedron_triangle_iterator_2 it = tri_range_2.begin(); it != tri_range_2.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( tri_range_2.begin(), tri_range_2.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    
    
    
    
    typedef viennagrid::result_of::const_element_range<triangle_type, viennagrid::line_tag>::type triangle_line_range;
    typedef viennagrid::result_of::const_iterator<triangle_line_range>::type triangle_line_iterator;

    cout << "All lines of the first triangle in the domain" << endl;
    triangle_line_range lin_range = viennagrid::elements<viennagrid::line_tag>(test_tri);
    for (triangle_line_iterator it = lin_range.begin(); it != lin_range.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( lin_range.begin(), lin_range.end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;

    return 0;
}


