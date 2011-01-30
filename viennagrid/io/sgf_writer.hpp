/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_IO_SGF_WRITER_GUARD
#define VIENNAGRID_IO_SGF_WRITER_GUARD

#include <fstream>
#include <iostream>
#include "viennagrid/forwards.h"
#include "viennagrid/io/helper.hpp"

namespace viennagrid
{
  namespace io
  {
  
    struct sgf_writer
    {
      template <typename DomainType>
      int operator()(DomainType & domain, std::string const & filename)
      {
        typedef typename DomainType::config_type                     DomainConfiguration;
      
        typedef typename DomainConfiguration::numeric_type                 CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;
      
        typedef typename result_of::point_type<DomainConfiguration>::type                              PointType;
        typedef typename result_of::ncell_type<DomainConfiguration, 0>::type                           VertexType;
        typedef typename result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type     CellType;
        //typedef typename DomainTypes<DomainConfiguration>::segment_type     Segment;
      
        typedef typename viennagrid::result_of::ncell_container<DomainType, 0>::type   VertexContainer;
        typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
            
        typedef typename viennagrid::result_of::ncell_container<DomainType, 1>::type   EdgeContainer;
        typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

        typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level-1>::type   FacetContainer;
        typedef typename viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

        typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level>::type     CellContainer;
        typedef typename viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;

        typedef typename viennagrid::result_of::ncell_container<CellType, 0>::type      VertexOnCellContainer;
        typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type   VertexOnCellIterator;
      
        std::ofstream writer(filename.c_str());
        
        //Segment & segment = *(domain.begin());
      
        writer << "Dimension: " << DimensionTag::value << std::endl;
        writer << "Vertices: " << domain.template size<0>() << std::endl;
      
        //segment.template begin<0>()->setCurrentSegment(segment);

        //Nodes:
        VertexContainer vertices = viennagrid::ncells<0>(domain);
        for (VertexIterator vit = vertices.begin();
            vit != vertices.end();
            ++vit)
        {
          VertexType & vertex = *vit;
          writer << vertex.getID() << " ";
          PointWriter<DimensionTag::value>::write(writer, vertex.getPoint());
          writer << std::endl;
        }
      
        //Cells:
        writer << "Cell-Type: Simplex" << std::endl;
        writer << "Cells: " << domain.template size<CellTag::topology_level>() << std::endl;
      
        CellContainer cells = viennagrid::ncells<CellTag::topology_level>(domain);
        for (CellIterator cit = cells.begin();
            cit != cells.end();
            ++cit)
        {
          CellType & cell = *cit;
          writer << cell.getID() << " ";
          VertexOnCellContainer vertices_for_cell = viennagrid::ncells<0>(cell);
          for (VertexOnCellIterator vocit = vertices_for_cell.begin();
                vocit != vertices_for_cell.end();
                ++vocit)
          {
            writer << vocit->getID() << " ";
          }

          writer << std::endl;
        }
        return EXIT_SUCCESS;
      } //operator()
      
    }; // sgf_writer
    
  } //namespace io
} //namespace viennagrid

#endif


