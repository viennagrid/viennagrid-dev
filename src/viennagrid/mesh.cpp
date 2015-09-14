#include "mesh.hpp"
#include "mesh_hierarchy.hpp"




viennagrid_mesh_::viennagrid_mesh_() : hierarchy_(new viennagrid_mesh_hierarchy_(this)), parent_(0)
{
  hierarchy_->add_mesh(this);
  clear();
}

viennagrid_mesh_::viennagrid_mesh_(viennagrid_mesh_hierarchy_ * hierarchy_in, viennagrid_mesh parent_in) : hierarchy_(hierarchy_in), parent_(parent_in)
{
  hierarchy_in->add_mesh(this);
  clear();
}

viennagrid_mesh_::~viennagrid_mesh_()
{
  for (std::size_t i = 0; i != children.size(); ++i)
    delete children[i];

  if (is_root())
    delete hierarchy_;
}

viennagrid_mesh viennagrid_mesh_::make_child()
{
  // switching to full boundary layout, sparse boundary layout is support for root-only meshes only
  mesh_hierarchy()->set_boundary_layout(VIENNAGRID_BOUNDARY_LAYOUT_FULL);

  viennagrid_mesh mesh = new viennagrid_mesh_( mesh_hierarchy(), this );
  children.push_back( mesh );
  element_children.push_back( viennagrid_element_children_() );
  mesh_children_map[ children.back() ] = children.size()-1;

  return mesh;
}


void viennagrid_mesh_::add_elements(viennagrid_element_id first_id, viennagrid_int count)
{
  viennagrid_dimension topological_dimension = viennagrid_topological_dimension_from_element_id(first_id);
//   viennagrid_element_index index = viennagrid_index_from_element_id(element_id);

  element_handle_buffer(topological_dimension).add_elements( mesh_hierarchy(), first_id, count );
}



