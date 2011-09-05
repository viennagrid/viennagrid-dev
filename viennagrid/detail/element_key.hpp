#ifndef VIENNAGRID_ELEMENT_KEY_HPP
#define VIENNAGRID_ELEMENT_KEY_HPP

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


#include <map>
#include <iostream>
#include <algorithm>

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/topology/point.hpp"

namespace viennagrid
{
  
  /////////////// Segment is a container for elements ////////////////

  //helper: holds an ordered number of vertices
  //comparisons also take permutations into account
  template <typename ElementType,
            typename IDHandler = typename traits::element_id<point_tag>::id_handler>
  struct ElementKeyStorageType
  {
    typedef ElementType *           result_type;
  };

  template <typename ElementType>
  struct ElementKeyStorageType<ElementType, integral_id>
  {
    typedef long          result_type;
  };

  
  
  /** @brief A key type that uniquely identifies an element by its vertices */
  template <typename ElementType>
  class element_key
  {
      typedef typename ElementType::element_tag            ElementTag;
      typedef typename ElementKeyStorageType<ElementType>::result_type  StorageType;
    public:
      element_key( ElementType & el2) : vertexIDs(topology::subelements<ElementTag, 0>::num_elements)
      {
        typedef typename result_of::ncell_range<ElementType, 0>::type       VertexRange;
        typedef typename result_of::iterator<ElementType, 0>::type          VertexIterator;
        long i = 0;
        VertexRange vertices_el2 = ncells<0>(el2);
        for (VertexIterator vit = vertices_el2.begin();
             vit != vertices_el2.end();
             ++vit, ++i)
          vertexIDs[i] = vit->id();
        //sort it:
        std::sort(vertexIDs.begin(), vertexIDs.end());
      }

      element_key( const element_key & ek2) : vertexIDs(ek2.vertexIDs.size())
      {
        //std::cout << "Copy constructor ElementKey " << this << std::endl;
        for (typename std::vector<StorageType>::size_type i=0; i<ek2.vertexIDs.size(); ++i)
          vertexIDs[i] = ek2.vertexIDs[i];
      }

      bool operator < (element_key const & epc2) const
      {
        for (long i=0; i<topology::subelements<ElementTag, 0>::num_elements; ++i)
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
        for (typename std::vector<StorageType>::const_iterator vit = vertexIDs.begin();
              vit != vertexIDs.end();
              ++vit)
          std::cout << *vit << " ";
        std::cout << std::endl;
      }

    private:
      std::vector< StorageType > vertexIDs;
  };


  
  
  
}
#endif
