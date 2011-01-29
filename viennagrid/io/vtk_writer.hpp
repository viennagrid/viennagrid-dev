/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
   authors:    Markus Bina                        bina@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_IO_VTK_WRITER_GUARD
#define VIENNAGRID_IO_VTK_WRITER_GUARD

#include <fstream>
#include <iostream>
#include "viennagrid/forwards.h"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/vtk_tags.hpp"

namespace viennagrid
{
  namespace io
  {

    /////////////////// VTK export ////////////////////////////

    //helper: translate element tags to VTK-element types
    // (see: http://www.vtk.org/pdf/file-formats.pdf , page 9)

    template < typename DomainType >
    class Vtk_writer
    {
      protected:

      typedef typename DomainType::config_type                         DomainConfiguration;

      typedef typename DomainConfiguration::numeric_type               CoordType;
      typedef typename DomainConfiguration::dimension_tag              DimensionTag;
      typedef typename DomainConfiguration::cell_tag                   CellTag;

      typedef typename result_of::point_type<DomainConfiguration>::type                              PointType;
      typedef typename result_of::ncell_type<DomainConfiguration, 0>::type                           VertexType;
      typedef typename result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type     CellType;

      //typedef typename DomainTypes<DomainConfiguration>::segment_type  Segment;
      
      typedef typename viennagrid::result_of::ncell_container<DomainType, 0>::type   VertexContainer;
      typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
          
      typedef typename viennagrid::result_of::ncell_container<DomainType, 1>::type   EdgeContainer;
      typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

      typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level-1>::type   FacetContainer;
      typedef typename viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

      typedef typename viennagrid::result_of::ncell_container<DomainType, CellTag::topology_level>::type     CellContainer;
      typedef typename viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;


      typedef typename viennagrid::result_of::iterator<CellType, 0>::type      VertexOnCellIterator;

      void writeHeader(std::ofstream & writer)
      {
        writer << "<?xml version=\"1.0\"?>" << std::endl;
        writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
        writer << " <UnstructuredGrid>" << std::endl;
      }

      template <typename Segment>
      void writePoints(Segment & segment, std::ofstream & writer)
      {
        writer << "   <Points>" << std::endl;
        writer << "    <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

        VertexContainer vertices = viennagrid::ncells<0>(segment);
        for (VertexIterator vit = vertices.begin();
            vit != vertices.end();
            ++vit)
        {
          PointWriter<DimensionTag::value>::write(writer, vit->getPoint());

          // add 0's for less than three dimensions
            if (DimensionTag::value == 2)
              writer << " " << 0;
            if(DimensionTag::value == 1)
              writer << " " << 0 << " " << 0;

            writer << std::endl;
        }
        writer << std::endl;
        writer << "    </DataArray>" << std::endl;
        writer << "   </Points> " << std::endl;
      } //writePoints()

      template <typename Segment>
      void writeCells(Segment & segment, std::ofstream & writer)
      {
        writer << "   <Cells> " << std::endl;
        writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;
        CellContainer cells = viennagrid::ncells<CellTag::topology_level>(segment);
        for (CellIterator cit = cells.begin();
            cit != cells.end();
            ++cit)
        {
            CellType & cell = *cit;
            for (VertexOnCellIterator vocit = cell.template begin<0>();
                vocit != cell.template end<0>();
                ++vocit)
            {
              writer << vocit->getID() << " ";
            }
            writer << std::endl;
          }
          writer << std::endl;
          writer << "    </DataArray>" << std::endl;

          writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
          for (size_t offsets = 1;
               offsets <= segment.template size<CellTag::topology_level>();
               ++offsets)
          {
            writer << ( offsets * viennagrid::subcell_traits<CellTag, 0>::num_elements) << " ";
          }
          writer << std::endl;
          writer << "    </DataArray>" << std::endl;

          writer << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
          for (size_t offsets = 1;
                offsets <= segment.template size<CellTag::topology_level>();
                ++offsets)
          {
            writer << ELEMENT_TAG_TO_VTK_TYPE<CellTag>::ReturnValue << " ";
          }
          writer << std::endl;
          writer << "    </DataArray>" << std::endl;
          writer << "   </Cells>" << std::endl;
      }

