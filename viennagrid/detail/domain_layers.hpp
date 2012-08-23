#ifndef VIENNAGRID_DETAIL_DOMAIN_LAYERS_HPP
#define VIENNAGRID_DETAIL_DOMAIN_LAYERS_HPP

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
#include "viennagrid/forwards.h"
#include "viennagrid/point.hpp"
#include "viennagrid/element.hpp"
#include "viennagrid/detail/element_key.hpp"
#include "viennagrid/detail/element_orientation.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/traits/container.hpp"

/** @file domain_layers.hpp
    @brief Provides the topological layers for the domain
*/

namespace viennagrid
{
  namespace detail
  {
    /** @brief Class responsible for holding the segments inside a domain */
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
            segments_[i].domain(*domain_ptr);
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
  

    /** @brief Class holding all n-cells of a particular topological dimension n. Uses recursive inheritance similar to n-cells themselves 
     * 
     * @tparam Config           Configuration class
     * @tparam dim              Topological dimension n of the layer
     * @tparam is_cell          Helper flag for resolving ambiguities at cell level
     * @tparam STOR             The storage scheme used for this layer
     */
    template <typename Config, // config class
              long dim,  // dimension of the elements covered here
              bool is_cell = false,                   // whether this layer holds the cells (i.e. highest topological element)
              typename STOR = typename viennagrid::result_of::bndcell_handling<Config, domain_t<Config>, dim>::type       //Storage scheme: Full storage, or ignore layer
            >
    class domain_layers  : public domain_layers<Config,
                                                dim-1>
    {
        //typedef typename result_of::tag<typename Config::cell_tag, dim>::type    tag;
        typedef domain_t<Config>                                                          domain_type;
        typedef typename topology::bndcells<typename Config::cell_tag, dim>::tag    tag;
        typedef topology::bndcells<tag, 0>                                       VertexOnElementSpecs;
        typedef element_t<Config, tag >                                              element_type;
        typedef element_t<Config, typename Config::cell_tag>                                   cell_type;
        typedef typename result_of::element_container<domain_type, dim, Config::cell_tag::dim>::type           container_type;
        typedef domain_layers<Config,
                              dim-1>                                               base_type;
        typedef element_orientation<VertexOnElementSpecs::num>                    ElementOrientationType;
        typedef element_key<Config, element_type>                                 ElementKeyType;
      
      public:
        typedef Config                                    config_type;
        
        using base_type::push_back;
        
        /** @brief Adds a n-cell to the domain. 
         * 
         * @param elem          The n-cell to be added
         * @param orientation   Pointer to a permutation array
         */
        element_type *
        push_back(element_type & elem, ElementOrientationType * orientation) {

          typedef typename std::map< element_key<Config, element_type>, element_type >::iterator  ElementIterator;
          
          typedef typename result_of::ncell_range<element_type, 0>::type      VertexOnElementRange;
          typedef typename result_of::iterator<element_type, 0>::type         VertexOnElementIterator;

          ElementKeyType epc(elem);
          //check whether already inserted:
          ElementIterator elit = elements.find(epc);
          //std::cout << "Candidate: "; elem.print_short();

          if (elit == elements.end())
          {
            //provide ID for element:
            elem.id(elements.size());

            //std::cout << "ACCEPTED " << std::endl;

            //set default orientation:
            if (orientation != NULL)
              orientation->setDefaultOrientation();

            std::pair<ElementKeyType, element_type> p(epc, elem);
            return &((elements.insert(p).first)->second);
          }

          //std::cout << "REJECTED" << std::endl;
          dim_type i=0; dim_type j=0;
          
          
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
                  if (orientation != NULL)
                    orientation->setPermutation(i,j);   //local (elem) to global (elit->second)
                    //orientation->setPermutation(j,i);   //global (elit->second) to local (elem)
                  break;
                }
              }
              j=0;
          }

