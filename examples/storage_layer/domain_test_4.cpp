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


#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/element/element_config.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/generic_config.hpp"

#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/metainfo.hpp"
#include "viennagrid/domain/geometric_domain.hpp"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;


int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    //typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    typedef viennagrid::storage::id_hook_tag hook_tag;
    
    
    
    
    
    typedef viennameta::make_typelist< viennameta::static_pair<viennagrid::tetrahedron_tag, double> >::type metainfo_typelist;
    

    typedef ublas::vector<double> vector_type;
    typedef viennagrid::result_of::geometric_domain_config< viennagrid::tetrahedron_tag, vector_type, hook_tag, metainfo_typelist >::type domain_config;

    
    
    
    //
    // typedefing and setting up the topological domain
    //
    
    typedef viennagrid::result_of::geometric_domain< domain_config >::type domain_type;
    domain_type domain;
    
    //
    // typedefs for the element types
    //    
     
    typedef viennagrid::result_of::ncell<domain_type, 0>::type vertex_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 0>::type vertex_hook_type;
    
    typedef viennagrid::result_of::ncell<domain_type, 1>::type line_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 1>::type line_hook_type;
    
    typedef viennagrid::result_of::ncell<domain_type, 2>::type triangle_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 2>::type triangle_hook_type;
    
    typedef viennagrid::result_of::ncell<domain_type, 3>::type tetrahedron_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 3>::type tetrahedron_hook_type;
    
    
    
    //
    // typedefs for the view
    //   
    
    // only tetrahedrons are stored in the view
    typedef viennameta::make_typelist< vertex_type, tetrahedron_type >::type view_types;
    
    
    
    
    
