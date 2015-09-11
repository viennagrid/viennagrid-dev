#ifndef VIENNAGRID_IO_VTK_COMMON_HPP
#define VIENNAGRID_IO_VTK_COMMON_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/viennagrid.h"

/** @file viennagrid/io/vtk_common.hpp
    @brief Provides common things for VTK reader and writer
*/



inline int vtk_element_type(viennagrid_element_type type)
{
  switch (type)
  {
    case VIENNAGRID_ELEMENT_TYPE_VERTEX:
      return 1;
    case VIENNAGRID_ELEMENT_TYPE_LINE:
      return 3;
    case VIENNAGRID_ELEMENT_TYPE_TRIANGLE:
      return 5;
    case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
      return 9;
    case VIENNAGRID_ELEMENT_TYPE_POLYGON:
      return 7;
    case VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON:
      return 10;
    case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
      return 12;

    default:
      return VIENNAGRID_ELEMENT_TYPE_NO_ELEMENT;
  }
}

inline viennagrid_element_type from_vtk_element_type(int type)
{
  switch (type)
  {
    case 1:
      return VIENNAGRID_ELEMENT_TYPE_VERTEX;
    case 3:
      return VIENNAGRID_ELEMENT_TYPE_LINE;
    case 5:
      return VIENNAGRID_ELEMENT_TYPE_TRIANGLE;
    case 9:
      return VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL;
    case 7:
      return VIENNAGRID_ELEMENT_TYPE_POLYGON;
    case 10:
      return VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON;
    case 12:
      return VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON;

    default:
      return VIENNAGRID_ELEMENT_TYPE_NO_ELEMENT;
  }
}

#endif

