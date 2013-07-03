#ifndef VIENNAGRID_ALGORITHM_INTERFACE_HPP
#define VIENNAGRID_ALGORITHM_INTERFACE_HPP

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
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/boundary.hpp"

/** @file interface.hpp
    @brief Provides the detection and check for boundary n-cells at the interface of two segments.
*/


namespace viennagrid
{
  /** @brief A key used for ViennaData in order to store interface information */
  template<typename internal_id>
  class interface_key
  {
    public:
      interface_key(internal_id id1, internal_id id2) : id_(id1, id2)
      {
          if (id1 > id2)
              std::swap(id_.first, id_.second);
      }
      
      /** @brief For compatibility with std::map<> */
      bool operator<(interface_key const & other) const
      {
        return id_ < other.id_;
      }
      
    private:
      std::pair<internal_id, internal_id> id_;
  };


//   /** @brief Helper struct for setting interface flag of boundary k-cells of a facet */
//   template <typename cell_type, typename element_tag, bool has_boundary = result_of::has_boundary<cell_type, element_tag>::value >
//   struct interface_setter;
//   
//   template <typename cell_type, typename element_tag>
//   struct interface_setter<cell_type, element_tag, true>
//   {
//     template <typename FacetType, typename KeyType>
//     static void apply(FacetType const & facet, KeyType const & key)
//     {
//       //typedef typename FacetType::config_type        ConfigType;
//       //typedef typename ConfigType::cell_tag          CellTag;
//       
//       typedef typename viennagrid::result_of::const_element_range<FacetType, element_tag>::type    ElementOnFacetRange;
//       typedef typename result_of::iterator<ElementOnFacetRange>::type                    ElementOnFacetIterator;
// 
//       ElementOnFacetRange eof_container = elements<element_tag>(facet);
//       for (ElementOnFacetIterator eofit = eof_container.begin();
//             eofit != eof_container.end();
//             ++eofit)
//       {
//         viennadata::access<KeyType, bool>(key)(*eofit) = true;
//       }
// 
//       //proceed to lower level:
//       typedef typename element_tag::facet_tag facet_tag;
//       interface_setter<cell_type, facet_tag, result_of::has_boundary<cell_type, facet_tag>::value >::apply(facet, key);
//     }
//   };
//   
//   template <typename cell_type, typename element_tag>
//   struct interface_setter<cell_type, element_tag, false>
//   {
//     
//     template <typename FacetType, typename KeyType>
//     static void apply(FacetType const & facet, KeyType const & key)
//     {
// //       typedef typename FacetType::config_type        ConfigType;
// //       typedef typename ConfigType::cell_tag          CellTag;
//       
//       //proceed to lower level:
//       typedef typename element_tag::facet_tag facet_tag;
//       interface_setter<cell_type, facet_tag, result_of::has_boundary<cell_type, facet_tag>::value >::apply(facet, key);
//     }
//     
//   };
//   
//   
//   
//   //end recursion of topological dimension = -1
//   /** @brief Specialization that stops recursion below the vertex level */
//   template <typename cell_type>
//   struct interface_setter< cell_type, viennagrid::meta::null_type, true >
//   {
//     template <typename FacetType, typename KeyType>
//     static void apply(FacetType const & facet, KeyType const & key) {}
//   };
//   template <typename cell_type>
//   struct interface_setter< cell_type, viennagrid::meta::null_type, false >
//   {
//     template <typename FacetType, typename KeyType>
//     static void apply(FacetType const & facet, KeyType const & key) {}
//   };

  
  
  
  template<typename viennadata_key_type>
  class interface_setter_functor
  {
  public:
      interface_setter_functor(viennadata_key_type viennadata_key_) : viennadata_key(viennadata_key_) {}
      
      template<typename element_type>
      void operator()( const element_type & element )
      {
          viennadata::access<viennadata_key_type, bool>(viennadata_key)(element) = true;
      }
  private:
      
      viennadata_key_type viennadata_key;
  };
  
  
  template<typename element_type, typename viennadata_key_type>
  void mark_interfacet_cell(element_type & element, viennadata_key_type viennadata_key)
  {
      interface_setter_functor<viennadata_key_type> setter_functor(viennadata_key);
      viennagrid::for_each_boundary_cell( element, setter_functor );
  }
  
  


  
  
  
  
  
  
