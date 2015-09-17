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
    viennagrid_mesh_regions_get(internal(), &begin, &end);
    return end-begin;
  }

  template std::size_t base_mesh<false>::region_count() const;
  template std::size_t base_mesh<true>::region_count() const;







  template<bool is_const>
  typename base_mesh<is_const>::region_type base_mesh<is_const>::get_or_create_region(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_mesh_region_get_or_create( internal(), region_id, &region );
    return region_type(internal(), region);
  }

  template base_mesh<false>::region_type base_mesh<false>::get_or_create_region(region_id_type region_id) const;
  template base_mesh<true>::region_type base_mesh<true>::get_or_create_region(region_id_type region_id) const;



  template<bool is_const>
  typename base_mesh<is_const>::region_type base_mesh<is_const>::create_region() const
  {
    viennagrid_region region;
    viennagrid_mesh_region_create( internal(), &region );
    return region_type(internal(), region);
  }

  template base_mesh<false>::region_type base_mesh<false>::create_region() const;
  template base_mesh<true>::region_type base_mesh<true>::create_region() const;


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



  template<bool element_is_const>
  point get_point(viennagrid_mesh mesh, base_element<element_is_const> const & vertex)
  {
    point result( viennagrid::geometric_dimension(mesh) );
    viennagrid_numeric const * tmp;
    viennagrid_mesh_vertex_coords_get(mesh, vertex.id().internal(), const_cast<viennagrid_numeric **>(&tmp));
    std::copy(tmp, tmp+result.size(), result.begin());
    return result;
  }

  template point get_point<false>(viennagrid_mesh mesh, base_element<false> const & vertex);
  template point get_point<true>(viennagrid_mesh mesh, base_element<true> const & vertex);



  void set_point(viennagrid_mesh mesh, base_element<false> const & vertex, point const & p)
  {
    viennagrid_numeric * tmp;
    viennagrid_mesh_vertex_coords_get(mesh, vertex.id().internal(), &tmp);
    std::copy(p.begin(), p.end(), tmp);
  }


  template<bool element_is_const>
  point get_point(base_element<element_is_const> const & element, viennagrid_int index)
  {
    return get_point( viennagrid::vertices(element)[index] );
  }

  template point get_point(base_element<false> const & element, viennagrid_int index);
  template point get_point(base_element<true> const & element, viennagrid_int index);



  template<bool is_const>
  typename base_mesh<is_const>::region_type base_mesh<is_const>::get_or_create_region(std::string const & name) const
  {
    typedef base_mesh<is_const> MeshHierarchyType;
    typedef typename viennagrid::result_of::region_range<MeshHierarchyType>::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(*this);
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ( (*rit).get_name() == name )
        return *rit;
    }

    region_type region = create_region();
    region.set_name(name);

    return region;
  }

  template base_mesh<false>::region_type base_mesh<false>::get_or_create_region(std::string const & name) const;
  template base_mesh<true>::region_type base_mesh<true>::get_or_create_region(std::string const & name) const;


  template<bool is_const>
  typename base_mesh<is_const>::const_region_type base_mesh<is_const>::get_region(std::string const & name) const
  {
    typedef base_mesh<is_const> MeshHierarchyType;
    typedef typename viennagrid::result_of::region_range<MeshHierarchyType>::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(*this);
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ( (*rit).get_name() == name )
        return *rit;
    }

    assert(false);
    return const_region_type();
  }

  template base_mesh<false>::const_region_type base_mesh<false>::get_region(std::string const & name) const;
  template base_mesh<true>::const_region_type base_mesh<true>::get_region(std::string const & name) const;


  template<bool is_const>
  bool base_mesh<is_const>::region_exists(std::string const & name) const
  {
    typedef base_mesh<is_const> MeshHierarchyType;
    typedef typename viennagrid::result_of::region_range<MeshHierarchyType>::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(*this);
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ( (*rit).get_name() == name )
        return true;
    }

    return false;
  }

  template bool base_mesh<false>::region_exists(std::string const & name) const;
  template bool base_mesh<true>::region_exists(std::string const & name) const;








  template<bool element_is_const>
  point get_point(base_element<element_is_const> const & vertex)
  {
    assert( topologic_dimension(vertex) == 0 );
    return get_point(internal_mesh(vertex), vertex);
  }

  template point get_point(base_element<false> const & vertex);
  template point get_point(base_element<true> const & vertex);

  void set_point(base_element<false> const & vertex, point const & p)
  {
    assert( topologic_dimension(vertex) == 0 );
    set_point(internal_mesh(vertex), vertex, p );
  }







  template<bool mesh_is_const, bool element_is_const>
  bool is_boundary( base_mesh<mesh_is_const> const & mesh, base_element<element_is_const> const & element )
  {
    viennagrid_bool result;
    viennagrid_element_is_mesh_boundary(mesh.internal(),
                                        element.id().internal(),
                                        &result);
    return result == VIENNAGRID_TRUE;
  }

  template bool is_boundary( base_mesh<false> const & mesh, base_element<false> const & element );
  template bool is_boundary( base_mesh<false> const & mesh, base_element<true> const & element );
  template bool is_boundary( base_mesh<true> const & mesh, base_element<false> const & element );
  template bool is_boundary( base_mesh<true> const & mesh, base_element<true> const & element );



  template<bool element_is_const>
  bool is_any_boundary(base_element<element_is_const> const & element)
  {
    typedef typename viennagrid::result_of::region_range< base_element<element_is_const> >::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(element);
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if (is_boundary(*rit, element))
        return true;
    }

    return false;
  }

  template bool is_any_boundary(base_element<false> const & element);
  template bool is_any_boundary(base_element<true> const & element);
//   template bool is_any_boundary(base_element<false> const & element);
//   template bool is_any_boundary(base_element<true> const & element);

}
