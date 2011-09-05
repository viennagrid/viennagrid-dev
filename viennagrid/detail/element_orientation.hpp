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

namespace viennagrid
{
  
  
  /************** Level 1: Elements contained by a higher-level element *******/

  //local vertex numbering is in general different from global vertex numbering for lower-level topological elements
  //this permutator maps local numbers to global numbers
  //alternative implementation: vertexnum as template parameter
  template <size_t num_vertices>
  class element_orientation
  {
    public:
      void setDefaultOrientation()
      {
        for (size_t i=0; i<num_vertices; ++i)
          permutator_[i] = i;
      };

      short operator()(short in) const { return permutator_[in]; }

      void setPermutation(short index, short mappedTo) { permutator_[index] = mappedTo; };

      void print() const
      {
        for (size_t i=0; i<num_vertices; ++i)
        {
          std::cout << i << "->" << permutator_[i] << ",";
        }
        std::cout << std::endl;
      }

    private:
      short permutator_[num_vertices];
  };

}


#endif