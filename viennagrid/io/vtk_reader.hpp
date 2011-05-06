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

#include "boost/unordered_map.hpp"

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
    class vtk_reader
    {
    protected:
      typedef typename DomainType::config_type                     DomainConfiguration;

      typedef typename DomainConfiguration::numeric_type                 CoordType;
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

      std::ifstream                        reader;
      boost::unordered_map<CoordType*, bool>        global_geometry_uniquer;
      boost::unordered_map<CoordType*, std::size_t> global_geometry_map;
      std::map<std::size_t, std::size_t>            local_global_geometry_map;
      std::size_t                                   global_geometry_index;
      std::size_t                                   local_geometry_index;

      typedef std::vector<CoordType*>               geometry_type;
      typedef typename geometry_type::iterator      geometry_iterator_type;
      
      geometry_type                                 geometry_container;


      void openFile(std::string const & filename)
      {
          this->reader.open(filename.c_str());
          if(!this->reader)
          {
            std::cerr << "Error open file!" << std::endl;
            throw;
          }
      }
      
      void closeFile()
      {
        reader.close();
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

        //std::cout << "Number of Nodes: " << token << ", " << token.substr(0,16) << std::endl;

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

        //std::cout << "Number of Cells: " << token << ", " << token.substr(0,15) << std::endl;

        long lastIdx = token.size() - 1;

        //std::cout << "Test" << std::endl;

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

        //std::cout << "NumberOfComponents: " << token << ", " << token.substr(0,15) << std::endl;

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

      void readNodeCoordinates(long nodeNum, long numberOfComponents)
      {
        VertexType vertex;
        double nodeCoord;

        for(long i = 0; i < nodeNum; i++)
        {
          CoordType vmCoords[DimensionTag::value];

          //std::cout << "Vertex#" << i << ": ";

          for(long j = 0; j < numberOfComponents; j++)
          {
            this->reader >> nodeCoord;
            vmCoords[j] = nodeCoord;
            //std::cout << nodeCoord << " ";
          }

          //std::cout << std::endl;

          // test if point has not been added sofar ..
          if(global_geometry_uniquer[vmCoords] == false)
          {
            global_geometry_map[vmCoords] = global_geometry_index;
            local_global_geometry_map[local_geometry_index] = global_geometry_index;
            global_geometry_index++;
            local_geometry_index++;
            
            // store the point vector
            // note: maps do not offer a defined linear storage, hence, 
            // we cannot rely on the already defined maps to realize the global
            // geometry storage
            // todo: get rid of this additional geometry container ..
            //
            // global_geometry_index = point vector access index!
            geometry_container.push_back(vmCoords);
            
            // make sure that the point won't be added again ..
            global_geometry_uniquer[vmCoords] = true;
          }
          // if the point has already been added ..
          else
          {
            local_global_geometry_map[local_geometry_index] = global_geometry_map[vmCoords];
            local_geometry_index++;            
          }
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

          //std::cout << "Vertex#: " << cellNode << std::endl;
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

            //std::cout << "Vertex#: " << offset << std::endl;
            this->reader >> token;
          }
      }

      template <typename Segment>
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
          CellType cell;
          long numVertices = 0;
          long offsetIdx = 0;

          segment.reserve_cells(cellNum);

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

            std::vector<VertexType *> vertices(numVertices);

            //std::cout << "offsetIdx: " << offsetIdx << ", numVertices: " << numVertices << std::endl;

            //****************************************************
            // read out the node indices form the "cells"-vector
            // and add the cells to the "vertices"-array
            //****************************************************
            for (long j = 0; j < numVertices; j++)
            {
              vertices[j] = &(segment.vertex(cells[j + offsetIdx]));
              //std::cout << "j+offsetidx: " << j+offsetIdx << std::endl;
            }

            cell.setVertices(&(vertices[0]));
            cell.setID(i);
            segment.add(cell);
          }
      }
     
      void process_vtu(DomainType & domain, std::string const & filename, int segment_id = 0)
      {
        try
        {
          openFile(filename);
          
          // each vtu file represents a segment, hence, 
          // new geometry information, hence, the local (segment related) 
          // geometry index has to be reset
          local_geometry_index = 0;          
          
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
          //std::cout << "#Nodes: " << nodeNum << std::endl;

          cellNum = getNumberOfCells();
          //std::cout << "#Cells: " << cellNum << std::endl;
          
          checkNextToken("<Points>");
          checkNextToken("<DataArray");
          checkNextToken("type=\"Float32\"");

          numberOfComponents = getNumberOfComponents();
          
          //std::cout << "Dimensions: " << numberOfComponents << std::endl;

          checkNextToken("format=\"ascii\">");
          
          // Read in the coordinates of the nodes
          readNodeCoordinates(nodeNum, numberOfComponents);
          
          checkNextToken("</DataArray>");
          checkNextToken("</Points>");
          checkNextToken("<Cells>");
          checkNextToken("<DataArray");
          checkNextToken("type=\"Int32\"");
          checkNextToken("Name=\"connectivity\"");
          checkNextToken("format=\"ascii\">");

          //std::cout << "Cells: " << std::endl;
          
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

          readCells(domain, cellNum, cells, offsets);          
          
          closeFile();          
        }
        catch (...) {
          std::cerr << "Problems while reading file " << filename << std::endl;
        }      
      }

      void process_pvd(DomainType & domain, std::string const & filename)
      {
      
      }


    public:
      vtk_reader() 
      { 
        local_geometry_index = 0;
        global_geometry_index = 0;        
      };
      ~vtk_reader() { };

      int readDomain(DomainType & domain, std::string const & filename)
      {
        return this->operator()(domain, filename);
      }

      int operator()(DomainType & domain, std::string const & filename)
      {
        std::string::size_type pos  = filename.rfind(".")+1;
        std::string extension = filename.substr(pos, filename.size());      
      
        if(extension == "vtu")
        {
          // process a vtu file, load temporary containers with 
          // the input data
          //
          process_vtu(domain, filename);
          
          // transfer the geometry information from the temporary container 
          // to the viennagrid datastructure
          domain.reserve_vertices(geometry_container.size());
          std::size_t index = 0;
          for(geometry_iterator_type it = geometry_container.begin();
              it != geometry_container.end(); it++)
          {
            VertexType vertex;
            for (dim_type i=0; i<vertex.getPoint().size(); ++i)
              vertex.getPoint()[i] = *it[i];
            vertex.setID(index);
            domain.add(vertex);            
            index++;
          }
          
          
        }
        else
        if(extension == "pvd")
        {
          process_pvd(domain, filename);
        }
        else
        {
          std::cerr << "Error: vtk-reader does not support file extension: " << extension << std::endl;          
          std::cerr << "       the vtk-reader supports: " << std::endl;
          std::cerr << "       *.vtu -- single-segment domains" << std::endl;
          std::cerr << "       *.pvd -- multi-segment domains" << std::endl;
          return EXIT_FAILURE;          
        }
        
        return EXIT_SUCCESS;
      } //operator()
    }; //class vtk_reader

    template < typename DomainType > 
    int import_vtk(DomainType & domain, std::string const & filename)
    {
      vtk_reader<DomainType> vtk_reader;
      return vtk_reader.readDomain(domain, filename);
    }

  } //namespace io
} //namespace viennagrid

#endif
