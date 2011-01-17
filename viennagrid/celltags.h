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
    typedef topology_level_no_handling    HandlingTag;
  };

  template <>
  struct SegmentConfig<0>
  {
    typedef topology_level_full_handling    HandlingTag;
  };

  template <>
  struct SegmentConfig<1>
  {
    typedef topology_level_full_handling    HandlingTag;
  };

  template <>
  struct SegmentConfig<2>
  {
    typedef topology_level_full_handling    HandlingTag;
  };

  template <>
  struct SegmentConfig<3>
  {
    typedef topology_level_full_handling    HandlingTag;
  };


  //Cell Tags:
  struct null_tag {};

  struct point_tag
  {
    enum { topology_level = 0 };
    typedef ProvideID             IDHandler;

    static std::string name() { return "Point"; }
  };

  struct line_tag
  {
    enum { topology_level = 1 };
    typedef ProvideID             IDHandler;

    static std::string name() { return "Line"; }

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
      children.push_back( seg.template add<CellTag::topology_level>(cell_id, cell) );
      ++cell_id;

      //std::cout << "Cell 2..." << std::endl;
      cellvertices[0] = newvertices[1];
      cellvertices[1] = newvertices[2];
      cell.setVertices(&(cellvertices[0]));
      cell.setID(cell_id);
      children.push_back( seg.template add<CellTag::topology_level>(cell_id, cell) );
      ++cell_id;
    }

  };

  struct triangle_tag
  {
    enum{ topology_level = 2 };
    typedef ProvideID             IDHandler;

    static std::string name() { return "Triangle"; }

  };

  ///////////////// tetrahedron_tag /////////////////

  struct tetrahedron_tag
  {
    enum{ topology_level = 3 };
    typedef ProvideID             IDHandler;

    static std::string name() { return "Tetrahedron"; }
  };

  //static members:
  //double tetrahedron_tag::facetvolume[] = { 1.0, 2.449489742783178098, 1.0, 1.0 };


  
  
  
  
  
  
  template <typename ElementTag_, long i = ElementTag_::topology_level>
  struct TopologyLevel
  {
    //the default case is simultaneously a pathetic case:
    //cell-handling within the cell

    enum{ ElementNum = 1 };     //1 cell

    typedef ElementTag_                  ElementTag;
    //compatibility with cell-on-cell-iterator
    typedef topology_level_full_handling     HandlingTag;
  };

  //Point:
  template <>
  struct TopologyLevel<point_tag, 0>
  {
    typedef point_tag              ElementTag;
    typedef ProvideID             IDHandler;
    typedef topology_level_full_handling     HandlingTag;

    enum{ ElementNum = 1 };     //1 vertex
  };

  //Line:
  template <>
  struct TopologyLevel<line_tag, 0>
  {
    typedef point_tag              ElementTag;
    typedef topology_level_full_handling     HandlingTag;

    enum{ ElementNum = 2 };     //2 vertices
  };

  //Triangle:
  template <>
  struct TopologyLevel<triangle_tag, 0>
  {
    typedef point_tag                  ElementTag;
    typedef topology_level_full_handling     HandlingTag;

    enum{ ElementNum = 3 };     //3 vertices
  };

  template <>
  struct TopologyLevel<triangle_tag, 1>
  {
    typedef line_tag                   ElementTag;
    typedef topology_level_full_handling     HandlingTag;

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
  struct TopologyLevel<tetrahedron_tag, 0>
  {
    typedef point_tag             ElementTag;
    typedef topology_level_full_handling     HandlingTag;

    enum{ ElementNum = 4 };     //4 vertices
  };

  template <>
  struct TopologyLevel<tetrahedron_tag, 1>
  {
    typedef line_tag              ElementTag;
    typedef topology_level_full_handling     HandlingTag;

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
  struct TopologyLevel<tetrahedron_tag, 2>
  {
    typedef triangle_tag          ElementTag;
    typedef topology_level_full_handling     HandlingTag;

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