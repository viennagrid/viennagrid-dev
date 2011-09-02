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

#ifndef VIENNAGRID_ALGORITHM_BOUNDARY_HPP
#define VIENNAGRID_ALGORITHM_BOUNDARY_HPP

#include <vector>
#include "viennagrid/forwards.h"
#include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/detail/domain_iterators.hpp"

namespace viennagrid
{

  template <typename T>
  class boundary_key
  {
    typedef typename T::ERROR_BOUNDARY_KEY_MUST_BE_USED_WITH_DOMAIN_OR_SEGMENT   error_type;
  };
  
  template <typename ConfigType, typename ElementTag>
  class boundary_key<element_t<ConfigType, ElementTag> >
  {
      typedef element_t<ConfigType, ElementTag>  element_type;
    
    public:
      boundary_key(element_type const & e) : e_(&e) {}
      
      //for compatibility with std::map
      bool operator<(boundary_key const & other) const
      {
        return e_ < other.e_;
      }
    private:
      element_type const * e_;
  };
  
  
  template <typename ConfigType>
  class boundary_key< segment_t<ConfigType> >
  {
    public:
      boundary_key(std::size_t seg_id) : seg_id_(seg_id) {}
      
      //for compatibility with std::map
      bool operator<(boundary_key const & other) const
      {
        return seg_id_ < other.seg_id_;
      }
    private:
      std::size_t seg_id_;
  };
  
  template <typename ConfigType>
  class boundary_key< domain_t<ConfigType> > {};
  
}

namespace viennadata
{
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

  //helper struct for setting boundary flag of lower level elements of a facet
  template <long topology_level>
  struct boundary_setter
  {
    template <typename FacetType, typename KeyType>
    static void apply(FacetType const & facet, KeyType const & key)
    {
      typedef typename viennagrid::result_of::const_ncell_range<FacetType, topology_level>::type    ElementOnFacetRange;
      typedef typename result_of::iterator<ElementOnFacetRange>::type                    ElementOnFacetIterator;

      ElementOnFacetRange eof_container = ncells<topology_level>(facet);
      for (ElementOnFacetIterator eofit = eof_container.begin();
            eofit != eof_container.end();
            ++eofit)
      {
        viennadata::access<KeyType, bool>(key)(*eofit) = true;
      }

      //proceed to lower level:
      boundary_setter<topology_level - 1>::apply(facet, key);
    }
  };

  //end recursion of topolevel = -1
  template <>
  struct boundary_setter< -1 >
  {
    template <typename FacetType, typename KeyType>
    static void apply(FacetType const & facet, KeyType const & key)
    {
    }
  };


  template <typename DomainSegmentType, typename KeyType>
  void detect_boundary_impl(DomainSegmentType const & seg, KeyType const & key, no_handling_tag)
  {
    typedef typename DomainSegmentType::ERROR_CANNOT_DETECT_BOUNDARY_BECAUSE_FACETS_ARE_DISABLED        error_type;
  }

  template <typename DomainSegmentType, typename KeyType>
  void detect_boundary_impl(DomainSegmentType const & seg, KeyType const & key, full_handling_tag)
  {
    typedef typename DomainSegmentType::config_type                            ConfigType;
    typedef typename ConfigType::cell_tag                                      CellTag;
    typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::topology_level-1>::type   FacetType;
    typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::topology_level>::type     CellType;

    typedef typename viennagrid::result_of::const_ncell_range<DomainSegmentType, CellTag::topology_level-1>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;
      
    typedef typename viennagrid::result_of::const_ncell_range<DomainSegmentType, CellTag::topology_level>::type        CellRange;
    typedef typename viennagrid::result_of::iterator<CellRange>::type                                            CellIterator;

    typedef typename viennagrid::result_of::const_ncell_range<CellType, CellTag::topology_level-1>::type     FacetOnCellRange;
    typedef typename viennagrid::result_of::iterator<FacetOnCellRange>::type                           FacetOnCellIterator;

    //iterate over all cells, over facets there and tag them:
    CellRange cells = viennagrid::ncells<CellTag::topology_level>(seg);
    for (CellIterator cit = cells.begin();
          cit != cells.end();
          ++cit)
    {
      FacetOnCellRange facets_on_cell = ncells<CellTag::topology_level-1>(*cit);
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
    FacetRange facets = viennagrid::ncells<CellTag::topology_level-1>(seg);
    for (FacetIterator fit = facets.begin();
          fit != facets.end();
          ++fit)
    {
      if (viennadata::find<KeyType, bool>(key)(*fit) != NULL)
      {
        if (viennadata::access<KeyType, bool>(key)(*fit) == true)
          boundary_setter<CellTag::topology_level-2>::apply(*fit, key);
      }
    }
  }



  //
  // public interface functions:
  //

  template <typename DomainSegmentType, typename KeyType>
  void detect_boundary(DomainSegmentType const & segment, KeyType const & key)
  {
    typedef typename DomainSegmentType::config_type::cell_tag                   CellTag;
    typedef typename topology::subcell_desc<CellTag, CellTag::topology_level-1>::handling_tag  HandlingTag;
    
    if (viennadata::access<KeyType, bool>(key)(segment) == false)
    {
      detect_boundary_impl(segment, key, HandlingTag());
      viennadata::access<KeyType, bool>(key)(segment) = true;
    }
  }



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

}

#endif
