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



#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;



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
                    typename viennagrid::result_of::element_id_tag<domain_config, element_tag>::type
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
        typedef typename viennagrid::result_of::element_container_typemap<domain_config>::type element_container_typelist;
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
            typedef viennagrid::storage::container_range_wrapper<
                typename viennagrid::storage::result_of::container_of<
                    typename topological_domain_t<domain_config>::domain_container_collection_type,
                    typename ncell<topological_domain_t<domain_config>, dim>::type
                >::type
            > type;
        };
        
        template<typename domain_config, long dim>
        struct const_ncell_range< topological_domain_t<domain_config>, dim >
        {
            typedef viennagrid::storage::container_range_wrapper<
                const typename viennagrid::storage::result_of::container_of<
                    typename topological_domain_t<domain_config>::domain_container_collection_type,
                    typename ncell<topological_domain_t<domain_config>, dim>::type
                >::type
            > type;
        };

        
    }
    
    template<long dim, typename domain_config>
    typename result_of::ncell_range<topological_domain_t<domain_config>, dim>::type ncells(topological_domain_t<domain_config> & domain)
    {
        typedef typename result_of::ncell<topological_domain_t<domain_config>, dim>::type element_type;
        return typename result_of::ncell_range<topological_domain_t<domain_config>, dim>::type(
            viennagrid::storage::collection::get<element_type>(domain.get_container_collection()) );
    }
    
    template<long dim, typename domain_config>
    typename result_of::const_ncell_range<topological_domain_t<domain_config>, dim>::type ncells(const topological_domain_t<domain_config> & domain)
    {
        typedef const typename result_of::ncell<topological_domain_t<domain_config>, dim>::type element_type;
        return typename result_of::const_ncell_range<topological_domain_t<domain_config>, dim>::type(
            viennagrid::storage::collection::get<element_type>(domain.get_container_collection()) );
    }
    
    
    template<typename element_type, typename domain_config>
    element_type create_element( topological_domain_t<domain_config> & domain )
    {
        element_type element(domain.get_container_collection());
        return element;
    }
    
    template<typename domain_config, typename element_tag, typename boundary_cell_container_typelist, typename id_type>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename topological_domain_t<domain_config>::domain_container_collection_type,
                    viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type>
                >::type::hook_type,
                bool
            >
        push_element( topological_domain_t<domain_config> & domain, const viennagrid::element_t<element_tag, boundary_cell_container_typelist, id_type> & element)
    {
        return domain.get_inserter()(element);
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
        return viennagrid::storage::collection::get<vertex_type>( domain.get_container_collection() ).hook_at(pos);
    }
}













namespace viennagrid
{
    struct random_access_tag {};
    struct associative_access_tag {};
    
    namespace result_of
    {
        
        template<typename container_type>
        struct associative_container_value_type
        {
            typedef typename container_type::value_type type;
        };
        
        template<typename key_type, typename value_type, typename compare, typename allocator>
        struct associative_container_value_type< std::map<key_type, value_type, compare, allocator> >
        {
            typedef value_type type;
        };
        
        
        template<typename container_type>
        struct associative_container_access_tag
        {
            typedef random_access_tag type;
        };
        
        template<typename key_type, typename value_type, typename compare, typename allocator>
        struct associative_container_access_tag< std::map<key_type, value_type, compare, allocator> >
        {
            typedef associative_access_tag type;
        };
        
    }
    
    

    
    
    template<typename geometric_container_type, typename element_type>
    typename result_of::associative_container_value_type<geometric_container_type>::type & look_up( geometric_container_type & container, const element_type & element, random_access_tag )
    {
        return container[ element.id().get() ];
    }
    
    template<typename geometric_container_type, typename element_type>
    typename result_of::associative_container_value_type<geometric_container_type>::type & look_up( geometric_container_type & container, const element_type & element, associative_access_tag )
    {
        typename geometric_container_type::iterator it = container.find( element.id() );
        return it->second;
    }
    
    template<typename geometric_container_type, typename element_type>
    typename result_of::associative_container_value_type<geometric_container_type>::type & look_up( geometric_container_type & container, const element_type & element )
    {
        return look_up(container, element, typename result_of::associative_container_access_tag<geometric_container_type>::type() );
    }
    
    
    
    
    template<typename geometric_container_type, typename element_type>
    void set( geometric_container_type & container, const element_type & element, typename result_of::associative_container_value_type<geometric_container_type>::type & info, random_access_tag )
    {
        if (container.size() >= element.id().get() )
            container.resize( element.id().get()+1 );
        
        container[ element.id().get() ] = info;
    }
    
    template<typename geometric_container_type, typename element_type>
    void set( geometric_container_type & container, const element_type & element, typename result_of::associative_container_value_type<geometric_container_type>::type & info, associative_access_tag )
    {
        container.insert(
            std::make_pair( element.id(), info )
        );
    }
    
