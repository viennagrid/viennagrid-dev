#include "viennagrid/backend/mesh_hierarchy.hpp"

viennagrid_index viennagrid_mesh_hierarchy_::get_make_element(viennagrid_element_tag element_tag,
                                                              viennagrid_index * indices,
                                                              viennagrid_dimension * topo_dims,
                                                              viennagrid_int count)
{
  viennagrid_int element_topologic_dimension = viennagrid_topological_dimension(element_tag);
  viennagrid_index id = element_buffer(element_topologic_dimension).get_element(indices, count);
  if (id != -1)
    return id;

//   std::cout << "Make element " << viennagrid_element_tag_string(element_tag) << " at " << element_topologic_dimension << std::endl;


  increment_change_counter();

  id = element_buffer(element_topologic_dimension).make_element(this, element_tag, indices, count);
  cell_dimension_ = std::max( cell_dimension_, element_topologic_dimension );

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
