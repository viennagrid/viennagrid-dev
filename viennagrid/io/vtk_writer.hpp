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
#include "viennadata/interface.hpp"

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
      
      typedef typename viennagrid::result_of::const_ncell_container<DomainType, 0>::type   VertexContainer;
      typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
          
      typedef typename viennagrid::result_of::const_ncell_container<DomainType, 1>::type   EdgeContainer;
      typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

      typedef typename viennagrid::result_of::const_ncell_container<DomainType, CellTag::topology_level-1>::type   FacetContainer;
      typedef typename viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

      typedef typename viennagrid::result_of::const_ncell_container<DomainType, CellTag::topology_level>::type     CellContainer;
      typedef typename viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;


      typedef typename viennagrid::result_of::const_ncell_container<CellType, 0>::type      VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type   VertexOnCellIterator;
      //typedef typename viennagrid::result_of::iterator<CellType, 0>::type      VertexOnCellIterator;

      void writeHeader(std::ofstream & writer)
      {
        writer << "<?xml version=\"1.0\"?>" << std::endl;
        writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
        writer << " <UnstructuredGrid>" << std::endl;
      }

      template <typename Segment>
      void writePoints(Segment const & segment, std::ofstream & writer)
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
      void writeCells(Segment const & segment, std::ofstream & writer)
      {
        writer << "   <Cells> " << std::endl;
        writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;
        CellContainer cells = viennagrid::ncells<CellTag::topology_level>(segment);
        for (CellIterator cit = cells.begin();
            cit != cells.end();
            ++cit)
        {
            VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(*cit);
            for (VertexOnCellIterator vocit = vertices_on_cell.begin();
                vocit != vertices_on_cell.end();
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

      template <typename Segment, typename KeyType >
      void writePointData(Segment const & segment, KeyType const & keyname, std::ofstream & writer)
      {
        writer << "   <PointData Scalars=\"scalars\">" << std::endl;
        writer << "    <DataArray type=\"Float32\" Name=\"result\" format=\"ascii\">" << std::endl;
        
        VertexContainer vertices = viennagrid::ncells<0>(segment);
        for (VertexIterator vit = vertices.begin();
            vit != vertices.end();
            ++vit)
        {
          //TODO: User provided key instead of hard-coded string
          writer << viennadata::access<std::string, double>("vtk_data")(*vit) << " ";
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

      int writeDomain(DomainType const & domain, std::string const & filename)
      {
        return this->operator()(domain, filename);
      }

      int operator()(DomainType const & domain, std::string const & filename)
      {
        std::ofstream writer(filename.c_str());

        writeHeader(writer);

        //TODO Add iteration over segments again
        //for (SegmentIterator segit = domain.begin(); segit != domain.end(); ++segit)
        //{
          //std::cout << "Writing segment" << std::endl;
          //Segment & curSeg = *segit;

          writer << "  <Piece NumberOfPoints=\""
                << domain.template size<0>()
                << "\" NumberOfCells=\""
                << domain.template size<CellTag::topology_level>()
                << "\">" << std::endl;


          writePoints(domain, writer);

          //TODO User-provided key instead of hard-coded string
          writePointData(domain, std::string("vtk_data"), writer);

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


  } //namespace io
} //namespace viennagrid

#endif
