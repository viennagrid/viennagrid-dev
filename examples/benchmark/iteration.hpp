#ifndef VIENNAGRID_BENCHMARK_ITERATION_HPP
#define VIENNAGRID_BENCHMARK_ITERATION_HPP

#include "viennagrid/algorithm/volume.hpp"

template<typename PointT1, typename PointT2>
viennagrid_numeric signed_spanned_volume(PointT1 const & p1,
                                         PointT2 const & p2,
                                         std::size_t size)
{
  if (size == 1)
    return p2[0] - p1[0];
  else if (size == 2)
    return sqrt(   (p2[0] - p1[0]) * (p2[0] - p1[0])
                  + (p2[1] - p1[1]) * (p2[1] - p1[1])  );
  else if (size == 3)
    return sqrt(   (p2[0] - p1[0]) * (p2[0] - p1[0])
                  + (p2[1] - p1[1]) * (p2[1] - p1[1])
                  + (p2[2] - p1[2]) * (p2[2] - p1[2]) );
  return 0;
}

template<typename PointT1, typename PointT2, typename PointT3>
viennagrid_numeric signed_spanned_volume(PointT1 const & p1,
                                         PointT2 const & p2,
                                         PointT3 const & p3,
                                         std::size_t size)
{
  if (size == 2)
  {
    //a triangle:
    return (  p1[0] * (p2[1] - p3[1])
            + p2[0] * (p3[1] - p1[1])
            + p3[0] * (p1[1] - p2[1]) ) / 2.0;
  }
  else if (size == 3)
  {
    viennagrid_numeric v1[3];
    viennagrid_numeric v2[3];
    viennagrid_numeric v3[3];

    for (int i = 0; i != 3; ++i)
    {
      v1[i] = p2[i] - p1[i];
      v2[i] = p3[i] - p1[i];
    }

    v3[0] = v1[1]*v2[2] - v1[2]*v2[1];
    v3[1] = v1[2]*v2[0] - v1[0]*v2[2];
    v3[2] = v1[0]*v2[1] - v1[1]*v2[0];

    return std::sqrt( v3[0]*v3[0] + v3[1]*v3[1] + v3[2]*v3[2] ) / 2.0;
  }

  return 0;
}

template<typename PointT1, typename PointT2, typename PointT3, typename PointT4>
viennagrid_numeric signed_spanned_volume(PointT1 const & p1,
                                         PointT2 const & p2,
                                         PointT3 const & p3,
                                         PointT4 const & p4,
                                         std::size_t size)
{
  if (size == 3)
  {
    viennagrid_numeric v1[3];
    viennagrid_numeric v2[3];
    viennagrid_numeric v3[3];
    viennagrid_numeric v4[3];

    for (int i = 0; i != 3; ++i)
    {
      v1[i] = p2[i] - p1[i];
      v2[i] = p3[i] - p1[i];
      v3[i] = p4[i] - p1[i];
    }


    v4[0] = v2[1]*v3[2] - v2[2]*v3[1];
    v4[1] = v2[2]*v3[0] - v2[0]*v3[2];
    v4[2] = v2[0]*v3[1] - v2[1]*v3[0];

    return ( v1[0]*v4[0] + v1[1]*v4[1] + v1[2]*v4[2] ) / 6.0;
  }

  return 0;
}






template<typename PointT1, typename PointT2>
viennagrid_numeric spanned_volume(PointT1 const & p1,
                                  PointT2 const & p2,
                                  std::size_t size)
{
  return std::abs( signed_spanned_volume(p1, p2, size) );
}

template<typename PointT1, typename PointT2, typename PointT3>
viennagrid_numeric spanned_volume(PointT1 const & p1,
                                  PointT2 const & p2,
                                  PointT3 const & p3,
                                  std::size_t size)
{
  return std::abs( signed_spanned_volume(p1, p2, p3, size) );
}

template<typename PointT1, typename PointT2, typename PointT3, typename PointT4>
viennagrid_numeric spanned_volume(PointT1 const & p1,
                                  PointT2 const & p2,
                                  PointT3 const & p3,
                                  PointT4 const & p4,
                                  std::size_t size)
{
  return std::abs( signed_spanned_volume(p1, p2, p3, p4, size) );
}









