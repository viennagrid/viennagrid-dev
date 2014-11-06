#include "viennagrid/backend/backend.hpp"



void viennagrid_mesh_::make_coboundary(viennagrid_element_tag element_tag, viennagrid_element_tag coboundary_tag)
{
  if (!is_coboundary_obsolete(element_tag, coboundary_tag))
    return;

  viennagrid_index * coboundary_ids = element_handle_buffer(coboundary_tag).ids();
  viennagrid_int coboundary_count = element_handle_buffer(coboundary_tag).count();

  std::map< viennagrid_index, std::vector<viennagrid_index> > tmp;

  viennagrid_element_buffer & hierarchy_coboundary_buffer = mesh_hierarchy()->element_buffer(coboundary_tag);

  for (viennagrid_int i = 0; i < coboundary_count; ++i)
  {
    viennagrid_index coboundary_index = coboundary_ids[i];
    viennagrid_index * boundary_it = hierarchy_coboundary_buffer.boundary_indices_begin(element_tag, coboundary_index);
    viennagrid_index * boundary_end = hierarchy_coboundary_buffer.boundary_indices_end(element_tag, coboundary_index);

    for (; boundary_it != boundary_end; ++boundary_it)
      tmp[*boundary_it].push_back(coboundary_index);
  }


  viennagrid_coboundary_buffer & coboundary_buffer = element_handle_buffer(element_tag).coboundary_buffer(coboundary_tag);

  coboundary_buffer.clear();
  for (std::map< viennagrid_index, std::vector<viennagrid_index> >::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    viennagrid_index * indices = coboundary_buffer.insert( it->first, it->second.size() );
    std::copy( it->second.begin(), it->second.end(), indices );
  }

  set_coboundary_uptodate(element_tag, coboundary_tag);
}



void viennagrid_mesh_::make_boundary_flags(viennagrid_region region)
{
  const viennagrid_element_tag cell_tag = mesh_hierarchy()->cell_tag();
  const viennagrid_element_tag facet_tag = viennagrid_facet_tag(cell_tag);

  if (!region->are_boundary_flags_obsolete())
    return;

  make_coboundary(facet_tag, cell_tag);
  region->clear_boundary(facet_tag);

  for (viennagrid_index * facet = elements_begin(facet_tag); facet != elements_end(facet_tag); ++facet)
  {
    viennagrid_index * it = coboundary_begin(facet_tag, *facet, cell_tag);
    viennagrid_index * cells_end = coboundary_end(facet_tag, *facet, cell_tag);

    viennagrid_int size = 0;
    for (; it != cells_end; ++it)
    {
      if ( mesh_hierarchy()->is_in_region(cell_tag, *it, region->id()) )
        ++size;
    }

    if (size == 1)
    {
      for (viennagrid_element_tag et = VIENNAGRID_ELEMENT_TAG_START; et != facet_tag; ++et)
      {
        if (viennagrid_is_boundary_tag(facet_tag, et) == VIENNAGRID_TRUE)
        {
          viennagrid_index * it = mesh_hierarchy()->element_buffer(facet_tag).boundary_buffer(et).begin(*facet);
          viennagrid_index * boundary_end = mesh_hierarchy()->element_buffer(facet_tag).boundary_buffer(et).end(*facet);
          for (; it != boundary_end; ++it)
          {
            region->set_boundary(et, *it);
          }
        }
      }

      region->set_boundary(facet_tag, *facet);
    }
  }

  region->set_boundary_flags_uptodate();
}


