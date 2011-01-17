/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_FORWARDS_GUARD
#define VIENNAGRID_FORWARDS_GUARD

#include <iostream>
#include "viennagrid/idhandler.h"

namespace viennagrid
{
  
  /********* Tags ***********************/

  //Dimension Tags:
  struct three_dimensions_tag{
      enum{ value = 3 };
  };
  
  struct two_dimensions_tag{
      enum{ value = 2 };
  };

  struct one_dimension_tag{
      enum{ value = 1 };
  };

  //Tags for the handling of elements at different topological levels (see subcell_traits)
  struct full_handling_tag {};
  struct no_handling_tag {};

  //multigrid:
  struct full_multigrid_tag {};
  struct no_multigrid_tag {};

  //segment connection:
  template <long id>
  struct SegmentConnectionKey
  {
    bool operator<(SegmentConnectionKey const & sck2) const
    {
      return false;     // a < b is false AND b < a is false, hence a == b
    }
  };

  
  /********* Forward definitions of main classes *******************/

  //forward declaration of DomainTypes:
  template <typename Configuration>
  struct DomainTypes;

  template <typename Domain>
  struct DomainTraits;

  template <typename T_Configuration, typename ElementTag>
  struct element;

  template <typename ElementType>
  class ElementKey;

  template <long level>
  struct segment_traits;

  //Segment type: 
  template <typename T_Configuration,
             unsigned long levelnum = T_Configuration::cell_tag::topology_level,
             typename HandlingTag = typename segment_traits<levelnum>::handling_tag,
             bool specialHandling = (levelnum == T_Configuration::cell_tag::topology_level) || (levelnum == 0),
             typename MultigridTag = typename T_Configuration::multigrid_tag>
  class segment;


  /********* Other *******************/

  
  struct less_tag {};
  struct equal_tag {};

  template <long i, long j, bool ilargerj = (i>j), bool iequalj = (i==j) >
  struct level_discriminator
  {  };

  template <long i, long j>
  struct level_discriminator <i, j, false, true>
  {
    typedef equal_tag   result_type;
  };

  template <long i, long j>
  struct level_discriminator <i, j, true, false>
  {
    typedef less_tag    result_type;
  };



} 
#endif