viennagrid_numeric volume(viennagrid_mesh mesh,
                          viennagrid_dimension geometry_dimension,
                          viennagrid_element_type element_type,
                          viennagrid_element_id * vertices_start,
                          viennagrid_element_id * /*vertices_end*/)
{
  viennagrid_numeric * coords;
  viennagrid_mesh_vertex_coords_pointer(mesh, &coords);

  switch (element_type)
  {
    case VIENNAGRID_ELEMENT_TYPE_LINE:
    {
      viennagrid_numeric * p0 = coords + *(vertices_start+0) * geometry_dimension;
      viennagrid_numeric * p1 = coords + *(vertices_start+1) * geometry_dimension;

      return spanned_volume(p0, p1, geometry_dimension);
    }
    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
    {
      viennagrid_numeric * p0 = coords + *(vertices_start+0) * geometry_dimension;
      viennagrid_numeric * p1 = coords + *(vertices_start+1) * geometry_dimension;
      viennagrid_numeric * p2 = coords + *(vertices_start+2) * geometry_dimension;

      return spanned_volume(p0, p1, p2, geometry_dimension);
    }
    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
    {
      viennagrid_numeric * p0 = coords + *(vertices_start+0) * geometry_dimension;
      viennagrid_numeric * p1 = coords + *(vertices_start+1) * geometry_dimension;
      viennagrid_numeric * p2 = coords + *(vertices_start+2) * geometry_dimension;
      viennagrid_numeric * p3 = coords + *(vertices_start+3) * geometry_dimension;

      return spanned_volume(p0, p1, p2, p3, geometry_dimension);
    }
  }

  return -1;
}


viennagrid_numeric volume(viennagrid_mesh mesh,
                          viennagrid_element_id element_id)
{
  viennagrid_element_type element_type;
  viennagrid_element_type_get(mesh, element_id, &element_type);

  viennagrid_dimension geometry_dimension;
  viennagrid_mesh_geometric_dimension_get(mesh, &geometry_dimension);

  viennagrid_element_id * vertices_start;
  viennagrid_element_id * vertices_end;

  viennagrid_element_boundary_elements(mesh, element_id, 0, &vertices_start, &vertices_end);

  return volume(mesh, geometry_dimension, element_type, vertices_start, vertices_end);
}











viennagrid_numeric volume_CPP(viennagrid::mesh const & mesh)
{
  typedef viennagrid::mesh MeshType;
  typedef viennagrid::result_of::element<MeshType>::type ElementType;

  typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
  typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

  viennagrid_numeric sum = 0;

  ConstElementRangeType cells(mesh, viennagrid::cell_dimension(mesh));
  for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
  {
    sum += viennagrid::volume(*cit);
  }

//   std::cout << sum << std::endl;
  return sum;
}

viennagrid_numeric volume_C(viennagrid::mesh const & mesh_)
{
  viennagrid_mesh mesh = mesh_.internal();

  viennagrid_dimension cell_dimension;
  viennagrid_mesh_cell_dimension_get(mesh, &cell_dimension);

  viennagrid_element_id * cells_start;
  viennagrid_element_id * cells_end;
  viennagrid_mesh_elements_get(mesh, cell_dimension, &cells_start, &cells_end);

  viennagrid_numeric sum = 0;
  for (viennagrid_element_id * cell_id = cells_start; cell_id != cells_end; ++cell_id)
  {
    sum += volume(mesh, *cell_id);
  }

//   std::cout << sum << std::endl;
  return sum;
}

viennagrid_numeric volume_C_pure(viennagrid::mesh const & mesh_)
{
  viennagrid_mesh mesh = mesh_.internal();

  viennagrid_dimension cell_dimension;
  viennagrid_mesh_cell_dimension_get(mesh, &cell_dimension);

  viennagrid_dimension geometry_dimension;
  viennagrid_mesh_geometric_dimension_get(mesh, &geometry_dimension);

  viennagrid_element_id * cells_start;
  viennagrid_element_id * cells_end;
  viennagrid_mesh_elements_get(mesh, cell_dimension, &cells_start, &cells_end);


  viennagrid_element_id * boundary_offsets;
  viennagrid_element_id * boundary_indices;
  viennagrid_element_boundary_element_pointers(mesh, cell_dimension, 0, &boundary_offsets, &boundary_indices);


  viennagrid_numeric sum = 0;
  for (viennagrid_element_id * cell_id = cells_start; cell_id != cells_end; ++cell_id)
  {
    viennagrid_element_type element_type;
    viennagrid_element_type_get(mesh, *cell_id, &element_type);

    sum += volume(mesh, geometry_dimension, element_type,
                  boundary_indices + boundary_offsets[*cell_id],
                  boundary_indices + boundary_offsets[*cell_id+1]);
  }

//   std::cout << sum << std::endl;
  return sum;
}

viennagrid_numeric volume_mixed_CPP_C(viennagrid::mesh const & mesh_)
{
  typedef viennagrid::mesh                                                  MeshType;
  typedef viennagrid::result_of::element<MeshType>::type                    ElementType;

  typedef viennagrid::result_of::const_element_range<MeshType>::type        ConstElementRangeType;
  typedef viennagrid::result_of::iterator<ConstElementRangeType>::type      ConstElementIteratorType;

  viennagrid_mesh mesh = mesh_.internal();

  viennagrid_numeric sum = 0;

  ConstElementRangeType cells(mesh, viennagrid::cell_dimension(mesh));
  for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
  {
    sum += volume(mesh, (*cit).id());
  }

//   std::cout << sum << std::endl;
  return sum;
}



