  template<typename CellTypeOrTag, bool boundary_storage>
  struct detect_interface_impl;
  
  template<typename CellTypeOrTag>
  struct detect_interface_impl<CellTypeOrTag, false>
  {
        template <typename SegmentType, typename KeyType>
        static void detect(SegmentType const & seg1,
                             SegmentType const & seg2,
                             KeyType const & key)
        {
            typedef typename SegmentType::ERROR_CANNOT_DETECT_INTERFACE_BECAUSE_FACETS_ARE_DISABLED        error_type;
        }
  };
  
  template<typename CellTypeOrTag>
  struct detect_interface_impl<CellTypeOrTag, true>
  {
        template <typename SegmentType, typename KeyType>
        static void detect(SegmentType const & seg1,
                             SegmentType const & seg2,
                             KeyType const & key)
        {
   //typedef typename SegmentType::config_type                                  ConfigType;
    //typedef typename ConfigType::cell_tag                                      CellTag;
    typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
    typedef typename viennagrid::result_of::point_type<SegmentType>::type                              PointType;
    typedef typename viennagrid::result_of::element<SegmentType, typename CellTag::facet_tag>::type   FacetType;
    typedef typename viennagrid::result_of::const_handle<SegmentType, typename CellTag::facet_tag>::type   ConstFacetHandleType;
    typedef typename viennagrid::result_of::element<SegmentType, CellTag>::type     CellType;

    typedef typename viennagrid::result_of::const_element_range<SegmentType, typename CellTag::facet_tag>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;
    typedef typename viennagrid::result_of::handle_iterator<FacetRange>::type                                           FacetHandleIterator;
    
    //typedef typename ConfigType::numeric_type         numeric_type;

    std::set<ConstFacetHandleType>  facets_ptrs_seg1;
    
    //
    // Step 1: Write facets of segment 1 to a map:
    //
    FacetRange facets_seg1 = viennagrid::elements<typename CellTag::facet_tag>(seg1);
    for (FacetHandleIterator fit = facets_seg1.handle_begin();
          fit != facets_seg1.handle_end();
          ++fit)
    {
        const FacetType & facet = viennagrid::dereference_handle(seg1, *fit);
        
      if (is_boundary<CellTypeOrTag>(facet, seg1))
        facets_ptrs_seg1.insert( *fit );
    }
    
    //
    // Step 2: Compare facet in segment 2 with those stored in the map
    //
    FacetRange facets_seg2 = viennagrid::elements<typename CellTag::facet_tag>(seg2);
    for (FacetHandleIterator fit = facets_seg2.handle_begin();
          fit != facets_seg2.handle_end();
          ++fit)
    {
        const FacetType & facet = viennagrid::dereference_handle(seg2, *fit);
        
      if (facets_ptrs_seg1.find( *fit ) != facets_ptrs_seg1.end())
        viennadata::access<KeyType, bool>(key)(facet) = true;
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
            mark_interfacet_cell(*fit, key);
          //interface_setter<CellType, typename CellTag::facet_tag::facet_tag>::apply(*fit, key);
      }
    }
        }
  };
  
  
  
  /** @brief A guard that forces a compile time error if no facets are available (i.e. disabled). */
//   template <typename SegmentType, typename KeyType>
//   void detect_interface_impl(SegmentType const & seg1,
//                              SegmentType const & seg2,
//                              KeyType const & key,
//                              no_handling_tag)
//   {
//     typedef typename SegmentType::ERROR_CANNOT_DETECT_INTERFACE_BECAUSE_FACETS_ARE_DISABLED        error_type;
//   }

  /** @brief Implementation of interface facet detection with complexity O(N log(N)), where N is the number of facets in both segments.
   * 
   * @param seg1     The first segment
   * @param seg2     The second segment
   * @param key      Key object for use with ViennaData.
   */
