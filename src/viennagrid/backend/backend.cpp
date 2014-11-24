#include "viennagrid/backend/backend.hpp"



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
  const viennagrid_int cell_topo_dim = topologic_dimension();
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
  const viennagrid_int cell_topo_dim = topologic_dimension();
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
        viennagrid_index * vtx = mesh_hierarchy->element_buffer(VIENNAGRID_ELEMENT_TAG_LINE).boundary_indices_begin(VIENNAGRID_ELEMENT_TAG_VERTEX, *(indices+i));
        viennagrid_index * end = mesh_hierarchy->element_buffer(VIENNAGRID_ELEMENT_TAG_LINE).boundary_indices_end(VIENNAGRID_ELEMENT_TAG_VERTEX, *(indices+i));

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








viennagrid_index viennagrid_mesh_hierarchy_::get_make_element(viennagrid_element_tag element_tag,
                                                              viennagrid_index * indices,
                                                              viennagrid_int count)
{
  viennagrid_int element_topologic_dimension = viennagrid_topological_dimension(element_tag);
  viennagrid_index id = element_buffer(element_topologic_dimension).get_element(indices, count);
  if (id != -1)
    return id;

//   std::cout << "Make element " << viennagrid_element_tag_string(element_tag) << " at " << element_topologic_dimension << std::endl;


  increment_change_counter();

  id = element_buffer(element_topologic_dimension).make_element(this, element_tag, indices, count);
  topologic_dimension_ = std::max( topologic_dimension_, element_topologic_dimension );

  viennagrid_int index = 0;
  viennagrid_index * ptr = 0;

  switch (element_tag)
  {
    case VIENNAGRID_ELEMENT_TAG_VERTEX:
      return id;

    case VIENNAGRID_ELEMENT_TAG_LINE:
      return id;

    case VIENNAGRID_ELEMENT_TAG_TRIANGLE:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TRIANGLE, VIENNAGRID_ELEMENT_TAG_LINE);
      for (viennagrid_int i = 0; i < 3; ++i)
        for (viennagrid_int j = i+1; j < 3; ++j)
          ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, i, j);

      return id;
    }

    case VIENNAGRID_ELEMENT_TAG_QUADRILATERAL:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, VIENNAGRID_ELEMENT_TAG_LINE);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 1);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 2);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 1, 3);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 2, 3);

      return id;
    }

    case VIENNAGRID_ELEMENT_TAG_POLYGON:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_POLYGON, VIENNAGRID_ELEMENT_TAG_LINE, count);
      for (viennagrid_int i = 0; i < count-1; ++i)
        ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, i, i+1);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, count-1, 0);
    }

    case VIENNAGRID_ELEMENT_TAG_PLC:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_PLC, VIENNAGRID_ELEMENT_TAG_LINE, count);

      std::copy( indices, indices+count, ptr );

//       ptr[index++] = indices

//       for (viennagrid_int i = 0; i < count-1; ++i)
//         ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, i, i+1);
//
//       ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, count-1, 0);
    }

    case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, VIENNAGRID_ELEMENT_TAG_LINE);
      for (viennagrid_int i = 0; i < 4; ++i)
        for (viennagrid_int j = i+1; j < 4; ++j)
          ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, i, j);

      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, VIENNAGRID_ELEMENT_TAG_TRIANGLE);
      for (viennagrid_int i = 0; i < 4; ++i)
        for (viennagrid_int j = i+1; j < 4; ++j)
          for (viennagrid_int k = j+1; k < 4; ++k)
            ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_TRIANGLE, indices, i, j, k);

      return id;
    }

    case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_HEXAHEDRON, VIENNAGRID_ELEMENT_TAG_LINE);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 1);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 2);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 0, 4);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 1, 3);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 1, 5);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 2, 3);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 2, 5);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 3, 7);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 4, 5);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 4, 6);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 5, 7);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, indices, 6, 7);


      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_HEXAHEDRON, VIENNAGRID_ELEMENT_TAG_QUADRILATERAL);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 0, 1, 2, 3);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 0, 1, 4, 5);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 0, 2, 4, 6);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 1, 3, 5, 7);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 2, 3, 6, 7);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, indices, 4, 5, 6, 7);

      return id;
    }

    default:
      assert(false);
  }

  return -1;
}








// viennagrid_index viennagrid_mesh_hierarchy_::delete_element_simple(viennagrid_element_tag element_tag, viennagrid_index element_id)
// {
//
// }
//
// viennagrid_index viennagrid_mesh_hierarchy_::delete_element(viennagrid_element_tag element_tag, viennagrid_index element_id);





