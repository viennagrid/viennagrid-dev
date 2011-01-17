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

  //Topological Handling Tag
  struct topology_level_full_handling {};
  struct topology_level_no_handling {};

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
  struct SegmentConfig;

  //Segment type: 
  template <typename T_Configuration,
             unsigned long levelnum = T_Configuration::cell_tag::topology_level,
             typename HandlingTag = typename SegmentConfig<levelnum>::HandlingTag,
             bool specialHandling = (levelnum == T_Configuration::cell_tag::topology_level) || (levelnum == 0),
             typename MultigridTag = typename T_Configuration::multigrid_tag>
  class segment;


  /********* Other *******************/

  
  struct LessType {};
  struct EqualType {};

  template <long i, long j, bool ilargerj = (i>j), bool iequalj = (i==j) >
  struct LevelDiscriminator
  {  };

  template <long i, long j>
  struct LevelDiscriminator <i, j, false, true>
  {
    typedef EqualType   ResultType;
  };

  template <long i, long j>
  struct LevelDiscriminator <i, j, true, false>
  {
    typedef LessType    ResultType;
  };



} 
#endif
