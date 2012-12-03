#ifndef VIENNAGRID_FORWARDS_H
#define VIENNAGRID_FORWARDS_H
 
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

/**
 @mainpage Source Code Documentation for ViennaGrid 1.0.1

 This is the source code documentation of ViennaGrid. Detailed information about the functions in ViennaGrid can be found here.
 
 For a general overview over the types and functionality provided by ViennaGrid, please refer to the file doc/viennagrid.pdf

*/

#include <iostream>
#include <vector>
#include <map>
#include <cstddef>     //for std::size_t
#include <cstdlib>     //for EXIT_SUCCESS and EXIT_FAILURE

#include "viennadata/api.hpp"
#include "viennagrid/storage/static_array.hpp"
#include "viennagrid/utils/remove_pointer.hpp"

#include "viennagrid/storage/forwards.hpp"
 

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

/** @file forwards.h
    @brief Contains forward declarations and definition of small classes that must be defined at an early stage
*/

/** @brief The main ViennaGrid namespace. Most functionality resides in this namespace */
namespace viennagrid
{
  /** @brief A global size_type equivalent for use throughout ViennaData. */
  typedef std::size_t       dim_type;
  
  /********* Tags ***********************/
  
  /** @brief Forward declaration of a d-dimensional Cartesian coordinate system */
  template <long d>
  struct cartesian_cs;    //Cartesian coordinate system
  
  /** @brief Forward declaration of a polar coordinate system */
  struct polar_cs;        //Polar coordinate system (r, phi)
  /** @brief Forward declaration of a spherical coordinate system */
  struct spherical_cs;    //Spherical coordinate system (r, theta, phi)
  /** @brief Forward declaration of a cylindrical coordinate system */
  struct cylindrical_cs;  //Cylindrical coordinate system (rho, theta, z)

  //Dimension Tag: (for tag dispatching)
  /** @brief A helper tag for dispatches based on the geometrical or topological dimension */
  template <long d>
  struct dimension_tag
  {
      enum{ value = d };
  };
  
  //Tags for the handling of elements at different topological levels (see topology::subcell_desc)
  /** @brief A tag denoting full storage of (boundary) k-cells of a certain topological dimension */
  struct full_handling_tag {};
  /** @brief A tag denoting that the (boundary) k-cells should not be stored */
  struct no_handling_tag {};
  
  struct no_orientation_handling_tag {};
  struct no_orientation_lazy_handling_tag {};
  struct full_lazy_handling_tag {};

  
  /********* Forward definitions of main classes *******************/

  // Simplex family:
  /** @brief A tag for the simplex family (triangle, tetrahedron, etc.) */
  template <long dim>
  struct simplex_tag;

  /** @brief Convenience type definition for a vertex */
  typedef simplex_tag<0>    vertex_tag;
  /** @brief Convenience type definition for a line */
  typedef simplex_tag<1>    line_tag;
  /** @brief Convenience type definition for a triangle */
  typedef simplex_tag<2>    triangle_tag;
  /** @brief Convenience type definition for a tetrahedron */
  typedef simplex_tag<3>    tetrahedron_tag;
  
  // Hypercube family:
  /** @brief A tag for the family of hypercubes (line, quadrilateral, hexahedron, etc.) */
  template <long dim>
  struct hypercube_tag;

  //typedef hypercube_tag<1>    line_tag;
  /** @brief Convenience type definition for a quadrilateral */
  typedef hypercube_tag<2>  quadrilateral_tag;
  /** @brief Convenience type definition for a hexahedron */
  typedef hypercube_tag<3>  hexahedron_tag;
  
  
    struct element_id_tag {};
    struct element_container_tag {};
    struct element_boundary_storage_layout_tag {};


  //forward declarations:
  template <typename CoordType, typename CoordinateSystem>
  class point_t;


  template<typename element_tag, typename bnd_cell_container_typelist_, typename id_tag>
  class element_t;

  template <typename element_type>
  class element_key;

  //Segment type: 
  template <typename ConfigType>
  class segment_t;

  //template <typename ConfigType>
  //class domain_t;
  
  /********* Other *******************/

  template <typename host_element,
            long dim,
            bool is_coboundary = false>
  class ncell_range;

  template <typename host_element,
            long dim,
            bool is_coboundary = false>
  class const_ncell_range;
  
  /** @brief A key used for ViennaData to store coboundary information on n-cells */
  template <typename T, long dim> //topological dimension of the elements over which to iterate
  class coboundary_key
  {
    public:
      coboundary_key(T const & t_) : t(&t_) {}
      
