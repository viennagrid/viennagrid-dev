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
        typedef typename DomainType::Configuration                     DomainConfiguration;
      
        typedef typename DomainConfiguration::numeric_type                 CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;
      
        typedef typename DomainTypes<DomainConfiguration>::point_type    Point;
        typedef typename DomainTypes<DomainConfiguration>::vertex_type   Vertex;
        typedef typename DomainTypes<DomainConfiguration>::cell_type     Cell;
        typedef typename DomainTypes<DomainConfiguration>::segment_type     Segment;
      
        typedef typename DomainTypes<DomainConfiguration>::vertex_iterator      VertexIterator;
        typedef typename DomainTypes<DomainConfiguration>::cell_iterator        CellIterator;

        typedef typename DomainTypes<DomainConfiguration>::VertexOnCellIterator      VertexOnCellIterator;
      
        std::ofstream writer(filename.c_str());
        
        Segment & segment = *(domain.begin());
      
        writer << "Dimension: " << DimensionTag::dim << std::endl;
        writer << "Vertices: " << segment.template size<0>() << std::endl;
      
        //segment.template begin<0>()->setCurrentSegment(segment);

        //Nodes:
        for (VertexIterator vit = segment.template begin<0>();
            vit != segment.template end<0>();
            ++vit)
        {
          Vertex & vertex = *vit;
          writer << vertex.getID() << " ";
          PointWriter<Point, DimensionTag::dim>::write(writer, vertex.getPoint());
          writer << std::endl;
        }
      
        //Cells:
        writer << "Cell-Type: Simplex" << std::endl;
        writer << "Cells: " << segment.template size<CellTag::topology_level>() << std::endl;
      
        for (CellIterator cit = segment.template begin<CellTag::topology_level>();
            cit != segment.template end<CellTag::topology_level>();
            ++cit)
        {
          Cell & cell = *cit;
          writer << cell.getID() << " ";
          for (VertexOnCellIterator vocit = cell.template begin<0>();
                vocit != cell.template end<0>();
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


