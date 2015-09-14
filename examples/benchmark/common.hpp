#ifndef VIENNAGRID_BENCHMARK_COMMON_HPP
#define VIENNAGRID_BENCHMARK_COMMON_HPP

#include <vector>
#include "viennautils/timer.hpp"
#include "boost/bind.hpp"








void make_triangle(viennagrid_mesh mesh,
                   viennagrid_element_id * indices,
                   viennagrid_element_id * cells)
{
  viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, indices+0, cells+0);
  viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TRIANGLE, 3, indices+1, cells+1);
}

template<typename ElementT>
void make_triangle(viennagrid::mesh const & mesh,
                   ElementT * indices,
                   ElementT * cells)
{
  cells[0] = viennagrid::make_element(mesh, viennagrid::triangle_tag(), indices, indices+3);
  cells[1] = viennagrid::make_element(mesh, viennagrid::triangle_tag(), indices+1, indices+4);
}

void make_tetrahedron(viennagrid_mesh mesh,
                      viennagrid_element_id * indices,
                      viennagrid_element_id * cells)
{
  viennagrid_element_id local_indices[4];

  local_indices[0] = indices[0];
  local_indices[1] = indices[2];
  local_indices[2] = indices[6];
  local_indices[3] = indices[7];
  viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, local_indices, cells+0);

  local_indices[0] = indices[0];
  local_indices[1] = indices[2];
  local_indices[2] = indices[3];
  local_indices[3] = indices[7];
  viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, local_indices, cells+1);

  local_indices[0] = indices[0];
  local_indices[1] = indices[1];
  local_indices[2] = indices[3];
  local_indices[3] = indices[7];
  viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, local_indices, cells+2);

  local_indices[0] = indices[0];
  local_indices[1] = indices[1];
  local_indices[2] = indices[5];
  local_indices[3] = indices[7];
  viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, local_indices, cells+3);

  local_indices[0] = indices[0];
  local_indices[1] = indices[4];
  local_indices[2] = indices[5];
  local_indices[3] = indices[7];
  viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, local_indices, cells+4);

  local_indices[0] = indices[0];
  local_indices[1] = indices[4];
  local_indices[2] = indices[6];
  local_indices[3] = indices[7];
  viennagrid_mesh_element_create(mesh, VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON, 4, local_indices, cells+5);
}

template<typename ElementT>
void make_tetrahedron(viennagrid::mesh const & mesh,
                      ElementT * indices,
                      ElementT * cells)
{
  ElementT local_indices[4];

  local_indices[0] = indices[0];
  local_indices[1] = indices[2];
  local_indices[2] = indices[6];
  local_indices[3] = indices[7];
  cells[0] = viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

  local_indices[0] = indices[0];
  local_indices[1] = indices[2];
  local_indices[2] = indices[3];
  local_indices[3] = indices[7];
  cells[1] = viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

  local_indices[0] = indices[0];
  local_indices[1] = indices[1];
  local_indices[2] = indices[3];
  local_indices[3] = indices[7];
  cells[2] = viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

  local_indices[0] = indices[0];
  local_indices[1] = indices[1];
  local_indices[2] = indices[5];
  local_indices[3] = indices[7];
  cells[3] = viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

  local_indices[0] = indices[0];
  local_indices[1] = indices[4];
  local_indices[2] = indices[5];
  local_indices[3] = indices[7];
  cells[4] = viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

  local_indices[0] = indices[0];
  local_indices[1] = indices[4];
  local_indices[2] = indices[6];
  local_indices[3] = indices[7];
  cells[5] = viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);
}











