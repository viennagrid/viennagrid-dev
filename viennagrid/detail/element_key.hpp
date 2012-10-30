#ifndef VIENNAGRID_ELEMENT_KEY_HPP
#define VIENNAGRID_ELEMENT_KEY_HPP

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
#include <iostream>
#include <algorithm>

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/topology/point.hpp"

#include "viennagrid/storage/container.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"

/** @file element_key.hpp
    @brief Provides a key that uniquely identifies n-cells
*/

namespace viennagrid
{
  
  /////////////// Segment is a container for elements ////////////////

  /** @brief Internal helper class: holds an ordered number of vertices. Comparisons also take permutations into account.  */
  template <typename ConfigType, 
            typename ElementType,
            typename IDHandler = typename result_of::element_id_handler<ConfigType, vertex_tag>::type>
  struct ElementKeyStorageType
  {
    typedef ElementType *           result_type;
  };

  template <typename ConfigType, typename ElementType>
  struct ElementKeyStorageType<ConfigType, ElementType, integral_id>
  {
    typedef long          result_type;
  };

  
  
  
  
  /** @brief A key type that uniquely identifies an element by its vertices */
  //template <typename ConfigType, typename ElementType>
  template <typename element_type, typename id_type>
  class element_key
  {
      typedef typename element_type::tag            ElementTag;
      //typedef typename ElementKeyStorageType<ConfigType, ElementType>::result_type  StorageType;
    public:
      element_key( const element_type & el2) : vertexIDs(topology::bndcells<ElementTag, 0>::num)
      {
        typedef typename result_of::const_ncell_range<element_type, 0>::type       VertexConstRange;
        typedef typename result_of::iterator<VertexConstRange>::type          VertexConstIterator;
        long i = 0;
        VertexConstRange vertices_el2 = ncells<0>(el2);
        for (VertexConstIterator vit = vertices_el2.begin();
             vit != vertices_el2.end();
             ++vit, ++i)
          vertexIDs[i] = static_cast<id_type>(vit->id());
        //sort it:
        std::sort(vertexIDs.begin(), vertexIDs.end());
      }

      element_key( const element_key & ek2) : vertexIDs(ek2.vertexIDs.size())
      {
        //std::cout << "Copy constructor ElementKey " << this << std::endl;
        for (typename std::vector<id_type>::size_type i=0; i<ek2.vertexIDs.size(); ++i)
          vertexIDs[i] = ek2.vertexIDs[i];
      }

      bool operator < (element_key const & epc2) const
      {
        for (long i=0; i<topology::bndcells<ElementTag, 0>::num; ++i)
        {
          if ( vertexIDs[i] > epc2.vertexIDs[i] )
            return false;
          else if ( vertexIDs[i] < epc2.vertexIDs[i] )
            return true;
        }
        return false;
      }

      void print() const
      { 
        for (typename std::vector<id_type>::const_iterator vit = vertexIDs.begin();
              vit != vertexIDs.end();
              ++vit)
          std::cout << *vit << " ";
        std::cout << std::endl;
      }

    private:
        // TODO: statt std::vector abhängig vom element_type
      std::vector< id_type > vertexIDs;
  };
  
  
  // TODO abstract version for references
  /** @brief A key type that uniquely identifies an element by its vertices */
  //template <typename ConfigType, typename ElementType>
  template <typename element_type, typename id_type>
  class element_key<element_type *, id_type>
  {
      typedef typename element_type::tag            ElementTag;
      //typedef typename ElementKeyStorageType<ConfigType, ElementType>::result_type  StorageType;
    public:
      element_key( const element_type * el2) : vertexIDs(topology::bndcells<ElementTag, 0>::num)
      {
        typedef typename result_of::const_ncell_range<element_type, 0>::type       VertexConstRange;
        typedef typename result_of::iterator<VertexConstRange>::type          VertexConstIterator;
        long i = 0;
        VertexConstRange vertices_el2 = ncells<0>(*el2);
        for (VertexConstIterator vit = vertices_el2.begin();
             vit != vertices_el2.end();
             ++vit, ++i)
          vertexIDs[i] = static_cast<id_type>(vit->id());
        //sort it:
        std::sort(vertexIDs.begin(), vertexIDs.end());
      }

