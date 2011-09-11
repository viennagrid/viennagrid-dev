#ifndef VIENNAGRID_FORWARDS_H
#define VIENNAGRID_FORWARDS_H

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

#include <iostream>
#include <vector>
#include <map>
#include <cstddef>     //for std::size_t
#include <cstdlib>     //for EXIT_SUCCESS and EXIT_FAILURE

#include "viennadata/api.hpp"

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
  
  template <long d>
  struct cartesian_cs;    //Cartesian coordinate system
  
  struct polar_cs;        //Polar coordinate system (r, phi)
  struct spherical_cs;    //Spherical coordinate system (r, theta, phi)
  struct cylindrical_cs;  //Cylindrical coordinate system (rho, theta, z)

  //Dimension Tag: (for tag dispatching)
  template <long d>
  struct dimension_tag
  {
      enum{ value = d };
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
  
  struct point_tag;

  // Simplex family:
  template <long dim>
  struct simplex_tag;
  
  typedef simplex_tag<1>    line_tag;
  typedef simplex_tag<2>    triangle_tag;
  typedef simplex_tag<3>    tetrahedron_tag;
  
  // Hypercube family:
  template <long dim>
  struct hypercube_tag;

  //typedef hypercube_tag<1>    line_tag;
  typedef hypercube_tag<2>  quadrilateral_tag;
  typedef hypercube_tag<3>  hexahedron_tag;


  //forward declarations:
  template <typename CoordType, typename CoordinateSystem>
  class point_t;
  
  template <typename ConfigType, typename ElementTag>
  class element_t;

  template <typename ConfigType, typename ElementType>
  class element_key;

  //Segment type: 
  template <typename ConfigType>
  class segment_t;

  template <typename ConfigType>
  class domain_t;
  
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
            long dim,
            bool is_coboundary = false>
  class ncell_range;

  template <typename host_element,
            long dim,
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

  template <typename T>
  class boundary_key;

  class interface_key;
  
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
  
  //template <long dim, typename Config, typename ElementTag>
  //ncell_proxy< element<Config, ElementTag> >
  //ncells(element<Config, ElementTag> & d);

  //ID handling:
  class pointer_id
  {
    public:
      typedef pointer_id *  id_type;
      
      //for compatibility:
      void id(const pointer_id *) { };
      void id(long) { };
      const pointer_id * id() const { return this; };
      pointer_id * id() { return this; };
  };

  class integral_id
  {
    public:
      typedef long   id_type;
      
      integral_id() : id_(-1) {};

      long id() const { return id_; };
      void id(long new_id) { id_ = new_id; };

    protected:
      long id_;
  };
  
  namespace topology
  {
    template <typename ElementTag, 
              long level = ElementTag::dim>
    struct bndcells
    {
      //the default case is simultaneously a pathetic case:
      //cell-handling within the cell

      enum{ num = 1 };     //1 cell

      typedef ElementTag            tag;
    };

    template <typename CellType, long level>
    struct bndcell_filler {};
    
  }
  
  
  
  namespace result_of
  {
    template <typename ConfigType, typename ElementTag>    //by default, every element is equipped with an ID
    struct element_id_handler
    {
      typedef integral_id     type;
    };

    template <typename T, long dim = 0>
    struct iterator;
    
    template <typename T>
    struct config
    {
      typedef typename T::config_type     type; 
    };
    
    template <typename ConfigType>
    struct config< domain_t<ConfigType> >
    {
      typedef ConfigType     type; 
    };

    template <typename ConfigType>
    struct config< segment_t<ConfigType> >
    {
      typedef ConfigType     type; 
    };

    template <typename ConfigType, typename ElementTag>
    struct config< element_t<ConfigType, ElementTag> >
    {
      typedef ConfigType     type; 
    };

    
    template <typename T,   //type of host (domain, segment, other element)
              long dim,
              long cell_level = config<T>::type::cell_tag::dim>
    struct element_container;
    
    template <typename T, 
              long dim>  //topological level
    struct ncell_range;
    
    
    template <typename Config,
              long dim,
              long cell_level = Config::cell_tag::dim>
    struct ncell
    {
      typedef element_t<Config, 
                        typename topology::bndcells<typename Config::cell_tag,
                                                       dim>::tag
                       > type;
    };
    
    template <typename Config,
              long cell_level>
    struct ncell <Config, cell_level, cell_level>
    {
      typedef element_t<Config, 
                        typename Config::cell_tag>       type;
    };
    
    
    template <typename Config>
    struct point
    {
      typedef viennagrid::point_t<typename Config::numeric_type, typename Config::coordinate_system_tag>   type;
    };
    

    // Metafunction for local-to-global-orienters: By default, store orienters
    template <typename ConfigType, typename T, long dim>
    struct bndcell_orientation
    {
      typedef full_handling_tag    type;
    };
    
    
    
    template <typename ConfigType, typename T, long dim>
    struct bndcell_handling
    {
      typedef full_handling_tag    type;
    };
    
    // Vertex level always uses full handling (it is the defining entity of an element).
    // Even though the full_handling_tag is covered by the default overload, make this specialization explicit such that the user cannot accidentally modify it.
    template <typename ConfigType, typename T>
    struct bndcell_handling<ConfigType, T, 0>
    {
      typedef full_handling_tag    type; 
    };
    
    

    //for domains
    template <typename ConfigType, long dim>
    struct bndcell_handling<ConfigType, domain_t<ConfigType>, dim>
    {
      typedef full_handling_tag    type;
    };
    
    // Vertex level always uses full handling (it is the defining entity of an element).
    // Even though the full_handling_tag is covered by the default overload, make this specialization explicit such that the user cannot accidentally modify it.
    template <typename ConfigType>
    struct bndcell_handling<ConfigType, domain_t<ConfigType>, 0>
    {
      typedef full_handling_tag    type; 
    };

    
    //for segments:
    template <typename ConfigType, long dim>
    struct bndcell_handling<ConfigType, segment_t<ConfigType>, dim>
    {
      typedef typename bndcell_handling<ConfigType, typename ConfigType::cell_tag, dim>::type    type;
    };

    template <typename ConfigType>
    struct bndcell_handling<ConfigType, segment_t<ConfigType>, 0>  //avoid ambiguities
    {
      typedef typename bndcell_handling<ConfigType, typename ConfigType::cell_tag, 0>::type    type;
    };
    
  }
  
  // providing forwards for the ncells function
  template <long dim, typename DomainConfig>
  ncell_range<domain_t<DomainConfig>, dim>
  ncells(domain_t<DomainConfig> & d);

  template <typename DomainConfig>
  ncell_proxy< domain_t<DomainConfig> >
  ncells(domain_t<DomainConfig> & d);

  template <long dim, typename DomainConfig>
  ncell_range<segment_t<DomainConfig>, dim>
  ncells(segment_t<DomainConfig> & d);

  template <typename DomainConfig>
  ncell_proxy< segment_t<DomainConfig> >
  ncells(segment_t<DomainConfig> & d);

  template <long dim, typename Config, typename ElementTag>
  typename result_of::ncell_range< element_t<Config, ElementTag>, dim>::type
  ncells(element_t<Config, ElementTag> & d);
  
  template <typename Config, typename ElementTag>
  ncell_proxy< element_t<Config, ElementTag> >
  ncells(element_t<Config, ElementTag> & d);

   // norm tags for: algorithm/norm.hpp
   struct one_tag {};
   struct two_tag {};
   struct inf_tag {};
   
   // norm tags/types for: algorithm/cell_normals.hpp
   struct seg_cell_normal_tag {};
   struct seg_cell_normal_data {
      typedef viennagrid::point_t<double, cartesian_cs<3> >         point_type;
      typedef std::map<std::size_t, point_type>    type;
   };
   
   
  // 
  //refinement
  //
   
  template <typename T>
  struct element_refinement;
   
  struct refinement_key {};
   
  template <typename DomainType, typename RefinementTag>
  class refinement_proxy;
   
  struct uniform_refinement_tag {};
  struct local_refinement_tag {};
  
  namespace detail
  {
    template <typename ConfigTypeIn, typename ConfigTypeOut>
    void refine_impl(domain_t<ConfigTypeIn> const & domain_in,
                    domain_t<ConfigTypeOut> & domain_out,
                    uniform_refinement_tag);
    
    template <typename ConfigTypeIn, typename ConfigTypeOut>
    void refine_impl(domain_t<ConfigTypeIn> const & domain_in,
                    domain_t<ConfigTypeOut> & domain_out,
                    local_refinement_tag);    
  }
  
  //
  // Voronoi information:
  //
  struct voronoi_interface_area_key {};   //interface area associated with an edge
  struct voronoi_box_volume_key {};       //box volume associated with an edge or vertex
}

