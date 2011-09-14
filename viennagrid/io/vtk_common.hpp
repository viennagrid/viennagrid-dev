#ifndef VIENNAGRID_IO_VTK_COMMON_HPP
#define VIENNAGRID_IO_VTK_COMMON_HPP

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


#include <fstream>
#include <iostream>
#include "viennagrid/io/helper.hpp"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/forwards.h"

/** @file vtk_common.hpp
    @brief Provides common things for VTK reader and writer
*/

namespace viennagrid
{
  namespace io
  {
    /** @brief Translates element tags to VTK type identifiers
     * 
     * see http://www.vtk.org/VTK/img/file-formats.pdf, Figure 2, for an overview
     * 
     * @tparam ElementTag    The tag identifying the n-cell type
     */
    template <typename ElementTag>
    struct ELEMENT_TAG_TO_VTK_TYPE
    {
      enum{ ReturnValue = ElementTag::ERROR_ELEMENT_TYPE_NOT_SUPPORTED };
    };

    /** @brief Specialization for a hexahedron */
    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<hexahedron_tag>
    {
      enum{ value = 12 };
    };

    /** @brief Specialization for a tetrahedron */
    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<tetrahedron_tag>
    {
      enum{ value = 10 };
    };
    
    /** @brief Specialization for a quadrilateral */
    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<quadrilateral_tag>
    {
      enum{ value = 9 };
    };

    /** @brief Specialization for a triangle */
    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<triangle_tag>
    {
      enum{ value = 5 };
    };

    /** @brief Specialization for a line (one-dimensional hypercube) */
    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<hypercube_tag<1> >
    {
      enum{ value = 3 };
    };
    
    /** @brief Specialization for a line (one-dimensional simplex) */
    template <>
    struct ELEMENT_TAG_TO_VTK_TYPE<simplex_tag<1> >
    {
      enum{ value = 3 };
    };
    
    /** @brief Tranformations of reference orientations from VTK to ViennaGrid
     * 
     * While nothing needs to be done for simplices, hypercubes are oriented differently.
     */
    template <typename CellTag>
    struct vtk_to_viennagrid_orientations
    {
      long operator()(long j) const { return j; }  //default case: do nothing
    };

    /** @brief Tranformations of reference orientations from ViennaGrid to VTK
     * 
     * While nothing needs to be done for simplices, hypercubes are oriented differently.
     */
    template <typename CellTag>
    struct viennagrid_to_vtk_orientations
    {
      long operator()(long j) const { return j; }  //default case: do nothing
    };

    /** @brief Specialization for quadrilaterals: Switch vertices 2 and 3 */
    template <>
    struct vtk_to_viennagrid_orientations<quadrilateral_tag>
    {
      long operator()(long j) const
      {
        switch (j)
        {
          case 2: return 3;
          case 3: return 2;
          default: return j;
        }
        return j;
      }
    };
    
    /** @brief Specialization for quadrilaterals: Switch vertices 2 and 3 */
    template <>
    struct viennagrid_to_vtk_orientations<quadrilateral_tag>
    {
      long operator()(long j) const
      {
        return vtk_to_viennagrid_orientations<quadrilateral_tag>()(j);
      }
    };

    // for hexahedra:
    /** @brief Specialization for hexahedra: Switch vertices (2, 3) and (6, 7) */
    template <>
    struct vtk_to_viennagrid_orientations<hexahedron_tag>
    {
      long operator()(long j) const
      {
        switch (j)
        {
          case 2: return 3;
          case 3: return 2;
          case 6: return 7;
          case 7: return 6;
          default: return j;
        }
        return j;
      }
    };

    /** @brief Specialization for hexahedra: Switch vertices (2, 3) and (6, 7) */
    template <>
    struct viennagrid_to_vtk_orientations<hexahedron_tag>
    {
      long operator()(long j) const
      {
        return vtk_to_viennagrid_orientations<hexahedron_tag>()(j);
      }
    };





    /** @brief A utility class managing the vertex ID in the domain. This is trivial if the vertices provide an ID, but a little tricky if vertex objects don't provide integral IDs.
     * 
     * @tparam DomainSegmentType   Type of either a domain or a segment
     * @tparam IDHandler           The ID handling class used for dispatching.
     */
    template <typename DomainSegmentType,
              typename IDHandler = typename viennagrid::result_of::element_id_handler<typename DomainSegmentType::config_type,
                                                                                      viennagrid::point_tag>::type
             >
    class vtk_vertex_id_repository //default case: This is a segment
    {
        typedef typename DomainSegmentType::config_type                      ConfigType;
        typedef typename viennagrid::result_of::segment<ConfigType>::type    SegmentType;
        typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type   VertexType;
        
      public:
        vtk_vertex_id_repository(SegmentType const & seg) {}
      
        /** @brief Returns the ID of the the vertex with respect to the segment */
        long operator()(VertexType const & v, SegmentType const & seg) const
        {
          return viennadata::access<segment_mapping_key<SegmentType>, long>(seg)(v);
        }
    };
    
    /** @brief Specialization for a domain. The simple case: Vertices know their global IDs. */
    template <typename ConfigType>
    class vtk_vertex_id_repository< viennagrid::domain_t<ConfigType>, viennagrid::integral_id >
    {
        typedef typename viennagrid::result_of::domain<ConfigType>::type     DomainType;
        typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type   VertexType;
        
      public:
        vtk_vertex_id_repository(DomainType const & seg) {}
      
        /** @brief Returns the global ID of the vertex */
        long operator()(VertexType const & v, DomainType const & seg) const
        {
          return v.id();
        }
    };

    /** @brief Specialization for a domain. The tough case: Vertices don't know their global IDs. Set up a map first. */
    template <typename ConfigType>
    class vtk_vertex_id_repository< viennagrid::domain_t<ConfigType>, viennagrid::pointer_id >
    {
        typedef typename viennagrid::domain_t<ConfigType>                    DomainType;
        typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type   VertexType;
        
      public:
        vtk_vertex_id_repository(DomainType const & domain)
        {
          typedef typename viennagrid::result_of::const_ncell_range<DomainType, 0>::type    VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
          
          VertexRange vertices = viennagrid::ncells(domain);
          long vertex_index = 0;
          for (VertexIterator vit = vertices.begin();
                              vit != vertices.end();
                            ++vit, ++vertex_index)
          {
            ptr_to_global_id[&(*vit)] = vertex_index;
          }
        }
      
        /** @brief Returns the global ID of the vertex */
        long operator()(VertexType const & v, DomainType const &)
        {
          return ptr_to_global_id[&v];
        }
        
      private:
        std::map<VertexType const *, long>  ptr_to_global_id;
    };
    

    
  } //namespace io
} //namespace viennagrid

#endif

