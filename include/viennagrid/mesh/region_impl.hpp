#ifndef VIENNAGRID_REGION_IMPL_HPP
#define VIENNAGRID_REGION_IMPL_HPP

#include "viennagrid/range.hpp"

namespace viennagrid
{

  template<typename SomethingT>
  typename viennagrid::result_of::region_range<SomethingT>::type regions(SomethingT const & something)
  {
    return typename viennagrid::result_of::region_range<SomethingT>::type(something);
  }

  template<typename SomethingT, typename ElementT>
  typename viennagrid::result_of::region_range<SomethingT, ElementT>::type regions(SomethingT const & something, ElementT const & element)
  {
    return typename viennagrid::result_of::region_range<SomethingT, ElementT>::type(something, element);
  }




  template<typename RegionRangeT1, typename RegionRangeT2>
  bool equal_regions(RegionRangeT1 const & region1, RegionRangeT2 const & region2)
  {
    if (region1.size() != region2.size())
      return false;

    for (typename RegionRangeT1::iterator r1it = region1.begin(); r1it != region1.end(); ++r1it)
    {
      typename RegionRangeT2::iterator r2it = region2.begin();
      for (; r2it != region2.end(); ++r2it)
      {
        if ( (*r1it).id() == (*r2it).id() )
          break;
      }

      if (r2it == region2.end())
        return false;
    }

    return true;
  }





  inline void add(viennagrid_region region, element_t const & element)
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


}

#endif
