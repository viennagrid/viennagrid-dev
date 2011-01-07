/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */

/******* Provides sample domain configurations
*******  A domain configuration is a container
         holding the (class) types of the topological elements ************/

#ifndef VIENNAMESH_DOMAINCONFIGURATION_GUARD
#define VIENNAMESH_DOMAINCONFIGURATION_GUARD

namespace viennamesh{

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

  struct UniformTriangleSegment
  {
    typedef double                                              CoordType;
    typedef TwoDimensionsTag                                    DimensionTag;

    //Cell and Facet tags:
    typedef TriangleTag                                         CellTag;

    //Cell info:
    typedef TopoHandleAll                                	CellTopologyHandling;
    typedef DtDxStoreStatically                                 CellDtDxHandler;

    /**************************************************************/
    /***********   handler for topological elements: **************/

    //ID:
    typedef ProvideID                                           VertexIDHandler;
    typedef ProvideID                                           EdgeIDHandler;
    typedef NoID                                                FacetIDHandler;
    typedef ProvideID                                           CellIDHandler;
    typedef NoID                                                SegmentIDHandler;

    //Edge Handling on a segment:
    typedef GetSegmentFromTopo<CellTopologyHandling>::EdgeHandling
                                                                  EdgeHandler;

    //Facet Handling on a segment:
    typedef GetSegmentFromTopo<CellTopologyHandling>::FacetHandling
                                                                  FacetHandler;
    //Read boundary from file?
    //yes: use BoundaryKeyType<id>
    //no:  use NoBoundaryRead
    //typedef BoundaryKeyType<11>                                       BoundaryReadTag;
    typedef NoBoundaryRead                                  BoundaryReadTag;

  };

  struct UniformTetrahedraSegment
  {
    typedef double                                              CoordType;
    typedef ThreeDimensionsTag                                  DimensionTag;

    //Cell and Facet tags:
    typedef TetrahedraTag                                       CellTag;

    //Cell info:
    typedef TopoHandleEdgesOnly                                      CellTopologyHandling;
    typedef DtDxStoreStatically                                    CellDtDxHandler;

    /**************************************************************/
    /***********   handler for topological elements: **************/

    //ID:
    typedef ProvideID                                           VertexIDHandler;
    typedef NoID                                           EdgeIDHandler;
    typedef NoID                                           FacetIDHandler;
    typedef NoID                                           CellIDHandler;
    typedef NoID                                                SegmentIDHandler;

    //Edge Handling on a segment:
    typedef GetSegmentFromTopo<CellTopologyHandling>::EdgeHandling
                                                                  EdgeHandler;

    //Facet Handling on a segment:
    typedef GetSegmentFromTopo<CellTopologyHandling>::FacetHandling
                                                                  FacetHandler;
    //Read boundary from file?
    //yes: use BoundaryKeyType<id>
    //no:  use NoBoundaryRead
    //typedef BoundaryKeyType<11>                                  BoundaryReadTag;
    typedef NoBoundaryRead                                  BoundaryReadTag;
  };




}

#endif