      /** @brief Comparison operator for compatibility with std::map */
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
  /** @brief A proxy class for retrieving const n-cell ranges */
  template <typename T>
  class const_ncell_proxy
  {
    public:
      const_ncell_proxy(T const & t_) : t(t_) {}
      
      T const & get() const { return t; }
    
    private:
      T const & t;
  };
  
  /** @brief A proxy class for retrieving n-cell ranges */
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
  /** @brief ID handler class for IDs based on the n-cell address */
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

  /** @brief ID handler class for IDs of type long. Has to be used for dense quantity storage with ViennaData. */
  class integral_id
  {
    public:
      typedef long   id_type;
      
      integral_id() : id_(-1) {};

      long id() const { return id_; };
      void id(long new_id) { id_ = new_id; };

    protected:
      id_type id_;
  };
  
    struct static_layout_tag;
    struct dynamic_layout_tag;
  
  /** @brief Namespace for definition and handling of the individual topological elements (triangles, hexahedra, etc.) */
  namespace topology
  {
    
    /** @brief Holds the topological information about boundary cells for a n-cell
     * 
     * @tparam ElementTag   Identifier for the n-cell
     * @tparam level        Topological dimension of the boundary that is specified here
     */
    template <typename ElementTag, 
              long level = ElementTag::dim>
    struct bndcells
    {
      //the default case is simultaneously a pathetic case:
      //cell-handling within the cell

      /** @brief Number of boundary cells at this level */
      enum{ num = 1 };     //1 cell

      /** @brief k-cell tag for identification of the type */
      typedef ElementTag            tag;
    };
    
    template <typename ElementTag, typename BoundaryNCellTag = ElementTag>
    struct boundary_cells
    {
      //the default case is simultaneously a pathetic case:
      //cell-handling within the cell

      /** @brief Number of boundary cells at this level */
      enum{ num = 1 };     //1 cell

      /** @brief k-cell tag for identification of the type */
      typedef ElementTag            tag;
    };

    /** @brief Worker class that sets up the boundary k-cells of a n-cell
     * 
     * @tparam ElementTag   Tag for the identification of the hosting n-cell
     * @tparam k            Topological boundary dimension
     */
    template <typename ElementTag, long k>
    struct bndcell_filler {};

    template <typename element_tag, long k, typename bnd_cell_type>
    struct bndcell_generator {};
    
  }
  
  
  /** @brief The metafunction layer. Provides type generators that derive the respective type from the domain configuration */
  namespace result_of
  {

        template<typename something>
        struct metainfo_collection;
        
        template<typename something>
        struct point_type;
        

        
        
    /** @brief Metafunction for the type retrieval of n-cells
     * 
     * @tparam Config       The configuration class
     * @tparam dim          Topological level n
     * @tparam cell_level   Internal flag that refers to the maximum dimension in the domain
     */
    
    template<typename config_domain_segment_element_or_something_like_that, typename element_tag>
    struct element;
    
    template<typename config_domain_segment_element_or_something_like_that, typename element_tag, typename bnd_cell_container_typelist, typename id_type>
    struct element<config_domain_segment_element_or_something_like_that, element_t<element_tag, bnd_cell_container_typelist, id_type> >
    {
        typedef element_t<element_tag, bnd_cell_container_typelist, id_type> type;
    };

    template<typename config_domain_segment_element_or_something_like_that, typename element_tag>
    struct element_hook;
    
    template<typename config_domain_segment_element_or_something_like_that, typename element_tag>
    struct const_element_hook;
    
    template<typename config_domain_segment_element_or_something_like_that, long dim>
    struct ncell;
    
    template<typename config_domain_segment_element_or_something_like_that, long dim>
    struct ncell_hook;
    
    template<typename config_domain_segment_element_or_something_like_that, long dim>
    struct const_ncell_hook;


    
    template <typename T, 
              long dim>  //topological level
    struct ncell_range;
    
    template <typename T, 
              long dim>  //topological level
    struct const_ncell_range;
    
    template <typename T, 
              typename element_tag>
    struct element_range;
    
    template <typename T, 
              typename element_tag>
    struct const_element_range;
    

    
    
    template <typename container>
    struct iterator
    {
        typedef typename container::iterator type;
    };

    template <typename container>
    struct const_iterator
    {
        typedef typename container::const_iterator type;
    };
    
