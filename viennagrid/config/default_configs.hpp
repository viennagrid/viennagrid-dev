#ifndef VIENNAGRID_DOMAIN_DEFAULT_CONFIGS_HPP
#define VIENNAGRID_DOMAIN_DEFAULT_CONFIGS_HPP

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


#include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/topology/quadrilateral.hpp"
#include "viennagrid/topology/polygon.hpp"
#include "viennagrid/topology/hexahedron.hpp"
#include "viennagrid/topology/plc.hpp"

#include "viennagrid/config/domain_config.hpp"

/** @file config/default_configs.hpp
    @brief Provides default configuration classes for simplex domains
*/

namespace viennagrid
{
  ////////////////// topologically 1d domains ///////////////////

  /** @brief Provides a number of predefined configuration classes, which cover the most frequent use cases. */
  namespace config
  {
      typedef viennagrid::point_t<double, viennagrid::cartesian_cs<1> > point_type_1d;
      typedef viennagrid::point_t<double, viennagrid::cartesian_cs<2> > point_type_2d;
      typedef viennagrid::point_t<double, viennagrid::cartesian_cs<3> > point_type_3d;

      // topologically 1d domains (cell type is line)

      struct line_1d
      {
        typedef result_of::full_domain_config< viennagrid::line_tag, point_type_1d, viennagrid::storage::pointer_handle_tag >::type type;
      };

      struct line_2d
      {
        typedef result_of::full_domain_config< viennagrid::line_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type type;
      };

      struct line_3d
      {
        typedef result_of::full_domain_config< viennagrid::line_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type type;
      };

  }

  typedef viennagrid::domain_t< config::line_1d >                                         line_1d_domain;
  typedef viennagrid::result_of::segmentation< line_1d_domain >::type                     line_1d_segmentation;
  typedef viennagrid::result_of::segment< line_1d_segmentation >::type                    line_1d_segment;
  typedef viennagrid::result_of::domain_view< line_1d_domain >::type                      line_1d_view;
  typedef viennagrid::result_of::element<line_1d_domain, viennagrid::line_tag>::type      line_1d_cell;

  typedef viennagrid::domain_t< config::line_2d >                                         line_2d_domain;
  typedef viennagrid::result_of::segmentation< line_2d_domain >::type                     line_2d_segmentation;
  typedef viennagrid::result_of::segment< line_2d_segmentation >::type                    line_2d_segment;
  typedef viennagrid::result_of::domain_view< line_2d_domain >::type                      line_2d_view;
  typedef viennagrid::result_of::element<line_2d_domain, viennagrid::line_tag>::type      line_2d_cell;

  typedef viennagrid::domain_t< config::line_3d >                                         line_3d_domain;
  typedef viennagrid::result_of::segmentation< line_3d_domain >::type                     line_3d_segmentation;
  typedef viennagrid::result_of::segment< line_3d_segmentation >::type                    line_3d_segment;
  typedef viennagrid::result_of::domain_view< line_3d_domain >::type                      line_3d_view;
  typedef viennagrid::result_of::element<line_3d_domain, viennagrid::line_tag>::type      line_3d_cell;


  ////////////////// topologically 2d domains ///////////////////

  namespace config
  {

