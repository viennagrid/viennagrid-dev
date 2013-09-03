#ifndef VIENNAGRID_ELEMENT_DELETION_HPP
#define VIENNAGRID_ELEMENT_DELETION_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/forwards.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"
#include <iterator>


/** @file element_deletion.hpp
    @brief Contains functions for deleting elements from a domin
*/


namespace viennagrid
{
  /** @brief For internal use only */
  template<typename domain_type, typename to_switch_element_handle>
  struct switch_handle_functor
  {
      typedef typename viennagrid::storage::handle::result_of::value_type<to_switch_element_handle>::type to_switch_element_type;

      switch_handle_functor(domain_type & domain_, to_switch_element_handle from_, to_switch_element_handle to_) : domain(domain_), from(from_), to(to_) {}

      template<typename parent_element_type_or_tag>
      void operator() ( viennagrid::meta::tag<parent_element_type_or_tag> )
      {
          typedef typename viennagrid::result_of::element<domain_type, parent_element_type_or_tag>::type parent_element_type;
          typedef typename viennagrid::result_of::element_range<domain_type, parent_element_type_or_tag>::type parent_element_range_type;
          typedef typename viennagrid::result_of::iterator<parent_element_range_type>::type parent_element_range_iterator;

          parent_element_range_type parent_elements = viennagrid::elements(domain);
          for (parent_element_range_iterator it = parent_elements.begin(); it != parent_elements.end(); ++it)
          {
              typedef typename viennagrid::result_of::element_range<parent_element_type, to_switch_element_type>::type to_switch_element_range_type;
              typedef typename viennagrid::result_of::iterator<to_switch_element_range_type>::type to_switch_element_range_iterator;

              to_switch_element_range_type to_switch_elements = viennagrid::elements(*it);
              for (to_switch_element_range_iterator jt = to_switch_elements.begin(); jt != to_switch_elements.end(); ++jt)
              {
                  if (jt.handle() == from)
                  {
                      jt.handle() = to;
                  }
              }
          }
      }

      domain_type & domain;
      to_switch_element_handle from;
      to_switch_element_handle to;
  };



  /** @brief For internal use only */
  template<typename domain_type, typename handle_type>
  void switch_handle( domain_type & domain, handle_type old_handle, handle_type new_handle)
  {
      typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type to_switch_element_type;
      typedef typename viennagrid::result_of::referencing_element_typelist<domain_type, to_switch_element_type>::type parent_element_typelist;

      switch_handle_functor<domain_type, handle_type> functor(domain, old_handle, new_handle);

      viennagrid::meta::typelist::for_each<parent_element_typelist>( functor );
  }



  /** @brief For internal use only */
  template<typename domain_type, typename domain_view_type, typename handle_type, typename referencing_element_typelist =
      typename viennagrid::result_of::referencing_element_typelist<domain_type, typename viennagrid::storage::handle::result_of::value_type<handle_type>::type >::type >
  struct mark_erase_elements_impl;

  template<typename domain_type, typename domain_view_type, typename handle_type, typename coboundary_element_type, typename tail>
  struct mark_erase_elements_impl<domain_type, domain_view_type, handle_type, viennagrid::meta::typelist_t<coboundary_element_type, tail> >
  {
      static void mark(domain_type & domain, domain_view_type & domain_view, handle_type to_erase)
      {
          typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type element_type;

          typedef typename viennagrid::result_of::element_range<domain_view_type, element_type>::type element_range_type;
          typedef typename viennagrid::result_of::iterator<element_range_type>::type element_range_iterator;

          element_range_type view_elements = viennagrid::elements( domain_view );
          if ( viennagrid::find_by_handle(domain_view, to_erase) == view_elements.end() )
          {
              view_elements.insert_unique_handle( to_erase );
          }

          typedef typename viennagrid::result_of::handle<domain_type, coboundary_element_type>::type coboundary_element_handle;
          typedef typename viennagrid::result_of::coboundary_range<domain_type, element_type, coboundary_element_type>::type coboundary_element_range_type;
          typedef typename viennagrid::result_of::iterator<coboundary_element_range_type>::type coboundary_element_range_iterator;

          coboundary_element_range_type coboundary_elements = viennagrid::coboundary_elements<element_type, coboundary_element_type>(domain, to_erase);
          for (coboundary_element_range_iterator it = coboundary_elements.begin(); it != coboundary_elements.end(); ++it)
          {
              mark_erase_elements_impl<domain_type, domain_view_type, coboundary_element_handle, tail>::mark(domain, domain_view, it.handle());
          }
      }
  };

  /** @brief For internal use only */
  template<typename domain_type, typename domain_view_type, typename handle_type>
  struct mark_erase_elements_impl<domain_type, domain_view_type, handle_type, viennagrid::meta::null_type >
  {
      static void mark(domain_type &, domain_view_type & domain_view, handle_type to_erase)
      {
          typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type element_type;

          typedef typename viennagrid::result_of::element_range<domain_view_type, element_type>::type element_range_type;
          typedef typename viennagrid::result_of::iterator<element_range_type>::type element_range_iterator;

          element_range_type view_elements = viennagrid::elements( domain_view );
          if ( viennagrid::find_by_handle(domain_view, to_erase) == view_elements.end() )
          {
              view_elements.insert_unique_handle( to_erase );
          }
      }
  };




