/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_IO_OPENDX_WRITER_GUARD
#define VIENNAGRID_IO_OPENDX_WRITER_GUARD

#include <fstream>
#include <iostream>
#include "viennagrid/forwards.h"

namespace viennagrid
{
  namespace io
  {
  
    /////////////////// OpenDX export ////////////////////////////
    
    
    // Fix for data-explorer-bug: if floating-values occur, no integers (i.e. only zeros after decimal point) allowed
    double DXfixer(double value)
    {
      if (value * 10000 == static_cast<long>(value * 10000))
        return value + 0.00001;
    
      return value;
    }

    template <int DIM>
    struct DXHelper {};

    template <>
    struct DXHelper<1>
    {
      static std::string getAttributes()
      { return "attribute \"element type\" string \"line\" "; }
    };
    
    template <>
    struct DXHelper<2>
    {
      static std::string getAttributes()
      { return "attribute \"element type\" string \"triangles\" "; }
    };
    
    template <>
    struct DXHelper<3>
    {
      static std::string getAttributes()
      { return "attribute \"element type\" string \"tetrahedra\" "; }
    };
    
    struct opendx_writer
    {
      template <typename DomainType>
      int operator()(DomainType & domain, std::string const & filename, bool isVertexOriented = true)
      {
        typedef typename DomainType::Configuration                     DomainConfiguration;
      
        typedef typename DomainConfiguration::CoordType                 CoordType;
        typedef typename DomainConfiguration::DimensionTag              DimensionTag;
        typedef typename DomainConfiguration::CellTag                   CellTag;
      
        typedef typename DomainTypes<DomainConfiguration>::PointType    Point;
        typedef typename DomainTypes<DomainConfiguration>::VertexType   Vertex;
        typedef typename DomainTypes<DomainConfiguration>::CellType     Cell;

        typedef typename DomainTypes<DomainConfiguration>::SegmentType  Segment;
      
        typedef typename DomainTypes<DomainConfiguration>::VertexIterator      VertexIterator;
        typedef typename DomainTypes<DomainConfiguration>::CellIterator        CellIterator;
        typedef typename DomainTypes<DomainConfiguration>::SegmentIterator     SegmentIterator;

        typedef typename DomainTypes<DomainConfiguration>::VertexOnCellIterator      VertexOnCellIterator;

        typedef DXHelper<DimensionTag::dim>  DXHelper;
      
        std::ofstream writer(filename.c_str());
      
        long pointnum = 0;
        for (SegmentIterator segit = domain.begin();
              segit != domain.end();
              ++segit)
        {
          //testing:
          //Segment & curSeg = segit->getRefinedSegment(level);
          Segment & curSeg = *segit;

          pointnum += curSeg.template size<0>();
        }
      
        writer << "object \"points\" class array type float rank 1 shape " << DimensionTag::dim << " items ";
        writer << pointnum << " data follows" << std::endl;
      
        //Nodes:
        for (SegmentIterator segit = domain.begin();
              segit != domain.end();
              ++segit)
        {
          //testing:
          //Segment & curSeg = segit->getRefinedSegment(level);
          Segment & curSeg = *segit;

          for (VertexIterator vit = curSeg.template begin<0>();
              vit != curSeg.template end<0>();
              ++vit)
          {
            PointWriter<Point, DimensionTag::dim>::write(writer, vit->getPoint());
            writer << std::endl;
          }
        }
        writer << std::endl;

        //Cells:
        long cellnum = 0;
        for (SegmentIterator segit = domain.begin();
              segit != domain.end();
              ++segit)
        {
          //testing:
          //Segment & curSeg = segit->getRefinedSegment(level);
          Segment & curSeg = *segit;

          cellnum += curSeg.template size<Cell::ElementTag::TopoLevel>();
        }
        writer << "object \"grid_Line_One\" class array type int rank 1 shape " << (DimensionTag::dim + 1) << " items " << cellnum << " data follows" << std::endl;

        long id_offset = 0;
        for (SegmentIterator segit = domain.begin();
              segit != domain.end();
              ++segit)
        {
          //testing:
          //Segment & curSeg = segit->getRefinedSegment(level);
          Segment & curSeg = *segit;


          for (CellIterator cit = curSeg.template begin<Cell::ElementTag::TopoLevel>();
              cit != curSeg.template end<Cell::ElementTag::TopoLevel>();
              ++cit)
          {
            Cell & cell = *cit;
            for (VertexOnCellIterator vocit = cell.template begin<0>();
                vocit != cell.template end<0>();
                ++vocit)
            {
              Vertex & vertex = *vocit;
              writer << (vertex.getID() + id_offset) << " ";
            }
            writer << std::endl;
          }
          id_offset += curSeg.template size<0>();
        }
      
        writer << DXHelper::getAttributes() << std::endl;
        writer << "attribute \"ref\" string \"positions\" " << std::endl;
        writer << std::endl;
      
        //set output-format:
        writer.flags(std::ios::fixed);
        writer.precision(5);
      
        //write quantity:
        if (isVertexOriented)
        {
          writer << "object \"VisData\" class array items " << pointnum << " data follows" << std::endl;
          //some quantity here
          for (SegmentIterator segit = domain.begin();
                segit != domain.end();
                ++segit)
          {
            //testing:
            //Segment & curSeg = segit->getRefinedSegment(level);
            Segment & curSeg = *segit;

            //curSeg.template begin<0>()->setCurrentSegment(curSeg);
            for (VertexIterator vit = curSeg.template begin<0>();
                vit != curSeg.template end<0>();
                ++vit)
            {
              //Vertex & vertex = *vit;
              double val = 1.0;//vertex.template retrieveQuantity<double>(keyname);
              writer << DXfixer(val);
              writer << std::endl;
            }
          }
          writer << "attribute \"dep\" string \"positions\"" << std::endl;
        }
        else
        {
          writer << "object \"VisData\" class array items " << cellnum << " data follows" << std::endl;

          for (SegmentIterator segit = domain.begin();
                segit != domain.end();
                ++segit)
          {
            //testing:
            //Segment & curSeg = segit->getRefinedSegment(level);
            Segment & curSeg = *segit;

            //curSeg.template begin<0>()->setCurrentSegment(curSeg);

            //some quantity here
            for (CellIterator cit = curSeg.template begin<Cell::ElementTag::TopoLevel>();
                cit != curSeg.template end<Cell::ElementTag::TopoLevel>();
                ++cit)
            {
              //Cell & cell = *cit;
              //writer << DXfixer(cell.template retrieveQuantity<double>(keyname));
              writer << DXfixer(1.0);
              writer << std::endl;
            }
          }
          writer << "attribute \"dep\" string \"connections\"" << std::endl;
        }
      
        writer << std::endl;
        writer << "object \"AttPotential\" class field " << std::endl;
        writer << "component \"data\" \"VisData\" " << std::endl;
        writer << "component \"positions\" \"points\"" << std::endl;
        writer << "component \"connections\" \"grid_Line_One\"" << std::endl;
      
        return EXIT_SUCCESS;
      } // operator()

    }; // opendx_writer