void viennagrid_mesh_::add_element(viennagrid_element_id element_id)
{
//   viennagrid_dimension topological_dimension = viennagrid_topological_dimension_from_element_id(element_id);
//   viennagrid_element_index index = viennagrid_index_from_element_id(element_id);

  viennagrid_element_id parent_id = parent(element_id);

  if (TOPODIM(element_id) == cell_dimension())
  {
    if (parent())
    {
      if (parent_id >= 0)
      {
        assert( parent()->element_handle_buffer(TOPODIM(element_id)).element_present(parent_id) ||
                parent()->element_handle_buffer(TOPODIM(element_id)).element_present(element_id) );
      }
      else
      {
        assert( parent()->element_handle_buffer(TOPODIM(element_id)).element_present(element_id) );
      }
    }
    else
    {
      assert( parent_id < 0 );
    }
  }

  element_handle_buffer(TOPODIM(element_id)).add_element(mesh_hierarchy(), element_id);
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







viennagrid_element_id viennagrid_mesh_::make_refined_element(
  viennagrid_element_id           parent_id,
  viennagrid_element_type         refined_element_type,
  viennagrid_int                  refined_element_vertex_count,
  viennagrid_element_id *         refined_element_vertex_ids,
  viennagrid_int                  intersects_count,
  viennagrid_element_id *         intersect_vertices_ids,
  viennagrid_element_id *         intersects_ids)
{
//   viennagrid_dimension topological_dimension = viennagrid_topological_dimension_from_element_id(element_id);
//   viennagrid_element_index index = viennagrid_index_from_element_id(element_id);

  std::vector<viennagrid_int> refined_elements;

  std::map< viennagrid_dimension, std::map<viennagrid_int, std::vector<viennagrid_int> > > intersects;

  for (viennagrid_int i = 0; i != intersects_count; ++i)
  {
    intersects[TOPODIM(intersects_ids[i])][intersects_ids[i]].push_back( intersect_vertices_ids[i] );
  }


  viennagrid_element_id child_element_id = mesh_hierarchy()->get_make_element(
          refined_element_type,
          refined_element_vertex_count,
          refined_element_vertex_ids,
          NULL ).first;



  set_parent( child_element_id, parent_id );
//   mesh_hierarchy()->element_buffer(element_topo_dim).set_parent_id( child_element_id, element_id );


  add_element(child_element_id);
  for (viennagrid_dimension boundary_dim = 0; boundary_dim != TOPODIM(parent_id); ++boundary_dim)
  {
    viennagrid_element_id * boundary = boundary_begin(child_element_id, boundary_dim);
    viennagrid_element_id * end = boundary_end(child_element_id, boundary_dim);

    for (; boundary != end; ++boundary)
    {
      viennagrid_element_id boundary_id = *boundary;
      add_element(boundary_id);

      // no parents for vertices
      if (boundary_dim == 0)
        continue;


      viennagrid_element_id * bv = boundary_begin(boundary_id, 0);
      viennagrid_element_id * bv_end = boundary_end(boundary_id, 0);

      std::vector<viennagrid_int> boundary_vertices(bv_end-bv);

      std::copy(bv, bv_end, boundary_vertices.begin());
      std::sort(boundary_vertices.begin(), boundary_vertices.end());

      if (mesh_hierarchy()->element_buffer(boundary_dim).parent_id(boundary_id) < 0)
      {
        std::map<viennagrid_int, std::vector<viennagrid_int> > & intersected_elements = intersects[boundary_dim];

        for (std::map<viennagrid_int, std::vector<viennagrid_int> >::iterator kt = intersected_elements.begin(); kt != intersected_elements.end(); ++kt)
        {
          viennagrid_int * iv = boundary_begin(kt->first, 0);
          viennagrid_int * iv_end = boundary_end(kt->first, 0);

          std::vector<viennagrid_element_id> intersected_element_ids(iv_end-iv + kt->second.size());

          std::copy(iv, iv_end, intersected_element_ids.begin());
          std::copy(kt->second.begin(), kt->second.end(), &intersected_element_ids[iv_end-iv]);
          std::sort(intersected_element_ids.begin(), intersected_element_ids.end());

          if ( is_subset(boundary_vertices, intersected_element_ids) )
            mesh_hierarchy()->element_buffer(boundary_dim).set_parent_id(boundary_id, kt->first);
        }
      }
    }
  }

  return child_element_id;
}






bool viennagrid_mesh_::is_root() const { return hierarchy_->root() == this; }

viennagrid_dimension viennagrid_mesh_::geometric_dimension() { return hierarchy_->geometric_dimension(); }
viennagrid_dimension viennagrid_mesh_::cell_dimension() { return hierarchy_->cell_dimension(); }

// viennagrid_int * viennagrid_mesh_::boundary_begin(viennagrid_dimension element_topo_dim, viennagrid_int element_id, viennagrid_dimension boundary_topo_dim)
// { return mesh_hierarchy()->boundary_begin(element_topo_dim, element_id, boundary_topo_dim); }
// viennagrid_int * viennagrid_mesh_::boundary_end(viennagrid_dimension element_topo_dim, viennagrid_int element_id, viennagrid_dimension boundary_topo_dim)
// { return mesh_hierarchy()->boundary_end(element_topo_dim, element_id, boundary_topo_dim); }
//

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





bool viennagrid_mesh_::is_element_children_obsolete(viennagrid_int child_mesh_id_, viennagrid_int element_topo_dim)
{ return mesh_hierarchy()->is_obsolete( element_children_change_counter(child_mesh_id_, element_topo_dim) ); }
void viennagrid_mesh_::set_element_children_uptodate(viennagrid_int child_mesh_id_, viennagrid_int element_topo_dim)
{ return mesh_hierarchy()->update_change_counter( element_children_change_counter(child_mesh_id_, element_topo_dim) ); }


void viennagrid_mesh_::make_element_children(viennagrid_mesh child, viennagrid_int element_topo_dim)
{
  viennagrid_int child_mesh_id_ = child_mesh_id(child);

  if (!is_element_children_obsolete(child_mesh_id_, element_topo_dim))
    return;


  std::map<viennagrid_int, std::vector<viennagrid_int> > children;

  viennagrid_int * it       = child->elements_begin(element_topo_dim);
  viennagrid_int * it_end   = child->elements_end(element_topo_dim);

  for (; it != it_end; ++it)
  {
    viennagrid_int parent_id = mesh_hierarchy()->element_buffer(element_topo_dim).parent_id(*it);
    assert( element_handle_buffer(element_topo_dim).element_present(parent_id) );
    children[parent_id].push_back(*it);
  }



  ViennaGridElementChildrenBufferType & element_children_buffer = element_children[child_mesh_id_].children_ids_buffer(element_topo_dim);
  element_children_buffer.clear();



  set_element_children_uptodate(child_mesh_id_, element_topo_dim);
}





void viennagrid_mesh_::make_coboundary(viennagrid_dimension element_topo_dim, viennagrid_dimension coboundary_topo_dim)
{
  if (!is_coboundary_obsolete(element_topo_dim, coboundary_topo_dim))
    return;

  viennagrid_element_id * coboundary_ids = element_handle_buffer(coboundary_topo_dim).ids();
  viennagrid_int coboundary_count = element_handle_buffer(coboundary_topo_dim).count();

  std::map< viennagrid_element_id, std::vector<viennagrid_element_id> > tmp;

  for (viennagrid_int i = 0; i < coboundary_count; ++i)
  {
    viennagrid_element_id coboundary_id = coboundary_ids[i];
    viennagrid_element_id * element_it = boundary_begin(coboundary_id, element_topo_dim);
    viennagrid_element_id * element_end = boundary_end(coboundary_id, element_topo_dim);

    for (; element_it != element_end; ++element_it)
      tmp[*element_it].push_back(coboundary_id);
  }

  ViennaGridCoBoundaryBufferType & coboundary_buffer = element_handle_buffer(element_topo_dim).coboundary_buffer(coboundary_topo_dim);

  coboundary_buffer.clear();
  for (std::map< viennagrid_element_id, std::vector<viennagrid_element_id> >::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    viennagrid_element_id * ids = coboundary_buffer.insert( INDEX(it->first), it->second.size() );
    std::copy( it->second.begin(), it->second.end(), ids );
  }

  set_coboundary_uptodate(element_topo_dim, coboundary_topo_dim);
}




void viennagrid_mesh_::make_neighbor(viennagrid_dimension element_topo_dim, viennagrid_dimension connector_topo_dim, viennagrid_dimension neighbor_topo_dim)
{
  if (!is_neighbor_obsolete(element_topo_dim, connector_topo_dim, neighbor_topo_dim))
    return;

  if ((element_topo_dim > connector_topo_dim) && (connector_topo_dim > neighbor_topo_dim))
    return;

  if ((element_topo_dim == connector_topo_dim) || (connector_topo_dim == neighbor_topo_dim))
    return;

  viennagrid_element_id * element_ids = element_handle_buffer(element_topo_dim).ids();
  viennagrid_int element_count = element_handle_buffer(element_topo_dim).count();


  ViennaGridNeighborBufferType & neighbor_buffer = element_handle_buffer(element_topo_dim).neighbor_buffer(connector_topo_dim, neighbor_topo_dim);
  neighbor_buffer.clear();

  for (viennagrid_int i = 0; i < element_count; ++i)
  {
    viennagrid_element_id element_id = element_ids[i];
    std::set<viennagrid_element_id> tmp;

    viennagrid_element_id * connector_it = connected_begin(element_id, connector_topo_dim);
    viennagrid_element_id * connector_end = connected_end(element_id, connector_topo_dim);

    for (; connector_it != connector_end; ++connector_it)
    {
      viennagrid_element_id connector_id = *connector_it;

      viennagrid_element_id * neighbor_it = connected_begin(connector_id, neighbor_topo_dim);
      viennagrid_element_id * neighbor_end = connected_end(connector_id, neighbor_topo_dim);


      for (; neighbor_it != neighbor_end; ++neighbor_it)
      {
        viennagrid_element_id neighbor_id = *neighbor_it;
        if (element_id == neighbor_id)
          continue;

        tmp.insert( neighbor_id );
      }
    }

    if (element_topo_dim > neighbor_topo_dim)
    {
      viennagrid_element_id * it = boundary_begin(element_id, neighbor_topo_dim);
      viennagrid_element_id * end = boundary_begin(element_id, neighbor_topo_dim);

      for (; it != end; ++it)
        tmp.erase(*it);
    }


    viennagrid_element_id * ids = neighbor_buffer.insert( INDEX(element_id), tmp.size() );
    std::copy( tmp.begin(), tmp.end(), ids );
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

  for (viennagrid_element_id * facet = elements_begin(facet_topo_dim); facet != elements_end(facet_topo_dim); ++facet)
  {
    viennagrid_element_id * it = coboundary_begin(*facet, cell_topo_dim);
    viennagrid_element_id * cells_end = coboundary_end(*facet, cell_topo_dim);

    viennagrid_int size = 0;
    for (; it != cells_end; ++it)
    {
      if ( mesh_hierarchy()->is_in_region(*it, region->id()) )
        ++size;
    }

    if (size == 1)
    {
      for (viennagrid_int boundary_topo_dim = 0; boundary_topo_dim != facet_topo_dim; ++boundary_topo_dim)
      {
        if (viennagrid_is_boundary_type(facet_topo_dim, boundary_topo_dim) == VIENNAGRID_TRUE)
        {
          viennagrid_element_id * it = mesh_hierarchy()->boundary_begin(*facet, boundary_topo_dim);
          viennagrid_element_id * boundary_end = mesh_hierarchy()->boundary_end(*facet, boundary_topo_dim);
          for (; it != boundary_end; ++it)
          {
            region->set_boundary(*it);
          }
        }
      }

      region->set_boundary(*facet);
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

  for (viennagrid_element_id * facet = elements_begin(facet_topo_dim); facet != elements_end(facet_topo_dim); ++facet)
  {
    viennagrid_element_id * cells_begin = coboundary_begin(*facet, cell_topo_dim);
    viennagrid_element_id * cells_end = coboundary_end(*facet, cell_topo_dim);

    viennagrid_int size = cells_end - cells_begin;

    if (size == 1)
    {
      for (viennagrid_int boundary_topo_dim = 0; boundary_topo_dim != facet_topo_dim; ++boundary_topo_dim)
      {
        if (viennagrid_is_boundary_type(facet_topo_dim, boundary_topo_dim) == VIENNAGRID_TRUE)
        {
          viennagrid_element_id * it = mesh_hierarchy()->boundary_begin(*facet, boundary_topo_dim);
          viennagrid_element_id * boundary_end = mesh_hierarchy()->boundary_end(*facet, boundary_topo_dim);
          for (; it != boundary_end; ++it)
          {
            set_boundary(*it);
          }
        }
      }

      set_boundary(*facet);
    }
  }

  set_boundary_flags_uptodate();
}

void viennagrid_mesh_::clear()
{
  if (is_root())
    mesh_hierarchy()->clear();

  children.clear();
  element_children.clear();
  mesh_children_map.clear();

  for (viennagrid_int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
  {
    element_handle_buffers[i].clear(i);
  }

  for (viennagrid_int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
    for (viennagrid_int j = 0; j != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++j)
      coboundary_change_counters[i][j] = 0;

  for (viennagrid_int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
    for (viennagrid_int j = 0; j != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++j)
      for (viennagrid_int k = 0; k != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++k)
        neighbor_change_counters[i][j][k] = 0;

  for (viennagrid_int i = 0; i != VIENNAGRID_TOPOLOGIC_DIMENSION_END; ++i)
  {
    boundary_elements_[i].clear();
  }

  boundary_elements_change_counter = 0;
}



