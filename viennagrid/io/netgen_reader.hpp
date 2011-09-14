#ifndef VIENNAGRID_IO_NETGEN_READER_GUARD
#define VIENNAGRID_IO_NETGEN_READER_GUARD

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
#include <assert.h>
#include "viennagrid/forwards.h"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/iterators.hpp"

/** @file netgen_reader.hpp
    @brief Provides a reader for Netgen files
*/

namespace viennagrid
{
  namespace io
  {

    /** @brief Reader for Netgen files obtained from the 'Export mesh...' menu item. Tested with Netgen version 4.9.12. */
    struct netgen_reader
    {
      /** @brief The functor interface triggering the read operation.
       * 
       * @param domain    A ViennaGrid domain
       * @param filename  Name of the file
       */
      template <typename DomainType>
      int operator()(DomainType & domain, std::string const & filename) const
      {
        typedef typename DomainType::config_type                         ConfigType;

        typedef typename ConfigType::numeric_type                        CoordType;
        typedef typename ConfigType::coordinate_system_tag               CoordinateSystemTag;
        typedef typename ConfigType::cell_tag                            CellTag;

        typedef typename result_of::point<ConfigType>::type                              PointType;
        typedef typename result_of::ncell<ConfigType, 0>::type                           VertexType;
        typedef typename result_of::ncell<ConfigType, CellTag::dim>::type     CellType;

        typedef typename viennagrid::result_of::ncell_range<DomainType, 0>::type   VertexRange;
        typedef typename viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
            
        typedef typename viennagrid::result_of::ncell_range<DomainType, 1>::type   EdgeRange;
        typedef typename viennagrid::result_of::iterator<EdgeRange>::type          EdgeIterator;

        typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::dim-1>::type   FacetRange;
        typedef typename viennagrid::result_of::iterator<FacetRange>::type                                 FacetIterator;

        typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::dim>::type     CellRange;
        typedef typename viennagrid::result_of::iterator<CellRange>::type                                  CellIterator;
        
        std::ifstream reader(filename.c_str());
        
        typedef typename DomainType::segment_container      SegmentContainer;

        //Segment & segment = *(domain.begin());
        SegmentContainer & segments = domain.segments();

        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading file " << filename << std::endl;
        #endif

        if (!reader)
        {
          throw cannot_open_file_exception(filename);
          return EXIT_FAILURE;
        }

        std::string token;
        long node_num = 0;
        long cell_num = 0;
      
        if (!reader.good())
          throw bad_file_format_exception(filename, "File is empty.");
        
        //
        // Read vertices:
        //
        reader >> node_num;
        assert(node_num > 0);
        
        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading " << node_num << " vertices... " << std::endl;  
        #endif

        VertexType vertex;
    
        for (int i=0; i<node_num; i++)
        {
          if (!reader.good())
            throw bad_file_format_exception(filename, "EOF encountered while reading vertices.");
          
          for (int j=0; j<CoordinateSystemTag::dim; j++)
            reader >> vertex.point()[j];
          
          vertex.id(i);
          domain.push_back(vertex);
        }
    
        //std::cout << "DONE" << std::endl;
        if (!reader.good())
          throw bad_file_format_exception(filename, "EOF encountered when reading number of cells.");
          

        //
        // Read cells:
        //
        reader >> cell_num;
        //domain.reserve_cells(cell_num);
        
        #if defined VIENNAGRID_DEBUG_STATUS || defined VIENNAGRID_DEBUG_IO
        std::cout << "* netgen_reader::operator(): Reading " << cell_num << " cells... " << std::endl;  
        #endif
        
        CellType cell;
        //std::cout << "Filling " << cell_num << " cells:" << std::endl;
    
        for (int i=0; i<cell_num; ++i)
        {
          long vertex_num;
          VertexType *vertices[topology::bndcells<CellTag, 0>::num];

          if (!reader.good())
            throw bad_file_format_exception(filename, "EOF encountered while reading cells (segment index expected).");
          
          std::size_t segment_index;
          reader >> segment_index;
    
          for (int j=0; j<topology::bndcells<CellTag, 0>::num; ++j)
          {
            if (!reader.good())
              throw bad_file_format_exception(filename, "EOF encountered while reading cells (cell ID expected).");
            
            reader >> vertex_num;
            vertices[j] = &(viennagrid::ncells<0>(domain)[vertex_num - 1]);  //Note that Netgen uses vertex indices with base 1
          }
    
          //std::cout << std::endl << "Adding cell: " << &cell << " at " << i << std::endl;
          cell.vertices(&(vertices[0]));
          cell.id(i);
          if (segments.size() < segment_index) //not that segment_index is 1-based
            segments.resize(segment_index);
          
          segments[segment_index - 1].push_back(cell);  //note that Netgen uses a 1-based indexing scheme, while ViennaGrid uses a zero-based one
    
          //progress info:
          //if (i % 50000 == 0 && i > 0)
          //  std::cout << "* netgen_reader::operator(): " << i << " out of " << cell_num << " cells read." << std::endl;
        }
        
        //std::cout << "All done!" << std::endl;
        
        return EXIT_SUCCESS;
      } //operator()
      
    }; //class netgen_reader
  
  } //namespace io
} //namespace viennagrid

#endif
