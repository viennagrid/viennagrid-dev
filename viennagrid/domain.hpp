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

#ifndef VIENNAGRID_DOMAIN_GUARD
#define VIENNAGRID_DOMAIN_GUARD


#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include "viennagrid/forwards.h"
//#include "viennagrid/topology/celltags.hpp"  
#include "viennagrid/point.hpp"
#include "viennagrid/element.hpp"
#include "viennagrid/detail/element_key.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/traits/container.hpp"
//#include "viennagrid/config/domain_configs.hpp"

namespace viennagrid
{


/***************************** Domain Type ***********************/

  namespace result_of
  {
    template <typename config_type,
              dim_type dim,
              dim_type cell_level /* see forwards.h for default argument */>
    struct element_container< domain<config_type>, dim, cell_level >
    {
      typedef typename result_of::ncell<config_type, dim>::type            element_type;
      
      typedef std::map< element_key<element_type>, element_type >      type;
    };

    //at point level:
    template <typename config_type, dim_type cell_level>
    struct element_container< domain<config_type>, 0, cell_level>
    {
      typedef typename result_of::ncell<config_type, 0>::type              element_type;
      
      //typedef std::vector< element_type >      type;
      typedef std::deque< element_type >      type;
    };

    //at cell level:
    template <typename config_type, dim_type cell_level>
    struct element_container< domain<config_type>, cell_level, cell_level>
    {
      typedef typename result_of::ncell<config_type, cell_level>::type     element_type;
      
      //typedef std::vector< element_type >      type;
      typedef std::deque< element_type >      type;
    };
    
  }
  
  
  
  template <typename DomainType, typename SegmentType>
  class domain_segment_container
  {
      typedef std::deque<SegmentType>     container_type;
    
    public:
      typedef typename container_type::iterator           iterator;
      typedef typename container_type::const_iterator     const_iterator;
      typedef typename container_type::value_type         value_type;
      typedef typename container_type::reference          reference;
      typedef typename container_type::const_reference    const_reference;
      typedef typename container_type::difference_type    difference_type;
      typedef typename container_type::pointer            pointer;
      typedef typename container_type::size_type          size_type;
      
      domain_segment_container(DomainType * d) : domain_ptr(d) {}
      
      SegmentType & operator[](std::size_t i) { return segments_[i]; }
      SegmentType const & operator[](std::size_t i) const { return segments_[i]; }
      
      SegmentType & at(std::size_t i) { return segments_.at(i); }
      SegmentType const & at(std::size_t i) const { return segments_.at(i); }
      
      std::size_t size() const { return segments_.size(); }
      std::size_t max_size() const { return segments_.max_size(); }
      bool empty() const { return segments_.empty(); }
      
      void resize(std::size_t n)
      {
        std::size_t old_size = segments_.size();
        segments_.resize(n);
        for (std::size_t i=old_size; i<n; ++i)
          segments_[i].set_domain(*domain_ptr);
      }
      
      iterator begin() { return segments_.begin(); }
      const_iterator begin() const { return segments_.begin(); }
      
      iterator end() { return segments_.end(); }
      const_iterator end() const { return segments_.end(); }

      void swap(domain_segment_container & other)
      {
        assert(domain_ptr == other.domain_ptr);
        segments_.swap(other.segments_);
      }
    private:
      container_type segments_;
      DomainType * domain_ptr;
  };
  


  template <typename Config, // config class
            dim_type dim,  // dimension of the elements covered here
            bool is_cell = false,                   // whether this layer holds the cells (i.e. highest topological element)
            typename STOR = full_handling_tag       //Storage scheme: Full storage, or ignore layer
           >
  class domain_layers  : public domain_layers<Config, dim-1>
  {
      //typedef typename result_of::element_tag<typename Config::cell_tag, dim>::type    element_tag;
      typedef domain<Config>                                                          domain_type;
      typedef typename topology::subcell_desc<typename Config::cell_tag, dim>::element_tag    element_tag;
      typedef topology::subcell_desc<element_tag, 0>                                       VertexOnElementSpecs;
      typedef element<Config, element_tag >                                              element_type;
      typedef element<Config, typename Config::cell_tag>                                   cell_type;
      typedef typename result_of::element_container<domain_type, dim, Config::cell_tag::topology_level>::type           container_type;
      typedef domain_layers<Config, dim-1>                                               base_type;
      typedef element_orientation<VertexOnElementSpecs::num_elements>                    ElementOrientationType;
    
