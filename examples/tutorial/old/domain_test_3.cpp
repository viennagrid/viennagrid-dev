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
#include "viennagrid/storage/range.hpp"


#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"

#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/domain_config.hpp"

#include "viennagrid/point.hpp"

#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/element_creation.hpp"


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

    return 0;
}


