/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */

#ifndef VIENNAMESH_FORWARDS_GUARD
#define VIENNAMESH_FORWARDS_GUARD

#include <iostream>
#include "viennamesh/idhandler.h"

namespace viennamesh
{
  
  /********* Tags ***********************/

  //Dimension Tags:
  struct ThreeDimensionsTag{
      enum{ dim = 3 };
  };
  
  struct TwoDimensionsTag{
      enum{ dim = 2 };
  };

  struct OneDimensionTag{
      enum{ dim = 1 };
  };

  //Topological Handling Tag
  struct TopoLevelFullHandling {};
  struct TopoLevelNoHandling {};

  //multigrid:
  struct FullMultigridTag {};
  struct NoMultigridTag {};

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
             unsigned long levelnum = T_Configuration::CellTag::TopoLevel,
             typename HandlingTag = typename SegmentConfig<levelnum>::HandlingTag,
             bool specialHandling = (levelnum == T_Configuration::CellTag::TopoLevel) || (levelnum == 0),
             typename MultigridTag = typename T_Configuration::MultigridTag>
  class segment;

  
  /********* Boundary stuff *******************/

  struct NoBoundaryRead {};                     //do not read boundary from file
  
  // Key type for boundary (c.f. MappingKeyType):
  // Template parameter id is to enable several simultaneous boundaries on a domain/segment
  template <long id>
  struct BoundaryKeyType
  {
    //needed for compatibility with std::map
    bool operator<(BoundaryKeyType const & bkt2) const
    {
      return false;     // a < b is false AND b < a is false, hence a == b
    }
  };


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
