#ifndef VIENNAGRID_ALGORITHM_BOUNDARY_HPP
#define VIENNAGRID_ALGORITHM_BOUNDARY_HPP

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

#include <vector>
#include "viennagrid/forwards.hpp"
#include "viennagrid/element/element.hpp"
#include "viennagrid/domain/accessor.hpp"



namespace viennagrid
{

  /** @brief Implementation of boundary detection. Should not be called by library users. */
  template <typename DomainType, typename AccessorType>
  void detect_boundary(DomainType & domain, AccessorType boundary_info_accessor)
  {
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
    
    boundary_info_accessor.resize( facets.size() );
    
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
      
      destination_boundary_info_accessor.resize( dst_elements.size() );
      
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
  
  
  
  
  
    template<typename domain_type>
    class boundary_setter_functor
    {
    public:
        boundary_setter_functor(domain_type & domain_) : domain(domain_) {}
        
        template<typename something>
        void operator()( viennameta::tag<something> )
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
            viennagrid::accessor::dense_container_accessor_t< const typename src_boundary_information_container_wrapper_type::container_type, facet_type > src_accessor( src_boundary_information_container_wrapper.container );
            
            
            
            
          typedef typename viennagrid::storage::result_of::value_type<
                  typename viennagrid::storage::result_of::value_type<
                      typename domain_type::appendix_type,
                      boundary_information_collection_tag
                    >::type,
                    element_tag
                  >::type dst_boundary_information_container_wrapper_type;
                    
            dst_boundary_information_container_wrapper_type & dst_boundary_information_container_wrapper = boundary_information_collection<element_tag>( domain );
            viennagrid::accessor::dense_container_accessor_t< typename dst_boundary_information_container_wrapper_type::container_type, element_type > dst_accessor( dst_boundary_information_container_wrapper.container );

            transfer_boundary_information(domain, src_accessor, dst_accessor);
            
            dst_boundary_information_container_wrapper.change_counter = domain.change_counter();
        }
    private:
        
        domain_type & domain;
    };
  
  
  
    template<typename A, typename B, typename C>
    void transfer_boundary_information( domain_t<A,B,C> & domain )
    {
        typedef domain_t<A,B,C> domain_type;
        typedef typename viennagrid::result_of::cell_tag< domain_type >::type cell_tag;
        typedef typename viennagrid::result_of::facet_tag< cell_tag >::type facet_tag;
        
        typedef typename viennameta::typelist::result_of::erase<
            typename viennameta::typemap::result_of::key_typelist<
                typename viennagrid::storage::result_of::value_type<
                    typename domain_type::appendix_type,
                    boundary_information_collection_tag
                >::type::typemap
            >::type,
            facet_tag
        >::type typelist;
        
        boundary_setter_functor<domain_type> functor(domain);
        
        viennameta::typelist::for_each< typelist >( functor );
    }
  
  
    template<typename A, typename B, typename C>
    void detect_boundary( domain_t<A,B,C> & domain )
    {
        typedef domain_t<A,B,C> domain_type;
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
        
//         viennagrid::accessor::dense_container_accessor_t< typename boundary_information_container_wrapper_type::container_type, facet_type > accessor( boundary_information_container_wrapper.container );
        detect_boundary( domain, accessor::dense_container_accessor<facet_type>( boundary_information_container_wrapper.container ) );
        
        transfer_boundary_information(domain);
        boundary_information_container_wrapper.change_counter = domain.change_counter();
    }
  
  

  

  
  
  /** @brief Returns true if a n-cell is located on the boundary of the domain 
   *
   * @param el      The n-cell
   * @param domain  The ViennaGrid domain
   */
  template <typename ElementType, typename AccessorType>
  bool is_boundary(AccessorType const boundary_info_accessor,
                   ElementType const & element)
  {
      return boundary_info_accessor(element);
  }
  
  template <typename A, typename B, typename C, typename ElementType>
  bool is_boundary(domain_t<A, B, C> const & domain, ElementType const & element)
  {
        typedef domain_t<A,B,C> domain_type;
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
                
        if (boundary_information_container_wrapper.change_counter != domain.change_counter())
            detect_boundary( const_cast<domain_type&>(domain) );
        
        viennagrid::accessor::dense_container_accessor_t< const typename boundary_information_container_wrapper_type::container_type, ElementType > accessor( boundary_information_container_wrapper.container );        
        return is_boundary( accessor, element );
  }
  

  /** @brief Returns true if the k-cell provided as first argument is on the boundary of the n-cell provided as second argument
   *
   * @param el      The n-cell
   * @param segment A segment of a domain
   */
  template <typename element_tag_1, typename bnd_cell_container_typelist_1, typename id_tag_1, typename appendix_type_1,
            typename element_tag_2, typename bnd_cell_container_typelist_2, typename id_tag_2, typename appendix_type_2>
  bool is_boundary(element_t<element_tag_1, bnd_cell_container_typelist_1, id_tag_1, appendix_type_1> const & el1,
                   element_t<element_tag_2, bnd_cell_container_typelist_2, id_tag_2, appendix_type_2> const & el2)
  {

    typedef typename viennagrid::result_of::const_element_range<element_t<element_tag_2, bnd_cell_container_typelist_2, id_tag_2, appendix_type_2>,
                                                              element_tag_1>::type   BoundaryRange;
    typedef typename viennagrid::result_of::iterator<BoundaryRange>::type               BoundaryIterator;
        
    BoundaryRange bnd_cells = viennagrid::elements<element_tag_1>(el2);
    for (BoundaryIterator bit = bnd_cells.begin();
                          bit != bnd_cells.end();
                        ++bit)
    {
      if (&(*bit) == &el1)
        return true;
    }
    
    return false;
  }
  
}

#endif
