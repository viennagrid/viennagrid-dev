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


#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/element/element_config.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/generic_config.hpp"




namespace viennagrid
{

    namespace result_of
    {
        template<typename domain_config, typename typemap>
        struct continuous_id_generator_config_from_domain_config_helper;
        
        
        template<typename domain_config, typename element_tag, typename element_config, typename tail>
        struct continuous_id_generator_config_from_domain_config_helper< domain_config, viennameta::typelist_t<viennameta::static_pair<element_tag, element_config>, tail> >
        {
            typedef typename viennameta::typemap::result_of::insert<
                typename continuous_id_generator_config_from_domain_config_helper<domain_config, tail>::type,
                viennameta::static_pair<
                    typename viennagrid::result_of::element<domain_config, element_tag>::type,
                    typename viennagrid::result_of::element_id_type<domain_config, element_tag>::type
                >
            >::type type;
        };
        
        template<typename domain_config>
        struct continuous_id_generator_config_from_domain_config_helper<domain_config, viennameta::null_type>
        {
            typedef viennameta::null_type type;
        };
        
        
        template<typename domain_config>
        struct continuous_id_generator_config_from_domain_config
        {
            typedef typename continuous_id_generator_config_from_domain_config_helper<domain_config, domain_config>::type type;
        };
        
        
    }
    
    
}



namespace viennagrid
{

    template<typename domain_config_>
    class topological_domain_t
    {
    public:
        typedef domain_config_ domain_config;
        typedef typename viennagrid::result_of::element_container_typelist<domain_config>::type element_container_typelist;
        typedef typename viennagrid::storage::result_of::collection< element_container_typelist >::type domain_container_collection_type;
        
        typedef typename viennagrid::result_of::continuous_id_generator_config_from_domain_config<domain_config>::type id_generator_config;
        typedef typename viennagrid::storage::result_of::continuous_id_generator< id_generator_config >::type id_generator_type;
        typedef typename viennagrid::storage::result_of::physical_inserter<domain_container_collection_type, id_generator_type&>::type inserter_type;
        
        
        topological_domain_t() : id_generator(), container_collection(), inserter(container_collection, id_generator) {}
        
        
        
        
        domain_container_collection_type & get_container_collection() { return container_collection; }
        const domain_container_collection_type & get_container_collection() const { return container_collection; }
        
        inserter_type & get_inserter() { return inserter; }
        
    protected:
        id_generator_type id_generator;
        domain_container_collection_type container_collection;
        inserter_type inserter;
    };

}


// namespace viennagrid
// {
//     
//     template<typename geometric_config_>
//     class geometric_container
//     {
//         
//     };
//     
// }



namespace viennagrid
{
    
    namespace result_of
    {
        
        template<typename domain_config, typename element_tag>
        struct element< topological_domain_t<domain_config>, element_tag >
        {
            typedef typename element<domain_config, element_tag>::type type;
        };
        
        template<typename domain_config, long dim>
        struct ncell< topological_domain_t<domain_config>, dim >
        {
            typedef typename ncell<domain_config, dim>::type type;
        };
        
        template<typename domain_config, long dim>
        struct ncell_range< topological_domain_t<domain_config>, dim >
        {
            typedef typename viennagrid::storage::result_of::container_of<
                typename topological_domain_t<domain_config>::domain_container_collection_type,
                typename ncell<topological_domain_t<domain_config>, dim>::type
            >::type type;
        };
        
        template<typename domain_config, long dim>
        struct const_ncell_range< topological_domain_t<domain_config>, dim >
        {
            typedef const typename viennagrid::storage::result_of::container_of<
                typename topological_domain_t<domain_config>::domain_container_collection_type,
                typename ncell<topological_domain_t<domain_config>, dim>::type
            >::type type;
        };

        
    }
    
    template<long dim, typename domain_config>
    typename result_of::ncell_range<topological_domain_t<domain_config>, dim>::type & ncells(topological_domain_t<domain_config> & domain)
    {
        typedef typename result_of::ncell<topological_domain_t<domain_config>, dim>::type element_type;
        return viennagrid::storage::container_collection::get<element_type>(domain.get_container_collection());
    }
    
    template<long dim, typename domain_config>
    typename result_of::const_ncell_range<topological_domain_t<domain_config>, dim>::type & ncells(const topological_domain_t<domain_config> & domain)
    {
        typedef typename result_of::ncell<topological_domain_t<domain_config>, dim>::type element_type;
        return viennagrid::storage::container_collection::get<element_type>(domain.get_container_collection());
    }
    
    
    template<typename element_type, typename domain_config>
    element_type create_element( topological_domain_t<domain_config> & domain )
    {
        element_type element(domain.get_container_collection());
        return element;
    }
    
