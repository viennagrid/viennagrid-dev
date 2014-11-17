#ifndef VIENNAGRID_ELEMENT_PLC_HPP
#define VIENNAGRID_ELEMENT_PLC_HPP

#include "viennagrid/forwards.hpp"

namespace viennagrid
{
  inline void add_hole_point(base_mesh<false> mesh, base_element<false> element, point_t const & point)
  {
    assert( element.tag().is_plc() );
    viennagrid_plc_add_hole_point( mesh.internal(), element.id(), &point[0] );
  }

  inline void add_hole_point(base_element<false> element, point_t const & point)
  {
    add_hole_point(element.mesh_hierarchy().root(), element, point);
  }


  inline std::vector< point_t > hole_points(base_mesh<false> mesh, base_element<false> element)
  {
    assert( element.tag().is_plc() );


    const viennagrid_int dimension = mesh.dimension();
    viennagrid_numeric const * begin;
    viennagrid_numeric const * end;

    viennagrid_plc_get_hole_points(mesh.internal(), element.id(), &begin, &end);

    assert( (end-begin) % dimension == 0 );
    std::size_t vertex_count = (end-begin)/dimension;

    std::vector< point_t > result(vertex_count);

    for (std::size_t i = 0; i != vertex_count; ++i)
    {
      point_t tmp( dimension );
      std::copy( begin+dimension*i, begin+dimension*(i+1), &tmp[0] );
      result[i] = tmp;
    }

    return result;
  }

  inline std::vector< point_t > hole_points(base_element<false> element)
  {
    return hole_points(element.mesh_hierarchy().root(), element);
  }



}

#endif
