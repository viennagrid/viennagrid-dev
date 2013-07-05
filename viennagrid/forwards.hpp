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

#include "viennagrid/storage/static_array.hpp"
#include "viennagrid/utils/remove_pointer.hpp"

#include "viennagrid/storage/forwards.hpp"
//#include "topology/polygon.hpp"
 

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
    
    
    template<typename iterator_type, typename distance_type>
    iterator_type advance(const iterator_type & in, distance_type distance)
    {
        iterator_type out(in);
        std::advance(out, distance);
        return out;
    }
    
    
    
    
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

  struct coboundary_collection_tag;
  struct neighbour_collection_tag;
  struct boundary_information_collection_tag;
  
  struct interface_information_collection_tag;

  
  /********* Forward definitions of main classes *******************/

  // Simplex family:
  /** @brief A tag for the simplex family (triangle, tetrahedron, etc.) */
  template <long dim>
  struct simplex_tag;

  /** @brief Convenience type definition for a vertex */
  typedef simplex_tag<0>    vertex_tag;
  /** @brief Convenience type definition for a line */
  typedef simplex_tag<1>    line_tag;
  typedef simplex_tag<1>    edge_tag;
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
  
  struct polygon_tag;
//   struct hole_polygon_tag;
  
  struct plc_tag;
  
  //forward declarations:
  template <typename CoordType, typename CoordinateSystem>
  class point_t;


  template<typename element_tag, typename WrappedConfigType>
  class element_t;

  template <typename element_type>
  class element_key;

  
  /** @brief A key used for ViennaData to store coboundary information on n-cells */
  template <typename T, typename element_tag> //topological dimension of the elements over which to iterate
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
  
  template <typename T, typename connector_boundary_element_tag> //topological dimension of the elements over which to iterate
  class neighbour_key
  {
    public:
      neighbour_key(T const & t_) : t(&t_) {}
      
      /** @brief Comparison operator for compatibility with std::map */
      bool operator<(neighbour_key const & other) const
      {
        return t < other.t;
      }
    private:
      T const * t;
  };

  template <typename T>
  class boundary_key;

  template <typename T>
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
  

  struct static_layout_tag;
  struct dynamic_layout_tag;
  struct dynamic_unique_layout_tag;
  
  /** @brief Namespace for definition and handling of the individual topological elements (triangles, hexahedra, etc.) */
  namespace element_topology
  {
    
    /** @brief Holds the topological information about boundary cells for a n-cell
     * 
     * @tparam ElementTag   Identifier for the n-cell
     * @tparam level        Topological dimension of the boundary that is specified here
     */

    
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

    template <typename element_tag, typename bnd_cell_tag, typename bnd_cell_type>
    struct bndcell_generator {};
    
    template<typename cell_type, typename bnd_cell_type>
    struct bndcell_generator<cell_type, simplex_tag<0>, bnd_cell_type>
    {
        template<typename element_type, typename inserter_type>
        static void create_bnd_cells(element_type & element, inserter_type & inserter)
        {}
    };
    
  }
  
  
  /** @brief The metafunction layer. Provides type generators that derive the respective type from the domain configuration */
  namespace result_of
  {
    template<typename something>
    struct domain;
    
    template<typename something>
    struct element_collection;
  
    template<typename something>
    struct container_collection_typemap;

    template<typename something>
    struct topologic_cell_dimension;

    template<typename something>
    struct cell_types;

    template<typename something>
    struct cell_type;
    
    template<typename something>
    struct cell_tag;
    
    
    template<typename something>
    struct point_type;
    
    template<typename something>
    struct coord_type
    {
        typedef typename coord_type< typename point_type<something>::type >::type type;
    };
    
    template<typename something>
    struct geometric_dimension
    {
        static const int value = geometric_dimension< typename point_type<something>::type >::value;
    };
        

        
        
    /** @brief Metafunction for the type retrieval of n-cells
     * 
     * @tparam Config       The configuration class
     * @tparam dim          Topological level n
     * @tparam cell_level   Internal flag that refers to the maximum dimension in the domain
     */
    
    template<typename config_domain_segment_element_or_something_like_that, typename element_tag>
    struct element;
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct vertex
    {
        typedef typename element<config_domain_segment_element_or_something_like_that, vertex_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct line
    {
        typedef typename element<config_domain_segment_element_or_something_like_that, line_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct triangle
    {
        typedef typename element<config_domain_segment_element_or_something_like_that, triangle_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct quadrilateral
    {
        typedef typename element<config_domain_segment_element_or_something_like_that, quadrilateral_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct plc
    {
        typedef typename element<config_domain_segment_element_or_something_like_that, plc_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct tetrahedron
    {
        typedef typename element<config_domain_segment_element_or_something_like_that, tetrahedron_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct hexahedron
    {
        typedef typename element<config_domain_segment_element_or_something_like_that, hexahedron_tag>::type type;
    };
    
    

    template<typename config_domain_segment_element_or_something_like_that, typename element_tag>
    struct handle;
    
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct vertex_handle
    {
        typedef typename handle<config_domain_segment_element_or_something_like_that, vertex_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct line_handle
    {
        typedef typename handle<config_domain_segment_element_or_something_like_that, line_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct triangle_handle
    {
        typedef typename handle<config_domain_segment_element_or_something_like_that, triangle_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct quadrilateral_handle
    {
        typedef typename handle<config_domain_segment_element_or_something_like_that, quadrilateral_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct plc_handle
    {
        typedef typename handle<config_domain_segment_element_or_something_like_that, plc_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct tetrahedron_handle
    {
        typedef typename handle<config_domain_segment_element_or_something_like_that, tetrahedron_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct hexahedron_handle
    {
        typedef typename handle<config_domain_segment_element_or_something_like_that, hexahedron_tag>::type type;
    };
    
    
    
    template<typename config_domain_segment_element_or_something_like_that, typename element_tag>
    struct const_handle;
    
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_vertex_handle
    {
        typedef typename const_handle<config_domain_segment_element_or_something_like_that, vertex_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_line_handle
    {
        typedef typename const_handle<config_domain_segment_element_or_something_like_that, line_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_triangle_handle
    {
        typedef typename const_handle<config_domain_segment_element_or_something_like_that, triangle_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_quadrilateral_handle
    {
        typedef typename const_handle<config_domain_segment_element_or_something_like_that, quadrilateral_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_plc_handle
    {
        typedef typename const_handle<config_domain_segment_element_or_something_like_that, plc_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_tetrahedron_handle
    {
        typedef typename const_handle<config_domain_segment_element_or_something_like_that, tetrahedron_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_hexahedron_handle
    {
        typedef typename const_handle<config_domain_segment_element_or_something_like_that, hexahedron_tag>::type type;
    };
    
    
    
    template<typename element_type>
    struct facet_type;
    
    template<typename element_type>
    struct facet_handle;
        
    template<typename element_type>
    struct const_facet_handle;


    
    template <typename T, 
              typename element_tag>
    struct element_range;
    
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct cell_range
    {
        typedef typename cell_tag<config_domain_segment_element_or_something_like_that>::type cell_tag;
        typedef typename element_range<config_domain_segment_element_or_something_like_that, cell_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct vertex_range
    {
        typedef typename element_range<config_domain_segment_element_or_something_like_that, vertex_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct line_range
    {
        typedef typename element_range<config_domain_segment_element_or_something_like_that, line_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct triangle_range
    {
        typedef typename element_range<config_domain_segment_element_or_something_like_that, triangle_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct quadrilateral_range
    {
        typedef typename element_range<config_domain_segment_element_or_something_like_that, quadrilateral_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct plc_range
    {
        typedef typename element_range<config_domain_segment_element_or_something_like_that, plc_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct tetrahedron_range
    {
        typedef typename element_range<config_domain_segment_element_or_something_like_that, tetrahedron_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct hexahedron_range
    {
        typedef typename element_range<config_domain_segment_element_or_something_like_that, hexahedron_tag>::type type;
    };

    
    
    
    
    
    template <typename T, 
              typename element_tag>
    struct const_element_range;

    
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_cell_range
    {
        typedef typename cell_tag<config_domain_segment_element_or_something_like_that>::type cell_tag;
        typedef typename const_element_range<config_domain_segment_element_or_something_like_that, cell_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_vertex_range
    {
        typedef typename const_element_range<config_domain_segment_element_or_something_like_that, vertex_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_line_range
    {
        typedef typename const_element_range<config_domain_segment_element_or_something_like_that, line_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_triangle_range
    {
        typedef typename const_element_range<config_domain_segment_element_or_something_like_that, triangle_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_quadrilateral_range
    {
        typedef typename const_element_range<config_domain_segment_element_or_something_like_that, quadrilateral_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_plc_range
    {
        typedef typename const_element_range<config_domain_segment_element_or_something_like_that, plc_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_tetrahedron_range
    {
        typedef typename const_element_range<config_domain_segment_element_or_something_like_that, tetrahedron_tag>::type type;
    };
    
    template<typename config_domain_segment_element_or_something_like_that>
    struct const_hexahedron_range
    {
        typedef typename const_element_range<config_domain_segment_element_or_something_like_that, hexahedron_tag>::type type;
    };
    
    
    
    
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
    struct handle_iterator
    {
        typedef typename container::handle_iterator type;
    };

    template <typename container>
    struct const_handle_iterator
    {
        typedef typename container::const_handle_iterator type;
    };
    
    
    
    template<typename element_tag_>
    struct element_tag
    {
        typedef element_tag_ type;
    };
    
    template<typename element_tag_, typename WrappedConfigType>
    struct element_tag< element_t<element_tag_, WrappedConfigType> >
    {
        typedef element_tag_ type;
    };
    
    template<typename element_tag_, typename WrappedConfigType>
    struct element_tag< const element_t<element_tag_, WrappedConfigType> >
    {
        typedef element_tag_ type;
    };
    
    
    
    template<typename element_type_or_tag>
    struct facet_tag
    {
        typedef typename element_tag<element_type_or_tag>::type::facet_tag type;
    };
    
    
    
    template<typename element_type>
    struct facet_type
    {
        typedef typename element<element_type, typename facet_tag<element_type>::type >::type type;
    };

    template<typename element_tag, typename WrappedConfigType>
    struct facet_handle< element_t<element_tag, WrappedConfigType> >
    {
        typedef typename handle< element_t<element_tag, WrappedConfigType>, typename facet_tag<element_tag>::type >::type type;
    };
    
    template<typename element_tag, typename WrappedConfigType>
    struct const_facet_handle< element_t<element_tag, WrappedConfigType> >
    {
        typedef typename const_handle< element_t<element_tag, WrappedConfigType>, typename facet_tag<element_tag>::type >::type type;
    };
    
    
    template<typename element_type>
    struct facet_range
    {
        typedef typename element_range<element_type, typename facet_tag<element_type>::type >::type type;
    };
    
    template<typename element_type>
    struct const_facet_range
    {
        typedef typename const_element_range<element_type, typename facet_tag<element_type>::type >::type type;
    };
  }
  
    
    
    
    template<typename element_domain_segment_config_or_something_like_that>
    struct element_range_proxy
    {
        element_range_proxy(element_domain_segment_config_or_something_like_that & something_) : something(something_) {}
        
        element_domain_segment_config_or_something_like_that const & operator() () const { return something; }
        element_domain_segment_config_or_something_like_that & operator() () { return something; }
        
        element_domain_segment_config_or_something_like_that & something;
    };
    

    template<typename element_domain_segment_config_or_something_like_that>
    element_range_proxy<element_domain_segment_config_or_something_like_that> elements( element_domain_segment_config_or_something_like_that & something)
    { return element_range_proxy<element_domain_segment_config_or_something_like_that>(something); }
    
    template<typename element_tag, typename element_domain_segment_config_or_something_like_that>
    typename result_of::element_range<element_domain_segment_config_or_something_like_that, element_tag>::type elements( element_domain_segment_config_or_something_like_that & something);
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::cell_range<element_domain_segment_config_or_something_like_that>::type cells( element_domain_segment_config_or_something_like_that & something)
    { return elements<typename result_of::cell_tag<element_domain_segment_config_or_something_like_that>::type>(something); }    

    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::element_range<element_domain_segment_config_or_something_like_that, vertex_tag>::type vertices( element_domain_segment_config_or_something_like_that & something)
    { return elements<vertex_tag>(something); }

    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::element_range<element_domain_segment_config_or_something_like_that, line_tag>::type lines( element_domain_segment_config_or_something_like_that & something)
    { return elements<line_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::element_range<element_domain_segment_config_or_something_like_that, triangle_tag>::type triangles( element_domain_segment_config_or_something_like_that & something)
    { return elements<triangle_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::element_range<element_domain_segment_config_or_something_like_that, quadrilateral_tag>::type quadrilaterals( element_domain_segment_config_or_something_like_that & something)
    { return elements<quadrilateral_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::element_range<element_domain_segment_config_or_something_like_that, plc_tag>::type plcs( element_domain_segment_config_or_something_like_that & something)
    { return elements<plc_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::element_range<element_domain_segment_config_or_something_like_that, tetrahedron_tag>::type tetrahedrons( element_domain_segment_config_or_something_like_that & something)
    { return elements<tetrahedron_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::element_range<element_domain_segment_config_or_something_like_that, hexahedron_tag>::type hexahedrons( element_domain_segment_config_or_something_like_that & something)
    { return elements<hexahedron_tag>(something); }

    
     
    template<typename element_domain_segment_config_or_something_like_that>
    element_range_proxy<const element_domain_segment_config_or_something_like_that> elements( const element_domain_segment_config_or_something_like_that & something)
    { return element_range_proxy<const element_domain_segment_config_or_something_like_that>(something); }
    
    template<typename element_tag, typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_element_range<element_domain_segment_config_or_something_like_that, element_tag>::type elements( const element_domain_segment_config_or_something_like_that & something);
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_cell_range<element_domain_segment_config_or_something_like_that>::type cells( element_domain_segment_config_or_something_like_that const & something)
    { return elements<typename result_of::cell_tag<element_domain_segment_config_or_something_like_that>::type>(something); }   
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_element_range<element_domain_segment_config_or_something_like_that, vertex_tag>::type vertices( const element_domain_segment_config_or_something_like_that & something)
    { return elements<vertex_tag>(something); }

    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_element_range<element_domain_segment_config_or_something_like_that, line_tag>::type lines( const element_domain_segment_config_or_something_like_that & something)
    { return elements<line_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_element_range<element_domain_segment_config_or_something_like_that, triangle_tag>::type triangles( const element_domain_segment_config_or_something_like_that & something)
    { return elements<triangle_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_element_range<element_domain_segment_config_or_something_like_that, quadrilateral_tag>::type quadrilaterals( const element_domain_segment_config_or_something_like_that & something)
    { return elements<quadrilateral_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_element_range<element_domain_segment_config_or_something_like_that, plc_tag>::type plcs( const element_domain_segment_config_or_something_like_that & something)
    { return elements<plc_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_element_range<element_domain_segment_config_or_something_like_that, tetrahedron_tag>::type tetrahedrons( const element_domain_segment_config_or_something_like_that & something)
    { return elements<tetrahedron_tag>(something); }
    
    template<typename element_domain_segment_config_or_something_like_that>
    typename result_of::const_element_range<element_domain_segment_config_or_something_like_that, hexahedron_tag>::type hexahedrons( const element_domain_segment_config_or_something_like_that & something)
    { return elements<hexahedron_tag>(something); }
    
    

    template<typename element_type>
    typename result_of::facet_range<element_type>::type facets(element_type & element)
    {
      assert(false && "implementation missing!");
//         return elements< typename result_of::facet_tag<element_type>::type >(element);
    }
    
    template<typename element_type>
    typename result_of::const_facet_range<element_type>::type facets(const element_type & element)
    {
        return elements< typename result_of::facet_tag<element_type>::type >(element);
    }

    
    template <typename DomainConfigType, typename ElementTypeList, typename ContainerConfig>
    class decorated_domain_view_config;
    
    template <typename WrappedConfigType>
    class domain_t;
    
    template<typename container_type_>
    struct coboundary_container_wrapper;
    
    template<typename container_type_>
    struct neighbour_container_wrapper;
    
    template<typename container_type_>
    struct boundary_information_wrapper;
    
    
    template<typename container_type, typename vertex_type>
    typename container_type::value_type & point( container_type & geometric_container, vertex_type const & vertex );
    
    
    template<typename element_type, typename domain_type, typename handle_array_iterator_type>
    typename result_of::handle<domain_type, typename element_type::tag>::type create_element( domain_type & domain,
                                                                                              handle_array_iterator_type array_start,
                                                                                              const handle_array_iterator_type & array_end );

    template<typename element_type, typename domain_type, typename handle_array_iterator_type>
    typename result_of::handle<domain_type, typename element_type::tag>::type create_element( domain_type & domain,
                                                                                              handle_array_iterator_type array_start,
                                                                                              const handle_array_iterator_type & array_end,
                                                                                              typename element_type::id_type id );
    
  
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

#endif
