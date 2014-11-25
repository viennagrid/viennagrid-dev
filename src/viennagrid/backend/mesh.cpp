#include "viennagrid/backend/mesh.hpp"
#include "viennagrid/backend/mesh_hierarchy.hpp"




void viennagrid_mesh_::add_element(viennagrid_dimension element_topo_dim,
                                   viennagrid_index element_id)
{
  assert( (element_topo_dim >= 0) && (element_topo_dim < VIENNAGRID_TOPOLOGIC_DIMENSION_END) );
  assert( element_id >= 0 );

  viennagrid_index parent_id = mesh_hierarchy()->element_buffer(element_topo_dim).parent_id(element_id);

  if (element_topo_dim == cell_dimension())
  {
    if (parent())
    {
      if (parent_id >= 0)
      {
//         bool parent_is_in_parent_mesh = parent()->element_handle_buffer(element_topo_dim).element_present(parent_id);
//         bool is_in_parent_mesh = parent()->element_handle_buffer(element_topo_dim).element_present(element_id);

        assert( parent()->element_handle_buffer(element_topo_dim).element_present(parent_id) ||
                parent()->element_handle_buffer(element_topo_dim).element_present(element_id) );
      }
      else
      {
//         bool is_in_parent_mesh = parent()->element_handle_buffer(element_topo_dim).element_present(element_id);

        assert( parent()->element_handle_buffer(element_topo_dim).element_present(element_id) );
      }
    }
    else
    {
      assert( parent_id < 0 );
    }
  }

  element_handle_buffer(element_topo_dim).add_element(element_id);
}




template<typename SortedContainerT1, typename SortedContainerT2>
bool is_subset(SortedContainerT1 const & cont1, SortedContainerT2 const & cont2)
{
  typename SortedContainerT1::const_iterator it1 = cont1.begin();
  typename SortedContainerT1::const_iterator it2 = cont2.begin();

  while ((it1 != cont1.end()) && (it2 != cont2.end()))
  {
    if (*it1 == *it2)
      ++it1;
    ++it2;
  }

  return it1 == cont1.end();
}







