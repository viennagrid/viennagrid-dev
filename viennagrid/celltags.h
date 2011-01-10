/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_CELLTAGS_GUARD
#define VIENNAGRID_CELLTAGS_GUARD

#include "viennagrid/forwards.h"

namespace viennagrid
{

  //Segment:
  template <long level>
  struct SegmentConfig
  {
    //default: No handling
    typedef TopoLevelNoHandling    HandlingTag;
  };

  template <>
  struct SegmentConfig<0>
  {
    typedef TopoLevelFullHandling    HandlingTag;
  };

  template <>
  struct SegmentConfig<1>
  {
    typedef TopoLevelFullHandling    HandlingTag;
  };

  template <>
  struct SegmentConfig<2>
  {
    typedef TopoLevelFullHandling    HandlingTag;
  };

  template <>
  struct SegmentConfig<3>
  {
    typedef TopoLevelFullHandling    HandlingTag;
  };


  //Cell Tags:
  struct NullTag {};

  struct PointTag
  {
    static std::string getName() { return std::string("Point"); }
    enum { TopoLevel = 0 };
    typedef ProvideID             IDHandler;

    static std::string getString() { return "Line"; }
    static double getRefVolume() { return 1.0; }

    //integration: arbitrary order
    template <typename EXPR, typename Cell, typename Basisfun1, typename Basisfun2, typename IntegrationTag>
    static double integrate(EXPR const & expr, Cell & cell, Basisfun1 const & bf1, Basisfun2 const & bf2, IntegrationTag)
    {
      typedef typename DomainTypes<typename Cell::Configuration>::PointType    PointType;

      //std::cout << "Integrating!" << std::endl;
      return expr(cell, bf1, bf2, PointType());
    }

  };

  struct LineTag
  {
    static std::string getName() { return std::string("Line/Edge"); }
    enum { TopoLevel = 1 };
    typedef ProvideID             IDHandler;

    //static double facetvolume[2];

    static std::string getString() { return "Line"; }
    static double getRefVolume() { return 1.0; }

    template <typename PointType>
    static PointType mapFacetToCellCoordinates( long facetnum)
    {
      switch (facetnum)
      {
        case 0:   return PointType(0.0); 
        case 1:   return PointType(1.0); 
        default:   return PointType(0.0); 
      }
    }


    //refinement:
    template <typename VertexType, typename ChildrenList, typename SegmentType>
    static void refineUniformly(VertexType ** vertices, ChildrenList & children, long & cell_id, SegmentType & seg)
    {
      typedef typename DomainTypes<typename SegmentType::Configuration>::CellType    CellType;
      typedef typename CellType::ElementTag                             CellTag;

      VertexType * newvertices[3];
      VertexType vertex;

      newvertices[0] = seg.template add<0>(0, *(vertices[0]));

      vertex.getPoint() = (vertices[0]->getPoint() + vertices[1]->getPoint());
      vertex.getPoint() /= 2.0;
      newvertices[1] = seg.template add<0>(0, vertex);

      newvertices[2] = seg.template add<0>(0, *(vertices[1]));

      CellType cell;
      VertexType * cellvertices[2];

      //std::cout << "Setting up cells..." << std::endl;
      //std::cout << "Cell 1..." << std::endl;
      cellvertices[0] = newvertices[0];
      cellvertices[1] = newvertices[1];
      cell.setVertices(&(cellvertices[0]));
      cell.setID(cell_id);
      children.push_back( seg.template add<CellTag::TopoLevel>(cell_id, cell) );
      ++cell_id;

      //std::cout << "Cell 2..." << std::endl;
      cellvertices[0] = newvertices[1];
      cellvertices[1] = newvertices[2];
      cell.setVertices(&(cellvertices[0]));
      cell.setID(cell_id);
      children.push_back( seg.template add<CellTag::TopoLevel>(cell_id, cell) );
      ++cell_id;
    }

  };

  //static members:
  //double LineTag::facetvolume[] = { 1.0, 1.0 };


  struct TriangleTag
  {
    enum{ TopoLevel = 2 };
    typedef ProvideID             IDHandler;

    static std::string getName() { return std::string("Triangle"); }
    static double getRefVolume() { return 0.5; }


      //length of edges of the reference triangle
      //static double facetvolume[3];

      //map facet-coordinate r to cell-coordinates (x,y)
      template <typename PointType>
      static PointType mapFacetToCellCoordinates( long facetnum, double r)
      {
        switch (facetnum)
        {
          case 0:   return PointType(r, 0.0); 
          case 1:   return PointType(0.0, r); 
          case 2:   return PointType(1.0 - r, r); 
          default:   return PointType(0.0, 0.0, 0.0); 
        }
      }


  };

  //static members:
  //double TriangleTag::facetvolume[] = { 1.0, 1.414213562373095049, 1.0 };

  ///////////////// TetrahedronTag /////////////////

  struct TetrahedronTag
  {
    enum{ TopoLevel = 3 };
    typedef ProvideID             IDHandler;

    static std::string getName() { return std::string("Tetrahedron"); }
    static double getRefVolume() { return 1.0/6.0; }

    //area of (triangle) facets of the reference tetrahedron
    //static double facetvolume[4];

    //map facet-coordinate r to cell-coordinates (x,y)
    template <typename PointType>
    static PointType mapFacetToCellCoordinates( long facetnum, double r, double s)
    {
      switch (facetnum)
      {
        case 0:   return PointType(r, s, 0.0); 
        case 1:   return PointType(r, 0.0, s); 
        case 2:   return PointType(0.0, r, s); 
        case 3:   return PointType(1.0 - r - s, r , s); 
        default:   return PointType(0.0, 0.0, 0.0);
      }
    }


  };

