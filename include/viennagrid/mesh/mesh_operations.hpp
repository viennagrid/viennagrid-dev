#ifndef VIENNAGRID_MESH_OPERATIONS_HPP
#define VIENNAGRID_MESH_OPERATIONS_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/forwards.hpp"
#include "viennagrid/utils.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element_creation.hpp"
// #include "viennagrid/mesh/coboundary_iteration.hpp"


/** @file viennagrid/mesh/mesh_operations.hpp
    @brief Helper routines for meshes
*/

namespace viennagrid
{

  /** @brief A helper class for element copy operation between two differen meshes.
    *
    * @tparam SrcMeshT      The mesh type of the source mesh
    * @tparam DstMeshT      The mesh type of the destination mesh
    */
  template<typename SrcMeshHierarchyT,
           typename DstMeshT,
           typename NumericConfigT>
  class element_copy_map
  {
  public:

    typedef typename viennagrid::result_of::coord<DstMeshT>::type DstNumericType;
    typedef typename viennagrid::result_of::element<DstMeshT>::type DstElementType;
    typedef typename viennagrid::result_of::element_id<SrcMeshHierarchyT>::type SrcElementIDType;

    /** @brief The constructor, requires the destination mesh where the elements are copied to.
      *
      * @param  dst_mesh_                The destination mesh
      */
    element_copy_map( DstMeshT & dst_mesh_ ) : dst_mesh(dst_mesh_), nc_(-1.0) {}
    element_copy_map( DstMeshT & dst_mesh_, NumericConfigT nc_in ) : dst_mesh(dst_mesh_), nc_(nc_in) {}

    /** @brief Copies one vertex to the destination mesh. If the vertex is already present in the destination mesh, the vertex handle of this vertex is return, otherwise a new vertex is created in the destination mesh.
      *
      * @param  src_vertex              The vertex to be copied
      */
    template<bool element_is_const>
    DstElementType operator()( base_element<element_is_const> const & src )
    {
      if (src.mesh_hierarchy() == dst_mesh.mesh_hierarchy())
      {
        viennagrid::add( dst_mesh, src );
        return src;
      }

      if (src.tag().is_vertex())
        return copy_vertex(src);
      return copy_element(src);
    }

//     /** @brief Copies one vertex to the destination mesh. If the vertex is already present in the destination mesh, the vertex handle of this vertex is return, otherwise a new vertex is created in the destination mesh.
//       *
//       * @param  src_vertex              The vertex to be copied
//       * @param  tol                     A geometric tolerance for vertex creation
//       */
//     template<typename OtherNumericConfigT>
//     DstVertexType operator()( SrcVertexType const & src_vertex, OtherNumericConfigT nc )
//     {
//       typename std::map<SrcVertexIDType, DstVertexType>::iterator vit = vertex_map.find( src_vertex.id() );
//       if (vit != vertex_map.end())
//         return vit->second;
//       else
//       {
//         DstVertexType vtx = viennagrid::make_unique_vertex(dst_mesh,
//                                                            viennagrid::get_point(src_vertex),
//                                                            nc);
//         vertex_map[src_vertex.id()] = vtx;
//         copy_region_information(src_vertex, vtx);
//
//         return vtx;
//       }
//     }




    /** @brief Copies a whole element including its vertices to the destination mesh.
      *
      * @param  el                      The element to be copied
      */
    template<bool element_is_const>
    DstElementType copy_vertex( base_element<element_is_const> const & src )
    {
      typename std::map<SrcElementIDType, DstElementType>::iterator vit = vertex_map.find( src.id() );
      if (vit != vertex_map.end())
        return vit->second;
      else
      {
        DstElementType vtx = viennagrid::make_unique_vertex(dst_mesh,
                                                           viennagrid::get_point(src),
                                                           nc_);
        vertex_map[src.id()] = vtx;
        copy_region_information(src, vtx);

        return vtx;
      }
    }


