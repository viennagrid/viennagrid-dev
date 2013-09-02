#ifndef VIENNAGRID_DOMAIN_DEFAULT_CONFIGS_HPP
#define VIENNAGRID_DOMAIN_DEFAULT_CONFIGS_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/topology/quadrilateral.hpp"
#include "viennagrid/topology/polygon.hpp"
#include "viennagrid/topology/hexahedron.hpp"
#include "viennagrid/topology/plc.hpp"

/** @file config/default_configs.hpp
    @brief Provides default configuration classes for domains
*/

namespace viennagrid
{
  ////////////////// topologically 1d domains ///////////////////

  /** @brief Provides a number of predefined configuration classes, which cover the most frequent use cases. */
  namespace config
  {
    /** @brief Point type with cartesian coordinate system in 1d */
    typedef viennagrid::point_t<double, viennagrid::cartesian_cs<1> > point_type_1d;
    /** @brief Point type with cartesian coordinate system in 2d */
    typedef viennagrid::point_t<double, viennagrid::cartesian_cs<2> > point_type_2d;
    /** @brief Point type with cartesian coordinate system in 3d */
    typedef viennagrid::point_t<double, viennagrid::cartesian_cs<3> > point_type_3d;
  }

  
  namespace config
  {
    /** @brief A default config for vertices in 1d, pointer handles are used */
    struct vertex_1d
    {
      typedef result_of::full_domain_config< viennagrid::vertex_tag, point_type_1d, viennagrid::storage::pointer_handle_tag >::type type;
    };
    
    /** @brief A default config for vertices in 2d, pointer handles are used */
    struct vertex_2d
    {
      typedef result_of::full_domain_config< viennagrid::vertex_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type type;
    };

    /** @brief A default config for vertices in 3d, pointer handles are used */
    struct vertex_3d
    {
      typedef result_of::full_domain_config< viennagrid::vertex_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type type;
    };    
  }
  
  
  /** @brief A default domain for vertices in 1d, default config is used */
  typedef viennagrid::domain_t< config::vertex_1d >                                       vertex_1d_domain;
  /** @brief A default segmentation for vertices in 1d, default config is used */
  typedef viennagrid::result_of::segmentation< vertex_1d_domain >::type                   vertex_1d_segmentation;
  /** @brief A default segment for vertices in 1d, default config is used */
  typedef viennagrid::result_of::segment< vertex_1d_segmentation >::type                  vertex_1d_segment;
  /** @brief A default domain view for vertices in 1d, default config is used */
  typedef viennagrid::result_of::domain_view< vertex_1d_domain >::type                    vertex_1d_view;
  /** @brief A default cell for vertices in 1d, default config is used */
  typedef viennagrid::result_of::cell<vertex_1d_domain>::type                             vertex_1d_cell;

  /** @brief A default domain for vertices in 2d, default config is used */
  typedef viennagrid::domain_t< config::vertex_2d >                                       vertex_2d_domain;
  /** @brief A default segmentation for vertices in 2d, default config is used */
  typedef viennagrid::result_of::segmentation< vertex_2d_domain >::type                   vertex_2d_segmentation;
  /** @brief A default segment for vertices in 2d, default config is used */
  typedef viennagrid::result_of::segment< vertex_2d_segmentation >::type                  vertex_2d_segment;
  /** @brief A default domain view for vertices in 2d, default config is used */
  typedef viennagrid::result_of::domain_view< vertex_2d_domain >::type                    vertex_2d_view;
  /** @brief A default cell for vertices in 2d, default config is used */
  typedef viennagrid::result_of::cell<vertex_2d_domain>::type                             vertex_2d_cell;
  
  /** @brief A default domain for vertices in 3d, default config is used */
  typedef viennagrid::domain_t< config::vertex_3d >                                       vertex_3d_domain;
  /** @brief A default segmentation for vertices in 3d, default config is used */
  typedef viennagrid::result_of::segmentation< vertex_3d_domain >::type                   vertex_3d_segmentation;
  /** @brief A default segment for vertices in 3d, default config is used */
  typedef viennagrid::result_of::segment< vertex_3d_segmentation >::type                  vertex_3d_segment;
  /** @brief A default domain view for vertices in 3d, default config is used */
  typedef viennagrid::result_of::domain_view< vertex_3d_domain >::type                    vertex_3d_view;
  /** @brief A default cell for vertices in 3d, default config is used */
  typedef viennagrid::result_of::cell<vertex_3d_domain>::type                             vertex_3d_cell;
  
  
  namespace config
  {
    // topologically 1d domains (cell type is line)

    /** @brief A default config for lines in 1d, pointer handles are used */
    struct line_1d
    {
      typedef result_of::full_domain_config< viennagrid::line_tag, point_type_1d, viennagrid::storage::pointer_handle_tag >::type type;
    };

