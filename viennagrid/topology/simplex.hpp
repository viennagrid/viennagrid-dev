#ifndef VIENNAGRID_TOPOLOGY_SIMPLEX_HPP
#define VIENNAGRID_TOPOLOGY_SIMPLEX_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <sstream>

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"

/** @file topology/simplex.hpp
    @brief Provides the topological definition of an arbitrary simplex element
*/

namespace viennagrid
{
  namespace meta
  {
    template <int n, int k>
    struct n_over_k
    {
      static const int value = n_over_k<n-1, k-1>::value + n_over_k<n-1, k>::value;
    };

    template <int n>
    struct n_over_k<n, 0>
    {
      static const int value = 1;
    };

    template <int k>
    struct n_over_k<0, k>
    {
      static const int value = 0;
    };

    template <>
    struct n_over_k<0, 0>
    {
      static const int value = 1;
    };

  }


  /** @brief Topological description of an n-simplex.*/
  template <int n>
  struct simplex_tag
  {
    typedef simplex_tag<n-1> facet_tag;

    static const int dim = n;
    static std::string name()
    {
      std::stringstream ss;
      ss << n << "-simplex";
      return ss.str();
    }
  };

  /** @brief Topological description of the boundary k-cells an n-simplex */
  template <int n, int k>
  struct boundary_elements<simplex_tag<n>, simplex_tag<k> >
  {
    //typedef simplex_tag<k>             tag;

    typedef static_layout_tag     layout_tag;
    static const int num = meta::n_over_k<n+1, k+1>::value;
  };


  namespace element_topology
  {
    ///////////////////////////////// Generator for boundary cell elements ///////////////////////////////////


    template<int n, typename BoundaryElementType>
    struct boundary_element_generator<simplex_tag<n>, simplex_tag<1>, BoundaryElementType>
    {
        template<typename element_type, typename inserter_type>
        static void create_boundary_elements(element_type & element, inserter_type & inserter)
        {
            BoundaryElementType boundary_element( inserter.get_physical_container_collection() );

            int index = 0;
            for (int i = 0; i < boundary_elements<simplex_tag<n>, vertex_tag >::num; ++i)
                for (int j = i+1; j < boundary_elements<simplex_tag<n>, vertex_tag >::num; ++j)
                {
                    boundary_element.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(i), 0 );
                    boundary_element.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(j), 1 );

                    element.set_boundary_element( boundary_element, inserter.template insert<true, true>(boundary_element), index++ );
                }
        }
    };


    template<int n, typename BoundaryElementType>
    struct boundary_element_generator<simplex_tag<n>, simplex_tag<2>, BoundaryElementType>
    {
        template<typename element_type, typename inserter_type>
        static void create_boundary_elements(element_type & element, inserter_type & inserter)
        {
            BoundaryElementType boundary_element( inserter.get_physical_container_collection() );

            int index = 0;
            for (int i = 0; i < boundary_elements<simplex_tag<n>, vertex_tag >::num; ++i)
                for (int j = i+1; j < boundary_elements<simplex_tag<n>, vertex_tag >::num; ++j)
                    for (int k = j+1; k < boundary_elements<simplex_tag<n>, vertex_tag >::num; ++k)
                    {
                        boundary_element.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(i), 0 );
                        boundary_element.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(j), 1 );
                        boundary_element.container(dimension_tag<0>()).set_handle( element.container( dimension_tag<0>() ).handle_at(k), 2 );

                        element.set_boundary_element( boundary_element, inserter.template insert<true, true>(boundary_element), index++ );
                    }
        }
    };


    // similarly for higher topological dimensions...

  } //topology


}

#endif

