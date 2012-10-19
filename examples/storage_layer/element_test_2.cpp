#include <iostream>
#include <typeinfo>

#include <map>

using std::cout;
using std::endl;


#include "viennagrid/forwards.h"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"

#include "viennagrid/detail/boundary_ncell_layer.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/topology/point.hpp"

    


int main()
{
    typedef viennagrid::point_t< double, viennagrid::cartesian_cs<3> > point_type;
    
    //point_type p;
    
    //typedef viennameta::static_pair< viennagrid::storage::static_array<point_type, 1>, viennameta::null_type > vertex_vertex_containers;
    //typedef VIENNAMETA_MAKE_TYPELIST_1(vertex_vertex_containers) vertex_containers_typelist;
    
    typedef viennagrid::viennagrid_ng::vertex_t< point_type > vertex_type;
    
    
    //typedef viennameta::static_pair< viennagrid::storage::static_array<vertex_type *, 2>, viennameta::null_type > line_vertex_containers;
    

    
    typedef viennameta::static_pair<
        viennagrid::storage::view_t<
            vertex_type *,
            viennagrid::storage::static_array_tag<2>
        >,
        viennameta::null_type
    > line_vertex_containers;
    
    typedef VIENNAMETA_MAKE_TYPELIST_1(line_vertex_containers) line_containers_typelist;
    
    typedef viennagrid::viennagrid_ng::element_t< viennagrid::line_tag, line_containers_typelist > line_type;
    
    
    vertex_type v0;
    vertex_type v1;
    
    v0[0] = 1;
    v0[1] = 0;
    v0[2] = 0;

    v1[0] = 0;
    v1[1] = 1;
    v1[2] = 0;

    
    line_type line;
    
    line.container( viennagrid::dimension_tag<0>() )[0] = v0;
    //line.container( viennagrid::dimension_tag<0>() )[1] = v1;
    
    
    typedef viennagrid::viennagrid_ng::result_of::container_of_dimension<line_type, 0>::type range_type;
    range_type range = viennagrid::viennagrid_ng::ncells<0>(line);
    
    for (range_type::iterator it = range.begin(); it != range.end(); ++it)
        cout << *it << endl;
    
    
    //cout << line << endl;
    
    

    return 0;
}