    /** @brief Copies a whole element including its vertices to the destination mesh.
      *
      * @param  el                      The element to be copied
      */
    template<bool element_is_const>
    DstElementType copy_element( base_element<element_is_const> const & src )
    {
      typedef base_element<element_is_const> SrcElementType;
      typedef typename viennagrid::result_of::const_element_range<SrcElementType>::type ConstVerticesOnElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVerticesOnElementRangeType>::type ConstVerticesOnElementIteratorType;

      std::vector<DstElementType> dst_vertices;
      ConstVerticesOnElementRangeType vertices(src, 0);
      for (ConstVerticesOnElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        dst_vertices.push_back( (*this)(*vit) );

      DstElementType dst = viennagrid::make_element( dst_mesh, src.tag(), dst_vertices.begin(), dst_vertices.end() );
      copy_region_information(src, dst);
      return dst;
    }

//     /** @brief Copies a whole element including its vertices to the destination mesh.
//       *
//       * @param  el                      The element to be copied
//       * @param  tol                     A geometric tolerance for vertex creation
//       */
//     template<bool element_is_const, typename OtherNumericConfigT>
//     typename viennagrid::result_of::element<DstMeshT>::type copy_element( base_element<element_is_const> src, OtherNumericConfigT nc )
//     {
//       typedef base_element<element_is_const> SrcElementType;
//       typedef typename viennagrid::result_of::element<DstMeshT>::type DstElementType;
//       typedef typename viennagrid::result_of::const_vertex_range<SrcElementType>::type ConstVerticesOnElementRangeType;
//       typedef typename viennagrid::result_of::iterator<ConstVerticesOnElementRangeType>::type ConstVerticesOnElementIteratorType;
//
//       std::vector<DstVertexType> dst_vertices;
//       ConstVerticesOnElementRangeType vertices(src);
//       for (ConstVerticesOnElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
//         dst_vertices.push_back( (*this)(*vit, nc) );
//
//       DstElementType dst = viennagrid::make_element( dst_mesh, src.tag(), dst_vertices.begin(), dst_vertices.end() );
//       copy_region_information(src, dst);
//       return dst;
//     }



  private:

    template<typename SrcElementT, typename DstElementT>
    void copy_region_information(SrcElementT src, DstElementT dst)
    {
      typedef typename viennagrid::result_of::region_range<SrcMeshHierarchyT, SrcElementT>::type ElementRegionType;
      typedef typename viennagrid::result_of::iterator<ElementRegionType>::type ElementRegionIterator;

      ElementRegionType regions(src);
      for (ElementRegionIterator rit = regions.begin(); rit != regions.end(); ++rit)
        viennagrid::add( dst_mesh.get_make_region((*rit).id()), dst );
    }


    DstMeshT & dst_mesh;
    std::map<SrcElementIDType, DstElementType> vertex_map;

    NumericConfigT nc_;
  };




  namespace result_of
  {
    template<typename SrcT, typename DstT, typename NumericConfigT = typename viennagrid::result_of::coord<SrcT>::type>
    struct element_copy_map
    {
      typedef viennagrid::element_copy_map<
        typename viennagrid::result_of::mesh_hierarchy<SrcT>::type,
        typename viennagrid::result_of::mesh<SrcT>::type,
        NumericConfigT> type;
    };


    template<bool mesh_hierarchy_is_const, typename NumericConfigT>
    struct element_copy_map<base_mesh_hierarchy<mesh_hierarchy_is_const>, base_mesh<true>, NumericConfigT>
    {
      typedef viennagrid::element_copy_map< base_mesh_hierarchy<mesh_hierarchy_is_const>, base_mesh<true>, NumericConfigT > type;
    };
  }