    /** @brief A default config for lines in 2d, pointer handles are used */
    struct line_2d
    {
      typedef result_of::full_domain_config< viennagrid::line_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type type;
    };

    /** @brief A default config for lines in 3d, pointer handles are used */
    struct line_3d
    {
      typedef result_of::full_domain_config< viennagrid::line_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type type;
    };
  }
  
  /** @brief A default domain for lines in 1d, default config is used */
  typedef viennagrid::domain_t< config::line_1d >                                       line_1d_domain;
  /** @brief A default segmentation for lines in 1d, default config is used */
  typedef viennagrid::result_of::segmentation< line_1d_domain >::type                   line_1d_segmentation;
  /** @brief A default segment for lines in 1d, default config is used */
  typedef viennagrid::result_of::segment< line_1d_segmentation >::type                  line_1d_segment;
  /** @brief A default domain view for lines in 1d, default config is used */
  typedef viennagrid::result_of::domain_view< line_1d_domain >::type                    line_1d_view;
  /** @brief A default cell for lines in 1d, default config is used */
  typedef viennagrid::result_of::cell<line_1d_domain>::type                             line_1d_cell;
  
  /** @brief A default domain for lines in 2d, default config is used */
  typedef viennagrid::domain_t< config::line_2d >                                       line_2d_domain;
  /** @brief A default segmentation for lines in 2d, default config is used */
  typedef viennagrid::result_of::segmentation< line_2d_domain >::type                   line_2d_segmentation;
  /** @brief A default segment for lines in 2d, default config is used */
  typedef viennagrid::result_of::segment< line_2d_segmentation >::type                  line_2d_segment;
  /** @brief A default domain view for lines in 2d, default config is used */
  typedef viennagrid::result_of::domain_view< line_2d_domain >::type                    line_2d_view;
  /** @brief A default cell for lines in 2d, default config is used */
  typedef viennagrid::result_of::cell<line_2d_domain>::type                             line_2d_cell;
  
  /** @brief A default domain for lines in 3d, default config is used */
  typedef viennagrid::domain_t< config::line_3d >                                       line_3d_domain;
  /** @brief A default segmentation for lines in 3d, default config is used */
  typedef viennagrid::result_of::segmentation< line_3d_domain >::type                   line_3d_segmentation;
  /** @brief A default segment for lines in 3d, default config is used */
  typedef viennagrid::result_of::segment< line_3d_segmentation >::type                  line_3d_segment;
  /** @brief A default domain view for lines in 3d, default config is used */
  typedef viennagrid::result_of::domain_view< line_3d_domain >::type                    line_3d_view;
  /** @brief A default cell for lines in 3d, default config is used */
  typedef viennagrid::result_of::cell<line_3d_domain>::type                             line_3d_cell;


  ////////////////// topologically 2d domains ///////////////////

  namespace config
  {

    /** @brief A default config for triangles in 2d, pointer handles are used */
    struct triangular_2d
    {
      typedef result_of::full_domain_config< viennagrid::triangle_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    /** @brief A default config for triangles in 3d, pointer handles are used */
    struct triangular_3d
    {
      typedef result_of::full_domain_config< viennagrid::triangle_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    /** @brief A default config for quadrilaterals in 2d, pointer handles are used */
    struct quadrilateral_2d
    {
      typedef result_of::full_domain_config< viennagrid::quadrilateral_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    /** @brief A default config for quadrilaterals in 3d, pointer handles are used */
    struct quadrilateral_3d
    {
      typedef result_of::full_domain_config< viennagrid::quadrilateral_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    /** @brief A default config for polygons in 2d, pointer handles are used */
    struct polygonal_2d
    {
      typedef result_of::full_domain_config< viennagrid::polygon_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    /** @brief A default config for polygons in 3d, pointer handles are used */
    struct polygonal_3d
    {
      typedef result_of::full_domain_config< viennagrid::polygon_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    /** @brief A default config for PLCs in 2d, pointer handles are used */
    struct plc_2d
    {
      typedef result_of::full_domain_config< viennagrid::plc_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    /** @brief A default config for PLCs in 3d, pointer handles are used */
    struct plc_3d
    {
      typedef result_of::full_domain_config< viennagrid::plc_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type     type;
    };
  }
  
  
  /** @brief A default domain for triangles in 2d, default config is used */
  typedef viennagrid::domain_t< config::triangular_2d >                                       triangular_2d_domain;
  /** @brief A default segmentation for triangles in 2d, default config is used */
  typedef viennagrid::result_of::segmentation< triangular_2d_domain >::type                   triangular_2d_segmentation;
  /** @brief A default segment for triangles in 2d, default config is used */
  typedef viennagrid::result_of::segment< triangular_2d_segmentation >::type                  triangular_2d_segment;
  /** @brief A default domain view for triangles in 2d, default config is used */
  typedef viennagrid::result_of::domain_view< triangular_2d_domain >::type                    triangular_2d_view;
  /** @brief A default cell for triangles in 2d, default config is used */
  typedef viennagrid::result_of::cell<triangular_2d_domain>::type                             triangular_2d_cell;
  