          return &(elit->second);
        }


        ///////////////////// container retrieval /////////////////////////

        using base_type::container;

        //non-const:
        container_type * 
        container(dimension_tag<dim>) { return &elements; }
        
        //const:
        const container_type * 
        container(dimension_tag<dim>) const { return &elements; }
        
      private:
        container_type    elements;        //container of elements
    };

    /** @brief Specialization of a domin layer without the storage of n-cells. */
    template <typename Config,
              long dim>
    class domain_layers<Config, dim, false, no_handling_tag> : public domain_layers<Config, dim-1>
    {
        //typedef typename result_of::tag<typename Config::cell_tag, dim>::type    tag;
        typedef typename Config::cell_tag                                                 CellTag;
        typedef domain_t<Config>                                                          domain_type;
        typedef domain_layers<Config, dim-1>        base_type;
        typedef typename result_of::element_container<domain_type, dim, Config::cell_tag::dim>::type           container_type;
      
      public:

        ///////////////////// container retrieval /////////////////////////
        using base_type::container;

        //non-const:
        container_type * 
        container(dimension_tag<dim>)
        {
          typedef typename result_of::bndcell_handling<Config,
                                                          domain_type,
                                                          dim
                                                         >::ERROR_HANDLING_OF_ELEMENTS_AT_THIS_TOPOLOGICAL_LEVEL_NOT_PROVIDED   error_type;
          return NULL;
        }

        //const:
        const container_type * 
        container(dimension_tag<dim>) const
        {
          typedef typename result_of::bndcell_handling<Config,
                                                          domain_type,
                                                          dim
                                                         >::ERROR_HANDLING_OF_ELEMENTS_AT_THIS_TOPOLOGICAL_LEVEL_NOT_PROVIDED   error_type;
          return NULL;
        }
    };
     
    
    // special handling for cells:
    /** @brief Specialization for the cell level */
    template <typename Config,
              long dim>
    class domain_layers<Config, dim, true, full_handling_tag> : public domain_layers<Config,
                                                                                     dim-1>
    {
        //typedef typename result_of::tag<typename Config::cell_tag, 0>::type   tag;
        typedef domain_t<Config>                                                          domain_type;
        typedef element_t<Config, typename Config::cell_tag >                                         element_type;
        typedef typename result_of::element_container<domain_type,
                                                      Config::cell_tag::dim,
                                                      Config::cell_tag::dim>::type                container_type;
        typedef domain_layers<Config,
                              dim-1>                                               base_type;
      
      public:
        typedef Config    config_type;
        typedef std::size_t   size_type;

        using base_type::push_back;
        
        element_type * push_back(const element_type & e)
        {
          assert(viennagrid::traits::capacity(elements) > elements.size() && "Not enough memory for cells reserved!");
          
          elements.push_back(e);
          elements.back().id(elements.size()-1);
          //std::cout << "Filling cell for domain " << this << std::endl;
          elements.back().fill(*this);
          return &(elements.back());
        }
        
        using base_type::container;

        //non-const:
        container_type * 
        container(dimension_tag<dim>) { return &elements; }
        
        //const:
        const container_type * 
        container(dimension_tag<dim>) const { return &elements; }
        
      private:
        container_type    elements;        //container of elements
    };
    
    //terminate recursion at vertex level:
    /** @brief Specialization for the vertex level. Terminates the recursive inheritance */
    template <typename Config,
              bool is_cell,
              typename STOR >
    class domain_layers<Config, 0, is_cell, STOR>
    {
        //typedef typename result_of::tag<typename Config::cell_tag, 0>::type   tag;
        typedef typename result_of::point<Config>::type                                   PointType;
        typedef domain_t<Config>                                                          domain_type;
        typedef element_t<Config, point_tag >                                             element_type;
        typedef element_t<Config, typename Config::cell_tag >                             cell_type;
        typedef typename result_of::element_container<domain_type,
                                                      0,
                                                      Config::cell_tag::dim
                                                      >::type                            container_type;
      
      public:
        typedef Config    config_type;
        typedef std::size_t size_type;
        
        element_type * push_back(element_type const & e)
        {
          assert(viennagrid::traits::capacity(elements) > elements.size() && "Not enough memory for vertices reserved!");
          //element_type temp(e);
          //temp.id(elements.size());
          elements.push_back(e);
          elements.back().id(elements.size()-1);
          return &(elements.back());
        }
        
        element_type * push_back(PointType const & p)
        {
          element_type temp_vertex(p);
          return push_back(temp_vertex);
        }

        //non-const:
        container_type * 
        container(dimension_tag<0>) { return &elements; }
        
        //const:
        const container_type * 
        container(dimension_tag<0>) const { return &elements; }
        
      private:
        container_type    elements;        //container of elements
    };

  } //namespace detail
  
} //namespace viennagrid
#endif
