/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
   
======================================================================= */


#ifndef VIENNAMESH_IO_VTK_READER_GUARD
#define VIENNAMESH_IO_VTK_READER_GUARD

/** @file vtk_reader.hpp
 *  @brief    This is a very simple vtk-reader implementation. There's only a part of the vtk-standard implemented.
 */

#include <fstream>
#include <iostream>
#include <vector>
#include "viennagrid/forwards.h"
#include "viennagrid/io/vtk_tags.hpp"
#include "stdio.h"
#include "helper.hpp"

//using namespace std;

namespace viennagrid
{
  namespace io
  {
    
    template <typename DomainType>
    class Vtk_reader
    {
    protected:
        typedef typename DomainType::config_type                     DomainConfiguration;

        typedef typename DomainConfiguration::numeric_type                 CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;

        typedef typename DomainTypes<DomainConfiguration>::point_type    Point;
        typedef typename DomainTypes<DomainConfiguration>::vertex_type   Vertex;
        typedef typename DomainTypes<DomainConfiguration>::cell_type     Cell;
        typedef typename DomainTypes<DomainConfiguration>::segment_type  Segment;

        typedef typename DomainTypes<DomainConfiguration>::vertex_iterator      VertexIterator;
        typedef typename DomainTypes<DomainConfiguration>::cell_iterator        CellIterator;

        std::ifstream reader;

        void checkFilename(std::string const & filename) const
        {
            long len = filename.size();
            std::string ending = filename.substr(len-4,len-1);

            if(ending != ".vtk")
            {
              std::cerr << "This is a wrong file-ending!" << std::endl;
              std::cerr << "Expected: \".vtk\", got: \"" <<  ending << "\"" << std::endl;
              throw;
            }
        }

        void openVTKFile(std::string const & filename)
        {
            this->reader.open(filename.c_str());
            if(!this->reader)
            {
              std::cerr << "Error open file!" << std::endl;
              throw;
            }
        }

        void checkNextToken(std::string const & expectedToken)
        {
          std::string token;
          this->reader >> token;
          if(token != expectedToken) {
            std::cerr << "Expected \"" << expectedToken << "\", but got \"" << token << "\"" << std::endl;
            throw;
          }
        }

        long getNumberOfNodes()
        {
          std::string token;

          this->reader >> token;

          std::cout << "Number of Nodes: " << token << ", " << token.substr(0,16) << std::endl;

          long lastIdx = token.size()-1;

          if(token.substr(0,16) != "NumberOfPoints=\"" || token.substr(lastIdx,lastIdx) != "\"")
          {
            std::cerr << "Expected \"NumberOfPoints=\"...\"\", but got " << token << std::endl;
              throw;
          }

          token.replace(0,16,"");
          lastIdx = token.size()-1;
          token.replace(lastIdx,lastIdx,"");

          return atoi(token.c_str());
        }

        long getNumberOfCells()
        {
          std::string token;

          this->reader >> token;

          std::cout << "Number of Cells: " << token << ", " << token.substr(0,15) << std::endl;

          long lastIdx = token.size() - 1;

          std::cout << "Test" << std::endl;

          if(token.substr(0,15) != "NumberOfCells=\"" || token.substr(lastIdx-1,lastIdx) != "\">")
          {
            std::cerr << "Expected \"NumberOfCells=\"...\"\", but got " << token << std::endl;
            throw;
          }

          token.replace(0,15,"");
          lastIdx =token.size()-1;
          token.replace(lastIdx-1,lastIdx,"");

          return atoi(token.c_str());
        }

        long getNumberOfComponents()
        {
          std::string token;

          this->reader >> token;

          std::cout << "NumberOfComponents: " << token << ", " << token.substr(0,15) << std::endl;

          long lastIdx = token.size()-1;

          if(token.substr(0,20) != "NumberOfComponents=\"" || token.substr(lastIdx,lastIdx) != "\"")
          {
            std::cerr << "Expected \"NumberOfComponents=\"...\"\", but got " << token << std::endl;
            throw;
          }

          token.replace(0,20,"");
          lastIdx =token.size()-1;
          token.replace(lastIdx,lastIdx,"");

          return atoi(token.c_str());
        }

        void readNodeCoordinates(Segment & segment, long nodeNum, long numberOfComponents)
        {
          Vertex vertex;
          double nodeCoord;

          for(long i = 0; i < nodeNum; i++)
          {
            CoordType vmCoords[DimensionTag::value];

            std::cout << "Vertex#" << i << ": ";

            for(long j = 0; j < numberOfComponents; j++)
            {
              this->reader >> nodeCoord;
              vmCoords[j] = nodeCoord;
              std::cout << nodeCoord << " ";
            }

            std::cout << std::endl;

            //insert node into segment:
            vertex.getPoint().setCoordinates(vmCoords);
            vertex.setID(i);
            segment.template add<0>(i, vertex);
          }
        }

        void readNodeIndices(std::vector<long> & cells)
        {
          long cellNode = 0;
          std::string token;
          this->reader >> token;

          while(token != "</DataArray>" && strChecker::myIsNumber(token) )
          {
            cellNode = atoi(token.c_str());
            cells.push_back(cellNode);

            std::cout << "Vertex#: " << cellNode << std::endl;
            this->reader >> token;
          }
          //cout << "#: " << cellNode << endl;
        }

