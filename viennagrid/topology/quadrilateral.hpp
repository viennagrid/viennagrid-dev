/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_TOPOLOGY_QUADRILATERAL_HPP
#define VIENNAGRID_TOPOLOGY_QUADRILATERAL_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"

namespace viennagrid
{

  struct quadrilateral_tag
  {
    enum{ topology_level = 2 };
    static std::string name() { return "Quadrilateral"; }
  };
  
  namespace traits
  {
  
    //parallelogram
    template <>
    struct subcell_desc<quadrilateral_tag, 0>
    {
      typedef point_tag             element_tag;
      typedef full_handling_tag     handling_tag;

      enum{ num_elements = 4 };     //3 vertices
    };

    template <>
    struct subcell_desc<quadrilateral_tag, 1>
    {
      typedef line_tag              element_tag;
      typedef full_handling_tag     handling_tag;

      enum{ num_elements = 4 };     //3 edges

    };



    //////// Parallelogram ////////
    template <>
    struct subcell_filler<quadrilateral_tag, 1>
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
        elements[0] = seg.add(edge, orientations);

        edgevertices[0] = vertices[1];
        edgevertices[1] = vertices[2];
        edge.setVertices(edgevertices);
        elements[1] = seg.add(edge, orientations + 1 );

        edgevertices[0] = vertices[2];
        edgevertices[1] = vertices[3];
        edge.setVertices(edgevertices);
        elements[2] = seg.add(edge, orientations + 2 );

        edgevertices[0] = vertices[3];
        edgevertices[1] = vertices[0];
        edge.setVertices(edgevertices);
        elements[3] = seg.add(edge, orientations + 3 );
        
      }
    };
  }
    
}

#endif