      // TODO [MB] deprecated retrieveQuantity<double>(keyname);
      template <typename Segment, typename KeyType >
      void writePointData(Segment & segment, KeyType & keyname, std::ofstream & writer)
      {
        writer << "   <PointData Scalars=\"scalars\">" << std::endl;
        writer << "    <DataArray type=\"Float32\" Name=\"result\" format=\"ascii\">" << std::endl;
        for (VertexIterator vit = segment.template getLevelIteratorBegin<0>();
            vit != segment.template getLevelIteratorEnd<0>();
            ++vit)
        {
          double val = 0.0;
          // TODO [MB] DEPRECATED ... ref is ViennaData
          //double val = vit->template retrieveQuantity<double>(keyname);
          writer << val << " ";
        }
        writer << std::endl;
        writer << "    </DataArray>" << std::endl;
        writer << "   </PointData>"  << std::endl;
      } //writePointData

      void writeFooter(std::ofstream & writer)
      {
        writer << " </UnstructuredGrid>" << std::endl;
        writer << "</VTKFile>" << std::endl;
      } 

    public:

      Vtk_writer() { };
      ~Vtk_writer() { };

      int writeDomain(DomainType & domain, std::string const & filename)
      {
        return this->operator()(domain, filename);
      }

      int operator()(DomainType & domain, std::string const & filename)
      {
        std::ofstream writer(filename.c_str());

        writeHeader(writer);

        //TODO Add iteration over segments again
        //for (SegmentIterator segit = domain.begin(); segit != domain.end(); ++segit)
        //{
          std::cout << "Writing segment" << std::endl;
          //Segment & curSeg = *segit;

          writer << "  <Piece NumberOfPoints=\""
                << domain.template size<0>()
                << "\" NumberOfCells=\""
                << domain.template size<CellTag::topology_level>()
                << "\">" << std::endl;


          writePoints(domain, writer);

          //writePointData(curSeg, writer);

          writeCells(domain, writer);

          writer << "  </Piece>" << std::endl;
        //}
        
        writeFooter(writer);

        return EXIT_SUCCESS;
      }

    };

  template < typename DomainType > int exportVTK(DomainType domain, std::string const & filename)
  {
    Vtk_writer<DomainType> vtk_writer;
    return vtk_writer.writeDomain(domain, filename);
  }


  #if USE_WRITEQUANVTK
    //obsolete for the moment
    template <typename FEMConfig, typename DomainType, typename Vector>
    void writeQuantityToVTK(DomainType & domain, long level, Vector const & vec, std::string outfile, long component = 0)
    {
      typedef typename DomainType::Configuration                     DomainConfiguration;
    
      typedef typename DomainConfiguration::CoordType                 CoordType;
      typedef typename DomainConfiguration::DimensionTag              DimensionTag;
      typedef typename DomainConfiguration::CellTag                   CellTag;
      typedef typename DomainConfiguration::BoundaryReadTag           BoundaryReadTag;
    
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

      for (SegmentIterator segit = domain.getSegmentBegin();
            segit != domain.getSegmentEnd();
            ++segit)
      {
        //testing only:
        Segment & curSeg = segit->getRefinedSegment(level);
        //Segment & curSeg = *segit;

        curSeg.template getLevelIteratorBegin<0>()->setCurrentSegment(curSeg);
        curSeg.template getLevelIteratorBegin<0>()
                ->template reserveQuantity<double>(keyString);
      
        for (VertexIterator vit = curSeg.template getLevelIteratorBegin<0>();
              vit != curSeg.template getLevelIteratorEnd<0>();
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

      writeDomainVTK(domain, level, outfile, keyString);
      std::cout << "DONE" << std::endl;
    
    }
  #endif
  } //namespace io
} //namespace viennagrid

#endif
