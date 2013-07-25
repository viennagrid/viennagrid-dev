#ifndef VIENNAGRID_IO_VTK_READER_HPP
#define VIENNAGRID_IO_VTK_READER_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
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

#include "viennagrid/forwards.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/io/vtk_common.hpp"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/xml_tag.hpp"
#include "viennagrid/domain/element_creation.hpp"

namespace viennagrid
{
  namespace io
  {

    /** @brief A VTK reader class that allows to read meshes from XML-based VTK files as defined in http://www.vtk.org/pdf/file-formats.pdf
     *
     * @tparam DomainType   The type of the domain to be read. Must not be a segment type!
     */
    template <typename DomainType, typename SegmentationType = typename viennagrid::result_of::segmentation<DomainType>::type >
    class vtk_reader
    {
    protected:

        typedef typename SegmentationType::segment_type SegmentType;
        typedef typename SegmentationType::segment_id_type segment_id_type;


        typedef typename viennagrid::result_of::point<DomainType>::type PointType;
        typedef typename viennagrid::result_of::coord<PointType>::type CoordType;
        static const int geometric_dim = viennagrid::traits::static_size<PointType>::value;

        typedef typename viennagrid::result_of::cell_tag<DomainType>::type CellTag;


      typedef typename result_of::element<DomainType, viennagrid::vertex_tag>::type                           VertexType;
      typedef typename result_of::handle<DomainType, viennagrid::vertex_tag>::type                           VertexHandleType;
      typedef typename result_of::element<DomainType, CellTag>::type     CellType;
      typedef typename result_of::handle<DomainType, CellTag>::type     CellHandleType;

      typedef typename viennagrid::result_of::element_range<DomainType, viennagrid::vertex_tag>::type   VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type        VertexIterator;

      typedef typename viennagrid::result_of::element_range<DomainType, viennagrid::line_tag>::type   EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type          EdgeIterator;

      typedef typename viennagrid::result_of::element_range<DomainType, typename CellTag::facet_tag>::type   FacetRange;
      typedef typename viennagrid::result_of::iterator<FacetRange>::type                                 FacetIterator;

      typedef typename viennagrid::result_of::element_range<DomainType, CellTag>::type     CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                  CellIterator;


      typedef std::vector<double> vector_data_type;

      typedef std::map< std::string, base_dynamic_accessor_t<double, VertexType> * > VertexScalarOutputAccessorContainer;
      typedef std::map< std::string, base_dynamic_accessor_t<vector_data_type, VertexType> * > VertexVectorOutputAccessorContainer;

      typedef std::map< std::string, base_dynamic_accessor_t<double, CellType> * > CellScalarOutputAccessorContainer;
      typedef std::map< std::string, base_dynamic_accessor_t<vector_data_type, CellType> * > CellVectorOutputAccessorContainer;





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





        template<typename map_type>
        void clear_map( map_type & map )
        {
          for (typename map_type::iterator it = map.begin(); it != map.end(); ++it)
            delete it->second;

          map.clear();
        }

        void clear()
        {
          clear_map(registered_vertex_scalar_data);
          clear_map(registered_vertex_vector_data);

          clear_map(registered_cell_scalar_data);
          clear_map(registered_cell_vector_data);


          for (typename std::map< segment_id_type, VertexScalarOutputAccessorContainer>::iterator it = registered_segment_vertex_scalar_data.begin(); it != registered_segment_vertex_scalar_data.end(); ++it)
            clear_map(it->second);

          for (typename std::map< segment_id_type, VertexVectorOutputAccessorContainer>::iterator it = registered_segment_vertex_vector_data.begin(); it != registered_segment_vertex_vector_data.end(); ++it)
            clear_map(it->second);


          for (typename std::map< segment_id_type, CellScalarOutputAccessorContainer>::iterator it = registered_segment_cell_scalar_data.begin(); it != registered_segment_cell_scalar_data.end(); ++it)
            clear_map(it->second);

          for (typename std::map< segment_id_type, CellVectorOutputAccessorContainer>::iterator it = registered_segment_cell_vector_data.begin(); it != registered_segment_cell_vector_data.end(); ++it)
            clear_map(it->second);


          registered_segment_vertex_scalar_data.clear();
          registered_segment_vertex_vector_data.clear();

          registered_segment_cell_scalar_data.clear();
          registered_segment_cell_vector_data.clear();
        }





