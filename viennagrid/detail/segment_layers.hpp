#ifndef VIENNAGRID_DETAIL_SEGMENT_LAYERS_HPP
#define VIENNAGRID_DETAIL_SEGMENT_LAYERS_HPP

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


#include <map>
#include <set>
#include <deque>

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/detail/element_key.hpp"

/** @file segment_layers.hpp
    @brief Provides the topological layers for segments
*/

namespace viennagrid
{

  /////////////// Segment is a container for elements ////////////////
  
  namespace detail
  {
    /** @brief Helper class for comparing the ID of n-cells, for which only the pointers are available. Used as comparison in std::map */
    struct id_ptr_compare
    {
      template <typename ElementType>
      bool operator()(ElementType * e1, ElementType * e2) const
      {
        return e1->id() < e2->id();
      }
    };
    
    /** @brief Class that holds a reference to the domain the segment belongs to */
    template <typename Conf>
    class segment_domain_holder
    {
        typedef viennagrid::domain_t<Conf>                         domain_type;
      public:
        void domain(domain_type & d) { domain_ = &d; }

        domain_type & domain() { return * domain_; }
        domain_type const & domain() const { return * domain_; }

        std::size_t id() const 
        {
          for (std::size_t i=0; i<domain_->segments().size(); ++i)
          {
            if (this == &(domain_->segments()[i]))
              return i;
          }
          
          assert(false && "Segment ID cannot be found!");
          return domain_->segments().size();
        }
        
      protected:
        domain_type * domain_;
    };
    
    template <typename Config,
              long dim>
    class segment_layers_full;

    template <typename Config,
              long dim>
    class segment_layers_empty;
    
    /** @brief Helper class that returns the next class type for recursive inheritance. Was introduced due to Visual Studio compilation problems of the first approach */
    template <typename Config,
              long dim,
              typename handling_tag = typename result_of::bndcell_handling<Config, segment_t<Config>, dim>::type
              >
    struct next_segment_layer_selector
    {
      typedef segment_layers_full<Config, dim>    type;
    };
    
    template <typename Config, long dim>
    struct next_segment_layer_selector<Config, dim, no_handling_tag>
    {
      typedef segment_layers_empty<Config, dim>   type;
    };
    
