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
#include "viennagrid/element/element_view.hpp"

/** @file polygon.hpp
    @brief Provides the topological definition of a polygon
*/

namespace viennagrid
{

  struct plc_tag
  {
    typedef polygon_tag facet_tag;
      
    enum{ dim = 2 };
    static std::string name() { return "PLC"; }
  };
  
  
  namespace topology
  {

    //Line:
    /** @brief Topological description of the 0-cells of a polygon */
    template <>
    struct boundary_cells<plc_tag, vertex_tag>
    {
      typedef dynamic_unique_layout_tag     layout_tag;
      enum{ num = -1 };
    };
    
    template <>
    struct boundary_cells<plc_tag, line_tag>
    {
      typedef dynamic_unique_layout_tag     layout_tag;
      enum{ num = -1 };
    };
    
    template <>
    struct boundary_cells<plc_tag, polygon_tag>
    {
      typedef dynamic_unique_layout_tag     layout_tag;
      enum{ num = -1 };
    };


    
    
    template<typename bnd_cell_type>
    struct bndcell_generator<plc_tag, polygon_tag, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & element, inserter_type & inserter)
        {}
    };
    
    template<typename bnd_cell_type>
    struct bndcell_generator<plc_tag, line_tag, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & plc, inserter_type & inserter)
        {
            typedef typename result_of::element<element_type, polygon_tag>::type PolygonType;
            typedef typename result_of::element_range<element_type, polygon_tag>::type PolygonRange;
            typedef typename result_of::iterator<PolygonRange>::type PolygonIterator;
            
            PolygonRange polygons = elements<polygon_tag>(plc);
            for (PolygonIterator pit = polygons.begin(); pit != polygons.end(); ++pit)
            {
                typedef typename result_of::element_range<PolygonType, line_tag>::type LineOnPolygonRange;
                typedef typename result_of::hook_iterator<LineOnPolygonRange>::type LineOnPolygonIterator;
                
                LineOnPolygonRange lines = elements<line_tag>(*pit);
                for (LineOnPolygonIterator lphit = lines.begin(); lphit != lines.end(); ++lphit)
                {
                    plc.container( line_tag() ).insert_hook( *lphit );
                    if (is_tagged<bounding_tag>(*pit, plc))
                        tag<bounding_tag>(viennagrid::dereference_hook(plc,*lphit), plc);
                }
            }
        }
    };
    
    template<typename bnd_cell_type>
    struct bndcell_generator<plc_tag, simplex_tag<0>, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & plc, inserter_type & inserter)
        {
            typedef typename result_of::element<element_type, line_tag>::type LineType;
            typedef typename result_of::element_range<element_type, line_tag>::type LineRange;
            typedef typename result_of::iterator<LineRange>::type LineIterator;
            
            LineRange lines = elements<line_tag>(plc);
            for (LineIterator lit = lines.hook_begin(); lit != lines.hook_end(); ++lit)
            {
                typedef typename result_of::element_range<LineType, vertex_tag>::type VertexOnLineRange;
                typedef typename result_of::hook_iterator<VertexOnLineRange>::type VertexOnLineHookIterator;
                
                VertexOnLineRange vertices = elements<vertex_tag>(*lit);
                for (VertexOnLineHookIterator vlhit = vertices.begin(); vlhit != vertices.end(); ++vlhit)
                    plc.container( vertex_tag() ).insert_hook( *vlhit );
            }
            
            
            
            typedef typename result_of::element<element_type, polygon_tag>::type PolygonType;
            typedef typename result_of::element_range<element_type, polygon_tag>::type PolygonRange;
            typedef typename result_of::iterator<PolygonRange>::type PolygonIterator;
            
            PolygonRange polygons = elements<polygon_tag>(plc);
            for (PolygonIterator pit = polygons.hook_begin(); pit != polygons.hook_end(); ++pit)
            {
                typedef typename result_of::element_range<PolygonType, vertex_tag>::type VertexOnPolygonRange;
                typedef typename result_of::hook_iterator<VertexOnPolygonRange>::type VertexOnPolygonIterator;
                
                VertexOnPolygonRange vertices = elements<vertex_tag>(*pit);
                for (VertexOnPolygonIterator vphit = vertices.begin(); vphit != vertices.end(); ++vphit)
                {
                    plc.container( vertex_tag() ).insert_hook( *vphit );
                    if (is_tagged<bounding_tag>(*pit, plc))
                        tag<bounding_tag>(viennagrid::dereference_hook(plc,*vphit), plc);
                }
            }
        }
    };


    

  } //topology
}

#endif

