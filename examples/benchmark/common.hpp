#ifndef VIENNAGRID_BENCHMARK_COMMON_HPP
#define VIENNAGRID_BENCHMARK_COMMON_HPP

#include <vector>
#include "viennautils/timer.hpp"

void make_aabb_triangles_C(viennagrid_mesh mesh,
                           viennagrid_numeric ll_x, viennagrid_numeric ll_y,
                           viennagrid_numeric ur_x, viennagrid_numeric ur_y,
                           int point_count_x, int point_count_y)
{
  viennagrid_mesh_hierarchy mesh_hierarchy;
  viennagrid_mesh_get_mesh_hierarchy(mesh, &mesh_hierarchy);
  viennagrid_mesh_hierarchy_set_geometric_dimension(mesh_hierarchy, 2);

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);

  std::vector<viennagrid_index> vertex_indices(point_count_x*point_count_y);
  int i = 0;

  double p[2];
  p[1] = ll_y;
  for (int yi = 0; yi != point_count_y; ++yi, p[1] += step_y)
  {
    p[0] = ll_x;
    for (int xi = 0; xi != point_count_x; ++xi, p[0] += step_x, ++i)
    {
      viennagrid_vertex_create(mesh_hierarchy, p, &vertex_indices[i]);
    }
  }

  for (int yi = 0; yi != point_count_y-1; ++yi)
    for (int xi = 0; xi != point_count_x-1; ++xi)
    {
      viennagrid_index indices[4];
      indices[0] = vertex_indices[(yi+0)*point_count_y+(xi+0)];
      indices[1] = vertex_indices[(yi+0)*point_count_y+(xi+1)];
      indices[2] = vertex_indices[(yi+1)*point_count_y+(xi+0)];
      indices[3] = vertex_indices[(yi+1)*point_count_y+(xi+1)];

      viennagrid_index cell;
      viennagrid_element_create(mesh_hierarchy, VIENNAGRID_ELEMENT_TAG_TRIANGLE, 3, indices+0, 0, &cell);
      viennagrid_element_create(mesh_hierarchy, VIENNAGRID_ELEMENT_TAG_TRIANGLE, 3, indices+1, 0, &cell);
    }
}

void make_aabb_triangles_C(viennagrid::mesh_t const & mesh, int cell_count)
{
  int point_count = std::sqrt(cell_count/2)+0.5;
  make_aabb_triangles_C(mesh.internal(), -1.0, -1.0, 1.0, 1.0, point_count, point_count);
}



void make_aabb_triangles_CPP(viennagrid::mesh_t const & mesh,
                             viennagrid_numeric ll_x, viennagrid_numeric ll_y,
                             viennagrid_numeric ur_x, viennagrid_numeric ur_y,
                             int point_count_x, int point_count_y)
{
  typedef viennagrid::mesh_t MeshType;
  typedef viennagrid::result_of::point<MeshType>::type PointType;
  typedef viennagrid::result_of::element<MeshType>::type ElementType;

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

      viennagrid::make_element(mesh, viennagrid::triangle_tag(), indices, indices+3);
      viennagrid::make_element(mesh, viennagrid::triangle_tag(), indices+1, indices+4);
    }
}

void make_aabb_triangles_CPP(viennagrid::mesh_t const & mesh, int cell_count)
{
  int point_count = std::sqrt(cell_count/2)+0.5;
  make_aabb_triangles_CPP(mesh, -1.0, -1.0, 1.0, 1.0, point_count, point_count);
}




