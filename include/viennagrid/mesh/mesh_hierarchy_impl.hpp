#ifndef VIENNAGRID_MESH_HIERARCHY_IMPL_HPP
#define VIENNAGRID_MESH_HIERARCHY_IMPL_HPP

#include "viennagrid/mesh/mesh_hierarchy.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/range.hpp"

namespace viennagrid
{
  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::mesh_type base_mesh_hierarchy<is_const>::root()
  {
    viennagrid_mesh internal_root;
    viennagrid_mesh_hierarchy_get_root(internal(), &internal_root);
    return mesh_type(internal_root);
  }

  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::const_mesh_type base_mesh_hierarchy<is_const>::root() const
  {
    viennagrid_mesh internal_root;
    viennagrid_mesh_hierarchy_get_root(internal(), &internal_root);
    return const_mesh_type(internal_root);
  }

  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::region_type base_mesh_hierarchy<is_const>::get_make_region(region_id_type region_id)
  {
    viennagrid_region region;
    viennagrid_region_get_create( internal(), region_id, &region );
    return region_type(region);
  }

  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::region_type base_mesh_hierarchy<is_const>::make_region()
  {
    viennagrid_region region;
    viennagrid_region_create( internal(), &region );
    return region_type(region);
  }

  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::const_region_type base_mesh_hierarchy<is_const>::get_region(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_region_get( internal(), region_id, &region );
    return const_region_type(region);
  }

  template<bool is_const>
  bool base_mesh_hierarchy<is_const>::region_exists(region_id_type region_id) const
  {
    viennagrid_region region;
    viennagrid_region_get( internal(), region_id, &region );
    return region != NULL;
  }



  template<bool element_is_const>
  point_t get_point(viennagrid_mesh_hierarchy mesh_hierarchy, base_element<element_is_const> const & vertex)
  {
    point_t result( viennagrid::geometric_dimension(mesh_hierarchy) );
    viennagrid_numeric const * tmp;
    viennagrid_vertex_get(mesh_hierarchy, vertex.id(), const_cast<viennagrid_numeric **>(&tmp));
    std::copy(tmp, tmp+result.size(), result.begin());
    return result;
  }

  inline void set_point(viennagrid_mesh_hierarchy mesh_hierarchy, base_element<false> const & vertex, point_t const & point)
  {
    viennagrid_numeric * tmp;
    viennagrid_vertex_get(mesh_hierarchy, vertex.id(), &tmp);
    std::copy(point.begin(), point.end(), tmp);
  }


  template<bool element_is_const>
  point_t get_point(base_element<element_is_const> const & element, viennagrid_int index)
  {
    return get_point( viennagrid::vertices(element)[index] );
  }



  template<bool is_const>
  typename base_mesh_hierarchy<is_const>::region_type base_mesh_hierarchy<is_const>::get_make_region(std::string const & name)
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

    region_type region = make_region();
    region.set_name(name);

    return region;
  }

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

}


#endif
