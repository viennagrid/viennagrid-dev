#ifndef VIENNAGRID_FORWARDS_H
#define VIENNAGRID_FORWARDS_H

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

/**
 @mainpage Source Code Documentation for ViennaGrid 1.1.0

 This is the source code documentation of ViennaGrid. Detailed information about the functions in ViennaGrid can be found here.

 For a general overview over the types and functionality provided by ViennaGrid, please refer to the file doc/viennagrid.pdf

*/

#include <iostream>
#include <vector>
#include <map>
#include <cstddef>     //for std::size_t
#include <cstdlib>     //for EXIT_SUCCESS and EXIT_FAILURE


#include "viennagrid/storage/static_array.hpp"
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

/** @file forwards.hpp
    @brief Contains forward declarations and definition of small classes that must be defined at an early stage
*/

/** @brief The main ViennaGrid namespace. Most functionality resides in this namespace */
namespace viennagrid
{
  /** @brief Advances an iterator and returns it
    *
    * @param in          the input iterator
    * @param distance    the distance to advance
    * @return            the iterator, advanced by distance
    */
  template<typename iterator_type, typename distance_type>
  iterator_type advance(iterator_type const & in, distance_type distance)
  {
      iterator_type out(in);
      std::advance(out, distance);
      return out;
  }




  /** @brief A global size_type equivalent for use throughout ViennaData. */
  typedef std::size_t       dim_type;

  /********* Tags ***********************/

  /** @brief Forward declaration of a d-dimensional Cartesian coordinate system */
  template <int d>
  struct cartesian_cs;    //Cartesian coordinate system

  /** @brief Forward declaration of a polar coordinate system */
  struct polar_cs;        //Polar coordinate system (r, phi)
  /** @brief Forward declaration of a spherical coordinate system */
  struct spherical_cs;    //Spherical coordinate system (r, theta, phi)
  /** @brief Forward declaration of a cylindrical coordinate system */
  struct cylindrical_cs;  //Cylindrical coordinate system (rho, theta, z)

  //Dimension Tag: (for tag dispatching)
  /** @brief A helper tag for dispatches based on the geometrical or topological dimension */
  template <int d>
  struct dimension_tag
  {
      static const int value = d;
  };

  //Tags for the handling of elements at different topological levels (see topology::subcell_desc)
  /** @brief A tag denoting full storage of boundary elements of a certain topological element */
  struct full_handling_tag {};
  /** @brief A tag denoting that the boundary elements should not be stored at all */
  struct no_handling_tag {};

  /** @brief A tag denoting that the boundary elements should stored without orientation */
  struct no_orientation_handling_tag {};
  
//   Lazy storage reserved for future use
  struct no_orientation_lazy_handling_tag {};
  struct full_lazy_handling_tag {};

  
  struct coboundary_collection_tag;
  struct neighbour_collection_tag;
  struct boundary_information_collection_tag;

  struct interface_information_collection_tag;


  /********* Forward definitions of main classes *******************/

  // Simplex family:
  /** @brief A tag for the simplex family (triangle, tetrahedron, etc.) */
  template <int dim>
  struct simplex_tag;

  /** @brief Convenience type definition for a vertex */
  typedef simplex_tag<0>    vertex_tag;
  /** @brief Convenience type definition for a line (same as edge_tag) */
  typedef simplex_tag<1>    line_tag;
  /** @brief Convenience type definition for a line (same as line_tag) */
  typedef simplex_tag<1>    edge_tag;
  /** @brief Convenience type definition for a triangle */
  typedef simplex_tag<2>    triangle_tag;
  /** @brief Convenience type definition for a tetrahedron */
  typedef simplex_tag<3>    tetrahedron_tag;

  // Hypercube family:
  /** @brief A tag for the family of hypercubes (line, quadrilateral, hexahedron, etc.) */
  template <int dim>
  struct hypercube_tag;

  //typedef hypercube_tag<1>    line_tag;
  /** @brief Convenience type definition for a quadrilateral */
  typedef hypercube_tag<2>  quadrilateral_tag;
  /** @brief Convenience type definition for a hexahedron */
  typedef hypercube_tag<3>  hexahedron_tag;

  /** @brief A tag representing polygons */
  struct polygon_tag;
  
  /** @brief A tag representing plcs */
  struct plc_tag;
  
  
  /** @brief Tag that indicates that a static container can be used for storing the boundary element handles (e.g. used in simplices and hypercubes) */
  struct static_layout_tag;
  /** @brief Tag that indicates that a dynamic container has to be used for storing the boundary element handles (e.g. polygon and plc) */
  struct dynamic_layout_tag;
  
  

