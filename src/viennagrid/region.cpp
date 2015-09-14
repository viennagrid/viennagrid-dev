#include "region.hpp"
#include "mesh_hierarchy.hpp"

bool viennagrid_region_::are_boundary_flags_obsolete()
{ return mesh_hierarchy()->is_obsolete( boundary_elements_change_counter ); }
void viennagrid_region_::set_boundary_flags_uptodate()
{ mesh_hierarchy()->update_change_counter( boundary_elements_change_counter ); }



bool viennagrid_region_::element_id_valid(viennagrid_element_id element_id)
{
  return mesh_hierarchy()->element_id_valid(element_id);
}


void viennagrid_region_::add_element(viennagrid_element_id element_id)
{
  mesh_hierarchy()->element_buffer(TOPODIM(element_id)).add_to_region(element_id, id());

  for (viennagrid_dimension boundary_topological_dimension = 0; boundary_topological_dimension != TOPODIM(element_id); ++boundary_topological_dimension)
  {
    viennagrid_int * boundary = mesh_hierarchy()->boundary_begin(element_id, boundary_topological_dimension);
    viennagrid_int * boundary_end = mesh_hierarchy()->boundary_end(element_id, boundary_topological_dimension);

    for (; boundary != boundary_end; ++boundary)
      mesh_hierarchy()->element_buffer(boundary_topological_dimension).add_to_region(*boundary, id());
  }
}

bool viennagrid_region_::contains_element(viennagrid_element_id element_id)
{
  viennagrid_region_id * region_ids_begin = mesh_hierarchy()->element_buffer(TOPODIM(element_id)).regions_begin(element_id);
  viennagrid_region_id * region_ids_end = mesh_hierarchy()->element_buffer(TOPODIM(element_id)).regions_end(element_id);

  for (viennagrid_region_id * it = region_ids_begin; it != region_ids_end; ++it)
  {
    if ( *it == id() )
      return true;
  }

  return false;
}