    template <typename container>
    struct hook_iterator
    {
        typedef typename container::hook_iterator type;
    };

    template <typename container>
    struct const_hook_iterator
    {
        typedef typename container::const_hook_iterator type;
    };
    
    
        

        template<typename element_type_or_tag>
        struct facet_tag
        {
            typedef typename element_type_or_tag::facet_tag type;
        };
        
        template<typename element_tag, typename boundary_cell_container_typelist, typename id_type>
        struct facet_tag< element_t<element_tag, boundary_cell_container_typelist, id_type> >
        {
            typedef typename element_tag::facet_tag type;
        };
        
        
        template<typename element_type>
        struct facet_type
        {
            typedef typename element<element_type, typename element_type::tag::facet_tag>::type type;
        };
    
  }
   
    template<long dim, typename element_domain_segment_config_or_something_like_that>
    typename result_of::ncell_range<element_domain_segment_config_or_something_like_that, dim>::type ncells( element_domain_segment_config_or_something_like_that & something);
  
    template<long dim, typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_ncell_range<element_domain_segment_config_or_something_like_that, dim>::type ncells( const element_domain_segment_config_or_something_like_that & something);
    
    template<typename element_tag, typename element_domain_segment_config_or_something_like_that>
    typename result_of::ncell_range<element_domain_segment_config_or_something_like_that, element_tag::dim>::type elements( element_domain_segment_config_or_something_like_that & something)
    { return ncells<element_tag::dim>(something); }
  
    template<typename element_tag, typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_ncell_range<element_domain_segment_config_or_something_like_that, element_tag::dim>::type elements( const element_domain_segment_config_or_something_like_that & something)
    { return ncells<element_tag::dim>(something); }

    
    
    template<typename something>
    typename result_of::point_type<something>::type point( something &, const typename result_of::element<something, vertex_tag>::type &);
    
    template<typename something>
    const typename result_of::point_type<something>::type point( const something &, const typename result_of::element<something, vertex_tag>::type &);

  
  
//   // providing forwards for the ncells function
//   template <long dim, typename DomainConfig>
//   ncell_range<domain_t<DomainConfig>, dim>
//   ncells(domain_t<DomainConfig> & d);
// 
//   template <typename DomainConfig>
//   ncell_proxy< domain_t<DomainConfig> >
//   ncells(domain_t<DomainConfig> & d);
// 
//   template <long dim, typename DomainConfig>
//   ncell_range<segment_t<DomainConfig>, dim>
//   ncells(segment_t<DomainConfig> & d);
// 
//   template <typename DomainConfig>
//   ncell_proxy< segment_t<DomainConfig> >
//   ncells(segment_t<DomainConfig> & d);

//   template <long dim, typename Config, typename ElementTag>
//   typename result_of::ncell_range< element_t<Config, ElementTag>, dim>::type
//   ncells(element_t<Config, ElementTag> & d);
  
//   template <typename Config, typename ElementTag>
//   ncell_proxy< element_t<Config, ElementTag> >
//   ncells(element_t<Config, ElementTag> & d);
  
//   //same for const:  
//   template <long dim, typename DomainConfig>
//   const_ncell_range<domain_t<DomainConfig>, dim>
//   ncells(domain_t<DomainConfig> const & d);
// 
//   template <typename DomainConfig>
//   const_ncell_proxy< domain_t<DomainConfig> >
//   ncells(domain_t<DomainConfig> const & d);
// 
//   template <long dim, typename DomainConfig>
//   const_ncell_range<segment_t<DomainConfig>, dim>
//   ncells(segment_t<DomainConfig> const & d);
// 
//   template <typename DomainConfig>
//   const_ncell_proxy< segment_t<DomainConfig> >
//   ncells(segment_t<DomainConfig> const & d);

//   template <long dim, typename Config, typename ElementTag>
//   typename result_of::const_ncell_range< element_t<Config, ElementTag>, dim>::type
//   ncells(element_t<Config, ElementTag> const & d);
  
//   template <typename Config, typename ElementTag>
//   const_ncell_proxy< element_t<Config, ElementTag> >
//   ncells(element_t<Config, ElementTag> const & d);

  
   // norm tags for: algorithm/norm.hpp
  /** @brief Tag for denoting the 1-norm */
  struct one_tag {};
  /** @brief Tag for denoting the 2-norm */
  struct two_tag {};
  /** @brief Tag for denoting the infinity-norm */
  struct inf_tag {};
   