viennagrid_index viennagrid_mesh_::make_refined_element(
  viennagrid_dimension      element_topo_dim,
  viennagrid_index          element_id,
  viennagrid_element_tag    refined_element_tag,
  viennagrid_int            refined_element_base_count,
  viennagrid_index *        refined_element_base_indices,
  viennagrid_dimension *    refined_element_base_dimensions,
  viennagrid_int            intersects_count,
  viennagrid_index *        intersect_vertices_indices,
  viennagrid_index *        intersects_indices,
  viennagrid_dimension *    intersects_topo_dims)
{
  assert( !is_root() );
  assert( parent()->element_handle_buffer(element_topo_dim).element_present(element_id) );

  std::vector<viennagrid_int> refined_elements;


  std::map< viennagrid_dimension, std::map<viennagrid_index, std::vector<viennagrid_index> > > intersects;

  for (viennagrid_int i = 0; i != intersects_count; ++i)
  {
    intersects[intersects_topo_dims[i]][intersects_indices[i]].push_back( intersect_vertices_indices[i] );
  }


  viennagrid_index child_element_id = mesh_hierarchy()->get_make_element(
          refined_element_tag,
          refined_element_base_indices,
          refined_element_base_dimensions,
          refined_element_base_count );

  mesh_hierarchy()->element_buffer(element_topo_dim).set_parent_id( child_element_id, element_id );


  add_element(element_topo_dim, child_element_id);
  for (viennagrid_dimension boundary_dim = 0; boundary_dim != element_topo_dim; ++boundary_dim)
  {
    viennagrid_index * boundary = boundary_begin(element_topo_dim, child_element_id, boundary_dim);
    viennagrid_index * end = boundary_end(element_topo_dim, child_element_id, boundary_dim);

    for (; boundary != end; ++boundary)
    {
      viennagrid_int boundary_id = *boundary;

      add_element(boundary_dim, boundary_id);

      if (boundary_dim == 0)
        continue;

      std::vector<viennagrid_index> boundary_vertices;
      viennagrid_index * bv = boundary_begin(boundary_dim, boundary_id, 0);
      viennagrid_index * bv_end = boundary_end(boundary_dim, boundary_id, 0);
      std::copy(bv, bv_end, std::back_inserter(boundary_vertices));
      std::sort(boundary_vertices.begin(), boundary_vertices.end());

      if (mesh_hierarchy()->element_buffer(boundary_dim).parent_id(boundary_id) < 0)
      {
        std::map<viennagrid_index, std::vector<viennagrid_index> > & intersected_elements = intersects[boundary_dim];

        for (std::map<viennagrid_index, std::vector<viennagrid_index> >::iterator kt = intersected_elements.begin(); kt != intersected_elements.end(); ++kt)
        {
          std::vector<viennagrid_index> intersected_element_indices;

          viennagrid_index * intersects = boundary_begin(boundary_dim, kt->first, 0);
          viennagrid_index * intersects_end = boundary_end(boundary_dim, kt->first, 0);

          std::copy(intersects, intersects_end, std::back_inserter(intersected_element_indices));
          std::copy(kt->second.begin(), kt->second.end(), std::back_inserter(intersected_element_indices));
          std::sort(intersected_element_indices.begin(), intersected_element_indices.end());

          if ( is_subset(boundary_vertices, intersected_element_indices) )
          {
//               std::cout << "FOUND MATCH!" << std::endl;
//               std::cout << boundary_dim << "/" << kt->first << " is a parent of " << boundary_dim << "/" << boundary_id << std::endl;

            mesh_hierarchy()->element_buffer(boundary_dim).set_parent_id(boundary_id, kt->first);
          }
        }
      }
    }
  }

  return child_element_id;
}






bool viennagrid_mesh_::is_root() const { return hierarchy_->root() == this; }

viennagrid_dimension viennagrid_mesh_::geometric_dimension() { return hierarchy_->geometric_dimension(); }
viennagrid_dimension viennagrid_mesh_::cell_dimension() { return hierarchy_->cell_dimension(); }

// inline viennagrid_element_tag viennagrid_mesh_::unpack_element_tag(viennagrid_element_tag et)
// { return mesh_hierarchy()->unpack_element_tag(et); }


viennagrid_index * viennagrid_mesh_::boundary_begin(viennagrid_dimension element_topo_dim, viennagrid_index element_id, viennagrid_dimension boundary_topo_dim)
{ return mesh_hierarchy()->boundary_begin(element_topo_dim, element_id, boundary_topo_dim); }
viennagrid_index * viennagrid_mesh_::boundary_end(viennagrid_dimension element_topo_dim, viennagrid_index element_id, viennagrid_dimension boundary_topo_dim)
{ return mesh_hierarchy()->boundary_end(element_topo_dim, element_id, boundary_topo_dim); }


bool viennagrid_mesh_::is_coboundary_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim)
{ return mesh_hierarchy()->is_obsolete( coboundary_change_counter(element_topo_dim, coboundary_topo_dim) ); }
void viennagrid_mesh_::set_coboundary_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim)
{ mesh_hierarchy()->update_change_counter( coboundary_change_counter(element_topo_dim, coboundary_topo_dim) ); }

bool viennagrid_mesh_::is_neighbor_obsolete(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim)
{ return mesh_hierarchy()->is_obsolete( neighbor_change_counter(element_topo_dim, connector_topo_dim, neighbor_topo_dim) ); }
void viennagrid_mesh_::set_neighbor_uptodate(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim)
{ mesh_hierarchy()->update_change_counter( neighbor_change_counter(element_topo_dim, connector_topo_dim, neighbor_topo_dim) ); }


