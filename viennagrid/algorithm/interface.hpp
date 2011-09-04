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

#ifndef VIENNAGRID_ALGORITHM_INTERFACE_HPP
#define VIENNAGRID_ALGORITHM_INTERFACE_HPP

#include <vector>
#include "viennagrid/forwards.h"
#include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/detail/domain_iterators.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/boundary.hpp"

namespace viennagrid
{

  class interface_key
  {
    public:
      interface_key(std::size_t id1, std::size_t id2) : id_(id1, id2) {}
      
      bool operator<(interface_key const & other) const
      {
        return id_ < other.id_;
      }
      
    private:
      std::pair<std::size_t, std::size_t> id_;
  };


  //helper struct for setting interface flag of lower level elements of a facet
  template <long topology_level>
  struct interface_setter
  {
    template <typename FacetType, typename KeyType>
    static void apply(FacetType const & facet, KeyType const & key, full_handling_tag)
    {
      typedef typename FacetType::config_type        ConfigType;
      typedef typename ConfigType::cell_tag          CellTag;
      
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
      interface_setter<topology_level - 1>::apply(facet, key, typename viennagrid::result_of::subelement_handling<CellTag, topology_level-1>::type());
    }
    
    template <typename FacetType, typename KeyType>
    static void apply(FacetType const & facet, KeyType const & key, no_handling_tag)
    {
      typedef typename FacetType::config_type        ConfigType;
      typedef typename ConfigType::cell_tag          CellTag;
      
      //proceed to lower level:
      interface_setter<topology_level - 1>::apply(facet, key, typename viennagrid::result_of::subelement_handling<CellTag, topology_level-1>::type());
    }
    
  };

  //end recursion of topolevel = -1
  template <>
  struct interface_setter< -1 >
  {
    template <typename FacetType, typename KeyType, typename HandlingTag>
    static void apply(FacetType const & facet, KeyType const & key, HandlingTag)
    {
    }
  };

  
  //
  // A point comparison that is stable with respect to round-off errors.
  // Can be further improved by using tolerances for each coordinate component separately
  // Needs some more tweaks for non-Cartesian coordinates
  //
  /*
  template <typename NumericType = double>
  class fuzzy_point_less
  {
    public:
      fuzzy_point_less(NumericType tol_abs = 1e-10,
                       NumericType tol_rel = 1e-12) : tol_abs_(tol_abs), tol_rel_(tol_rel) {}
      
      template <typename PointType>
      bool operator()(PointType const & p1, PointType const & p2) const
      {
        assert(p1.size() == p2.size() && "Points must have the same size!");
        
        for (std::size_t i=0; i<p1.size(); ++i)
        {
          if (fabs(p1[i]) < tol_abs_) && fabs(p2[i]) < tol_abs_)  //use absolute tolerance (coordinates are close to zero:
            continue;   //coordinates are equal up to round-off errors near zero
          
          NumericType rel_diff = (p1[i] - p2[i]) / std::max(std::fabs(p1[i]), std::fabs(p2[i]));
          if ( rel_diff < -tol_rel_)
            return true;
          else if ( rel_diff  > tol_rel_)
            return false;
        }
        return false;
      }
      
    private:
      NumericType tol_rel_;
      NumericType tol_abs_;
  };*/
  

  template <typename SegmentType, typename KeyType>
  void detect_interface_impl(SegmentType const & seg1,
                             SegmentType const & seg2,
                             KeyType const & key,
                             no_handling_tag)
  {
    typedef typename SegmentType::ERROR_CANNOT_DETECT_INTERFACE_BECAUSE_FACETS_ARE_DISABLED        error_type;
  }

  //
  // Interface facet detection with complexity O(N log(N)), where N is the number of facets in both segments.
  //
  template <typename SegmentType, typename KeyType>
  void detect_interface_impl(SegmentType const & seg1,
                             SegmentType const & seg2,
                             KeyType const & key,
                             full_handling_tag)
  {
    typedef typename SegmentType::config_type                                  ConfigType;
    typedef typename ConfigType::cell_tag                                      CellTag;
    typedef typename viennagrid::result_of::point<ConfigType>::type                              PointType;
    typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::topology_level-1>::type   FacetType;
    typedef typename viennagrid::result_of::ncell<ConfigType, CellTag::topology_level>::type     CellType;

    typedef typename viennagrid::result_of::const_ncell_range<SegmentType, CellTag::topology_level-1>::type      FacetRange;
    typedef typename viennagrid::result_of::iterator<FacetRange>::type                                           FacetIterator;
    
    typedef typename ConfigType::numeric_type         numeric_type;

    // Get an estimate for the segment length scale from the distance of two random vertices:
    // (If the domain is in the size of nanometer (10^-9), a fixed absolute tolerance of 10^{-10} would lead to wrong results.
    //  Similarly, if the domain is in the size of lightyears, a fixed absolute tolerance of 10^{-10} is again useless...)
    //double abs_tol = viennagrid::norm( viennagrid::ncells<0>(seg1)[0].point() - viennagrid::ncells<0>(seg1)[1].point() ) * 1e-8;
    //std::map<PointType,
    //         FacetType const *,
    //         fuzzy_point_less<double> > facet_centroids_seg1( fuzzy_point_less<double>(abs_tol) );
    std::set<FacetType const *>  facets_ptrs_seg1;
    
    //
    // Step 1: Write facets of segment 1 to a map:
    //
    FacetRange facets_seg1 = viennagrid::ncells<CellTag::topology_level-1>(seg1);
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
    FacetRange facets_seg2 = viennagrid::ncells<CellTag::topology_level-1>(seg2);
    for (FacetIterator fit = facets_seg2.begin();
          fit != facets_seg2.end();
          ++fit)
    {
      if (facets_ptrs_seg1.find( &(*fit) ) != facets_ptrs_seg1.end())
      {
        viennadata::access<KeyType, bool>(key)(*fit) = true;
      }
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
          interface_setter<CellTag::topology_level-2>::apply(*fit,
                                                             key,
                                                             typename viennagrid::result_of::subelement_handling<CellTag, CellTag::topology_level-2>::type()
                                                            );
      }
    }
  }



  //
  // public interface functions:
  //

  template <typename SegmentType, typename KeyType>
  void detect_interface(SegmentType const & seg1,
                        SegmentType const & seg2,
                        KeyType const & key)
  {
    typedef typename SegmentType::config_type               ConfigType;
    typedef typename ConfigType::cell_tag                   CellTag;
    typedef typename result_of::subelement_handling<CellTag,
                                                    CellTag::topology_level-1>::type  HandlingTag;
    
    if (viennadata::access<KeyType, bool>(key)(seg1) == false)
    {
      detect_interface_impl(seg1, seg2, key, HandlingTag());
      viennadata::access<KeyType, bool>(key)(seg1) = true;
      viennadata::access<KeyType, bool>(key)(seg2) = true;
    }
  }


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
    {
      return viennadata::access<InterfaceKey, bool>(key)(el);
    }
    return false;
  }

}

#endif
