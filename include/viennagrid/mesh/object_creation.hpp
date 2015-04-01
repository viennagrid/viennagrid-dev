#ifndef VIENNAGRID_OBJECT_CREATION_HPP
#define VIENNAGRID_OBJECT_CREATION_HPP

#include "viennagrid/mesh/mesh.hpp"

namespace viennagrid
{

  void make_aa_cube_hull(viennagrid::mesh_t const & mesh,
                         viennagrid::point_t const & ll, viennagrid::point_t const & ur,
                         viennagrid::element_tag_t element_tag = viennagrid::triangle_tag())
  {
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::point_t PointType;

    assert( element_tag.is_triangle() || element_tag.is_quadrilateral() );

    PointType points[8];
    points[0] = viennagrid::make_point( ll[0], ll[1], ll[2] );
    points[1] = viennagrid::make_point( ll[0], ur[1], ll[2] );
    points[2] = viennagrid::make_point( ur[0], ll[1], ll[2] );
    points[3] = viennagrid::make_point( ur[0], ur[1], ll[2] );

    points[4] = viennagrid::make_point( ll[0], ll[1], ur[2] );
    points[5] = viennagrid::make_point( ll[0], ur[1], ur[2] );
    points[6] = viennagrid::make_point( ur[0], ll[1], ur[2] );
    points[7] = viennagrid::make_point( ur[0], ur[1], ur[2] );

    typedef viennagrid::result_of::element<MeshType>::type VertexType;

    VertexType vertices[8];

    for (int i = 0; i != 8; ++i)
      vertices[i] = viennagrid::make_vertex(mesh, points[i]);


    if ( element_tag.is_triangle() )
    {
      viennagrid::make_triangle(mesh, vertices[0], vertices[1], vertices[2]);
      viennagrid::make_triangle(mesh, vertices[1], vertices[2], vertices[3]);

      viennagrid::make_triangle(mesh, vertices[4], vertices[5], vertices[6]);
      viennagrid::make_triangle(mesh, vertices[5], vertices[6], vertices[7]);

      viennagrid::make_triangle(mesh, vertices[0], vertices[4], vertices[2]);
      viennagrid::make_triangle(mesh, vertices[2], vertices[6], vertices[4]);

      viennagrid::make_triangle(mesh, vertices[1], vertices[3], vertices[5]);
      viennagrid::make_triangle(mesh, vertices[5], vertices[7], vertices[3]);

      viennagrid::make_triangle(mesh, vertices[0], vertices[1], vertices[4]);
      viennagrid::make_triangle(mesh, vertices[4], vertices[5], vertices[1]);

      viennagrid::make_triangle(mesh, vertices[2], vertices[6], vertices[3]);
      viennagrid::make_triangle(mesh, vertices[3], vertices[7], vertices[6]);
    }
    else if ( element_tag.is_triangle() )
    {
      viennagrid::make_quadrilateral(mesh, vertices[0], vertices[2], vertices[3], vertices[1]);
      viennagrid::make_quadrilateral(mesh, vertices[4], vertices[6], vertices[7], vertices[5]);
      viennagrid::make_quadrilateral(mesh, vertices[0], vertices[2], vertices[6], vertices[4]);
      viennagrid::make_quadrilateral(mesh, vertices[1], vertices[5], vertices[7], vertices[3]);
      viennagrid::make_quadrilateral(mesh, vertices[0], vertices[4], vertices[5], vertices[1]);
      viennagrid::make_quadrilateral(mesh, vertices[2], vertices[3], vertices[7], vertices[6]);
    }
  }


  void make_aa_cube_hull(viennagrid::mesh_t const & mesh,
                         viennagrid::point_t const & position,
                         viennagrid_numeric size_x, viennagrid_numeric size_y, viennagrid_numeric size_z,
                         viennagrid::element_tag_t element_tag = viennagrid::triangle_tag())
  {
    make_aa_cube_hull(mesh,
                      position - viennagrid::make_point(size_x, size_y, size_z)/2.0,
                      position + viennagrid::make_point(size_x, size_y, size_z)/2.0,
                      element_tag);
  }









  // http://en.wikipedia.org/wiki/Regular_icosahedron#Cartesian_coordinates
  void make_icosahedron_hull(viennagrid::mesh_t const & mesh,
                             viennagrid::point_t const & position,
                             viennagrid_numeric radius)
  {
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    double phi = 1.0/2.0 * (1.0 + std::sqrt(5.0));
    PointType pts[12];

    pts[0] = viennagrid::make_point(0, phi, 1);
    pts[1] = viennagrid::make_point(0, phi, -1);
    pts[2] = viennagrid::make_point(0, -phi, 1);
    pts[3] = viennagrid::make_point(0, -phi, -1);

    pts[4] = viennagrid::make_point(1, 0, phi);
    pts[5] = viennagrid::make_point(-1, 0, phi);
    pts[6] = viennagrid::make_point(1, 0, -phi);
    pts[7] = viennagrid::make_point(-1, 0, -phi);

    pts[8] = viennagrid::make_point(phi, 1, 0);
    pts[9] = viennagrid::make_point(phi, -1, 0);
    pts[10] = viennagrid::make_point(-phi, 1, 0);
    pts[11] = viennagrid::make_point(-phi, -1, 0);

    for (int i = 0; i != 12; ++i)
      pts[i] /= viennagrid::norm_2(pts[i]);

    ElementType vertices[12];
    for (int i = 0; i != 12; ++i)
      vertices[i] = viennagrid::make_vertex( mesh, pts[i] );


    viennagrid::make_triangle( mesh, vertices[1], vertices[6], vertices[7] );
    viennagrid::make_triangle( mesh, vertices[1], vertices[7], vertices[10] );
    viennagrid::make_triangle( mesh, vertices[0], vertices[1], vertices[10] );
    viennagrid::make_triangle( mesh, vertices[0], vertices[1], vertices[8] );
    viennagrid::make_triangle( mesh, vertices[0], vertices[1], vertices[8] );
    viennagrid::make_triangle( mesh, vertices[1], vertices[6], vertices[8] );
    viennagrid::make_triangle( mesh, vertices[0], vertices[5], vertices[10] );
    viennagrid::make_triangle( mesh, vertices[0], vertices[4], vertices[5] );
    viennagrid::make_triangle( mesh, vertices[0], vertices[4], vertices[8] );
    viennagrid::make_triangle( mesh, vertices[4], vertices[8], vertices[9] );
    viennagrid::make_triangle( mesh, vertices[6], vertices[8], vertices[9] );

    viennagrid::make_triangle( mesh, vertices[5], vertices[10], vertices[11] );
    viennagrid::make_triangle( mesh, vertices[7], vertices[10], vertices[11] );
    viennagrid::make_triangle( mesh, vertices[2], vertices[3], vertices[9] );
    viennagrid::make_triangle( mesh, vertices[2], vertices[3], vertices[11] );
    viennagrid::make_triangle( mesh, vertices[2], vertices[4], vertices[5] );
    viennagrid::make_triangle( mesh, vertices[3], vertices[6], vertices[7] );
    viennagrid::make_triangle( mesh, vertices[2], vertices[5], vertices[11] );
    viennagrid::make_triangle( mesh, vertices[3], vertices[7], vertices[11] );
    viennagrid::make_triangle( mesh, vertices[2], vertices[4], vertices[9] );
    viennagrid::make_triangle( mesh, vertices[3], vertices[6], vertices[9] );
  }








}

#endif
