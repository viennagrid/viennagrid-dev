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

#ifndef VIENNAGRID_FORWARDS_GUARD
#define VIENNAGRID_FORWARDS_GUARD

#include <iostream>
#include <vector>
#include <map>
#include <cstddef>     //for std::size_t
#include <cstdlib>     //for EXIT_SUCCESS and EXIT_FAILURE


//Debug levels:
//VIENNAGRID_DEBUG_ALL          Output every little piece of debug information
//VIENNAGRID_DEBUG_IO           Debug IO operations
//VIENNAGRID_DEBUG_REFINEMENT   Debug refinement algorithms
//VIENNAGRID_DEBUG_STATUS       Print status messages to std::cout (very little debug info)

#ifdef VIENNAGRID_DEBUG_ALL
  #define VIENNAGRID_DEBUG_IO
  #define VIENNAGRID_DEBUG_REFINEMENT
  #define VIENNAGRID_DEBUG_STATUS
#endif

namespace viennagrid
{
  
  typedef std::size_t       dim_type;
  
  /********* Tags ***********************/
  
  struct cartesian_cs;    //Cartesian coordinate system
  struct polar_cs;        //Polar coordinate system (r, phi)
  struct spherical_cs;    //Spherical coordinate system (r, theta, phi)

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

  //Tags for the handling of elements at different topological levels (see topology::subcell_desc)
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
  
  class line_tag;
  class hexahedron_tag;
  class point_tag;
  class quadrilateral_tag;
  class tetrahedron_tag;
  class triangle_tag;

  //forward declarations:
  /*template <typename NumericT,
            typename DimTag>
  class point; */

  template <typename CoordType, dim_type d, typename CoordinateSystem = cartesian_cs>
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
  class ncell_range;

  template <typename host_element,
            dim_type dim,
            bool is_coboundary = false>
  class const_ncell_range;
  
  template <typename T, long dim> //topological dimension of the elements over which to iterate
  class coboundary_key
  {
    public:
      coboundary_key(T const & t_) : t(&t_) {}
      
      //for compatibility with std::map
      bool operator<(coboundary_key const & other) const
      {
        return t < other.t;
      }
    private:
      T const * t;
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
  
  //template <dim_type dim, typename Config, typename ElementTag>
  //ncell_proxy< element<Config, ElementTag> >
  //ncells(element<Config, ElementTag> & d);

  //ID handling:
  
  class pointer_id
  {
    //for compatibility:
    void id(long id) { };
    const pointer_id * id() const { return this; };

  };

  class integral_id
  {
    public:
      integral_id() : id_(-1) {};

      long id() const { return id_; };
      void id(long new_id) { id_ = new_id; };

    protected:
      long id_;
  };
  
  
  namespace traits
  {
    // cell_traits can be specialized by library users in order to use 'NoID' instead of 'ProvideID' 
    template <typename ElementTag>
    struct element_id
    {
      typedef integral_id     id_handler;
    };
  }
    
  namespace topology
  {
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
    struct ncell_range;
    
    
    template <typename Config,
              dim_type dim,
              dim_type cell_level = Config::cell_tag::topology_level>
    struct ncell_type;
    
    template <typename Config>
    struct point_type
    {
      //typedef point<typename Config::numeric_type, typename Config::dimension_tag>   type;
      typedef point<typename Config::numeric_type, Config::dimension_tag::value>   type;
    };
    
    
    template <typename T, dim_type dim = T::element_tag::topology_level>
    struct element_tag
    {
      typedef typename viennagrid::topology::subcell_desc<typename T::element_tag,
                                                        dim>::element_tag           type; 
    };
    
    
 //   template <typename T>
 //   struct element_tag<T, T::element_tag::topology_level>
 //   {
 //     typedef typename T::element_tag    type; 
 //   };
    
    
    template <typename T, dim_type dim = T::element_tag::topology_level>
    struct handling_tag
    {
      typedef typename viennagrid::topology::subcell_desc<T, dim>::handling_tag    type; 
    };
    
//    //cell level always uses full handling
//    template <typename T>
//    struct handling_tag<T, T::element_tag::topology_level>
//    {
//      typedef full_handling_tag    type; 
//    };

    //vertex level always uses full handling
    template <typename T>
    struct handling_tag<T, 0>
    {
      typedef full_handling_tag    type; 
    };
    
  }
  
   // norm tags for: algorithm/norm.hpp
   struct one_tag {};
   struct two_tag {};
   struct inf_tag {};
   
   // norm tags/types for: algorithm/cell_normals.hpp
   struct seg_cell_normal_tag {};
   struct seg_cell_normal_data {
      typedef viennagrid::point<double, 3>         point_type;
      typedef std::map<std::size_t, point_type>    type;
   };
   
   
  // 
  //refinement
  //
   
  template <typename T>
  struct element_refinement;
   
  struct refinement_key
  {
    bool operator<(refinement_key const & other) const
    {
      return false;     // a < b is false AND b < a is false, hence a == b
    }
  };
   
  template <typename DomainType, typename RefinementTag>
  class refinement_proxy;
   
  struct uniform_refinement_tag {};
  struct adaptive_refinement_tag {};
  
  
}
#endif