  //forward declarations:
  
  /** @brief Represents an arbitrary point in the underlying coordinate system.
   *
   * @tparam CoordType          Type of each coordinate entry (usually double or float)
   * @tparam CoordinateSystem   The underlying coordinate system of the point.
   */
  template <typename CoordType, typename CoordinateSystem>
  class point_t;

  /** @brief Represents a topological element within a mesh
   *
   * @tparam ElementTag         Tag representing the element (e.g. vertex_tag)
   * @tparam WrappedConfigType  The configuration of the complex where the element lives
   */
  template<typename ElementTag, typename WrappedConfigType>
  class element_t;



  /** @brief Helper class for defining boundary element information
   *
   * @tparam ElementTag         Tag for the identification of the hosting element
   * @tparam BoundaryElementTag A tag representing the boundary element of the element with ElementTag
   */
  template <typename ElementTag, typename BoundaryElementTag = ElementTag>
  struct boundary_elements
  {
    //the default case is simultaneously a pathetic case:
    //cell-handling within the cell

    /** @brief Number of boundary cells at this level */
    static const int num = 1;     //1 cell

    /** @brief k-cell tag for identification of the type */
    typedef ElementTag            tag;
  };


  /** @brief Namespace for definition and handling of the individual topological elements (triangles, hexahedra, etc.) */
  namespace element_topology
  {
    /** @brief Worker class that generates the boundary elements of a host element
     *
     * @tparam ElementTag           Tag for the identification of the hosting n-cell
     * @tparam BoundaryElementTag   Topological boundary tag
     * @tparam BoundaryElementType  Topological boundary type, used within create_boundary_elements method
     */
    template <typename ElementTag, typename BoundaryElementTag, typename BoundaryElementType>
    struct boundary_element_generator {};

    template<typename ElementTag, typename BoundaryElementType>
    struct boundary_element_generator<ElementTag, vertex_tag, BoundaryElementType>
    {
        template<typename element_type, typename inserter_type>
        static void create_boundary_elements(element_type &, inserter_type &) {}
    };

  }






  /** @brief Config class for domain view */
  template <typename DomainConfigT, typename ElementTypeListT, typename ContainerConfigT>
  class decorated_domain_view_config;

  /** @brief A domain represents a mesh and contains all elements
    *
    * @tparam WrappedConfigT    The configuration of the domain
    */
  template <typename WrappedConfigT>
  class domain_t;

  /** @brief A segmentation defines a set of submeshes of a domain
    *
    * @tparam WrappedConfigT    The configuration of the segmentation
    */
  template<typename WrappedConfigT>
  class segmentation_t;

  /** @brief A segment defines a submesh within a segmentation. Each segmentation contains a view of elements of the domain
    *
    * @tparam SegmentationT     The segmentation type where the segment lives
    */
  template<typename SegmentationT>
  class segment_t;

  template<typename container_type_, typename change_counter_type>
  struct coboundary_container_wrapper;

  template<typename container_type_, typename change_counter_type>
  struct neighbour_container_wrapper;

  template<typename container_type_, typename change_counter_type>
  struct boundary_information_wrapper;






  /** @brief The metafunction layer. Provides type generators that derive the respective type from the domain configuration */
  namespace result_of
  {
    /** @brief Metafunction for obtaining a domain from a config
     *
     * @tparam WrappedConfigT     The configuration of the domain
     */
    template<typename WrappedConfigT>
    struct domain;

    /** @brief Metafunction for obtaining the element collection of something
     *
     * @tparam SomethingT     The host type, can be a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct element_collection;

    /** @brief Metafunction for obtaining the container collection typemap of something
     *
     * @tparam SomethingT     The host type, can be a typelist or a collection
     */
    template<typename SomethingT>
    struct container_collection_typemap;

    /** @brief Metafunction for obtaining the topologic dimension of the cell elements of something
     *
     * @tparam SomethingT           The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct topologic_cell_dimension;
    
    /** @brief Metafunction for obtaining an element typelist of all element with a specific topologic dimension of something
     *
     * @tparam SomethingT           The host type, can be a collection, a domain, a segmentation or a segment
     * @tparam TopologicDimensionV  The topologic dimension
     */
    template<typename SomethingT, int TopologicDimensionV>
    struct elements_of_topologic_dim;

