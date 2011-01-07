/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */

#ifndef VIENNAMESH_IO_SGF_WRITER_GUARD
#define VIENNAMESH_IO_SGF_WRITER_GUARD

#include <fstream>
#include <iostream>
#include "viennamesh/forwards.h"
#include "viennamesh/io/helper.hpp"

namespace viennamesh{
  
  struct sgf_writer
  {
    template <typename DomainType>
    int operator()(DomainType & domain, std::string const & filename)
    {
      typedef typename DomainType::Configuration                     DomainConfiguration;
    
      typedef typename DomainConfiguration::CoordType                 CoordType;
      typedef typename DomainConfiguration::DimensionTag              DimensionTag;
      typedef typename DomainConfiguration::CellTag                   CellTag;
      typedef typename DomainConfiguration::BoundaryReadTag           BoundaryReadTag;
    
      typedef typename DomainTypes<DomainConfiguration>::PointType    Point;
      typedef typename DomainTypes<DomainConfiguration>::VertexType   Vertex;
      typedef typename DomainTypes<DomainConfiguration>::CellType     Cell;
      typedef typename DomainTypes<DomainConfiguration>::SegmentType     Segment;
    
      typedef typename DomainTypes<DomainConfiguration>::VertexIterator      VertexIterator;
      typedef typename DomainTypes<DomainConfiguration>::CellIterator        CellIterator;

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
      writer << "Cells: " << segment.template size<CellTag::TopoLevel>() << std::endl;
    
      for (CellIterator cit = segment.template begin<CellTag::TopoLevel>();
          cit != segment.template end<CellTag::TopoLevel>();
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

} //namespace

#endif


