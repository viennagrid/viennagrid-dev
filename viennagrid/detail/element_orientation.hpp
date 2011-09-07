#ifndef VIENNAGRID_DETAIL_ELEMENT_ORIENTATION_HPP
#define VIENNAGRID_DETAIL_ELEMENT_ORIENTATION_HPP

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


#include <vector>
#include <iostream>

#include "viennagrid/forwards.h"

namespace viennagrid
{
  
  
  /************** Level 1: Elements contained by a higher-level element *******/

  //local vertex numbering is in general different from global vertex numbering for lower-level topological elements
  //this permutator maps local numbers to global numbers
  //alternative implementation: vertexnum as template parameter
  template <long num_vertices>
  class element_orientation
  {
    public:
      void setDefaultOrientation()
      {
        for (dim_type i=0; i<static_cast<dim_type>(num_vertices); ++i)
          permutator_[i] = i;
      };

      dim_type operator()(dim_type in) const { return permutator_[in]; }

      void setPermutation(dim_type index, dim_type mappedTo) { permutator_[index] = mappedTo; };

      void print() const
      {
        for (dim_type i=0; i<static_cast<dim_type>(num_vertices); ++i)
        {
          std::cout << i << "->" << permutator_[i] << ",";
        }
        std::cout << std::endl;
      }

    private:
      dim_type permutator_[num_vertices];
  };

}


#endif

