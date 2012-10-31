#include <iostream>
#include <typeinfo>

#include <map>
#include <boost/concept_check.hpp>

using std::cout;
using std::endl;


#include "viennagrid/forwards.h"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"


#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/detail/element_key.hpp"
#include "viennagrid/detail/boundary_ncell_layer.hpp"
#include "viennagrid/point.hpp"




namespace viennagrid
{
    
    struct element_id_type_tag {};
    struct element_container_tag {};
    struct element_boundary_storage_layout_tag {};
    
    
    
    namespace result_of
    {
        
        template<typename config__, typename element_tag__, typename boundary_cell_tag__ = element_tag__>
        struct element;
        
        
        template<typename complete_config, typename element_tag>
        struct element_config
        {
            typedef typename viennameta::typemap::result_of::find< complete_config, element_tag >::type::second config;
            
            typedef typename viennameta::typemap::result_of::find< config, element_id_type_tag >::type::second id_type;
            typedef typename viennameta::typemap::result_of::find< config, element_container_tag >::type::second element_container_tag;
            typedef typename viennameta::typemap::result_of::find< config, element_boundary_storage_layout_tag >::type::second element_boundary_storage_layout;
            
            static const bool is_present = !viennameta::_equal<config, viennameta::null_type>::value;
        };
        
        
        
        template<typename complete_config, typename element_tag, typename boundary_cell_tag>
        struct boundary_storage_layout
        {
            typedef typename viennameta::typemap::result_of::find< complete_config, element_tag >::type::second config;
            typedef typename viennameta::typemap::result_of::find< config, viennagrid::element_boundary_storage_layout_tag >::type::second element_storage_config;
            typedef typename viennameta::typemap::result_of::find< element_storage_config, boundary_cell_tag >::type::second boundary_cell_storage_layout;
            
            typedef typename boundary_cell_storage_layout::first storage_tag;
            typedef typename boundary_cell_storage_layout::second orientation_tag;
        };


        
        
//         template<typename >
//         struct element_container_tag
//         {
//             
//         };
        
        template<typename config, typename element_tag, typename boundary_ncell_tag>
        struct boundary_ncell_container
        {
            typedef typename element<config, boundary_ncell_tag>::type boundary_cell_type;
            
            
            typedef typename viennagrid::storage::result_of::hooked_container<
                boundary_cell_type,                                         // the 'value_type', i.e. vertices     
                typename element_config<config, boundary_ncell_tag>::element_container_tag
            >::type boundary_cell_container;
            
            
            typedef typename viennagrid::storage::result_of::view<
                    boundary_cell_container,
                    viennagrid::storage::hooked_container_tag<
                        viennagrid::storage::static_array_tag<
                            viennagrid::topology::boundary_cells< element_tag, boundary_ncell_tag >::num
                        >,
                        viennagrid::storage::no_hook_tag
                    >
                >::type
            boundary_cell_view;
            
            typedef typename viennameta::_if<
                viennameta::_equal< typename boundary_storage_layout<config, element_tag, boundary_ncell_tag>::storage_tag, viennagrid::full_handling_tag >::value,
                boundary_cell_view,
                viennameta::null_type
            >::type type;
            
        };
        
        
        
        template<typename config__, typename element_tag__, typename boundary_cell_tag__>
        struct element
        {
            typedef element_tag__ element_tag;
            typedef boundary_cell_tag__ boundary_cell_tag;
            typedef config__ config;
            
            typedef typename boundary_cell_tag::facet_tag facet_tag;
            typedef typename element_config<config, boundary_cell_tag>::id_type id_type;
            
            typedef typename boundary_ncell_container<config, element_tag, facet_tag>::type facet_element_container_type;
            typedef viennameta::null_type facet_orientation_container_type;
            
            
            typedef typename viennameta::typelist::result_of::push_back<
                typename element< config, element_tag, facet_tag >::boundary_ncell_container_list,
                viennameta::static_pair<
                    facet_element_container_type,
                    facet_orientation_container_type
                >
            >::type boundary_ncell_container_list;
            
            
            typedef viennagrid::viennagrid_ng::element_t<element_tag, boundary_ncell_container_list, id_type> type;
        };
        
        template<typename config__, typename element_tag__>
        struct element<config__, element_tag__, viennagrid::vertex_tag>
        {
            typedef element_tag__ element_tag;
            typedef viennagrid::vertex_tag boundary_cell_tag;
            typedef config__ config;
            
            typedef typename element_config<config, boundary_cell_tag>::id_type id_type;
            
            typedef viennameta::null_type boundary_ncell_container_list;
            
            typedef viennagrid::viennagrid_ng::element_t<viennagrid::vertex_tag, viennameta::null_type, id_type> type;
        };
        
        
        template<typename config, typename element_tag>
        struct element_container_ng
        {
            typedef typename element<config, element_tag, element_tag>::type element_type;
            typedef typename viennagrid::storage::result_of::hooked_container<element_type, typename element_config<config, element_tag>::element_container_tag >::type type;
        };
        
        
        template<typename config, typename cur_config = config>
        struct element_container_typelist;
        
        template<typename config, typename key, typename value, typename tail>
        struct element_container_typelist< config, viennameta::typelist_t< viennameta::static_pair<key, value>, tail > >
        {
            typedef viennameta::typelist_t<
                typename element_container_ng<config, key>::type,
                typename element_container_typelist<config, tail>::type
            > type;
        };
        
