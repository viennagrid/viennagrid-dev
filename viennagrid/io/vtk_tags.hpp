/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */


#ifndef VIENNAMESH_IO_VTK_TAGS_GUARD
#define VIENNAMESH_IO_VTK_TAGS_GUARD

#include <fstream>
#include <iostream>
#include "viennamesh/io/helper.hpp"
#include "viennamesh/forwards.h"

namespace viennamesh {
  template <typename ElementTag>
  struct ELEMENT_TAG_TO_VTK_TYPE
  {
    enum{ ReturnValue = ElementTag::ERROR_ELEMENT_TYPE_NOT_SUPPORTED };
  };

  template <>
  struct ELEMENT_TAG_TO_VTK_TYPE<TetrahedronTag>
  {
    enum{ ReturnValue = 10 };
  };

  template <>
  struct ELEMENT_TAG_TO_VTK_TYPE<TriangleTag>
  {
    enum{ ReturnValue = 5 };
  };

  template <>
  struct ELEMENT_TAG_TO_VTK_TYPE<LineTag>
  {
    enum{ ReturnValue = 3 };
  };
}

#endif