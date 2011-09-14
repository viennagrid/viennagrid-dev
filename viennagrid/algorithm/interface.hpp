#ifndef VIENNAGRID_ALGORITHM_INTERFACE_HPP
#define VIENNAGRID_ALGORITHM_INTERFACE_HPP

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
#include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/detail/domain_iterators.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/boundary.hpp"

/** @file interface.hpp
    @brief Provides the detection and check for boundary n-cells at the interface of two segments.
*/


namespace viennagrid
{
  /** @brief A key used for ViennaData in order to store interface information */
  class interface_key
  {
    public:
      interface_key(std::size_t id1, std::size_t id2) : id_(id1, id2) {}
      
      /** @brief For compatibility with std::map<> */
      bool operator<(interface_key const & other) const
      {
        return id_ < other.id_;
      }
      
    private:
      std::pair<std::size_t, std::size_t> id_;
  };


  /** @brief Helper struct for setting interface flag of boundary k-cells of a facet */
  template <long dim>
  struct interface_setter
  {
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
      interface_setter<dim-1>::apply(facet,
                                                key,
                                                typename viennagrid::result_of::bndcell_handling<ConfigType, CellTag, dim-1>::type());
    }
    
    template <typename FacetType, typename KeyType>
    static void apply(FacetType const & facet, KeyType const & key, no_handling_tag)
    {
      typedef typename FacetType::config_type        ConfigType;
      typedef typename ConfigType::cell_tag          CellTag;
      
      //proceed to lower level:
      interface_setter<dim-1>::apply(facet,
                                                key,
                                                typename viennagrid::result_of::bndcell_handling<ConfigType, CellTag, dim-1>::type());
    }
    
  };

  //end recursion of topological dimension = -1
  /** @brief Specialization that stops recursion below the vertex level */
  template <>
  struct interface_setter< -1 >
  {
    template <typename FacetType, typename KeyType, typename HandlingTag>
    static void apply(FacetType const & facet, KeyType const & key, HandlingTag) {}
  };

  /** @brief A guard that forces a compile time error if no facets are available (i.e. disabled). */
  template <typename SegmentType, typename KeyType>
  void detect_interface_impl(SegmentType const & seg1,
                             SegmentType const & seg2,
                             KeyType const & key,
                             no_handling_tag)
  {
    typedef typename SegmentType::ERROR_CANNOT_DETECT_INTERFACE_BECAUSE_FACETS_ARE_DISABLED        error_type;
  }

  /** @brief Implementation of interface facet detection with complexity O(N log(N)), where N is the number of facets in both segments.
   * 
   * @param seg1     The first segment
   * @param seg2     The second segment
   * @param key      Key object for use with ViennaData.
   */
  template <typename SegmentType, typename KeyType>
  void detect_interface_impl(SegmentType const & seg1,
                             SegmentType const & seg2,
                             KeyType const & key,
                             full_handling_tag)
  {
    typedef typename SegmentType::config_type                                  ConfigType;
    typedef typename ConfigType::cell_tag                                      CellTag;
    typedef typename viennagrid::result_of::point<ConfigType>::type                              PointType;
    typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::dim-1>::type   FacetType;
    typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::dim>::type     CellType;

    typedef typename viennagrid::result_of::const_ncell_range<SegmentType, CellTag::dim-1>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;
    
    typedef typename ConfigType::numeric_type         numeric_type;

    std::set<FacetType const *>  facets_ptrs_seg1;
    
    //
    // Step 1: Write facets of segment 1 to a map:
    //
    FacetRange facets_seg1 = viennagrid::ncells<CellTag::dim-1>(seg1);
    for (FacetIterator fit = facets_seg1.begin();
          fit != facets_seg1.end();
          ++fit)
    {
      if (is_boundary(*fit, seg1))
        facets_ptrs_seg1.insert( &(*fit) );
    }
    
    //
    // Step 2: Compare facet in segment 2 with those stored in the map
    //
    FacetRange facets_seg2 = viennagrid::ncells<CellTag::dim-1>(seg2);
    for (FacetIterator fit = facets_seg2.begin();
          fit != facets_seg2.end();
          ++fit)
    {
      if (facets_ptrs_seg1.find( &(*fit) ) != facets_ptrs_seg1.end())
        viennadata::access<KeyType, bool>(key)(*fit) = true;
    }
    
    //
    // Step 3: Iterate over all facets again and tag all lower level topological elements on facets that belong to the boundary:
    //
    for (FacetIterator fit = facets_seg1.begin();
          fit != facets_seg1.end();
          ++fit)
    {
      if (viennadata::find<KeyType, bool>(key)(*fit) != NULL)
      {
        if (viennadata::access<KeyType, bool>(key)(*fit) == true)
          interface_setter<CellTag::dim-2>::apply(*fit,
                                                             key,
                                                             typename viennagrid::result_of::bndcell_handling<ConfigType, CellTag, CellTag::dim-2>::type()
                                                            );
      }
    }
  }



  /** @brief Public interface function for the detection of interface n-cells between two segments. No need to call this function explicitly, since it is called by is_interface()
   * 
   * @param seg1  The first segment
   * @param seg2  The second segment
   * @param key   The key object for ViennaData
   */
  template <typename SegmentType, typename KeyType>
  void detect_interface(SegmentType const & seg1,
                        SegmentType const & seg2,
                        KeyType const & key)
  {
    typedef typename SegmentType::config_type               ConfigType;
    typedef typename ConfigType::cell_tag                   CellTag;
    typedef typename result_of::bndcell_handling<ConfigType, CellTag,
                                                    CellTag::dim-1>::type  HandlingTag;
    
    if (viennadata::access<KeyType, bool>(key)(seg1) == false)
    {
      detect_interface_impl(seg1, seg2, key, HandlingTag());
      viennadata::access<KeyType, bool>(key)(seg1) = true;
      viennadata::access<KeyType, bool>(key)(seg2) = true;
    }
  }

  /** @brief Returns true if the n-cell is located at the interface between two segments
   * 
   * @param el    The n-cell under test
   * @param seg1  The first segment
   * @param seg2  The sevond segment
   */
  template <typename ConfigType, typename ElementTag>
  bool is_interface(element_t<ConfigType, ElementTag> const & el,
                    segment_t<ConfigType> const & seg1,
                    segment_t<ConfigType> const & seg2)
  {
    typedef interface_key    InterfaceKey;
    
    std::size_t seg_id_lower = std::min(seg1.id(), seg2.id());
    std::size_t seg_id_higher = std::max(seg1.id(), seg2.id());
    
    InterfaceKey key(seg_id_lower, seg_id_higher);
    detect_interface(seg1, seg2, key);
    
    if (viennadata::find<InterfaceKey, bool>(key)(el) != NULL)
      return viennadata::access<InterfaceKey, bool>(key)(el);

    return false;
  }

}

#endif