    //obsolete:
    template <typename FEMConfig, typename DomainType, typename Vector>
    void writeQuantityToDX(DomainType & domain, long level, Vector const & vec, std::string outfile, long component = 0)
    {
      typedef typename DomainType::Configuration                     DomainConfiguration;
    
      typedef typename DomainConfiguration::CoordType                 CoordType;
      typedef typename DomainConfiguration::DimensionTag              DimensionTag;
      typedef typename DomainConfiguration::CellTag                   CellTag;
    
      typedef typename DomainTypes<DomainConfiguration>::PointType    Point;
      typedef typename DomainTypes<DomainConfiguration>::VertexType   Vertex;
      typedef typename DomainTypes<DomainConfiguration>::CellType     Cell;

      typedef typename DomainTypes<DomainConfiguration>::SegmentType  Segment;
    
      typedef typename DomainTypes<DomainConfiguration>::VertexIterator      VertexIterator;
      typedef typename DomainTypes<DomainConfiguration>::CellIterator        CellIterator;
      typedef typename DomainTypes<DomainConfiguration>::SegmentIterator     SegmentIterator;

      typedef typename DomainTypes<DomainConfiguration>::VertexOnCellIterator      VertexOnCellIterator;

      typedef typename FEMConfig::MappingKey                             MappingKey;
      typedef typename FEMConfig::BoundaryKey                             BoundaryKey;
      typedef typename FEMConfig::BoundaryData                            BoundaryData;
      typedef typename FEMConfig::BoundaryData2                           BoundaryData2;
      typedef typename FEMConfig::BoundaryData3                           BoundaryData3;
    
      long dim = FEMConfig::ResultDimension::dim;
      std::string keyString = "result";

      for (SegmentIterator segit = domain.begin();
            segit != domain.end();
            ++segit)
      {
        //testing only:
        Segment & curSeg = segit->getRefinedSegment(level);
        //Segment & curSeg = *segit;

        curSeg.template begin<0>()->setCurrentSegment(curSeg);
        curSeg.template begin<0>()
                ->template reserveQuantity<double>(keyString);
      
        for (VertexIterator vit = curSeg.template begin<0>();
              vit != curSeg.template end<0>();
              ++vit)
        {
          if (vit->template retrieveQuantity<bool>( BoundaryKey()))
          {
            double bndval = 0;
            //terribe runtime-selection: (however, boundary vertices usually sparse)
            if (component == 0)
              bndval = (*vit).template retrieveQuantity<double>( BoundaryData() );
            else if (component == 1)
              bndval = (*vit).template retrieveQuantity<double>( BoundaryData2() );
            else if (component == 2)
              bndval = (*vit).template retrieveQuantity<double>( BoundaryData3() );
      
            (*vit).template storeQuantity<double>( keyString, bndval );
          }
          else
          {
            long index = (*vit).template retrieveQuantity<long>( MappingKey() );
            (*vit).template storeQuantity<double>( keyString, vec(dim * index + component) );
          }
        }
      }  

      writeDomainDX(domain, level, outfile, keyString, true);
      std::cout << "DONE" << std::endl;
    
    }

  } //namespace io
} //namespace viennagrid

#endif


