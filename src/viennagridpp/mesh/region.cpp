#include "viennagridpp/range.hpp"

namespace viennagrid
{

 void add(viennagrid_region region, element_t const & element)
  {
    viennagrid_add_to_region(internal_mesh_hierarchy(element),
                             viennagrid::topologic_dimension(element),
                             element.id(),
                             region );
  }


  template<bool element_is_const>
  bool is_in_region( viennagrid_region region, base_element<element_is_const> const & element )
  {
    viennagrid_region * it;
    viennagrid_region * end;

    viennagrid_get_regions(internal_mesh_hierarchy(element),
                           viennagrid::topologic_dimension(element),
                           element.id(),
                           &it,
                           &end);

    for (; it != end; ++it)
    {
      if (*it == region)
        return true;
    }

    return false;
  }

  template bool is_in_region( viennagrid_region region, base_element<false> const & element );
  template bool is_in_region( viennagrid_region region, base_element<true> const & element );




  template<bool element_is_const, bool region_is_const>
  bool is_boundary( base_mesh_region<region_is_const> const & region, base_element<element_is_const> const & element )
  {
    viennagrid_bool result;
    viennagrid_is_boundary_region(region.region().internal(),
                                  region.mesh().internal(),
                                  viennagrid::topologic_dimension(element),
                                  element.id(),
                                  &result);
    return result == VIENNAGRID_TRUE;
  }

  template bool is_boundary( base_mesh_region<false> const & region, base_element<false> const & element );
  template bool is_boundary( base_mesh_region<false> const & region, base_element<true> const & element );
  template bool is_boundary( base_mesh_region<true> const & region, base_element<false> const & element );
  template bool is_boundary( base_mesh_region<true> const & region, base_element<true> const & element );


}