      element_key( const element_key & ek2) : vertexIDs(ek2.vertexIDs.size())
      {
        //std::cout << "Copy constructor ElementKey " << this << std::endl;
        for (typename std::vector<id_type>::size_type i=0; i<ek2.vertexIDs.size(); ++i)
          vertexIDs[i] = ek2.vertexIDs[i];
      }

      bool operator < (element_key const & epc2) const
      {
        for (long i=0; i<topology::bndcells<ElementTag, 0>::num; ++i)
        {
          if ( vertexIDs[i] > epc2.vertexIDs[i] )
            return false;
          else if ( vertexIDs[i] < epc2.vertexIDs[i] )
            return true;
        }
        return false;
      }

      void print() const
      { 
        for (typename std::vector<id_type>::const_iterator vit = vertexIDs.begin();
              vit != vertexIDs.end();
              ++vit)
          std::cout << *vit << " ";
        std::cout << std::endl;
      }

    private:
        // TODO: statt std::vector abhängig vom element_type
      std::vector< id_type > vertexIDs;
  };
  
  
  
  
  // TODO abstract version for references
  /** @brief A key type that uniquely identifies an element by its vertices */
  //template <typename ConfigType, typename ElementType>
  template <typename element_type, typename hook_type>
  class hooked_element_key
  {
      typedef typename element_type::tag            ElementTag;
      //typedef typename ElementKeyStorageType<ConfigType, ElementType>::result_type  StorageType;
    public:
      hooked_element_key( const element_type & el2) : vertex_hooks(topology::bndcells<ElementTag, 0>::num)
      {
            typedef typename element_type::vertex_container_type vertex_container_type;
            typedef typename vertex_container_type::const_hook_iterator const_hook_iterator;
          
          
        //typedef typename result_of::const_ncell_range<element_type, 0>::type       VertexConstRange;
        //typedef typename result_of::iterator<VertexConstRange>::type          VertexConstIterator;
        long i = 0;
        vertex_container_type vertices_el2 = ncells<0>(el2);
        for (const_hook_iterator vit = vertices_el2.hook_begin();
             vit != vertices_el2.hook_end();
             ++vit, ++i)
          vertex_hooks[i] = static_cast<hook_type>( *vit );
        //sort it:
        std::sort(vertex_hooks.begin(), vertex_hooks.end());
      }

      hooked_element_key( const hooked_element_key & ek2) : vertex_hooks(ek2.vertex_hooks.size())
      {
        //std::cout << "Copy constructor ElementKey " << this << std::endl;
        for (typename std::vector<hook_type>::size_type i=0; i<ek2.vertex_hooks.size(); ++i)
          vertex_hooks[i] = ek2.vertex_hooks[i];
      }

      bool operator < (hooked_element_key const & epc2) const
      {
        for (long i=0; i<topology::bndcells<ElementTag, 0>::num; ++i)
        {
          if ( vertex_hooks[i] > epc2.vertex_hooks[i] )
            return false;
          else if ( vertex_hooks[i] < epc2.vertex_hooks[i] )
            return true;
        }
        return false;
      }

      void print() const
      { 
        for (typename std::vector<hook_type>::const_iterator vit = vertex_hooks.begin();
              vit != vertex_hooks.end();
              ++vit)
          std::cout << *vit << " ";
        std::cout << std::endl;
      }

    private:
        // TODO: statt std::vector abhängig vom element_type
      std::vector< hook_type > vertex_hooks;
  };
}



namespace viennagrid
{
    namespace storage
    {
        template<typename storage_type>
        struct element_key_tag {};
        
        template<typename hook_type>
        struct hooked_element_key_tag {};
        
        namespace result_of
        {
            template<typename element_type, typename storage_type>
            struct hidden_key_map_key_type_from_tag<element_type, element_key_tag<storage_type> >
            {
                typedef element_key<element_type, storage_type> type;
            };
            
            template<typename element_type, typename hook_type>
            struct hidden_key_map_key_type_from_tag<element_type, hooked_element_key_tag<hook_type> >
            {
                typedef hooked_element_key<element_type, hook_type> type;
            };

        }
    }
    
}





#endif
