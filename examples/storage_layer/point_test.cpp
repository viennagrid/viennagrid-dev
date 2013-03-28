#include <iostream>

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
#include "viennagrid/element/element_view.hpp"
#include "viennagrid/element/tagging.hpp"
#include "viennagrid/point.hpp"


#include "viennagrid/config/generic_config.hpp"

#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/metainfo.hpp"
#include "viennagrid/domain/geometric_domain.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/domain/metainfo_range.hpp"


#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;



struct test_tag {};
VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(test_tag)



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
    // typedefing and setting up the geometric domain
    //
    
    typedef viennagrid::result_of::geometric_domain< domain_config >::type domain_type;
    domain_type domain;
        
    //
    // typedefs for the element types
    //
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::vertex_tag>::type vertex_hook_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type triangle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_hook_type;
    

    //
    // Adding a tetrahedron
    //
    
    // creates four vertices within the domain, vh is short vor vertex hook
    // create_element is responsible for resizing all meta-info container which are associated with vertex_type
    vertex_hook_type vh0 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh1 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh2 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh3 = viennagrid::create_element<vertex_type>( domain );
    
    
    viennagrid::tagging::tag<test_tag>( viennagrid::dereference_hook(domain, vh1) );
    viennagrid::tagging::tag<test_tag>( viennagrid::dereference_hook(domain, vh2) );
    
    
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
    viennagrid::point(domain, vh0) = p0;
    viennagrid::point(domain, vh1) = p1;
    viennagrid::point(domain, vh2) = p2;
    viennagrid::point(domain, vh3) = p3;
    
    
    // creates a hook buffer for the vertex hooks of the tetdrahedron
    std::vector<vertex_hook_type> hooks(4);
    hooks[0] = vh0; hooks[1] = vh1; hooks[2] = vh2; hooks[3] = vh3;
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    tetrahedron_hook_type tet = viennagrid::create_element<tetrahedron_type>( domain, hooks );
    
    // set a double value to a tetdrahedron
    viennagrid::look_up<double>(domain, tet) = 1.0;
    
    
    
    const domain_type & test = domain;

    typedef viennagrid::result_of::const_element_range<domain_type, viennagrid::vertex_tag>::type const_vertex_range;
    typedef viennagrid::result_of::iterator<const_vertex_range>::type const_vertex_iterator;
    
    cout << "All vertices of the domain using point_range" << endl;
    const_vertex_range r = viennagrid::elements<viennagrid::vertex_tag>(test);
    for (const_vertex_iterator i = r.begin(); i != r.end(); ++i)
        cout << *i << " " << viennagrid::point(test, *i) << endl;
    cout << endl;
    
    
    cout << "All points of the domain using point_range" << endl;
    viennagrid::result_of::metainfo_range<domain_type, vertex_type, vector_type>::type point_range = viennagrid::metainfo_range<vertex_type, vector_type>(domain);
    for (viennagrid::result_of::metainfo_range<domain_type, vertex_type, vector_type>::type::iterator it = point_range.begin(); it != point_range.end(); ++it)
        cout << (*it) << endl;
    cout << endl;

    
    
    cout << "All points of the domain using point_range and std::copy" << endl;
    std::copy( point_range.begin(), point_range.end(), std::ostream_iterator<vector_type>(std::cout, "\n") );
    cout << endl;
    
    
    cout << "All points of the domain using point_range using const domain" << endl;
    viennagrid::result_of::const_metainfo_range<domain_type, vertex_type, vector_type>::type const_point_range =
            viennagrid::metainfo_range<vertex_type, vector_type>( test );
    
    for (viennagrid::result_of::const_metainfo_range<domain_type, vertex_type, vector_type>::type::iterator it = const_point_range.begin(); it != const_point_range.end(); ++it)
        cout << it.element() << " " << it.metainfo() << endl;
    cout << endl;


    
    cout << "All tagged points of the domain using point_range and std::copy" << endl;
    typedef viennagrid::result_of::element_view<domain_type, vertex_type>::type VertexView;
    VertexView view = viennagrid::tagged_elements<vertex_type>(domain, test_tag());
    viennagrid::result_of::metainfo_view<VertexView, viennagrid::result_of::metainfo_container<domain_type, vertex_type, vector_type>::type>::type
        point_range2 = viennagrid::metainfo_view<vector_type>( view, domain );    
    std::copy( point_range2.begin(), point_range2.end(), std::ostream_iterator< vector_type >(std::cout, "\n") );
    

    
    return 0;
}