void make_aabb_triangles_C_full(viennagrid_mesh mesh,
                           viennagrid_numeric ll_x, viennagrid_numeric ll_y,
                           viennagrid_numeric ur_x, viennagrid_numeric ur_y,
                           int point_count_x, int point_count_y,
                           bool make_boundary)
{
  viennagrid_mesh_geometric_dimension_set(mesh, 2);

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);

  std::vector<viennagrid_element_id> vertex_indices(point_count_x*point_count_y);
  int i = 0;

  double p[2];
  p[1] = ll_y;
  for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
  {
    p[0] = ll_x;
    for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x, ++i)
    {
      viennagrid_mesh_vertex_create(mesh, p, &vertex_indices[i]);
    }
  }

  for (int yi = 0; yi != point_count_y-1; ++yi)
    for (int xi = 0; xi != point_count_x-1; ++xi)
    {
      viennagrid_element_id indices[4];
      indices[0] = vertex_indices[(yi+0)*point_count_y+(xi+0)];
      indices[1] = vertex_indices[(yi+0)*point_count_y+(xi+1)];
      indices[2] = vertex_indices[(yi+1)*point_count_y+(xi+0)];
      indices[3] = vertex_indices[(yi+1)*point_count_y+(xi+1)];

      viennagrid_element_id cells[2];
      make_triangle(mesh, indices, cells);
    }

  if (make_boundary)
    viennagrid_mesh_property_set(mesh, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void make_aabb_triangles_C(viennagrid::mesh const & mesh, int cell_count, bool make_boundary)
{
  int point_count = std::sqrt(cell_count/2)+1.5;
  make_aabb_triangles_C_full(mesh.internal(), -1.0, -1.0, 1.0, 1.0, point_count, point_count, make_boundary);
}



void make_aabb_triangles_CPP_full(viennagrid::mesh const & mesh,
                                  viennagrid_numeric ll_x, viennagrid_numeric ll_y,
                                  viennagrid_numeric ur_x, viennagrid_numeric ur_y,
                                  int point_count_x, int point_count_y,
                                  bool make_boundary)
{
  typedef viennagrid::mesh                                  MeshType;
  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    ElementType;

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);

  std::vector<ElementType> vertex_indices(point_count_x*point_count_y);
  int i = 0;

  PointType p(2);
  p[1] = ll_y;
  for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
  {
    p[0] = ll_x;
    for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x, ++i)
    {
      vertex_indices[i] = viennagrid::make_vertex(mesh, p);
    }
  }

  for (int yi = 0; yi != point_count_y-1; ++yi)
    for (int xi = 0; xi != point_count_x-1; ++xi)
    {
      ElementType indices[4];
      indices[0] = vertex_indices[(yi+0)*point_count_y+(xi+0)];
      indices[1] = vertex_indices[(yi+0)*point_count_y+(xi+1)];
      indices[2] = vertex_indices[(yi+1)*point_count_y+(xi+0)];
      indices[3] = vertex_indices[(yi+1)*point_count_y+(xi+1)];

      ElementType cells[2];
      make_triangle(mesh, indices, cells);
    }

  if (make_boundary)
    viennagrid_mesh_property_set(mesh.internal(), VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void make_aabb_triangles_CPP(viennagrid::mesh const & mesh, int cell_count, bool make_boundary)
{
  int point_count = std::sqrt(cell_count/2)+1.5;
  make_aabb_triangles_CPP_full(mesh, -1.0, -1.0, 1.0, 1.0, point_count, point_count, make_boundary);
}




void make_aabb_tetrahedrons_C_full(viennagrid_mesh mesh,
                                   viennagrid_numeric ll_x, viennagrid_numeric ll_y, viennagrid_numeric ll_z,
                                   viennagrid_numeric ur_x, viennagrid_numeric ur_y, viennagrid_numeric ur_z,
                                   int point_count_x, int point_count_y, int point_count_z,
                                   bool make_boundary)
{
  viennagrid_mesh_geometric_dimension_set(mesh, 3);

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);
  viennagrid_numeric step_z = (ur_z-ll_z) / (point_count_z-1);

  std::vector<viennagrid_element_id> vertex_indices(point_count_x*point_count_y*point_count_z);
  int i = 0;

  double p[3];
  p[2] = ll_z;
  for (int zi = 0; zi != point_count_z; ++zi, p[2] += step_z)
  {
    p[1] = ll_y;
    for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
    {
      p[0] = ll_x;
      for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x, ++i)
      {
        viennagrid_mesh_vertex_create(mesh, p, &vertex_indices[i]);
      }
    }
  }

  for (int zi = 0; zi != point_count_z-1; ++zi)
    for (int yi = 0; yi != point_count_y-1; ++yi)
      for (int xi = 0; xi != point_count_x-1; ++xi)
      {
        viennagrid_element_id indices[8];
        indices[0] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[1] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[2] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[3] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];
        indices[4] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[5] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[6] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[7] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];

        viennagrid_element_id cells[6];
        make_tetrahedron(mesh, indices, cells);
      }

  if (make_boundary)
    viennagrid_mesh_property_set(mesh, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void make_aabb_tetrahedrons_C(viennagrid::mesh const & mesh, int cell_count, bool make_boundary)
{
  int point_count = std::pow(cell_count/6, 1.0/3.0)+1.5;
  make_aabb_tetrahedrons_C_full(mesh.internal(), -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, point_count, point_count, point_count, make_boundary);
}





void make_aabb_tetrahedrons_CPP_full(viennagrid::mesh const & mesh,
                                     viennagrid_numeric ll_x, viennagrid_numeric ll_y, viennagrid_numeric ll_z,
                                     viennagrid_numeric ur_x, viennagrid_numeric ur_y, viennagrid_numeric ur_z,
                                     int point_count_x, int point_count_y, int point_count_z,
                                     bool make_boundary)
{
  typedef viennagrid::mesh                                  MeshType;
  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    ElementType;

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);
  viennagrid_numeric step_z = (ur_z-ll_z) / (point_count_z-1);

  std::vector<ElementType> vertex_indices(point_count_x*point_count_y*point_count_z);
  int i = 0;

  PointType p(3);
  p[2] = ll_z;
  for (int zi = 0; zi != point_count_z; ++zi, p[2] += step_z)
  {
    p[1] = ll_y;
    for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
    {
      p[0] = ll_x;
      for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x, ++i)
      {
        vertex_indices[i] = viennagrid::make_vertex(mesh, p);
      }
    }
  }

  for (int zi = 0; zi != point_count_z-1; ++zi)
    for (int yi = 0; yi != point_count_y-1; ++yi)
      for (int xi = 0; xi != point_count_x-1; ++xi)
      {
        ElementType indices[8];
        indices[0] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[1] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[2] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[3] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];
        indices[4] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[5] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[6] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[7] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];

        ElementType cells[6];
        make_tetrahedron(mesh, indices, cells);
      }

  if (make_boundary)
    viennagrid_mesh_property_set(mesh.internal(), VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void make_aabb_tetrahedrons_CPP(viennagrid::mesh const & mesh, int cell_count, bool make_boundary)
{
  int point_count = std::pow(cell_count/6, 1.0/3.0)+1.5;
  make_aabb_tetrahedrons_CPP_full(mesh.internal(), -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, point_count, point_count, point_count, make_boundary);
}





























void make_aabb_triangles_region_C_full(viennagrid_mesh mesh,
                           viennagrid_numeric ll_x, viennagrid_numeric ll_y,
                           viennagrid_numeric ur_x, viennagrid_numeric ur_y,
                           int point_count_x, int point_count_y,
                           int region_count, bool make_boundary)
{
  viennagrid_mesh_geometric_dimension_set(mesh, 2);

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);

  std::vector<viennagrid_region> regions(region_count);

  for (int i = 0; i != region_count; ++i)
    viennagrid_mesh_region_get_or_create(mesh, i%region_count, &regions[i%region_count]);

  std::vector<viennagrid_element_id> vertex_indices(point_count_x*point_count_y);
  int i = 0;

  double p[2];
  p[1] = ll_y;
  for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
  {
    p[0] = ll_x;
    for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x)
    {
      viennagrid_mesh_vertex_create(mesh, p, &vertex_indices[i++]);
    }
  }

  i = 0;
  for (int yi = 0; yi != point_count_y-1; ++yi)
    for (int xi = 0; xi != point_count_x-1; ++xi)
    {
      viennagrid_element_id indices[4];
      indices[0] = vertex_indices[(yi+0)*point_count_y+(xi+0)];
      indices[1] = vertex_indices[(yi+0)*point_count_y+(xi+1)];
      indices[2] = vertex_indices[(yi+1)*point_count_y+(xi+0)];
      indices[3] = vertex_indices[(yi+1)*point_count_y+(xi+1)];

      viennagrid_element_id cells[2];
      make_triangle(mesh, indices, cells);

      viennagrid_region_element_add( regions[(i++)%region_count], cells[0] );
      viennagrid_region_element_add( regions[(i++)%region_count], cells[1] );
    }

  if (make_boundary)
    viennagrid_mesh_property_set(mesh, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void make_aabb_triangles_region_C(viennagrid::mesh const & mesh, int cell_count, int region_count, bool make_boundary)
{
  int point_count = std::sqrt(cell_count/2)+1.5;
  make_aabb_triangles_region_C_full(mesh.internal(), -1.0, -1.0, 1.0, 1.0, point_count, point_count, region_count, make_boundary);
}



void make_aabb_triangles_region_CPP_full(viennagrid::mesh const & mesh,
                                         viennagrid_numeric ll_x, viennagrid_numeric ll_y,
                                         viennagrid_numeric ur_x, viennagrid_numeric ur_y,
                                         int point_count_x, int point_count_y,
                                         int region_count, bool make_boundary)
{
  typedef viennagrid::mesh                                  MeshType;
  typedef viennagrid::result_of::region<MeshType>::type     RegionType;

  typedef viennagrid::result_of::point<MeshType>::type      PointType;
  typedef viennagrid::result_of::element<MeshType>::type    ElementType;

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);

  std::vector<RegionType> regions;
  for (int i = 0; i != region_count; ++i)
    regions.push_back( mesh.create_region() );

  std::vector<ElementType> vertex_indices(point_count_x*point_count_y);
  int i = 0;

  PointType p(2);
  p[1] = ll_y;
  for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
  {
    p[0] = ll_x;
    for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x)
    {
      vertex_indices[i++] = viennagrid::make_vertex(mesh, p);
    }
  }

  i = 0;
  for (int yi = 0; yi != point_count_y-1; ++yi)
    for (int xi = 0; xi != point_count_x-1; ++xi)
    {
      ElementType indices[4];
      indices[0] = vertex_indices[(yi+0)*point_count_y+(xi+0)];
      indices[1] = vertex_indices[(yi+0)*point_count_y+(xi+1)];
      indices[2] = vertex_indices[(yi+1)*point_count_y+(xi+0)];
      indices[3] = vertex_indices[(yi+1)*point_count_y+(xi+1)];

      ElementType cells[2];
      make_triangle(mesh, indices, cells);

      viennagrid::add(regions[(i++)%region_count], cells[0]);
      viennagrid::add(regions[(i++)%region_count], cells[1]);
    }

  if (make_boundary)
    viennagrid_mesh_property_set(mesh.internal(), VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void make_aabb_triangles_region_CPP(viennagrid::mesh const & mesh, int cell_count, int region_count, bool make_boundary)
{
  int point_count = std::sqrt(cell_count/2)+1.5;
  make_aabb_triangles_region_CPP_full(mesh, -1.0, -1.0, 1.0, 1.0, point_count, point_count, region_count, make_boundary);
}




void make_aabb_tetrahedrons_region_C_full(viennagrid_mesh mesh,
                                          viennagrid_numeric ll_x, viennagrid_numeric ll_y, viennagrid_numeric ll_z,
                                          viennagrid_numeric ur_x, viennagrid_numeric ur_y, viennagrid_numeric ur_z,
                                          int point_count_x, int point_count_y, int point_count_z,
                                          int region_count, bool make_boundary)
{
  viennagrid_mesh_geometric_dimension_set(mesh, 3);

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);
  viennagrid_numeric step_z = (ur_z-ll_z) / (point_count_z-1);

  std::vector<viennagrid_region> regions;
  for (int i = 0; i != region_count; ++i)
    viennagrid_mesh_region_get_or_create(mesh, i%region_count, &regions[i%region_count]);

  std::vector<viennagrid_element_id> vertex_indices(point_count_x*point_count_y*point_count_z);
  int i = 0;

  double p[3];
  p[2] = ll_z;
  for (int zi = 0; zi != point_count_z; ++zi, p[2] += step_z)
  {
    p[1] = ll_y;
    for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
    {
      p[0] = ll_x;
      for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x)
      {
        viennagrid_mesh_vertex_create(mesh, p, &vertex_indices[i++]);
      }
    }
  }

  for (int zi = 0; zi != point_count_z-1; ++zi)
    for (int yi = 0; yi != point_count_y-1; ++yi)
      for (int xi = 0; xi != point_count_x-1; ++xi)
      {
        viennagrid_element_id indices[8];
        indices[0] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[1] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[2] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[3] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];
        indices[4] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[5] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[6] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[7] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];

        viennagrid_element_id cells[6];
        make_tetrahedron(mesh, indices, cells);

        viennagrid_region_element_add( regions[(i++)%region_count], cells[0] );
        viennagrid_region_element_add( regions[(i++)%region_count], cells[1] );
        viennagrid_region_element_add( regions[(i++)%region_count], cells[2] );
        viennagrid_region_element_add( regions[(i++)%region_count], cells[3] );
        viennagrid_region_element_add( regions[(i++)%region_count], cells[4] );
        viennagrid_region_element_add( regions[(i++)%region_count], cells[5] );
      }

  if (make_boundary)
    viennagrid_mesh_property_set(mesh, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void make_aabb_tetrahedrons_region_C(viennagrid::mesh const & mesh, int cell_count, int region_count, bool make_boundary)
{
  int point_count = std::pow(cell_count/6, 1.0/3.0)+1.5;
  make_aabb_tetrahedrons_region_C_full(mesh.internal(), -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, point_count, point_count, point_count, region_count, make_boundary);
}





void make_aabb_tetrahedrons_region_CPP_full(viennagrid::mesh const & mesh,
                                            viennagrid_numeric ll_x, viennagrid_numeric ll_y, viennagrid_numeric ll_z,
                                            viennagrid_numeric ur_x, viennagrid_numeric ur_y, viennagrid_numeric ur_z,
                                            int point_count_x, int point_count_y, int point_count_z,
                                            int region_count, bool make_boundary)
{
  typedef viennagrid::mesh                                    MeshType;
  typedef viennagrid::result_of::region<MeshType>::type       RegionType;

  typedef viennagrid::result_of::point<MeshType>::type        PointType;
  typedef viennagrid::result_of::element<MeshType>::type      ElementType;

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);
  viennagrid_numeric step_z = (ur_z-ll_z) / (point_count_z-1);

  std::vector<RegionType> regions;
  for (int i = 0; i != region_count; ++i)
    regions.push_back( mesh.create_region() );

  std::vector<ElementType> vertex_indices(point_count_x*point_count_y*point_count_z);
  int i = 0;

  PointType p(3);
  p[2] = ll_z;
  for (int zi = 0; zi != point_count_z; ++zi, p[2] += step_z)
  {
    p[1] = ll_y;
    for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
    {
      p[0] = ll_x;
      for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x)
      {
        vertex_indices[i++] = viennagrid::make_vertex(mesh, p);
      }
    }
  }

  for (int zi = 0; zi != point_count_z-1; ++zi)
    for (int yi = 0; yi != point_count_y-1; ++yi)
      for (int xi = 0; xi != point_count_x-1; ++xi)
      {
        ElementType indices[8];
        indices[0] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[1] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[2] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[3] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];
        indices[4] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[5] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[6] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[7] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];

        ElementType cells[6];
        make_tetrahedron(mesh, indices, cells);

        viennagrid::add(regions[(i++)%region_count], cells[0]);
        viennagrid::add(regions[(i++)%region_count], cells[1]);
        viennagrid::add(regions[(i++)%region_count], cells[2]);
        viennagrid::add(regions[(i++)%region_count], cells[3]);
        viennagrid::add(regions[(i++)%region_count], cells[4]);
        viennagrid::add(regions[(i++)%region_count], cells[5]);
      }

  if (make_boundary)
    viennagrid_mesh_property_set(mesh.internal(), VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void make_aabb_tetrahedrons_region_CPP(viennagrid::mesh const & mesh, int cell_count, int region_count, bool make_boundary)
{
  int point_count = std::pow(cell_count/6, 1.0/3.0)+1.5;
  make_aabb_tetrahedrons_region_CPP_full(mesh.internal(), -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, point_count, point_count, point_count, region_count, make_boundary);
}























void batch_make_aabb_triangles_region_C_full(viennagrid_mesh mesh,
                                             viennagrid_numeric ll_x, viennagrid_numeric ll_y,
                                             viennagrid_numeric ur_x, viennagrid_numeric ur_y,
                                             int point_count_x, int point_count_y,
                                             int region_count, bool make_boundary)
{
  viennagrid_mesh_geometric_dimension_set(mesh, 2);

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);

  for (int i = 0; i != region_count; ++i)
    viennagrid_mesh_region_get_or_create(mesh, i%region_count, NULL);


  viennagrid_numeric p[2];
  p[1] = ll_y;
  std::vector<viennagrid_numeric> coords( point_count_x*point_count_y*2 );
  for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
  {
    p[0] = ll_x;
    for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x)
    {
      coords[ 2*(yi * point_count_x + xi)+0 ] = p[0];
      coords[ 2*(yi * point_count_x + xi)+1 ] = p[1];
    }
  }


  viennagrid_element_id first_vertex_id;
  viennagrid_mesh_vertex_batch_create( mesh, point_count_x*point_count_y, &coords[0], &first_vertex_id );


  int cell_count = (point_count_y-1)*(point_count_x-1)*2;
  std::vector<viennagrid_element_type> element_types( cell_count, VIENNAGRID_ELEMENT_TYPE_TRIANGLE );

  std::vector<viennagrid_int> vertex_index_offsets( cell_count+1 );
  for (int i = 0; i != cell_count+1; ++i)
    vertex_index_offsets[i] = 3*i;

  std::vector<viennagrid_element_id> vertex_indices( cell_count*3 );

  int i = 0;
  for (int yi = 0; yi != point_count_y-1; ++yi)
    for (int xi = 0; xi != point_count_x-1; ++xi)
    {
      viennagrid_element_id indices[4];
      indices[0] = first_vertex_id + (yi+0)*point_count_y+(xi+0);
      indices[1] = first_vertex_id + (yi+0)*point_count_y+(xi+1);
      indices[2] = first_vertex_id + (yi+1)*point_count_y+(xi+0);
      indices[3] = first_vertex_id + (yi+1)*point_count_y+(xi+1);

      vertex_indices[3*i+0] = indices[0];
      vertex_indices[3*i+1] = indices[1];
      vertex_indices[3*i+2] = indices[2];
      ++i;

      vertex_indices[3*i+0] = indices[1];
      vertex_indices[3*i+1] = indices[2];
      vertex_indices[3*i+2] = indices[3];
      ++i;
    }

  std::vector<viennagrid_region_id> region_ids( cell_count );
  for (int i = 0; i != cell_count; ++i)
    region_ids[i] = i%region_count;

  viennagrid_mesh_element_batch_create( mesh, cell_count, &element_types[0], &vertex_index_offsets[0], &vertex_indices[0], &region_ids[0], NULL );

  if (make_boundary)
    viennagrid_mesh_property_set(mesh, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void batch_make_aabb_triangles_region_C(viennagrid::mesh const & mesh, int cell_count, int region_count, bool make_boundary)
{
  int point_count = std::sqrt(cell_count/2)+1.5;
  batch_make_aabb_triangles_region_C_full(mesh.internal(), -1.0, -1.0, 1.0, 1.0, point_count, point_count, region_count, make_boundary);
}



void batch_make_aabb_tetrahedrons_region_C_full(viennagrid_mesh mesh,
                                                viennagrid_numeric ll_x, viennagrid_numeric ll_y, viennagrid_numeric ll_z,
                                                viennagrid_numeric ur_x, viennagrid_numeric ur_y, viennagrid_numeric ur_z,
                                                int point_count_x, int point_count_y, int point_count_z,
                                                int region_count, bool make_boundary)
{
  viennagrid_mesh_geometric_dimension_set(mesh, 3);

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);
  viennagrid_numeric step_z = (ur_z-ll_z) / (point_count_z-1);

  for (int i = 0; i != region_count; ++i)
    viennagrid_mesh_region_get_or_create(mesh, i%region_count, NULL);



  viennagrid_numeric p[3];
  p[2] = ll_z;
  std::vector<viennagrid_numeric> coords( point_count_x*point_count_y*3 );
  for (int zi = 0; zi != point_count_z; ++zi, p[2] += step_z)
  {
    p[1] = ll_y;
    for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
    {
      p[0] = ll_x;
      for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x)
      {
        coords[ 3*(yi * point_count_x + xi)+0 ] = p[0];
        coords[ 3*(yi * point_count_x + xi)+1 ] = p[1];
        coords[ 3*(yi * point_count_x + xi)+2 ] = p[2];
      }
    }
  }


  viennagrid_element_id first_vertex_id;
  viennagrid_mesh_vertex_batch_create( mesh, point_count_x*point_count_y, &coords[0], &first_vertex_id );




  int cell_count = (point_count_z-1)*(point_count_y-1)*(point_count_x-1)*6;
  std::vector<viennagrid_element_type> element_types( cell_count, VIENNAGRID_ELEMENT_TYPE_TRIANGLE );

  std::vector<viennagrid_int> vertex_index_offsets( cell_count+1 );
  for (int i = 0; i != cell_count+1; ++i)
    vertex_index_offsets[i] = 3*i;

  std::vector<viennagrid_element_id> vertex_indices( cell_count*4 );

  int i = 0;
  for (int zi = 0; zi != point_count_z-1; ++zi)
    for (int yi = 0; yi != point_count_y-1; ++yi)
      for (int xi = 0; xi != point_count_x-1; ++xi)
      {
        viennagrid_element_id indices[8];
        indices[0] = first_vertex_id + (zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0);
        indices[1] = first_vertex_id + (zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1);
        indices[2] = first_vertex_id + (zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0);
        indices[3] = first_vertex_id + (zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1);
        indices[4] = first_vertex_id + (zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0);
        indices[5] = first_vertex_id + (zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1);
        indices[6] = first_vertex_id + (zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0);
        indices[7] = first_vertex_id + (zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1);


        vertex_indices[4*i+0] = indices[0];
        vertex_indices[4*i+1] = indices[2];
        vertex_indices[4*i+2] = indices[6];
        vertex_indices[4*i+3] = indices[7];
        ++i;

        vertex_indices[4*i+0] = indices[0];
        vertex_indices[4*i+1] = indices[2];
        vertex_indices[4*i+2] = indices[3];
        vertex_indices[4*i+3] = indices[7];
        ++i;

        vertex_indices[4*i+0] = indices[0];
        vertex_indices[4*i+1] = indices[1];
        vertex_indices[4*i+2] = indices[3];
        vertex_indices[4*i+3] = indices[7];
        ++i;

        vertex_indices[4*i+0] = indices[0];
        vertex_indices[4*i+1] = indices[1];
        vertex_indices[4*i+2] = indices[5];
        vertex_indices[4*i+3] = indices[7];
        ++i;

        vertex_indices[4*i+0] = indices[0];
        vertex_indices[4*i+1] = indices[4];
        vertex_indices[4*i+2] = indices[5];
        vertex_indices[4*i+3] = indices[7];
        ++i;

        vertex_indices[4*i+0] = indices[0];
        vertex_indices[4*i+1] = indices[4];
        vertex_indices[4*i+2] = indices[6];
        vertex_indices[4*i+3] = indices[7];
        ++i;
      }

  std::vector<viennagrid_region_id> region_ids( cell_count );
  for (int i = 0; i != cell_count; ++i)
    region_ids[i] = i%region_count;

  viennagrid_mesh_element_batch_create( mesh, cell_count, &element_types[0], &vertex_index_offsets[0], &vertex_indices[0], &region_ids[0], NULL );

  if (make_boundary)
    viennagrid_mesh_property_set(mesh, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_FULL);
}

void batch_make_aabb_tetrahedrons_region_C(viennagrid::mesh const & mesh, int cell_count, int region_count, bool make_boundary)
{
  int point_count = std::pow(cell_count/6, 1.0/3.0)+1.5;
  batch_make_aabb_tetrahedrons_region_C_full(mesh.internal(), -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, point_count, point_count, point_count, region_count, make_boundary);
}







#endif