        void readOffsets(std::vector<long> & offsets)
        {
            //****************************************************************************
            // read in the offsets: describe the affiliation of the nodes to the cells
            // (see: http://www.vtk.org/pdf/file-formats.pdf , page 9)
            //****************************************************************************

            std::string token;
            long offset = 0;
            this->reader >> token;

            while(token != "</DataArray>" && strChecker::myIsNumber(token) )
            {
              offset = atoi(token.c_str());
              offsets.push_back(offset);

              std::cout << "Vertex#: " << offset << std::endl;
              this->reader >> token;
            }
        }

        void readCells(Segment & segment, long cellNum, std::vector<long> const & cells, std::vector<long> const & offsets)
        {
            //***************************************************
            // building up the cells in ViennaGrid
            // -------------------------------------------------
            // "cells"   ... contain the indices of the nodes
            // "offsets" ... contain the information about
            //               the affiliation of the nodes
            //               to the cells
            //***************************************************
            Cell cell;
            long numVertices = 0;
            long offsetIdx = 0;

            segment.reserveCells(cellNum);

            for (long i = 0; i < cellNum; i++)
            {
              //*************************************************
              // choose the offset index for the i-th cell
              // in the "cells"-vector and calculate the
              // number of vertices belonging to the i-th cell
              //*************************************************
              if(i == 0)
              {
                offsetIdx = 0;
                numVertices = offsets[i];
              }
              else
              {
                offsetIdx = offsets[i-1];
                numVertices = offsets[i]-offsets[i-1];
              }

              std::vector<Vertex *> vertices(numVertices);

              std::cout << "offsetIdx: " << offsetIdx << ", numVertices: " << numVertices << std::endl;

              //****************************************************
              // read out the node indices form the "cells"-vector
              // and add the cells to the "vertices"-array
              //****************************************************
              for (long j = 0; j < numVertices; j++)
              {
                vertices[j] = segment.getVertexAddress(cells[j + offsetIdx]);
                std::cout << "j+offsetidx: " << j+offsetIdx << std::endl;
              }

              cell.setVertices(&(vertices[0]));
              cell.setID(i);
              segment.template add<CellTag::topology_level>(i, cell);
            }
        }

    public:
      Vtk_reader() { };
      ~Vtk_reader() { };

      int readDomain(DomainType & domain, std::string const & filename)
      {
        return this->operator()(domain, filename);
      }

      int operator()(DomainType & domain, std::string const & filename)
      {
        try
        {
          checkFilename(filename);
          openVTKFile(filename);

          Segment & segment = *(domain.begin());

          std::cout << "Reading vtk-file " << filename << std::endl;

          long nodeNum = 0;
          long cellNum = 0;
          long numberOfComponents = 0;           
          std::vector<long> cells;     // cells
          std::vector<long> offsets;   // .vtk-file stores offsets
                                  // (see: http://www.vtk.org/pdf/file-formats.pdf , page 9)
                      
          checkNextToken("<?xml");
          checkNextToken("version=\"1.0\"?>");
          checkNextToken("<VTKFile");
          checkNextToken("type=\"UnstructuredGrid\"");
          checkNextToken("version=\"0.1\"");
          checkNextToken("byte_order=\"LittleEndian\">");
          checkNextToken("<UnstructuredGrid>");
          checkNextToken("<Piece");
          
          nodeNum = getNumberOfNodes();
          segment.template reserveVertices(nodeNum);
          std::cout << "#Nodes: " << nodeNum << std::endl;

          cellNum = getNumberOfCells();
          std::cout << "#Cells: " << cellNum << std::endl;
          
          checkNextToken("<Points>");
          checkNextToken("<DataArray");
          checkNextToken("type=\"Float32\"");

          numberOfComponents = getNumberOfComponents();
          
          std::cout << "Dimensions: " << numberOfComponents << std::endl;

          checkNextToken("format=\"ascii\">");
          
          // Read in the coordinates of the nodes
          readNodeCoordinates(segment, nodeNum, numberOfComponents);
          
          checkNextToken("</DataArray>");
          checkNextToken("</Points>");
          checkNextToken("<Cells>");
          checkNextToken("<DataArray");
          checkNextToken("type=\"Int32\"");
          checkNextToken("Name=\"connectivity\"");
          checkNextToken("format=\"ascii\">");

          std::cout << "Cells: " << std::endl;
          
          readNodeIndices(cells);

          checkNextToken("<DataArray");
          checkNextToken("type=\"Int32\"");
          checkNextToken("Name=\"offsets\"");
          checkNextToken("format=\"ascii\">");

          readOffsets(offsets);

          checkNextToken("<DataArray");
          checkNextToken("type=\"UInt8\"");
          checkNextToken("Name=\"types\"");
          checkNextToken("format=\"ascii\">");

          readOffsets(offsets);

          checkNextToken("</Cells>");
          checkNextToken("</Piece>");
          checkNextToken("</UnstructuredGrid>");
          checkNextToken("</VTKFile>");

          readCells(segment, cellNum, cells, offsets);

        }
        catch (...) {
          std::cerr << "Problems while reading file " << filename << std::endl;
        }
      
        std::cout << "File-Reader finished!" << std::endl;  
        
        return EXIT_SUCCESS;
      } //operator()
    }; //class vtk_reader

    template < typename DomainType > 
    int importVTK(DomainType domain, std::string const & filename)
    {
      Vtk_reader<DomainType> vtk_reader;
      return vtk_reader.readDomain(domain, filename);
    }

  } //namespace io
} //namespace viennagrid

#endif