    template<typename domain_config, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    void push_element( topological_domain_t<domain_config> & domain, const viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        domain.get_inserter()(element);
    }
        
    
    template<typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    void set_vertex(
            viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element,
            typename viennagrid::result_of::ncell_hook< viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type>, 0 >::type vertex_hook,
            unsigned int pos
        )
    {
        element.container( viennagrid::dimension_tag<0>() ).set_hook( vertex_hook, pos );
    }
    
    template<typename domain_config>
    typename result_of::element_hook<domain_config, viennagrid::vertex_tag>::type get_vertex_hook( topological_domain_t<domain_config> & domain, unsigned int pos )
    {
        typedef typename result_of::element<domain_config, viennagrid::vertex_tag>::type vertex_type;
        return viennagrid::storage::container_collection::get<vertex_type>( domain.get_container_collection() ).hook_at(pos);
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
    // Generates the same config as show above in comment
    //
    
    typedef viennagrid::result_of::default_config<viennagrid::tetrahedron_tag, hook_tag>::type config;
    
    
    //
    // typedefing and setting up the topological domain
    //
    typedef viennagrid::topological_domain_t<config> domain_type;
    domain_type domain;
    
    
    //
    // typedefs for the element types
    //
    typedef viennagrid::result_of::ncell<domain_type, 0>::type vertex_type;
    typedef viennagrid::result_of::ncell<domain_type, 1>::type line_type;
    typedef viennagrid::result_of::ncell<domain_type, 2>::type triangle_type;
    typedef viennagrid::result_of::ncell<domain_type, 3>::type tetrahedron_type;
    

    //
    // Adding a tetrahedron
    //

    vertex_type v0;
    vertex_type v1;
    vertex_type v2;
    vertex_type v3;
    
    viennagrid::push_element(domain, v0);
    viennagrid::push_element(domain, v1);
    viennagrid::push_element(domain, v2);
    viennagrid::push_element(domain, v3);
    
    
    tetrahedron_type tet = viennagrid::create_element<tetrahedron_type>(domain);
    
    viennagrid::set_vertex( tet, viennagrid::get_vertex_hook(domain, 0), 0 );
    viennagrid::set_vertex( tet, viennagrid::get_vertex_hook(domain, 1), 1 );
    viennagrid::set_vertex( tet, viennagrid::get_vertex_hook(domain, 2), 2 );
    viennagrid::set_vertex( tet, viennagrid::get_vertex_hook(domain, 3), 3 );
    
    viennagrid::push_element(domain, tet);
    
    
    
    //
    // display the domain content
    //
    
    cout << "All vertices of the domain" << endl;
    std::copy( viennagrid::ncells<0>(domain).begin(), viennagrid::ncells<0>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    cout << "All lines of the domain" << endl;
    std::copy( viennagrid::ncells<1>(domain).begin(), viennagrid::ncells<1>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    cout << "All triangles of the domain" << endl;
    std::copy( viennagrid::ncells<2>(domain).begin(), viennagrid::ncells<2>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetraherons of the domain" << endl;
    std::copy( viennagrid::ncells<3>(domain).begin(), viennagrid::ncells<3>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    
    
    
    //
    // doing some boundary cell iteration
    //
    
    const tetrahedron_type & test_tet = *viennagrid::ncells<3>(domain).begin();
    
    typedef viennagrid::result_of::const_ncell_range<tetrahedron_type, 2>::type tetrahedron_triangle_range;
    typedef viennagrid::result_of::const_iterator<tetrahedron_triangle_range>::type tetrahedron_triangle_iterator;
    
    cout << "All triangles of the first tetdrahedron in the domain" << endl;
    tetrahedron_triangle_range & tri_range = viennagrid::ncells<2>(test_tet);
    for (tetrahedron_triangle_iterator it = tri_range.begin(); it != tri_range.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( tri_range.begin(), tri_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    
    const triangle_type & test_tri = *viennagrid::ncells<2>(domain).begin();
    
    typedef viennagrid::result_of::const_ncell_range<triangle_type, 1>::type triangle_line_range;
    typedef viennagrid::result_of::const_iterator<triangle_line_range>::type triangle_line_iterator;

    cout << "All lines of the first triangle in the domain" << endl;
    triangle_line_range & lin_range = viennagrid::ncells<1>(test_tri);
    for (triangle_line_iterator it = lin_range.begin(); it != lin_range.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( lin_range.begin(), lin_range.end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;

    
    
    return 0;
}


