#ifndef VIENNAGRID_ELEMENT_PLC_HPP
#define VIENNAGRID_ELEMENT_PLC_HPP

#include "viennagrid/core/forwards.hpp"

namespace viennagrid
{
  inline point get_point(viennagrid_plc plc, viennagrid_int vertex_id)
  {
    viennagrid_dimension geometric_dimension;
    THROW_ON_ERROR( viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension) );

    viennagrid_numeric * coords;
    THROW_ON_ERROR( viennagrid_plc_vertex_coords_get(plc, vertex_id, &coords) );

    point tmp(geometric_dimension);
    std::copy( coords, coords+geometric_dimension, &tmp[0] );

    return tmp;
  }
}

#endif