    public:
      typedef Config                                    config_type;
      
      using base_type::add;
      
      /*
      element_type * add(element_type & e)
      {
        e.id(elements.size());
        elements.push_back(e);
        elements.back().fill(*this);
        return &(elements.back());
      } */

      element_type *
      add(element_type & elem, ElementOrientationType * orientation) {

        typedef typename std::map< element_key<element_type>, element_type >::iterator  ElementIterator;
        
        typedef typename result_of::ncell_range<element_type, 0>::type      VertexOnElementRange;
        typedef typename result_of::iterator<element_type, 0>::type         VertexOnElementIterator;

        element_key<element_type> epc(elem);
        //check whether already inserted:
        ElementIterator elit = elements.find(epc);
        //std::cout << "Candidate: "; elem.print_short();

        if (elit == elements.end())
        {
          //provide ID for element:
          elem.id(elements.size());

          //std::cout << "ACCEPTED " << std::endl;

          //set default orientation:
          orientation->setDefaultOrientation();

          std::pair<element_key<element_type>, element_type> p(epc, elem);
          return &((elements.insert(p).first)->second);
        }

        //std::cout << "REJECTED" << std::endl;
        long i=0; long j=0;
        
        
        //set orientation:
        VertexOnElementRange vertices_on_element = ncells<0>(elem);
        for (VertexOnElementIterator voeit = vertices_on_element.begin();
              voeit != vertices_on_element.end();
              ++voeit, ++i)
        {
            
            VertexOnElementRange vertices_on_element_2 = ncells<0>(elit->second);
            for (VertexOnElementIterator voeit2 = vertices_on_element_2.begin();
                  voeit2 != vertices_on_element_2.end();
                  ++voeit2, ++j)
            {
              if (voeit->id() == voeit2->id())
              {
                //orientation->setPermutation(i,j);   //local (elem) to global (elit->second)
                orientation->setPermutation(j,i);   //global (elit->second) to local (elem)
                break;
              }
            }
            j=0;
        }

        return &(elit->second);
      }


      ///////////////////// container retrieval /////////////////////////

      //non-const:
      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) { return &elements; }

      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(less_tag) { return base_type::template container<dim_container>(); }

