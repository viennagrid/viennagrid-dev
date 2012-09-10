#ifndef VIENNAGRID_DOMAIN_ITERATORS_HPP
#define VIENNAGRID_DOMAIN_ITERATORS_HPP

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
#include <list>
#include <map>
#include <stack>
#include <assert.h>

#include "viennagrid/forwards.h"

/** @file domain_iterators.hpp
    @brief Provides the iterators and ranges for the domain
*/

namespace viennagrid
{

  //metafunction for return type:
  namespace result_of
  {
    /** @brief Specialization of the iterator metafunction for returning the correct iterator from a range */
    template <typename T, long dim, bool is_coboundary>
    struct iterator<viennagrid::ncell_range<T, dim, is_coboundary>, 0>
    {
      typedef typename viennagrid::ncell_range<T, dim, is_coboundary>::iterator     type;
    };
    
    /** @brief Specialization of the iterator metafunction for returning the correct iterator from a const range */
    template <typename T, long dim, bool is_coboundary>
    struct iterator<viennagrid::const_ncell_range<T, dim, is_coboundary>, 0>
    {
      typedef typename viennagrid::const_ncell_range<T, dim, is_coboundary>::iterator     type;
    };
    
    /** @brief Meta function for returning a range of n-cells.
     * 
     * @tparam T       The enclosing complex (domain, segment or a n-cell of higher dimension)
     * @tparam dim     The topological dimension of the n-cells in the range */
    template <typename T, 
              long dim>  //topological level
    struct ncell_range
    {
      typedef viennagrid::ncell_range<T, dim>       type;
    };

    /** @brief Meta function for returning a const-range of n-cells.
     * 
     * @tparam T       The enclosing complex (domain, segment or a n-cell of higher dimension)
     * @tparam dim     The topological dimension of the n-cells in the range */
    template <typename T, 
              long dim>  //topological level
    struct const_ncell_range
    {
      typedef viennagrid::const_ncell_range<T, dim>       type;
    };
    
  }
  
} //namespace
#endif

