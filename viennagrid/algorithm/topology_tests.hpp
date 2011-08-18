/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Josef Weinbub                   weinbub@iue.tuwien.ac.at   

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_ALGORITHM_TOPOLOGYTESTS_HPP
#define VIENNAGRID_ALGORITHM_TOPOLOGYTESTS_HPP

#include "viennagrid/forwards.h"

/** @file topology_tests.hpp
    @brief Tests a domain on topology errors.
*/


namespace viennagrid
{
// -----------------------------------------------------------------------------
template<int TopoLevel>
struct duplicates 
{
   typedef std::size_t result_type;

   template<typename DomainT>
   static result_type eval(DomainT& domain)
   {
      typedef typename DomainT::segment_type                                                    SegmentType;
      typedef typename DomainT::config_type                                                     DomainConfiguration;      
      typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, TopoLevel>::type  NCellType;            
      typedef typename viennagrid::result_of::ncell_range<SegmentType, TopoLevel>::type     NCellContainer;      
      typedef typename viennagrid::result_of::iterator<NCellContainer>::type                    NCellIterator;         
      typedef typename viennagrid::result_of::ncell_range<NCellType, 0>::type               VertexOnNCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnNCellContainer>::type            VertexOnNCellIterator;   

      static const int CELLSIZE = TopoLevel + 1; // holds only for simplex topology ..

      std::size_t duplicates_cnt = 0;
      
      for (std::size_t si = 0; si < domain.segment_size(); ++si)
      {
         SegmentType & seg = domain.segment(si);
         
         std::size_t cell_cnt; 

         typedef std::vector<std::size_t>        temp_cell_type;
         temp_cell_type cell(CELLSIZE); 
         std::map<temp_cell_type, bool>          uniquer;
         NCellContainer cells = viennagrid::ncells<TopoLevel>(seg);      
         for (NCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
            // extract the cell
            std::size_t i = 0;
            VertexOnNCellContainer vertices = viennagrid::ncells<0>(*cit);
            for (VertexOnNCellIterator vocit = vertices.begin(); vocit != vertices.end(); ++vocit)
            {        
               cell[i++] = vocit->id();
            }

            // sort the edge indices in ascending order
            // this way we can check for uniqueness
            std::sort(cell.begin(), cell.end());
            
            // check for uniqueness
            if( !uniquer[cell] )
            {
               uniquer[cell] = true;
            }
            else // the edge is already present ..
            {
               //std::cout << cell << std::endl;
               duplicates_cnt++;
            }
         }      
         
      }
      return duplicates_cnt;   
   }
};

// specialication for 0-cells (vertices)
template<>
struct duplicates <0>
{
   typedef std::size_t result_type;