      /** @brief Opens a file */
      void openFile(std::string const & filename)
      {
        reader.open(filename.c_str());
        if(!reader)
        {
          throw cannot_open_file_exception(filename);
        }
      }

      /** @brief Closes a file */
      void closeFile()
      {
        reader.close();
      }

     /** @brief compares the lower-case representation of two strings */
      bool lowercase_compare(std::string const & s1, std::string const & s2)
      {
        std::string s1_lower = s1;
        std::transform(s1.begin(), s1.end(), s1_lower.begin(), char_to_lower<>());

        std::string s2_lower = s2;
        std::transform(s2.begin(), s2.end(), s2_lower.begin(), char_to_lower<>());

        return s1_lower == s2_lower;
      }

      /** @brief Make sure that the next token is given by 'expectedToken'. Throws a bad_file_format_exception if this is not the case */
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

      /** @brief Reads the coordinates of the points/vertices in the domain */
      void readNodeCoordinates(long nodeNum, long numberOfComponents, segment_id_type seg_id)
      {
        double nodeCoord;
        local_to_global_map[seg_id].resize(nodeNum);

        for(long i = 0; i < nodeNum; i++)
        {
          PointType p;

          for(long j = 0; j < numberOfComponents; j++)
          {
            reader >> nodeCoord;
            if (j < geometric_dim)
              p[j] = nodeCoord;
          }

          //add point to global list if not already there
          if (global_points.find(p) == global_points.end())
          {
            std::size_t new_global_id = global_points.size();
            global_points.insert( std::make_pair(p, new_global_id) );
            global_points_2.insert( std::make_pair(new_global_id, p) );
            local_to_global_map[seg_id][i] = new_global_id;
          }
          else
          {
            local_to_global_map[seg_id][i] = global_points[p];
          }
        }
      }

      /** @brief Reads the vertex indices of the cells inside the domain */
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

      /** @brief Read the cell offsets for the vertex indices */
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

      /** @brief Read the types of each cell. */
      void readTypes()
      {
          //****************************************************************************
          // read in the offsets: describe the affiliation of the nodes to the cells
          // (see: http://www.vtk.org/pdf/file-formats.pdf , page 9)
          //****************************************************************************

          std::string token;
#ifndef NDEBUG
          long type = 0;
#endif
          reader >> token;

          while(token != "</DataArray>")
          {
            assert( strChecker::myIsNumber(token) && "Cell type is not a number!" );
#ifndef NDEBUG
            type = atoi(token.c_str());
            assert(type == ELEMENT_TAG_TO_VTK_TYPE<CellTag>::value && "Error in VTK reader: Type mismatch!");
#endif
            //std::cout << "Vertex#: " << offset << std::endl;
            reader >> token;
          }
      }

      /** @brief Read data and push it to a container. Helper function. */
      void readData(std::deque<double> & container)
      {
        std::string token;
        reader >> token;

        while (string_to_lower(token) != "</dataarray>")
        {
          container.push_back( atof(token.c_str()) );
          reader >> token;
        }
      }