  /** @brief Types for cell normals in algorithm/cell_normals.hpp (to be added in afuture release) */
  struct seg_cell_normal_tag {};
  
  /** @brief Data type for cell normals in algorithm/cell_normals.hpp (to be added in afuture release) */
  struct seg_cell_normal_data {
    typedef viennagrid::point_t<double, cartesian_cs<3> >         point_type;
    typedef std::map<std::size_t, point_type>    type;
  };
   
   
  // 
  //refinement
  //
   
  template <typename T>
  struct element_refinement;
   
  /** @brief A key type used for tagging edges for refinement using ViennaData */
  struct refinement_key {};
   
  template <typename CellTag, typename DomainType, typename RefinementTag>
  class refinement_proxy;
   
  /** @brief A tag denoting uniform refinement */
  struct uniform_refinement_tag {};
  
  /** @brief A tag denoting local refinement */
  struct local_refinement_tag {};
  
  /** @brief The namespace holding the implementations of domains, segments and ncells as well as some algorithm implementations. Not of interest for library users */
  namespace detail
  {
    template <typename CellTagIn, typename DomainTypeIn, typename DomainTypeOut>
    void refine_impl(DomainTypeIn const & domain_in,
                    DomainTypeOut & domain_out,
                    uniform_refinement_tag);
    
    template <typename CellTagIn, typename DomainTypeIn, typename DomainTypeOut>
    void refine_impl(DomainTypeIn const & domain_in,
                    DomainTypeOut & domain_out,
                    local_refinement_tag);
  }
  
  //
  // Voronoi information:
  //
  /** @brief ViennaData key for the Voronoi interface area associated with an edge */
  struct voronoi_interface_area_key {};
  /** @brief ViennaData key for the Voronoi box volume associated with an edge or vertex */
  struct voronoi_box_volume_key {}; 
}

/*
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
*/

// tell ViennaData to use a type-based key dispatch for the refinement and the voronoi keys
VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(viennagrid::refinement_key)
VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(viennagrid::voronoi_interface_area_key)
VIENNADATA_ENABLE_TYPE_BASED_KEY_DISPATCH(viennagrid::voronoi_box_volume_key)

////// storage scheme for cells ////////

/** @brief Convenience macro for disabling boundary k-cells of dimension arg_DIM for a n-cell identified by arg_TAG when used with a certain configuration class arg_CONFIG */
#define VIENNAGRID_DISABLE_BOUNDARY_NCELL(arg_CONFIG, arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_handling<arg_CONFIG, arg_TAG, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

/** @brief Convenience macro for enabling boundary k-cells of dimension arg_DIM for a n-cell identified by arg_TAG when used with a certain configuration class arg_CONFIG */
#define VIENNAGRID_ENABLE_BOUNDARY_NCELL(arg_CONFIG, arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_handling<arg_CONFIG, arg_TAG, arg_DIM> { \
      typedef full_handling_tag    type; \
    }; \
 } }

/** @brief Convenience macro for disabling boundary k-cells of dimension arg_DIM for a n-cell identified by arg_TAG for all configuration classes */
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
/** @brief Convenience macro for disabling local orientation storage for boundary k-cells of dimension arg_DIM for a n-cell identified by arg_TAG when used with a certain configuration class arg_CONFIG */
#define VIENNAGRID_DISABLE_BOUNDARY_NCELL_ORIENTATION(arg_CONFIG, arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_orientation<arg_CONFIG, arg_TAG, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

/** @brief Convenience macro for enabling local orientation storage for boundary k-cells of dimension arg_DIM for a n-cell identified by arg_TAG when used with a certain configuration class arg_CONFIG */
#define VIENNAGRID_ENABLE_BOUNDARY_NCELL_ORIENTATION(arg_CONFIG, arg_TAG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_orientation<arg_CONFIG, arg_TAG, arg_DIM> { \
      typedef full_handling_tag    type; \
    }; \
 } }

/** @brief Convenience macro for disabling local orientation storage for boundary k-cells of dimension arg_DIM for a n-cell identified by arg_TAG for all configuration classes */
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
/** @brief Convenience macro for disabling the storage of an ID for a n-cell identified by arg_TAG when used with a certain configuration class arg_CONFIG */
#define VIENNAGRID_DISABLE_NCELL_ID(arg_CONFIG, arg_TAG) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct element_id_handler<arg_CONFIG, arg_TAG> { \
      typedef pointer_id    type; \
    }; \
 } }

