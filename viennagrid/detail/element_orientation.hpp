#ifndef VIENNAGRID_DETAIL_ELEMENT_ORIENTATION_HPP
#define VIENNAGRID_DETAIL_ELEMENT_ORIENTATION_HPP

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


#include <vector>
#include <iostream>
#include <boost/iterator/iterator_concepts.hpp>

#include "viennagrid/forwards.h"

/** @file element_orientation.hpp
    @brief Provides an orienter for boundary k-cells of a n-cell
*/

namespace viennagrid
{
  
    
    namespace result_of
    {
    
        template<long num_elements>
        struct permutator_type
        {
                typedef typename permutator_type<num_elements+1>::type type;
        };
        
        template<>
        struct permutator_type<256>
        {
                typedef unsigned char type;
        };
        
        template<>
        struct permutator_type<65536>
        {
                typedef unsigned short type;
        };
    }
    
    
  
  /************** Level 1: Elements contained by a higher-level element *******/

  /** @brief A permutator for mapping local orientations to global vertices of a boundary k-cell 
   * 
   * Local vertex numbering is in general different from global vertex numbering for lower-level topological elements
   * this permutator maps local numbers to global numbers
   * alternative implementation: vertexnum as template parameter
   */
  template <long num_vertices>
  class element_orientation
  {
      typedef typename result_of::permutator_type<num_vertices>::type permutator_type;
      
    public:
      void setDefaultOrientation()
      {
        for (dim_type i=0; i<static_cast<dim_type>(num_vertices); ++i)
          permutator_[i] = static_cast<unsigned char>(i);
      };

      dim_type operator()(dim_type in) const { return static_cast<dim_type>(permutator_[in]); }

      void setPermutation(dim_type index, dim_type mappedTo) { permutator_[index] = static_cast<unsigned char>(mappedTo); };

      void print() const
      {
        for (dim_type i=0; i<static_cast<dim_type>(num_vertices); ++i)
        {
          std::cout << i << "->" << permutator_[i] << ",";
        }
        std::cout << std::endl;
      }

    private:
      permutator_type permutator_[num_vertices];
  };

}


#endif

