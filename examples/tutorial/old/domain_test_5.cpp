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
    // typedefing and setting up the topological mesh
    //

    //typedef viennagrid::mesh_t<my_mesh_config> mesh_type;
    typedef viennagrid::tetrahedral_3d_mesh mesh_type;
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

    // only tetrahedrons are stored in the view
    // typedefing the view
    typedef viennagrid::result_of::mesh_view<mesh_type, tetrahedron_type>::type mesh_view_type;

    // creating two views of the mesh
    mesh_view_type mesh_view_1 = viennagrid::make_view(mesh);
    mesh_view_type mesh_view_2 = viennagrid::make_view(mesh);


    //
    // filling the view/mesh
    //

    std::vector<vertex_handle_type> handles(4);

    // pushing a tetrahedron to mesh_view_1
    handles[0] = viennagrid::make_vertex( mesh );
    handles[1] = viennagrid::make_vertex( mesh );
    handles[2] = viennagrid::make_vertex( mesh );
    handles[3] = viennagrid::make_vertex( mesh );

    // creates the tetrahedron within the mesh, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( mesh_view_1, handles.begin(), handles.end() );


    // pushing a tetrahedron to mesh_view_2
    handles[0] = viennagrid::make_vertex( mesh );
    handles[1] = viennagrid::make_vertex( mesh );
    handles[2] = viennagrid::make_vertex( mesh );
    handles[3] = viennagrid::make_vertex( mesh );

    // creates the tetrahedron within the mesh, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( mesh_view_2, handles.begin(), handles.end() );






    //
    // output
    //

    cout << "All tetrahedrons of the mesh" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(mesh).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(mesh).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the mesh view 1" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_1).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the mesh view 2" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_2).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_2).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;




    cout << "--------------" << endl;
    cout << " inserting to mesh_view_view" << endl;
    cout << "--------------" << endl;


    typedef viennagrid::result_of::mesh_view< mesh_view_type, tetrahedron_type >::type mesh_view_view_type;

    mesh_view_view_type mesh_view_view_1 = viennagrid::make_view(mesh_view_1);

    // pushing a tetrahedron to mesh_view_1
    handles[0] = viennagrid::make_vertex( mesh );
    handles[1] = viennagrid::make_vertex( mesh );
    handles[2] = viennagrid::make_vertex( mesh );
    handles[3] = viennagrid::make_vertex( mesh );

    // creates the tetrahedron within the mesh, all boundary cell generation is done here implicit
    viennagrid::make_element<tetrahedron_type>( mesh_view_view_1, handles.begin(), handles.end() );



    //
    // output
    //

    cout << "All tetrahedrons of the mesh" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(mesh).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(mesh).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the mesh view 1" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_1).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the mesh view 2" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_2).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_2).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All tetrahedrons of the mesh view view" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_view_1).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(mesh_view_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;



    cout << "All tetrahedrons of the mesh using for_each" << endl;
    viennagrid::for_each<3>( mesh, element_output_functor() );



    return 0;
}


