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


#include <fstream>
#include <iostream>

#include "viennagrid/io/helper.hpp"
#include "viennagrid/core/forwards.hpp"

/** @file viennagrid/io/vtk_common.hpp
    @brief Provides common things for VTK reader and writer
*/

namespace viennagrid
{
  namespace io
  {
    inline int vtk_element_tag( element_tag et)
    {
      switch (et.internal())
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
        case VIENNAGRID_ELEMENT_TYPE_WEDGE:
          return 13;
        case VIENNAGRID_ELEMENT_TYPE_PYRAMID:
          return 14;

        default:
          return -1;
      }
    }

    inline element_tag from_vtk_element_tag(int tag)
    {
      switch (tag)
      {
        case 1:
          return viennagrid::vertex_tag();
        case 3:
          return viennagrid::line_tag();
        case 5:
          return viennagrid::triangle_tag();
        case 9:
          return viennagrid::quadrilateral_tag();
        case 7:
          return viennagrid::polygon_tag();
        case 10:
          return viennagrid::tetrahedron_tag();
        case 12:
          return viennagrid::hexahedron_tag();
        case 13:
          return viennagrid::wedge_tag();
        case 14:
          return viennagrid::pyramid_tag();

        default:
          return element_tag();
      }
    }


    namespace detail
    {
      /** @brief Tranformations of reference orientations from VTK to ViennaGrid
       *
       * While nothing needs to be done for simplices, hypercubes are oriented differently.
       */
      struct vtk_to_viennagrid_orientations
      {
        vtk_to_viennagrid_orientations(element_tag ct) : cell_tag_(ct) {}

        std::size_t operator()(std::size_t j) const
        {
          switch (cell_tag_.internal())
          {
            case VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL:
            {
              switch (j)
              {
                case 2: return 3;
                case 3: return 2;
                default: return j;
              }
            }
            case VIENNAGRID_ELEMENT_TYPE_HEXAHEDRON:
            {
              switch (j)
              {
                case 2: return 3;
                case 3: return 2;
                case 6: return 7;
                case 7: return 6;
                default: return j;
              }
            }
            case VIENNAGRID_ELEMENT_TYPE_PYRAMID:
            {
              switch (j)
              {
                case 2: return 3;
                case 3: return 2;
                default: return j;
              }
            }
            default:
              return j;
          }
        }

        element_tag cell_tag_;
      };

      /** @brief Tranformations of reference orientations from ViennaGrid to VTK
       *
       * While nothing needs to be done for simplices, hypercubes are oriented differently.
       */
      typedef vtk_to_viennagrid_orientations viennagrid_to_vtk_orientations;


    } //namespace detail
  } //namespace io
} //namespace viennagrid

#endif