bool viennagrid_mesh_::are_boundary_flags_obsolete()
{ return hierarchy_->is_obsolete( boundary_elements_change_counter ); }
void viennagrid_mesh_::set_boundary_flags_uptodate()
{ hierarchy_->update_change_counter( boundary_elements_change_counter ); }





bool viennagrid_mesh_::is_element_children_obsolete(viennagrid_int child_mesh_index_, viennagrid_int element_topo_dim)
{ return mesh_hierarchy()->is_obsolete( element_children_change_counter(child_mesh_index_, element_topo_dim) ); }
void viennagrid_mesh_::set_element_children_uptodate(viennagrid_int child_mesh_index_, viennagrid_int element_topo_dim)
{ return mesh_hierarchy()->update_change_counter( element_children_change_counter(child_mesh_index_, element_topo_dim) ); }


void viennagrid_mesh_::make_element_children(viennagrid_mesh child, viennagrid_int element_topo_dim)
{
  viennagrid_int child_mesh_index_ = child_mesh_index(child);

  if (!is_element_children_obsolete(child_mesh_index_, element_topo_dim))
    return;


  std::map<viennagrid_index, std::vector<viennagrid_index> > children;

  viennagrid_index * it       = child->elements_begin(element_topo_dim);
  viennagrid_index * it_end   = child->elements_end(element_topo_dim);

  for (; it != it_end; ++it)
  {
    viennagrid_index parent_id = mesh_hierarchy()->element_buffer(element_topo_dim).parent_id(*it);
    assert( element_handle_buffer(element_topo_dim).element_present(parent_id) );
    children[parent_id].push_back(*it);
  }



  viennagrid_element_children_buffer & element_children_buffer = element_children[child_mesh_index_].children_indices_buffer(element_topo_dim);
  element_children_buffer.clear();



  set_element_children_uptodate(child_mesh_index_, element_topo_dim);
}





void viennagrid_mesh_::make_coboundary(viennagrid_int element_topo_dim, viennagrid_int coboundary_topo_dim)
{
  if (!is_coboundary_obsolete(element_topo_dim, coboundary_topo_dim))
    return;

  viennagrid_index * coboundary_ids = element_handle_buffer(coboundary_topo_dim).ids();
  viennagrid_int coboundary_count = element_handle_buffer(coboundary_topo_dim).count();

  std::map< viennagrid_index, std::vector<viennagrid_index> > tmp;

  for (viennagrid_int i = 0; i < coboundary_count; ++i)
  {
    viennagrid_index coboundary_index = coboundary_ids[i];
    viennagrid_index * element_it = boundary_begin(coboundary_topo_dim, coboundary_index, element_topo_dim);
    viennagrid_index * element_end = boundary_end(coboundary_topo_dim, coboundary_index, element_topo_dim);

    for (; element_it != element_end; ++element_it)
      tmp[*element_it].push_back(coboundary_index);
  }


  viennagrid_coboundary_buffer & coboundary_buffer = element_handle_buffer(element_topo_dim).coboundary_buffer(coboundary_topo_dim);

  coboundary_buffer.clear();
  for (std::map< viennagrid_index, std::vector<viennagrid_index> >::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    viennagrid_index * indices = coboundary_buffer.insert( it->first, it->second.size() );
    std::copy( it->second.begin(), it->second.end(), indices );
  }

  set_coboundary_uptodate(element_topo_dim, coboundary_topo_dim);
}




