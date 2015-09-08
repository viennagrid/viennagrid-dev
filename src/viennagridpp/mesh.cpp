#include "viennagrid/core/range.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element.hpp"

namespace viennagrid
{
  template<bool is_const>
  std::size_t base_mesh<is_const>::region_count() const
  {
    viennagrid_region_id * begin;
    viennagrid_region_id * end;
    viennagrid_mesh_hierarchy_regions_get(internal_mesh_hierarchy(), &begin, &end);
    return end-begin;
  }

  template std::size_t base_mesh<false>::region_count() const;
  template std::size_t base_mesh<true>::region_count() const;


  template<>
  typename base_mesh<false>::region_type base_mesh<false>::get_or_create_region(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_mesh_region_get_or_create( internal(), region_id, &region );
    return region_type(internal(), region);
  }

  template<>
  typename base_mesh<false>::region_type base_mesh<false>::create_region() const
  {
    viennagrid_region region;
    viennagrid_mesh_region_create( internal(), &region );
    return region_type(internal(), region);
  }

  template<bool is_const>
  typename base_mesh<is_const>::const_region_type base_mesh<is_const>::get_region(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_mesh_region_get( internal(), region_id, &region );
    return const_region_type(internal(), region);
  }

  template base_mesh<false>::const_region_type base_mesh<false>::get_region(region_id_type region_id) const;
  template base_mesh<true>::const_region_type base_mesh<true>::get_region(region_id_type region_id) const;


  template<bool is_const>
  bool base_mesh<is_const>::region_exists(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_mesh_region_get( internal(), region_id, &region );
    return region != NULL;
  }

  template bool base_mesh<false>::region_exists(region_id_type region_id) const;
  template bool base_mesh<true>::region_exists(region_id_type region_id) const;


  template<>
  typename base_mesh<false>::region_type base_mesh<false>::get_or_create_region(std::string const & name) const
  { return region_type(internal(), get_mesh_hierarchy().get_or_create_region(name).internal()); }

  template<bool is_const>
  typename base_mesh<is_const>::const_region_type base_mesh<is_const>::get_region(std::string const & name) const
  { return region_type(internal(), get_mesh_hierarchy().get_region(name).internal()); }

  template base_mesh<false>::const_region_type base_mesh<false>::get_region(std::string const & name) const;
  template base_mesh<true>::const_region_type base_mesh<true>::get_region(std::string const & name) const;

  template<bool is_const>
  bool base_mesh<is_const>::region_exists(std::string const & name) const
  { return get_mesh_hierarchy().region_exists(name); }

  template bool base_mesh<false>::region_exists(std::string const & name) const;
  template bool base_mesh<true>::region_exists(std::string const & name) const;




  template<bool mesh_is_const, bool element_is_const>
  bool is_boundary( base_mesh<mesh_is_const> const & mesh, base_element<element_is_const> const & element )
  {
    viennagrid_bool result;
    viennagrid_element_is_mesh_boundary(mesh.internal(),
                                        viennagrid::topologic_dimension(element),
                                        element.id(),
                                        &result);
    return result == VIENNAGRID_TRUE;
  }

  template bool is_boundary( base_mesh<false> const & mesh, base_element<false> const & element );
  template bool is_boundary( base_mesh<false> const & mesh, base_element<true> const & element );
  template bool is_boundary( base_mesh<true> const & mesh, base_element<false> const & element );
  template bool is_boundary( base_mesh<true> const & mesh, base_element<true> const & element );



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

  template bool is_any_boundary(base_mesh<false> const & mesh, base_element<false> const & element);
  template bool is_any_boundary(base_mesh<false> const & mesh, base_element<true> const & element);
  template bool is_any_boundary(base_mesh<true> const & mesh, base_element<false> const & element);
  template bool is_any_boundary(base_mesh<true> const & mesh, base_element<true> const & element);


  void non_recursive_add(mesh const & m, element const & e)
  {
    viennagrid_mesh_element_add(m.internal(),
                           viennagrid::topologic_dimension(e),
                           e.id());
  }

  void add(mesh const & m, element const & e)
  {
    non_recursive_add(m, e);

    for (viennagrid_int i = 0; i < viennagrid::topologic_dimension(e); ++i)
    {
      typedef viennagrid::result_of::element_range<element>::type ElementRangeType;
      ElementRangeType elements(e, i);
      for (ElementRangeType::iterator it = elements.begin(); it != elements.end(); ++it)
        non_recursive_add(m, *it);
    }
  }



}