/** @brief Convenience macro for enabling the storage of an ID for a n-cell identified by arg_TAG when used with a certain configuration class arg_CONFIG */
#define VIENNAGRID_ENABLE_NCELL_ID(arg_CONFIG, arg_TAG) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct element_id_handler<arg_CONFIG, arg_TAG> { \
      typedef integral_id    type; \
    }; \
 } }
 
/** @brief Convenience macro for disabling the storage of an ID for a n-cell identified by arg_TAG for all configuration classes. */
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
/** @brief Disable the storage of n-cells for the domain with a certain configuration class arg_CONFIG. Requires disabled storage of boundary n-cells. */
#define VIENNAGRID_DISABLE_DOMAIN_NCELL(arg_CONFIG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_handling<arg_CONFIG, domain_t<arg_CONFIG>, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

/** @brief Selectively enable the storage of n-cells for the domain with a certain configuration class arg_CONFIG. */
#define VIENNAGRID_ENABLE_DOMAIN_NCELL(arg_CONFIG, arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <> \
    struct bndcell_handling<arg_CONFIG, domain_t<arg_CONFIG>, arg_DIM> { \
      typedef full_handling_tag    type; \
    }; \
 } }

/** @brief Disable the storage of n-cells for the domain for all configuration classes. Requires disabled storage of boundary n-cells. */
#define VIENNAGRID_GLOBAL_DISABLE_DOMAIN_NCELL(arg_DIM) \
 namespace viennagrid { namespace result_of { \
    template <typename ConfigType> \
    struct bndcell_handling<ConfigType, domain_t<ConfigType>, arg_DIM> { \
      typedef no_handling_tag    type; \
    }; \
 } }

//
//////////// Enable ID for ncells
//
/** @brief Enable the use of the n-cell ID for ViennaData for a certain configuration class arg_CONFIG. Required for dense storage with ViennaData. arg_CELLTAG denotes the n-cell tag. */
#define VIENNAGRID_ENABLE_NCELL_ID_FOR_DATA(arg_CONFIG, arg_CELLTAG) \
namespace viennadata { namespace config { \
    template <> struct object_identifier<viennagrid::element_t<arg_CONFIG, arg_CELLTAG> >  { \
      typedef object_provided_id    tag; \
      typedef size_t                id_type; \
      static size_t get(viennagrid::element_t<arg_CONFIG, arg_CELLTAG> const & obj) { return obj.id(); } \
    };  }  }

/** @brief Enable the use of the n-cell ID for ViennaData for all n-cells using a certain configuration class arg_CONFIG. Required for dense storage with ViennaData. */
#define VIENNAGRID_ENABLE_ALL_NCELL_ID_FOR_DATA(arg_CONFIG) \
namespace viennadata { namespace config { \
    template <typename CellTag> struct object_identifier<viennagrid::element_t<arg_CONFIG, CellTag> >  { \
      typedef object_provided_id    tag; \
      typedef size_t                id_type; \
      static size_t get(viennagrid::element_t<arg_CONFIG, CellTag> const & obj) { return obj.id(); } \
    };  }  }

/** @brief Enable the use of the n-cell ID for ViennaData for n-cells identified by the tag arg_CELLTAG, irrespective of the configuration class. Required for dense storage with ViennaData. */
#define VIENNAGRID_GLOBAL_ENABLE_NCELL_ID_FOR_DATA(arg_CELLTAG) \
namespace viennadata { namespace config { \
    template <typename ConfigType> struct object_identifier<viennagrid::element_t<ConfigType, arg_CELLTAG> >  { \
      typedef object_provided_id    tag; \
      typedef size_t                id_type; \
      static size_t get(viennagrid::element_t<ConfigType, arg_CELLTAG> const & obj) { return obj.id(); } \
    };  }  }

/** @brief Enable the use of the n-cell ID for ViennaData for all n-cells and all configuration classes. Required for dense storage with ViennaData. */
#define VIENNAGRID_GLOBAL_ENABLE_ALL_NCELL_ID_FOR_DATA() \
namespace viennadata { namespace config { \
    template <typename ConfigType, typename CellTag> struct object_identifier<viennagrid::element_t<ConfigType, CellTag> >  { \
      typedef object_provided_id    tag; \
      typedef size_t                id_type; \
      static size_t get(viennagrid::element_t<ConfigType, CellTag> const & obj) { return obj.id(); } \
    };  }  }

#endif
