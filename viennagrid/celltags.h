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
#include <../ViennaSHE/external/CL/cl_platform.h>

namespace viennagrid
{

  //Segment:
  template <long level>
  struct segment_traits
  {
    //default: Full handling
    typedef topology_level_full_handling    handling_tag;
  };


  //Cell Tags:
  struct null_tag {};

  struct point_tag
  {
    enum { topology_level = 0 };
    static std::string name() { return "Point"; }
  };

  struct line_tag
  {
    enum { topology_level = 1 };
    static std::string name() { return "Line"; }
  };

  ///////////////// two-dimensional objects: /////////////////
  struct triangle_tag
  {
    enum{ topology_level = 2 };
    static std::string name() { return "Triangle"; }
  };

  struct parallelogram_tag
  {
    enum{ topology_level = 2 };
    static std::string name() { return "Parallelogram"; }
  };
  
  ///////////////// three-dimensional objects: /////////////////

  struct tetrahedron_tag
  {
    enum{ topology_level = 3 };
    static std::string name() { return "Tetrahedron"; }
  };

  struct parallelepiped_tag
  {
    enum{ topology_level = 3 };
    static std::string name() { return "Parallelepiped"; }
  };
  
  
  
  
  
  
  
  
  template <typename CellTag>
  struct cell_traits
  {
    typedef ProvideID     id_handler;
  };
  
  // cell_traits can be specialized by library users in order to use 'NoID' instead of 'ProvideID' 
  
  
  
  
  template <typename ElementTag_, 
            long level = ElementTag_::topology_level>
  struct subcell_traits
  {
    //the default case is simultaneously a pathetic case:
    //cell-handling within the cell

    enum{ ElementNum = 1 };     //1 cell

    typedef ElementTag_                      element_tag;
    //compatibility with cell-on-cell-iterator
    typedef topology_level_full_handling     handling_tag;
  };

