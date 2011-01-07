/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
   
   This is a very simple vtk-reader implementation. There's only
   a part of the vtk-standard implemented.
======================================================================= */


#ifndef VIENNAMESH_IO_VTK_READER_GUARD
#define VIENNAMESH_IO_VTK_READER_GUARD

#include <fstream>
#include <iostream>
#include <vector>
#include "viennamesh/forwards.h"
#include "viennamesh/io/vtk_tags.hpp"
#include "stdio.h"
#include "helper.hpp"

using namespace std;

namespace viennamesh
{ 
  template <typename DomainType>
  class Vtk_reader
  {
  protected:
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

      std::ifstream reader;

      void checkFilename(std::string const & filename) const
      {
          long len = filename.size();
          string ending = filename.substr(len-4,len-1);

          if(ending != ".vtk")
          {
            cerr << "This is a wrong file-ending!" << endl;
            cerr << "Expected: \".vtk\", got: \"" <<  ending << "\"" << endl;
            throw;
          }
      }

      void openVTKFile(std::string const & filename)
      {
          std::ifstream local_reader(filename.c_str());
          if(!local_reader)
          {
            cerr << "Error open file!" << endl;
            throw;
          }
          this->reader = local_reader;
      }

      void checkNextToken(std::string const & expectedToken) const
      {
    	  std::string token;
    	  this->reader >> token;
          if(token != expectedToken) {
            cerr << "Expected \"" << expectedToken << "\", but got \"" << token << "\"" << endl;
            throw;
          }
      }

      long getNumberOfNodes() const
      {
    	  std::string token;

    	  this->reader >> token;

          cout << "Number of Nodes: " << token << ", " << token.substr(0,16) << endl;

          long lastIdx = token.size()-1;

          if(token.substr(0,16) != "NumberOfPoints=\"" || token.substr(lastIdx,lastIdx) != "\"")
          {
            cerr << "Expected \"NumberOfPoints=\"...\"\", but got " << token << endl;
            throw;
          }

          token.replace(0,16,"");
          lastIdx = token.size()-1;
          token.replace(lastIdx,lastIdx,"");

          return atoi(token.c_str());
      }

      long getNumberOfCells() const
      {
    	  std::string token;

    	  this->reader >> token;

          cout << "Number of Cells: " << token << ", " << token.substr(0,15) << endl;

          long lastIdx = token.size() - 1;

          cout << "Test" << endl;

          if(token.substr(0,15) != "NumberOfCells=\"" || token.substr(lastIdx-1,lastIdx) != "\">")
          {
            cerr << "Expected \"NumberOfCells=\"...\"\", but got " << token << endl;
            throw;
          }

          token.replace(0,15,"");
          lastIdx =token.size()-1;
          token.replace(lastIdx-1,lastIdx,"");

          return atoi(token.c_str());
      }

      long getNumberOfComponents() const
      {
    	  std::string token;

    	  this->reader >> token;

          cout << "NumberOfComponents: " << token << ", " << token.substr(0,15) << endl;

          long lastIdx = token.size()-1;

          if(token.substr(0,20) != "NumberOfComponents=\"" || token.substr(lastIdx,lastIdx) != "\"")
          {
            cerr << "Expected \"NumberOfComponents=\"...\"\", but got " << token << endl;
            throw;
          }

          token.replace(0,20,"");
          lastIdx =token.size()-1;
          token.replace(lastIdx,lastIdx,"");

          return atoi(token.c_str());
      }

      void readNodeCoordinates(Segment & segment, long nodeNum, long numberOfComponents) const
      {
    	  Vertex vertex;
          double nodeCoord;

          for(long i = 0; i < nodeNum; i++)
          {
            CoordType vmCoords[DimensionTag::dim];

            cout << "Vertex#" << i << ": ";

            for(long j = 0; j < numberOfComponents; j++)
            {
              this->reader >> nodeCoord;
              vmCoords[j] = nodeCoord;
              cout << nodeCoord << " ";
            }

            cout << endl;

            //insert node into segment:
            vertex.getPoint().setCoordinates(vmCoords);
            vertex.setID(i);
            segment.template add<0>(i, vertex);
          }
      }

      void readNodeIndices(vector<long> & cells) const
      {
    	  long cellNode = 0;
    	  std::string token;
          this->reader >> token;

          while(token != "</DataArray>" && strChecker::myIsNumber(token) )
          {
            cellNode = atoi(token.c_str());
            cells.push_back(cellNode);

            cout << "Vertex#: " << cellNode << endl;
            this->reader >> token;
          }
          //cout << "#: " << cellNode << endl;
      }

      void readOffsets(vector<long> & offsets) const
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

            cout << "Vertex#: " << offset << endl;
            this->reader >> token;
          }
      }

      void readCells(Segment & segment, long cellNum, vector<long> const & cells, vector<long> const & offsets) const
      {
          //***************************************************
          // building up the cells in ViennaMesh
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
            if(i == 0) {
              offsetIdx = 0;
              numVertices = offsets[i];
            } else {
              offsetIdx = offsets[i-1];
              numVertices = offsets[i]-offsets[i-1];
            }

            Vertex *vertices[numVertices];

            cout << "offsetIdx: " << offsetIdx << ", numVertices: " << numVertices << endl;

            //****************************************************
            // read out the node indices form the "cells"-vector
            // and add the cells to the "vertices"-array
            //****************************************************
            for (long j = 0; j < numVertices; j++)
            {
              vertices[j] = segment.getVertexAddress(cells[j + offsetIdx]);
              cout << "j+offsetidx: " << j+offsetIdx << endl;
            }

            cell.setVertices(&(vertices[0]));
            cell.setID(i);
            segment.template add<CellTag::TopoLevel>(i, cell);
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
      try {
    	checkFilename(filename);
        openVTKFile(filename.c_str());

        Segment & segment = *(domain.begin());

        std::cout << "Reading vtk-file " << filename << std::endl;

        long nodeNum = 0;
        long cellNum = 0;
        long numberOfComponents = 0;           
        vector<long> cells;     // cells
        vector<long> offsets;   // .vtk-file stores offsets
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
        cout << "#Nodes: " << nodeNum << endl;

        cellNum = getNumberOfCells();
        cout << "#Cells: " << cellNum << endl;
        
        checkNextToken("<Points>");
        checkNextToken("<DataArray");
        checkNextToken("type=\"Float32\"");

        numberOfComponents = getNumberOfComponents();
        
        cout << "Dimensions: " << numberOfComponents << endl;

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

        cout << "Cells: " << endl;
        
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

        readCells(segment, cellNum, cells);

      } catch (...) {
        std::cerr << "Problems while reading file " << filename << std::endl;
      }
     
       std::cout << "File-Reader finished!" << std::endl;  
       
       return EXIT_SUCCESS;
    } //operator()
  }; //class vtk_reader

  template < typename DomainType > int importVTK(DomainType domain, std::string const & filename)
  {
 	  Vtk_reader<DomainType> vtk_reader;
 	  return vtk_reader.readDomain(domain, filename);
  }


}

#endif
