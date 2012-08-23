#ifndef VIENNAGRID_TOPOLOGY_LINE_HPP
#define VIENNAGRID_TOPOLOGY_LINE_HPP

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

#include "viennagrid/forwards.h"
#include "viennagrid/topology/point.hpp"

/** @file line.hpp
    @brief Provides the topological definition of a line
*/

namespace viennagrid
{
  /** @brief Topological description of a 1-cell (line). Same as hypercube_tag<1>. */
  template <>
  struct simplex_tag<1>
  {
    enum { dim = 1 };
    static std::string name() { return "Line"; }
  };


  namespace topology
  {

    //Line:
    /** @brief Topological description of the 0-cells of a line */
    template <>
    struct bndcells<simplex_tag<1>, 0>
    {
      typedef point_tag             tag;

      typedef static_layout_tag     layout_tag;
      enum{ num = 2 };     //2 vertices
    };

  }  
  

  //for compatibility: allow hypercube<1> to be used as line_tag as well:
  /** @brief Topological description of a 1-cell (line). Same as simplex_tag<1>. */
  template <>
  struct hypercube_tag<1>
  {
    enum { dim = 1 };
    static std::string name() { return "Line"; }
  };


  namespace topology
  {

    //Line:
    /** @brief Topological description of the 0-cells of a line */
    template <>
    struct bndcells<hypercube_tag<1>, 0>
    {
      typedef point_tag             tag;

      enum{ num = 2 };     //2 vertices
    };

  }  

}

#endif

