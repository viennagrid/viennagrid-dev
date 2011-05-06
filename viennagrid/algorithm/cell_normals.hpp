/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Josef Weinbub                   weinbub@iue.tuwien.ac.at
               Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_ALGORITHM_CELLNORMALS_GUARD
#define VIENNAGRID_ALGORITHM_CELLNORMALS_GUARD

#include "viennagrid/forwards.h"
#include "viennagrid/point.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennadata/api.hpp"

// -----------------------------------------------------------------------------
// 
// prepare viennadata specific stuff
//
namespace viennagrid {
   namespace tag {
      struct seg_cell_normal    {};
   }

   namespace data {
      struct seg_cell_normal {
         typedef viennagrid::point<double, viennagrid::three_dimensions_tag>         point_type;
         typedef std::map<std::size_t, point_type>    type;
      };
   }
} // end namespace viennagrid

namespace viennadata {
  namespace config {
    template <>
    struct key_dispatch<viennagrid::tag::seg_cell_normal>
    {
      typedef type_key_dispatch_tag    tag;
    };    
  }
}


namespace viennagrid {
// -----------------------------------------------------------------------------
// 
// a domain specific cell normal vector algorithm
//
template<int DIMG, typename CellTagT> 
struct assignCellNormals_impl
{
   template <typename DomainT>
   static void eval(DomainT& domain)
   {
      std::cerr << "ViennaGrid - Error: Cell Normal Algorithm not implemented for this domain type" << std::endl;
   }
};

// for triangular hull meshes
//
template<> 
struct assignCellNormals_impl <3, viennagrid::triangle_tag>
{
   template <typename DomainT>
   static void eval(DomainT& domain)
   {
      typedef typename DomainT::config_type                    DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag           CellTag;   
      typedef typename DomainConfiguration::numeric_type       NumericType;

      typedef typename DomainT::segment_type                                                                      SegmentType; 
      typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;
      typedef typename viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type         CellContainer;          
      typedef typename viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;         
      typedef typename viennagrid::result_of::ncell_container<CellType, 0>::type                                  VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;            
      
      typedef typename viennagrid::result_of::point_type<DomainConfiguration>::type                               PointType;
      
      static const int CELLSIZE = viennagrid::traits::subcell_desc<CellTag, 0>::num_elements;

      std::vector<PointType>     cell_points(CELLSIZE);
      
      // traverse the segments
      //
      for (std::size_t si = 0; si < domain.segment_size(); ++si)
      {
         SegmentType & seg = domain.segment(si);

         // traverse the cells of this segment
         //
         CellContainer cells = viennagrid::ncells<CellTag::topology_level>(seg);
         
         for (CellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
            // get the cell points
            //
            std::size_t vi = 0;       
            VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(*cit);
            for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                vocit != vertices_for_cell.end();
                ++vocit)
            {
               cell_points[vi++] = vocit->getPoint();
            }
            
            // setup two vectors which represent the cell plane
            //  based on one common vertex
            //
            PointType vec1 = cell_points[1]-cell_points[0];
            PointType vec2 = cell_points[2]-cell_points[0];
            
            // compute the normal vector on this plane
            //
            PointType normal_vector = viennagrid::cross_prod(vec1,vec2);            
            
            // compute the magnitude of the normal vector
            //
            NumericType magnitude = viennagrid::norm(normal_vector);

            // normalize the normal-vector
            //
            normal_vector /= magnitude;
            
            // store it via viennadata on the cell in respect to the 
            // current cell
            // 
            viennadata::access<
               viennagrid::tag::seg_cell_normal,         // key-type
               viennagrid::data::seg_cell_normal::type   // data-type
            >()(*cit)[si]= normal_vector;
         }
      }
   }
};

// for quadrilateral hull meshes
//
template<> 
struct assignCellNormals_impl <3, viennagrid::quadrilateral_tag>
{
   template <typename DomainT>
   static void eval(DomainT& domain)
   {
      // the 3d_triangular specialization holds for 3d_quadrilateral
      // configuration too ..
      //
      assignCellNormals_impl<3, viennagrid::triangle_tag>::eval(domain);
   }
};

// -----------------------------------------------------------------------------
// 
// the generic access function
// note: extracts meta-data from the domain and forwards it to the 
//       tag-dispatched algorithm specialization hierarchy
//
template <typename DomainT>
void assign_cell_normals(DomainT & domain)
{
   typedef typename DomainT::config_type                          DomainConfiguration;

   typedef typename DomainConfiguration::dimension_tag            DimensionTag;
   typedef typename DomainConfiguration::cell_tag                 CellTag;
   
   assignCellNormals_impl<DimensionTag::value, CellTag>::eval(domain);
}

} // end namespace viennagrid


#endif 

