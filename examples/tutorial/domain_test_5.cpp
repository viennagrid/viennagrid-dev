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
#include "viennagrid/config/domain_config.hpp"

#include "viennagrid/point.hpp"

#include "viennagrid/domain/topology.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/config/default_configs.hpp"


struct element_output_functor
{
    template<typename element_type>
    void operator() ( element_type const & element ) const
    {
        std::cout << element << std::endl;
    }
};


int main()
{
    
    //
    // typedefing and setting up the topological domain
    //
    
    //typedef viennagrid::domain_t<my_domain_config> domain_type;
    typedef viennagrid::tetrahedral_3d_domain domain_type;
    domain_type domain;
    
    //
    // typedefs for the element types
    //    
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::line_tag>::type line_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type triangle_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::triangle_tag>::type triangle_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_handle_type;
    
    
    
    //
    // typedefs for the view
    //   
    
    // only tetrahedrons are stored in the view
    // typedefing the view
    typedef viennagrid::result_of::domain_view<domain_type, tetrahedron_type>::type domain_view_type;
    
    // creating two views of the domain
    domain_view_type domain_view_1 = viennagrid::make_view(domain);
    domain_view_type domain_view_2 = viennagrid::make_view(domain);

    
    //
    // filling the view/domain
    //   
    
    std::vector<vertex_handle_type> handles(4);
    
    // pushing a tetrahedron to domain_view_1
    handles[0] = viennagrid::make_vertex( domain );
    handles[1] = viennagrid::make_vertex( domain );
    handles[2] = viennagrid::make_vertex( domain );
    handles[3] = viennagrid::make_vertex( domain );
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( domain_view_1, handles.begin(), handles.end() );
    
    
    // pushing a tetrahedron to domain_view_2
    handles[0] = viennagrid::make_vertex( domain );
    handles[1] = viennagrid::make_vertex( domain );
    handles[2] = viennagrid::make_vertex( domain );
    handles[3] = viennagrid::make_vertex( domain );
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( domain_view_2, handles.begin(), handles.end() );
    
    
    
    
    
    
    //
    // output
    //  
    
    cout << "All tetrahedrons of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the domain view 1" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_1).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetrahedrons of the domain view 2" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_2).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_2).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    
    
    
    cout << "--------------" << endl;
    cout << " inserting to domain_view_view" << endl;
    cout << "--------------" << endl;
    
    
    typedef viennagrid::result_of::domain_view< domain_view_type, tetrahedron_type >::type domain_view_view_type;
    
    domain_view_view_type domain_view_view_1 = viennagrid::make_view(domain_view_1);

    // pushing a tetrahedron to domain_view_1
    handles[0] = viennagrid::make_vertex( domain );
    handles[1] = viennagrid::make_vertex( domain );
    handles[2] = viennagrid::make_vertex( domain );
    handles[3] = viennagrid::make_vertex( domain );
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( domain_view_view_1, handles.begin(), handles.end() );


    
    //
    // output
    //  
    
    cout << "All tetrahedrons of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the domain view 1" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_1).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetrahedrons of the domain view 2" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_2).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_2).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetrahedrons of the domain view view" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_view_1).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain_view_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    
    
    cout << "All tetrahedrons of the domain using for_each" << endl;
    viennagrid::for_each<3>( domain, element_output_functor() );
    
    

    return 0;
}


