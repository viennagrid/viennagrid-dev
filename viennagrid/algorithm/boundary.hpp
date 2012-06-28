#ifndef VIENNAGRID_ALGORITHM_BOUNDARY_HPP
#define VIENNAGRID_ALGORITHM_BOUNDARY_HPP

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

#include <vector>
#include "viennagrid/forwards.h"
#include "viennagrid/domain.hpp"
#include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/detail/domain_iterators.hpp"

/** @file boundary.hpp
    @brief Provides the detection and check for boundary n-cells.
*/

namespace viennagrid
{
  /** @brief A key type for storing boundary information on n-cells, segments and domains */
  template <typename T>
  class boundary_key
  {
    typedef typename T::ERROR_BOUNDARY_KEY_MUST_BE_USED_WITH_NCELL_OR_DOMAIN_OR_SEGMENT   error_type;
  };
  
  /** @brief Specialization for n-cells */
  template <typename ConfigType, typename ElementTag>
  class boundary_key<element_t<ConfigType, ElementTag> >
  {
      typedef element_t<ConfigType, ElementTag>  element_type;
    
    public:
      boundary_key(element_type const & e) : e_(&e) {}
      
      /** @brief Provide less-than operator for compatibility with std::map */
      bool operator<(boundary_key const & other) const
      {
        return e_ < other.e_;
      }
    private:
      element_type const * e_;
  };
  
  /** @brief Specialization for segments */
  template <typename ConfigType>
  class boundary_key< segment_t<ConfigType> >
  {
    public:
      boundary_key(std::size_t seg_id) : seg_id_(seg_id) {}
      
      /** @brief Provide less-than operator for compatibility with std::map */
      bool operator<(boundary_key const & other) const
      {
        return seg_id_ < other.seg_id_;
      }
    private:
      std::size_t seg_id_;
  };
  
  /** @brief Specialization for domains */
  template <typename ConfigType>
  class boundary_key< domain_t<ConfigType> > {};
  
}

// Configure ViennaData to use a type-based dispatch on the domain

/** @brief Customizations of keys for ViennaData */
namespace viennadata
{
  /** @brief Customizations of keys for ViennaData */
  namespace config
  {
    template <typename ConfigType>
    struct key_dispatch<viennagrid::boundary_key< viennagrid::domain_t<ConfigType> > >
    {
      typedef type_key_dispatch_tag    tag;
    };
  }
}
  
namespace viennagrid
{

  /** @brief Helper struct for setting boundary flag of lower level elements of a facet
   * 
   * @tparam dim    Topological dimension the boundary setter is acting on
   */
  template <long dim>
  struct boundary_setter
  {
    /** @brief The static actor for the case that boundary k-cells for this level are available on the n-cell */
    template <typename FacetType, typename KeyType>
    static void apply(FacetType const & facet, KeyType const & key, full_handling_tag)
    {
      typedef typename FacetType::config_type        ConfigType;
      typedef typename ConfigType::cell_tag          CellTag;
      
      typedef typename viennagrid::result_of::const_ncell_range<FacetType, dim>::type    ElementOnFacetRange;
      typedef typename result_of::iterator<ElementOnFacetRange>::type                    ElementOnFacetIterator;

      ElementOnFacetRange eof_container = ncells<dim>(facet);
      for (ElementOnFacetIterator eofit = eof_container.begin();
            eofit != eof_container.end();
            ++eofit)
      {
        viennadata::access<KeyType, bool>(key)(*eofit) = true;
      }

      //proceed to lower level:
      boundary_setter<dim - 1>::apply(facet,
                                                 key,
                                                 typename viennagrid::result_of::bndcell_handling<ConfigType, CellTag, dim-1>::type()
                                                );
    }
    
    /** @brief The static actor for the case that no boundary k-cells for this level are available on the n-cell */
    template <typename FacetType, typename KeyType>
    static void apply(FacetType const & facet, KeyType const & key, no_handling_tag)
    {
      typedef typename FacetType::config_type        ConfigType;
      typedef typename ConfigType::cell_tag          CellTag;
      
      //no elements handled at this level, thus proceed to lower level:
      boundary_setter<dim - 1>::apply(facet,
                                                 key,
                                                 typename viennagrid::result_of::bndcell_handling<ConfigType, CellTag, dim-1>::type()
                                                );
    }
    
  };

  //end recursion of topolevel = -1
  /** @brief Ends the recursion below the vertex level */
  template <>
  struct boundary_setter< -1 >
  {
    template <typename FacetType, typename KeyType, typename HandlingTag>
    static void apply(FacetType const & facet, KeyType const & key, HandlingTag)
    {
    }
  };

  /** @brief Implementation of boundary detection for the case no facets are available. Issues a hopefully useful compiler error. */
  template <typename DomainSegmentType, typename KeyType>
  void detect_boundary_impl(DomainSegmentType const & seg, KeyType const & key, no_handling_tag)
  {
    typedef typename DomainSegmentType::ERROR_CANNOT_DETECT_BOUNDARY_BECAUSE_FACETS_ARE_DISABLED        error_type;
  }

