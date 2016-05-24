#include "element_handle_buffer.hpp"
#include "mesh_hierarchy.hpp"

void viennagrid_element_handle_buffer::add_element(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id element_id)
{
  insert( mesh_hierarchy, element_id );
}

void viennagrid_element_handle_buffer::add_elements(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id start_id, viennagrid_int count)
{
  if (ids_.empty() || (start_id > ids_.back()))
  {
    viennagrid_int old_size = ids_.size();
    ids_.resize( ids_.size() + count );
    for (viennagrid_int i = 0; i != count; ++i)
    {
      ids_[ old_size+i ] = start_id+i;
      element_counts[ +mesh_hierarchy->element_buffer(topologic_dimension).element_type(start_id+i) ]++;
    }
  }
  else
  {
    // TODO implement
    assert(false);
  }
}

void viennagrid_element_handle_buffer::save_add_elements(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id start_id, viennagrid_int count)
{
  for (viennagrid_int i = 0; i != count; ++i)
  {
    save_add_element(mesh_hierarchy, start_id+i);
  }
}

void viennagrid_element_handle_buffer::insert(viennagrid_mesh_hierarchy mesh_hierarchy, viennagrid_element_id id)
{
  if (ids_.empty() || (id > ids_.back()))
  {
    ids_.push_back(id);
    element_counts[ +mesh_hierarchy->element_buffer(topologic_dimension).element_type(id) ]++;
  }
  else
  {
    iterator it = std::lower_bound(begin(), end(), id);
    if ( (*it) != id )
    {
      ids_.insert(it, id);
      element_counts[ +mesh_hierarchy->element_buffer(topologic_dimension).element_type(id) ]++;
    }
  }
}