      /** @brief Read point or cell data and fill the respective data containers */
      template <typename ContainerType, typename NameContainerType>
      void readPointCellData(segment_id_type seg_id,
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

      /** @brief Pushes the vertices read to the domain */
      void setupVertices(DomainType & domain)
      {
        for (std::size_t i=0; i<global_points_2.size(); ++i)
          viennagrid::make_vertex( domain, typename VertexType::id_type(i), global_points_2[i] );
      }

      /** @brief Pushes the cells read to the domain. Preserves segment information. */
      void setupCells(DomainType & domain, SegmentationType & segmentation, segment_id_type seg_id)
      {
          //***************************************************
          // building up the cells in ViennaGrid
          // -------------------------------------------------
          // "cells"   ... contain the indices of the nodes
          // "offsets" ... contain the information about
          //               the affiliation of the nodes
          //               to the cells
          //***************************************************
          //CellType cell;
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


            //****************************************************
            // read out the node indices form the "cells"-vector
            // and add the cells to the "vertices"-array
            //****************************************************

            viennagrid::storage::static_array<VertexHandleType, boundary_elements<CellTag, vertex_tag>::num> cell_vertex_handles;

            vtk_to_viennagrid_orientations<CellTag> reorderer;
            for (long j = 0; j < numVertices; j++)
            {
              long reordered_j = reorderer(j);
              std::size_t local_index = local_cell_vertices[seg_id][reordered_j + offsetIdx];
              std::size_t global_vertex_index = local_to_global_map[seg_id][local_index];

              //std::cout << global_vertex_index << " ";
              cell_vertex_handles[j] = viennagrid::elements<viennagrid::vertex_tag>(domain).handle_at(global_vertex_index);

              viennagrid::add_handle( segmentation[seg_id], domain, cell_vertex_handles[j] );

            }

            viennagrid::make_element<CellType>(segmentation[seg_id], cell_vertex_handles.begin(), cell_vertex_handles.end());//, typename CellType::id_type(i));
          }
      }

      /** @brief Writes data for vertices to the ViennaGrid domain using ViennaData */
      template <typename ContainerType>
      void setupDataVertex(DomainType & domain, SegmentType & segment, segment_id_type seg_id, ContainerType const & container, std::size_t num_components)
      {
        std::string const & name = container.first;

        if (num_components == 1)
        {
          VertexScalarOutputAccessorContainer & current_registered_segment_vertex_scalar_data = registered_segment_vertex_scalar_data[seg_id];
          if (registered_vertex_scalar_data.find(name) != registered_vertex_scalar_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(domain)[global_vertex_id];

              (*registered_vertex_scalar_data[name])(vertex) = (container.second)[i];
            }
          }
          else if (current_registered_segment_vertex_scalar_data.find(name) != current_registered_segment_vertex_scalar_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(domain)[global_vertex_id];

              (*current_registered_segment_vertex_scalar_data[name])(vertex) = (container.second)[i];
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading scalar quantity "
                      << container.first << " to vertices." << std::endl;
            #endif
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(domain)[global_vertex_id];

              if ( static_cast<typename VertexType::id_type::base_id_type>(vertex_scalar_data[container.first][seg_id].size()) <= vertex.id().get()) vertex_scalar_data[container.first][seg_id].resize(vertex.id().get()+1);
              vertex_scalar_data[container.first][seg_id][vertex.id().get()] = (container.second)[i];
            }
          }
        }
        else
        {
          VertexVectorOutputAccessorContainer & current_registered_segment_vertex_vector_data = registered_segment_vertex_vector_data[seg_id];
          if (registered_vertex_vector_data.find(name) != registered_vertex_vector_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(domain)[global_vertex_id];

              (*registered_vertex_vector_data[name])(vertex).resize(3);
              (*registered_vertex_vector_data[name])(vertex)[0] = (container.second)[3*i+0];
              (*registered_vertex_vector_data[name])(vertex)[1] = (container.second)[3*i+1];
              (*registered_vertex_vector_data[name])(vertex)[2] = (container.second)[3*i+2];
            }
          }
          else if (current_registered_segment_vertex_vector_data.find(name) != current_registered_segment_vertex_vector_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(domain)[global_vertex_id];

              (*current_registered_segment_vertex_vector_data[name])(vertex).resize(3);
              (*current_registered_segment_vertex_vector_data[name])(vertex)[0] = (container.second)[3*i+0];
              (*current_registered_segment_vertex_vector_data[name])(vertex)[1] = (container.second)[3*i+1];
              (*current_registered_segment_vertex_vector_data[name])(vertex)[2] = (container.second)[3*i+2];
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading vector quantity "
                      << container.first << " to vertices." << std::endl;
            #endif
            assert( 3 * viennagrid::elements<viennagrid::vertex_tag>(segment).size() == container.second.size());
            for (std::size_t i=0; i<container.second.size() / 3; ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[seg_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(domain)[global_vertex_id];

              if ( static_cast<typename VertexType::id_type::base_id_type>(vertex_vector_data[container.first][seg_id].size()) <= vertex.id().get()) vertex_vector_data[container.first][seg_id].resize(vertex.id().get()+1);
              vertex_vector_data[container.first][seg_id][vertex.id().get()].resize(3);
              vertex_vector_data[container.first][seg_id][vertex.id().get()][0] = (container.second)[3*i+0];
              vertex_vector_data[container.first][seg_id][vertex.id().get()][1] = (container.second)[3*i+1];
              vertex_vector_data[container.first][seg_id][vertex.id().get()][2] = (container.second)[3*i+2];
            }
          }
        }
      }

