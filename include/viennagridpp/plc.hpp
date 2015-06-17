#ifndef VIENNAGRID_ELEMENT_PLC_HPP
#define VIENNAGRID_ELEMENT_PLC_HPP

#include "viennagridpp/forwards.hpp"

namespace viennagrid
{
  inline point_t get_point(viennagrid_plc plc, viennagrid_int vertex_id)
  {
    viennagrid_dimension geometric_dimension;
    viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);

    viennagrid_numeric * coords;
    viennagrid_plc_point_get(plc, vertex_id, &coords);

    point_t tmp(geometric_dimension);
    std::copy( coords, coords+geometric_dimension, &tmp[0] );

    return tmp;
  }
}

#endif
