#ifndef VIENNAGRID_IO_VTK_READER_HPP
#define VIENNAGRID_IO_VTK_READER_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


/** @file viennagrid/io/vtk_reader.hpp
 *  @brief    This is a simple vtk-reader implementation. Refer to the vtk-standard (cf. http://www.vtk.org/pdf/file-formats.pdf) and make sure the same order of XML tags is preserved.
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>

#include "viennagrid/core.hpp"
#include "viennagrid/io/vtk_common.hpp"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/xml_tag.hpp"
#include "viennagrid/accessor.hpp"

namespace viennagrid
{
  namespace io
  {

    /** @brief A VTK reader class that allows to read meshes from XML-based VTK files as defined in http://www.vtk.org/pdf/file-formats.pdf
     *
     * @tparam MeshType         The type of the mesh to be read. Must not be a region type!
     * @tparam SegmentationType   The type of the regionation to be read, default is the default regionation of MeshType
     */
    template<typename SomethingT>
    class vtk_reader
    {
    protected:

      typedef typename result_of::mesh_hierarchy<SomethingT>::type mesh_hierarchy_type;
      typedef typename result_of::mesh<mesh_hierarchy_type>::type mesh_type;

      typedef typename mesh_type::region_type RegionType;
      typedef typename viennagrid::result_of::id<RegionType>::type region_id_type;

      typedef typename viennagrid::result_of::coord<mesh_type>::type CoordType;

      typedef typename result_of::vertex<mesh_type>::type                          VertexType;
      typedef typename result_of::vertex_id<mesh_type>::type                           VertexIDType;
      typedef typename result_of::cell<mesh_type>::type     CellType;

      typedef typename viennagrid::result_of::vertex_range<mesh_type>::type   VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                             VertexIterator;

      typedef typename viennagrid::result_of::line_range<mesh_type>::type     EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                               EdgeIterator;

      typedef typename viennagrid::result_of::facet_range<mesh_type>::type   FacetRange;
      typedef typename viennagrid::result_of::iterator<FacetRange>::type                                   FacetIterator;

      typedef typename viennagrid::result_of::cell_range<mesh_type>::type     CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                  CellIterator;


      typedef std::vector<double> vector_data_type;

      typedef std::map< std::string, base_dynamic_field<double, VertexType> * >             VertexScalarOutputFieldContainer;
      typedef std::map< std::string, base_dynamic_field<vector_data_type, VertexType> * >   VertexVectorOutputFieldContainer;

      typedef std::map< std::string, base_dynamic_field<double, CellType> * >               CellScalarOutputFieldContainer;
      typedef std::map< std::string, base_dynamic_field<vector_data_type, CellType> * >     CellVectorOutputFieldContainer;





      std::ifstream                                        reader;

      std::size_t                                          geometric_dim;
      element_tag                                          cell_tag;

      typedef std::vector<CoordType>                       PointType;
      std::map<PointType, std::size_t, point_less>         global_points;
      std::map<std::size_t, PointType>                     global_points_2;
      std::map<int, std::deque<std::size_t> >              local_to_global_map;
      std::map<int, std::deque<std::size_t> >              local_cell_vertices;
      std::map<int, std::deque<std::size_t> >              local_cell_offsets;
      std::map<int, std::size_t>                           local_cell_num;
      std::map<int, std::deque<CellType> >                 local_cell_handle;

      //data containers:
      std::map<int, std::deque<std::pair<std::string, std::deque<double> > > >  local_scalar_vertex_data;
      std::map<int, std::deque<std::pair<std::string, std::deque<double> > > >  local_vector_vertex_data;
      std::map<int, std::deque<std::pair<std::string, std::deque<double> > > >  local_scalar_cell_data;
      std::map<int, std::deque<std::pair<std::string, std::deque<double> > > >  local_vector_cell_data;


      template<typename map_type>
      void clear_map( map_type & map )
      {
        for (typename map_type::iterator it = map.begin(); it != map.end(); ++it)
          delete it->second;

        map.clear();
      }

      void post_clear()
      {
        clear_map(registered_vertex_scalar_data);
        clear_map(registered_vertex_vector_data);

        clear_map(registered_cell_scalar_data);
        clear_map(registered_cell_vector_data);


        for (typename std::map< region_id_type, VertexScalarOutputFieldContainer>::iterator it = registered_region_vertex_scalar_data.begin(); it != registered_region_vertex_scalar_data.end(); ++it)
          clear_map(it->second);

        for (typename std::map< region_id_type, VertexVectorOutputFieldContainer>::iterator it = registered_region_vertex_vector_data.begin(); it != registered_region_vertex_vector_data.end(); ++it)
          clear_map(it->second);


        for (typename std::map< region_id_type, CellScalarOutputFieldContainer>::iterator it = registered_region_cell_scalar_data.begin(); it != registered_region_cell_scalar_data.end(); ++it)
          clear_map(it->second);

        for (typename std::map< region_id_type, CellVectorOutputFieldContainer>::iterator it = registered_region_cell_vector_data.begin(); it != registered_region_cell_vector_data.end(); ++it)
          clear_map(it->second);


        registered_region_vertex_scalar_data.clear();
        registered_region_vertex_vector_data.clear();

        registered_region_cell_scalar_data.clear();
        registered_region_cell_vector_data.clear();
      }


      void pre_clear()
      {
        vertex_data_scalar_read.clear();
        vertex_data_vector_read.clear();

        cell_data_scalar_read.clear();
        cell_data_vector_read.clear();

        vertex_scalar_data.clear();
        vertex_vector_data.clear();

        cell_scalar_data.clear();
        cell_vector_data.clear();

        global_points.clear();
        global_points_2.clear();
        local_to_global_map.clear();
        local_cell_vertices.clear();
        local_cell_offsets.clear();
        local_cell_num.clear();
        local_cell_handle.clear();

        local_scalar_vertex_data.clear();
        local_vector_vertex_data.clear();
        local_scalar_cell_data.clear();
        local_vector_cell_data.clear();
      }





      /** @brief Opens a file */
      void openFile(std::string const & filename)
      {
        reader.open(filename.c_str());
        if(!reader)
        {
          throw cannot_open_file_exception("* ViennaGrid: vtk_reader::openFile(): File " + filename + ": Cannot open file!");
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
          ss << "* ViennaGrid: vtk_reader::operator(): Expected \"" << expectedToken << "\", but got \"" << token << "\"";
          throw bad_file_format_exception(ss.str());
        }
      }

      /** @brief Reads the coordinates of the points/vertices in the mesh */
      void readNodeCoordinates(std::size_t nodeNum, std::size_t numberOfComponents, region_id_type region_id)
      {
        double nodeCoord;
        local_to_global_map[region_id].resize(nodeNum);

        for(std::size_t i = 0; i < nodeNum; i++)
        {
          PointType p(geometric_dim);
//           PointType p;

          for(std::size_t j = 0; j < numberOfComponents; j++)
          {
            reader >> nodeCoord;
            if (j < static_cast<std::size_t>(geometric_dim))
              p[j] = nodeCoord;
          }

          //add point to global list if not already there
          if (global_points.find(p) == global_points.end())
          {
            std::size_t new_global_id = global_points.size();
            global_points.insert( std::make_pair(p, new_global_id) );
            global_points_2.insert( std::make_pair(new_global_id, p) );
            local_to_global_map[region_id][i] = new_global_id;
          }
          else
          {
            local_to_global_map[region_id][i] = global_points[p];
          }
        }
      }

      /** @brief Reads the vertex indices of the cells inside the mesh */
      void readCellIndices(region_id_type region_id)
      {
        std::size_t cellNode = 0;
        std::string token;
        reader >> token;

        while (token != "</DataArray>")
        {
          assert( strChecker::myIsNumber(token) && "Cell vertex index is not a number!" );

          cellNode = static_cast<std::size_t>(atoi(token.c_str()));
          local_cell_vertices[region_id].push_back(cellNode);

          reader >> token;
        }
      }

      /** @brief Read the cell offsets for the vertex indices */
      void readOffsets(region_id_type region_id)
      {
          //****************************************************************************
          // read in the offsets: describe the affiliation of the nodes to the cells
          // (see: http://www.vtk.org/pdf/file-formats.pdf , page 9)
          //****************************************************************************

          std::string token;
          std::size_t offset = 0;
          reader >> token;

          while(token != "</DataArray>")
          {
            assert( strChecker::myIsNumber(token) && "Cell offset is not a number!" );

            offset = static_cast<std::size_t>(atoi(token.c_str()));
            local_cell_offsets[region_id].push_back(offset);

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
            assert(type == vtk_element_tag(cell_tag) && "Error in VTK reader: Type mismatch!");
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
      void readPointCellData(region_id_type region_id,
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
            components = static_cast<std::size_t>(atoi(tag.get_value("numberofcomponents").c_str()));

          //now read data:
          if (components == 1)
          {
            data_names_scalar.push_back(std::make_pair(region_id, name));
            scalar_data[region_id].push_back( std::make_pair(name, std::deque<double>()) );
            readData(scalar_data[region_id].back().second);
          }
          else if (components == 3)
          {
            data_names_vector.push_back(std::make_pair(region_id, name));
            vector_data[region_id].push_back( std::make_pair(name, std::deque<double>()) );
            readData(vector_data[region_id].back().second);
          }
          else
            throw bad_file_format_exception("* ViennaGrid: vtk_reader::readPointCellData(): Number of components for data invalid!");

          tag.parse(reader);
        }


        if (tag.name() != "/pointdata" && tag.name() != "/celldata")
            throw bad_file_format_exception("* ViennaGrid: vtk_reader::readPointCellData(): XML Parse error: Expected </PointData> or </CellData>!");

      }

      /////////////////////////// Routines for pushing everything to mesh ///////////////

      /** @brief Pushes the vertices read to the mesh */
      void setupVertices(mesh_type & mesh_obj)
      {
        for (std::size_t i=0; i<global_points_2.size(); ++i)
          viennagrid::make_vertex( mesh_obj, &global_points_2[i][0] );
      }

      /** @brief Pushes the cells read to the mesh. Preserves region information. */
      void setupCells(mesh_type & mesh_obj, region_id_type region_id)
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
        std::size_t numVertices = 0;
        std::size_t offsetIdx = 0;

        std::deque<std::size_t> const & offsets = local_cell_offsets[region_id];

        for (std::size_t i = 0; i < local_cell_num[region_id]; i++)
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

//           viennagrid::static_array<VertexHandleType, boundary_elements<CellTag, vertex_tag>::num> cell_vertex_handles;
          std::vector<VertexType> cell_vertex_handles( cell_tag.vertex_count() );
          std::vector<VertexIDType> cell_vertex_ids(numVertices);

          VertexRange vertices(mesh_obj);

          detail::vtk_to_viennagrid_orientations reorderer(cell_tag);
          for (std::size_t j = 0; j < numVertices; j++)
          {
            std::size_t reordered_j = reorderer(j);
            std::size_t local_index = local_cell_vertices[region_id][reordered_j + offsetIdx];
            std::size_t global_vertex_index = local_to_global_map[region_id][local_index];

            cell_vertex_handles[j] = vertices[global_vertex_index];
            viennagrid::add( mesh_obj.get_make_region(region_id), cell_vertex_handles[j] );

            cell_vertex_ids[j] = cell_vertex_handles[j].id();
          }


          CellType cell = viennagrid::make_cell(mesh_obj, cell_vertex_handles.begin(), cell_vertex_handles.end());
          viennagrid::add( mesh_obj.get_make_region(region_id), cell );
          local_cell_handle[region_id].push_back( cell );
        }
      }

      /** @brief Writes data for vertices to the ViennaGrid mesh using ViennaData */
      template <typename ContainerType>
      void setupDataVertex(mesh_type & mesh_obj, region_id_type region_id, ContainerType const & container, std::size_t num_components)
      {
        std::string const & name = container.first;

        if (num_components == 1)
        {
          VertexScalarOutputFieldContainer & current_registered_region_vertex_scalar_data = registered_region_vertex_scalar_data[region_id];
          if (registered_vertex_scalar_data.find(name) != registered_vertex_scalar_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[region_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(mesh_obj)[global_vertex_id];

              (*registered_vertex_scalar_data[name])(vertex) = (container.second)[i];
            }
          }
          else if (current_registered_region_vertex_scalar_data.find(name) != current_registered_region_vertex_scalar_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[region_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(mesh_obj)[global_vertex_id];

              (*current_registered_region_vertex_scalar_data[name])(vertex) = (container.second)[i];
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
              std::size_t global_vertex_id = local_to_global_map[region_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(mesh_obj)[global_vertex_id];

              if ( static_cast<typename result_of::id<VertexType>::type>(vertex_scalar_data[container.first][region_id].size()) <= vertex.id())
                vertex_scalar_data[container.first][region_id].resize(static_cast<std::size_t>(vertex.id()+1));
              vertex_scalar_data[container.first][region_id][static_cast<std::size_t>(vertex.id())] = (container.second)[i];
            }
          }
        }
        else
        {
          VertexVectorOutputFieldContainer & current_registered_region_vertex_vector_data = registered_region_vertex_vector_data[region_id];
          if (registered_vertex_vector_data.find(name) != registered_vertex_vector_data.end())
          {
            for (std::size_t i=0; i<container.second.size()/3; ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[region_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(mesh_obj)[global_vertex_id];

              (*registered_vertex_vector_data[name])(vertex).resize(3);
              (*registered_vertex_vector_data[name])(vertex)[0] = (container.second)[3*i+0];
              (*registered_vertex_vector_data[name])(vertex)[1] = (container.second)[3*i+1];
              (*registered_vertex_vector_data[name])(vertex)[2] = (container.second)[3*i+2];
            }
          }
          else if (current_registered_region_vertex_vector_data.find(name) != current_registered_region_vertex_vector_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[region_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(mesh_obj)[global_vertex_id];

              (*current_registered_region_vertex_vector_data[name])(vertex).resize(3);
              (*current_registered_region_vertex_vector_data[name])(vertex)[0] = (container.second)[3*i+0];
              (*current_registered_region_vertex_vector_data[name])(vertex)[1] = (container.second)[3*i+1];
              (*current_registered_region_vertex_vector_data[name])(vertex)[2] = (container.second)[3*i+2];
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading vector quantity "
                      << container.first << " to vertices." << std::endl;
            #endif
            assert( 3 * viennagrid::elements<viennagrid::vertex_tag>(mesh_obj.get_make_region(region_id)).size() == container.second.size());
            for (std::size_t i=0; i<container.second.size() / 3; ++i)
            {
              std::size_t global_vertex_id = local_to_global_map[region_id][i];
              VertexType const & vertex = viennagrid::elements<viennagrid::vertex_tag>(mesh_obj.get_make_region(region_id))[global_vertex_id];

              if ( static_cast<typename result_of::id<VertexType>::type>(vertex_vector_data[container.first][region_id].size()) <= vertex.id())
                vertex_vector_data[container.first][region_id].resize(static_cast<std::size_t>(vertex.id()+1));
              vertex_vector_data[container.first][region_id][static_cast<std::size_t>(vertex.id())].resize(3);
              vertex_vector_data[container.first][region_id][static_cast<std::size_t>(vertex.id())][0] = (container.second)[3*i+0];
              vertex_vector_data[container.first][region_id][static_cast<std::size_t>(vertex.id())][1] = (container.second)[3*i+1];
              vertex_vector_data[container.first][region_id][static_cast<std::size_t>(vertex.id())][2] = (container.second)[3*i+2];
            }
          }
        }
      }

      /** @brief Writes data for cells to the ViennaGrid mesh using ViennaData */
      template <typename ContainerType>
      void setupDataCell(mesh_type &, RegionType & region, region_id_type region_id, ContainerType const & container, std::size_t num_components)
      {
        std::string const & name = container.first;

        if (num_components == 1)
        {
          CellScalarOutputFieldContainer & current_registered_region_cell_scalar_data = registered_region_cell_scalar_data[region_id];
          if (registered_cell_scalar_data.find(name) != registered_cell_scalar_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = local_cell_handle[region_id][i];

              (*registered_cell_scalar_data[name])(cell) = (container.second)[i];
            }
          }
          else if (current_registered_region_cell_scalar_data.find(name) != current_registered_region_cell_scalar_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = local_cell_handle[region_id][i];

              (*current_registered_region_cell_scalar_data[name])(cell) = (container.second)[i];
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
              CellType const & cell = local_cell_handle[region_id][i];

              if ( static_cast<typename result_of::id<CellType>::type>(cell_scalar_data[container.first][region_id].size()) <= cell.id())
                cell_scalar_data[container.first][region_id].resize(static_cast<std::size_t>(cell.id()+1));
              cell_scalar_data[container.first][region_id][static_cast<std::size_t>(cell.id())] = (container.second)[i];
            }
          }
        }
        else
        {
          CellVectorOutputFieldContainer & current_registered_region_cell_vector_data = registered_region_cell_vector_data[region_id];
          if (registered_cell_vector_data.find(name) != registered_cell_vector_data.end())
          {
            for (std::size_t i=0; i<container.second.size()/3; ++i)
            {
              CellType const & cell = local_cell_handle[region_id][i];

              (*registered_cell_vector_data[name])(cell).resize(3);
              (*registered_cell_vector_data[name])(cell)[0] = (container.second)[3*i+0];
              (*registered_cell_vector_data[name])(cell)[1] = (container.second)[3*i+1];
              (*registered_cell_vector_data[name])(cell)[2] = (container.second)[3*i+2];
            }
          }
          else if (current_registered_region_cell_vector_data.find(name) != current_registered_region_cell_vector_data.end())
          {
            for (std::size_t i=0; i<container.second.size(); ++i)
            {
              CellType const & cell = local_cell_handle[region_id][i];

              (*current_registered_region_cell_vector_data[name])(cell).resize(3);
              (*current_registered_region_cell_vector_data[name])(cell)[0] = (container.second)[3*i+0];
              (*current_registered_region_cell_vector_data[name])(cell)[1] = (container.second)[3*i+1];
              (*current_registered_region_cell_vector_data[name])(cell)[2] = (container.second)[3*i+2];
            }
          }
          else
          {
            #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
            std::cout << "* vtk_reader::operator(): Reading vector quantity "
                      << container.first << " to vertices." << std::endl;
            #endif
            assert( 3 * viennagrid::cells(region).size() == container.second.size());
            for (std::size_t i=0; i<container.second.size() / 3; ++i)
            {
              CellType const & cell = local_cell_handle[region_id][i];

              if ( static_cast<typename result_of::id<CellType>::type>(cell_vector_data[container.first][region_id].size()) <= cell.id())
                cell_vector_data[container.first][region_id].resize(static_cast<std::size_t>(cell.id()+1));
              cell_vector_data[container.first][region_id][static_cast<std::size_t>(cell.id())].resize(3);
              cell_vector_data[container.first][region_id][static_cast<std::size_t>(cell.id())][0] = (container.second)[3*i+0];
              cell_vector_data[container.first][region_id][static_cast<std::size_t>(cell.id())][1] = (container.second)[3*i+1];
              cell_vector_data[container.first][region_id][static_cast<std::size_t>(cell.id())][2] = (container.second)[3*i+2];
            }
          }
        }
      }

      /** @brief Writes all data read from files to the mesh */
      void setupData(mesh_type & mesh_obj, region_id_type region_id)
      {
        RegionType region = mesh_obj.get_make_region(region_id);

        for (size_t i=0; i<local_scalar_vertex_data[region_id].size(); ++i)
        {
          setupDataVertex(mesh_obj, region_id, local_scalar_vertex_data[region_id][i], 1);
        }

        for (size_t i=0; i<local_vector_vertex_data[region_id].size(); ++i)
        {
          setupDataVertex(mesh_obj, region_id, local_vector_vertex_data[region_id][i], 3);
        }


        for (size_t i=0; i<local_scalar_cell_data[region_id].size(); ++i)
        {
          setupDataCell(mesh_obj, region, region_id, local_scalar_cell_data[region_id][i], 1);
        }

        for (size_t i=0; i<local_vector_cell_data[region_id].size(); ++i)
        {
          setupDataCell(mesh_obj, region, region_id, local_vector_cell_data[region_id][i], 3);
        }

      }

      /** @brief Parses a .vtu file referring to a region of the mesh */
      void parse_vtu_region(std::string filename, region_id_type region_id)
      {

        try
        {
          openFile(filename);

          std::size_t nodeNum = 0;
          std::size_t numberOfComponents = 0;

          xml_tag<> tag;

          tag.parse(reader);
          if (tag.name() != "?xml" && tag.name() != "?xml?")
            throw bad_file_format_exception("* ViennaGrid: vtk_reader::parse_vtu_region(): Parse error: No opening ?xml tag!");

          tag.parse_and_check_name(reader, "vtkfile", filename);
          tag.parse_and_check_name(reader, "unstructuredgrid", filename);

          tag.parse_and_check_name(reader, "piece", filename);

          tag.check_attribute("numberofpoints", filename);

          nodeNum = static_cast<std::size_t>(atoi(tag.get_value("numberofpoints").c_str()));
          #ifdef VIENNAGRID_DEBUG_IO
          std::cout << "#Nodes: " << nodeNum << std::endl;
          #endif

          tag.check_attribute("numberofcells", filename);

          local_cell_num[region_id] = static_cast<std::size_t>(atoi(tag.get_value("numberofcells").c_str()));
          #ifdef VIENNAGRID_DEBUG_IO
          std::cout << "#Cells: " << local_cell_num[region_id] << std::endl;
          #endif

          tag.parse_and_check_name(reader, "points", filename);

          tag.parse_and_check_name(reader, "dataarray", filename);
          tag.check_attribute("numberofcomponents", filename);

          numberOfComponents = static_cast<std::size_t>(atoi(tag.get_value("numberofcomponents").c_str()));
          readNodeCoordinates(nodeNum, numberOfComponents, region_id);

          tag.parse_and_check_name(reader, "/dataarray", filename);
          tag.parse_and_check_name(reader, "/points", filename);

          tag.parse(reader);
          if (tag.name() == "pointdata")
          {
             readPointCellData(region_id, local_scalar_vertex_data, local_vector_vertex_data,
                                      vertex_data_scalar_read, vertex_data_vector_read);
            tag.parse(reader);
          }

          if (tag.name() != "cells")
            throw bad_file_format_exception("* ViennaGrid: vtk_reader::parse_vtu_region(): Parse error: Expected Cells tag!");

          for (std::size_t i=0; i<3; ++i)
          {
            tag.parse_and_check_name(reader, "dataarray", filename);
            tag.check_attribute("name", filename);

            if (tag.get_value("name") == "connectivity")
              readCellIndices(region_id);
            else if (tag.get_value("name") == "offsets")
              readOffsets(region_id);
            else if (tag.get_value("name") == "types")
              readTypes();
            else
              throw bad_file_format_exception("* ViennaGrid: vtk_reader::parse_vtu_region(): Parse error: <DataArray> is not named 'connectivity', 'offsets' or 'types'!");
          }

          tag.parse_and_check_name(reader, "/cells", filename);

          tag.parse(reader);
          if (tag.name() == "celldata")
          {
            readPointCellData(region_id, local_scalar_cell_data, local_vector_cell_data,
                                      cell_data_scalar_read, cell_data_vector_read);
            tag.parse(reader);
          }


          if (tag.name() != "/piece")
            throw bad_file_format_exception("* ViennaGrid: vtk_reader::parse_vtu_region(): Parse error: Expected </Piece> tag!");

          tag.parse_and_check_name(reader, "/unstructuredgrid", filename);
          tag.parse_and_check_name(reader, "/vtkfile", filename);

          closeFile();
        }
        catch (std::exception const & ex) {
          std::cerr << "Problems while reading file " << filename << std::endl;
          std::cerr << "what(): " << ex.what() << std::endl;
        }

      }

      /** @brief Processes a .vtu file that represents a full mesh */
      void process_vtu(std::string const & filename)
      {
        parse_vtu_region(filename, 0);
      }

      /** @brief Processes a .pvd file containing the links to the regions stored in individual .vtu files */
      void process_pvd(std::string const & filename)
      {
        std::map<int, std::string> filenames;

        //extract path from .pvd file:
        std::string::size_type pos = filename.rfind("/");
        std::string path_to_pvd;
        if (pos == std::string::npos)
          pos = filename.rfind("\\"); //a tribute to Windows... ;-)

        if (pos != std::string::npos)
          path_to_pvd = filename.substr(0, pos + 1);

        openFile(filename);

        //
        // Step 1: Get regions from pvd file:
        //
        xml_tag<> tag;

        tag.parse(reader);
        if (tag.name() != "?xml" && tag.name() != "?xml?")
          throw bad_file_format_exception("* ViennaGrid: vtk_reader::process_pvd(): Parse error: No opening <?xml?> tag!");

        tag.parse(reader);
        if (tag.name() != "vtkfile")
          throw bad_file_format_exception("* ViennaGrid: vtk_reader::process_pvd(): Parse error: VTKFile tag expected!");

        if (!tag.has_attribute("type"))
          throw bad_file_format_exception("* ViennaGrid: vtk_reader::process_pvd(): Parse error: VTKFile tag has no attribute 'type'!");

        if (string_to_lower(tag.get_value("type")) != "collection")
          throw bad_file_format_exception("* ViennaGrid: vtk_reader::process_pvd(): Parse error: Type-attribute of VTKFile tag is not 'Collection'!");

        tag.parse(reader);
        if (tag.name() != "collection")
          throw bad_file_format_exception("* ViennaGrid: vtk_reader::process_pvd(): Parse error: Collection tag expected!");

        while (reader.good())
        {
          tag.parse(reader);

          if (tag.name() == "/collection")
            break;

          if (tag.name() != "dataset")
            throw bad_file_format_exception("* ViennaGrid: vtk_reader::process_pvd(): Parse error: DataSet tag expected!");

          if (!tag.has_attribute("file"))
            throw bad_file_format_exception("* ViennaGrid: vtk_reader::process_pvd(): Parse error: DataSet tag has no file attribute!");

          filenames[ atoi(tag.get_value("part").c_str()) ] = tag.get_value("file");

        }

        tag.parse(reader);
        if (tag.name() != "/vtkfile")
          throw bad_file_format_exception("* ViennaGrid: vtk_reader::process_pvd(): Parse error: Closing VTKFile tag expected!");

        closeFile();

        assert(filenames.size() > 0 && "No regions in pvd-file specified!");

        //
        // Step 2: Parse .vtu files:
        //
        for (std::map<int, std::string>::iterator it = filenames.begin(); it != filenames.end(); ++it)
        {
          #if defined VIENNAGRID_DEBUG_ALL || defined VIENNAGRID_DEBUG_IO
          std::cout << "Parsing file " << path_to_pvd + it->second << std::endl;
          #endif
          parse_vtu_region(path_to_pvd + it->second, it->first);
        }

      }


    public:


      ~vtk_reader() { pre_clear(); post_clear(); }


      /** @brief Triggers the read process.
       *
       * @param mesh_obj      The mesh to which the file content is read
       * @param regionation  The regionation to which the file content is read
       * @param filename      Name of the file containing the mesh. Either .pvd (multi-region) or .vtu (single region)
       */
      void operator()(mesh_type & mesh_obj, std::string const & filename)
      {
        geometric_dim = mesh_obj.dimension();
        cell_tag = mesh_obj.cell_tag();
        pre_clear();

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
          std::stringstream ss;
          ss << "* ViennaGrid: " << filename << " - ";
          ss << "Error: vtk-reader does not support file extension: " << extension << "\n";
          ss << "       the vtk-reader supports: \n";
          ss << "       *.vtu -- single-region meshs\n";
          ss << "       *.pvd -- multi-region meshs\n";

          throw bad_file_format_exception(ss.str());
        }

        //
        // push everything to the ViennaGrid mesh:
        //
        setupVertices(mesh_obj);
//         for (size_t region_id = 0; region_id < local_cell_num.size(); ++region_id)
        for (std::map<int, std::size_t>::iterator it = local_cell_num.begin(); it != local_cell_num.end(); ++it)
        {
          mesh_obj.get_make_region( it->first );
        }

//         for (size_t region_id = 0; region_id < local_cell_num.size(); ++region_id)
        for (std::map<int, std::size_t>::iterator it = local_cell_num.begin(); it != local_cell_num.end(); ++it)
        {
          setupCells(mesh_obj, it->first);
          setupData(mesh_obj, it->first);
        }

        post_clear();
      } //operator()




      /** @brief Returns the data names of all scalar vertex data read */
      std::vector<std::string> scalar_vertex_data_names(region_id_type region_id) const
      {
        std::vector<std::string> ret;

        std::map<int, std::deque<std::pair<std::string, std::deque<double> > > >::const_iterator it = local_scalar_vertex_data.find(region_id);
        if (it == local_scalar_vertex_data.end())
          return ret;

        for (std::size_t i=0; i<it->second.size(); ++i)
          ret.push_back(it->second[i].first);

        return ret;
      }

      /** @brief Returns the data names of all vector vertex data read */
      std::vector<std::string> vector_vertex_data_names(region_id_type region_id) const
      {
        std::vector<std::string> ret;

        std::map<int, std::deque<std::pair<std::string, std::deque<double> > > >::const_iterator it = local_vector_vertex_data.find(region_id);
        if (it == local_vector_vertex_data.end())
          return ret;

        for (std::size_t i=0; i<it->second.size(); ++i)
          ret.push_back(it->second[i].first);

        return ret;
      }

      /** @brief Returns the data names of all scalar cell data read */
      std::vector<std::string> scalar_cell_data_names(region_id_type region_id) const
      {
        std::vector<std::string> ret;

        std::map<int, std::deque<std::pair<std::string, std::deque<double> > > >::const_iterator it = local_scalar_cell_data.find(region_id);
        if (it == local_scalar_cell_data.end())
          return ret;

        for (std::size_t i=0; i<it->second.size(); ++i)
          ret.push_back(it->second[i].first);

        return ret;
      }

      /** @brief Returns the data names of all vector cell data read */
      std::vector<std::string> vector_cell_data_names(region_id_type region_id) const
      {
        std::vector<std::string> ret;

        std::map<int, std::deque<std::pair<std::string, std::deque<double> > > >::const_iterator it = local_vector_cell_data.find(region_id);
        if (it == local_vector_cell_data.end())
          return ret;

        for (std::size_t i=0; i<it->second.size(); ++i)
          ret.push_back(it->second[i].first);

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

      template<typename MapType, typename AccessorOrFieldType>
      void register_to_map(MapType & map, AccessorOrFieldType accessor_or_field, std::string const & name)
      {
          typename MapType::iterator it = map.find(name);
          if (it != map.end())
          {
            delete it->second;
            it->second = new dynamic_field_wrapper<AccessorOrFieldType>( accessor_or_field );
          }
          else
            map[name] = new dynamic_field_wrapper<AccessorOrFieldType>( accessor_or_field );
      }


    public:

      /** @brief Registers a vertex scalar accessor/field with a given quantity name */
      template<typename AccessorOrFieldType>
      void register_vertex_scalar(AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_to_map(registered_vertex_scalar_data, accessor_or_field, quantity_name); }

      /** @brief Registers a vertex scalar accessor/field with a given quantity name for a given region ID */
      template<typename AccessorOrFieldType>
      void register_vertex_scalar(region_id_type region_id, AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_to_map(registered_region_vertex_scalar_data[region_id], accessor_or_field, quantity_name); }

      /** @brief Registers a vertex scalar accessor/field with a given quantity name for a given region */
      template<typename AccessorOrFieldType>
      void register_vertex_scalar(RegionType const & region, AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_vertex_scalar(region.id(), accessor_or_field, quantity_name); }


      /** @brief Registers a vertex vector accessor/field with a given quantity name */
      template<typename AccessorOrFieldType>
      void register_vertex_vector(AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_to_map(registered_vertex_vector_data, accessor_or_field, quantity_name); }

      /** @brief Registers a vertex vector accessor/field with a given quantity name for a given region ID */
      template<typename AccessorOrFieldType>
      void register_vertex_vector(region_id_type region_id, AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_to_map(registered_region_vertex_vector_data[region_id], accessor_or_field, quantity_name); }

      /** @brief Registers a vertex vector accessor/field with a given quantity name for a given region */
      template<typename AccessorOrFieldType>
      void register_vertex_vector(RegionType const & region, AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_vertex_vector(region.id(), accessor_or_field, quantity_name); }




      /** @brief Registers a cell scalar accessor/field with a given quantity name */
      template<typename AccessorOrFieldType>
      void register_cell_scalar(AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_to_map(registered_cell_scalar_data, accessor_or_field, quantity_name); }

      /** @brief Registers a cell scalar accessor/field with a given quantity name for a given region ID */
      template<typename AccessorOrFieldType>
      void register_cell_scalar(region_id_type region_id, AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_to_map(registered_region_cell_scalar_data[region_id], accessor_or_field, quantity_name); }

      /** @brief Registers a cell scalar accessor/field with a given quantity name for a given region */
      template<typename AccessorOrFieldType>
      void register_cell_scalar(RegionType const & region, AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_cell_scalar(region.id(), accessor_or_field, quantity_name); }


      /** @brief Registers a cell vector accessor/field with a given quantity name */
      template<typename AccessorOrFieldType>
      void register_cell_vector(AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_to_map(registered_cell_vector_data, accessor_or_field, quantity_name); }

      /** @brief Registers a cell vector accessor/field with a given quantity name for a given region ID */
      template<typename AccessorOrFieldType>
      void register_cell_vector(region_id_type region_id, AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_to_map(registered_region_cell_vector_data[region_id], accessor_or_field, quantity_name); }

      /** @brief Registers a cell vector accessor/field with a given quantity name for a given region */
      template<typename AccessorOrFieldType>
      void register_cell_vector(RegionType const & region, AccessorOrFieldType accessor_or_field, std::string const & quantity_name)
      { register_cell_vector(region.id(), accessor_or_field, quantity_name); }






      /** @brief Returns the vertex scalar field for a given quantity name and a given region ID. If the quantity name was registered before an invalid field is returned. */
      typename viennagrid::result_of::field<const std::deque<double>, VertexType >::type vertex_scalar_field( std::string const & quantity_name, region_id_type region_id ) const
      {
        typename std::map< std::string, std::map<region_id_type, std::deque<double> > >::const_iterator it = vertex_scalar_data.find(quantity_name);
        if (it == vertex_scalar_data.end()) return typename viennagrid::result_of::field<const std::deque<double>, VertexType >::type();

        typename std::map<region_id_type, std::deque<double> >::const_iterator jt = it->second.find( region_id );
        if (jt == it->second.end()) return typename viennagrid::result_of::field<const std::deque<double>, VertexType >::type();

        return viennagrid::make_field<VertexType>( jt->second );
      }

      /** @brief Returns the vertex scalar field for a given quantity name and a given region. If the quantity name was registered before an invalid field is returned. */
      typename viennagrid::result_of::field<const std::deque<double>, VertexType >::type vertex_scalar_field( std::string const & quantity_name, RegionType const & region ) const
      { return vertex_scalar_field(quantity_name, region.id()); }


      /** @brief Returns the vertex vector field for a given quantity name and a given region ID. If the quantity name was registered before an invalid field is returned. */
      typename viennagrid::result_of::field<const std::deque<vector_data_type>, VertexType >::type vertex_vector_field( std::string const & quantity_name, region_id_type region_id ) const
      {
        typename std::map< std::string, std::map<region_id_type, std::deque<vector_data_type> > >::const_iterator it = vertex_vector_data.find(quantity_name);
        if (it == vertex_vector_data.end()) return typename viennagrid::result_of::field<const std::deque<vector_data_type>, VertexType >::type();

        typename std::map<region_id_type, std::deque<vector_data_type> >::const_iterator jt = it->second.find( region_id );
        if (jt == it->second.end()) return typename viennagrid::result_of::field<const std::deque<vector_data_type>, VertexType >::type();

        return viennagrid::make_field<VertexType>( jt->second );
      }

      /** @brief Returns the vertex vector field for a given quantity name and a given region. If the quantity name was registered before an invalid field is returned. */
      typename viennagrid::result_of::field<const std::deque<vector_data_type>, VertexType >::type vertex_vector_field( std::string const & quantity_name, RegionType const & region )
      { return vertex_vector_field(quantity_name, region.id()); }



      /** @brief Returns the cell scalar field for a given quantity name and a given region ID. If the quantity name was registered before an invalid field is returned. */
      typename viennagrid::result_of::field<const std::deque<double>, CellType >::type cell_scalar_field( std::string const & quantity_name, region_id_type region_id ) const
      {
        typename std::map< std::string, std::map<region_id_type, std::deque<double> > >::const_iterator it = cell_scalar_data.find(quantity_name);
        if (it == cell_scalar_data.end()) return typename viennagrid::result_of::field<const std::deque<double>, CellType >::type();

        typename std::map<region_id_type, std::deque<double> >::const_iterator jt = it->second.find( region_id );
        if (jt == it->second.end()) return typename viennagrid::result_of::field<const std::deque<double>, CellType >::type();

        return viennagrid::make_field<CellType>( jt->second );
      }

      /** @brief Returns the cell scalar field for a given quantity name and a given region. If the quantity name was registered before an invalid field is returned. */
      typename viennagrid::result_of::field<const std::deque<double>, CellType >::type cell_scalar_field( std::string const & quantity_name, RegionType const & region ) const
      { return cell_scalar_field(quantity_name, region.id()); }


      /** @brief Returns the cell vector field for a given quantity name and a given region ID. If the quantity name was registered before an invalid field is returned. */
      typename viennagrid::result_of::field<const std::deque<vector_data_type>, CellType >::type cell_vector_field( std::string const & quantity_name, region_id_type region_id ) const
      {
        typename std::map< std::string, std::map<region_id_type, std::deque<vector_data_type> > >::const_iterator it = cell_vector_data.find(quantity_name);
        if (it == cell_vector_data.end()) return typename viennagrid::result_of::field<const std::deque<vector_data_type>, CellType >::type();

        typename std::map<region_id_type, std::deque<vector_data_type> >::const_iterator jt = it->second.find( region_id );
        if (jt == it->second.end()) return typename viennagrid::result_of::field<const std::deque<vector_data_type>, CellType >::type();

        return viennagrid::make_field<CellType>( jt->second );
      }

      /** @brief Returns the cell vector field for a given quantity name and a given region. If the quantity name was registered before an invalid field is returned. */
      typename viennagrid::result_of::field<const std::deque<vector_data_type>, CellType >::type cell_vector_field( std::string const & quantity_name, RegionType const & region ) const
      { return cell_vector_field(quantity_name, region.id()); }


    private:

      // Quantities read:
      std::vector<std::pair<std::size_t, std::string> >         vertex_data_scalar_read;
      std::vector<std::pair<std::size_t, std::string> >         vertex_data_vector_read;

      std::vector<std::pair<std::size_t, std::string> >         cell_data_scalar_read;
      std::vector<std::pair<std::size_t, std::string> >         cell_data_vector_read;


      std::map< std::string, std::map<region_id_type, std::deque<double> > >           vertex_scalar_data;
      std::map< std::string, std::map<region_id_type, std::deque<vector_data_type> > > vertex_vector_data;

      std::map< std::string, std::map<region_id_type, std::deque<double> > >           cell_scalar_data;
      std::map< std::string, std::map<region_id_type, std::deque<vector_data_type> > > cell_vector_data;


      VertexScalarOutputFieldContainer          registered_vertex_scalar_data;
      VertexVectorOutputFieldContainer          registered_vertex_vector_data;

      CellScalarOutputFieldContainer          registered_cell_scalar_data;
      CellVectorOutputFieldContainer          registered_cell_vector_data;

      std::map< region_id_type, VertexScalarOutputFieldContainer > registered_region_vertex_scalar_data;
      std::map< region_id_type, VertexVectorOutputFieldContainer > registered_region_vertex_vector_data;

      std::map< region_id_type, CellScalarOutputFieldContainer >   registered_region_cell_scalar_data;
      std::map< region_id_type, CellVectorOutputFieldContainer >   registered_region_cell_vector_data;

    }; //class vtk_reader


    /** @brief Convenience function for importing a mesh using a single line of code. */
    template <typename MeshType>
    int import_vtk(MeshType & mesh_obj, std::string const & filename)
    {
      vtk_reader<MeshType> vtk_reader;
      return vtk_reader(mesh_obj, filename);
    }





    /** @brief Registers scalar-valued data on vertices at the VTK reader
      *
      * @tparam MeshT             The mesh type to be read to
      * @tparam AccessorOrFieldT    An accessor/field holding scalar data
      * @param  reader              The VTK reader object for which the data should be registered
      * @param  accessor_or_field   The accessor object holding scalar data on vertices
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorOrFieldT>
    vtk_reader<MeshT> & add_scalar_data_on_vertices(vtk_reader<MeshT> & reader,
                                                    AccessorOrFieldT const accessor_or_field,
                                                    std::string const & quantity_name)
    {
      reader.register_vertex_scalar(accessor_or_field, quantity_name);
      return reader;
    }

    /** @brief Registers vector-valued data on vertices at the VTK reader
      *
      * @tparam MeshT             The mesh type to be read to
      * @tparam AccessorOrFieldT    An accessor/field holding vector data
      * @param  reader              The VTK reader object for which the data should be registered
      * @param  accessor_or_field   The accessor object holding vector data on vertices
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorOrFieldT>
    vtk_reader<MeshT> & add_vector_data_on_vertices(vtk_reader<MeshT> & reader,
                                                    AccessorOrFieldT const accessor_or_field,
                                                    std::string const & quantity_name)
    {
      reader.register_vertex_vector(accessor_or_field, quantity_name);
      return reader;
    }





    /** @brief Registers scalar-valued data on cells at the VTK reader
      *
      * @tparam MeshT             The mesh type to be read to
      * @tparam AccessorOrFieldT    An accessor/field holding scalar data
      * @param  reader              The VTK reader object for which the data should be registered
      * @param  accessor_or_field   The accessor object holding scalar data on cells
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorOrFieldT>
    vtk_reader<MeshT> & add_scalar_data_on_cells(vtk_reader<MeshT> & reader,
                                                  AccessorOrFieldT const accessor_or_field,
                                                  std::string const & quantity_name)
    {
      reader.register_cell_scalar(accessor_or_field, quantity_name);
      return reader;
    }

    /** @brief Registers vector-valued data on cells at the VTK reader
      *
      * @tparam MeshT             The mesh type to be read to
      * @tparam AccessorOrFieldT    An accessor/field holding vector data
      * @param  reader              The VTK reader object for which the data should be registered
      * @param  accessor_or_field   The accessor object holding vector data on cells
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorOrFieldT>
    vtk_reader<MeshT> & add_vector_data_on_cells(vtk_reader<MeshT> & reader,
                                                  AccessorOrFieldT const accessor_or_field,
                                                  std::string const & quantity_name)
    {
      reader.register_cell_vector(accessor_or_field, quantity_name);
      return reader;
    }




    /** @brief Returns the names of all scalar-valued data read for vertices */
    template<typename VTKReaderT>
    std::vector<std::pair<std::size_t, std::string> > const & get_scalar_data_on_vertices(VTKReaderT const & reader)
    {
      return reader.get_scalar_data_on_vertices();
    }

    /** @brief Returns the names of all vector-valued data read for vertices */
    template<typename VTKReaderT>
    std::vector<std::pair<std::size_t, std::string> > const & get_vector_data_on_vertices(VTKReaderT const & reader)
    {
      return reader.get_vector_data_on_vertices();
    }

    /** @brief Returns the names of all scalar-valued data read for cells */
    template<typename VTKReaderT>
    std::vector<std::pair<std::size_t, std::string> > const & get_scalar_data_on_cells(VTKReaderT const & reader)
    {
      return reader.get_scalar_data_on_cells();
    }

    /** @brief Returns the names of all vector-valued data read for cells */
    template<typename VTKReaderT>
    std::vector<std::pair<std::size_t, std::string> > const & get_vector_data_on_cells(VTKReaderT const & reader)
    {
      return reader.get_vector_data_on_cells();
    }



  } //namespace io
} //namespace viennagrid

#endif