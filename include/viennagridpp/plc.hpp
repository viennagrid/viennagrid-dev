#ifndef VIENNAGRID_ELEMENT_PLC_HPP
#define VIENNAGRID_ELEMENT_PLC_HPP

#include "viennagridpp/forwards.hpp"

namespace viennagrid
{
  inline point get_point(viennagrid_plc plc, viennagrid_int vertex_id)
  {
    viennagrid_dimension geometric_dimension;
    viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);

    viennagrid_numeric * coords;
    viennagrid_plc_vertex_coords_get(plc, vertex_id, &coords);

    point tmp(geometric_dimension);
    std::copy( coords, coords+geometric_dimension, &tmp[0] );

    return tmp;
  }
}

#endif