void viennagrid_mesh_::make_boundary_flags()
{
  const viennagrid_element_tag cell_tag = mesh_hierarchy()->cell_tag();
  const viennagrid_element_tag facet_tag = viennagrid_facet_tag(cell_tag);

  if (!are_boundary_flags_obsolete())
    return;

  make_coboundary(facet_tag, cell_tag);
  clear_boundary(facet_tag);

  for (viennagrid_index * facet = elements_begin(facet_tag); facet != elements_end(facet_tag); ++facet)
  {
    viennagrid_index * cells_begin = coboundary_begin(facet_tag, *facet, cell_tag);
    viennagrid_index * cells_end = coboundary_end(facet_tag, *facet, cell_tag);

    viennagrid_int size = cells_end - cells_begin;

    if (size == 1)
    {
      for (viennagrid_element_tag et = VIENNAGRID_ELEMENT_TAG_START; et != facet_tag; ++et)
      {
        if (viennagrid_is_boundary_tag(facet_tag, et) == VIENNAGRID_TRUE)
        {
          viennagrid_index * it = mesh_hierarchy()->element_buffer(facet_tag).boundary_buffer(et).begin(*facet);
          viennagrid_index * boundary_end = mesh_hierarchy()->element_buffer(facet_tag).boundary_buffer(et).end(*facet);
          for (; it != boundary_end; ++it)
          {
            set_boundary(et, *it);
          }
        }
      }

      set_boundary(facet_tag, *facet);
    }
  }

  set_boundary_flags_uptodate();
}






viennagrid_index viennagrid_mesh_hierarchy_::get_make_element(viennagrid_element_tag element_tag,
                                  viennagrid_index * vertex_indices,
                                  viennagrid_int vertex_count)
{
  viennagrid_index id = element_buffer(element_tag).get_element(vertex_indices, vertex_count);
  if (id != -1)
    return id;

  increment_change_counter();

  id = element_buffer(element_tag).make_element(vertex_indices, vertex_count);
  viennagrid_int index = 0;
  viennagrid_index * ptr = 0;

  switch (element_tag)
  {
    case VIENNAGRID_ELEMENT_TAG_VERTEX:
    case VIENNAGRID_ELEMENT_TAG_LINE:
    {
      return id;
    }

    case VIENNAGRID_ELEMENT_TAG_TRIANGLE:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TRIANGLE, VIENNAGRID_ELEMENT_TAG_LINE);
      for (viennagrid_int i = 0; i < 3; ++i)
        for (viennagrid_int j = i+1; j < 3; ++j)
          ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, i, j);

      return id;
    }

    case VIENNAGRID_ELEMENT_TAG_QUADRILATERAL:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, VIENNAGRID_ELEMENT_TAG_LINE);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 1);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 2);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 1, 3);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 2, 3);

      return id;
    }

    case VIENNAGRID_ELEMENT_TAG_TETRAHEDRON:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, VIENNAGRID_ELEMENT_TAG_LINE);
      for (viennagrid_int i = 0; i < 4; ++i)
        for (viennagrid_int j = i+1; j < 4; ++j)
          ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, i, j);

      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, VIENNAGRID_ELEMENT_TAG_TRIANGLE);
      for (viennagrid_int i = 0; i < 4; ++i)
        for (viennagrid_int j = i+1; j < 4; ++j)
          for (viennagrid_int k = j+1; k < 4; ++k)
            ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_TRIANGLE, vertex_indices, i, j, k);

      return id;
    }

    case VIENNAGRID_ELEMENT_TAG_HEXAHEDRON:
    {
      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_HEXAHEDRON, VIENNAGRID_ELEMENT_TAG_LINE);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 1);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 2);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 0, 4);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 1, 3);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 1, 5);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 2, 3);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 2, 5);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 3, 7);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 4, 5);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 4, 6);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 5, 7);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_LINE, vertex_indices, 6, 7);


      index = 0;
      ptr = make_boundary_indices(VIENNAGRID_ELEMENT_TAG_HEXAHEDRON, VIENNAGRID_ELEMENT_TAG_QUADRILATERAL);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 0, 1, 2, 3);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 0, 1, 4, 5);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 0, 2, 4, 6);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 1, 3, 5, 7);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 2, 3, 6, 7);
      ptr[index++] = get_make_element(VIENNAGRID_ELEMENT_TAG_QUADRILATERAL, vertex_indices, 4, 5, 6, 7);

      return id;
    }

    default:
      assert(false);
  }

  return -1;
}