      //const:
      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) const { return &elements; }

      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(less_tag) const { return base_type::template container<dim_container>(); }
      
      
      ////////////////////// size ////////////////////////
      template <dim_type j>
      size_t size(less_tag) const { return base_type::template size<j>(); }
      template <dim_type j>
      size_t size(equal_tag) const { return elements.size(); }
      template <dim_type j>
      size_t size() const
      {
        return size<j>( typename level_discriminator<dim, j>::result_type() );
      }
      
      
    private:
      container_type    elements;        //container of elements
  };
  
  
  // special handling for cells:
  template <typename Config,
            dim_type dim,
            typename STOR >
  class domain_layers<Config, dim, true, STOR> : public domain_layers<Config, dim-1, false, STOR>
  {
      //typedef typename result_of::element_tag<typename Config::cell_tag, 0>::type   element_tag;
      typedef domain<Config>                                                          domain_type;
      typedef element<Config, typename Config::cell_tag >                                         element_type;
      typedef typename result_of::element_container<domain_type,
                                                    Config::cell_tag::topology_level,
                                                    Config::cell_tag::topology_level>::type                container_type;
      typedef domain_layers<Config, dim-1>                                               base_type;
    
    public:
      typedef Config    config_type;
      typedef std::size_t   size_type;

      void reserve_cells(size_type num) 
      {
        viennagrid::traits::reserve(elements, num); 
      }
      
      using base_type::add;
      
      element_type * add(element_type & e)
      {
        assert(viennagrid::traits::capacity(elements) > elements.size() && "Not enough memory for cells reserved!");
        
        elements.push_back(e);
        elements.back().id(elements.size());
        //std::cout << "Filling cell for domain " << this << std::endl;
        elements.back().fill(*this);
        return &(elements.back());
      }
      
      //non-const:
      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container() { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) { return &elements; }

      template <dim_type dim_container>
      typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(less_tag) { return base_type::template container<dim_container>(); }

      //const:
      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container() const { return container<dim_container>(typename level_discriminator<dim, dim_container>::result_type()); }
      
      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(equal_tag) const { return &elements; }

      template <dim_type dim_container>
      const typename result_of::element_container<domain_type, dim_container, Config::cell_tag::topology_level>::type * 
      container(less_tag) const { return base_type::template container<dim_container>(); }
      
      ////////////////////// size ////////////////////////
      template <dim_type j>
      size_type size(less_tag) const { return base_type::template size<j>(); }
      template <dim_type j>
      size_type size(equal_tag) const { return elements.size(); }
      template <dim_type j>
      size_type size() const
      {
        return size<j>( typename level_discriminator<dim, j>::result_type() );
      }
      
      element_type cells(size_type i) { return elements[i]; }
      element_type cells(size_type i) const { return elements[i]; }      
      
    private:
      container_type    elements;        //container of elements
  };
  
  //terminate recursion at vertex level:
  template <typename Config,
            bool is_cell,
            typename STOR >
  class domain_layers<Config, 0, is_cell, STOR>
  {
      //typedef typename result_of::element_tag<typename Config::cell_tag, 0>::type   element_tag;
      typedef domain<Config>                                                          domain_type;
      typedef element<Config, point_tag >                                             element_type;
      typedef element<Config, typename Config::cell_tag >                             cell_type;
      typedef typename result_of::element_container<domain_type,
                                                    0,
                                                    Config::cell_tag::topology_level
                                                    >::type                            container_type;
    
    public:
      typedef Config    config_type;
      typedef std::size_t size_type;
      
      void reserve_vertices(size_type num) 
      {
        viennagrid::traits::reserve(elements, num);
      }
      
      element_type * add(element_type const & e)
      {
        assert(viennagrid::traits::capacity(elements) > elements.size() && "Not enough memory for vertices reserved!");
        //element_type temp(e);
        //temp.id(elements.size());
        elements.push_back(e);
        elements.back().id(elements.size()-1);
        return &(elements.back());
      }
      
      element_type & vertex(size_type id) 
      {
        assert(id < elements.size() && "Vertex index out of bounds!");
        return elements[id]; 
      }
      
      template <dim_type dim>
      container_type * container() { return &elements; }
      template <dim_type dim>
      const container_type * container() const { return &elements; }
      
      ////////////////////// size ////////////////////////
      template <dim_type j>
      size_type size(equal_tag) const { return elements.size(); }
      template <dim_type j>
      size_type size() const
      {
        return size<j>( typename level_discriminator<0, j>::result_type() );
      }
      
      
    private:
      container_type    elements;        //container of elements
  };

  
  
  

  template <typename Config>
  class domain : public domain_layers<Config,
                                      Config::cell_tag::topology_level,
                                      true>  //we start with cells
  {
      typedef domain_layers<Config, Config::cell_tag::topology_level, true>           base_type;
      typedef domain<Config>                                                          self_type;
    
    public:
      typedef Config                                    config_type;
      typedef std::size_t                               size_type;
      typedef segment_t<Config>                         segment_type;
      
      typedef domain_segment_container<self_type, segment_type>    segment_container;
      
      domain() : segments_(this) {}
      
      using base_type::add;
      
      template <typename OtherDomainType, typename RefinementTag>
      self_type & operator=(refinement_proxy<OtherDomainType, RefinementTag> const & proxy)
      {
        refine_impl(proxy.get(), *this, proxy.tag());
        return *this;
      }
      
      /*
      void segment_resize(size_type num)
      {
        assert(segments.size() == 0 || "Segments in domain already created!");
        segments.resize(num);
        
        for (size_t i=0; i<segments.size(); ++i)
        {
          segments[i].set_domain(*this);
        }
      }

      segment_type const& segment(size_type seg_index) const
      {
        assert(seg_index < segments.size() || "Segment index out of bounds!");
        return segments[seg_index];
      }

      segment_type & segment(size_type seg_index)
      {
        assert(seg_index < segments.size() || "Segment index out of bounds!");
        return segments[seg_index];
      }

      const std::vector< segment_type > * segment_container() const { return & segments; }
      const std::vector< segment_type > * segment_container()       { return & segments; }      
      
      size_type segment_size() const { return segments.size(); } */
      
      segment_container & segments() { return segments_; }
      segment_container const & segments() const { return segments_; }
    
    private:
      //store segments here
      segment_container segments_;
  };

  
  template <typename ConfigType>
  typename domain<ConfigType>::segment_container &
  segments(domain<ConfigType> & domain)
  {
    return domain.segments(); 
  }
  
  template <typename ConfigType>
  typename domain<ConfigType>::segment_container const &
  segments(domain<ConfigType> const & domain)
  {
    return domain.segments(); 
  }
  
}
#endif