    template <typename Config, typename handling_tag>
    struct next_segment_layer_selector<Config, 0, handling_tag>
    {
      typedef segment_layers_full<Config, 0>    type;
    };
    
    
    /** @brief Full handling of a particular topological dimension of a segment */
    template <typename Config,
              long dim>
    class segment_layers_full : //public segment_layers<Config, 0, handling_tag>
                          public next_segment_layer_selector<Config, dim-1>::type
                          //public segment_domain_holder<Config>
    {
        typedef typename Config::cell_tag                                      CellTag;
        typedef typename viennagrid::topology::bndcells<CellTag, dim>::tag     ElementTag;
        typedef viennagrid::element_t<Config, ElementTag >                                 LevelElementType;
        typedef typename viennagrid::topology::bndcells<CellTag, 0>::tag       VertexTag; 
        typedef viennagrid::element_t<Config, VertexTag >                                  VertexType;
        typedef viennagrid::element_t<Config, typename Config::cell_tag>                   CellType;
        typedef typename viennagrid::result_of::element_container< segment_t<Config>,
                                                      dim>::type                   container_type;
        typedef typename next_segment_layer_selector<Config, dim-1>::type                  base_type;
        
      public:
        
        void fill(CellType & cell)
        {
          //add vertices to segment: (note that the ncells<0>() is not available here)
          typedef typename result_of::element_container< CellType, dim >::type container_type;
          container_type * level_elements = cell.container(dimension_tag<dim>());
          
          for (long i=0; i<topology::bndcells<typename Config::cell_tag, dim>::num; ++i)
            elements.insert( (*level_elements)[i] );
          
          base_type::fill(cell);
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
        container_type elements;
    };

    
    //vertex level:
    /** @brief Specialization of full handling at vertex level. Terminates recursion. */
    template <typename Config>
    class segment_layers_full<Config, 0> : public segment_domain_holder<Config>
    {
        typedef typename Config::cell_tag                                 CellTag;
        typedef typename viennagrid::topology::bndcells<CellTag, 0>::tag    VertexTag;
        typedef viennagrid::element_t<Config, VertexTag >                               VertexType;
        typedef viennagrid::element_t<Config, typename Config::cell_tag>                CellType;
        typedef typename result_of::element_container< segment_t<Config>, 
                                                      0>::type           ContainerType;
        
      public:
        
        void fill(CellType & cell)
        {
          //add vertices to segment: (note that the ncells<0>() is not available here)
          typedef typename result_of::element_container< CellType, 0 >::type container_type;
          container_type * cell_vertices = cell.container(dimension_tag<0>());

          for (long i=0; i<viennagrid::topology::bndcells<typename Config::cell_tag, 0>::num; ++i)
            elements.insert( (*cell_vertices)[i] );
            
        }
        
        ContainerType * container(dimension_tag<0>) { return &elements; }
        const ContainerType * container(dimension_tag<0>) const { return &elements; }

      private:
        ContainerType    elements;        //container of elements
    };

    
    /** @brief No handling of elements by cells on that level. Therefore, don't handle it within segments as well. Uses recursive inheritance */
    template <typename Config,
              long dim>
    class segment_layers_empty : public next_segment_layer_selector<Config, dim-1>::type
    {
        typedef typename Config::cell_tag                                      CellTag;
        typedef typename viennagrid::topology::bndcells<CellTag, dim>::tag     ElementTag;
        typedef viennagrid::element_t<Config, ElementTag >                                 LevelElementType;
        typedef typename viennagrid::topology::bndcells<CellTag, 0>::tag       VertexTag; 
        typedef viennagrid::element_t<Config, VertexTag >                                  VertexType;
        typedef viennagrid::element_t<Config, typename Config::cell_tag>                   CellType;
        typedef typename viennagrid::result_of::element_container< segment_t<Config>,
                                                      dim>::type                   container_type;
        typedef typename next_segment_layer_selector<Config, dim-1>::type          base_type;
        
      public:
        
        void fill(CellType & cell)
        {
          //nothing to do here. Just forward to lower level:
          base_type::fill(cell);
        }
        
        ///////////////////// container retrieval /////////////////////////

        using base_type::container;
        
        //non-const:
        container_type * 
        container(dimension_tag<dim>)
        { 
          typedef typename result_of::bndcell_handling<Config,
                                                          CellTag,
                                                          dim
                                                         >::ERROR_HANDLING_OF_ELEMENTS_AT_THIS_TOPOLOGICAL_LEVEL_NOT_PROVIDED   error_type;
          return NULL; 
        }

        //const:
        const container_type * 
        container(dimension_tag<dim>) const
        { 
          typedef typename result_of::bndcell_handling<Config, 
                                                          CellTag,
                                                          dim
                                                         >::ERROR_HANDLING_OF_ELEMENTS_AT_THIS_TOPOLOGICAL_LEVEL_NOT_PROVIDED  error_type;
          return NULL; 
        }

    };

    /** @brief Handler for cells in a segment. Since cells are the defining entity of segments, this requires special treatment */
    template <typename Config,
              long dim>
    class segment_layers_top : public next_segment_layer_selector<Config, dim-1>::type
    {
        typedef typename Config::cell_tag                                                        CellTag;
        typedef typename viennagrid::topology::bndcells<typename Config::cell_tag, dim>::tag     ElementTag;
        typedef viennagrid::element_t<Config, ElementTag >                                                  ElementType;
        typedef typename viennagrid::topology::bndcells<typename Config::cell_tag, 0>::tag       VertexTag;
        typedef viennagrid::element_t<Config, VertexTag >                                               VertexType;
        typedef viennagrid::element_t<Config, typename Config::cell_tag>                                 CellType;
        typedef typename viennagrid::result_of::element_container< segment_t<Config>, dim>::type      container_type;
        typedef typename next_segment_layer_selector<Config, dim-1>::type                             base_type;
        
      public:
        
        void push_back(CellType & cell)
        {
          //add cell to domain and keep pointer in segment
          //std::cout << "Adding cell to domain and to segment" << std::endl;
          elements.push_back(base_type::domain_->push_back(cell));
          
          base_type::fill(*elements.back());
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
        container_type elements;
    };
  
  } //namespace detail


  // a few metafunctions returning the correct container for segments:
  namespace result_of
  {
    //at cell level
    /** @brief Metafunction returning the internal storage scheme of a segment at cell level */
    template <typename config_type, long cell_level>
    struct element_container< segment_t<config_type>, cell_level, cell_level>
    {
      typedef typename viennagrid::result_of::ncell<config_type, cell_level>::type     element_type;
      
      typedef std::deque< element_type * >      type;
    };

    //at vertex level
    /** @brief Metafunction returning the internal storage scheme of a segment at vertex level */
    template <typename config_type, long cell_level>
    struct element_container< segment_t<config_type>, 0, cell_level>
    {
      typedef typename viennagrid::result_of::ncell<config_type, 0>::type     element_type;
      
      // Note that with the use of deque, pointer comparison does not induce a valid ordering!
      // This is a problem when trying to write mesh files (vertices are not printed in order)
      typedef std::set< element_type *, viennagrid::detail::id_ptr_compare >      type;
    };

    //at any other level:
    /** @brief Metafunction returning the internal storage scheme of a segment at non-vertex and non-cell level */
    template <typename config_type, long dim, long cell_level>
    struct element_container< segment_t<config_type>, dim, cell_level>
    {
      typedef typename viennagrid::result_of::ncell<config_type, dim>::type     element_type;
      
      typedef std::set< element_type * >      type;
    };
    
    /** @brief Metafunction for returning the type of a segment derived from the configuration class */
    template <typename ConfigType>
    struct segment
    {
      typedef segment_t<ConfigType>     type; 
    };
    
    
  } //namespace result_of

} //namespace viennagrid
#endif