  /** @brief Implementation of boundary detection. Should not be called by library users. */
  template <typename DomainSegmentType, typename KeyType>
  void detect_boundary_impl(DomainSegmentType const & seg, KeyType const & key, full_handling_tag)
  {
    typedef typename DomainSegmentType::config_type                            ConfigType;
    typedef typename ConfigType::cell_tag                                      CellTag;
    typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::dim-1>::type   FacetType;
    typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type     CellType;

    typedef typename viennagrid::result_of::const_ncell_range<DomainSegmentType, CellTag::dim-1>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;
      
    typedef typename viennagrid::result_of::const_ncell_range<DomainSegmentType, CellTag::dim>::type        CellRange;
    typedef typename viennagrid::result_of::iterator<CellRange>::type                                            CellIterator;

    typedef typename viennagrid::result_of::const_ncell_range<CellType, CellTag::dim-1>::type     FacetOnCellRange;
    typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                           FacetOnCellIterator;

    //iterate over all cells, over facets there and tag them:
    CellRange cells = viennagrid::ncells<CellTag::dim>(seg);
    for (CellIterator cit = cells.begin();
          cit != cells.end();
          ++cit)
    {
      FacetOnCellRange facets_on_cell = ncells<CellTag::dim-1>(*cit);
      for (FacetOnCellIterator focit = facets_on_cell.begin();
            focit != facets_on_cell.end();
            ++focit)
      {
        bool * data_ptr = viennadata::find<KeyType, bool>(key)(*focit);
        if (data_ptr == NULL)  //Facet has not yet been tagged
        {
          viennadata::access<KeyType, bool>(key)(*focit) = true;
        }
        else
        {
          if (*data_ptr == false)          //mind densely stored data
            *data_ptr = true;
          else          
            viennadata::erase<KeyType, bool>(key)(*focit);
        }        
      }
    }
    
    //iterate over all facets again and tag all lower level topological elements on facets that belong to the boundary:
    FacetRange facets = viennagrid::ncells<CellTag::dim-1>(seg);
    for (FacetIterator fit = facets.begin();
          fit != facets.end();
          ++fit)
    {
      if (viennadata::find<KeyType, bool>(key)(*fit) != NULL)
      {
        if (viennadata::access<KeyType, bool>(key)(*fit) == true)
          boundary_setter<CellTag::dim-2>::apply(*fit,
                                                            key,
                                                            typename viennagrid::result_of::bndcell_handling<ConfigType, CellTag, CellTag::dim-2>::type()
                                                           );
      }
    }
  }



  /** @brief Public interface functions for boundary detection. No need to call it explicitly, since it is called by is_boundary().
   * 
   * @tparam DomainSegmentType    Either a segment or a domain type
   * @tparam KeyType              Type of the key used with ViennaData.
   * @param segment               Either a segment or a domain object
   * @param key                   The key object for ViennaData
   */
  template <typename DomainSegmentType, typename KeyType>
  void detect_boundary(DomainSegmentType const & segment, KeyType const & key)
  {
    typedef typename DomainSegmentType::config_type            ConfigType;
    typedef typename ConfigType::cell_tag                   CellTag;
    typedef typename result_of::domain<ConfigType>::type                        DomainType;
    typedef typename result_of::bndcell_handling<ConfigType, DomainType,
                                                    CellTag::dim-1>::type  HandlingTag;
    
    if (viennadata::access<KeyType, bool>(key)(segment) == false)
    {
      detect_boundary_impl(segment, key, HandlingTag());
      viennadata::access<KeyType, bool>(key)(segment) = true;
    }
  }


  /** @brief Returns true if a n-cell is located on the boundary of the domain 
   *
   * @param el      The n-cell
   * @param domain  The ViennaGrid domain
   */
  template <typename ConfigType, typename ElementTag>
  bool is_boundary(element_t<ConfigType, ElementTag> const & el,
                   domain_t<ConfigType> const & domain)
  {
    typedef boundary_key<domain_t<ConfigType> >    BoundaryKey;
    
    detect_boundary(domain, BoundaryKey());
    if (viennadata::find<BoundaryKey, bool>()(el) != NULL)
    {
      return viennadata::access<BoundaryKey, bool>()(el);
    }
    return false;
  }

  /** @brief Returns true if a n-cell is located on the boundary of a segment
   *
   * @param el      The n-cell
   * @param segment A segment of a domain
   */
  template <typename ConfigType, typename ElementTag>
  bool is_boundary(element_t<ConfigType, ElementTag> const & el,
                   segment_t<ConfigType> const & segment)
  {
    typedef boundary_key<segment_t<ConfigType> >    BoundaryKey;
    
    BoundaryKey key(segment.id());
    detect_boundary(segment, key);
    
    if (viennadata::find<BoundaryKey, bool>(key)(el) != NULL)
    {
      return viennadata::access<BoundaryKey, bool>(key)(el);
    }
    return false;
  }

  /** @brief Returns true if the k-cell provided as first argument is on the boundary of the n-cell provided as second argument
   *
   * @param el      The n-cell
   * @param segment A segment of a domain
   */
  template <typename ConfigType, typename ElementTag1, typename ElementTag2>
  bool is_boundary(element_t<ConfigType, ElementTag1> const & el1,
                   element_t<ConfigType, ElementTag2> const & el2)
  {

    typedef typename viennagrid::result_of::const_ncell_range<element_t<ConfigType, ElementTag2>,
                                                              ElementTag1::dim>::type   BoundaryRange;
    typedef typename viennagrid::result_of::iterator<BoundaryRange>::type               BoundaryIterator;
        
    BoundaryRange bnd_cells = viennagrid::ncells(el2);
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
