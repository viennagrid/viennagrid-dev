/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_TOPOLOGY_TETRAHEDRON_HPP
#define VIENNAGRID_TOPOLOGY_TETRAHEDRON_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/triangle.hpp"

namespace viennagrid
{


  struct tetrahedron_tag
  {
    enum{ topology_level = 3 };
    static std::string name() { return "Tetrahedron"; }
  };



  //Tetrahedron:
  template <>
  struct subcell_traits<tetrahedron_tag, 0>
  {
    typedef point_tag             element_tag;
    typedef full_handling_tag     handling_tag;

    enum{ num_elements = 4 };     //4 vertices
  };

  template <>
  struct subcell_traits<tetrahedron_tag, 1>
  {
    typedef line_tag              element_tag;
    typedef full_handling_tag     handling_tag;

    enum{ num_elements = 6 };     //6 edges
  };

  template <>
  struct subcell_traits<tetrahedron_tag, 2>
  {
    typedef triangle_tag          element_tag;
    typedef full_handling_tag     handling_tag;

    enum{ num_elements = 4 };     //4 facets
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
      elements[0] = seg.add(edge, orientations);

      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[2];
      edge.setVertices(edgevertices);
      elements[1] = seg.add(edge, orientations + 1);

      edgevertices[0] = vertices[0];
      edgevertices[1] = vertices[3];
      edge.setVertices(edgevertices);
      elements[2] = seg.add(edge, orientations + 2);

      edgevertices[0] = vertices[1];
      edgevertices[1] = vertices[2];
      edge.setVertices(edgevertices);
      elements[3] = seg.add(edge, orientations + 3);

      edgevertices[0] = vertices[1];
      edgevertices[1] = vertices[3];
      edge.setVertices(edgevertices);
      elements[4] = seg.add(edge, orientations + 4);

      edgevertices[0] = vertices[2];
      edgevertices[1] = vertices[3];
      edge.setVertices(edgevertices);
      elements[5] = seg.add(edge, orientations + 5);
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
      elements[0] = seg.add(facet, orientations );
      //this new facet must be initialized too:
      elements[0]->fill(seg);

      facetvertices[0] = vertices[0];
      facetvertices[1] = vertices[1];
      facetvertices[2] = vertices[3];
      facet.setVertices(facetvertices);
      elements[1] = seg.add(facet, orientations + 1 );
      elements[1]->fill(seg);

      facetvertices[0] = vertices[0];
      facetvertices[1] = vertices[2];
      facetvertices[2] = vertices[3];
      facet.setVertices(facetvertices);
      elements[2] = seg.add(facet, orientations + 2 );
      elements[2]->fill(seg);

      facetvertices[0] = vertices[1];
      facetvertices[1] = vertices[2];
      facetvertices[2] = vertices[3];
      facet.setVertices(facetvertices);
      elements[3] = seg.add(facet, orientations + 3 );
      elements[3]->fill(seg);

    }
  };

  
  
}

#endif