  //static members:
  //double TetrahedronTag::facetvolume[] = { 1.0, 2.449489742783178098, 1.0, 1.0 };


  template <typename ElementTag_, long i = ElementTag_::TopoLevel>
  struct TopologyLevel
  {
    //the default case is simultaneously a pathetic case:
    //cell-handling within the cell

    enum{ ElementNum = 1 };     //1 cell

    typedef ElementTag_                  ElementTag;
    //compatibility with cell-on-cell-iterator
    typedef TopoLevelFullHandling     HandlingTag;
  };

  //Point:
  template <>
  struct TopologyLevel<PointTag, 0>
  {
    typedef PointTag              ElementTag;
    typedef ProvideID             IDHandler;
    typedef TopoLevelFullHandling     HandlingTag;

    enum{ ElementNum = 1 };     //1 vertex
  };

  //Line:
  template <>
  struct TopologyLevel<LineTag, 0>
  {
    typedef PointTag              ElementTag;
    typedef TopoLevelFullHandling     HandlingTag;

    enum{ ElementNum = 2 };     //2 vertices
  };

  //Triangle:
  template <>
  struct TopologyLevel<TriangleTag, 0>
  {
    typedef PointTag                  ElementTag;
    typedef TopoLevelFullHandling     HandlingTag;

    enum{ ElementNum = 3 };     //3 vertices
  };

  template <>
  struct TopologyLevel<TriangleTag, 1>
  {
    typedef LineTag                   ElementTag;
    typedef TopoLevelFullHandling     HandlingTag;

    enum{ ElementNum = 3 };     //3 edges

    //fill edges:
    template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
    static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
    {
      Vertices * edgevertices[2];
      ElementType edge;

      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[1];
      edge.setVertices(edgevertices);
      elements[0] = seg.template add<1>(0, edge, orientations);

      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[2];
      edge.setVertices(edgevertices);
      elements[1] = seg.template add<1>(0, edge, orientations + 1 );

      edgevertices[0] = vertices[1];
      edgevertices[1] = vertices[2];
      edge.setVertices(edgevertices);
      elements[2] = seg.template add<1>(0, edge, orientations + 2 );
    }
  };


  //Tetrahedron:
  template <>
  struct TopologyLevel<TetrahedronTag, 0>
  {
    typedef PointTag             ElementTag;
    typedef TopoLevelFullHandling     HandlingTag;

    enum{ ElementNum = 4 };     //4 vertices
  };

  template <>
  struct TopologyLevel<TetrahedronTag, 1>
  {
    typedef LineTag              ElementTag;
    typedef TopoLevelFullHandling     HandlingTag;

    enum{ ElementNum = 6 };     //6 edges

    //fill edges:
    template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
    static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
    {
      Vertices * edgevertices[2];
      ElementType edge;

      //fill edges according to orientation and ordering induced by k-tuple-metafunction:
      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[1];
      edge.setVertices(edgevertices);
      elements[0] = seg.template add<1>(0, edge, orientations);

      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[2];
      edge.setVertices(edgevertices);
      elements[1] = seg.template add<1>(0, edge, orientations + 1);

      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[3];
      edge.setVertices(edgevertices);
      elements[2] = seg.template add<1>(0, edge, orientations + 2);

      edgevertices[0] = vertices[1];
      edgevertices[1] = vertices[2];
      edge.setVertices(edgevertices);
      elements[3] = seg.template add<1>(0, edge, orientations + 3);

      edgevertices[0] = vertices[1];
      edgevertices[1] = vertices[3];
      edge.setVertices(edgevertices);
      elements[4] = seg.template add<1>(0, edge, orientations + 4);

      edgevertices[0] = vertices[2];
      edgevertices[1] = vertices[3];
      edge.setVertices(edgevertices);
      elements[5] = seg.template add<1>(0, edge, orientations + 5);
    }
  };

  template <>
  struct TopologyLevel<TetrahedronTag, 2>
  {
    typedef TriangleTag          ElementTag;
    typedef TopoLevelFullHandling     HandlingTag;

    enum{ ElementNum = 4 };     //4 facets

    //fill facets:
    template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
    static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
    {
      Vertices * facetvertices[3];
      ElementType facet;

      //fill edges according to orientation and ordering induced by k-tuple-metafunction:

      facetvertices[0] = vertices[0];
      facetvertices[1] = vertices[1];
      facetvertices[2] = vertices[2];
      facet.setVertices(facetvertices);
      elements[0] = seg.template add<2>(0, facet, orientations );
      //this new facet must be initialized too:
      elements[0]->fill(seg);

      facetvertices[0] = vertices[0];
      facetvertices[1] = vertices[1];
      facetvertices[2] = vertices[3];
      facet.setVertices(facetvertices);
      elements[1] = seg.template add<2>(0, facet, orientations + 1 );
      elements[1]->fill(seg);

      facetvertices[0] = vertices[0];
      facetvertices[1] = vertices[2];
      facetvertices[2] = vertices[3];
      facet.setVertices(facetvertices);
      elements[2] = seg.template add<2>(0, facet, orientations + 2 );
      elements[2]->fill(seg);

      facetvertices[0] = vertices[1];
      facetvertices[1] = vertices[2];
      facetvertices[2] = vertices[3];
      facet.setVertices(facetvertices);
      elements[3] = seg.template add<2>(0, facet, orientations + 3 );
      elements[3]->fill(seg);

    }
  };

}

#endif