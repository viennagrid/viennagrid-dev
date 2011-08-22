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

#ifndef VIENNAGRID_IO_NETGEN_READER_GUARD
#define VIENNAGRID_IO_NETGEN_READER_GUARD


#include <fstream>
#include <iostream>
#include <assert.h>
#include "viennagrid/forwards.h"
#include "viennagrid/io/helper.hpp"

namespace viennagrid
{
  namespace io
  {

    struct netgen_reader
    {
      template <typename DomainType>
      int operator()(DomainType & domain, std::string const & filename) const
      {
        typedef typename DomainType::config_type                     DomainConfiguration;

        typedef typename DomainConfiguration::numeric_type                 CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;

        typedef typename result_of::point<DomainConfiguration>::type                              PointType;
        typedef typename result_of::ncell<DomainConfiguration, 0>::type                           VertexType;
        typedef typename result_of::ncell<DomainConfiguration, CellTag::topology_level>::type     CellType;
        //typedef typename DomainTypes<DomainConfiguration>::segment_type  Segment;

        typedef typename viennagrid::result_of::ncell_range<DomainType, 0>::type   VertexRange;
        typedef typename viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;
            
        typedef typename viennagrid::result_of::ncell_range<DomainType, 1>::type   EdgeRange;
        typedef typename viennagrid::result_of::iterator<EdgeRange>::type          EdgeIterator;

        typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level-1>::type   FacetRange;
        typedef typename viennagrid::result_of::iterator<FacetRange>::type                                 FacetIterator;

        typedef typename viennagrid::result_of::ncell_range<DomainType, CellTag::topology_level>::type     CellRange;
        typedef typename viennagrid::result_of::iterator<CellRange>::type                                  CellIterator;

        std::ifstream reader(filename.c_str());

        //Segment & segment = *(domain.begin());

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
        
        //std::cout << "Reading " << node_num << " vertices... " << std::endl;  
        //reserve the memory:
        domain.reserve_vertices(node_num);
        VertexType vertex;
    
        for (int i=0; i<node_num; i++)
        {
          if (!reader.good())
            throw bad_file_format_exception(filename, "EOF encountered while reading vertices.");
          
          for (int j=0; j<DimensionTag::value; j++)
            reader >> vertex.getPoint()[j];
          
          vertex.id(i);
          domain.add(vertex);
        }
    
        //std::cout << "DONE" << std::endl;
    
        if (domain.segment_size() == 0)
          domain.create_segments(1);

        if (!reader.good())
          throw bad_file_format_exception(filename, "EOF encountered when reading number of cells.");
          

        //
        // Read cells:
        //
        reader >> cell_num;
        domain.reserve_cells(cell_num);
        CellType cell;
        //std::cout << "Filling " << cell_num << " cells:" << std::endl;
    
        for (int i=0; i<cell_num; ++i)
        {
          long vertex_num;
          VertexType *vertices[topology::subcell_desc<CellTag, 0>::num_elements];

          if (!reader.good())
            throw bad_file_format_exception(filename, "EOF encountered while reading cells (segment index expected).");
          
          size_t segment_index;
          reader >> segment_index;
    
          for (int j=0; j<topology::subcell_desc<CellTag, 0>::num_elements; ++j)
          {
            if (!reader.good())
              throw bad_file_format_exception(filename, "EOF encountered while reading cells (vertex ID expected).");
            
            reader >> vertex_num;
            vertices[j] = &(domain.vertex(vertex_num - 1));  //Note that Netgen uses vertex indices with base 1
          }
    
          //std::cout << std::endl << "Adding cell: " << &cell << " at " << cell_id << std::endl;
          cell.setVertices(&(vertices[0]));
          cell.id(i);
          assert(domain.segment_size() >= segment_index && "Segment index in input file out of bounds!");
          domain.segment(segment_index - 1).add(cell);  //note that Netgen uses a 1-based indexing scheme, while ViennaGrid uses a zero-based one
    
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