void make_aabb_tetrahedrons_C(viennagrid_mesh mesh,
                           viennagrid_numeric ll_x, viennagrid_numeric ll_y, viennagrid_numeric ll_z,
                           viennagrid_numeric ur_x, viennagrid_numeric ur_y, viennagrid_numeric ur_z,
                           int point_count_x, int point_count_y, int point_count_z)
{
  viennagrid_mesh_hierarchy mesh_hierarchy;
  viennagrid_mesh_get_mesh_hierarchy(mesh, &mesh_hierarchy);
  viennagrid_mesh_hierarchy_set_geometric_dimension(mesh_hierarchy, 3);

  viennagrid_numeric step_x = (ur_x-ll_x) / (point_count_x-1);
  viennagrid_numeric step_y = (ur_y-ll_y) / (point_count_y-1);
  viennagrid_numeric step_z = (ur_z-ll_z) / (point_count_z-1);

  std::vector<viennagrid_index> vertex_indices(point_count_x*point_count_y*point_count_z);
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
        viennagrid_vertex_create(mesh_hierarchy, p, &vertex_indices[i]);
      }
    }
  }

  for (int zi = 0; zi != point_count_z-1; ++zi)
    for (int yi = 0; yi != point_count_y-1; ++yi)
      for (int xi = 0; xi != point_count_x-1; ++xi)
      {
        viennagrid_index indices[8];
        indices[0] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[1] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[2] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[3] = vertex_indices[(zi+0)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];
        indices[4] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+0)];
        indices[5] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+0)*point_count_y+(xi+1)];
        indices[6] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+0)];
        indices[7] = vertex_indices[(zi+1)*point_count_z*point_count_y+(yi+1)*point_count_y+(xi+1)];

        viennagrid_index local_indices[4];
        viennagrid_index cell;

        local_indices[0] = indices[0];
        local_indices[1] = indices[2];
        local_indices[2] = indices[6];
        local_indices[3] = indices[7];
        viennagrid_element_create(mesh_hierarchy, VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, 4, local_indices, 0, &cell);

        local_indices[0] = indices[0];
        local_indices[1] = indices[2];
        local_indices[2] = indices[3];
        local_indices[3] = indices[7];
        viennagrid_element_create(mesh_hierarchy, VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, 4, local_indices, 0, &cell);

        local_indices[0] = indices[0];
        local_indices[1] = indices[1];
        local_indices[2] = indices[3];
        local_indices[3] = indices[7];
        viennagrid_element_create(mesh_hierarchy, VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, 4, local_indices, 0, &cell);

        local_indices[0] = indices[0];
        local_indices[1] = indices[1];
        local_indices[2] = indices[5];
        local_indices[3] = indices[7];
        viennagrid_element_create(mesh_hierarchy, VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, 4, local_indices, 0, &cell);

        local_indices[0] = indices[0];
        local_indices[1] = indices[4];
        local_indices[2] = indices[5];
        local_indices[3] = indices[7];
        viennagrid_element_create(mesh_hierarchy, VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, 4, local_indices, 0, &cell);

        local_indices[0] = indices[0];
        local_indices[1] = indices[4];
        local_indices[2] = indices[6];
        local_indices[3] = indices[7];
        viennagrid_element_create(mesh_hierarchy, VIENNAGRID_ELEMENT_TAG_TETRAHEDRON, 4, local_indices, 0, &cell);
      }
}

void make_aabb_tetrahedrons_C(viennagrid::mesh_t const & mesh, int cell_count)
{
  int point_count = std::pow(cell_count/6, 1.0/3.0)+0.5;
  make_aabb_tetrahedrons_C(mesh.internal(), -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, point_count, point_count, point_count);
}





void make_aabb_tetrahedrons_CPP(viennagrid::mesh_t const & mesh,
                           viennagrid_numeric ll_x, viennagrid_numeric ll_y, viennagrid_numeric ll_z,
                           viennagrid_numeric ur_x, viennagrid_numeric ur_y, viennagrid_numeric ur_z,
                           int point_count_x, int point_count_y, int point_count_z)
{
  typedef viennagrid::mesh_t MeshType;
  typedef viennagrid::result_of::point<MeshType>::type PointType;
  typedef viennagrid::result_of::element<MeshType>::type ElementType;

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

        ElementType local_indices[4];

        local_indices[0] = indices[0];
        local_indices[1] = indices[2];
        local_indices[2] = indices[6];
        local_indices[3] = indices[7];
        viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

        local_indices[0] = indices[0];
        local_indices[1] = indices[2];
        local_indices[2] = indices[3];
        local_indices[3] = indices[7];
        viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

        local_indices[0] = indices[0];
        local_indices[1] = indices[1];
        local_indices[2] = indices[3];
        local_indices[3] = indices[7];
        viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

        local_indices[0] = indices[0];
        local_indices[1] = indices[1];
        local_indices[2] = indices[5];
        local_indices[3] = indices[7];
        viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

        local_indices[0] = indices[0];
        local_indices[1] = indices[4];
        local_indices[2] = indices[5];
        local_indices[3] = indices[7];
        viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);

        local_indices[0] = indices[0];
        local_indices[1] = indices[4];
        local_indices[2] = indices[6];
        local_indices[3] = indices[7];
        viennagrid::make_element(mesh, viennagrid::tetrahedron_tag(), local_indices, local_indices+4);
      }
}

void make_aabb_tetrahedrons_CPP(viennagrid::mesh_t const & mesh, int cell_count)
{
  int point_count = std::pow(cell_count/6, 1.0/3.0)+0.5;
  make_aabb_tetrahedrons_CPP(mesh.internal(), -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, point_count, point_count, point_count);
}








#endif