    /** @brief Metafunction for obtaining an element typelist of all cell types of something
     *
     * @tparam SomethingT           The host type, can be a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct cells;

    /** @brief Metafunction for obtaining the cell type of something. Will fail if there is more than one cell type
     *
     * @tparam SomethingT           The host type, can be a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct cell;

    /** @brief Metafunction for obtaining the cell tag of something. Will fail if there is more than one cell type
     *
     * @tparam SomethingT           The host type, can be a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct cell_tag;


    /** @brief Metafunction for obtaining the geometric point type of something
     *
     * @tparam SomethingT           The host type, can be an element, a domain, a segmentation, a segment, an accessor or a field
     */
    template<typename SomethingT>
    struct point;

    /** @brief Metafunction for obtaining the coordinate type of the geometric point type of something
     *
     * @tparam SomethingT           The host type, can be a point, an element, a domain, a segmentation, a segment, an accessor or a field
     */
    template<typename SomethingT>
    struct coord
    {
        typedef typename coord< typename point<SomethingT>::type >::type type;
    };

    /** @brief Metafunction for obtaining the geometric dimension of something
     *
     * @tparam SomethingT           The host type, can be an element, a domain, a segmentation, a segment, an accessor or a field
     */
    template<typename SomethingT>
    struct geometric_dimension
    {
        static const int value = geometric_dimension< typename point<SomethingT>::type >::value;
    };


    /** @brief Metafunction for obtaining the id type of an element
     *
     * @tparam ElementT           The host element type
     */
    template<typename ElementT>
    struct id
    {
        typedef typename ElementT::id_type type;
    };
    

    /** @brief Metafunction for the type retrieval of an element
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     * @tparam ElementTypeOrTagT  The requested element tag, element type is also supported, in this case the requested element type is returned
     */
    template<typename SomethingT, typename ElementTypeOrTagT>
    struct element;

    template<typename element_tag, typename WrappedConfigType>
    struct element< element_t<element_tag, WrappedConfigType>, element_t<element_tag, WrappedConfigType> >
    {
        typedef element_t<element_tag, WrappedConfigType> type;
    };

    template<typename element_tag, typename WrappedConfigType>
    struct element< element_t<element_tag, WrappedConfigType>, element_tag >
    {
        typedef element_t<element_tag, WrappedConfigType> type;
    };