      /** @brief Writes data for cells to the ViennaGrid domain using ViennaData */
      template <typename ContainerType>
      void setupDataCell(DomainType &, SegmentType & segment, segment_id_type seg_id, ContainerType const & container, std::size_t num_components)
      {
        std::string const & name = container.first;

        if (num_components == 1)
        {
          CellScalarOutputAccessorContainer & current_registered_segment_cell_scalar_data = registered_segment_cell_scalar_data[seg_id];
          if (registered_cell_scalar_data.find(name) != registered_cell_scalar_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = viennagrid::elements<CellTag>(segment)[i];

              (*registered_cell_scalar_data[name])(cell) = (container.second)[i];
            }
          }
          else if (current_registered_segment_cell_scalar_data.find(name) != current_registered_segment_cell_scalar_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = viennagrid::elements<CellTag>(segment)[i];

              (*current_registered_segment_cell_scalar_data[name])(cell) = (container.second)[i];
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading scalar quantity "
                      << container.first << " to vertices." << std::endl;
            #endif
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = viennagrid::elements<CellTag>(segment)[i];

              if ( static_cast<typename CellType::id_type::base_id_type>(cell_scalar_data[container.first][seg_id].size()) <= cell.id().get()) cell_scalar_data[container.first][seg_id].resize(cell.id().get()+1);
              cell_scalar_data[container.first][seg_id][cell.id().get()] = (container.second)[i];
            }
          }
        }
        else
        {
          CellVectorOutputAccessorContainer & current_registered_segment_cell_vector_data = registered_segment_cell_vector_data[seg_id];
          if (registered_cell_vector_data.find(name) != registered_cell_vector_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = viennagrid::elements<CellTag>(segment)[i];

              (*registered_cell_vector_data[name])(cell).resize(3);
              (*registered_cell_vector_data[name])(cell)[0] = (container.second)[3*i+0];
              (*registered_cell_vector_data[name])(cell)[1] = (container.second)[3*i+1];
              (*registered_cell_vector_data[name])(cell)[2] = (container.second)[3*i+2];
            }
          }
          else if (current_registered_segment_cell_vector_data.find(name) != current_registered_segment_cell_vector_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = viennagrid::elements<CellTag>(segment)[i];

              (*current_registered_segment_cell_vector_data[name])(cell).resize(3);
              (*current_registered_segment_cell_vector_data[name])(cell)[0] = (container.second)[3*i+0];
              (*current_registered_segment_cell_vector_data[name])(cell)[1] = (container.second)[3*i+1];
              (*current_registered_segment_cell_vector_data[name])(cell)[2] = (container.second)[3*i+2];
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading vector quantity "
                      << container.first << " to vertices." << std::endl;
            #endif
            assert( 3 * viennagrid::elements<CellTag>(segment).size() == container.second.size());
            for (std::size_t i=0; i<container.second.size() / 3; ++i)
            {
              CellType const & cell = viennagrid::elements<CellTag>(segment)[i];

              if ( static_cast<typename CellType::id_type::base_id_type>(cell_vector_data[container.first][seg_id].size()) <= cell.id().get()) cell_vector_data[container.first][seg_id].resize(cell.id().get()+1);
              cell_vector_data[container.first][seg_id][cell.id().get()].resize(3);
              cell_vector_data[container.first][seg_id][cell.id().get()][0] = (container.second)[3*i+0];
              cell_vector_data[container.first][seg_id][cell.id().get()][1] = (container.second)[3*i+1];
              cell_vector_data[container.first][seg_id][cell.id().get()][2] = (container.second)[3*i+2];
            }
          }
        }
      }

      /** @brief Writes all data read from files to the domain */
      void setupData(DomainType & domain, SegmentationType & segmentation, segment_id_type seg_id)
      {
        for (size_t i=0; i<local_scalar_vertex_data[seg_id].size(); ++i)
        {
          setupDataVertex(domain, segmentation[seg_id], seg_id, local_scalar_vertex_data[seg_id][i], 1);
        }

        for (size_t i=0; i<local_vector_vertex_data[seg_id].size(); ++i)
        {
          setupDataVertex(domain, segmentation[seg_id], seg_id, local_vector_vertex_data[seg_id][i], 3);
        }


        for (size_t i=0; i<local_scalar_cell_data[seg_id].size(); ++i)
        {
          setupDataCell(domain, segmentation[seg_id], seg_id, local_scalar_cell_data[seg_id][i], 1);
        }

        for (size_t i=0; i<local_vector_cell_data[seg_id].size(); ++i)
        {
          setupDataCell(domain, segmentation[seg_id], seg_id, local_vector_cell_data[seg_id][i], 3);
        }

      }

      /** @brief Parses a .vtu file referring to a segment of the domain */
      void parse_vtu_segment(std::string filename, segment_id_type seg_id)
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

      /** @brief Processes a .vtu file that represents a full domain */
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

      /** @brief Processes a .pvd file containing the links to the segments stored in individual .vtu files */
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

        if (string_to_lower(tag.get_value("type")) != "collection")
          throw bad_file_format_exception(filename, "Parse error: Type-attribute of VTKFile tag is not 'Collection'!");

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


      ~vtk_reader() { clear(); }


      /** @brief Triggers the read process.
       *
       * @param domain    The ViennaGrid domain
       * @param filename  Name of the file containing the mesh. Either .pvd (multi-segment) or .vtu (single segment)
       */
      int operator()(DomainType & domain, SegmentationType & segmentation, std::string const & filename)
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
        for (size_t seg_id = 0; seg_id < local_cell_num.size(); ++seg_id)
        {
          segmentation.make_segment();
        }

        for (size_t seg_id = 0; seg_id < local_cell_num.size(); ++seg_id)
        {
          setupCells(domain, segmentation, seg_id);
          setupData(domain, segmentation, seg_id);
        }

        clear();

        return EXIT_SUCCESS;
      } //operator()



      void operator()(DomainType & domain, std::string const & filename)
      {
        SegmentationType tmp;
        (*this)(domain, tmp, filename);
      }




      /** @brief Returns the data names of all scalar vertex data read */
      std::vector<std::string> scalar_vertex_data_names(segment_id_type segment_id) const
      {
        std::vector<std::string> ret;
        for (std::size_t i=0; i<local_scalar_vertex_data[segment_id].size(); ++i)
          ret.push_back(local_scalar_vertex_data[segment_id][i].first);

        return ret;
      }

      /** @brief Returns the data names of all vector vertex data read */
      std::vector<std::string> vector_vertex_data_names(segment_id_type segment_id) const
      {
        std::vector<std::string> ret;
        for (std::size_t i=0; i<local_vector_vertex_data[segment_id].size(); ++i)
          ret.push_back(local_vector_vertex_data[segment_id][i].first);

        return ret;
      }

      /** @brief Returns the data names of all scalar cell data read */
      std::vector<std::string> scalar_cell_data_names(segment_id_type segment_id) const
      {
        std::vector<std::string> ret;
        for (std::size_t i=0; i<local_scalar_cell_data[segment_id].size(); ++i)
          ret.push_back(local_scalar_cell_data[segment_id][i].first);

        return ret;
      }

      /** @brief Returns the data names of all vector cell data read */
      std::vector<std::string> vector_cell_data_names(segment_id_type segment_id) const
      {
        std::vector<std::string> ret;
        for (std::size_t i=0; i<local_vector_cell_data[segment_id].size(); ++i)
          ret.push_back(local_vector_cell_data[segment_id][i].first);

        return ret;
      }

        // Extract data read from file:

        /** @brief Returns the names of all scalar-valued data read for vertices */
        std::vector<std::pair<std::size_t, std::string> > const & get_scalar_data_on_vertices() const
        {
          return vertex_data_scalar_read;
        }

        /** @brief Returns the names of all vector-valued data read for vertices */
        std::vector<std::pair<std::size_t, std::string> > const & get_vector_data_on_vertices() const
        {
          return vertex_data_vector_read;
        }

        /** @brief Returns the names of all scalar-valued data read for cells */
        std::vector<std::pair<std::size_t, std::string> > const & get_scalar_data_on_cells() const
        {
          return cell_data_scalar_read;
        }

        /** @brief Returns the names of all vector-valued data read for cells */
        std::vector<std::pair<std::size_t, std::string> > const & get_vector_data_on_cells() const
        {
          return cell_data_vector_read;
        }



    private:

      template<typename MapType, typename AccessorType>
      void register_to_map(MapType & map, AccessorType accessor, std::string const & name)
      {
          typename MapType::iterator it = map.find(name);
          if (it != map.end())
          {
            delete it->second;
            it->second = new dynamic_accessor_t<AccessorType>( accessor );
          }
          else
            map[name] = new dynamic_accessor_t<AccessorType>( accessor );
      }


    public:


        template<typename AccessorType>
        void register_vertex_scalar_accessor(AccessorType accessor, std::string const & name)
        { register_to_map(registered_vertex_scalar_data, accessor, name); }

        template<typename AccessorType>
        void register_vertex_scalar_accessor(segment_id_type seg_id, AccessorType accessor, std::string const & name)
        { register_to_map(registered_segment_vertex_scalar_data[seg_id], accessor, name); }

        template<typename AccessorType>
        void register_vertex_scalar_accessor(SegmentType const & segment, AccessorType accessor, std::string const & name)
        { register_vertex_scalar_accessor(segment.id(), accessor, name); }


        template<typename AccessorType>
        void register_vertex_vector_accessor(AccessorType accessor, std::string const & name)
        { register_to_map(registered_vertex_vector_data, accessor, name); }

        template<typename AccessorType>
        void register_vertex_vector_accessor(segment_id_type seg_id, AccessorType accessor, std::string const & name)
        { register_to_map(registered_segment_vertex_vector_data[seg_id], accessor, name); }

        template<typename AccessorType>
        void register_vertex_vector_accessor(SegmentType const & segment, AccessorType accessor, std::string const & name)
        { register_vertex_vector_accessor(segment.id(), accessor, name); }





        template<typename AccessorType>
        void register_cell_scalar_accessor(AccessorType accessor, std::string const & name)
        { register_to_map(registered_cell_scalar_data, accessor, name); }

        template<typename AccessorType>
        void register_cell_scalar_accessor(segment_id_type seg_id, AccessorType accessor, std::string const & name)
        { register_to_map(registered_segment_cell_scalar_data[seg_id], accessor, name); }

        template<typename AccessorType>
        void register_cell_scalar_accessor(SegmentType const & segment, AccessorType accessor, std::string const & name)
        { register_cell_scalar_accessor(segment.id(), accessor, name); }


        template<typename AccessorType>
        void register_cell_vector_accessor(AccessorType accessor, std::string const & name)
        { register_to_map(registered_cell_vector_data, accessor, name); }

        template<typename AccessorType>
        void register_cell_vector_accessor(segment_id_type seg_id, AccessorType accessor, std::string const & name)
        { register_to_map(registered_segment_cell_vector_data[seg_id], accessor, name); }

        template<typename AccessorType>
        void register_cell_vector_accessor(SegmentType const & segment, AccessorType accessor, std::string const & name)
        { register_cell_vector_accessor(segment.id(), accessor, name); }







        typename viennagrid::result_of::accessor<std::deque<double>, VertexType >::type vertex_scalar_accessor( std::string const & name, segment_id_type seg_id )
        {
          typename std::map< std::string, std::map<segment_id_type, std::deque<double> > >::iterator it = vertex_scalar_data.find(name);
          if (it == vertex_scalar_data.end()) return typename viennagrid::result_of::accessor<std::deque<double>, VertexType >::type();

          typename std::map<segment_id_type, std::deque<double> >::iterator jt = it->second.find( seg_id );
          if (jt == it->second.end()) return typename viennagrid::result_of::accessor<std::deque<double>, VertexType >::type();

          return viennagrid::make_accessor<VertexType>( jt->second );
        }

        typename viennagrid::result_of::accessor<std::deque<double>, VertexType >::type vertex_scalar_accessor( std::string const & name, SegmentType const & segment )
        { return vertex_scalar_accessor(name, segment.id()); }

        typename viennagrid::result_of::accessor<std::deque<vector_data_type>, VertexType >::type vertex_vector_accessor( std::string const & name, segment_id_type seg_id )
        {
          typename std::map< std::string, std::map<segment_id_type, std::deque<vector_data_type> > >::iterator it = vertex_vector_data.find(name);
          if (it == vertex_vector_data.end()) return typename viennagrid::result_of::accessor<std::deque<vector_data_type>, VertexType >::type();

          typename std::map<segment_id_type, std::deque<vector_data_type> >::iterator jt = it->second.find( seg_id );
          if (jt == it->second.end()) return typename viennagrid::result_of::accessor<std::deque<vector_data_type>, VertexType >::type();

          return viennagrid::make_accessor<VertexType>( jt->second );
        }

        typename viennagrid::result_of::accessor<std::deque<vector_data_type>, VertexType >::type vertex_vector_accessor( std::string const & name, SegmentType const & segment )
        { return vertex_scalar_accessor(name, segment.id()); }



        typename viennagrid::result_of::accessor<std::deque<double>, CellType >::type cell_scalar_accessor( std::string const & name, segment_id_type seg_id )
        {
          typename std::map< std::string, std::map<segment_id_type, std::deque<double> > >::iterator it = cell_scalar_data.find(name);
          if (it == cell_scalar_data.end()) return typename viennagrid::result_of::accessor<std::deque<double>, CellType >::type();

          typename std::map<segment_id_type, std::deque<double> >::iterator jt = it->second.find( seg_id );
          if (jt == it->second.end()) return typename viennagrid::result_of::accessor<std::deque<double>, CellType >::type();

          return viennagrid::make_accessor<CellType>( jt->second );
        }

        typename viennagrid::result_of::accessor<std::deque<double>, CellType >::type cell_scalar_accessor( std::string const & name, SegmentType const & segment )
        { return cell_scalar_accessor(name, segment.id()); }

        typename viennagrid::result_of::accessor<std::deque<vector_data_type>, CellType >::type cell_vector_accessor( std::string const & name, segment_id_type seg_id )
        {
          typename std::map< std::string, std::map<segment_id_type, std::deque<vector_data_type> > >::iterator it = cell_vector_data.find(name);
          if (it == cell_vector_data.end()) return typename viennagrid::result_of::accessor<std::deque<vector_data_type>, CellType >::type();

          typename std::map<segment_id_type, std::deque<vector_data_type> >::iterator jt = it->second.find( seg_id );
          if (jt == it->second.end()) return typename viennagrid::result_of::accessor<std::deque<vector_data_type>, CellType >::type();

          return viennagrid::make_accessor<CellType>( jt->second );
        }

        typename viennagrid::result_of::accessor<std::deque<vector_data_type>, CellType >::type cell_vector_accessor( std::string const & name, SegmentType const & segment )
        { return cell_vector_accessor(name, segment.id()); }


      private:

        // Quantities read:
        std::vector<std::pair<std::size_t, std::string> >         vertex_data_scalar_read;
        std::vector<std::pair<std::size_t, std::string> >         vertex_data_vector_read;

        std::vector<std::pair<std::size_t, std::string> >         cell_data_scalar_read;
        std::vector<std::pair<std::size_t, std::string> >         cell_data_vector_read;





        std::map< std::string, std::map<segment_id_type, std::deque<double> > > vertex_scalar_data;
        std::map< std::string, std::map<segment_id_type, std::deque<vector_data_type> > > vertex_vector_data;

        std::map< std::string, std::map<segment_id_type, std::deque<double> > > cell_scalar_data;
        std::map< std::string, std::map<segment_id_type, std::deque<vector_data_type> > > cell_vector_data;





        VertexScalarOutputAccessorContainer          registered_vertex_scalar_data;
        VertexVectorOutputAccessorContainer          registered_vertex_vector_data;

        CellScalarOutputAccessorContainer          registered_cell_scalar_data;
        CellVectorOutputAccessorContainer          registered_cell_vector_data;

        std::map< segment_id_type, VertexScalarOutputAccessorContainer > registered_segment_vertex_scalar_data;
        std::map< segment_id_type, VertexVectorOutputAccessorContainer > registered_segment_vertex_vector_data;

        std::map< segment_id_type, CellScalarOutputAccessorContainer >   registered_segment_cell_scalar_data;
        std::map< segment_id_type, CellVectorOutputAccessorContainer >   registered_segment_cell_vector_data;



    }; //class vtk_reader


    /** @brief Convenience function for importing a mesh using a single line of code. */
    template <typename DomainType, typename SegmentationType >
    int import_vtk(DomainType & domain, SegmentationType & segmentation, std::string const & filename)
    {
      vtk_reader<DomainType, SegmentationType> vtk_reader;
      return vtk_reader(domain, segmentation, filename);
    }

    template <typename DomainType>
    int import_vtk(DomainType & domain, std::string const & filename)
    {
      vtk_reader<DomainType> vtk_reader;
      return vtk_reader(domain, filename);
    }



    
    
    
    template <typename DomainT, typename SegmentationT, typename AccessorT>
    vtk_reader<DomainT, SegmentationT> & add_scalar_data_on_vertices(vtk_reader<DomainT, SegmentationT> & writer,
                                                                    AccessorT const accessor,
                                                                    std::string const & quantity_name)
    {
      writer.register_vertex_scalar_accessor(accessor, quantity_name);
      return writer;
    }

    template <typename DomainT, typename SegmentationT, typename AccessorT>
    vtk_reader<DomainT, SegmentationT> & add_vector_data_on_vertices(vtk_reader<DomainT, SegmentationT> & writer,
                                                                    AccessorT const accessor,
                                                                    std::string const & quantity_name)
    {
      writer.register_vertex_vector_accessor(accessor, quantity_name);
      return writer;
    }






    template <typename DomainT, typename SegmentationT, typename AccessorT>
    vtk_reader<DomainT, SegmentationT> & add_scalar_data_on_cells(vtk_reader<DomainT, SegmentationT> & writer,
                                                                    AccessorT const accessor,
                                                                    std::string const & quantity_name)
    {
      writer.register_cell_scalar_accessor(accessor, quantity_name);
      return writer;
    }

    template <typename DomainT, typename SegmentationT, typename AccessorT>
    vtk_reader<DomainT, SegmentationT> & add_vector_data_on_cells(vtk_reader<DomainT, SegmentationT> & writer,
                                                                    AccessorT const accessor,
                                                                    std::string const & quantity_name)
    {
      writer.register_cell_vector_accessor(accessor, quantity_name);
      return writer;
    }


    



  } //namespace io
} //namespace viennagrid

#endif
