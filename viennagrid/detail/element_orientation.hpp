/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_DETAIL_ELEMENT_ORIENTATION_HPP
#define VIENNAGRID_DETAIL_ELEMENT_ORIENTATION_HPP

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