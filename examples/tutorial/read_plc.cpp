/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include "stdio.h"
#include "viennagrid/viennagrid.h"


int main()
{
  viennagrid_plc plc;
  viennagrid_plc_make(&plc);

  viennagrid_plc_read_tetgen_poly(plc, "../data/two_cubes.poly");


  viennagrid_int facet_count;
  viennagrid_plc_get_element_count(plc, 2, &facet_count);


  for (viennagrid_int facet = 0; facet != facet_count; ++facet)
  {
    printf("Facet %d\n", facet);

    printf("  All vertices of the facet\n");
    viennagrid_int * vertices_begin;
    viennagrid_int * vertices_end;
    viennagrid_plc_boundary_elements(plc, 2, facet, 0, &vertices_begin, &vertices_end);

    for (viennagrid_int * it = vertices_begin; it != vertices_end; ++it)
    {
      viennagrid_numeric * coords;
      viennagrid_plc_vertex_get(plc, *it, &coords);
      printf("    %d (%f, %f, %f)\n", *it, coords[0], coords[1], coords[2]);
    }


    printf("  All lines of the facet\n");
    viennagrid_int * lines_begin;
    viennagrid_int * lines_end;
    viennagrid_plc_boundary_elements(plc, 2, facet, 1, &lines_begin, &lines_end);

    for (viennagrid_int * it = lines_begin; it != lines_end; ++it)
    {
      viennagrid_int * vertices_on_line_begin;
      viennagrid_int * vertices_on_line_end;
      viennagrid_plc_boundary_elements(plc, 1, *it, 0, &vertices_on_line_begin, &vertices_on_line_end);

      printf("    [line %d: %d, %d]\n", *it, *vertices_on_line_begin, *(vertices_on_line_begin+1));
    }


    printf("  All hole points of the facet\n");
    viennagrid_numeric * hole_points;
    viennagrid_int hole_point_count;
    viennagrid_plc_facet_get_hole_points(plc, facet, &hole_points, &hole_point_count);

    for (viennagrid_int i = 0; i != hole_point_count; ++i)
    {
      printf("    (%f, %f, %f)\n", hole_points[3*i+0], hole_points[3*i+1], hole_points[3*i+2]);
    }
  }

  viennagrid_int hole_point_count;
  viennagrid_plc_hole_point_get_count(plc, &hole_point_count);
  printf("All hole points of the PLC\n");
  for (viennagrid_int i = 0; i != hole_point_count; ++i)
  {
    viennagrid_numeric * coords;
    viennagrid_plc_hole_point_get(plc, i, &coords);
    printf("  (%f, %f, %f)\n", coords[0], coords[1], coords[2]);
  }

  viennagrid_int seed_point_count;
  viennagrid_plc_seed_point_get_count(plc, &seed_point_count);
  printf("All seed points of the PLC\n");
  for (viennagrid_int i = 0; i != seed_point_count; ++i)
  {
    viennagrid_numeric * coords;
    viennagrid_int region_id;
    viennagrid_plc_seed_point_get(plc, i, &coords, &region_id);
    printf("  (%f, %f, %f) - %d\n", coords[0], coords[1], coords[2], region_id);
  }


  viennagrid_plc_delete(plc);
  return 0;
}
