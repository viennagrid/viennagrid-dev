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
#include "viennagrid/detail/element_orientation.hpp"
#include "viennagrid/detail/boundary_ncell_layer.hpp"
#include "viennagrid/point.hpp"




namespace viennagrid
{
    
    struct element_id_type_tag {};
    struct element_container_tag {};
    struct element_boundary_storage_layout_tag {};
    
    
    
    
    namespace viennagrid_ng
    {
        
        template <typename container_type>
        class element_orientation : public container_type
        {
            typedef typename container_type::value_type permutator_type;
            typedef typename container_type::size_type size_type;
            
            public:
            void setDefaultOrientation()
            {
                unsigned int index = 0;
                for (typename container_type::iterator it = container_type::begin(); it != container_type::end(); ++it, ++index)
                    *it = static_cast<permutator_type>(index);
            };

            size_type operator()(size_type in) const { return static_cast<size_type>( (*this)[in] ); }

            void setPermutation(size_type index, size_type mappedTo) { (*this)[index] = static_cast<permutator_type>(mappedTo); };

            void print() const
            {
                unsigned int index = 0;
                for (typename container_type::const_iterator it = container_type::begin(); it != container_type::end(); ++it, ++index)
                    std::cout << index << "->" << *it << ",";
                std::cout << std::endl;
            }
        };
        
    }
    
    
    namespace result_of
    {
        
        
        
        
        

        
        
        
        
        
        template<typename storage_layout_tag, long num>
        struct boundary_cell_container_tag
        {};
        
        template<long num>
        struct boundary_cell_container_tag<viennagrid::static_layout_tag, num>
        {
            typedef viennagrid::storage::static_array_tag<num> type;
        };
        
        template<long num>
        struct boundary_cell_container_tag<viennagrid::dynamic_layout_tag, num>
        {
            typedef viennagrid::storage::std_vector_tag type;
        };
        
        
        
        //
        // Generates an element type
        //
        template<typename config__, typename element_tag__>
        struct element;
        
        template<typename domain_config, typename element_tag>
        struct is_element_present
        {
            static const bool value =
                !viennameta::_equal<
                    typename viennameta::typemap::result_of::find<
                        domain_config,
                        element_tag
                    >::type,
                    viennameta::not_found
                >::value;
        };
        
        
        template<typename domain_config, typename element_tag>
        struct element_id_type
        {
            typedef typename viennameta::typemap::result_of::find< domain_config, element_tag >::type::second element_config;
            typedef typename viennameta::typemap::result_of::find< element_config, element_id_type_tag >::type::second type;
        };
        
        template<typename domain_config, typename element_tag>
        struct element_container_tag
        {
            typedef typename viennameta::typemap::result_of::find< domain_config, element_tag >::type::second element_config;
            typedef typename viennameta::typemap::result_of::find< element_config, viennagrid::element_container_tag >::type::second type;
        };
        

        template<typename domain_config, typename element_tag, typename boundary_cell_tag>
        struct boundary_storage_layout
        {
            typedef typename viennameta::typemap::result_of::find< domain_config, element_tag >::type::second element_config;
            typedef typename viennameta::typemap::result_of::find< element_config, viennagrid::element_boundary_storage_layout_tag >::type::second element_storage_layout_config;
            typedef typename viennameta::typemap::result_of::find< element_storage_layout_config, boundary_cell_tag >::type::second boundary_cell_storage_layout;
            
            typedef typename boundary_cell_storage_layout::first storage_tag;
            typedef typename boundary_cell_storage_layout::second orientation_tag;
        };
        
        
        
        
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__, bool is_present = is_element_present<domain_config__, boundary_cell_tag__>::value >
        struct has_boundary_cells_helper
        {
            static const bool value = 
                !viennameta::_equal<
                    typename boundary_storage_layout<domain_config__, element_tag__, boundary_cell_tag__>::storage_tag,
                    viennagrid::no_handling_tag
                >::value;
        };
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__>
        struct has_boundary_cells_helper<domain_config__, element_tag__, boundary_cell_tag__, false>
        {
            static const bool value = false;
        };
        
        
        template<typename domain_config__, typename element_tag__, typename boundary_cell_tag__>
        struct has_boundary_cells
        {
            static const bool is_present = is_element_present<domain_config__, boundary_cell_tag__>::value;
                        
            static const bool value = has_boundary_cells_helper<domain_config__, element_tag__, boundary_cell_tag__, is_present>::value;
        };
        
        
        
        template<typename config, typename element_tag, typename boundary_cell_tag, bool is_present>
        struct element_boundary_cell_container_helper
        {
            typedef typename element<config, boundary_cell_tag>::type boundary_cell_type;
            
            typedef typename viennagrid::storage::result_of::container<
                boundary_cell_type,                                         // the 'value_type', i.e. vertices     
                typename element_container_tag<config, boundary_cell_tag>::type
            >::type boundary_cell_container;
            
            
            
            typedef typename boundary_cell_container_tag<
                    typename viennagrid::topology::boundary_cells< element_tag, boundary_cell_tag>::layout_tag,
                    viennagrid::topology::boundary_cells< element_tag, boundary_cell_tag>::num
                >::type container_tag;
            

            typedef typename viennagrid::storage::result_of::view<
                    boundary_cell_container,
                    container_tag
                >::type
            boundary_cell_view;
            
