#ifndef VIENNAGRID_REGION_IMPL_HPP
#define VIENNAGRID_REGION_IMPL_HPP

#include "viennagrid/range.hpp"

namespace viennagrid
{

  template<typename SomethingT>
  typename viennagrid::result_of::region_range<SomethingT>::type regions(SomethingT something)
  {
    return typename viennagrid::result_of::region_range<SomethingT>::type(something);
  }

  template<typename SomethingT, typename ElementT>
  typename viennagrid::result_of::region_range<SomethingT>::type regions(SomethingT something, ElementT element)
  {
    return typename viennagrid::result_of::region_range<SomethingT, ElementT>::type(something, element);
  }




  template<typename RegionRangeT1, typename RegionRangeT2>
  bool equal_regions(RegionRangeT1 region1, RegionRangeT2 region2)
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






  inline void add( region_t region, element_t element )
  {
    viennagrid_add_to_region(element.mesh_hierarchy().internal(),
                             element.tag().internal(),
                             element.id(),
                             region.internal() );

    if ( element.tag().facet_tag().valid() )
    {
      typedef viennagrid::result_of::element_range<element_t>::type FacetElemenRangeType;
      typedef viennagrid::result_of::iterator<FacetElemenRangeType>::type FacetElemenRangeIterator;

      FacetElemenRangeType elements( element, element.tag().facet_tag() );
      for (FacetElemenRangeIterator eit = elements.begin(); eit != elements.end(); ++eit)
        add(region, *eit);
    }
  }


  template<bool region_is_const, bool element_is_const>
  bool is_in_region( base_region<region_is_const> region, base_element<element_is_const> element )
  {
    viennagrid_region * it;
    viennagrid_region * end;

    viennagrid_get_regions(element.mesh_hierarchy().internal(),
                           element.tag().internal(),
                           element.id(),
                           &it,
                           &end);

    for (; it != end; ++it)
    {
      viennagrid_index id_;
      viennagrid_region_get_id(*it, &id_);

      if (id_ == region.id())
        return true;
    }

    return false;
  }





  inline void add( mesh_region_t mr, element_t element )
  {
    add( mr.region(), element );
  }

  template<bool region_is_const, bool element_is_const>
  bool is_in_region( base_mesh_region<region_is_const> mesh_region, base_element<element_is_const> element )
  {
    return is_in_region(mesh_region.region(), element);
  }


  template<bool element_is_const, bool region_is_const>
  bool is_boundary( base_mesh_region<region_is_const> r, base_element<element_is_const> e )
  {
    viennagrid_bool result;
    viennagrid_is_boundary_region(r.region().internal(), r.mesh().internal(), e.tag().internal(), e.id(), &result);
    return result == VIENNAGRID_TRUE;
  }




  template<bool mesh_region_is_const, bool element_is_const>
  typename result_of::point< base_mesh_region<mesh_region_is_const> >::type
            get_point(base_mesh_region<mesh_region_is_const> const & region,
                      base_element<element_is_const> const & vertex)
  {
    return get_point(region.mesh(), vertex);
  }

  inline void set_point(base_mesh_region<false> region,
                        base_element<false> vertex,
                        result_of::point< base_mesh_region<false> >::type const & point)
  {
    return set_point(region.mesh(), vertex, point);
  }

}

#endif
