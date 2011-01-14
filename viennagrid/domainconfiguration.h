/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

/******* Provides sample domain configurations
*******  A domain configuration is a container
         holding the (class) types of the topological elements ************/

#ifndef VIENNAGRID_DOMAINCONFIGURATION_GUARD
#define VIENNAGRID_DOMAINCONFIGURATION_GUARD

namespace viennagrid
{

  //metafunction for a proper segment-configuration:
  template <typename T>
  struct GetSegmentFromTopo {};

  template <>
  struct GetSegmentFromTopo<TopoHandleAll>
  {
    typedef Segment_FullEdgeHandling        EdgeHandling;
    typedef Segment_FullFacetHandling       FacetHandling;
  };

  template <>
  struct GetSegmentFromTopo<TopoHandleEdgesOnly>
  {
    typedef Segment_FullEdgeHandling        EdgeHandling;
    typedef Segment_NoFacetHandling         FacetHandling;
  };

  template <>
  struct GetSegmentFromTopo<TopoHandleNone>
  {
    typedef Segment_NoEdgeHandling          EdgeHandling;
    typedef Segment_NoFacetHandling         FacetHandling;
  };


}

#endif