  /** @brief Copies the cells of a mesh if a boolean functor is true.
    *
    * @param  vertex_map              A vertex copy map used for copying
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    * @param  functor                 Boolean functor, if functor(cell) returns true, the cell is copied.
    */
  template<typename SrcMeshT, typename DstMeshT, typename NumericConfigT, typename ToCopyFunctorT>
  void copy(element_copy_map<SrcMeshT, DstMeshT, NumericConfigT> & vertex_map,
            SrcMeshT const & src_mesh, DstMeshT & dst_mesh,
            ToCopyFunctorT functor)
  {
    dst_mesh.clear();

    typedef typename viennagrid::result_of::const_element_range<SrcMeshT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;

    for (element_tag_t cell_tag = cell_tag_begin(src_mesh); cell_tag != cell_tag_end(src_mesh); ++cell_tag)
    {
      ConstElementRangeType cells(src_mesh, cell_tag);
      for (ConstElementRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        if ( functor(*cit) )
          vertex_map.copy_element(*cit );
      }
    }
  }

  /** @brief Copies the cells of a mesh if a boolean functor is true.
    *
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    * @param  functor                 Boolean functor, if functor(cell) returns true, the cell is copied.
    */
  template<typename SrcMeshT, typename DstMeshT, typename ToCopyFunctorT>
  void copy(SrcMeshT const & src_mesh, DstMeshT & dst_mesh, ToCopyFunctorT functor)
  {
    viennagrid::result_of::element_copy_map<SrcMeshT, DstMeshT> vertex_map(dst_mesh);
    copy(vertex_map, src_mesh, dst_mesh, functor);
  }

  /** @brief Copies the cells of a mesh
    *
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    */
  template<typename SrcMeshT, typename DstMeshT>
  void copy(SrcMeshT const & src_mesh, DstMeshT & dst_mesh)
  {
    copy(src_mesh, dst_mesh, viennagrid::true_functor());
  }









  // doxygen doku in forwards.hpp
  template<typename ElementT, typename ElementCopyMapT>
  typename ElementCopyMapT::DstElementType copy_element(ElementT const & element,
                                                        ElementCopyMapT & copy_map)
  {
    return copy_map(element);
  }

  // doxygen doku in forwards.hpp
  template<typename ElementT>
  viennagrid::result_of::element<mesh_t>::type copy_element(ElementT const & element, mesh_t mesh)
  {
    typename viennagrid::result_of::element_copy_map<
      typename viennagrid::result_of::mesh_hierarchy<ElementT>::type,
      mesh_t
    >::type copy_map(mesh);
    return copy_element(element, copy_map);
  }


  template<typename IteratorT, typename ElementCopyMapT>
  void copy_elements(IteratorT it, IteratorT end, ElementCopyMapT & copy_map)
  {
    for (; it != end; ++it)
      copy_element(*it, copy_map);
  }

  template<typename IteratorT>
  void copy_elements(IteratorT it, IteratorT end, mesh_t mesh)
  {
    typedef typename std::iterator_traits<IteratorT>::value_type ElementType;
    typename viennagrid::result_of::element_copy_map<
      typename viennagrid::result_of::mesh_hierarchy<ElementType>::type,
      mesh_t
    >::type copy_map(mesh);

    copy_elements(it, end, copy_map);
  }



  template<typename SomethingT, typename ElementCopyMapT>
  void copy_elements(element_tag_t element_tag, SomethingT something, ElementCopyMapT & copy_map)
  {
    typedef typename viennagrid::result_of::const_element_range<SomethingT>::type ConstElementRangeType;
    ConstElementRangeType elements(something, element_tag);
    copy_elements(elements.begin(), elements.end(), copy_map);
  }

  template<typename SomethingT>
  void copy_elements(element_tag_t element_tag, SomethingT something, mesh_t mesh)
  {
    typedef typename viennagrid::result_of::element<SomethingT>::type ElementType;
    typename viennagrid::result_of::element_copy_map<
      typename viennagrid::result_of::mesh_hierarchy<ElementType>::type,
      mesh_t
    >::type copy_map(mesh);

    copy_elements(element_tag, something, copy_map);
  }

}

#endif
