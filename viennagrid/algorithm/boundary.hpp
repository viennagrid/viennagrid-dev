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
#include "viennagrid/domain/accessor.hpp"
#include "viennagrid/domain/segmentation.hpp"


/** @file boundary.hpp
    @brief Contains functions for detecting and querying boundary information
*/


namespace viennagrid
{

  /** @brief For internal use only. */
  template <typename DomainType, typename AccessorType>
  void detect_boundary(DomainType & domain, AccessorType boundary_info_accessor)
  {
//     std::cout << "DETECT BOUNDARY" << std::endl;

    typedef typename viennagrid::result_of::cell_tag<DomainType>::type CellTag;
    typedef typename viennagrid::result_of::facet_tag<CellTag>::type FacetTag;

    typedef typename viennagrid::result_of::element<DomainType, FacetTag >::type   FacetType;
    typedef typename viennagrid::result_of::element<DomainType, CellTag>::type     CellType;

    typedef typename viennagrid::result_of::element_range<DomainType, FacetTag>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;

    typedef typename viennagrid::result_of::element_range<DomainType, CellTag>::type        CellRange;
    typedef typename viennagrid::result_of::iterator<CellRange>::type                                            CellIterator;

    typedef typename viennagrid::result_of::element_range<CellType, FacetTag>::type     FacetOnCellRange;
    typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                           FacetOnCellIterator;


    FacetRange facets = viennagrid::elements(domain);

    for (FacetIterator fit = facets.begin();
          fit != facets.end();
          ++fit)
        boundary_info_accessor(*fit) = false;

    //iterate over all cells, over facets there and tag them:
    CellRange cells = viennagrid::elements(domain);
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
  template <typename DomainType, typename SourceAccessorType, typename DestinationAccessorType>
  void transfer_boundary_information(DomainType const & domain,
                       SourceAccessorType const source_boundary_info_accessor,
                       DestinationAccessorType destination_boundary_info_accessor
                      )
  {
    typedef typename SourceAccessorType::access_type src_element_type;
    typedef typename DestinationAccessorType::access_type dst_element_type;

    typedef typename viennagrid::result_of::const_element_range< DomainType, dst_element_type >::type dst_range_type;
    typedef typename viennagrid::result_of::iterator< dst_range_type >::type dst_range_iterator;

    dst_range_type dst_elements = viennagrid::elements( domain );

    for (dst_range_iterator it = dst_elements.begin(); it != dst_elements.end(); ++it)
        destination_boundary_info_accessor(*it) = false;


    typedef typename viennagrid::result_of::const_element_range< DomainType, src_element_type >::type src_range_type;
    typedef typename viennagrid::result_of::iterator< src_range_type >::type src_range_iterator;

    src_range_type src_elements = viennagrid::elements( domain );


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
  template<typename domain_type>
  class boundary_setter_functor
  {
  public:
      boundary_setter_functor(domain_type & domain_) : domain(domain_) {}

      template<typename something>
      void operator()( viennagrid::meta::tag<something> )
      {
          typedef typename viennagrid::result_of::element_tag< something >::type element_tag;
          typedef typename viennagrid::result_of::element< domain_type, element_tag >::type element_type;

          typedef typename viennagrid::result_of::cell_tag< domain_type >::type cell_tag;
          typedef typename viennagrid::result_of::facet_tag< cell_tag >::type facet_tag;
          typedef typename viennagrid::result_of::element< domain_type, facet_tag >::type facet_type;



        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    boundary_information_collection_tag
                  >::type,
                  facet_tag
                >::type src_boundary_information_container_wrapper_type;

          src_boundary_information_container_wrapper_type & src_boundary_information_container_wrapper = boundary_information_collection<facet_tag>( domain );


        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    boundary_information_collection_tag
                  >::type,
                  element_tag
                >::type dst_boundary_information_container_wrapper_type;

          dst_boundary_information_container_wrapper_type & dst_boundary_information_container_wrapper = boundary_information_collection<element_tag>( domain );

          transfer_boundary_information(domain,
                                        viennagrid::make_accessor<facet_type>( src_boundary_information_container_wrapper.container ),
                                        viennagrid::make_accessor<element_type>( dst_boundary_information_container_wrapper.container ));

          update_change_counter( domain, dst_boundary_information_container_wrapper.change_counter );
      }
  private:

      domain_type & domain;
  };


  /** @brief For internal use only. */
  template<typename WrappedConfigType>
  void transfer_boundary_information( domain_t<WrappedConfigType> & domain )
  {
      typedef domain_t<WrappedConfigType> domain_type;
      typedef typename viennagrid::result_of::cell_tag< domain_type >::type cell_tag;
      typedef typename viennagrid::result_of::facet_tag< cell_tag >::type facet_tag;

      typedef typename viennagrid::meta::typelist::result_of::erase<
          typename viennagrid::meta::typemap::result_of::key_typelist<
              typename viennagrid::storage::result_of::value_type<
                  typename domain_type::appendix_type,
                  boundary_information_collection_tag
              >::type::typemap
          >::type,
          facet_tag
      >::type typelist;

      boundary_setter_functor<domain_type> functor(domain);

      viennagrid::meta::typelist::for_each< typelist >( functor );
  }

  /** @brief For internal use only. */
  template<typename SegmentationType>
  void transfer_boundary_information( segment_t<SegmentationType> & segment )
  { transfer_boundary_information( segment.view() ); }


  /** @brief For internal use only. */
  template<typename WrappedConfigType>
  void detect_boundary( domain_t<WrappedConfigType> & domain )
  {
      typedef domain_t<WrappedConfigType> domain_type;
      typedef typename viennagrid::result_of::cell_tag< domain_type >::type cell_tag;
      typedef typename viennagrid::result_of::facet_tag< cell_tag >::type facet_tag;

      typedef typename viennagrid::result_of::element< domain_type, facet_tag >::type facet_type;

      typedef typename viennagrid::storage::result_of::value_type<
              typename viennagrid::storage::result_of::value_type<
                  typename domain_type::appendix_type,
                  boundary_information_collection_tag
                >::type,
                facet_tag
              >::type boundary_information_container_wrapper_type;
      boundary_information_container_wrapper_type & boundary_information_container_wrapper = boundary_information_collection<facet_tag>( domain );
      detect_boundary( domain, viennagrid::make_accessor<facet_type>( boundary_information_container_wrapper.container ) );

      transfer_boundary_information(domain);
      update_change_counter( domain, boundary_information_container_wrapper.change_counter );
  }

  /** @brief For internal use only. */
  template<typename SegmentationType>
  void detect_boundary( segment_t<SegmentationType> & segment )
  { detect_boundary( segment.view() ); }







  /** @brief For internal use only. */
  template <typename ElementType, typename AccessorType>
  bool is_boundary(AccessorType const boundary_info_accessor,
                   ElementType const & element)
  {
      return boundary_info_accessor(element);
  }

  /** @brief Returns true if an element is located on the boundary of the domain
   *
   * @param domain      The ViennaGrid domain
   * @param element     The element
   */
  template <typename WrappedConfigType, typename ElementType>
  bool is_boundary(domain_t<WrappedConfigType> const & domain, ElementType const & element)
  {
        typedef domain_t<WrappedConfigType> domain_type;
        typedef typename viennagrid::result_of::cell_tag< domain_type >::type cell_tag;
        typedef typename viennagrid::result_of::facet_tag< cell_tag >::type facet_tag;

        typedef typename viennagrid::result_of::element_tag<ElementType>::type element_tag;


        typedef typename viennagrid::storage::result_of::value_type<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    boundary_information_collection_tag
                  >::type,
                  element_tag
                >::type boundary_information_container_wrapper_type;
        boundary_information_container_wrapper_type const & boundary_information_container_wrapper = boundary_information_collection<element_tag>( domain );

        if (domain.is_obsolete(boundary_information_container_wrapper.change_counter))
            detect_boundary( const_cast<domain_type&>(domain) );

        return is_boundary( viennagrid::make_accessor<ElementType>(boundary_information_container_wrapper.container), element );
  }

  /** @brief Returns true if an element is located on the boundary of the segment
   *
   * @param segment      The ViennaGrid segment
   * @param element      The element
   */
  template <typename SegmentationType, typename ElementType>
  bool is_boundary(segment_t<SegmentationType> const & segment, ElementType const & element)
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