    template<typename SomethingT>
    struct element<SomethingT, viennagrid::meta::null_type>
    {
        typedef viennagrid::meta::null_type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a vertex
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct vertex
    {
        typedef typename element<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line (same as edge)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct line
    {
        typedef typename element<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a edge (same as line)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct edge
    {
        typedef typename element<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a triangle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct triangle
    {
        typedef typename element<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a quadrilateral
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct quadrilateral
    {
        typedef typename element<SomethingT, quadrilateral_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a polygon
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct polygon
    {
        typedef typename element<SomethingT, polygon_tag>::type type;
    };
    
    /** @brief Convenience metafunction for the type retrieval of a plc
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct plc
    {
        typedef typename element<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a tetrahedron
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct tetrahedron
    {
        typedef typename element<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a hexahedron
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct hexahedron
    {
        typedef typename element<SomethingT, hexahedron_tag>::type type;
    };


    /** @brief Metafunction for the type retrieval of an element handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     * @tparam ElementTypeOrTagT  The requested element tag, element type is also supported, in this case the requested element type is returned
     */
    template<typename SomethingT, typename ElementTypeOrTagT>
    struct handle;

    /** @brief Convenience metafunction for the type retrieval of a cell handle. Will fail if there is more than one cell type
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct cell_handle
    {
        typedef typename cell_tag<SomethingT>::type cell_tag;
        typedef typename handle<SomethingT, cell_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a vertex handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct vertex_handle
    {
        typedef typename handle<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line handle (same as edge_handle)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct line_handle
    {
        typedef typename handle<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line handle (same as line_handle)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct edge_handle
    {
        typedef typename handle<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a triangle handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct triangle_handle
    {
        typedef typename handle<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a quadrilateral handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct quadrilateral_handle
    {
        typedef typename handle<SomethingT, quadrilateral_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a polygon handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct polygon_handle
    {
        typedef typename handle<SomethingT, polygon_tag>::type type;
    };
    
    /** @brief Convenience metafunction for the type retrieval of a plc handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct plc_handle
    {
        typedef typename handle<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a tetrahedron handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct tetrahedron_handle
    {
        typedef typename handle<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a hexahedron handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct hexahedron_handle
    {
        typedef typename handle<SomethingT, hexahedron_tag>::type type;
    };



    /** @brief Metafunction for the type retrieval of a const element handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     * @tparam ElementTypeOrTagT  The requested element tag, element type is also supported, in this case the requested element type is returned
     */
    template<typename SomethingT, typename ElementTypeOrTagT>
    struct const_handle;

    /** @brief Convenience metafunction for the type retrieval of a const cell handle. Will fail if there is more than one cell type
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_cell_handle
    {
        typedef typename cell_tag<SomethingT>::type cell_tag;
        typedef typename const_handle<SomethingT, cell_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const vertex handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_vertex_handle
    {
        typedef typename const_handle<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const line handle (same as const_edge_handle)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_line_handle
    {
        typedef typename const_handle<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const line handle (same as const_line_handle)
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_edge_handle
    {
        typedef typename const_handle<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const triangle handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_triangle_handle
    {
        typedef typename const_handle<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const quadrilateral handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_quadrilateral_handle
    {
        typedef typename const_handle<SomethingT, quadrilateral_tag>::type type;
    };
    
    /** @brief Convenience metafunction for the type retrieval of a const polygon handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_polygon_handle
    {
        typedef typename const_handle<SomethingT, polygon_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const plc handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_plc_handle
    {
        typedef typename const_handle<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const tetrahedron handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_tetrahedron_handle
    {
        typedef typename const_handle<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const hexahedron handle
     *
     * @tparam SomethingT         The host type, can be a collection, an element, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_hexahedron_handle
    {
        typedef typename const_handle<SomethingT, hexahedron_tag>::type type;
    };


    
    /** @brief Metafunction for the type retrieval of an element range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, an element, a domain, a segmentation or a segment
     * @tparam ElementTypeOrTagT  The requested element tag or element type
     */
    template <typename SomethingT,
              typename ElementTypeOrTagT>
    struct element_range;


    /** @brief Convenience metafunction for the type retrieval of a cell range. Will fail if there is more than one cell type
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct cell_range
    {
        typedef typename cell_tag<SomethingT>::type cell_tag;
        typedef typename element_range<SomethingT, cell_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a vertex range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct vertex_range
    {
        typedef typename element_range<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a line range (same as edge_range)
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct line_range
    {
        typedef typename element_range<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a edge range (same as line_range)
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct edge_range
    {
        typedef typename element_range<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a triangle range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct triangle_range
    {
        typedef typename element_range<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a quadrilateral range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct quadrilateral_range
    {
        typedef typename element_range<SomethingT, quadrilateral_tag>::type type;
    };
    
    /** @brief Convenience metafunction for the type retrieval of a polygon range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct polygon_range
    {
        typedef typename element_range<SomethingT, polygon_tag>::type type;
    };
    
    /** @brief Convenience metafunction for the type retrieval of a PLC range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct plc_range
    {
        typedef typename element_range<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a tetrahedron range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct tetrahedron_range
    {
        typedef typename element_range<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a hexahedron range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct hexahedron_range
    {
        typedef typename element_range<SomethingT, hexahedron_tag>::type type;
    };






    /** @brief Metafunction for the type retrieval of a const element range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, an element, a domain, a segmentation or a segment
     * @tparam ElementTypeOrTagT  The requested element tag or element type
     */
    template <typename SomethingT,
              typename ElementTypeOrTagT>
    struct const_element_range;


    /** @brief Convenience metafunction for the type retrieval of a const cell range. Will fail if there is more than one cell type
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_cell_range
    {
        typedef typename cell_tag<SomethingT>::type cell_tag;
        typedef typename const_element_range<SomethingT, cell_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const vertex range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_vertex_range
    {
        typedef typename const_element_range<SomethingT, vertex_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const line range (same as const_edge_range)
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_line_range
    {
        typedef typename const_element_range<SomethingT, line_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const edge range (same as const_line_range)
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_edge_range
    {
        typedef typename const_element_range<SomethingT, edge_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const triangle range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_triangle_range
    {
        typedef typename const_element_range<SomethingT, triangle_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const quadrilateral range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_quadrilateral_range
    {
        typedef typename const_element_range<SomethingT, quadrilateral_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const polygon range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_polygon_range
    {
        typedef typename const_element_range<SomethingT, polygon_tag>::type type;
    };
    
    /** @brief Convenience metafunction for the type retrieval of a const PLC range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_plc_range
    {
        typedef typename const_element_range<SomethingT, plc_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const tetrahedron range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_tetrahedron_range
    {
        typedef typename const_element_range<SomethingT, tetrahedron_tag>::type type;
    };

    /** @brief Convenience metafunction for the type retrieval of a const hexahedron range
     *
     * @tparam SomethingT         The host type, can be a typelist, a collection, a domain, a segmentation or a segment
     */
    template<typename SomethingT>
    struct const_hexahedron_range
    {
        typedef typename const_element_range<SomethingT, hexahedron_tag>::type type;
    };




    /** @brief Metafunction for the type retrieval of an iterator from a container or range
     *
     * @tparam ContainerT         The container type, can also be a range
     */
    template <typename ContainerT>
    struct iterator
    {
        typedef typename ContainerT::iterator type;
    };

    /** @brief Metafunction for the type retrieval of a const iterator from a container or range
     *
     * @tparam ContainerT         The container type, can also be a range
     */
    template <typename ContainerT>
    struct const_iterator
    {
        typedef typename ContainerT::const_iterator type;
    };
    
    template <typename ContainerT>
    struct iterator<const ContainerT>
    {
        typedef typename const_iterator<ContainerT>::type type;
    };

    
    
    /** @brief Metafunction for retrieving the tag of an element
     *
     * @tparam ElementTypeOrTagT    The element type of which the tag is requested, can also be a tag. In this case the tag is returned.
     */
    template<typename ElementTypeOrTagT>
    struct element_tag
    {
        typedef ElementTypeOrTagT type;
    };

    template<typename ElementTagT, typename WrappedConfigT>
    struct element_tag< element_t<ElementTagT, WrappedConfigT> >
    {
        typedef ElementTagT type;
    };

    template<typename ElementTagT, typename WrappedConfigT>
    struct element_tag< const element_t<ElementTagT, WrappedConfigT> >
    {
        typedef ElementTagT type;
    };


    /** @brief Metafunction for retrieving the facet tag of an element, a domain, a segmentation or a segment
     *
     * @tparam SomethingT    The element type of which the facet tag is requested. Can also be a domain, a segmentation or a segment. In this case the facet tag of the cell type is returned.
     */
    template<typename SomethingT>
    struct facet_tag
    {
        typedef typename element_tag<SomethingT>::type::facet_tag type;
    };

    template<typename WrappedDomainConfigT>
    struct facet_tag< domain_t<WrappedDomainConfigT> >
    {
        typedef typename facet_tag< typename cell_tag< domain_t<WrappedDomainConfigT> >::type >::type type;
    };

    template<typename WrappedSegmentationConfigT>
    struct facet_tag< segmentation_t<WrappedSegmentationConfigT> >
    {
        typedef typename facet_tag< typename cell_tag< segmentation_t<WrappedSegmentationConfigT> >::type >::type type;
    };

    template<typename SegmentationT>
    struct facet_tag< segment_t<SegmentationT> >
    {
        typedef typename facet_tag< typename cell_tag< segment_t<SegmentationT> >::type >::type type;
    };
    
    
    

    /** @brief Metafunction for retrieving the facet element type an element, a domain, a segmentation or a segment
     *
     * @tparam SomethingT    The element type of which the facet type is requested. Can also be a domain, a segmentation or a segment. In this case the facet type of the cell type is returned.
     */
    template<typename SomethingT>
    struct facet
    {
        typedef typename element<SomethingT, typename facet_tag<SomethingT>::type >::type type;
    };

    template<typename WrappedDomainConfigT>
    struct facet< domain_t<WrappedDomainConfigT> >
    {
        typedef typename facet< typename cell< domain_t<WrappedDomainConfigT> >::type >::type type;
    };

    template<typename WrappedSegmentationConfigT>
    struct facet< segmentation_t<WrappedSegmentationConfigT> >
    {
        typedef typename facet< typename cell< segmentation_t<WrappedSegmentationConfigT> >::type >::type type;
    };

    template<typename SegmentationT>
    struct facet< segment_t<SegmentationT> >
    {
        typedef typename facet< typename cell< segment_t<SegmentationT> >::type >::type type;
    };



    /** @brief Metafunction for retrieving the facet handle type an element, a domain, a segmentation or a segment
     *
     * @tparam SomethingT    The element type of which the facet handle type is requested. Can also be a domain, a segmentation or a segment. In this case the facet handle type of the cell type is returned.
     */
    template<typename SomethingT>
    struct facet_handle
    {
        typedef typename handle< SomethingT, typename facet_tag<SomethingT>::type >::type type;
    };

    /** @brief Metafunction for retrieving the const facet handle type an element, a domain, a segmentation or a segment
     *
     * @tparam SomethingT    The element type of which the const facet handle type is requested. Can also be a domain, a segmentation or a segment. In this case the const facet handle type of the cell type is returned.
     */
    template<typename SomethingT>
    struct const_facet_handle
    {
        typedef typename const_handle< SomethingT, typename facet_tag<SomethingT>::type >::type type;
    };


    /** @brief Metafunction for retrieving the facet range type an element, a domain, a segmentation or a segment
     *
     * @tparam SomethingT    The element type of which the facet range type is requested. Can also be a domain, a segmentation or a segment. In this case the facet range type of the cell type is returned.
     */
    template<typename SomethingT>
    struct facet_range
    {
        typedef typename element_range<SomethingT, typename facet_tag<SomethingT>::type >::type type;
    };

    /** @brief Metafunction for retrieving the const facet range type an element, a domain, a segmentation or a segment
     *
     * @tparam SomethingT    The element type of which the const facet range type is requested. Can also be a domain, a segmentation or a segment. In this case the const facet range type of the cell type is returned.
     */
    template<typename SomethingT>
    struct const_facet_range
    {
        typedef typename const_element_range<SomethingT, typename facet_tag<SomethingT>::type >::type type;
    };
  }



  /** @brief Proxy object for element or boundary element range. This object holds a reference to the host type. A range has a constructor taking an object of type element_range_proxy and elements<tag> will be called in te constructor using the reference to the host type.
    *
    * @tparam SomethingT    The host type, can be an element, a collection, a domain, a segment or a segmentation
    */
  template<typename SomethingT>
  struct element_range_proxy
  {
  public:
    /** @brief Constructor of element_range_proxy, taking the reference to the host object
    *
    * @param  something_          The reference to the host object
    */
    element_range_proxy(SomethingT & something_) : something(something_) {}

    /** @brief Returns the reference to the host object
     * 
     * @return a reference to the host object
    */
    SomethingT & operator() () { return something; }

  private:
    SomethingT & something;
  };
  
  
  
  
  /** @brief Function for retrieving an element range or a boundary element range from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @tparam ElementTypeOrTagT  The element type/tag for the requested element range
    * @param  something          The host object of type SomethingT
    * @return                    An element range
    */
  template<typename ElementTypeOrTagT, typename SomethingT>
  typename result_of::element_range<SomethingT, ElementTypeOrTagT>::type elements(SomethingT & something);
  
  template<typename ElementTypeOrTagT, typename SegmentationT>
  typename result_of::element_range< segment_t<SegmentationT>, ElementTypeOrTagT>::type elements( segment_t<SegmentationT> & something);

  /** @brief Function for retrieving an element range proxy object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    An element range proxy, can be assigned to a range object
    */
  template<typename SomethingT>
  element_range_proxy<SomethingT> elements(SomethingT & something)
  { return element_range_proxy<SomethingT>(something); }
  



  /** @brief Function for retrieving a cell range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A cell range
    */
  template<typename SomethingT>
  typename result_of::cell_range<SomethingT>::type cells(SomethingT & something)
  { return elements<typename result_of::cell_tag<SomethingT>::type>(something); }
  
  /** @brief Function for retrieving a facet range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A facet range
    */
  template<typename element_type>
  typename result_of::facet_range<element_type>::type facets(element_type & element)
  { return elements< typename result_of::facet_tag<element_type>::type >(element); }

  /** @brief Function for retrieving a vertex range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A vertex range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, vertex_tag>::type vertices(SomethingT & something)
  { return elements<vertex_tag>(something); }

  /** @brief Function for retrieving a line range object from a host object (same as edges)
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A line range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, line_tag>::type lines(SomethingT & something)
  { return elements<line_tag>(something); }

  /** @brief Function for retrieving a edge range object from a host object (same as lines)
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    An edge range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, edge_tag>::type edges(SomethingT & something)
  { return elements<edge_tag>(something); }

  /** @brief Function for retrieving a triangle range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A triangle range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, triangle_tag>::type triangles(SomethingT & something)
  { return elements<triangle_tag>(something); }

  /** @brief Function for retrieving a quadrilaterals range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A quadrilateral range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, quadrilateral_tag>::type quadrilaterals(SomethingT & something)
  { return elements<quadrilateral_tag>(something); }

  /** @brief Function for retrieving a polygon range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A polygon range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, polygon_tag>::type polygons(SomethingT & something)
  { return elements<polygon_tag>(something); }
  
  /** @brief Function for retrieving a PLC range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A PLC range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, plc_tag>::type plcs(SomethingT & something)
  { return elements<plc_tag>(something); }

  /** @brief Function for retrieving a tetrahedron range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A tetrahedron range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, tetrahedron_tag>::type tetraheda(SomethingT & something)
  { return elements<tetrahedron_tag>(something); }

  /** @brief Function for retrieving a hexahedron range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A hexahedron range
    */
  template<typename SomethingT>
  typename result_of::element_range<SomethingT, hexahedron_tag>::type hexahedra(SomethingT & something)
  { return elements<hexahedron_tag>(something); }



  /** @brief Function for retrieving a const element range or a boundary element range from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @tparam ElementTypeOrTagT  The element type/tag for the requested element range
    * @param  something          The host object of type SomethingT
    * @return                    A const element range
    */
  template<typename ElementTypeOrTagT, typename SomethingT>
  typename result_of::const_element_range<SomethingT, ElementTypeOrTagT>::type elements(SomethingT const & something);
  
  template<typename ElementTypeOrTagT, typename SegmentationT>
  typename result_of::const_element_range< segment_t<SegmentationT>, ElementTypeOrTagT>::type elements( segment_t<SegmentationT> const & something);


  
  /** @brief Function for retrieving an element range proxy object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    An element range proxy, can be assigned to a const range object
    */
  template<typename SomethingT>
  element_range_proxy<const SomethingT> elements(SomethingT const & something)
  { return element_range_proxy<const SomethingT>(something); }


  /** @brief Function for retrieving a const cell range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const cell range
    */
  template<typename SomethingT>
  typename result_of::const_cell_range<SomethingT>::type cells( SomethingT const & something)
  { return elements<typename result_of::cell_tag<SomethingT>::type>(something); }
  
  /** @brief Function for retrieving a const facet range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const facet range
    */
  template<typename element_type>
  typename result_of::const_facet_range<element_type>::type facets(const element_type & element)
  { return elements< typename result_of::facet_tag<element_type>::type >(element); }

  /** @brief Function for retrieving a const vertex range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const vertex range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, vertex_tag>::type vertices(SomethingT const & something)
  { return elements<vertex_tag>(something); }

  /** @brief Function for retrieving a const line range object from a host object (same as edges)
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const line range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, line_tag>::type lines(SomethingT const & something)
  { return elements<line_tag>(something); }

  /** @brief Function for retrieving a const edge range object from a host object (same as lines)
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const edge range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, edge_tag>::type edges(SomethingT const & something)
  { return elements<edge_tag>(something); }

  /** @brief Function for retrieving a const triangles range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const triangle range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, triangle_tag>::type triangles(SomethingT const & something)
  { return elements<triangle_tag>(something); }

  /** @brief Function for retrieving a const quadrilateral range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const quadrilateral range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, quadrilateral_tag>::type quadrilaterals(SomethingT const & something)
  { return elements<quadrilateral_tag>(something); }

  /** @brief Function for retrieving a const polygon range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const polygon range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, polygon_tag>::type polygons(SomethingT const & something)
  { return elements<polygon_tag>(something); }
  
  /** @brief Function for retrieving a const PLC range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const PLC range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, plc_tag>::type plcs(SomethingT const & something)
  { return elements<plc_tag>(something); }

  /** @brief Function for retrieving a const tetrahedron range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const tetrahedron range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, tetrahedron_tag>::type tetraheda(SomethingT const & something)
  { return elements<tetrahedron_tag>(something); }

  /** @brief Function for retrieving a const hexahedron range object from a host object
    *
    * @tparam SomethingT         The host type, can be an element, a collection, a domain, a segment or a segmentation
    * @param  something          The host object of type SomethingT
    * @return                    A const hexahedron range
    */
  template<typename SomethingT>
  typename result_of::const_element_range<SomethingT, hexahedron_tag>::type hexahedra(SomethingT const & something)
  { return elements<hexahedron_tag>(something); }


  
  
  /** @brief Function for creating an element within a domain or a segment
    *
    * @tparam ElementTypeOrTagT       The element type/tag to be created
    * @tparam DomainOrSegmentT        The domain or segment type where the element is created
    * @tparam VertexHandleIteratorT   A vertex handle iterator type
    * @param  domain_segment          The domain or segment object where the element should be created
    * @param  vertices_begin          An iterator pointing to the first vertex handle of the element
    * @param  vertices_end            An iterator defining the end of the vertices
    * @return                         A handle to the created element
    */
  template<typename ElementTypeOrTagT, typename DomainOrSegmentT, typename VertexHandleIteratorT>
  typename result_of::handle<DomainOrSegmentT, ElementTypeOrTagT>::type make_element(
        DomainOrSegmentT & domain_segment,
        VertexHandleIteratorT array_start,
        VertexHandleIteratorT const & array_end );

  
  /** @brief Function for creating an element within a domain or segment with a specific ID
    *
    * @tparam ElementTypeOrTagT       The element type/tag to be created
    * @tparam DomainOrSegmentT        The domain or segment type where the element is created
    * @tparam VertexHandleIteratorT   A vertex handle iterator type
    * @param  domain_segment          The domain or segment object where the element should be created
    * @param  vertices_begin          An iterator pointing to the first vertex handle of the element
    * @param  vertices_end            An iterator defining the end of the vertices
    * @param  id                      The id of the new element
    * @return                         A handle to the created element
    */
#ifndef _MSC_VER
  template<typename ElementTypeOrTagT, typename DomainOrSegmentT, typename VertexHandleIteratorT>
  typename result_of::handle<DomainOrSegmentT, ElementTypeOrTagT>::type make_element_with_id(
        DomainOrSegmentT & domain_segment,
        VertexHandleIteratorT array_start,
        VertexHandleIteratorT const & array_end,
        typename result_of::id< typename result_of::element<DomainOrSegmentT, ElementTypeOrTagT>::type >::type id );
#endif


  /** @brief Function for dereferencing an element -> identity
    *
    * @tparam WrappedDomainConfigT    Wrapped config of the domain type
    * @tparam ElementTagT             Element tag of the element to dereference
    * @tparam WrappedConfigT          Wrapped config of the element to dereference
    * @param  element                 The element to be derferenced, this object is returned
    * @return                         element (identity)
    */
  template<typename WrappedDomainConfigT, typename ElementTagT, typename WrappedConfigT>
  element_t<ElementTagT, WrappedConfigT> & dereference_handle( domain_t<WrappedDomainConfigT> &, element_t<ElementTagT, WrappedConfigT> & element)
  { return element; }
  
  template<typename HostElementTagT, typename WrappedHostElementConfigT, typename ElementTagT, typename WrappedConfigT>
  element_t<ElementTagT, WrappedConfigT> & dereference_handle( element_t<HostElementTagT, WrappedHostElementConfigT> &, element_t<ElementTagT, WrappedConfigT> & element)
  { return element; }

  /** @brief Function for dereferencing a const element -> identity
    *
    * @tparam WrappedDomainConfigT    Wrapped config of the domain type
    * @tparam ElementTagT             Element tag of the element to dereference
    * @tparam WrappedConfigT          Wrapped config of the element to dereference
    * @param  element                 The element to be derferenced, this object is returned
    * @return                         element (identity)
    */
  template<typename WrappedDomainConfigT, typename ElementTagT, typename WrappedConfigT>
  element_t<ElementTagT, WrappedConfigT> const & dereference_handle( domain_t<WrappedDomainConfigT> const &, element_t<ElementTagT, WrappedConfigT> const & element)
  { return element; }

  template<typename HostElementTagT, typename WrappedHostElementConfigT, typename ElementTagT, typename WrappedConfigT>
  element_t<ElementTagT, WrappedConfigT> const & dereference_handle( element_t<HostElementTagT, WrappedHostElementConfigT> const &, element_t<ElementTagT, WrappedConfigT> const & element)
  { return element; }

  
  /** @brief Function for creating a handle, default implementation -> identity
    *
    * @tparam DomainOrSegmentT    Wrapped config of the domain type
    * @tparam HandleT             Type of the handle
    * @param  handle              The handle object
    * @return                     handle (identity)
    */
  template<typename DomainOrSegmentT, typename HandleT>
  HandleT handle( DomainOrSegmentT &, HandleT handle) { return handle; }

  /** @brief Function for creating a const handle, default implementation -> identity
    *
    * @tparam DomainOrSegmentT    Wrapped config of the domain type
    * @tparam HandleT             Type of the handle
    * @param  handle              The handle object
    * @return                     handle (identity)
    */
  template<typename DomainOrSegmentT, typename HandleT>
  const HandleT handle( DomainOrSegmentT const &, HandleT handle) { return handle; }
  

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


  /** @brief Helper struct for element refinement */
  template <typename ElementTagT>
  struct element_refinement;
}

#endif
