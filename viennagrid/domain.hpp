#ifndef VIENNAGRID_DOMAIN_HPP
#define VIENNAGRID_DOMAIN_HPP

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


#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include "viennagrid/forwards.hpp"
#include "viennagrid/traits/container.hpp"
#include "viennagrid/detail/domain_iterators.hpp"
#include "viennagrid/detail/domain_layers.hpp"

/** @file domain.hpp
    @brief Provides the main domain type
*/

namespace viennagrid
{


/***************************** Domain Type ***********************/

//   namespace result_of
//   {
//     /** @brief Metafunction returning the container used for storing the n-cells in the domain
//      * 
//      * @tparam ConfigType      The configuration class
//      * @tparam dim             The topological dimension n
//      * @tparam cell_level      Topological dimension of a cell inside the domain
//      */
//     template <typename ConfigType,
//               long dim,
//               long cell_level /* see forwards.h for default argument */>
//     struct element_container< domain_t<ConfigType>, dim, cell_level >
//     {
//       typedef typename result_of::ncell<ConfigType, dim>::type            element_type;
//       
//       typedef std::map< element_key<ConfigType, element_type>, element_type >      type;
//     };
// 
//     //at point level:
//     /** @brief Metafunction returning the container used for storing the n-cells in the domain. Specialization for a vertices */
//     template <typename ConfigType, long cell_level>
//     struct element_container< domain_t<ConfigType>, 0, cell_level>
//     {
//       typedef typename result_of::ncell<ConfigType, 0>::type              element_type;
//       
//       //typedef std::vector< element_type >      type;
//       typedef std::deque< element_type >      type;
//     };
// 
//     //at cell level:
//     /** @brief Metafunction returning the container used for storing the n-cells in the domain. Specialization for a cells */
//     template <typename ConfigType, long cell_level>
//     struct element_container< domain_t<ConfigType>, cell_level, cell_level>
//     {
//       typedef typename result_of::ncell<ConfigType, cell_level>::type     element_type;
//       
//       //typedef std::vector< element_type >      type;
//       typedef std::deque< element_type >      type;
//     };
//     
//     /** @brief Metafunction for returning the domain type for a given configuration class */
//     template <typename ConfigType>
//     struct domain
//     {
//       typedef domain_t<ConfigType>     type; 
//     };
//     
//   }
//   
//   
// 
//   
//   /** @brief The domain class. Assembled at compile time by recursive inheritance for each topological level by the detail::domain_layers class
//    * 
//    * 
//    * @tparam Config   The configuration class   
//    */
//   template <typename Config>
//   class domain_t : public detail::domain_layers<Config,
//                                                 Config::cell_tag::dim,
//                                                 true,
//                                                 full_handling_tag>  //we start with cells
//   {
//       typedef detail::domain_layers<Config, Config::cell_tag::dim, true>           base_type;
//       typedef domain_t<Config>                                                          self_type;
//     
//     public:
//       /** @brief Publishes the configuration class */
//       typedef Config                                    config_type;
//       /** @brief STL-like typedef */
//       typedef std::size_t                               size_type;
//       /** @brief Publish the segment type */
//       typedef segment_t<Config>                         segment_type;
//       
//       typedef detail::domain_segment_container<self_type, segment_type>    segment_container;
//       
//       /** @brief The default constructor.
//        * 
//        * Note: Visual Studio complains about the use of 'this'. However, we just need the address, so this is perfectly safe, since the pointer is not dereferenced.
//        */
//       domain_t() : segments_(this) {}
// 
//       /** @brief Constructor triggering the refinement of the domain */
//       template <typename OtherDomainType, typename RefinementTag>
//       domain_t(refinement_proxy<OtherDomainType, RefinementTag> const & proxy) : segments_(this)
//       {
//         detail::refine_impl(proxy.get(), *this, proxy.tag());
//       }
// 
//       /** @brief Destructor. Cleans up any data associated with the domain using ViennaData */
//       ~domain_t() { viennadata::erase<viennadata::all, viennadata::all>()(*this); }
// 
//       using base_type::push_back;
//       
//       /** @brief Triggers refinement of a domain and fills this domain with the new, refined mesh */
//       template <typename OtherDomainType, typename RefinementTag>
//       self_type & operator=(refinement_proxy<OtherDomainType, RefinementTag> const & proxy)
//       {
//         detail::refine_impl(proxy.get(), *this, proxy.tag());
//         return *this;
//       }
//       
//       /** @brief Return the container of segments */
//       segment_container & segments() { return segments_; }
//       /** @brief Return the container of segments. const-version. */
//       segment_container const & segments() const { return segments_; }
//     
//     private:
//       segment_container segments_;
//   };
// 
//   /** @brief Convenience function for accessing the segments of the domain. Equivalent to domain.segments() */
//   template <typename ConfigType>
//   typename domain_t<ConfigType>::segment_container &
//   segments(domain_t<ConfigType> & domain)
//   {
//     return domain.segments(); 
//   }
//   
//   /** @brief Convenience function for accessing the segments of the domain. Equivalent to domain.segments() */
//   template <typename ConfigType>
//   typename domain_t<ConfigType>::segment_container const &
//   segments(domain_t<ConfigType> const & domain)
//   {
//     return domain.segments(); 
//   }
  
}
#endif
