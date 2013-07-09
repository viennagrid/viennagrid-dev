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


#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/topology/quadrilateral.hpp"


#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/topology_config.hpp"
#include "viennagrid/config/domain_config.hpp"

#include "viennagrid/domain/topology.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/element_creation.hpp"


    //
    // First define the type of handle to use:
    //
    
    //typedef viennagrid::storage::pointer_handle_tag handle_tag;
    //typedef viennagrid::storage::iterator_handle_tag handle_tag;
    typedef viennagrid::storage::id_handle_tag handle_tag;
    
    
    
    //
    // A full config for a hybrid topologically 2d domain
    //
    
    typedef viennagrid::meta::make_typemap<
    
            viennagrid::config::topology_config_tag,
            viennagrid::meta::make_typemap<
            
                                                       // New: Topology not used separately in typelist.
                viennagrid::vertex_tag,
                viennagrid::meta::make_typemap<
                    viennagrid::config::element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,
                
                    viennagrid::config::element_container_tag,
                    viennagrid::storage::handled_container_tag<
                        viennagrid::storage::std_deque_tag,
                        handle_tag
                    >,
                    
                    viennagrid::config::element_boundary_storage_layout_tag,
                    viennagrid::meta::make_typemap<
                    >::type
                >::type,

                
                viennagrid::line_tag,
                viennagrid::meta::make_typemap<
                    viennagrid::config::element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,
                
                    viennagrid::config::element_container_tag,
                    viennagrid::storage::handled_container_tag<
                        viennagrid::storage::std_deque_tag,
                        handle_tag
                    >,
                    
                    viennagrid::config::element_boundary_storage_layout_tag,
                    viennagrid::meta::make_typemap<
                        viennagrid::vertex_tag, viennagrid::no_orientation_handling_tag
                    >::type
                >::type,

                
                viennagrid::quadrilateral_tag,
                viennagrid::meta::make_typemap<
                    viennagrid::config::element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,
                
                    viennagrid::config::element_container_tag,
                    viennagrid::storage::handled_container_tag<
                        viennagrid::storage::std_deque_tag,
                        handle_tag
                    >,
                    
                    viennagrid::config::element_boundary_storage_layout_tag,
                    viennagrid::meta::make_typemap<
                        viennagrid::vertex_tag, viennagrid::no_orientation_handling_tag,
                        viennagrid::line_tag, viennagrid::full_handling_tag
                    >::type
                >::type,
                
                viennagrid::triangle_tag,
                viennagrid::meta::make_typemap<
                    viennagrid::config::element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,
                
                    viennagrid::config::element_container_tag,
                    viennagrid::storage::handled_container_tag<
                        viennagrid::storage::std_deque_tag,
                        handle_tag
                    >,
                    
                    viennagrid::config::element_boundary_storage_layout_tag,
                    viennagrid::meta::make_typemap<
                        viennagrid::vertex_tag, viennagrid::no_orientation_handling_tag,
                        viennagrid::line_tag, viennagrid::full_handling_tag
                    >::type
                >::type
            >::type
    >::type inner_config;
    
    struct config
    {
      typedef inner_config type;
    };


int main()
{
    typedef viennagrid::domain_t<config> DomainType;
    
    DomainType domain;
    
    typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type VertexType;
    typedef viennagrid::result_of::element<DomainType, viennagrid::line_tag>::type LineType;
    typedef viennagrid::result_of::element<DomainType, viennagrid::triangle_tag>::type TriangleType;
    typedef viennagrid::result_of::element<DomainType, viennagrid::quadrilateral_tag>::type QuadrilateralType;
    
    typedef viennagrid::result_of::handle<DomainType, viennagrid::vertex_tag>::type VertexHandleType;
    
    VertexHandleType vh0 = viennagrid::make_vertex( domain );
    VertexHandleType vh1 = viennagrid::make_vertex( domain );
    VertexHandleType vh2 = viennagrid::make_vertex( domain );
    VertexHandleType vh3 = viennagrid::make_vertex( domain );
    VertexHandleType vh4 = viennagrid::make_vertex( domain );
    
    viennagrid::make_quadrilateral( domain, vh0, vh1, vh2, vh3 );
    viennagrid::make_triangle( domain, vh2, vh3, vh4 );
    
    std::cout << "Triangle of Domain:" << std::endl;
    std::copy( viennagrid::elements<viennagrid::triangle_tag>(domain).begin(), viennagrid::elements<viennagrid::triangle_tag>(domain).end(), std::ostream_iterator<TriangleType>(std::cout, "\n") );
    std::cout << std::endl;
    
    std::cout << "Quadrilaterals of Domain:" << std::endl;
    std::copy( viennagrid::elements<viennagrid::quadrilateral_tag>(domain).begin(), viennagrid::elements<viennagrid::quadrilateral_tag>(domain).end(), std::ostream_iterator<QuadrilateralType>(std::cout, "\n") );
    std::cout << std::endl;

    return 0;
}