            typedef viennameta::null_type facet_orientation_container_type;
            
//             typedef typename viennagrid::result_of::permutator_type<
//                 viennagrid::topology::boundary_cells<
//                     boundary_cell_tag,
//                     typename boundary_cell_tag::facet_tag
//                 >::num
//             >::type permutator_type;
//             
//             typedef typename boundary_cell_container_tag<
//                     typename viennagrid::topology::boundary_cells< element_tag, boundary_cell_tag>::layout_tag,
//                     viennagrid::topology::boundary_cells< element_tag, boundary_cell_tag>::num
//                 >::type orientation_container_tag;
//             
//             
//             typedef typename viennagrid::storage::result_of::container<permutator_type, container_tag>::type orientation_container_type;
//             typedef viennagrid::viennagrid_ng::element_orientation<orientation_container_type> facet_orientation_container_type;
            //typedef viennagrid::storage::result_of::container<  >
            //typedef element_orientation<> facet_orientation_container_type;
            
            typedef viennameta::static_pair<
                            boundary_cell_view,
                            facet_orientation_container_type
                        > type;
        };
        
        template<typename config__, typename element_tag__, typename boundary_cell_tag__>
        struct element_boundary_cell_container_helper<config__, element_tag__, boundary_cell_tag__, false>
        {
            typedef viennameta::null_type type;
        };
        
        
        template<typename config, typename element_tag, typename boundary_cell_tag>
        struct element_boundary_cell_container
        {
            static const bool is_present = has_boundary_cells<config, element_tag, boundary_cell_tag>::value;
            
            typedef typename element_boundary_cell_container_helper<config, element_tag, boundary_cell_tag, is_present>::type type;
        };
        
        
        
        template<typename config, typename element_tag, typename boundary_cell_tag>
        struct element_boundary_cell_container_typelist
        {
            typedef typename boundary_cell_tag::facet_tag facet_tag;
            
            typedef typename element_boundary_cell_container<config, element_tag, facet_tag>::type boundary_cell_layer_containers;
            typedef typename viennameta::typelist::result_of::push_back<
                typename element_boundary_cell_container_typelist< config, element_tag, facet_tag >::type,
                boundary_cell_layer_containers
            >::type type;
        };
        
        template<typename config, typename element_tag>
        struct element_boundary_cell_container_typelist<config, element_tag, viennagrid::vertex_tag>
        {
            typedef viennameta::null_type type;
        };
        
        

        template<typename config__, typename element_tag__>
        struct element
        {
            typedef typename element_id_type<config__, element_tag__>::type id_type;
            
            typedef typename element_boundary_cell_container_typelist<config__, element_tag__, element_tag__>::type container_typelist;
            
            typedef viennagrid::viennagrid_ng::element_t<element_tag__, container_typelist, id_type> type;
        };
        
       
        
        //
        // generates a container for a specified element_tag for the domain container collection
        //
        template<typename config, typename element_tag>
        struct element_container_ng
        {
            typedef typename element<config, element_tag>::type element_type;
            typedef typename viennagrid::storage::result_of::container<element_type, typename element_container_tag<config, element_tag>::type >::type type;
        };
        
        
        //
        // generates the container typelist for the domain container collection
        //
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
    
    
    
    //
    // A full config for tetrahedrons
    //
    
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
    
    
    
        
    //
    // typedefs for the element types
    //
    typedef viennagrid::result_of::element<config, viennagrid::vertex_tag>::type vertex_type;        
    typedef viennagrid::result_of::element<config, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::element<config, viennagrid::triangle_tag>::type triangle_type;
    typedef viennagrid::result_of::element<config, viennagrid::tetrahedron_tag>::type tetrahedron_type;
    
    vertex_type::print_class();
    line_type::print_class();
    triangle_type::print_class();
    tetrahedron_type::print_class();
    cout << endl;
    
    
    
    
//     cout << viennagrid::result_of::has_boundary_cells<config, viennagrid::tetrahedron_tag, viennagrid::triangle_tag>::value << endl;
//     
//     cout << viennagrid::result_of::is_element_present<config, viennagrid::triangle_tag>::value << endl;
    
    
//     cout << viennagrid::result_of::has_boundary_cells<config, viennagrid::tetrahedron_tag, viennagrid::line_tag>::value << endl;
//     
//     cout << typeid(viennagrid::result_of::boundary_storage_layout<config, viennagrid::tetrahedron_tag, viennagrid::line_tag>::boundary_cell_storage_layout).name() << endl;
    
//     cout << typeid(vertex_type).name() << endl;
//     cout << typeid(line_type).name() << endl;
//     cout << typeid(triangle_type).name() << endl;
//     cout << typeid(tetrahedron_type).name() << endl;
    
    

    //
    // setting up the domain
    //
    
    typedef viennagrid::result_of::element_container_typelist<config>::type element_container_typelist;
    
    typedef viennagrid::storage::result_of::collection< element_container_typelist >::type domain_container_collection_type;
    domain_container_collection_type domain_container_collection;
    
    
    typedef viennagrid::storage::result_of::continuous_id_generator< viennagrid::storage::container_collection::result_of::value_typelist<domain_container_collection_type>::type >::type id_generator_type;
    id_generator_type id_generator;
    
    typedef viennagrid::storage::result_of::physical_inserter<domain_container_collection_type, id_generator_type&>::type inserter_type;
    inserter_type inserter(domain_container_collection, id_generator);
    

    //
    // Adding a tetrahedron
    //

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
    
    
    //
    // display the domain content
    //
    
    std::copy( viennagrid::storage::container_collection::get<vertex_type>(domain_container_collection).begin(), viennagrid::storage::container_collection::get<vertex_type>(domain_container_collection).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<line_type>(domain_container_collection).begin(), viennagrid::storage::container_collection::get<line_type>(domain_container_collection).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<triangle_type>(domain_container_collection).begin(), viennagrid::storage::container_collection::get<triangle_type>(domain_container_collection).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<tetrahedron_type>(domain_container_collection).begin(), viennagrid::storage::container_collection::get<tetrahedron_type>(domain_container_collection).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    
    
    

    return 0;
}


