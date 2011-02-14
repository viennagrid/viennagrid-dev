/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_FORWARDS_GUARD
#define VIENNAGRID_FORWARDS_GUARD

#include <iostream>
#include <vector>
#include <map>
#include "viennagrid/idhandler.hpp"

namespace viennagrid
{
  
  typedef unsigned long       dim_type;
  
  /********* Tags ***********************/

  //Dimension Tags:
  struct three_dimensions_tag{
      enum{ value = 3 };
  };
  
  struct two_dimensions_tag{
      enum{ value = 2 };
  };

  struct one_dimension_tag{
      enum{ value = 1 };
  };

  //Tags for the handling of elements at different topological levels (see subcell_traits)
  struct full_handling_tag {};
  struct no_handling_tag {};

  //multigrid:
  struct full_multigrid_tag {};
  struct no_multigrid_tag {};

  //segment connection:
  template <long id>
  struct SegmentConnectionKey
  {
    bool operator<(SegmentConnectionKey const & sck2) const
    {
      return false;     // a < b is false AND b < a is false, hence a == b
    }
  };

  
  /********* Forward definitions of main classes *******************/

  //forward declarations:
  template <typename NumericT,
            typename DimTag>
  class point;

  template <typename T_Configuration, typename ElementTag>
  struct element;

  template <typename ElementType>
  class element_key;

  template <dim_type level>
  struct segment_traits;

  //Segment type: 
  template <typename ConfigType>
  class segment_t;

  template <typename ConfigType>
  class domain;
  
  /********* Other *******************/

  
  struct less_tag {};
  struct equal_tag {};

  template <long i, long j, bool ilargerj = (i>j), bool iequalj = (i==j) >
  struct level_discriminator
  {  };

  template <long i, long j>
  struct level_discriminator <i, j, false, true>
  {
    typedef equal_tag   result_type;
  };

  template <long i, long j>
  struct level_discriminator <i, j, true, false>
  {
    typedef less_tag    result_type;
  };

  template <typename host_element,
            dim_type dim,
            bool is_coboundary = false>
  class ncell_container;

  template <typename host_element,
            dim_type dim,
            bool is_coboundary = false>
  class const_ncell_container;
  
  template <typename T, long dim> //topological dimension of the elements over which to iterate
  class coboundary_key
  {
    public:
      coboundary_key(T & t_) : t(&t_) {}
      
      //for compatibility with std::map
      bool operator<(coboundary_key const & other) const
      {
        return t < other.t;
      }
    private:
      T * t;
  };

  //proxy classes for iterator/container retrieval:
  template <typename T>
  class const_ncell_proxy
  {
    public:
      const_ncell_proxy(T const & t_) : t(t_) {}
      
      T const & get() const { return t; }
    
    private:
      T const & t;
  };
  
  template <typename T>
  class ncell_proxy
  {
    public:
      ncell_proxy(T & t_) : t(t_) {}
      
      T & get() const { return t; }
    
    private:
      T & t;
  };
  
  
  namespace result_of
  {
    template <typename T, dim_type dim = 0>
    struct iterator;
    
    template <typename T>
    struct config
    {
      typedef typename T::config_type     type; 
    };
    

    template <typename T,   //type of host (domain, segment, other element)
              dim_type dim,
              dim_type cell_level = config<T>::type::cell_tag::topology_level>
    struct element_container;
    
    template <typename T, 
              dim_type dim>  //topological level
    struct ncell_container;
    
    
    template <typename Config,
              dim_type dim,
              dim_type cell_level = Config::cell_tag::topology_level>
    struct ncell_type;
    
    template <typename Config>
    struct point_type
    {
      typedef point<typename Config::numeric_type, typename Config::dimension_tag>   type;
    };
    
    
  }

}
#endif