        template<typename config>
        struct element_container_typelist<config, viennameta::null_type>
        {
            typedef viennameta::null_type type;
        };
        
        
    }
    
    

    
    
}



    


int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    //typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    typedef viennagrid::storage::id_hook_tag<int> hook_tag;
    
    
    
    
    
    typedef viennameta::make_typemap<    

        viennagrid::tetrahedron_tag,
        viennameta::make_typemap<
            viennagrid::element_id_type_tag,
            int,
            
            viennagrid::element_container_tag,
            viennagrid::storage::hooked_container_tag<           
                viennagrid::storage::std_deque_tag,
                hook_tag
            >,
            
            viennagrid::element_boundary_storage_layout_tag,
            viennameta::make_typemap<
                viennagrid::vertex_tag, viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::full_handling_tag >,
                viennagrid::line_tag, viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::full_handling_tag >,
                viennagrid::triangle_tag, viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::full_handling_tag >
            >::type
        >::type,
        
        viennagrid::triangle_tag,
        viennameta::make_typemap<
            viennagrid::element_id_type_tag,
            int,
        
            viennagrid::element_container_tag,
            viennagrid::storage::hooked_container_tag<           
                viennagrid::storage::hidden_key_map_tag< viennagrid::storage::hooked_element_key_tag<int> >,
                hook_tag
            >,
            
            viennagrid::element_boundary_storage_layout_tag,
            viennameta::make_typemap<
                viennagrid::vertex_tag, viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::full_handling_tag >,
                viennagrid::line_tag, viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::full_handling_tag >
            >::type
        >::type,
        
        viennagrid::line_tag,
        viennameta::make_typemap<
            viennagrid::element_id_type_tag,
            int,
        
            viennagrid::element_container_tag,
            viennagrid::storage::hooked_container_tag<           
                viennagrid::storage::hidden_key_map_tag< viennagrid::storage::hooked_element_key_tag<int> >,
                hook_tag
            >,
            
            viennagrid::element_boundary_storage_layout_tag,
            viennameta::make_typemap<
                viennagrid::vertex_tag, viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::full_handling_tag >
            >::type
        >::type,
        
        viennagrid::vertex_tag,
        viennameta::make_typemap<
            viennagrid::element_id_type_tag,
            int,
        
            viennagrid::element_container_tag,
            viennagrid::storage::hooked_container_tag<           
                viennagrid::storage::std_deque_tag,
                hook_tag
            >,
            
            viennagrid::element_boundary_storage_layout_tag,
            viennameta::make_typemap<
            >::type
        >::type

    >::type config;
    
    
    
        
    //typedef viennagrid::result_of::element<viennagrid::vertex_tag, viennagrid::vertex_tag, config>::type vertex_type;
    typedef viennagrid::result_of::element<config, viennagrid::vertex_tag>::type vertex_type;
    //cout << typeid(vertex_type).name() << endl << endl;
    
    typedef viennagrid::result_of::element<config, viennagrid::line_tag>::type line_type;
    //cout << typeid(line_type).name() << endl << endl;
    
    typedef viennagrid::result_of::element<config, viennagrid::triangle_tag>::type triangle_type;
    //cout << typeid(triangle_type).name() << endl << endl;
    
    typedef viennagrid::result_of::element<config, viennagrid::tetrahedron_tag>::type tetrahedron_type;
    //cout << typeid(tetrahedron_type).name() << endl << endl;
    

    
    
    typedef viennagrid::result_of::element_container_typelist<config>::type element_container_typelist;
    
    typedef viennagrid::storage::result_of::collection< element_container_typelist >::type domain_container_collection_type;
    domain_container_collection_type domain_container_collection;
    
    
    typedef viennagrid::storage::result_of::continuous_id_generator< viennagrid::storage::container_collection::result_of::value_typelist<domain_container_collection_type>::type >::type id_generator_type;
    id_generator_type id_generator;
    
    typedef viennagrid::storage::result_of::physical_inserter<domain_container_collection_type, id_generator_type&>::type inserter_type;
    inserter_type inserter(domain_container_collection, id_generator);
    

    
    vertex_type v0;
    vertex_type v1;
    vertex_type v2;
    vertex_type v3;
    
    inserter(v0);
    inserter(v1);
    inserter(v2);
    inserter(v3);
    
    
    tetrahedron_type tet;
    tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::container_collection::get<vertex_type>(domain_container_collection).hook_at(0), 0);
    tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::container_collection::get<vertex_type>(domain_container_collection).hook_at(1), 1);
    tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::container_collection::get<vertex_type>(domain_container_collection).hook_at(2), 2);
    tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::container_collection::get<vertex_type>(domain_container_collection).hook_at(3), 3);
    
    inserter(tet);
    
    
    std::copy( viennagrid::storage::container_collection::get<vertex_type>(domain_container_collection).begin(), viennagrid::storage::container_collection::get<vertex_type>(domain_container_collection).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<line_type>(domain_container_collection).begin(), viennagrid::storage::container_collection::get<line_type>(domain_container_collection).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<triangle_type>(domain_container_collection).begin(), viennagrid::storage::container_collection::get<triangle_type>(domain_container_collection).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<tetrahedron_type>(domain_container_collection).begin(), viennagrid::storage::container_collection::get<tetrahedron_type>(domain_container_collection).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    

    
    //cout << typeid( viennagrid::result_of::element_config<config, viennagrid::vertex_tag>::id_type ).name() << endl;
    
    
    

    return 0;
}


