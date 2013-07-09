#ifndef VIENNAGRID_TOPOLOGY_PLC_HPP
#define VIENNAGRID_TOPOLOGY_PLC_HPP

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
#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/topology/polygon.hpp"

/** @file polygon.hpp
    @brief Provides the topological definition of a polygon
*/


namespace viennagrid
{

  struct plc_tag
  {
    typedef line_tag facet_tag;
      
    enum{ dim = 2 };
    static std::string name() { return "PLC"; }
  };
  
  //Line:
  /** @brief Topological description of the 0-cells of a polygon */
  template <>
  struct boundary_elements<plc_tag, vertex_tag>
  {
    typedef dynamic_unique_layout_tag     layout_tag;
    enum{ num = -1 };
  };
  
  template <>
  struct boundary_elements<plc_tag, line_tag>
  {
    typedef dynamic_unique_layout_tag     layout_tag;
    enum{ num = -1 };
  };
  
  
  
  
  
  namespace element_topology
  {


    
    
    template<typename BoundaryElementType>
    struct boundary_element_generator<plc_tag, line_tag, BoundaryElementType>
    {
        template<typename element_type, typename inserter_type>
        static void create_boundary_elements(element_type &, inserter_type &)
        {
        }
    };
    
    template<typename BoundaryElementType>
    struct boundary_element_generator<plc_tag, simplex_tag<0>, BoundaryElementType>
    {
        template<typename element_type, typename inserter_type>
        static void create_boundary_elements(element_type & plc, inserter_type &)
        {
            typedef typename result_of::element<element_type, line_tag>::type LineType;
            typedef typename result_of::element_range<element_type, line_tag>::type LineRange;
            typedef typename result_of::iterator<LineRange>::type LineIterator;
            
            LineRange lines = elements<line_tag>(plc);
            for (LineIterator lit = lines.handle_begin(); lit != lines.handle_end(); ++lit)
            {
                typedef typename result_of::element_range<LineType, vertex_tag>::type VertexOnLineRange;
                typedef typename result_of::handle_iterator<VertexOnLineRange>::type VertexOnLineHandleIterator;
                
                VertexOnLineRange vertices = elements<vertex_tag>(*lit);
                for (VertexOnLineHandleIterator vlhit = vertices.begin(); vlhit != vertices.end(); ++vlhit)
                    plc.container( vertex_tag() ).insert_handle( *vlhit );
            }
            
        }
    };


    

  } //topology
}

#endif

