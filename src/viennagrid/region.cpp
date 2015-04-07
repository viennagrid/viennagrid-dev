#include "region.hpp"
#include "mesh_hierarchy.hpp"

bool viennagrid_region_::are_boundary_flags_obsolete()
{ return mesh_hierarchy()->is_obsolete( boundary_elements_change_counter ); }
void viennagrid_region_::set_boundary_flags_uptodate()
{ mesh_hierarchy()->update_change_counter( boundary_elements_change_counter ); }