  /** @brief Marks an element for deletion
    *
    * @tparam DomainT                   The domain type in which the element to erase lives
    * @tparam DomainViewT               The domain view type for all elements to erase
    * @tparam HandleT                   The handle type of the element to delete
    * @param  domain                    The host domain object
    * @param  elements_to_erase         A domain view which stores all elements marked for deletion
    * @param  to_erase                  A handle object referencing the element to delete
    */
  template<typename DomainT, typename DomainViewT, typename HandleT>
  void mark_erase_elements( DomainT & domain, DomainViewT & elements_to_erase, HandleT to_erase )
  {
      mark_erase_elements_impl<DomainT, DomainViewT, HandleT>::mark(domain, elements_to_erase, to_erase);
  }

  /** @brief For internal use only */
  template<typename domain_type, typename handle_type>
  void simple_erase_element(domain_type & domain, handle_type & element_to_erase)
  {
      typedef typename viennagrid::storage::handle::result_of::value_type<handle_type>::type element_type;
      typedef typename viennagrid::result_of::handle<domain_type, element_type>::type element_handle;

      typedef typename viennagrid::result_of::element_range<domain_type, element_type>::type element_range_type;
      typedef typename viennagrid::result_of::iterator<element_range_type>::type element_range_iterator;

      element_range_type elements = viennagrid::elements( domain );

      element_range_iterator to_erase_it = find_by_handle( domain, element_to_erase );

      element_handle from = (--elements.end()).handle();
      element_handle to = to_erase_it.handle();

      std::swap( *to_erase_it, *(--elements.end()) );
      elements.erase( --elements.end() );

      std::cout << "Switching " << viennagrid::dereference_handle(domain, from) << " with " << viennagrid::dereference_handle(domain, to) << std::endl;

      switch_handle( domain, from, to );
  }

  /** @brief For internal use only */
  template<typename domain_type, typename domain_view_type>
  struct erase_functor
  {
      erase_functor(domain_type & domain_, domain_view_type & view_to_erase_) : domain(domain_), view_to_erase(view_to_erase_) {}

      template<typename element_type>
      void operator()( viennagrid::meta::tag<element_type> )
      {
          typedef typename viennagrid::result_of::element_range<domain_view_type, element_type>::type to_erase_element_range_type;
          typedef typename viennagrid::result_of::iterator<to_erase_element_range_type>::type to_erase_element_range_iterator;

          typedef typename viennagrid::result_of::handle<domain_type, element_type>::type element_handle;
          typedef typename viennagrid::result_of::element_range<domain_type, element_type>::type element_range_type;
          typedef typename viennagrid::result_of::iterator<element_range_type>::type element_range_iterator;

          typedef typename viennagrid::result_of::id<element_type>::type id_type;

//             std::vector< std::pair<element_range_iterator, element_range_iterator> > swtiching_map;

          std::deque<id_type> ids_to_erase;

          to_erase_element_range_type elements_to_erase = viennagrid::elements<element_type>(view_to_erase);
          for (to_erase_element_range_iterator it = elements_to_erase.begin(); it != elements_to_erase.end(); ++it)
            ids_to_erase.push_back( it->id() );

          element_range_type elements = viennagrid::elements(domain);
          element_range_iterator back_it = --elements.end();


          for (typename std::deque<id_type>::iterator it = ids_to_erase.begin(); it != ids_to_erase.end(); ++it)
          {
              element_range_iterator to_erase_it = find( domain, *it );

              if (back_it != to_erase_it)
              {
                element_handle old_handle = back_it.handle();
                std::swap( *back_it, *to_erase_it );
                element_handle new_handle = to_erase_it.handle();

                switch_handle( domain, old_handle, new_handle );
              }


              elements.erase( back_it );

              back_it--;
          }
      }

      domain_type & domain;
      domain_view_type & view_to_erase;
  };

  /** @brief Erases all elements marked for deletion and all elements which references these elements from a domain
    *
    * @tparam DomainT                   The domain type in which the elements to erase live
    * @tparam DomainViewT               The domain view type which stores all elements to erase
    * @param  domain                    The host domain object
    * @param  elements_to_erase         A domain view which stores all elements marked for deletion
    */
  template<typename DomainT, typename DomainViewT>
  void erase_elements(DomainT & domain, DomainViewT & elements_to_erase)
  {
      typedef typename viennagrid::meta::typelist::result_of::reverse<
        typename viennagrid::result_of::element_typelist<DomainViewT>::type
      >::type element_typelist;

      erase_functor<DomainT, DomainViewT> functor( domain, elements_to_erase );
      viennagrid::meta::typelist::for_each<element_typelist>(functor);
  }

  /** @brief Erases a single elements all elements which references this element from a domain. If more than one element is erase use erase_elements instead for better performance.
    *
    * @tparam DomainT                   The domain type in which the elements to erase live
    * @tparam HandleT                   The handle type of the element to delete
    * @param  domain                    The host domain object
    * @param  element_to_erase          A handle to the element to be deleted
    */
  template<typename DomainT, typename HandleT>
  void erase_element(DomainT & domain, HandleT & element_to_erase)
  {
      typedef typename viennagrid::result_of::domain_view<DomainT>::type DomainViewType;
      DomainViewType elements_to_erase = viennagrid::make_view(domain);
      viennagrid::mark_erase_elements( domain, elements_to_erase, element_to_erase );
      viennagrid::erase_elements(domain, elements_to_erase);
  }
}


#endif