//   template <typename CellTypeOrTag, typename SegmentType, typename KeyType>
//   void detect_interface_impl(SegmentType const & seg1,
//                              SegmentType const & seg2,
//                              KeyType const & key,
//                              full_handling_tag)
//   {
//     //typedef typename SegmentType::config_type                                  ConfigType;
//     //typedef typename ConfigType::cell_tag                                      CellTag;
//     typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
//     typedef typename viennagrid::result_of::point_type<SegmentType>::type                              PointType;
//     typedef typename viennagrid::result_of::element<SegmentType, typename CellTag::facet_tag>::type   FacetType;
//     typedef typename viennagrid::result_of::const_handle<SegmentType, typename CellTag::facet_tag>::type   ConstFacetHandleType;
//     typedef typename viennagrid::result_of::element<SegmentType, CellTag>::type     CellType;
// 
//     typedef typename viennagrid::result_of::const_element_range<SegmentType, typename CellTag::facet_tag>::type      FacetRange;
//     typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;
//     typedef typename viennagrid::result_of::handle_iterator<FacetRange>::type                                           FacetHandleIterator;
//     
//     //typedef typename ConfigType::numeric_type         numeric_type;
// 
//     std::set<ConstFacetHandleType>  facets_ptrs_seg1;
//     
//     //
//     // Step 1: Write facets of segment 1 to a map:
//     //
//     FacetRange facets_seg1 = viennagrid::elements<typename CellTag::facet_tag>(seg1);
//     for (FacetHandleIterator fit = facets_seg1.handle_begin();
//           fit != facets_seg1.handle_end();
//           ++fit)
//     {
//         FacetType & facet = viennagrid::dereference_handle(seg1, *fit);
//         
//       if (is_boundary(facet, seg1))
//         facets_ptrs_seg1.insert( *fit );
//     }
//     
//     //
//     // Step 2: Compare facet in segment 2 with those stored in the map
//     //
//     FacetRange facets_seg2 = viennagrid::elements<typename CellTag::facet_tag>(seg2);
//     for (FacetIterator fit = facets_seg2.handle_begin();
//           fit != facets_seg2.handle_end();
//           ++fit)
//     {
//         FacetType & facet = viennagrid::dereference_handle(seg2, *fit);
//         
//       if (facets_ptrs_seg1.find( *fit ) != facets_ptrs_seg1.end())
//         viennadata::access<KeyType, bool>(key)(facet) = true;
//     }
//     
//     //
//     // Step 3: Iterate over all facets again and tag all lower level topological elements on facets that belong to the boundary:
//     //
//     for (FacetIterator fit = facets_seg1.begin();
//           fit != facets_seg1.end();
//           ++fit)
//     {
//       if (viennadata::find<KeyType, bool>(key)(*fit) != NULL)
//       {
//         if (viennadata::access<KeyType, bool>(key)(*fit) == true)
//           interface_setter<CellTag, typename CellTag::facet_tag::facet_tag>::apply(*fit, key);
//       }
//     }
//   }



  /** @brief Public interface function for the detection of interface n-cells between two segments. No need to call this function explicitly, since it is called by is_interface()
   * 
   * @param seg1  The first segment
   * @param seg2  The second segment
   * @param key   The key object for ViennaData
   */
  template <typename CellTypeOrTag, typename SegmentType, typename KeyType>
  void detect_interface(SegmentType const & seg1,
                        SegmentType const & seg2,
                        KeyType const & key)
  {
      typedef typename result_of::element_tag<CellTypeOrTag>::type CellTag;
      typedef typename result_of::element<SegmentType, CellTag>::type CellType;
//    typedef typename SegmentType::config_type               ConfigType;
//    typedef typename ConfigType::cell_tag                   CellTag;
//     typedef typename result_of::bndcell_handling<ConfigType, CellTag,
//                                                     CellTag::dim-1>::type  HandlingTag;
    
    if (viennadata::access<KeyType, bool>(key)(seg1) == false)
    {
      detect_interface_impl< CellTag, result_of::has_boundary<CellType, typename CellTag::facet_tag>::value >::detect(seg1, seg2, key);
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
  template <typename CellTypeOrTag, typename DomainType, typename ElementType>
  bool is_interface(ElementType const & el,
                    DomainType const & seg1,
                    DomainType const & seg2)
  {
    typedef interface_key<const DomainType*>    InterfaceKey;
    
    //std::size_t seg_id_lower = std::min(seg1.id(), seg2.id());
    //std::size_t seg_id_higher = std::max(seg1.id(), seg2.id());
    
    InterfaceKey key(&seg1, &seg2);
    detect_interface<CellTypeOrTag>(seg1, seg2, key);
    
    if (viennadata::find<InterfaceKey, bool>(key)(el) != NULL)
      return viennadata::access<InterfaceKey, bool>(key)(el);

    return false;
  }

}

#endif
