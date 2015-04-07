#ifndef VIENNAGRID_ELEMENT_PLC_HPP
#define VIENNAGRID_ELEMENT_PLC_HPP

#include "viennagridpp/forwards.hpp"

namespace viennagrid
{
  inline void add_hole_point(base_mesh_hierarchy<false> const & mesh_hierarchy,
                             base_element<false> const & element,
                             point_t const & point)
  {
    assert( element.tag().is_plc() );
    viennagrid_plc_add_hole_point( mesh_hierarchy.internal(), element.id(), &point[0] );
  }

  inline void add_hole_point(base_element<false> const & element, point_t const & point)
  {
    add_hole_point(element.mesh_hierarchy(), element, point);
  }

  template<typename PointIteratorT>
  void add_hole_points(base_element<false> const & element, PointIteratorT it, PointIteratorT end)
  {
    for (; it != end; ++it)
      add_hole_point(element, *it);
  }

  template<typename ContainerT>
  void add_hole_points(base_element<false> const & element, ContainerT const & point_container)
  {
    add_hole_points(element, point_container.begin(), point_container.end());
  }

  inline std::vector< point_t > hole_points(base_mesh_hierarchy<false> const & mesh_hierarchy,
                                            base_element<false> const & element)
  {
    assert( element.tag().is_plc() );


    const viennagrid_int dimension = viennagrid::geometric_dimension(mesh_hierarchy);
    viennagrid_numeric const * begin;
    viennagrid_numeric const * end;

    viennagrid_plc_get_hole_points(mesh_hierarchy.internal(), element.id(), &begin, &end);

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
    return hole_points(element.mesh_hierarchy(), element);
  }



}

#endif
