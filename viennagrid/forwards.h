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

  template <typename SegmentType>
  class segment_mapping_key
  {
    public:
      segment_mapping_key(SegmentType const & seg) : pSeg(&seg) {}
      
      //for compatibility with std::map
      bool operator<(segment_mapping_key const & other) const
      {
        return pSeg < other.pSeg;
      }
      
    private:
      SegmentType const * pSeg;
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
  

  //ID handling:
  
  class NoID
  {
    //for compatibility:
    void setID(long id) { };
    const NoID * getID() const { return this; };

  };

  class ProvideID
  {
    public:
      ProvideID() : id_(-1) {};

      long getID() const { return id_; };
      void setID(long id) { id_ = id; };

    protected:
      long id_;
  };
  
  
  namespace traits
  {
    // cell_traits can be specialized by library users in order to use 'NoID' instead of 'ProvideID' 
    template <typename ElementTag>
    struct element_id
    {
      typedef ProvideID     id_handler;
    };
    
    template <typename ElementTag, 
              long level = ElementTag::topology_level>
    struct subcell_desc
    {
      //the default case is simultaneously a pathetic case:
      //cell-handling within the cell

      enum{ num_elements = 1 };     //1 cell

      typedef ElementTag            element_tag;
      //compatibility with cell-on-cell-iterator
      typedef full_handling_tag     handling_tag;
    };

    template <typename CellType, long level>
    struct subcell_filler {};
    
  }
  
  
  
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
    
    
    template <typename T, dim_type dim = T::element_tag::topology_level>
    struct element_tag
    {
      typedef typename viennagrid::traits::subcell_desc<typename T::element_tag,
                                                        dim>::element_tag           type; 
    };
    
    
    template <typename T>
    struct element_tag<T, T::element_tag::topology_level>
    {
      typedef typename T::element_tag    type; 
    };
    
    
    template <typename T, dim_type dim = T::element_tag::topology_level>
    struct handling_tag
    {
      typedef typename viennagrid::traits::subcell_desc<T, dim>::handling_tag    type; 
    };
    
    //cell level always uses full handling
    template <typename T>
    struct handling_tag<T, T::element_tag::topology_level>
    {
      typedef full_handling_tag    type; 
    };

    //vertex level always uses full handling
    template <typename T>
    struct handling_tag<T, 0>
    {
      typedef full_handling_tag    type; 
    };
    
  }
  
  

}
#endif