  /** @brief A default domain for triangles in 3d, default config is used */
  typedef viennagrid::domain_t< config::triangular_3d >                                       triangular_3d_domain;
  /** @brief A default segmentation for triangles in 3d, default config is used */
  typedef viennagrid::result_of::segmentation< triangular_3d_domain >::type                   triangular_3d_segmentation;
  /** @brief A default segment for triangles in 3d, default config is used */
  typedef viennagrid::result_of::segment< triangular_3d_segmentation >::type                  triangular_3d_segment;
  /** @brief An oriented hull segmentation for triangles in 3d, default config is used */
  typedef viennagrid::result_of::oriented_3d_hull_segmentation< triangular_3d_domain >::type      triangular_hull_3d_segmentation;
  /** @brief An oriented hull segmentat for triangles in 3d, default config is used */
  typedef viennagrid::result_of::segment< triangular_hull_3d_segmentation >::type                 triangular_hull_3d_segment;
  /** @brief A default domain view for triangles in 3d, default config is used */
  typedef viennagrid::result_of::domain_view< triangular_3d_domain >::type                    triangular_3d_view;
  /** @brief A default cell for triangles in 3d, default config is used */
  typedef viennagrid::result_of::cell<triangular_3d_domain>::type                             triangular_3d_cell;

  
  /** @brief A default domain for quadrilaterals in 2d, default config is used */
  typedef viennagrid::domain_t< config::quadrilateral_2d >                                       quadrilateral_2d_domain;
  /** @brief A default segmentation for quadrilaterals in 2d, default config is used */
  typedef viennagrid::result_of::segmentation< quadrilateral_2d_domain >::type                   quadrilateral_2d_segmentation;
  /** @brief A default segment for quadrilaterals in 2d, default config is used */
  typedef viennagrid::result_of::segment< quadrilateral_2d_segmentation >::type                  quadrilateral_2d_segment;
  /** @brief A default domain view for quadrilaterals in 2d, default config is used */
  typedef viennagrid::result_of::domain_view< quadrilateral_2d_domain >::type                    quadrilateral_2d_view;
  /** @brief A default cell for quadrilaterals in 2d, default config is used */
  typedef viennagrid::result_of::cell<quadrilateral_2d_domain>::type                             quadrilateral_2d_cell;
  
  /** @brief A default domain for quadrilaterals in 3d, default config is used */
  typedef viennagrid::domain_t< config::quadrilateral_3d >                                       quadrilateral_3d_domain;
  /** @brief A default segmentation for quadrilaterals in 3d, default config is used */
  typedef viennagrid::result_of::segmentation< quadrilateral_3d_domain >::type                   quadrilateral_3d_segmentation;
  /** @brief A default segment for quadrilaterals in 3d, default config is used */
  typedef viennagrid::result_of::segment< quadrilateral_3d_segmentation >::type                  quadrilateral_3d_segment;
  /** @brief A default domain view for quadrilaterals in 3d, default config is used */
  typedef viennagrid::result_of::domain_view< quadrilateral_3d_domain >::type                    quadrilateral_3d_view;
  /** @brief A default cell for quadrilaterals in 3d, default config is used */
  typedef viennagrid::result_of::cell<quadrilateral_3d_domain>::type                             quadrilateral_3d_cell;
  
  
  /** @brief A default domain for polygons in 2d, default config is used */
  typedef viennagrid::domain_t< config::polygonal_2d >                                       polygonal_2d_domain;
  /** @brief A default segmentation for polygons in 2d, default config is used */
  typedef viennagrid::result_of::segmentation< polygonal_2d_domain >::type                   polygonal_2d_segmentation;
  /** @brief A default segment for polygons in 2d, default config is used */
  typedef viennagrid::result_of::segment< polygonal_2d_segmentation >::type                  polygonal_2d_segment;
  /** @brief A default domain view for polygons in 2d, default config is used */
  typedef viennagrid::result_of::domain_view< polygonal_2d_domain >::type                    polygonal_2d_view;
  /** @brief A default cell for polygons in 2d, default config is used */
  typedef viennagrid::result_of::cell<polygonal_2d_domain>::type                             polygonal_2d_cell;