viennagrid_int iteration_CPP(viennagrid::mesh const & mesh)
{
  typedef viennagrid::mesh                                                MeshType;
  typedef viennagrid::result_of::element<MeshType>::type                  ElementType;

  typedef viennagrid::result_of::const_element_range<MeshType>::type      ConstElementRangeType;
  typedef viennagrid::result_of::iterator<ConstElementRangeType>::type    ConstElementIteratorType;

  static viennagrid_int sum = 0;

  ConstElementRangeType cells(mesh, viennagrid::cell_dimension(mesh));
  for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
  {
    sum += (*cit).id();
  }

//   std::cout << sum << std::endl;
  return sum;
}

viennagrid_int iteration_C(viennagrid::mesh const & mesh_)
{
  viennagrid_mesh mesh = mesh_.internal();

  viennagrid_dimension cell_dimension;
  viennagrid_mesh_cell_dimension_get(mesh, &cell_dimension);

  viennagrid_element_id * cells_start;
  viennagrid_element_id * cells_end;
  viennagrid_mesh_elements_get(mesh, cell_dimension, &cells_start, &cells_end);

  static viennagrid_int sum = 0;
  for (viennagrid_element_id * cell_id = cells_start; cell_id != cells_end; ++cell_id)
  {
    sum += *cell_id;
  }

//   std::cout << sum << std::endl;
  return sum;
}









viennagrid_int boundary_iteration_CPP(viennagrid::mesh const & mesh, int boundary_dimension)
{
  typedef viennagrid::mesh                                                  MeshType;
  typedef viennagrid::result_of::element<MeshType>::type                    ElementType;

  typedef viennagrid::result_of::const_element_range<MeshType>::type        ConstElementRangeType;
  typedef viennagrid::result_of::iterator<ConstElementRangeType>::type      ConstElementIteratorType;

  typedef viennagrid::result_of::const_element_range<ElementType>::type     ConstBoundaryRangeType;
  typedef viennagrid::result_of::iterator<ConstBoundaryRangeType>::type     ConstBoundaryIteratorType;

  static viennagrid_int sum = 0;

  ConstElementRangeType cells(mesh, viennagrid::cell_dimension(mesh));
  for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
  {
    ConstBoundaryRangeType boundary_elements(*cit, boundary_dimension);
    for (ConstBoundaryIteratorType bit = boundary_elements.begin(); bit != boundary_elements.end(); ++bit)
      sum += (*bit).id();
  }

//   std::cout << sum << std::endl;
  return sum;
}

viennagrid_int boundary_iteration_C(viennagrid::mesh const & mesh_, int boundary_dimension)
{
  viennagrid_mesh mesh = mesh_.internal();

  viennagrid_dimension cell_dimension;
  viennagrid_mesh_cell_dimension_get(mesh, &cell_dimension);

  viennagrid_element_id * cells_start;
  viennagrid_element_id * cells_end;
  viennagrid_mesh_elements_get(mesh, cell_dimension, &cells_start, &cells_end);

  static viennagrid_int sum = 0;
  for (viennagrid_element_id * cell_id = cells_start; cell_id != cells_end; ++cell_id)
  {
    viennagrid_element_id * boundary_start;
    viennagrid_element_id * boundary_end;

    viennagrid_element_boundary_elements(mesh, *cell_id, boundary_dimension, &boundary_start, &boundary_end);

    for (viennagrid_element_id * boundary_id = boundary_start; boundary_id != boundary_end; ++boundary_id)
      sum += *boundary_id;
  }

//   std::cout << sum << std::endl;
  return sum;
}

viennagrid_int boundary_iteration_C_pure(viennagrid::mesh const & mesh_, int boundary_dimension)
{
  viennagrid_mesh mesh = mesh_.internal();

  viennagrid_dimension cell_dimension;
  viennagrid_mesh_cell_dimension_get(mesh, &cell_dimension);

  viennagrid_dimension geometry_dimension;
  viennagrid_mesh_geometric_dimension_get(mesh, &geometry_dimension);

  viennagrid_element_id * cells_start;
  viennagrid_element_id * cells_end;
  viennagrid_mesh_elements_get(mesh, cell_dimension, &cells_start, &cells_end);


  viennagrid_element_id * boundary_offsets;
  viennagrid_element_id * boundary_indices;
  viennagrid_element_boundary_element_pointers(mesh, cell_dimension, boundary_dimension, &boundary_offsets, &boundary_indices);


  viennagrid_int sum = 0;
  for (viennagrid_element_id * cell_id = cells_start; cell_id != cells_end; ++cell_id)
  {
    for (viennagrid_element_id * boundary_id = boundary_indices + boundary_offsets[*cell_id];
                            boundary_id != boundary_indices + boundary_offsets[*cell_id+1];
                          ++boundary_id)
    {
      sum += *boundary_id;
    }
  }

  return sum;
}


#endif
