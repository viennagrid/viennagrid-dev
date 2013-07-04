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



// namespace viennagrid
// {
//     struct plc_hole_point_tag;
// }
// 
// TODO: holes using appendix!!
// VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH( viennagrid::plc_hole_point_tag )
// 
// namespace viennagrid
// {
//     template<typename domain_type, typename plc_type>
//     std::vector< typename result_of::point_type<domain_type>::type > & hole_points( plc_type & plc )
//     {
//         return viennadata::access< plc_hole_point_tag, std::vector< typename result_of::point_type<domain_type>::type > >()(plc);
//     }
//     
//     template<typename domain_type, typename plc_type>
//     std::vector< typename result_of::point_type<domain_type>::type > const & hole_points( plc_type const & plc )
//     {
//         return viennadata::access< plc_hole_point_tag, std::vector< typename result_of::point_type<domain_type>::type > >()(plc);
//     }
// }
// 

namespace viennagrid
{

  struct plc_tag
  {
    typedef line_tag facet_tag;
      
    enum{ dim = 2 };
    static std::string name() { return "PLC"; }
  };
  
  
  
  
  
  
  
  namespace element_topology
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
    
//     template <>
//     struct boundary_cells<plc_tag, polygon_tag>
//     {
//       typedef dynamic_unique_layout_tag     layout_tag;
//       enum{ num = -1 };
//     };


    
    
//     template<typename bnd_cell_type>
//     struct bndcell_generator<plc_tag, polygon_tag, bnd_cell_type>
//     {
//         template<typename element_type, typename inserter_type>
//         static void create_bnd_cells(element_type & element, inserter_type & inserter)
//         {}
//     };
    
    template<typename bnd_cell_type>
    struct bndcell_generator<plc_tag, line_tag, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & plc, inserter_type & inserter)
        {
//             typedef typename result_of::element<element_type, polygon_tag>::type PolygonType;
//             typedef typename result_of::element_range<element_type, polygon_tag>::type PolygonRange;
//             typedef typename result_of::iterator<PolygonRange>::type PolygonIterator;
//             
//             PolygonRange polygons = elements<polygon_tag>(plc);
//             for (PolygonIterator pit = polygons.begin(); pit != polygons.end(); ++pit)
//             {
//                 typename tagging::result_of::element_tag<element_type, bounding_tag>::type plc_bounding_tag = tagging::create_element_tag<bounding_tag>(plc);
//                 
//                 typedef typename result_of::element_range<PolygonType, line_tag>::type LineOnPolygonRange;
//                 typedef typename result_of::handle_iterator<LineOnPolygonRange>::type LineOnPolygonIterator;
//                 
//                 LineOnPolygonRange lines = elements<line_tag>(*pit);
//                 for (LineOnPolygonIterator lphit = lines.begin(); lphit != lines.end(); ++lphit)
//                 {
//                     plc.container( line_tag() ).insert_handle( *lphit );
//                     tagging::tag(viennagrid::dereference_handle(plc,*lphit), plc_bounding_tag);
//                 }
//             }
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
            for (LineIterator lit = lines.handle_begin(); lit != lines.handle_end(); ++lit)
            {
                typedef typename result_of::element_range<LineType, vertex_tag>::type VertexOnLineRange;
                typedef typename result_of::handle_iterator<VertexOnLineRange>::type VertexOnLineHandleIterator;
                
                VertexOnLineRange vertices = elements<vertex_tag>(*lit);
                for (VertexOnLineHandleIterator vlhit = vertices.begin(); vlhit != vertices.end(); ++vlhit)
                    plc.container( vertex_tag() ).insert_handle( *vlhit );
            }
            
            
            
//             typename tagging::result_of::element_tag<element_type, bounding_tag>::type plc_bounding_tag = tagging::create_element_tag<bounding_tag>(plc);
//             
//             typedef typename result_of::element<element_type, polygon_tag>::type PolygonType;
//             typedef typename result_of::element_range<element_type, polygon_tag>::type PolygonRange;
//             typedef typename result_of::iterator<PolygonRange>::type PolygonIterator;
//             
//             PolygonRange polygons = elements<polygon_tag>(plc);
//             for (PolygonIterator pit = polygons.handle_begin(); pit != polygons.handle_end(); ++pit)
//             {
//                 typedef typename result_of::element_range<PolygonType, vertex_tag>::type VertexOnPolygonRange;
//                 typedef typename result_of::handle_iterator<VertexOnPolygonRange>::type VertexOnPolygonIterator;
//                 
//                 VertexOnPolygonRange vertices = elements<vertex_tag>(*pit);
//                 for (VertexOnPolygonIterator vphit = vertices.begin(); vphit != vertices.end(); ++vphit)
//                 {
//                     plc.container( vertex_tag() ).insert_handle( *vphit );
//                     tagging::tag(viennagrid::dereference_handle(plc,*vphit), plc_bounding_tag);
//                 }
//             }
        }
    };


    

  } //topology
}

#endif