void viennagrid_mesh_::make_neighbor(viennagrid_int element_topo_dim, viennagrid_int connector_topo_dim, viennagrid_int neighbor_topo_dim)
{
  if (!is_neighbor_obsolete(element_topo_dim, connector_topo_dim, neighbor_topo_dim))
    return;

  if ((element_topo_dim > connector_topo_dim) && (connector_topo_dim > neighbor_topo_dim))
    return;

  if ((element_topo_dim == connector_topo_dim) || (connector_topo_dim == neighbor_topo_dim))
    return;

  viennagrid_index * element_ids = element_handle_buffer(element_topo_dim).ids();
  viennagrid_int element_count = element_handle_buffer(element_topo_dim).count();


  viennagrid_neighbor_buffer & neighbor_buffer = element_handle_buffer(element_topo_dim).neighbor_buffer(connector_topo_dim, neighbor_topo_dim);
  neighbor_buffer.clear();

  for (viennagrid_int i = 0; i < element_count; ++i)
  {
    viennagrid_index element_index = element_ids[i];
    std::set<viennagrid_index> tmp;

    viennagrid_index * connector_it = connected_begin(element_topo_dim, element_index, connector_topo_dim);
    viennagrid_index * connector_end = connected_end(element_topo_dim, element_index, connector_topo_dim);

    for (; connector_it != connector_end; ++connector_it)
    {
      viennagrid_index connector_index = *connector_it;

      viennagrid_index * neighbor_it = connected_begin(connector_topo_dim, connector_index, neighbor_topo_dim);
      viennagrid_index * neighbor_end = connected_end(connector_topo_dim, connector_index, neighbor_topo_dim);


      for (; neighbor_it != neighbor_end; ++neighbor_it)
      {
        viennagrid_index neighbor_index = *neighbor_it;
        if ((element_topo_dim == neighbor_topo_dim) && (element_index == neighbor_index))
          continue;

        tmp.insert( neighbor_index );
      }
    }

    if (element_topo_dim > neighbor_topo_dim)
    {
      viennagrid_index * it = boundary_begin(element_topo_dim, element_index, neighbor_topo_dim);
      viennagrid_index * end = boundary_begin(element_topo_dim, element_index, neighbor_topo_dim);

      for (; it != end; ++it)
        tmp.erase(*it);
    }



    viennagrid_index * indices = neighbor_buffer.insert( element_index, tmp.size() );
    std::copy( tmp.begin(), tmp.end(), indices );
  }

  set_neighbor_uptodate(element_topo_dim, connector_topo_dim, neighbor_topo_dim);
}




void viennagrid_mesh_::make_boundary_flags(viennagrid_region region)
{
  const viennagrid_int cell_topo_dim = cell_dimension();
  const viennagrid_int facet_topo_dim = cell_topo_dim-1;

  if (!region->are_boundary_flags_obsolete())
    return;

  region->clear_boundary(facet_topo_dim);
  make_coboundary(facet_topo_dim, cell_topo_dim);

  for (viennagrid_index * facet = elements_begin(facet_topo_dim); facet != elements_end(facet_topo_dim); ++facet)
  {
    viennagrid_index * it = coboundary_begin(facet_topo_dim, *facet, cell_topo_dim);
    viennagrid_index * cells_end = coboundary_end(facet_topo_dim, *facet, cell_topo_dim);

    viennagrid_int size = 0;
    for (; it != cells_end; ++it)
    {
      if ( mesh_hierarchy()->is_in_region(cell_topo_dim, *it, region->id()) )
        ++size;
    }

    if (size == 1)
    {
      for (viennagrid_int boundary_topo_dim = 0; boundary_topo_dim != facet_topo_dim; ++boundary_topo_dim)
      {
        if (viennagrid_is_boundary_tag(facet_topo_dim, boundary_topo_dim) == VIENNAGRID_TRUE)
        {
          viennagrid_index * it = mesh_hierarchy()->element_buffer(facet_topo_dim).boundary_buffer(boundary_topo_dim).begin(*facet);
          viennagrid_index * boundary_end = mesh_hierarchy()->element_buffer(facet_topo_dim).boundary_buffer(boundary_topo_dim).end(*facet);
          for (; it != boundary_end; ++it)
          {
            region->set_boundary(boundary_topo_dim, *it);
          }
        }
      }

      region->set_boundary(facet_topo_dim, *facet);
    }
  }

  region->set_boundary_flags_uptodate();
}


