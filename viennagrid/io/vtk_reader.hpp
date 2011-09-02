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


#ifndef VIENNAGRID_IO_VTK_READER_HPP
#define VIENNAGRID_IO_VTK_READER_HPP

/** @file vtk_reader.hpp
 *  @brief    This is a very simple vtk-reader implementation. There's only a part of the vtk-standard implemented.
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>

#include "viennagrid/forwards.h"
#include "viennagrid/point.hpp"
#include "viennagrid/io/vtk_common.hpp"
#include "viennagrid/io/data_accessor.hpp"
#include "viennagrid/io/helper.hpp"

namespace viennagrid
{
  namespace io
  {
    
    namespace
    {
      //functor for conversion to lowercase (avoid ::tolower())
      char to_lower(char c)
      {
        if(c <= 'Z' && c >= 'A')
          return c - ('Z'-'z');
        return c;
      } 
      
    }
    
    
    template <typename DomainType>
    class vtk_reader
    {
    protected:
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

      std::ifstream                                 reader;
      std::map<PointType, std::size_t, point_less>         global_points;
      std::map<std::size_t, PointType>                     global_points_2;
      std::deque<std::deque<std::size_t> >                 local_to_global_map;
      std::deque<std::deque<std::size_t> >                 local_cell_vertices;
      std::deque<std::deque<std::size_t> >                 local_cell_offsets;
      std::deque<std::size_t>                              local_cell_num;
      
      //data containers:
      std::deque<std::deque<std::pair<std::string, std::deque<double> > > >  local_scalar_vertex_data;
      std::deque<std::deque<std::pair<std::string, std::deque<double> > > >  local_vector_vertex_data;
      std::deque<std::deque<std::pair<std::string, std::deque<double> > > >  local_scalar_cell_data;
      std::deque<std::deque<std::pair<std::string, std::deque<double> > > >  local_vector_cell_data;
      
      void openFile(std::string const & filename)
      {
        reader.open(filename.c_str());
        if(!reader)
        {
          throw cannot_open_file_exception(filename);
        }
      }
      
      void closeFile()
      {
        reader.close();
      }
     
      bool lowercase_compare(std::string const & s1, std::string const & s2)
      {
        std::string s1_lower = s1;
        std::transform(s1.begin(), s1.end(), s1_lower.begin(), to_lower);

        std::string s2_lower = s2;
        std::transform(s2.begin(), s2.end(), s2_lower.begin(), to_lower);

        return s1_lower == s2_lower;
      }
     
      void checkNextToken(std::string const & expectedToken)
      {
        std::string token;
        reader >> token;

        if ( !lowercase_compare(token, expectedToken) )
        {
          std::cerr << "* vtk_reader::operator(): Expected \"" << expectedToken << "\", but got \"" << token << "\"" << std::endl;
          std::stringstream ss;
          ss << "Expected \"" << expectedToken << "\", but got \"" << token << "\"";
          throw bad_file_format_exception("", ss.str());
        }
      }

      void goToToken(std::string const & expectedToken)
      {

        std::string expectedTokenLower = expectedToken;
        std::transform(expectedTokenLower.begin(), expectedTokenLower.end(), expectedTokenLower.begin(), to_lower);
        
        bool done = false;
        
        while (!done && reader.good())
        {
          //read token and convert to lower-case
          std::string token;
          reader >> token;
          std::string tokenLower = token;
          std::transform(tokenLower.begin(), tokenLower.end(), tokenLower.begin(), to_lower);
          
          if(tokenLower == expectedTokenLower) 
            done = true;
        }
        
        if (!reader.good())
        {
          std::stringstream ss;
          ss << "* vtk_reader::operator(): Reached EOF while waiting for \"" << expectedToken << "\"";
          std::cerr << ss.str() << std::endl;
          throw bad_file_format_exception("", ss.str());
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
          throw bad_file_format_exception("", "Expected \"NumberOfPoints='...'\"");
        }

        token.replace(0,16,"");
        lastIdx = token.size()-1;
        token.replace(lastIdx,lastIdx,"");

        return atoi(token.c_str());
      }

      long getNumberOfCells()
      {
        std::string token;

        reader >> token;

        //std::cout << "Number of Cells: " << token << ", " << token.substr(0,15) << std::endl;

        long lastIdx = token.size() - 1;

        //std::cout << "Test" << std::endl;

        if(token.substr(0,15) != "NumberOfCells=\"" || token.substr(lastIdx-1,lastIdx) != "\">")
        {
          std::cerr << "Expected \"NumberOfCells=\"...\"\", but got " << token << std::endl;
          throw bad_file_format_exception("", "Expected \"NumberOfCells='...'\"");
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

      void readNodeCoordinates(long nodeNum, long numberOfComponents, std::size_t seg_id)
      {
        double nodeCoord;
        local_to_global_map[seg_id].resize(nodeNum);

        for(long i = 0; i < nodeNum; i++)
        {
          PointType p;

          for(long j = 0; j < numberOfComponents; j++)
          {
            reader >> nodeCoord;
            if (j < DimensionTag::value)
              p[j] = nodeCoord;
            //std::cout << nodeCoord << " ";
          }
          
          //add point to global list if not already there
          if (global_points.find(p) == global_points.end())
          {
            std::size_t new_global_id = global_points.size();
            //std::cout << "new_global_id: " << new_global_id << std::endl;
            global_points.insert( std::make_pair(p, new_global_id) );
            global_points_2.insert( std::make_pair(new_global_id, p) );
            local_to_global_map[seg_id][i] = new_global_id;
          }
          else
          {
            local_to_global_map[seg_id][i] = global_points[p];
            //std::cout << "Found vertex: " << p << "with ID " << local_to_global_map[seg_id][i] << std::endl;
          }
        }
      }

      //read the vertex indices of the cells
      void readCellIndices(std::size_t seg_id)
      {
        long cellNode = 0;
        std::string token;
        reader >> token;

        while (token != "</DataArray>")
        {
          assert( strChecker::myIsNumber(token) && "Cell vertex index is not a number!" );
          
          cellNode = atoi(token.c_str());
          local_cell_vertices[seg_id].push_back(cellNode);

          reader >> token;
        }
      }

      void readOffsets(std::size_t seg_id)
      {
          //****************************************************************************
          // read in the offsets: describe the affiliation of the nodes to the cells
          // (see: http://www.vtk.org/pdf/file-formats.pdf , page 9)
          //****************************************************************************

          std::string token;
          long offset = 0;
          reader >> token;

          while(token != "</DataArray>")
          {
            assert( strChecker::myIsNumber(token) && "Cell offset is not a number!" );
            
            offset = atoi(token.c_str());
            local_cell_offsets[seg_id].push_back(offset);

            //std::cout << "Vertex#: " << offset << std::endl;
            reader >> token;
          }
      }

      void readTypes()
      {
          //****************************************************************************
          // read in the offsets: describe the affiliation of the nodes to the cells
          // (see: http://www.vtk.org/pdf/file-formats.pdf , page 9)
          //****************************************************************************

          std::string token;
          long type = 0;
          reader >> token;

          while(token != "</DataArray>")
          {
            assert( strChecker::myIsNumber(token) && "Cell type is not a number!" );
            
            type = atoi(token.c_str());
            assert(type == ELEMENT_TAG_TO_VTK_TYPE<CellTag>::value && "Error in VTK reader: Type mismatch!");

            //std::cout << "Vertex#: " << offset << std::endl;
            reader >> token;
          }
      }
      
      
      void readData(std::deque<double> & container)
      {
        std::string token;
        reader >> token;
        std::transform(token.begin(), token.end(), token.begin(), to_lower);
        
        while (token != "</dataarray>")
        {
          container.push_back( atof(token.c_str()) );
          reader >> token;
          std::transform(token.begin(), token.end(), token.begin(), to_lower);
        }
      }
      
      template <typename ContainerType>
      void readDataArray(size_t seg_id, ContainerType & scalar_data, ContainerType & vector_data)
      {
        checkNextToken("type=\"Float32\"");
        
        std::string name;
        std::size_t components = 1;
        std::string token;
        reader >> token;
        
        if (token.substr(0, 4) == "name" || token.substr(0, 4) == "Name")
        {
          name = token.substr(6, token.size()-7);
        }
        else
        {
          throw bad_file_format_exception("", "Name of quantity in <DataArray> tag expected!");
        }

        reader >> token;
        if (token.substr(0, 18) == "NumberOfComponents")
        {
          components = atoi(token.substr(20, token.size()-21).c_str());
          
          checkNextToken("format=\"ascii\">");
        }
        else if (token.substr(token.size()-1, 1) == ">")
        {
          //OK, continue below
        }
        else
        {
          throw bad_file_format_exception("", "Name of quantity in <DataArray> tag expected!");
        }
        
        //now read data:
        if (components == 1)
        {
          scalar_data[seg_id].push_back( std::make_pair(name, std::deque<double>()) );
          readData(scalar_data[seg_id].back().second);
        }
        else if (components == 3)
        {
          vector_data[seg_id].push_back( std::make_pair(name, std::deque<double>()) );
          readData(vector_data[seg_id].back().second);
        }
        else
        {
          throw bad_file_format_exception("", "Number of components for data invalid!");
        }
      }

      void readPointData(size_t seg_id)
      {
        std::string token;
        reader >> token;
        
        //go to end of XML tag:
        bool tag_still_open = true;
        while (tag_still_open)
        {
          if (token[token.size()-1] == '>')
            tag_still_open = false;
          reader >> token;
        }
        
        while (token != "</PointData>" && token != "</pointdata>")
        {
          readDataArray(seg_id, local_scalar_vertex_data, local_vector_vertex_data);
          reader >> token;
        }
      }
      
      void readCellData(size_t seg_id)
      {
        std::string token;
        reader >> token;
        
        //go to end of XML tag:
        bool tag_still_open = true;
        while (tag_still_open)
        {
          if (token[token.size()-1] == '>')
            tag_still_open = false;
          reader >> token;
        }
        
        while (token != "</CellData>" && token != "</celldata>")
        {
          readDataArray(seg_id, local_scalar_cell_data, local_vector_cell_data);
          reader >> token;
        }
      }
     
      void readSegments(std::deque<std::string> & filenames)
      {
        std::string token;
        reader >> token;
        std::size_t seg_id = 0;

        while(token != "</Collection>")
        {
          std::string tokenLower = token;
          std::transform(token.begin(), token.end(), tokenLower.begin(), to_lower);

          if (tokenLower.substr(0, 6) == "part=\"")
          {
            if (token.at(7) == '"')
            {
              assert( token[6] >= '0' && token[6] <= '9' && "Invalid segment ID specified!");
              seg_id = token[6] - '0';
            }
            else if (token.at(8) == '"')
            {
              std::string seg_id_string = token.substr(6,2);
              seg_id = atoi(seg_id_string.c_str());
            }
            else
            {
              throw bad_file_format_exception("", "Number of segments is above 99. The parser is not able to handle this!");
            }
          }
          
          if (tokenLower.substr(0, 6) == "file=\"")
          {
            if (seg_id != filenames.size())
              throw bad_file_format_exception("", "Number of segments in .pvd file is not in sequence, starting from 0!");
              
            filenames.push_back(token.substr(6, token.size()-7));
          }
  
          if (!reader)
            break;
          
          reader >> token;
        } //while
        
      } //readSegments
      
      
      /////////////////////////// Routines for pushing everything to domain ///////////////

      void setupVertices(DomainType & domain)
      {
        for (std::size_t i=0; i<global_points_2.size(); ++i)
        {
          VertexType v;
          v.point() = global_points_2[i];
          domain.push_back(v);
        }
      }
      
      
      void setupCells(DomainType & domain, std::size_t seg_id)
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
          
          std::deque<std::size_t> const & offsets = local_cell_offsets[seg_id];

          for (std::size_t i = 0; i < local_cell_num[seg_id]; i++)
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
              std::size_t local_index = local_cell_vertices[seg_id][j + offsetIdx];
              std::size_t global_vertex_index = local_to_global_map[seg_id][local_index];
              vertices[j] = &(viennagrid::ncells<0>(domain)[global_vertex_index]);
              //std::cout << "j+offsetidx: " << j+offsetIdx << std::endl;
            }

            cell.vertices(&(vertices[0]));
            cell.id(i);
            domain.segments()[seg_id].push_back(cell);
          }
      }
      
      template <typename ContainerType>
      void setupDataVertex(DomainType & domain, std::size_t seg_id, ContainerType const & container, std::size_t num_components)
      {
        //
        // Step 1: Try to find data name in list of registered quantity names:
        //
        bool name_registered = false;
        std::size_t data_accessor_index = 0;
        if (num_components == 1)
        {
          for (std::size_t i=0; i<vertex_data_scalar_names.size(); ++i)
          {
            if (container.first == vertex_data_scalar_names[i])
            {
              name_registered = true;
              data_accessor_index = i;
              #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
              std::cout << "* vtk_reader::operator(): Found registered scalar name " << container.first << std::endl;
              #endif
              break;
            }
          }
        }
        else
        {
          for (std::size_t i=0; i<vertex_data_vector_names.size(); ++i)
          {
            if (container.first == vertex_data_vector_names[i])
            {
              name_registered = true;
              data_accessor_index = i;
              #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
              std::cout << "* vtk_reader::operator(): Found registered vertex name " << container.first << std::endl;
              #endif
              break;
            }
          }
        }
        
        //
        // Step 2: Write to domain
        //
        if (name_registered)
        {
          if (num_components == 1)
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading scalar quantity " 
                      << container.first << " using data_accessor to vertices." << std::endl;
            #endif
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::ncells<0>(domain)[global_vertex_id];
              vertex_data_scalar[data_accessor_index](vertex, seg_id, 0, (container.second)[i]);
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading vector quantity "
                      << container.first << " using data_accessor to vertices." << std::endl;
            #endif
            assert( 3 * viennagrid::ncells<0>(domain.segments()[seg_id]).size() == container.second.size() );
            for (std::size_t i=0; i<container.second.size() / 3; ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::ncells<0>(domain)[global_vertex_id];
              vertex_data_vector[data_accessor_index](vertex, seg_id, 0, (container.second)[3*i]);
              vertex_data_vector[data_accessor_index](vertex, seg_id, 1, (container.second)[3*i+1]);
              vertex_data_vector[data_accessor_index](vertex, seg_id, 2, (container.second)[3*i+2]);
            }
          }
        }
        else
        {
          if (num_components == 1)
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading scalar quantity " 
                      << container.first << " using viennadata to vertices." << std::endl;
            #endif
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::ncells<0>(domain)[global_vertex_id];
              viennadata::access<std::string, double>(container.first)(vertex) = (container.second)[i];
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading vector quantity " 
                      << container.first << " using viennadata to vertices." << std::endl;
            #endif
            assert( 3 * viennagrid::ncells<0>(domain.segments()[seg_id]).size() == container.second.size());
            for (std::size_t i=0; i<container.second.size() / 3; ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::ncells<0>(domain)[global_vertex_id];
              viennadata::access<std::string, std::vector<double> >(container.first)(vertex).resize(3);
              viennadata::access<std::string,
                                 std::vector<double> >(container.first)(vertex)[0] = (container.second)[3*i];
              viennadata::access<std::string,
                                 std::vector<double> >(container.first)(vertex)[1] = (container.second)[3*i+1];
              viennadata::access<std::string,
                                 std::vector<double> >(container.first)(vertex)[2] = (container.second)[3*i+2];
            }
          }
        }
          
      }

      template <typename ContainerType>
      void setupDataCell(DomainType & domain, std::size_t seg_id, ContainerType const & container, std::size_t num_components)
      {
        //
        // Step 1: Try to find data name in list of registered quantity names:
        //
        bool name_registered = false;
        std::size_t data_accessor_index = 0;
        if (num_components == 1)
        {
          for (std::size_t i=0; i<cell_data_scalar_names.size(); ++i)
          {
            if (container.first == cell_data_scalar_names[i])
            {
              name_registered = true;
              data_accessor_index = i;
              #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
              std::cout << "* vtk_reader::operator(): Found registered cell scalar name " << container.first << std::endl;
              #endif
              break;
            }
          }
        }
        else
        {
          for (std::size_t i=0; i<cell_data_vector_names.size(); ++i)
          {
            if (container.first == cell_data_vector_names[i])
            {
              name_registered = true;
              data_accessor_index = i;
              #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
              std::cout << "* vtk_reader::operator(): Found registered cell vector name " << container.first << std::endl;
              #endif
              break;
            }
          }
        }
        
        //
        // Step 2: Write to domain
        //
        if (name_registered)
        {
          if (num_components == 1)
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading scalar quantity " 
                      << container.first << " using data_accessor to cells." << std::endl;
            #endif
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = viennagrid::ncells<DimensionTag::value>(domain.segments()[seg_id])[i];
              cell_data_scalar[data_accessor_index](cell, seg_id, 0, (container.second)[i]);
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading vector quantity " 
                      << container.first << " using data_accessor to cells." << std::endl;
            #endif
            assert( 3 * viennagrid::ncells<DimensionTag::value>(domain.segments()[seg_id]).size() == container.second.size());
            for (std::size_t i=0; i<container.second.size() / 3; ++i)
            {
              CellType const & cell = viennagrid::ncells<DimensionTag::value>(domain.segments()[seg_id])[i];
              cell_data_vector[data_accessor_index](cell, seg_id, 0, (container.second)[3*i]);
              cell_data_vector[data_accessor_index](cell, seg_id, 1, (container.second)[3*i+1]);
              cell_data_vector[data_accessor_index](cell, seg_id, 2, (container.second)[3*i+2]);
            }
          }
        }
        else
        {
          if (num_components == 1)
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading scalar quantity "
                      << container.first << " using viennadata to cells." << std::endl;
            #endif
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = viennagrid::ncells<DimensionTag::value>(domain.segments()[seg_id])[i];
              viennadata::access<std::string,
                                 double>(container.first)(cell) = (container.second)[i];
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading vector quantity "
                      << container.first << " using viennadata to cells." << std::endl;
            #endif
            assert( 3 * viennagrid::ncells<DimensionTag::value>(domain.segments()[seg_id]).size() == container.second.size());
            for (std::size_t i=0; i<container.second.size() / 3; ++i)
            {
              CellType const & cell = viennagrid::ncells<DimensionTag::value>(domain.segments()[seg_id])[i];
              viennadata::access<std::string, std::vector<double> >(container.first)(cell).resize(3);
              viennadata::access<std::string,
                                 std::vector<double> >(container.first)(cell)[0] = (container.second)[3*i];
              viennadata::access<std::string,
                                 std::vector<double> >(container.first)(cell)[1] = (container.second)[3*i+1];
              viennadata::access<std::string,
                                 std::vector<double> >(container.first)(cell)[2] = (container.second)[3*i+2];
            }
          }
        }
          
      }

      void setupData(DomainType & domain, std::size_t seg_id)
      {
        for (size_t i=0; i<local_scalar_vertex_data[seg_id].size(); ++i)
        {
          setupDataVertex(domain, seg_id, local_scalar_vertex_data[seg_id][i], 1);
        }

        for (size_t i=0; i<local_vector_vertex_data[seg_id].size(); ++i)
        {
          setupDataVertex(domain, seg_id, local_vector_vertex_data[seg_id][i], 3);
        }


        for (size_t i=0; i<local_scalar_cell_data[seg_id].size(); ++i)
        {
          setupDataCell(domain, seg_id, local_scalar_cell_data[seg_id][i], 1);
        }

        for (size_t i=0; i<local_vector_cell_data[seg_id].size(); ++i)
        {
          setupDataCell(domain, seg_id, local_vector_cell_data[seg_id][i], 3);
        }

      }
     
      void parse_vtu_segment(std::string filename, std::size_t seg_id)
      {

        try
        {
          openFile(filename);
          
          long nodeNum = 0;
          long numberOfComponents = 0;           
                      
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

          local_cell_num[seg_id] = getNumberOfCells();
          //std::cout << "#Cells: " << cellNum << std::endl;
          
          checkNextToken("<Points>");
          checkNextToken("<DataArray");
          checkNextToken("type=\"Float32\"");

          numberOfComponents = getNumberOfComponents();
          
          //std::cout << "Dimensions: " << numberOfComponents << std::endl;

          checkNextToken("format=\"ascii\">");
          
          // Read in the coordinates of the nodes
          readNodeCoordinates(nodeNum, numberOfComponents, seg_id);
          
          checkNextToken("</DataArray>");
          checkNextToken("</Points>");

          //read point data if available
          {
            std::string token;
            reader >> token;
            if (lowercase_compare(token, "<PointData"))
            {
              //std::cout << "Reading point data..." << std::endl;
              readPointData(seg_id);
              checkNextToken("<Cells>");
            }
            else if (lowercase_compare(token, "<Cells>"))
            {
              //okay, continue below
            }
            else
            {
              std::cerr << "* vtk_reader::operator(): Expected \"<PointData\" or \"<Cells>\", but got \"" << token << "\"" << std::endl;
              throw;
            }
          }
          
          checkNextToken("<DataArray");
          checkNextToken("type=\"Int32\"");
          checkNextToken("Name=\"connectivity\"");
          checkNextToken("format=\"ascii\">");
          readCellIndices(seg_id);

          checkNextToken("<DataArray");
          checkNextToken("type=\"Int32\"");
          checkNextToken("Name=\"offsets\"");
          checkNextToken("format=\"ascii\">");
          readOffsets(seg_id);
          
          
          checkNextToken("<DataArray");
          checkNextToken("type=\"UInt8\"");
          checkNextToken("Name=\"types\"");
          checkNextToken("format=\"ascii\">");
          readTypes();

          checkNextToken("</Cells>");
          
          //read cell data if available
          {
            std::string token;
            reader >> token;
            if (lowercase_compare(token, "<CellData"))
            {
              //std::cout << "Reading cell data..." << std::endl;
              readCellData(seg_id);
              checkNextToken("</Piece>");
            }
            else if (lowercase_compare(token, "</Piece>"))
            {
              //okay, continue below
            }
            else
            {
              std::cerr << "* vtk_reader::operator(): Expected \"<CellData\" or \"</Piece>\", but got \"" << token << "\"" << std::endl;
              throw;
            }
          }
          
          checkNextToken("</UnstructuredGrid>");
          checkNextToken("</VTKFile>");

          closeFile();          
        }
        catch (std::exception const & ex) {
          std::cerr << "Problems while reading file " << filename << std::endl;
          std::cerr << "what(): " << ex.what() << std::endl;
        }      
        
      }
     
      void process_vtu(std::string const & filename)
      {
        local_cell_num.resize(1);
        local_cell_offsets.resize(1);
        local_cell_vertices.resize(1);
        local_to_global_map.resize(1);
        
        local_scalar_vertex_data.resize(1);
        local_scalar_cell_data.resize(1);
        local_vector_vertex_data.resize(1);
        local_vector_cell_data.resize(1);
        
        parse_vtu_segment(filename, 0);        
      }

      void process_pvd(std::string const & filename)
      {
        std::deque<std::string> filenames;
        
        //extract path from .pvd file:
        std::string::size_type pos = filename.rfind("/");
        std::string path_to_pvd;
        if (pos == std::string::npos)
          pos = filename.rfind("\\"); //a tribute to Windows... ;-)
        
        if (pos != std::string::npos)
          path_to_pvd = filename.substr(0, pos + 1);
        
        openFile(filename);
        
        //
        // Step 1: Get segments from pvd file:
        //
        checkNextToken("<?xml");
        checkNextToken("version=\"1.0\"?>");
        checkNextToken("<VTKFile");
        checkNextToken("type=\"Collection\"");
        checkNextToken("version=\"0.1\"");
        checkNextToken("byte_order=\"LittleEndian\"");
        checkNextToken("compressor=\"vtkZLibDataCompressor\">");
        checkNextToken("<Collection>");
        
        readSegments(filenames);
        
        checkNextToken("</VTKFile>");
        
        closeFile();
        
        assert(filenames.size() > 0 && "No segments in pvd-file specified!");
        
        //
        // Step 2: Prepare reader for parsing .vtu files:
        //
        local_cell_num.resize(filenames.size());
        local_cell_offsets.resize(filenames.size());
        local_cell_vertices.resize(filenames.size());
        local_to_global_map.resize(filenames.size());
        
        local_scalar_vertex_data.resize(filenames.size());
        local_scalar_cell_data.resize(filenames.size());
        local_vector_vertex_data.resize(filenames.size());
        local_vector_cell_data.resize(filenames.size());
        
        //
        // Step 2: Parse .vtu files:
        //
        for (std::size_t i=0; i<filenames.size(); ++i)
        {
          #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
          std::cout << "Parsing file " << path_to_pvd + filenames[i] << std::endl;
          #endif
          parse_vtu_segment(path_to_pvd + filenames[i], i);
        }
        
      }


    public:

      int operator()(DomainType & domain, std::string const & filename)
      {
        std::string::size_type pos  = filename.rfind(".")+1;
        std::string extension = filename.substr(pos, filename.size());      
      
        if(extension == "vtu")
        {
          process_vtu(filename);
        }
        else if(extension == "pvd")
        {
          process_pvd(filename);
        }
        else
        {
          std::cerr << "Error: vtk-reader does not support file extension: " << extension << std::endl;          
          std::cerr << "       the vtk-reader supports: " << std::endl;
          std::cerr << "       *.vtu -- single-segment domains" << std::endl;
          std::cerr << "       *.pvd -- multi-segment domains" << std::endl;
          return EXIT_FAILURE;          
        }

        //
        // push everything to the ViennaGrid domain:
        // 
        setupVertices(domain);
        domain.segments().resize(local_cell_num.size());
        for (size_t seg_id = 0; seg_id < local_cell_num.size(); ++seg_id)
        {
          setupCells(domain, seg_id);
          setupData(domain, seg_id);
        }

        return EXIT_SUCCESS;
      } //operator()
      
      
      std::vector<std::string> scalar_vertex_data_names(std::size_t segment_id) const 
      {
        std::vector<std::string> ret;
        for (std::size_t i=0; i<local_scalar_vertex_data[segment_id].size(); ++i)
          ret.push_back(local_scalar_vertex_data[segment_id][i].first);
        
        return ret;
      }

      std::vector<std::string> vector_vertex_data_names(std::size_t segment_id) const 
      {
        std::vector<std::string> ret;
        for (std::size_t i=0; i<local_vector_vertex_data[segment_id].size(); ++i)
          ret.push_back(local_vector_vertex_data[segment_id][i].first);
        
        return ret;
      }

      std::vector<std::string> scalar_cell_data_names(std::size_t segment_id) const 
      {
        std::vector<std::string> ret;
        for (std::size_t i=0; i<local_scalar_cell_data[segment_id].size(); ++i)
          ret.push_back(local_scalar_cell_data[segment_id][i].first);
        
        return ret;
      }

      std::vector<std::string> vector_cell_data_names(std::size_t segment_id) const 
      {
        std::vector<std::string> ret;
        for (std::size_t i=0; i<local_vector_cell_data[segment_id].size(); ++i)
          ret.push_back(local_vector_cell_data[segment_id][i].first);
        
        return ret;
      }

      ///////////////////// data handling ///////////////////////

        //
        // Add scalar data for vertices:
        //
        template <typename T>
        void add_scalar_data_on_vertices(T const & accessor, std::string name)
        {
          data_accessor_wrapper<VertexType> wrapper(accessor.clone());
          vertex_data_scalar.push_back(wrapper);
          vertex_data_scalar_names.push_back(name);
        }

        //
        // Add vector data for verteix:
        //

        template <typename T>
        void add_vector_data_on_vertices(T const & accessor, std::string name)
        {
          data_accessor_wrapper<VertexType> wrapper(accessor.clone());
          vertex_data_vector.push_back(wrapper);
          vertex_data_vector_names.push_back(name);
        }

        //
        // Add point data normals:
        //
        template <typename T>
        void add_normal_data_on_vertices(T const & accessor, std::string name)
        {
          data_accessor_wrapper<VertexType> wrapper(accessor.clone());
          vertex_data_normal.push_back(wrapper);
          vertex_data_normal_names.push_back(name);
        }


        //
        // Add scalar cell data:
        //
        template <typename T>
        void add_scalar_data_on_cells(T const & accessor, std::string name)
        {
          data_accessor_wrapper<CellType> wrapper(accessor.clone());
          cell_data_scalar.push_back(wrapper);
          cell_data_scalar_names.push_back(name);
        }

        //
        // Add vector cell data:
        //
        template <typename T>
        void add_vector_data_on_cells(T const & accessor, std::string name)
        {
          data_accessor_wrapper<CellType> wrapper(accessor.clone());
          cell_data_vector.push_back(wrapper);
          cell_data_vector_names.push_back(name);
        }


        //
        // Add cell data normals:
        //
        template <typename T>
        void add_normal_data_on_cells(T const & accessor, std::string name)
        {
          data_accessor_wrapper<CellType> wrapper(accessor.clone());
          cell_data_normal.push_back(wrapper);
          cell_data_normal_names.push_back(name);
        }

      private:
        std::vector< data_accessor_wrapper<VertexType> >    vertex_data_scalar;
        std::vector< std::string >                          vertex_data_scalar_names;
        
        std::vector< data_accessor_wrapper<VertexType> >    vertex_data_vector;
        std::vector< std::string >                          vertex_data_vector_names;
        
        std::vector< data_accessor_wrapper<VertexType> >    vertex_data_normal;
        std::vector< std::string >                          vertex_data_normal_names;
        
        std::vector< data_accessor_wrapper<CellType> >      cell_data_scalar;
        std::vector< std::string >                          cell_data_scalar_names;
        
        std::vector< data_accessor_wrapper<CellType> >      cell_data_vector;
        std::vector< std::string >                          cell_data_vector_names;
        
        std::vector< data_accessor_wrapper<CellType> >      cell_data_normal;
        std::vector< std::string >                          cell_data_normal_names;
        
      
    }; //class vtk_reader

    template < typename DomainType > 
    int import_vtk(DomainType & domain, std::string const & filename)
    {
      vtk_reader<DomainType> vtk_reader;
      return vtk_reader(domain, filename);
    }





    //
    // Convenience functions for adding vertex-based data
    //    
    
    // scalar data
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_scalar_data_on_vertices(vtk_reader<DomainType> & reader,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new global_scalar_data_accessor<VertexType, KeyType, DataType>(key));
      reader.add_scalar_data_on_vertices(wrapper, quantity_name);
      return reader;
    }

    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_scalar_data_on_vertices_per_segment(vtk_reader<DomainType> & reader,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new segment_scalar_data_accessor<VertexType, KeyType, DataType>(key));
      reader.add_scalar_data_on_vertices(wrapper, quantity_name);
      return reader;
    }

    // vector data
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_vector_data_on_vertices(vtk_reader<DomainType> & reader,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new global_vector_data_accessor<VertexType, KeyType, DataType>(key));
      reader.add_vector_data_on_vertices(wrapper, quantity_name);
      return reader;
    }

    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_vector_data_on_vertices_per_segment(vtk_reader<DomainType> & reader,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new segment_vector_data_accessor<VertexType, KeyType, DataType>(key));
      reader.add_vector_data_on_vertices(wrapper, quantity_name);
      return reader;
    }


    // normal data
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_normal_data_on_vertices(vtk_reader<DomainType> & reader,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new global_vector_data_accessor<VertexType, KeyType, DataType>(key));
      reader.add_vector_data_on_vertices(wrapper, quantity_name);
      return reader;
    }

    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_normal_data_on_vertices_per_segment(vtk_reader<DomainType> & reader,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new segment_vector_data_accessor<VertexType, KeyType, DataType>(key));
      reader.add_vector_data_on_vertices(wrapper, quantity_name);
      return reader;
    }

    //
    // Convenience functions for adding cell-based data
    //
    
    // scalar data
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_scalar_data_on_cells(vtk_reader<DomainType> & reader,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::topology_level>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_scalar_data_accessor<CellType, KeyType, DataType>(key));
      reader.add_scalar_data_on_cells(wrapper, quantity_name);
      return reader;
    }

    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_scalar_data_on_cells_per_segment(vtk_reader<DomainType> & reader,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::topology_level>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new segment_scalar_data_accessor<CellType, KeyType, DataType>(key));
      reader.add_scalar_data_on_cells(wrapper, quantity_name);
      return reader;
    }

    // vector data
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_vector_data_on_cells(vtk_reader<DomainType> & reader,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::topology_level>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_vector_data_accessor<CellType, KeyType, DataType>(key));
      reader.add_vector_data_on_cells(wrapper, quantity_name);
      return reader;
    }

    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_vector_data_on_cells_per_segment(vtk_reader<DomainType> & reader,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::topology_level>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new segment_vector_data_accessor<CellType, KeyType, DataType>(key));
      reader.add_vector_data_on_cells(wrapper, quantity_name);
      return reader;
    }

    // cell data
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_normal_data_on_cells(vtk_reader<DomainType> & reader,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::topology_level>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_vector_data_accessor<CellType, KeyType, DataType>(key));
      reader.add_vector_data_on_cells(wrapper, quantity_name);
      return reader;
    }

    template <typename KeyType, typename DataType, typename DomainType>
    vtk_reader<DomainType> & add_normal_data_on_cells_per_segment(vtk_reader<DomainType> & reader,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::topology_level>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new segment_vector_data_accessor<CellType, KeyType, DataType>(key));
      reader.add_vector_data_on_cells(wrapper, quantity_name);
      return reader;
    }







  } //namespace io
} //namespace viennagrid

#endif
