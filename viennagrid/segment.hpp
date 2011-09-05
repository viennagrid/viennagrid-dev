#ifndef VIENNAGRID_SEGMENT_HPP
#define VIENNAGRID_SEGMENT_HPP

/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
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


#include <map>
#include <set>
#include <deque>

#include "viennagrid/forwards.h"
#include "viennagrid/detail/segment_layers.hpp"


namespace viennagrid
{
  
  
  //Segment type: set up using recursive inheritance, similar to domain_t and element_t
  template <typename Conf>
  class segment_t : public detail::segment_layers_top<Conf, Conf::cell_tag::topology_level>
  {
    typedef detail::segment_layers_top<Conf, Conf::cell_tag::topology_level>     base_type;
    
    public:
      typedef Conf                config_type;
      
      using base_type::push_back;

  };

  
  
  
}
#endif
