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

#include "viennagridpp/forwards.hpp"
#include "viennagridpp/utils.hpp"
#include "viennagridpp/mesh/mesh.hpp"
#include "viennagridpp/mesh/element_creation.hpp"
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
  template<typename NumericConfigT>
  class element_copy_map
  {
  public:

    typedef viennagrid::const_mesh_hierarchy_t SrcMeshHierarchyType;
    typedef viennagrid::mesh_t DstMeshType;

    typedef typename viennagrid::result_of::coord<DstMeshType>::type DstNumericType;
    typedef typename viennagrid::result_of::element<DstMeshType>::type DstElementType;
    typedef typename viennagrid::result_of::element_id<SrcMeshHierarchyType>::type SrcElementIDType;

    /** @brief The constructor, requires the destination mesh where the elements are copied to.
      *
      * @param  dst_mesh_                The destination mesh
      */
    element_copy_map( DstMeshType const & dst_mesh_in, bool copy_region_information_in = true ) : copy_region_information_(copy_region_information_in), dst_mesh_(dst_mesh_in), nc_(-1.0) {}
    element_copy_map( DstMeshType const & dst_mesh_in, NumericConfigT nc_in, bool copy_region_information_in = true ) : copy_region_information_(copy_region_information_in), dst_mesh_(dst_mesh_in), nc_(nc_in) {}

    /** @brief Copies one vertex to the destination mesh. If the vertex is already present in the destination mesh, the vertex handle of this vertex is return, otherwise a new vertex is created in the destination mesh.
      *
      * @param  src_vertex              The vertex to be copied
      */
    template<bool element_is_const>
    DstElementType operator()( base_element<element_is_const> const & src )
    {
      if (src.mesh_hierarchy() == dst_mesh().mesh_hierarchy())
      {
        viennagrid::add( dst_mesh(), src );
        return src;
      }

      if (src.is_vertex())
        return copy_vertex(src);
      return copy_element(src);
    }


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
        DstElementType vtx = viennagrid::make_unique_vertex(dst_mesh(), viennagrid::get_point(src), nc_);
        vertex_map[src.id()] = vtx;
        if (copy_region_information_)
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

      DstElementType dst = viennagrid::make_element( dst_mesh(), src.tag(), dst_vertices.begin(), dst_vertices.end() );
      if (copy_region_information_)
        copy_region_information(src, dst);
      return dst;
    }

    DstMeshType const & dst_mesh() const { return dst_mesh_; }

  private:

    bool copy_region_information_;
    DstMeshType const & dst_mesh_;
    std::map<SrcElementIDType, DstElementType> vertex_map;

    NumericConfigT nc_;
  };




  namespace result_of
  {
    template<typename NumericConfigT = viennagrid_numeric>
    struct element_copy_map
    {
      typedef viennagrid::element_copy_map<NumericConfigT> type;
    };
  }





  /** @brief Copies the cells of a mesh if a boolean functor is true.
    *
    * @param  copy_map                A vertex copy map used for copying
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    * @param  functor                 Boolean functor, if functor(cell) returns true, the cell is copied.
    */
  template<typename SrcMeshType, typename NumericConfigT, typename ToCopyFunctorT>
  void copy(element_copy_map<NumericConfigT> & copy_map,
            SrcMeshType const & src_mesh, viennagrid::mesh_t const & dst_mesh,
            ToCopyFunctorT functor)
  {
    clear(dst_mesh);

    typedef typename viennagrid::result_of::const_cell_range<SrcMeshType>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    ConstCellRangeType cells(src_mesh);
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( functor(*cit) )
        copy_map(*cit );
    }
  }

  /** @brief Copies the cells of a mesh if a boolean functor is true.
    *
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    * @param  functor                 Boolean functor, if functor(cell) returns true, the cell is copied.
    */
  template<typename SrcMeshType, typename ToCopyFunctorT>
  void copy(SrcMeshType const & src_mesh, viennagrid::mesh_t const & dst_mesh, ToCopyFunctorT functor)
  {
    typedef viennagrid::result_of::coord<viennagrid::const_mesh_t>::type NumericType;
    viennagrid::result_of::element_copy_map<NumericType>::type copy_map(dst_mesh);
    copy(copy_map, src_mesh, dst_mesh, functor);
  }

  /** @brief Copies the cells of a mesh
    *
    * @param  src_mesh                The source mesh
    * @param  dst_mesh                The destination mesh
    */
  template<typename SrcMeshType>
  void copy(SrcMeshType const & src_mesh, viennagrid::mesh_t const & dst_mesh)
  {
    clear(dst_mesh);
    typedef viennagrid::result_of::coord<viennagrid::const_mesh_t>::type NumericType;
    viennagrid::result_of::element_copy_map<NumericType>::type copy_map(dst_mesh);

    typedef typename viennagrid::result_of::const_vertex_range<SrcMeshType>::type ConstVertexRangeType;
    typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

    ConstVertexRangeType vertices(src_mesh);
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
      copy_map(*vit);

    typedef typename viennagrid::result_of::const_cell_range<SrcMeshType>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    ConstCellRangeType cells(src_mesh);
    for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
        copy_map(*cit );
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
    typedef viennagrid::result_of::coord<viennagrid::const_mesh_t>::type NumericType;
    viennagrid::result_of::element_copy_map<NumericType>::type copy_map(mesh);
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
    typedef typename viennagrid::result_of::coord<ElementType>::type NumericType;
    typename viennagrid::result_of::element_copy_map<NumericType>::type copy_map(mesh);

    copy_elements(it, end, copy_map);
  }



  template<typename SomethingT, typename ElementCopyMapT>
  void copy_elements(element_tag et, SomethingT something, ElementCopyMapT & copy_map)
  {
    typedef typename viennagrid::result_of::const_element_range<SomethingT>::type ConstElementRangeType;
    ConstElementRangeType elements(something, et);
    copy_elements(elements.begin(), elements.end(), copy_map);
  }

  template<typename SomethingT>
  void copy_elements(element_tag et, SomethingT something, mesh_t mesh)
  {
    typedef typename viennagrid::result_of::element<SomethingT>::type ElementType;
    typedef typename viennagrid::result_of::coord<ElementType>::type NumericType;
    typename viennagrid::result_of::element_copy_map<NumericType>::type copy_map(mesh);

    copy_elements(et, something, copy_map);
  }

}

#endif