  /** @brief A default domain for polygons in 3d, default config is used */
  typedef viennagrid::domain_t< config::polygonal_3d >                                       polygonal_3d_domain;
  /** @brief A default segmentation for polygons in 3d, default config is used */
  typedef viennagrid::result_of::segmentation< polygonal_3d_domain >::type                   polygonal_3d_segmentation;
  /** @brief A default segment for polygons in 3d, default config is used */
  typedef viennagrid::result_of::segment< polygonal_3d_segmentation >::type                  polygonal_3d_segment;
  /** @brief A default domain view for polygons in 3d, default config is used */
  typedef viennagrid::result_of::domain_view< polygonal_3d_domain >::type                    polygonal_3d_view;
  /** @brief A default cell for polygons in 3d, default config is used */
  typedef viennagrid::result_of::cell<polygonal_3d_domain>::type                             polygonal_3d_cell;
  
  /** @brief A default domain for PLCs in 2d, default config is used */
  typedef viennagrid::domain_t< config::plc_2d >                                       plc_2d_domain;
  /** @brief A default segmentation for PLCs in 2d, default config is used */
  typedef viennagrid::result_of::segmentation< plc_2d_domain >::type                   plc_2d_segmentation;
  /** @brief A default segment for PLCs in 2d, default config is used */
  typedef viennagrid::result_of::segment< plc_2d_segmentation >::type                  plc_2d_segment;
  /** @brief A default domain view for PLCs in 2d, default config is used */
  typedef viennagrid::result_of::domain_view< plc_2d_domain >::type                    plc_2d_view;
  /** @brief A default cell for PLCs in 2d, default config is used */
  typedef viennagrid::result_of::cell<plc_2d_domain>::type                             plc_2d_cell;

  /** @brief A default domain for PLCs in 3d, default config is used */
  typedef viennagrid::domain_t< config::plc_3d >                                       plc_3d_domain;
  /** @brief A default segmentation for PLCs in 3d, default config is used */
  typedef viennagrid::result_of::segmentation< plc_3d_domain >::type                   plc_3d_segmentation;
  /** @brief A default segment for PLCs in 3d, default config is used */
  typedef viennagrid::result_of::segment< plc_3d_segmentation >::type                  plc_3d_segment;
  /** @brief A default domain view for PLCs in 3d, default config is used */
  typedef viennagrid::result_of::domain_view< plc_3d_domain >::type                    plc_3d_view;
  /** @brief A default cell for PLCs in 3d, default config is used */
  typedef viennagrid::result_of::cell<plc_3d_domain>::type                             plc_3d_cell;



  ////////////////// topologically 3d domains ///////////////////

  namespace config
  {
    /** @brief A default config for tetrahedrons in 3d, pointer handles are used */
    struct tetrahedral_3d
    {
      typedef result_of::full_domain_config< viennagrid::tetrahedron_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type  type;
    };

    /** @brief A default config for hexahedrons in 3d, pointer handles are used */
    struct hexahedral_3d
    {
      typedef result_of::full_domain_config< viennagrid::hexahedron_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type   type;
    };
  }
  
  /** @brief A default domain for tetrahedrons in 3d, default config is used */
  typedef viennagrid::domain_t< config::tetrahedral_3d >                                       tetrahedral_3d_domain;
  /** @brief A default segmentation for tetrahedrons in 3d, default config is used */
  typedef viennagrid::result_of::segmentation< tetrahedral_3d_domain >::type                   tetrahedral_3d_segmentation;
  /** @brief A default segment for tetrahedrons in 3d, default config is used */
  typedef viennagrid::result_of::segment< tetrahedral_3d_segmentation >::type                  tetrahedral_3d_segment;
  /** @brief A default domain view for tetrahedrons in 3d, default config is used */
  typedef viennagrid::result_of::domain_view< tetrahedral_3d_domain >::type                    tetrahedral_3d_view;
  /** @brief A default cell for tetrahedrons in 3d, default config is used */
  typedef viennagrid::result_of::cell<tetrahedral_3d_domain>::type                             tetrahedral_3d_cell;
  
  
  /** @brief A default domain for hexahedrons in 3d, default config is used */
  typedef viennagrid::domain_t< config::hexahedral_3d >                                       hexahedral_3d_domain;
  /** @brief A default segmentation for hexahedrons in 3d, default config is used */
  typedef viennagrid::result_of::segmentation< hexahedral_3d_domain >::type                   hexahedral_3d_segmentation;
  /** @brief A default segment for hexahedrons in 3d, default config is used */
  typedef viennagrid::result_of::segment< hexahedral_3d_segmentation >::type                  hexahedral_3d_segment;
  /** @brief A default domain view for hexahedrons in 3d, default config is used */
  typedef viennagrid::result_of::domain_view< hexahedral_3d_domain >::type                    hexahedral_3d_view;
  /** @brief A default cell for hexahedrons in 3d, default config is used */
  typedef viennagrid::result_of::cell<hexahedral_3d_domain>::type                             hexahedral_3d_cell;

}


#endif
