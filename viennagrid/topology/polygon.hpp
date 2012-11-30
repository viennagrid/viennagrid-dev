#ifndef VIENNAGRID_TOPOLOGY_POLYGON_HPP
#define VIENNAGRID_TOPOLOGY_POLYGON_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
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

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/point.hpp"

/** @file polygon.hpp
    @brief Provides the topological definition of a polygon
*/

namespace viennagrid
{

  struct polygon_tag
  {
    enum{ dim = 2 };
    static std::string name() { return "Polygon"; }
  };


  namespace topology
  {

    //Line:
    /** @brief Topological description of the 0-cells of a polygon */
    template <>
    struct bndcells<polygon_tag, 0>
    {
      typedef vertex_tag            tag;
      
      typedef dynamic_layout_tag    layout_tag;
      enum{ num = -1 };
    };

    template <>
    struct bndcells<polygon_tag, 1>
    {
      typedef simplex_tag<1>              tag;
      
      typedef dynamic_layout_tag    layout_tag;
      enum{ num = -1 };
    };

    
    /** @brief Fills a segment or a domain with the edges of a polygon */
    template <>
    struct bndcell_filler<polygon_tag, 1>
    {
      //fill edges:
      template <typename ElementContainerType, typename VertexContainerType, typename OrientatationContainerType, typename Segment>
      static void fill(ElementContainerType & elements, const VertexContainerType & vertices, OrientatationContainerType & orientations, Segment & seg)
      {
        typename VertexContainerType::value_type edgevertices[2];
        typename utils::remove_pointer<typename ElementContainerType::value_type>::type edge;
        
        elements.resize(vertices.size());
        orientations.resize(vertices.size());
        
        for (size_t i = 0; i < vertices.size()-1; ++i)
        {
            edgevertices[0] = vertices[i];
            edgevertices[1] = vertices[i+1];
            edge.vertices(edgevertices);
            elements[i] = seg.push_back(edge, &orientations[i]);
        }
        
        edgevertices[0] = vertices[ vertices.size()-1 ];
        edgevertices[1] = vertices[ 0 ];
        edge.vertices(edgevertices);
        elements[vertices.size()-1] = seg.push_back(edge, &orientations[vertices.size()-1]);
      }
      
      template <typename ElementContainerType, typename VertexContainerType, typename Segment>
      static void fill(ElementContainerType & elements, const VertexContainerType & vertices, Segment & seg)
      {
        typename VertexContainerType::value_type edgevertices[2];
        typename utils::remove_pointer<typename ElementContainerType::value_type>::type edge;
        
        elements.resize(vertices.size());
        
        for (size_t i = 0; i < vertices.size()-1; ++i)
        {
            edgevertices[0] = vertices[i];
            edgevertices[1] = vertices[i+1];
            edge.vertices(edgevertices);
            elements[i] = seg.push_back(edge, NULL);
        }
        
        edgevertices[0] = vertices[ vertices.size()-1 ];
        edgevertices[1] = vertices[ 0 ];
        edge.vertices(edgevertices);
        elements[vertices.size()-1] = seg.push_back(edge, NULL);
      }
    };

  } //topology
}

#endif

