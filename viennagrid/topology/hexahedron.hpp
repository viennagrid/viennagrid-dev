#ifndef VIENNAGRID_CONFIG_HEXAHEDRON_HPP
#define VIENNAGRID_CONFIG_HEXAHEDRON_HPP

/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/forwards.h"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/quadrilateral.hpp"

namespace viennagrid
{

  template <>
  struct hypercube_tag<3>
  {
    enum{ dim = 3 };
    static std::string name() { return "Hexahedron"; }
  };
  

  namespace topology
  {

    template <>
    struct bndcells<hexahedron_tag, 0>
    {
      typedef point_tag             tag;

      enum{ num = 8 };     //8 vertices
    };

    template <>
    struct bndcells<hexahedron_tag, 1>
    {
      typedef hypercube_tag<1>              tag;

      enum{ num = 12 };     //12 edges
    };

    template <>
    struct bndcells<hexahedron_tag, 2>
    {
      typedef quadrilateral_tag     tag;

      enum{ num = 6 };     //6 facets
    };
    
    
    
    

    template <>
    struct bndcell_filler<hexahedron_tag, 1>
    {
      //fill edges:
      template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
      static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
      {
        Vertices * edgevertices[2];
        ElementType edge;

        //fill edges according to reference orientation (use monospaced font for Full-HD ascii-art):
        //
        //              e11
        //    v6-----------------v7
        // e9  /|            e10/|
        //    / |   e8      v5 / |
        // v4------------------  |e7
        //   |  |e6           |  |
        // e2|  |______e5_____|__|
        //   |  /v2         e4| /  v3
        //   | /e1            |/ e3
        // v0 ----------------  v1
        //           e0 
        //
        edgevertices[0] = vertices[0];
        edgevertices[1] = vertices[1];
        edge.vertices(edgevertices);
        elements[0] = seg.push_back(edge, orientations);

        edgevertices[0] = vertices[0];
        edgevertices[1] = vertices[2];
        edge.vertices(edgevertices);
        elements[1] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 1);

        edgevertices[0] = vertices[0];
        edgevertices[1] = vertices[4];
        edge.vertices(edgevertices);
        elements[2] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 2);

        edgevertices[0] = vertices[1];
        edgevertices[1] = vertices[3];
        edge.vertices(edgevertices);
        elements[3] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 3);

        edgevertices[0] = vertices[1];
        edgevertices[1] = vertices[5];
        edge.vertices(edgevertices);
        elements[4] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 4);

        edgevertices[0] = vertices[2];
        edgevertices[1] = vertices[3];
        edge.vertices(edgevertices);
        elements[5] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 5);
        
        edgevertices[0] = vertices[2];
        edgevertices[1] = vertices[5];
        edge.vertices(edgevertices);
        elements[6] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 6);
        
        edgevertices[0] = vertices[3];
        edgevertices[1] = vertices[7];
        edge.vertices(edgevertices);
        elements[7] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 7);
        
        edgevertices[0] = vertices[4];
        edgevertices[1] = vertices[5];
        edge.vertices(edgevertices);
        elements[8] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 8);
        
        edgevertices[0] = vertices[4];
        edgevertices[1] = vertices[6];
        edge.vertices(edgevertices);
        elements[9] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 9);
        
        edgevertices[0] = vertices[5];
        edgevertices[1] = vertices[7];
        edge.vertices(edgevertices);
        elements[10] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 10);
        
        edgevertices[0] = vertices[6];
        edgevertices[1] = vertices[7];
        edge.vertices(edgevertices);
        elements[11] = seg.push_back(edge, (orientations == NULL) ? NULL : orientations + 11);
      }
    };
    
    template <>
    struct bndcell_filler<hexahedron_tag, 2>
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
        //    v6-----------------v7
        //     /|      f5       /|
        //    / |           v5 / |
        // v4------------------  |
        //   |f2|  (f4-back)  |f3|
        //   |  |_____________|__|
        //   |  /v2   f1      | /  v3
        //   | /              |/ 
        // v0 ----------------  v1
        //       (f0-bottom)
        //
        // orientation is such that facet normals point out of the cell
        facetvertices[0] = vertices[0];
        facetvertices[1] = vertices[1];
        facetvertices[2] = vertices[2];
        facetvertices[3] = vertices[3];
        facet.vertices(facetvertices);
        elements[0] = seg.push_back(facet, (orientations == NULL) ? NULL : orientations );
        elements[0]->fill(seg);

        facetvertices[0] = vertices[0];
        facetvertices[1] = vertices[1];
        facetvertices[2] = vertices[4];
        facetvertices[3] = vertices[5];
        facet.vertices(facetvertices);
        elements[1] = seg.push_back(facet, (orientations == NULL) ? NULL : orientations + 1);
        elements[1]->fill(seg);

        facetvertices[0] = vertices[0];
        facetvertices[1] = vertices[2];
        facetvertices[2] = vertices[4];
        facetvertices[3] = vertices[6];
        facet.vertices(facetvertices);
        elements[2] = seg.push_back(facet, (orientations == NULL) ? NULL : orientations + 2);
        elements[2]->fill(seg);

        facetvertices[0] = vertices[1];
        facetvertices[1] = vertices[3];
        facetvertices[2] = vertices[5];
        facetvertices[3] = vertices[7];
        facet.vertices(facetvertices);
        elements[3] = seg.push_back(facet, (orientations == NULL) ? NULL : orientations + 3);
        elements[3]->fill(seg);

        facetvertices[0] = vertices[2];
        facetvertices[1] = vertices[3];
        facetvertices[2] = vertices[6];
        facetvertices[3] = vertices[7];
        facet.vertices(facetvertices);
        elements[4] = seg.push_back(facet, (orientations == NULL) ? NULL : orientations + 4);
        elements[4]->fill(seg);

        facetvertices[0] = vertices[4];
        facetvertices[1] = vertices[5];
        facetvertices[2] = vertices[6];
        facetvertices[3] = vertices[7];
        facet.vertices(facetvertices);
        elements[5] = seg.push_back(facet, (orientations == NULL) ? NULL : orientations + 5);
        elements[5]->fill(seg);
        
      }
    };
    
  } //topology
}

#endif

