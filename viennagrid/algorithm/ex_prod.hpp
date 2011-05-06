/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Josef Weinbub                   weinbub@iue.tuwien.ac.at
               Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_EXPROD_GUARD
#define VIENNAGRID_EXPROD_GUARD

#include "viennagrid/forwards.h"

namespace viennagrid {

template<typename VectorT>
VectorT ex(VectorT const& v1, VectorT const& v2)
{
   return VectorT(v1[1]*v2[2]-v1[2]*v2[1], 
                  v1[2]*v2[0]-v1[0]*v2[2],
                  v1[0]*v2[1]-v1[1]*v2[0]);
}

} 

#endif
