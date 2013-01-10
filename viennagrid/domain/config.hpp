#ifndef VIENNAGRID_DOMAIN_CONFIG_HPP
#define VIENNAGRID_DOMAIN_CONFIG_HPP

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


#include "viennagrid/domain/geometric_domain.hpp"
#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/topology/quadrilateral.hpp"
#include "viennagrid/topology/polygon.hpp"
#include "viennagrid/topology/hexahedron.hpp"

/** @file config/simplex.hpp
    @brief Provides default configuration classes for simplex domains
*/

namespace viennagrid
{
  /** @brief Provides a number of predefined configuration classes, which cover the most frequent use cases. */
  namespace config
  {
      typedef viennagrid::point_t<double, viennagrid::cartesian_cs<1> > point_type_1d;
      typedef viennagrid::point_t<double, viennagrid::cartesian_cs<2> > point_type_2d;
      typedef viennagrid::point_t<double, viennagrid::cartesian_cs<3> > point_type_3d;
      
      
      
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::line_tag, point_type_1d, viennagrid::storage::pointer_hook_tag >::type line_1d;
      typedef viennagrid::result_of::geometric_domain< line_1d >::type line_1d_domain;
      typedef viennagrid::result_of::geometric_view< line_1d_domain >::type line_1d_segment;
      typedef viennagrid::result_of::element<line_1d_domain, viennagrid::line_tag>::type line_1d_cell;
      
      
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::line_tag, point_type_2d, viennagrid::storage::pointer_hook_tag >::type line_2d;
      typedef viennagrid::result_of::geometric_domain< line_2d >::type line_2d_domain;
      typedef viennagrid::result_of::geometric_view< line_2d_domain >::type line_2d_segment;
      typedef viennagrid::result_of::element<line_2d_domain, viennagrid::line_tag>::type line_2d_cell;
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::triangle_tag, point_type_2d, viennagrid::storage::pointer_hook_tag >::type triangular_2d;
      typedef viennagrid::result_of::geometric_domain< triangular_2d >::type triangular_2d_domain;
      typedef viennagrid::result_of::geometric_view< triangular_2d_domain >::type triangular_2d_segment;
      typedef viennagrid::result_of::element<triangular_2d_domain, viennagrid::triangle_tag>::type triangular_2d_cell;
      


      
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::line_tag, point_type_3d, viennagrid::storage::pointer_hook_tag >::type line_3d;
      typedef viennagrid::result_of::geometric_domain< line_3d >::type line_3d_domain;
      typedef viennagrid::result_of::geometric_view< line_3d_domain >::type line_3d_segment;
      typedef viennagrid::result_of::element<line_3d_domain, viennagrid::line_tag>::type line_3d_cell;
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::triangle_tag, point_type_3d, viennagrid::storage::pointer_hook_tag >::type triangular_3d;
      typedef viennagrid::result_of::geometric_domain< triangular_3d >::type triangular_3d_domain;
      typedef viennagrid::result_of::geometric_view< triangular_3d_domain >::type triangular_3d_segment;
      typedef viennagrid::result_of::element<triangular_3d_domain, viennagrid::triangle_tag>::type triangular_3d_cell;
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::tetrahedron_tag, point_type_3d, viennagrid::storage::pointer_hook_tag >::type tetrahedral_3d;
      typedef viennagrid::result_of::geometric_domain< tetrahedral_3d >::type tetrahedral_3d_domain;
      typedef viennagrid::result_of::geometric_view< tetrahedral_3d_domain >::type tetrahedral_3d_segment;
      typedef viennagrid::result_of::element<tetrahedral_3d_domain, viennagrid::tetrahedron_tag>::type tetrahedral_3d_cell;
      
      
      
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::quadrilateral_tag, point_type_2d, viennagrid::storage::pointer_hook_tag >::type quadrilateral_2d;
      typedef viennagrid::result_of::geometric_domain< quadrilateral_2d >::type quadrilateral_2d_domain;
      typedef viennagrid::result_of::geometric_view< quadrilateral_2d_domain >::type quadrilateral_2d_segment;
      typedef viennagrid::result_of::element<quadrilateral_2d_domain, viennagrid::quadrilateral_tag>::type quadrilateral_2d_cell;


      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::quadrilateral_tag, point_type_3d, viennagrid::storage::pointer_hook_tag >::type quadrilateral_3d;
      typedef viennagrid::result_of::geometric_domain< quadrilateral_3d >::type quadrilateral_3d_domain;
      typedef viennagrid::result_of::geometric_view< quadrilateral_3d_domain >::type quadrilateral_3d_segment;
      typedef viennagrid::result_of::element<quadrilateral_3d_domain, viennagrid::quadrilateral_tag>::type quadrilateral_3d_cell;

      typedef viennagrid::result_of::geometric_domain_config< viennagrid::hexahedron_tag, point_type_3d, viennagrid::storage::pointer_hook_tag >::type hexahedral_3d;
      typedef viennagrid::result_of::geometric_domain< hexahedral_3d >::type hexahedral_3d_domain;
      typedef viennagrid::result_of::geometric_view< hexahedral_3d_domain >::type hexahedral_3d_segment;
      typedef viennagrid::result_of::element<hexahedral_3d_domain, viennagrid::hexahedron_tag>::type hexahedral_3d_cell;
      
      
      
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::polygon_tag, point_type_2d, viennagrid::storage::pointer_hook_tag >::type polygonal_2d;
      typedef viennagrid::result_of::geometric_domain< polygonal_2d >::type polygonal_2d_domain;
      typedef viennagrid::result_of::geometric_view< polygonal_2d_domain >::type polygonal_2d_segment;
      typedef viennagrid::result_of::element<polygonal_2d_domain, viennagrid::polygon_tag>::type polygonal_2d_cell;
      
      typedef viennagrid::result_of::geometric_domain_config< viennagrid::polygon_tag, point_type_3d, viennagrid::storage::pointer_hook_tag >::type polygonal_3d;
      typedef viennagrid::result_of::geometric_domain< polygonal_3d >::type polygonal_3d_domain;
      typedef viennagrid::result_of::geometric_view< polygonal_3d_domain >::type polygonal_3d_segment;
      typedef viennagrid::result_of::element<polygonal_3d_domain, viennagrid::polygon_tag>::type polygonal_3d_cell;
  }
}


#endif
