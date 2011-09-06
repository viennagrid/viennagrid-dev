#ifndef VIENNAGRID_IO_VTK_READER_HPP
#define VIENNAGRID_IO_VTK_READER_HPP

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


/** @file vtk_reader.hpp
 *  @brief    This is a simple vtk-reader implementation. Refer to the vtk-standard (cf. http://www.vtk.org/pdf/file-formats.pdf) and make sure the same order of XML tags is preserved.
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
#include "viennagrid/io/xml_tag.hpp"

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
        std::transform(s1.begin(), s1.end(), s1_lower.begin(), char_to_lower);

        std::string s2_lower = s2;
        std::transform(s2.begin(), s2.end(), s2_lower.begin(), char_to_lower);

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
        
        while (to_lower(token) != "</dataarray>")
        {
          container.push_back( atof(token.c_str()) );
          reader >> token;
        }
      }
      
      template <typename ContainerType, typename NameContainerType>
      void readPointCellData(size_t seg_id,
                             ContainerType & scalar_data,
                             ContainerType & vector_data,
                             NameContainerType & data_names_scalar,
                             NameContainerType & data_names_vector)
      {
        std::string name;
        std::size_t components = 1;
        
        xml_tag<> tag;
        
        tag.parse(reader);
        
        while (tag.name() == "dataarray")
        {
          tag.check_attribute("name", "");
          name = tag.get_value("name");

          if (tag.has_attribute("numberofcomponents"))
            components = atoi(tag.get_value("numberofcomponents").c_str());
          
          //now read data:
          if (components == 1)
          {
            data_names_scalar.push_back(std::make_pair(seg_id, name));
            scalar_data[seg_id].push_back( std::make_pair(name, std::deque<double>()) );
            readData(scalar_data[seg_id].back().second);
          }
          else if (components == 3)
          {
            data_names_vector.push_back(std::make_pair(seg_id, name));
            vector_data[seg_id].push_back( std::make_pair(name, std::deque<double>()) );
            readData(vector_data[seg_id].back().second);
          }
          else
            throw bad_file_format_exception("", "Number of components for data invalid!");
          
          tag.parse(reader);
        }


        if (tag.name() != "/pointdata" && tag.name() != "/celldata")
            throw bad_file_format_exception("", "XML Parse error: Expected </PointData> or </CellData>!");
        
      }

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
                      
          xml_tag<> tag;
          
          tag.parse(reader);
          if (tag.name() != "?xml" && tag.name() != "?xml?")
            throw bad_file_format_exception(filename, "Parse error: No opening ?xml tag!");

          tag.parse_and_check_name(reader, "vtkfile", filename);
          tag.parse_and_check_name(reader, "unstructuredgrid", filename);

          tag.parse_and_check_name(reader, "piece", filename);

          tag.check_attribute("numberofpoints", filename);
            
          nodeNum = atoi(tag.get_value("numberofpoints").c_str());
          #ifdef VIENNAGRID_DEBUG_IO
          std::cout << "#Nodes: " << nodeNum << std::endl;
          #endif

          tag.check_attribute("numberofcells", filename);
          
          local_cell_num[seg_id] = atoi(tag.get_value("numberofcells").c_str());
          #ifdef VIENNAGRID_DEBUG_IO
          std::cout << "#Cells: " << local_cell_num[seg_id] << std::endl;
          #endif
          
          tag.parse_and_check_name(reader, "points", filename);
          
          tag.parse_and_check_name(reader, "dataarray", filename);
          tag.check_attribute("numberofcomponents", filename);
          
          numberOfComponents = atoi(tag.get_value("numberofcomponents").c_str());
          readNodeCoordinates(nodeNum, numberOfComponents, seg_id);
          
          tag.parse_and_check_name(reader, "/dataarray", filename);
          tag.parse_and_check_name(reader, "/points", filename);

          tag.parse(reader);
          if (tag.name() == "pointdata")
          {
            readPointCellData(seg_id, local_scalar_vertex_data, local_vector_vertex_data,
                                      vertex_data_scalar_read, vertex_data_vector_read);
            tag.parse(reader);
          }
          
          if (tag.name() != "cells")
            throw bad_file_format_exception(filename, "Parse error: Expected Cells tag!");
          
          for (std::size_t i=0; i<3; ++i)
          {
            tag.parse_and_check_name(reader, "dataarray", filename);
            tag.check_attribute("name", filename);
            
            if (tag.get_value("name") == "connectivity")
              readCellIndices(seg_id);
            else if (tag.get_value("name") == "offsets")
              readOffsets(seg_id);
            else if (tag.get_value("name") == "types")
              readTypes();
            else
              throw bad_file_format_exception(filename, "Parse error: <DataArray> is not named 'connectivity', 'offsets' or 'types'!");
          }

          tag.parse_and_check_name(reader, "/cells", filename);
          
          tag.parse(reader);
          if (tag.name() == "celldata")
          {
            readPointCellData(seg_id, local_scalar_cell_data, local_vector_cell_data,
                                      cell_data_scalar_read, cell_data_vector_read);
            tag.parse(reader);
          }
          
          
          if (tag.name() != "/piece")
            throw bad_file_format_exception(filename, "Parse error: Expected </Piece> tag!");
          
          tag.parse_and_check_name(reader, "/unstructuredgrid", filename);
          tag.parse_and_check_name(reader, "/vtkfile", filename);

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
        xml_tag<> tag;
        
        tag.parse(reader);
        if (tag.name() != "?xml" && tag.name() != "?xml?")
          throw bad_file_format_exception(filename, "Parse error: No opening <?xml?> tag!");

        tag.parse(reader);
        if (tag.name() != "vtkfile")
          throw bad_file_format_exception(filename, "Parse error: VTKFile tag expected!");
        
        if (!tag.has_attribute("type"))
          throw bad_file_format_exception(filename, "Parse error: VTKFile tag has no attribute 'type'!");
        
        if (to_lower(tag.get_value("type")) != "collection")
          throw bad_file_format_exception(filename, "Parse error: Type-attribute of VTKFile tag is not 'Collection'!");
        
        //checkNextToken("<Collection>");
        tag.parse(reader);
        if (tag.name() != "collection")
          throw bad_file_format_exception(filename, "Parse error: Collection tag expected!");
        
        long seg_index = 0;
        while (reader.good())
        {
          tag.parse(reader);
          
          if (tag.name() == "/collection")
            break;
          
          if (tag.name() != "dataset")
            throw bad_file_format_exception(filename, "Parse error: DataSet tag expected!");
          
          if (tag.has_attribute("part"))
          {
            if (atoi(tag.get_value("part").c_str()) != seg_index)
              throw bad_file_format_exception(filename, "Parser limitation: Segments must be provided with increasing index!");
          }
          
          if (!tag.has_attribute("file"))
            throw bad_file_format_exception(filename, "Parse error: DataSet tag has no file attribute!");
          
          filenames.push_back(tag.get_value("file"));
          
          ++seg_index;
        }

        tag.parse(reader);
        if (tag.name() != "/vtkfile")
          throw bad_file_format_exception(filename, "Parse error: Closing VTKFile tag expected!");

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

        // Extract data read from file:
        std::vector<std::pair<std::size_t, std::string> > const & get_scalar_data_on_vertices() const { return vertex_data_scalar_read; }
        std::vector<std::pair<std::size_t, std::string> > const & get_vector_data_on_vertices() const { return vertex_data_vector_read; }

        std::vector<std::pair<std::size_t, std::string> > const & get_scalar_data_on_cells() const { return cell_data_scalar_read; }
        std::vector<std::pair<std::size_t, std::string> > const & get_vector_data_on_cells() const { return cell_data_vector_read; }

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
        
        // Quantities read:
        std::vector<std::pair<std::size_t, std::string> >         vertex_data_scalar_read;
        std::vector<std::pair<std::size_t, std::string> >         vertex_data_vector_read;

        std::vector<std::pair<std::size_t, std::string> >         cell_data_scalar_read;
        std::vector<std::pair<std::size_t, std::string> >         cell_data_vector_read;
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
