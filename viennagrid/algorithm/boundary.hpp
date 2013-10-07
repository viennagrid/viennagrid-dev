#ifndef VIENNAGRID_ALGORITHM_BOUNDARY_HPP
#define VIENNAGRID_ALGORITHM_BOUNDARY_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <vector>
#include "viennagrid/forwards.hpp"
#include "viennagrid/element/element.hpp"
#include "viennagrid/mesh/accessor.hpp"
#include "viennagrid/mesh/segmentation.hpp"


/** @file boundary.hpp
    @brief Contains functions for detecting and querying boundary information
*/


namespace viennagrid
{

  /** @brief For internal use only. */
  template <typename MeshType, typename AccessorType>
  void detect_boundary(MeshType & mesh, AccessorType boundary_info_accessor)
  {
//     std::cout << "DETECT BOUNDARY" << std::endl;

    typedef typename viennagrid::result_of::cell_tag<MeshType>::type CellTag;
    typedef typename viennagrid::result_of::facet_tag<CellTag>::type FacetTag;

    typedef typename viennagrid::result_of::element<MeshType, CellTag>::type     CellType;

    typedef typename viennagrid::result_of::element_range<MeshType, FacetTag>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;

    typedef typename viennagrid::result_of::element_range<MeshType, CellTag>::type        CellRange;
    typedef typename viennagrid::result_of::iterator<CellRange>::type                                            CellIterator;

    typedef typename viennagrid::result_of::element_range<CellType, FacetTag>::type     FacetOnCellRange;
    typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                           FacetOnCellIterator;


    FacetRange facets = viennagrid::elements(mesh);

    for (FacetIterator fit = facets.begin();
          fit != facets.end();
          ++fit)
        boundary_info_accessor(*fit) = false;

    //iterate over all cells, over facets there and tag them:
    CellRange cells = viennagrid::elements(mesh);
    for (CellIterator cit = cells.begin();
          cit != cells.end();
          ++cit)
    {
      FacetOnCellRange facets_on_cell = elements(*cit);
      for (FacetOnCellIterator focit = facets_on_cell.begin();
            focit != facets_on_cell.end();
            ++focit)
            boundary_info_accessor(*focit) = !boundary_info_accessor(*focit);
    }
  }



  /** @brief For internal use only. */
  template <typename MeshType, typename SourceAccessorType, typename DestinationAccessorType>
  void transfer_boundary_information(MeshType const & mesh,
                       SourceAccessorType const source_boundary_info_accessor,
                       DestinationAccessorType destination_boundary_info_accessor
                      )
  {
    typedef typename SourceAccessorType::access_type src_element_type;
    typedef typename DestinationAccessorType::access_type dst_element_type;

    typedef typename viennagrid::result_of::const_element_range< MeshType, dst_element_type >::type dst_range_type;
    typedef typename viennagrid::result_of::iterator< dst_range_type >::type dst_range_iterator;

    dst_range_type dst_elements = viennagrid::elements( mesh );

    for (dst_range_iterator it = dst_elements.begin(); it != dst_elements.end(); ++it)
        destination_boundary_info_accessor(*it) = false;


    typedef typename viennagrid::result_of::const_element_range< MeshType, src_element_type >::type src_range_type;
    typedef typename viennagrid::result_of::iterator< src_range_type >::type src_range_iterator;

    src_range_type src_elements = viennagrid::elements( mesh );


    for (src_range_iterator fit = src_elements.begin();
        fit != src_elements.end();
        ++fit)
    {
      if ( source_boundary_info_accessor(*fit) )
      {
        typedef typename viennagrid::result_of::const_element_range< src_element_type, dst_element_type >::type dst_on_src_range_type;
        typedef typename viennagrid::result_of::iterator< dst_on_src_range_type >::type dst_on_src_range_iterator;

        dst_on_src_range_type dst_on_src_range = viennagrid::elements(*fit);
        for (dst_on_src_range_iterator dosit = dst_on_src_range.begin(); dosit != dst_on_src_range.end(); ++dosit)
            destination_boundary_info_accessor(*dosit) = true;
      }
    }
  }




  /** @brief For internal use only. */
  template<typename mesh_type>
  class boundary_setter_functor
  {
  public:
      boundary_setter_functor(mesh_type & mesh_) : mesh(mesh_) {}

      template<typename something>
      void operator()( viennagrid::meta::tag<something> )
      {
          typedef typename viennagrid::result_of::element_tag< something >::type element_tag;
          typedef typename viennagrid::result_of::element< mesh_type, element_tag >::type element_type;

          typedef typename viennagrid::result_of::cell_tag< mesh_type >::type cell_tag;
          typedef typename viennagrid::result_of::facet_tag< cell_tag >::type facet_tag;
          typedef typename viennagrid::result_of::element< mesh_type, facet_tag >::type facet_type;



        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename mesh_type::appendix_type,
                    boundary_information_collection_tag
                  >::type,
                  facet_tag
                >::type src_boundary_information_container_wrapper_type;

          src_boundary_information_container_wrapper_type & src_boundary_information_container_wrapper = boundary_information_collection<facet_tag>( mesh );


        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename mesh_type::appendix_type,
                    boundary_information_collection_tag
                  >::type,
                  element_tag
                >::type dst_boundary_information_container_wrapper_type;

          dst_boundary_information_container_wrapper_type & dst_boundary_information_container_wrapper = boundary_information_collection<element_tag>( mesh );

          transfer_boundary_information(mesh,
                                        viennagrid::make_accessor<facet_type>( src_boundary_information_container_wrapper.container ),
                                        viennagrid::make_accessor<element_type>( dst_boundary_information_container_wrapper.container ));

          update_change_counter( mesh, dst_boundary_information_container_wrapper.change_counter );
      }
  private:

      mesh_type & mesh;
  };


  /** @brief For internal use only. */
  template<typename WrappedConfigType>
  void transfer_boundary_information( mesh_t<WrappedConfigType> & mesh )
  {
      typedef mesh_t<WrappedConfigType> mesh_type;
      typedef typename viennagrid::result_of::cell_tag< mesh_type >::type cell_tag;
      typedef typename viennagrid::result_of::facet_tag< cell_tag >::type facet_tag;

      typedef typename viennagrid::meta::typelist::result_of::erase<
          typename viennagrid::meta::typemap::result_of::key_typelist<
              typename viennagrid::storage::result_of::value_type<
                  typename mesh_type::appendix_type,
                  boundary_information_collection_tag
              >::type::typemap
          >::type,
          facet_tag
      >::type typelist;

      boundary_setter_functor<mesh_type> functor(mesh);

      viennagrid::meta::typelist::for_each< typelist >( functor );
  }

  /** @brief For internal use only. */
  template<typename SegmentationType>
  void transfer_boundary_information( segment_handle_t<SegmentationType> & segment )
  { transfer_boundary_information( segment.view() ); }


  /** @brief For internal use only. */
  template<typename WrappedConfigType>
  void detect_boundary( mesh_t<WrappedConfigType> & mesh )
  {
      typedef mesh_t<WrappedConfigType> mesh_type;
      typedef typename viennagrid::result_of::cell_tag< mesh_type >::type cell_tag;
      typedef typename viennagrid::result_of::facet_tag< cell_tag >::type facet_tag;

      typedef typename viennagrid::result_of::element< mesh_type, facet_tag >::type facet_type;

      typedef typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename mesh_type::appendix_type,
                  boundary_information_collection_tag
                >::type,
                facet_tag
              >::type boundary_information_container_wrapper_type;
      boundary_information_container_wrapper_type & boundary_information_container_wrapper = boundary_information_collection<facet_tag>( mesh );
      detect_boundary( mesh, viennagrid::make_accessor<facet_type>( boundary_information_container_wrapper.container ) );

      transfer_boundary_information(mesh);
      update_change_counter( mesh, boundary_information_container_wrapper.change_counter );
  }

  /** @brief For internal use only. */
  template<typename SegmentationType>
  void detect_boundary( segment_handle_t<SegmentationType> & segment )
  { detect_boundary( segment.view() ); }







  /** @brief For internal use only. */
  template <typename ElementType, typename AccessorType>
  bool is_boundary(AccessorType const boundary_info_accessor,
                   ElementType const & element)
  {
      return boundary_info_accessor(element);
  }

  /** @brief Returns true if an element is located on the boundary of the mesh
   *
   * @param mesh      The ViennaGrid mesh
   * @param element     The element
   */
  template <typename WrappedConfigType, typename ElementType>
  bool is_boundary(mesh_t<WrappedConfigType> const & mesh, ElementType const & element)
  {
        typedef mesh_t<WrappedConfigType> mesh_type;
        typedef typename viennagrid::result_of::element_tag<ElementType>::type element_tag;


        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename mesh_type::appendix_type,
                    boundary_information_collection_tag
                  >::type,
                  element_tag
                >::type boundary_information_container_wrapper_type;
        boundary_information_container_wrapper_type const & boundary_information_container_wrapper = boundary_information_collection<element_tag>( mesh );

        if (mesh.is_obsolete(boundary_information_container_wrapper.change_counter))
            detect_boundary( const_cast<mesh_type&>(mesh) );

        return is_boundary( viennagrid::make_accessor<ElementType>(boundary_information_container_wrapper.container), element );
  }

  /** @brief Returns true if an element is located on the boundary of the segment
   *
   * @param segment      The ViennaGrid segment
   * @param element      The element
   */
  template <typename SegmentationType, typename ElementType>
  bool is_boundary(segment_handle_t<SegmentationType> const & segment, ElementType const & element)
  { return is_boundary( segment.view(), element ); }



  /** @brief Returns true if the element provided as second argument is on the boundary of the element provided as first argument
   *
   * @param host_element    The host element
   * @param element         The (potential) boundary element
   */
  template <typename ElementTag1, typename WrappedConfigType1,
            typename ElementTag2, typename WrappedConfigType2>
  bool is_boundary(element_t<ElementTag1, WrappedConfigType1> const & host_element,
                   element_t<ElementTag2, WrappedConfigType2> const & element)
  {

    typedef typename viennagrid::result_of::const_element_range<element_t<ElementTag2, WrappedConfigType2>, ElementTag1>::type   BoundaryRange;
    typedef typename viennagrid::result_of::iterator<BoundaryRange>::type               BoundaryIterator;

    BoundaryRange bnd_cells = viennagrid::elements(host_element);
    for (BoundaryIterator bit = bnd_cells.begin();
                          bit != bnd_cells.end();
                        ++bit)
    {
      if (&(*bit) == &element)
        return true;
    }

    return false;
  }

}

#endif
