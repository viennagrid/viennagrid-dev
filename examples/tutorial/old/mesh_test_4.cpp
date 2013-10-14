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
#include "viennagrid/config/mesh_config.hpp"

#include "viennagrid/point.hpp"

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element_creation.hpp"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;

typedef ublas::vector<double> vector_type;

class my_mesh_config
{
  private:
    //typedef viennagrid::storage::pointer_handle_tag handle_tag;
    //typedef viennagrid::storage::iterator_handle_tag handle_tag;
    typedef viennagrid::storage::id_handle_tag          handle_tag;

  public:

    typedef viennagrid::config::result_of::full_mesh_config< viennagrid::tetrahedron_tag, vector_type, handle_tag >::type     type;
};

int main()
{

    //
    // typedefing and setting up the topological mesh
    //

    typedef viennagrid::mesh< my_mesh_config > mesh_type;
    mesh_type mesh;

    //
    // typedefs for the element types
    //

    typedef viennagrid::result_of::element<mesh_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::vertex_tag>::type vertex_handle_type;

    typedef viennagrid::result_of::element<mesh_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::line_tag>::type line_handle_type;

    typedef viennagrid::result_of::element<mesh_type, viennagrid::triangle_tag>::type triangle_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::triangle_tag>::type triangle_handle_type;

    typedef viennagrid::result_of::element<mesh_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
    typedef viennagrid::result_of::handle<mesh_type, viennagrid::tetrahedron_tag>::type tetrahedron_handle_type;



    //
    // typedefs for the view
    //

    // only vertices and tetrahedrons are stored in the view
    typedef viennagrid::result_of::mesh_view<mesh_type, vertex_type, tetrahedron_type>::type geometric_view_type;

    //geometric_view_type geometric_view( viennagrid::make_topologic_view<topologic_view_type>( viennagrid::topology(mesh) ), mesh.get_metainfo_collection() );

    geometric_view_type geometric_view_1 = viennagrid::make_view(mesh);
    geometric_view_type geometric_view_2 = viennagrid::make_view(mesh);



    //
    // Adding a tetrahedron
    //

    std::vector<vertex_handle_type> handles(4);

    // creates four vertices within the mesh, vh is short vor vertex handle
    // make_element is responsible for resizing all meta-info container which are associated with vertex_type
    vertex_handle_type vh0 = viennagrid::make_vertex( geometric_view_1 );
    vertex_handle_type vh1 = viennagrid::make_vertex( geometric_view_1 );
    vertex_handle_type vh2 = viennagrid::make_vertex( geometric_view_1 );
    vertex_handle_type vh3 = viennagrid::make_vertex( geometric_view_1 );

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
    // is equivalent to viennagrid::look_up<vector_type>(mesh, vhX)
    viennagrid::point(geometric_view_1, vh0) = p0;
    viennagrid::point(geometric_view_1, vh1) = p1;
    viennagrid::point(geometric_view_1, vh2) = p2;
    viennagrid::point(geometric_view_1, vh3) = p3;

    // pushing a tetrahedron to mesh_view_1
    handles[0] = vh0;
    handles[1] = vh1;
    handles[2] = vh2;
    handles[3] = vh3;

    // creates the tetrahedron within the mesh, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( geometric_view_1, handles.begin(), handles.end() );




    // creates four vertices within the mesh, vh is short vor vertex handle
    // make_element is responsible for resizing all meta-info container which are associated with vertex_type
    vertex_handle_type vh4 = viennagrid::make_vertex( geometric_view_2 );
    vertex_handle_type vh5 = viennagrid::make_vertex( geometric_view_2 );
    vertex_handle_type vh6 = viennagrid::make_vertex( geometric_view_2 );
    vertex_handle_type vh7 = viennagrid::make_vertex( geometric_view_2 );

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
    // is equivalent to viennagrid::look_up<vector_type>(mesh, vhX)
    viennagrid::point(geometric_view_2, vh4) = p4;
    viennagrid::point(geometric_view_2, vh5) = p5;
    viennagrid::point(geometric_view_2, vh6) = p6;
    viennagrid::point(geometric_view_2, vh7) = p7;

    // pushing a tetrahedron to mesh_view_1
    handles[0] = vh4;
    handles[1] = vh5;
    handles[2] = vh6;
    handles[3] = vh7;

    // creates the tetrahedron within the mesh, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( geometric_view_2, handles.begin(), handles.end() );


    //
    // output
    //

    cout << "All tetrahedrons of the mesh" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(mesh).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(mesh).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the geometric_view_1" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(geometric_view_1).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(geometric_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the geometric_view_2" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(geometric_view_2).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(geometric_view_2).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;




    //
    // geometric iteration
    //

    // iterating over all vertices and piping out the point information
    typedef viennagrid::result_of::const_element_range<mesh_type, viennagrid::vertex_tag>::type mesh_vertex_range;
    typedef viennagrid::result_of::iterator<mesh_vertex_range>::type mesh_vertex_iterator;

    cout << "All vertices of the geometric_view_1 with geometric info" << endl;
    mesh_vertex_range mesh_vtx_range = viennagrid::elements(mesh);
    for (mesh_vertex_iterator it = mesh_vtx_range.begin(); it != mesh_vtx_range.end(); ++it)
        cout << *it << " geometric information: " << viennagrid::point( mesh, *it ) << endl;
    cout << endl;




    typedef viennagrid::result_of::const_element_range<geometric_view_type, viennagrid::vertex_tag>::type view_vertex_range;
    typedef viennagrid::result_of::const_iterator<view_vertex_range>::type view_vertex_iterator;

    // iterating over all vertices and piping out the point information
    cout << "All vertices of the geometric_view_1 with geometric info" << endl;
    view_vertex_range view_vtx_range_1 = viennagrid::elements(geometric_view_1);
    for (view_vertex_iterator it = view_vtx_range_1.begin(); it != view_vtx_range_1.end(); ++it)
        cout << *it << " geometric information: " << viennagrid::point( mesh, *it ) << endl;
    cout << endl;

    // iterating over all vertices and piping out the point information
    cout << "All vertices of the geometric_view_2 with geometric info" << endl;
    view_vertex_range view_vtx_range_2 = viennagrid::elements(geometric_view_2);
    for (view_vertex_iterator it = view_vtx_range_2.begin(); it != view_vtx_range_2.end(); ++it)
        cout << *it << " geometric information: " << viennagrid::point( mesh, *it ) << endl;
    cout << endl;


    return 0;
}