    struct triangular_2d
    {
      typedef result_of::full_domain_config< viennagrid::triangle_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    struct triangular_3d
    {
      typedef result_of::full_domain_config< viennagrid::triangle_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    struct quadrilateral_2d
    {
      typedef result_of::full_domain_config< viennagrid::quadrilateral_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    struct quadrilateral_3d
    {
      typedef result_of::full_domain_config< viennagrid::quadrilateral_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    struct polygonal_2d
    {
      typedef result_of::full_domain_config< viennagrid::polygon_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    struct polygonal_3d
    {
      typedef result_of::full_domain_config< viennagrid::polygon_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    struct plc_2d
    {
      typedef result_of::full_domain_config< viennagrid::plc_tag, point_type_2d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

    struct plc_3d
    {
      typedef result_of::full_domain_config< viennagrid::plc_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type     type;
    };

  }

  typedef viennagrid::domain_t< config::triangular_2d >                                           triangular_2d_domain;
  typedef viennagrid::result_of::segmentation< triangular_2d_domain >::type                       triangular_2d_segmentation;
  typedef viennagrid::result_of::segment< triangular_2d_segmentation >::type                      triangular_2d_segment;
  typedef viennagrid::result_of::domain_view< triangular_2d_domain >::type                        triangular_2d_view;
  typedef viennagrid::result_of::element<triangular_2d_domain, viennagrid::triangle_tag>::type    triangular_2d_cell;

  typedef viennagrid::domain_t< config::triangular_3d >                                           triangular_3d_domain;
  typedef viennagrid::result_of::segmentation< triangular_3d_domain >::type                       triangular_3d_segmentation;
  typedef viennagrid::result_of::segment< triangular_3d_segmentation >::type                      triangular_3d_segment;
  typedef viennagrid::result_of::oriented_3d_hull_segmentation< triangular_3d_domain >::type      triangular_hull_3d_segmentation;
  typedef viennagrid::result_of::segment< triangular_hull_3d_segmentation >::type                 triangular_hull_3d_segment;
  typedef viennagrid::result_of::domain_view< triangular_3d_domain >::type                        triangular_3d_view;
  typedef viennagrid::result_of::element<triangular_3d_domain, viennagrid::triangle_tag>::type    triangular_3d_cell;

  typedef viennagrid::domain_t< config::quadrilateral_2d >                                              quadrilateral_2d_domain;
  typedef viennagrid::result_of::segmentation< quadrilateral_2d_domain >::type                          quadrilateral_2d_segmentation;
  typedef viennagrid::result_of::segment< quadrilateral_2d_segmentation >::type                         quadrilateral_2d_segment;
  typedef viennagrid::result_of::domain_view< quadrilateral_2d_domain >::type                           quadrilateral_2d_view;
  typedef viennagrid::result_of::element<quadrilateral_2d_domain, viennagrid::quadrilateral_tag>::type  quadrilateral_2d_cell;

  typedef viennagrid::domain_t< config::quadrilateral_3d >                                              quadrilateral_3d_domain;
  typedef viennagrid::result_of::segmentation< quadrilateral_3d_domain >::type                          quadrilateral_3d_segmentation;
  typedef viennagrid::result_of::segment< quadrilateral_3d_segmentation >::type                         quadrilateral_3d_segment;
  typedef viennagrid::result_of::domain_view< quadrilateral_3d_domain >::type                           quadrilateral_3d_view;
  typedef viennagrid::result_of::element<quadrilateral_3d_domain, viennagrid::quadrilateral_tag>::type  quadrilateral_3d_cell;

  typedef viennagrid::domain_t< config::polygonal_2d >                                            polygonal_2d_domain;
  typedef viennagrid::result_of::segmentation< polygonal_2d_domain >::type                        polygonal_2d_segmentation;
  typedef viennagrid::result_of::segment< polygonal_2d_segmentation >::type                       polygonal_2d_segment;
  typedef viennagrid::result_of::domain_view< polygonal_2d_domain >::type                         polygonal_2d_view;
  typedef viennagrid::result_of::element<polygonal_2d_domain, viennagrid::polygon_tag>::type      polygonal_2d_cell;

  typedef viennagrid::domain_t< config::polygonal_3d >                                            polygonal_3d_domain;
  typedef viennagrid::result_of::segmentation< polygonal_3d_domain >::type                        polygonal_3d_segmentation;
  typedef viennagrid::result_of::segment< polygonal_3d_segmentation >::type                       polygonal_3d_segment;
  typedef viennagrid::result_of::domain_view< polygonal_3d_domain >::type                         polygonal_3d_view;
  typedef viennagrid::result_of::element<polygonal_3d_domain, viennagrid::polygon_tag>::type      polygonal_3d_cell;

  typedef viennagrid::domain_t< config::plc_2d >                                                  plc_2d_domain;
  typedef viennagrid::result_of::segmentation< plc_2d_domain >::type                              plc_2d_segmentation;
  typedef viennagrid::result_of::segment< plc_2d_segmentation >::type                             plc_2d_segment;
  typedef viennagrid::result_of::domain_view< plc_2d_domain >::type                               plc_2d_view;
  typedef viennagrid::result_of::element<plc_2d_domain, viennagrid::plc_tag>::type                plc_2d_cell;

  typedef viennagrid::domain_t< config::plc_3d >                                                  plc_3d_domain;
  typedef viennagrid::result_of::segmentation< plc_3d_domain >::type                              plc_3d_segmentation;
  typedef viennagrid::result_of::segment< plc_3d_segmentation >::type                             plc_3d_segment;
  typedef viennagrid::result_of::domain_view< plc_3d_domain >::type                               plc_3d_view;
  typedef viennagrid::result_of::element<plc_3d_domain, viennagrid::plc_tag>::type                plc_3d_cell;


  ////////////////// topologically 3d domains ///////////////////

  namespace config
  {
    struct tetrahedral_3d
    {
      typedef result_of::full_domain_config< viennagrid::tetrahedron_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type  type;
    };

    struct hexahedral_3d
    {
      typedef result_of::full_domain_config< viennagrid::hexahedron_tag, point_type_3d, viennagrid::storage::pointer_handle_tag >::type   type;
    };
  }

  typedef viennagrid::domain_t< config::tetrahedral_3d >                                             tetrahedral_3d_domain;
  typedef viennagrid::result_of::segmentation< tetrahedral_3d_domain >::type                         tetrahedral_3d_segmentation;
  typedef viennagrid::result_of::segment< tetrahedral_3d_segmentation >::type                        tetrahedral_3d_segment;
  typedef viennagrid::result_of::domain_view< tetrahedral_3d_domain >::type                          tetrahedral_3d_view;
  typedef viennagrid::result_of::element<tetrahedral_3d_domain, viennagrid::tetrahedron_tag>::type   tetrahedral_3d_cell;

  typedef viennagrid::domain_t< config::hexahedral_3d >                                              hexahedral_3d_domain;
  typedef viennagrid::result_of::segmentation< hexahedral_3d_domain >::type                          hexahedral_3d_segmentation;
  typedef viennagrid::result_of::segment< hexahedral_3d_segmentation >::type                         hexahedral_3d_segment;
  typedef viennagrid::result_of::domain_view< hexahedral_3d_domain >::type                           hexahedral_3d_view;
  typedef viennagrid::result_of::element<hexahedral_3d_domain, viennagrid::hexahedron_tag>::type     hexahedral_3d_cell;

}


#endif
