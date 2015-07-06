#include "viennagridpp/mesh/mesh_hierarchy.hpp"
#include "viennagridpp/mesh/mesh.hpp"
#include "viennagridpp/range.hpp"

namespace viennagrid
{
  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::mesh_type base_mesh_hierarchy<is_const>::root()
  {
    viennagrid_mesh internal_root;
    viennagrid_mesh_hierarchy_root_mesh_get(internal(), &internal_root);
    return mesh_type(internal_root);
  }

  template base_mesh_hierarchy<false>::mesh_type base_mesh_hierarchy<false>::root();
  template base_mesh_hierarchy<true>::mesh_type base_mesh_hierarchy<true>::root();


  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::const_mesh_type base_mesh_hierarchy<is_const>::root() const
  {
    viennagrid_mesh internal_root;
    viennagrid_mesh_hierarchy_root_mesh_get(internal(), &internal_root);
    return const_mesh_type(internal_root);
  }

  template base_mesh_hierarchy<false>::const_mesh_type base_mesh_hierarchy<false>::root() const;
  template base_mesh_hierarchy<true>::const_mesh_type base_mesh_hierarchy<true>::root() const;




  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::region_type base_mesh_hierarchy<is_const>::get_or_create_region(region_id_type region_id)
  {
    viennagrid_region region;
    viennagrid_region_get_or_create( internal(), region_id, &region );
    return region_type(region);
  }

  template base_mesh_hierarchy<false>::region_type base_mesh_hierarchy<false>::get_or_create_region(region_id_type region_id);
  template base_mesh_hierarchy<true>::region_type base_mesh_hierarchy<true>::get_or_create_region(region_id_type region_id);



  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::region_type base_mesh_hierarchy<is_const>::create_region()
  {
    viennagrid_region region;
    viennagrid_region_create( internal(), &region );
    return region_type(region);
  }

  template base_mesh_hierarchy<false>::region_type base_mesh_hierarchy<false>::create_region();
  template base_mesh_hierarchy<true>::region_type base_mesh_hierarchy<true>::create_region();


  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::const_region_type base_mesh_hierarchy<is_const>::get_region(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_region_get( internal(), region_id, &region );
    return const_region_type(region);
  }

  template base_mesh_hierarchy<false>::const_region_type base_mesh_hierarchy<false>::get_region(region_id_type region_id) const;
  template base_mesh_hierarchy<true>::const_region_type base_mesh_hierarchy<true>::get_region(region_id_type region_id) const;


  template<bool is_const>
  bool base_mesh_hierarchy<is_const>::region_exists(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_region_get( internal(), region_id, &region );
    return region != NULL;
  }

  template bool base_mesh_hierarchy<false>::region_exists(region_id_type region_id) const;
  template bool base_mesh_hierarchy<true>::region_exists(region_id_type region_id) const;



  template<bool element_is_const>
  point get_point(viennagrid_mesh_hierarchy mesh_hierarchy, base_element<element_is_const> const & vertex)
  {
    point result( viennagrid::geometric_dimension(mesh_hierarchy) );
    viennagrid_numeric const * tmp;
    viennagrid_mesh_hierarchy_vertex_coords_get(mesh_hierarchy, vertex.id(), const_cast<viennagrid_numeric **>(&tmp));
    std::copy(tmp, tmp+result.size(), result.begin());
    return result;
  }

  template point get_point<false>(viennagrid_mesh_hierarchy mesh_hierarchy, base_element<false> const & vertex);
  template point get_point<true>(viennagrid_mesh_hierarchy mesh_hierarchy, base_element<true> const & vertex);



  void set_point(viennagrid_mesh_hierarchy mesh_hierarchy, base_element<false> const & vertex, point const & p)
  {
    viennagrid_numeric * tmp;
    viennagrid_mesh_hierarchy_vertex_coords_get(mesh_hierarchy, vertex.id(), &tmp);
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
  typename base_mesh_hierarchy<is_const>::region_type base_mesh_hierarchy<is_const>::get_or_create_region(std::string const & name)
  {
    typedef base_mesh_hierarchy<is_const> MeshHierarchyType;
    typedef typename viennagrid::result_of::region_range<MeshHierarchyType>::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(*this);
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ( (*rit).name() == name )
        return *rit;
    }

    region_type region = create_region();
    region.set_name(name);

    return region;
  }

  template base_mesh_hierarchy<false>::region_type base_mesh_hierarchy<false>::get_or_create_region(std::string const & name);
  template base_mesh_hierarchy<true>::region_type base_mesh_hierarchy<true>::get_or_create_region(std::string const & name);


  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::const_region_type base_mesh_hierarchy<is_const>::get_region(std::string const & name) const
  {
    typedef base_mesh_hierarchy<is_const> MeshHierarchyType;
    typedef typename viennagrid::result_of::region_range<MeshHierarchyType>::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(*this);
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ( (*rit).name() == name )
        return *rit;
    }

    assert(false);
    return const_region_type();
  }

  template base_mesh_hierarchy<false>::const_region_type base_mesh_hierarchy<false>::get_region(std::string const & name) const;
  template base_mesh_hierarchy<true>::const_region_type base_mesh_hierarchy<true>::get_region(std::string const & name) const;


  template<bool is_const>
  bool base_mesh_hierarchy<is_const>::region_exists(std::string const & name) const
  {
    typedef base_mesh_hierarchy<is_const> MeshHierarchyType;
    typedef typename viennagrid::result_of::region_range<MeshHierarchyType>::type RegionRangeType;
    typedef typename viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    RegionRangeType regions(*this);
    for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      if ( (*rit).name() == name )
        return true;
    }

    return false;
  }

  template bool base_mesh_hierarchy<false>::region_exists(std::string const & name) const;
  template bool base_mesh_hierarchy<true>::region_exists(std::string const & name) const;

}
