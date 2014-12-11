#ifndef VIENNAGRID_MESH_IMPL_HPP
#define VIENNAGRID_MESH_IMPL_HPP

#include "viennagrid/range.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/element/element.hpp"

namespace viennagrid
{
  template<bool is_const>
  std::size_t base_mesh<is_const>::region_count() const
  {
    viennagrid_region * begin;
    viennagrid_region * end;
    viennagrid_regions_get(internal_mesh_hierarchy(), &begin, &end);
    return end-begin;
  }

  template<>
  inline typename base_mesh<false>::region_type base_mesh<false>::get_make_region(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_region_get_create( internal_mesh_hierarchy(), region_id, &region );
    return region_type(internal(), region);
  }

  template<>
  inline typename base_mesh<false>::region_type base_mesh<false>::make_region() const
  {
    viennagrid_region region;
    viennagrid_region_create( internal_mesh_hierarchy(), &region );
    return region_type(internal(), region);
  }

  template<bool is_const>
  typename base_mesh<is_const>::const_region_type base_mesh<is_const>::get_region(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_region_get( internal_mesh_hierarchy(), region_id, &region );
    return const_region_type(internal(), region);
  }


  template<>
  inline typename base_mesh<false>::region_type base_mesh<false>::get_make_region(std::string const & name) const
  { return region_type(internal(), mesh_hierarchy().get_make_region(name).internal()); }

  template<bool is_const>
  typename base_mesh<is_const>::const_region_type base_mesh<is_const>::get_region(std::string const & name) const
  { return region_type(internal(), mesh_hierarchy().get_region(name).internal()); }




  template<bool mesh_is_const, bool element_is_const>
  bool is_boundary( base_mesh<mesh_is_const> const & mesh, base_element<element_is_const> const & element )
  {
    viennagrid_bool result;
    viennagrid_is_boundary_mesh(mesh.internal(),
                                viennagrid::topologic_dimension(element),
                                element.id(),
                                &result);
    return result == VIENNAGRID_TRUE;
  }

  template<bool mesh_is_const, bool element_is_const>
  bool is_any_boundary(base_mesh<mesh_is_const> const & mesh, base_element<element_is_const> const & element)
  {
    typedef typename viennagrid::result_of::region_range< base_mesh<mesh_is_const>, base_element<element_is_const> >::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(mesh, element);
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if (is_boundary(*rit, element))
        return true;
    }

    return false;
  }


  inline void non_recursive_add(mesh_t const & mesh, element_t const & element)
  {
    viennagrid_element_add(mesh.internal(),
                           viennagrid::topologic_dimension(element),
                           element.id());
  }

  inline void add(mesh_t const & mesh, element_t const & element)
  {
    non_recursive_add(mesh, element);

    for (viennagrid_int i = 0; i < viennagrid::topologic_dimension(element); ++i)
    {
      typedef viennagrid::result_of::element_range<element_t>::type ElementRangeType;
      ElementRangeType elements(element, i);
      for (ElementRangeType::iterator it = elements.begin(); it != elements.end(); ++it)
        non_recursive_add(mesh, *it);
    }
  }



}

#endif