//     typedef viennagrid::result_of::topologic_view<domain_type::topologic_domain_type, view_types>::type topologic_view_type;
//     
//     typedef domain_type::metainfo_collection_type metainfo_collection_type;
//     
//     typedef viennagrid::geometric_domain_t<vector_type, topologic_view_type, metainfo_collection_type&> geometric_view_type;
    
    typedef viennagrid::result_of::geometric_view<domain_type, view_types>::type geometric_view_type;
    
    //geometric_view_type geometric_view( viennagrid::create_topologic_view<topologic_view_type>( viennagrid::topologic_domain(domain) ), domain.get_metainfo_collection() );
    
    geometric_view_type geometric_view_1 = viennagrid::create_view<geometric_view_type>(domain);
    geometric_view_type geometric_view_2 = viennagrid::create_view<geometric_view_type>(domain);
    
    
    
    //
    // Adding a tetrahedron
    //
    
    std::vector<vertex_hook_type> hooks(4);
    
    // creates four vertices within the domain, vh is short vor vertex hook
    // create_element is responsible for resizing all meta-info container which are associated with vertex_type
    vertex_hook_type vh0 = viennagrid::create_element<vertex_type>( geometric_view_1 );
    vertex_hook_type vh1 = viennagrid::create_element<vertex_type>( geometric_view_1 );
    vertex_hook_type vh2 = viennagrid::create_element<vertex_type>( geometric_view_1 );
    vertex_hook_type vh3 = viennagrid::create_element<vertex_type>( geometric_view_1 );
    
    // create geometric information for the vertices
    vector_type p0(3);
    p0[0] = 0.0; p0[1] = 0.0; p0[2] = 0.0;
    vector_type p1(3);
    p1[0] = 1.0; p1[1] = 0.0; p1[2] = 0.0;
    vector_type p2(3);
    p2[0] = 0.0; p2[1] = 1.0; p2[2] = 0.0;
    vector_type p3(3);
    p3[0] = 0.0; p3[1] = 0.0; p3[2] = 1.0;
    
    
    // set the geometric information for the vertices
    // is equivalent to viennagrid::look_up<vector_type>(domain, vhX)
    viennagrid::point(geometric_view_1, vh0) = p0;
    viennagrid::point(geometric_view_1, vh1) = p1;
    viennagrid::point(geometric_view_1, vh2) = p2;
    viennagrid::point(geometric_view_1, vh3) = p3;
    
    // pushing a tetrahedron to domain_view_1
    hooks[0] = vh0;
    hooks[1] = vh1;
    hooks[2] = vh2;
    hooks[3] = vh3;
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::create_element<tetrahedron_type>( geometric_view_1, hooks );
    
    
    
    
    // creates four vertices within the domain, vh is short vor vertex hook
    // create_element is responsible for resizing all meta-info container which are associated with vertex_type
    vertex_hook_type vh4 = viennagrid::create_element<vertex_type>( geometric_view_2 );
    vertex_hook_type vh5 = viennagrid::create_element<vertex_type>( geometric_view_2 );
    vertex_hook_type vh6 = viennagrid::create_element<vertex_type>( geometric_view_2 );
    vertex_hook_type vh7 = viennagrid::create_element<vertex_type>( geometric_view_2 );
    
    // create geometric information for the vertices
    vector_type p4(3);
    p4[0] = 40.0; p4[1] = 40.0; p4[2] = 40.0;
    vector_type p5(3);
    p5[0] = 41.0; p5[1] = 40.0; p5[2] = 40.0;
    vector_type p6(3);
    p6[0] = 40.0; p6[1] = 41.0; p6[2] = 40.0;
    vector_type p7(3);
    p7[0] = 40.0; p7[1] = 40.0; p7[2] = 41.0;
    
    
    // set the geometric information for the vertices
    // is equivalent to viennagrid::look_up<vector_type>(domain, vhX)
    viennagrid::point(geometric_view_2, vh4) = p4;
    viennagrid::point(geometric_view_2, vh5) = p5;
    viennagrid::point(geometric_view_2, vh6) = p6;
    viennagrid::point(geometric_view_2, vh7) = p7;
    
    // pushing a tetrahedron to domain_view_1
    hooks[0] = vh4;
    hooks[1] = vh5;
    hooks[2] = vh6;
    hooks[3] = vh7;
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::create_element<tetrahedron_type>( geometric_view_2, hooks );
    
    
    //
    // output
    //  
    
    cout << "All tetrahedrons of the domain" << endl;
    std::copy( viennagrid::ncells<3>(domain).begin(), viennagrid::ncells<3>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetrahedrons of the geometric_view_1" << endl;
    std::copy( viennagrid::ncells<3>(geometric_view_1).begin(), viennagrid::ncells<3>(geometric_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetrahedrons of the geometric_view_2" << endl;
    std::copy( viennagrid::ncells<3>(geometric_view_2).begin(), viennagrid::ncells<3>(geometric_view_2).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;


    
    
    //
    // geometric iteration
    //
    
    // iterating over all vertices and piping out the point information
    typedef viennagrid::result_of::const_ncell_range<domain_type, 0>::type domain_vertex_range;
    typedef viennagrid::result_of::const_iterator<domain_vertex_range>::type domain_vertex_iterator;
    
    cout << "All vertices of the geometric_view_1 with geometric info" << endl;
    domain_vertex_range domain_vtx_range = viennagrid::ncells<0>(domain);
    for (domain_vertex_iterator it = domain_vtx_range.begin(); it != domain_vtx_range.end(); ++it)
        cout << *it << " geometric information: " << viennagrid::look_up<vector_type>( domain, *it ) << endl;
    cout << endl;

    
    
    
    typedef viennagrid::result_of::const_ncell_range<geometric_view_type, 0>::type view_vertex_range;
    typedef viennagrid::result_of::const_iterator<view_vertex_range>::type view_vertex_iterator;
    
    // iterating over all vertices and piping out the point information    
    cout << "All vertices of the geometric_view_1 with geometric info" << endl;
    view_vertex_range view_vtx_range_1 = viennagrid::ncells<0>(geometric_view_1);
    for (view_vertex_iterator it = view_vtx_range_1.begin(); it != view_vtx_range_1.end(); ++it)
        cout << *it << " geometric information: " << viennagrid::look_up<vector_type>( domain, *it ) << endl;
    cout << endl;
    
    // iterating over all vertices and piping out the point information
    cout << "All vertices of the geometric_view_2 with geometric info" << endl;
    view_vertex_range view_vtx_range_2 = viennagrid::ncells<0>(geometric_view_2);
    for (view_vertex_iterator it = view_vtx_range_2.begin(); it != view_vtx_range_2.end(); ++it)
        cout << *it << " geometric information: " << viennagrid::look_up<vector_type>( domain, *it ) << endl;
    cout << endl;
    

    return 0;
}

