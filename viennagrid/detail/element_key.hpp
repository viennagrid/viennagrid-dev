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
            typename IDHandler = typename result_of::element_id_handler<ConfigType, point_tag>::type>
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
}



namespace viennagrid
{
    namespace storage
    {
        template<typename storage_type>
        struct element_key_tag {};
        
        namespace result_of
        {
            template<typename element_type, typename storage_type>
            struct hidden_key_map_key_type_from_tag<element_type, element_key_tag<storage_type> >
            {
                typedef element_key<element_type, storage_type> type;
            };

        }
    }
    
}





#endif