void viennagrid_mesh_::make_boundary_flags()
{
  const viennagrid_int cell_topo_dim = cell_dimension();
  const viennagrid_int facet_topo_dim = cell_topo_dim-1;

  if (!are_boundary_flags_obsolete())
    return;


  clear_boundary(facet_topo_dim);
  make_coboundary(facet_topo_dim, cell_topo_dim);

  for (viennagrid_index * facet = elements_begin(facet_topo_dim); facet != elements_end(facet_topo_dim); ++facet)
  {
    viennagrid_index * cells_begin = coboundary_begin(facet_topo_dim, *facet, cell_topo_dim);
    viennagrid_index * cells_end = coboundary_end(facet_topo_dim, *facet, cell_topo_dim);

    viennagrid_int size = cells_end - cells_begin;

    if (size == 1)
    {
      for (viennagrid_int boundary_topo_dim = 0; boundary_topo_dim != facet_topo_dim; ++boundary_topo_dim)
      {
        if (viennagrid_is_boundary_tag(facet_topo_dim, boundary_topo_dim) == VIENNAGRID_TRUE)
        {
          viennagrid_index * it = mesh_hierarchy()->element_buffer(facet_topo_dim).boundary_buffer(boundary_topo_dim).begin(*facet);
          viennagrid_index * boundary_end = mesh_hierarchy()->element_buffer(facet_topo_dim).boundary_buffer(boundary_topo_dim).end(*facet);
          for (; it != boundary_end; ++it)
          {
            set_boundary(boundary_topo_dim, *it);
          }
        }
      }

      set_boundary(facet_topo_dim, *facet);
    }
  }

  set_boundary_flags_uptodate();
}






viennagrid_index viennagrid_element_buffer::make_element(viennagrid_mesh_hierarchy mesh_hierarchy,
                                                         viennagrid_element_tag element_tag,
                                                         viennagrid_index * indices,
                                                         viennagrid_int index_count)
{
  viennagrid_index id = size();

  element_tags.push_back(element_tag);
  parents.push_back(-1);
  region_buffer.push_back(0);
  if (element_tag == VIENNAGRID_ELEMENT_TAG_PLC)
    mesh_hierarchy->make_plc();

  if (indices)
  {
    if (element_tag == VIENNAGRID_ELEMENT_TAG_PLC)
    {
      std::set<viennagrid_index> vertices;
      for (viennagrid_int i = 0; i < index_count; ++i)
      {
        viennagrid_index * vtx = mesh_hierarchy->boundary_begin(1, *(indices+i), 0);
        viennagrid_index * end = mesh_hierarchy->boundary_end(1, *(indices+i), 0);

//         element_buffer(VIENNAGRID_ELEMENT_TAG_LINE).boundary_indices_begin(VIENNAGRID_ELEMENT_TAG_VERTEX, *(indices+i));
//         viennagrid_index * end = mesh_hierarchy->element_buffer(VIENNAGRID_ELEMENT_TAG_LINE).boundary_indices_end(VIENNAGRID_ELEMENT_TAG_VERTEX, *(indices+i));

        for (; vtx != end; ++vtx)
          vertices.insert(*vtx);
      }

      viennagrid_index * ptr = boundary_buffer(VIENNAGRID_ELEMENT_TAG_VERTEX).push_back( vertices.size() );
      std::copy( vertices.begin(), vertices.end(), ptr );
    }
    else
    {
      viennagrid_index * ptr = boundary_buffer(VIENNAGRID_ELEMENT_TAG_VERTEX).push_back( index_count );
      std::copy( indices, indices+index_count, ptr );
    }

    element_map[ element_key(indices, index_count) ] = id;
  }

  return id;
}

















// viennagrid_index viennagrid_mesh_hierarchy_::delete_element_simple(viennagrid_element_tag element_tag, viennagrid_index element_id)
// {
//
// }
//
// viennagrid_index viennagrid_mesh_hierarchy_::delete_element(viennagrid_element_tag element_tag, viennagrid_index element_id);