namespace viennadata
{
  namespace config
  {
    template <>
    struct key_dispatch<viennagrid::refinement_key>
    {
      typedef type_key_dispatch_tag    tag;
    };    
    
    template <>
    struct key_dispatch<viennagrid::voronoi_interface_area_key>
    {
      typedef type_key_dispatch_tag    tag;
    };
    
    template <>
    struct key_dispatch<viennagrid::voronoi_box_volume_key>
    {
      typedef type_key_dispatch_tag    tag;
    };
  }
}

////// storage scheme for cells ////////

//
// Define convenience macros for changing the storage scheme in ViennaGrid:
//
#define VIENNAGRID_DISABLE_BOUNDARY_NCELL(arg_CONFIG, arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_handling<arg_CONFIG, arg_TAG, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

#define VIENNAGRID_ENABLE_BOUNDARY_NCELL(arg_CONFIG, arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_handling<arg_CONFIG, arg_TAG, arg_DIM> { \
      typedef full_handling_tag    type; \
    }; \
 } }

#define VIENNAGRID_GLOBAL_DISABLE_BOUNDARY_NCELL(arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <typename ConfigType> \
    struct bndcell_handling<ConfigType, arg_TAG, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

//note that VIENNAGRID_GLOBAL_ENABLE_BOUNDARY_NCELL(arg_TAG, arg_DIM)  does not make sense, since the default is full_handling already.

