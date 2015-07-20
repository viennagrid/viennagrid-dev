#include "stdio.h"
#include "viennagrid/viennagrid.h"

int main()
{
  viennagrid_plc plc;
  viennagrid_plc_create(&plc);
  viennagrid_plc_geometric_dimension_set(plc, 2);

  viennagrid_int i = 0;
  viennagrid_int vertices[11];
  viennagrid_numeric point[2];

  point[0] = 0;
  point[1] = 0;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 10;
  point[1] = 0;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 20;
  point[1] = 10;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 20;
  point[1] = 20;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 10;
  point[1] = 20;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 0;
  point[1] = 10;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 5;
  point[1] = 5;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 10;
  point[1] = 10;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 12;
  point[1] = 10;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 10;
  point[1] = 12;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));

  point[0] = 8;
  point[1] = 10;
  viennagrid_plc_vertex_create(plc, point, vertices+(i++));



  i = 0;
  viennagrid_int lines[7+3+1];
  {
    viennagrid_int * start = vertices;
    viennagrid_int * end = vertices+7;

    viennagrid_int * it1 = start;
    viennagrid_int * it2 = it1; ++it2;
    for (; it2 != end; ++it1, ++it2)
      viennagrid_plc_line_create(plc, *it1, *it2, lines+(i++));
    viennagrid_plc_line_create(plc, *it1, *start, lines+(i++));
  }

  {
    viennagrid_int * start = vertices+7;
    viennagrid_int * end = vertices+10;

    viennagrid_int * it1 = start;
    viennagrid_int * it2 = it1; ++it2;
    for (; it2 != end; ++it1, ++it2)
      viennagrid_plc_line_create(plc, *it1, *it2, lines+(i++));
    viennagrid_plc_line_create(plc, *it1, *start, lines+(i++));
  }

  viennagrid_plc_line_create(plc, *(vertices+9), *(vertices+10), lines+(i++));


  viennagrid_int facet;
  viennagrid_plc_facet_create(plc, 11, lines, &facet);

  point[0] = 10.5;
  point[1] = 10.5;
  viennagrid_plc_facet_hole_point_add(plc, facet, point);

  point[0] = 0.5;
  point[1] = 0.5;
  viennagrid_plc_facet_hole_point_add(plc, facet, point);



  printf("All vertices of the facet\n");
  viennagrid_int * vertices_begin;
  viennagrid_int * vertices_end;
  viennagrid_plc_boundary_elements(plc, 2, facet, 0, &vertices_begin, &vertices_end);

  for (viennagrid_int * it = vertices_begin; it != vertices_end; ++it)
  {
    viennagrid_numeric * coords;
    viennagrid_plc_vertex_coords_get(plc, *it, &coords);
    printf("  %d (%f, %f)\n", *it, coords[0], coords[1]);
  }


  printf("All lines of the facet\n");
  viennagrid_int * lines_begin;
  viennagrid_int * lines_end;
  viennagrid_plc_boundary_elements(plc, 2, facet, 1, &lines_begin, &lines_end);

  for (viennagrid_int * it = lines_begin; it != lines_end; ++it)
  {
    viennagrid_int * vertices_on_line_begin;
    viennagrid_int * vertices_on_line_end;
    viennagrid_plc_boundary_elements(plc, 1, *it, 0, &vertices_on_line_begin, &vertices_on_line_end);

    printf("  [line %d: %d, %d]\n", *it, *vertices_on_line_begin, *(vertices_on_line_begin+1));
  }


  printf("All hole points of the facet\n");
  viennagrid_numeric * hole_points;
  viennagrid_int hole_point_count;
  viennagrid_plc_facet_hole_points_get(plc, facet, &hole_points, &hole_point_count);

  for (viennagrid_int i = 0; i != hole_point_count; ++i)
  {
    printf("  (%f, %f)\n", hole_points[2*i+0], hole_points[2*i+1]);
  }


  viennagrid_plc_release(plc);
  return 0;
}