  //Point:
  template <>
  struct subcell_traits<point_tag, 0>
  {
    typedef point_tag                        element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 1 };     //1 vertex
  };

  //Line:
  template <>
  struct subcell_traits<line_tag, 0>
  {
    typedef point_tag                        element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 2 };     //2 vertices
  };

  //Triangle:
  template <>
  struct subcell_traits<triangle_tag, 0>
  {
    typedef point_tag                        element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 3 };     //3 vertices
  };

  template <>
  struct subcell_traits<triangle_tag, 1>
  {
    typedef line_tag                         element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 3 };     //3 edges

  };
  
  //parallelogram
  template <>
  struct subcell_traits<parallelogram_tag, 0>
  {
    typedef point_tag                        element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 4 };     //3 vertices
  };

  template <>
  struct subcell_traits<parallelogram_tag, 1>
  {
    typedef line_tag                         element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 4 };     //3 edges

  };


  //Tetrahedron:
  template <>
  struct subcell_traits<tetrahedron_tag, 0>
  {
    typedef point_tag                        element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 4 };     //4 vertices
  };

  template <>
  struct subcell_traits<tetrahedron_tag, 1>
  {
    typedef line_tag                         element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 6 };     //6 edges
  };

  template <>
  struct subcell_traits<tetrahedron_tag, 2>
  {
    typedef triangle_tag                     element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 4 };     //4 facets
  };

  
  //Parallelepiped:
  template <>
  struct subcell_traits<parallelepiped_tag, 0>
  {
    typedef point_tag                        element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 8 };     //8 vertices
  };

  template <>
  struct subcell_traits<parallelepiped_tag, 1>
  {
    typedef line_tag                         element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 12 };     //12 edges
  };

  template <>
  struct subcell_traits<parallelepiped_tag, 2>
  {
    typedef parallelogram_tag                element_tag;
    typedef topology_level_full_handling     handling_tag;

    enum{ ElementNum = 6 };     //6 facets
  };
  
  ///////////////////////////////// Filler for subcell elements ///////////////////////////////////
  
  template <typename CellType, long level>
  struct subcell_filler {};

  //////// Triangle ////////
  template <>
  struct subcell_filler<triangle_tag, 1>
  {
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

  //////// Parallelogram ////////
  template <>
  struct subcell_filler<parallelogram_tag, 1>
  {
    //fill edges:
    //
    // Reference orientation: (v... vertices, e...edges)
    //        e2
    // v3 --------- v2
    //    |       |
    // e3 |       | e1
    //    |       |
    // v0 --------- v1
    //        e0
    template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
    static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
    {
      Vertices * edgevertices[2];
      ElementType edge;

      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[1];
      edge.setVertices(edgevertices);
      elements[0] = seg.template add<1>(0, edge, orientations);

      edgevertices[0] = vertices[1];
      edgevertices[1] = vertices[2];
      edge.setVertices(edgevertices);
      elements[1] = seg.template add<1>(0, edge, orientations + 1 );

      edgevertices[0] = vertices[2];
      edgevertices[1] = vertices[3];
      edge.setVertices(edgevertices);
      elements[2] = seg.template add<1>(0, edge, orientations + 2 );

      edgevertices[0] = vertices[3];
      edgevertices[1] = vertices[0];
      edge.setVertices(edgevertices);
      elements[3] = seg.template add<1>(0, edge, orientations + 3 );
      
    }
  };
    
  
  
    
  //////// Tetrahedron ////////
  template <>
  struct subcell_filler<tetrahedron_tag, 1>
  {
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
  struct subcell_filler<tetrahedron_tag, 2>
  {
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

  
  
  //////// Parallelepiped ////////
  template <>
  struct subcell_filler<parallelepiped_tag, 1>
  {
    //fill edges:
    template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
    static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
    {
      Vertices * edgevertices[2];
      ElementType edge;

      //fill edges according to reference orientation (use monospaced font for Full-HD ascii-art):
      //
      //              e10
      //    v7-----------------v6
      // e11 /|             e9/|
      //    / |   e8      v5 / |
      // v4------------------  |e6
      //   |  |e7           |  |
      // e4|  |______e2_____|__|
      //   |  /v3         e5| /  v2
      //   | /e3            |/ e1
      // v0 ----------------  v1
      //           e0 
      //
      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[1];
      edge.setVertices(edgevertices);
      elements[0] = seg.template add<1>(0, edge, orientations);

      edgevertices[0] = vertices[1];
      edgevertices[1] = vertices[2];
      edge.setVertices(edgevertices);
      elements[1] = seg.template add<1>(0, edge, orientations + 1);

      edgevertices[0] = vertices[2];
      edgevertices[1] = vertices[3];
      edge.setVertices(edgevertices);
      elements[2] = seg.template add<1>(0, edge, orientations + 2);

      edgevertices[0] = vertices[3];
      edgevertices[1] = vertices[0];
      edge.setVertices(edgevertices);
      elements[3] = seg.template add<1>(0, edge, orientations + 3);

      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[4];
      edge.setVertices(edgevertices);
      elements[4] = seg.template add<1>(0, edge, orientations + 4);

      edgevertices[0] = vertices[1];
      edgevertices[1] = vertices[5];
      edge.setVertices(edgevertices);
      elements[5] = seg.template add<1>(0, edge, orientations + 5);
      
      edgevertices[0] = vertices[2];
      edgevertices[1] = vertices[6];
      edge.setVertices(edgevertices);
      elements[6] = seg.template add<1>(0, edge, orientations + 6);
      
      edgevertices[0] = vertices[3];
      edgevertices[1] = vertices[7];
      edge.setVertices(edgevertices);
      elements[7] = seg.template add<1>(0, edge, orientations + 7);
      
      edgevertices[0] = vertices[4];
      edgevertices[1] = vertices[5];
      edge.setVertices(edgevertices);
      elements[8] = seg.template add<1>(0, edge, orientations + 8);
      
      edgevertices[0] = vertices[5];
      edgevertices[1] = vertices[6];
      edge.setVertices(edgevertices);
      elements[9] = seg.template add<1>(0, edge, orientations + 9);
      
      edgevertices[0] = vertices[6];
      edgevertices[1] = vertices[7];
      edge.setVertices(edgevertices);
      elements[10] = seg.template add<1>(0, edge, orientations + 10);
      
      edgevertices[0] = vertices[7];
      edgevertices[1] = vertices[4];
      edge.setVertices(edgevertices);
      elements[11] = seg.template add<1>(0, edge, orientations + 11);
    }
  };
  
  template <>
  struct subcell_filler<parallelepiped_tag, 2>
  {
    //fill facets:
    template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
    static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
    {
      Vertices * facetvertices[4];
      ElementType facet;

      //fill edges according to reference orientation (use monospaced font for Full-HD ascii-art):
      //
      //              
      //    v7-----------------v6
      //     /|      f5       /|
      //    / |           v5 / |
      // v4------------------  |
      //   |f4|  (f3-back)  |f2|
      //   |  |_____________|__|
      //   |  /v3   f1      | /  v2
      //   | /              |/ 
      // v0 ----------------  v1
      //       (f0-bottom)
      //
      // orientation is such that facet normals point out of the cell
      facetvertices[0] = vertices[0];
      facetvertices[1] = vertices[3];
      facetvertices[2] = vertices[2];
      facetvertices[3] = vertices[1];
      facet.setVertices(facetvertices);
      elements[0] = seg.template add<2>(0, facet, orientations );
      elements[0]->fill(seg);

      facetvertices[0] = vertices[0];
      facetvertices[1] = vertices[1];
      facetvertices[2] = vertices[5];
      facetvertices[3] = vertices[4];
      facet.setVertices(facetvertices);
      elements[1] = seg.template add<2>(0, facet, orientations + 1);
      elements[1]->fill(seg);

      facetvertices[0] = vertices[1];
      facetvertices[1] = vertices[2];
      facetvertices[2] = vertices[6];
      facetvertices[3] = vertices[5];
      facet.setVertices(facetvertices);
      elements[2] = seg.template add<2>(0, facet, orientations + 2);
      elements[2]->fill(seg);

      facetvertices[0] = vertices[2];
      facetvertices[1] = vertices[3];
      facetvertices[2] = vertices[7];
      facetvertices[3] = vertices[6];
      facet.setVertices(facetvertices);
      elements[3] = seg.template add<2>(0, facet, orientations + 3);
      elements[3]->fill(seg);

      facetvertices[0] = vertices[3];
      facetvertices[1] = vertices[0];
      facetvertices[2] = vertices[4];
      facetvertices[3] = vertices[7];
      facet.setVertices(facetvertices);
      elements[4] = seg.template add<2>(0, facet, orientations + 4);
      elements[4]->fill(seg);

      facetvertices[0] = vertices[4];
      facetvertices[1] = vertices[5];
      facetvertices[2] = vertices[6];
      facetvertices[3] = vertices[7];
      facet.setVertices(facetvertices);
      elements[5] = seg.template add<2>(0, facet, orientations + 5);
      elements[5]->fill(seg);
      
    }
  };
  
}

#endif