//
// Same for orientation
//
#define VIENNAGRID_DISABLE_BOUNDARY_NCELL_ORIENTATION(arg_CONFIG, arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_orientation<arg_CONFIG, arg_TAG, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

#define VIENNAGRID_ENABLE_BOUNDARY_NCELL_ORIENTATION(arg_CONFIG, arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_orientation<arg_CONFIG, arg_TAG, arg_DIM> { \
      typedef full_handling_tag    type; \
    }; \
 } }

#define VIENNAGRID_GLOBAL_DISABLE_BOUNDARY_NCELL_ORIENTATION(arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <typename ConfigType> \
    struct bndcell_orientation<ConfigType, arg_TAG, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

//
// ID for elements:
//
#define VIENNAGRID_DISABLE_NCELL_ID(arg_CONFIG, arg_TAG) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct element_id_handler<arg_CONFIG, arg_TAG> { \
      typedef pointer_id    type; \
    }; \
 } }

#define VIENNAGRID_ENABLE_NCELL_ID(arg_CONFIG, arg_TAG) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct element_id_handler<arg_CONFIG, arg_TAG> { \
      typedef integral_id    type; \
    }; \
 } }
 
#define VIENNAGRID_GLOBAL_DISABLE_NCELL_ID(arg_TAG) \
 namespace viennagrid { namespace result_of { \
    template <typename ConfigType> \
    struct element_id_handler<ConfigType, arg_TAG> { \
      typedef pointer_id    type; \
    }; \
 } }


//
////// storage scheme for domain ////////
//

#define VIENNAGRID_DISABLE_DOMAIN_NCELL(arg_CONFIG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_handling<arg_CONFIG, domain_t<arg_CONFIG>, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

#define VIENNAGRID_ENABLE_DOMAIN_NCELL(arg_CONFIG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_handling<arg_CONFIG, domain_t<arg_CONFIG>, arg_DIM> { \
      typedef full_handling_tag    type; \
    }; \
 } }

#define VIENNAGRID_GLOBAL_DISABLE_DOMAIN_NCELL(arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <typename ConfigType> \
    struct bndcell_handling<ConfigType, domain_t<ConfigType>, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }



#endif