   template<typename DomainT>
   static result_type eval(DomainT& domain)
   {
      typedef typename DomainT::segment_type                                                    SegmentType;
      typedef typename DomainT::config_type                                                     DomainConfiguration;      
      typedef typename viennagrid::result_of::ncell_range<SegmentType, 0>::type             NCellContainer;      
      typedef typename viennagrid::result_of::iterator<NCellContainer>::type                    NCellIterator;         

      std::size_t duplicates_cnt = 0;
      
      for (std::size_t si = 0; si < domain.segment_size(); ++si)
      {
         SegmentType & seg = domain.segment(si);
         
         std::size_t cell_cnt; 

         std::map<std::size_t, bool>          uniquer;

         NCellContainer cells = viennagrid::ncells<0>(seg);      
         for (NCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
         {
            // check for uniqueness
            if( !uniquer[cit->id()] )
            {
               uniquer[cit->id()] = true;
            }
            else // the edge is already present ..
            {
               duplicates_cnt++;
            }
         }      
         
      }
      return duplicates_cnt;   
   }
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// general case for a two/three-dimensional mesh
//   note: in this case, each edge/face has to be part of either one (at a boundary)
//         or two cells (in the interior)
//
template<int TopoDim, int GeomDim>
struct nonmanifolds_impl 
{ 
   typedef std::size_t           result_type;
   
   template<typename DomainT>
   static result_type eval(DomainT& domain)
   {
      typedef typename DomainT::segment_type                                                    SegmentType;
      typedef typename DomainT::config_type                                                     DomainConfiguration;         
      
      static const int DIMT    = DomainConfiguration::cell_tag::topology_level;
      static const int FACEDIM = DIMT-1;
      
      typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, FACEDIM>::type    FaceType;            
      typedef typename viennagrid::result_of::ncell_range<SegmentType, FACEDIM>::type       FaceContainer;      
      typedef typename viennagrid::result_of::iterator<FaceContainer>::type                     FaceIterator;            
      typedef typename viennagrid::result_of::ncell_range<FaceType, DIMT>::type             CellOnFaceContainer;
      typedef typename viennagrid::result_of::iterator<CellOnFaceContainer>::type               CellOnFaceIterator;            
      
      result_type cnt = 0;
      
      for (std::size_t si = 0; si < domain.segment_size(); ++si)
      {
         SegmentType & seg = domain.segment(si);
         
         std::size_t cell_cnt;
         
         FaceContainer faces = viennagrid::ncells<FACEDIM>(seg);      
         for (FaceIterator fit = faces.begin(); fit != faces.end(); ++fit)
         {
            cell_cnt = 0;
            CellOnFaceContainer cells = viennagrid::ncells<DIMT>(*fit, seg);
            for (CellOnFaceIterator cofit = cells.begin(); cofit != cells.end(); ++cofit)
            {         
               cell_cnt++;
            }
            if( (cell_cnt == 0) || (cell_cnt > 2)) cnt++;
         }      
      }
      
      return cnt;   
   }
};

// specialization for a hull mesh
//   note: in this case, each edge has to be part of exactly two cells
//
template<>
struct nonmanifolds_impl <2, 3>
{ 
   typedef std::size_t           result_type;
   
   template<typename DomainT>
   static result_type eval(DomainT& domain)
   {
      typedef typename DomainT::segment_type                                                    SegmentType;
      typedef typename DomainT::config_type                                                     DomainConfiguration;         
      
      static const int DIMT    = DomainConfiguration::cell_tag::topology_level;
      static const int FACEDIM = DIMT-1;
      
      typedef typename viennagrid::result_of::ncell_type<DomainConfiguration, FACEDIM>::type    FaceType;            
      typedef typename viennagrid::result_of::ncell_range<SegmentType, FACEDIM>::type       FaceContainer;      
      typedef typename viennagrid::result_of::iterator<FaceContainer>::type                     FaceIterator;            
      typedef typename viennagrid::result_of::ncell_range<FaceType, DIMT>::type             CellOnFaceContainer;
      typedef typename viennagrid::result_of::iterator<CellOnFaceContainer>::type               CellOnFaceIterator;            
      
      result_type cnt = 0;
      
      for (std::size_t si = 0; si < domain.segment_size(); ++si)
      {
         SegmentType & seg = domain.segment(si);
         
         std::size_t cell_cnt;
         
         FaceContainer faces = viennagrid::ncells<FACEDIM>(seg);      
         for (FaceIterator fit = faces.begin(); fit != faces.end(); ++fit)
         {
            cell_cnt = 0;
            CellOnFaceContainer cells = viennagrid::ncells<DIMT>(*fit, seg);
            for (CellOnFaceIterator cofit = cells.begin(); cofit != cells.end(); ++cofit)
            {         
               cell_cnt++;
            }
            if(cell_cnt != 2) cnt++;
         }      
      }
      
      return cnt;   
   }
};


struct nonmanifolds 
{
   typedef std::size_t           result_type;
   
   template<typename DomainT>
   static result_type eval(DomainT& domain)
   {
      typedef typename DomainT::config_type                                                     DomainConfiguration;         
      
      static const int DIMG    = DomainConfiguration::dimension_tag::value;      
      static const int DIMT    = DomainConfiguration::cell_tag::topology_level;

      if((DIMT < 2) || (DIMT > 3))
      {
         std::cerr << "Non-Manifold cell is not implemented for a domain of topology dimension: " << DIMT << std::endl;
         return -1;
      }

      viennagrid::nonmanifolds_impl<DIMT, DIMG>::eval(domain);
   }
};
// -----------------------------------------------------------------------------

} //namespace viennagrid
#endif



