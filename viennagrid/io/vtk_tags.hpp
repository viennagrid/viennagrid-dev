/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#ifndef VIENNAGRID_IO_VTK_TAGS_GUARD
#define VIENNAGRID_IO_VTK_TAGS_GUARD

#include <fstream>
#include <iostream>
#include "viennagrid/io/helper.hpp"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/forwards.h"

namespace viennagrid
{
  namespace io
  {
    // translates element tags to VTK type identifiers
    // see http://www.vtk.org/VTK/img/file-formats.pdf, Figure 2, for an overview
    template <typename ElementTag>
    struct ELEMENT_TAG_TO_VTK_TYPE
    {
      enum{ ReturnValue = ElementTag::ERROR_ELEMENT_TYPE_NOT_SUPPORTED };
    };

    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<hexahedron_tag>
    {
      enum{ ReturnValue = 12 };
    };

    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<tetrahedron_tag>
    {
      enum{ ReturnValue = 10 };
    };
    
    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<quadrilateral_tag>
    {
      enum{ ReturnValue = 9 };
    };

    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<triangle_tag>
    {
      enum{ ReturnValue = 5 };
    };

    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<line_tag>
    {
      enum{ ReturnValue = 3 };
    };
  } //namespace io
} //namespace viennagrid

#endif