    template<typename geometric_container_type, typename element_type>
    void set( geometric_container_type & container, const element_type & element, typename result_of::associative_container_value_type<geometric_container_type>::type & info )
    {
        set(container, element, info, typename result_of::associative_container_access_tag<geometric_container_type>::type() );
    }
    
}







namespace viennagrid
{
    
    namespace result_of
    {
        
        template<typename container_collection_type, typename element_type, typename metainfo_type>
        struct info_container
        {
            typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type >::type element_container_collection_type;
            typedef typename viennagrid::storage::result_of::container_of< element_container_collection_type, metainfo_type >::type type;
        };
    }
    
    
    template< typename element_type, typename metainfo_type, typename container_collection_type >
    typename result_of::info_container<container_collection_type, element_type, metainfo_type>::type & get_info( container_collection_type & container_collection )
    {
        return viennagrid::storage::collection::get<metainfo_type>( viennagrid::storage::collection::get<element_type>(container_collection) );
    }
    
    template< typename element_type, typename metainfo_type, typename container_collection_type >
    const typename result_of::info_container<container_collection_type, element_type, metainfo_type>::type & get_info( const container_collection_type & container_collection )
    {
        return viennagrid::storage::collection::get<metainfo_type>( viennagrid::storage::collection::get<element_type>(container_collection) );
    }
}






    


int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    //typedef viennagrid::storage::id_hook_tag hook_tag;
    
    
    
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
    
    
    
    
    
    
    typedef viennameta::make_typelist<
    
    >::type metainfo_config;
    
    
    

    //
    // Adding a tetrahedron
    //

    vertex_type v0;
    vertex_type v1;
    vertex_type v2;
    vertex_type v3;
    
    typedef ublas::vector<double> vector_type;
    
    vector_type gv0(3);
    gv0[0] = 0.0; gv0[1] = 0.0; gv0[2] = 0.0;
    vector_type gv1(3);
    gv1[0] = 1.0; gv1[1] = 0.0; gv1[2] = 0.0;
    vector_type gv2(3);
    gv2[0] = 0.0; gv2[1] = 1.0; gv2[2] = 0.0;
    vector_type gv3(3);
    gv3[0] = 0.0; gv3[1] = 0.0; gv3[2] = 1.0;
    
    typedef std::vector<vector_type> vector_container_type;
    vector_container_type vector_container;
    
    viennagrid::push_element(domain, v0);
    viennagrid::push_element(domain, v1);
    viennagrid::push_element(domain, v2);
    viennagrid::push_element(domain, v3);
    
    
    viennagrid::set( vector_container, viennagrid::ncells<0>(domain)[0], gv0 );
    viennagrid::set( vector_container, viennagrid::ncells<0>(domain)[1], gv1 );
    viennagrid::set( vector_container, viennagrid::ncells<0>(domain)[2], gv2 );
    viennagrid::set( vector_container, viennagrid::ncells<0>(domain)[3], gv3 );
    
    
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
    const triangle_type & test_tri = *viennagrid::ncells<2>(domain).begin();
    
    
    
    typedef viennagrid::result_of::const_ncell_range<tetrahedron_type, 2>::type tetrahedron_triangle_range;
    typedef viennagrid::result_of::const_iterator<tetrahedron_triangle_range>::type tetrahedron_triangle_iterator;
    
    cout << "All triangles of the first tetdrahedron in the domain" << endl;
    tetrahedron_triangle_range tri_range = viennagrid::ncells<2>(test_tet);
    for (tetrahedron_triangle_iterator it = tri_range.begin(); it != tri_range.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( tri_range.begin(), tri_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    
    
    
    typedef viennagrid::result_of::const_ncell_range<triangle_type, 1>::type triangle_line_range;
    typedef viennagrid::result_of::const_iterator<triangle_line_range>::type triangle_line_iterator;

    cout << "All lines of the first triangle in the domain" << endl;
    triangle_line_range lin_range = viennagrid::ncells<1>(test_tri);
    for (triangle_line_iterator it = lin_range.begin(); it != lin_range.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( lin_range.begin(), lin_range.end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    
    
    
    //
    // geometric iteration
    //
    
    typedef viennagrid::result_of::const_ncell_range<tetrahedron_type, 0>::type tetrahedron_vertex_range;
    typedef viennagrid::result_of::const_iterator<tetrahedron_vertex_range>::type tetrahedron_vertex_iterator;
    
    cout << "All vertices of the first tetdrahedron in the domain" << endl;
    tetrahedron_vertex_range vtx_range = viennagrid::ncells<0>(test_tet);
    for (tetrahedron_vertex_iterator it = vtx_range.begin(); it != vtx_range.end(); ++it)
        cout << *it << " geometric information: " << viennagrid::look_up(vector_container, *it) << endl;
    
    
    return 0;
}


