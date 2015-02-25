#ifndef VIENNAGRID_IO_VTK_WRITER_HPP
#define VIENNAGRID_IO_VTK_WRITER_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <set>

#include "viennagrid/core.hpp"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/vtk_common.hpp"
#include "viennagrid/accessor.hpp"

/** @file viennagrid/io/vtk_writer.hpp
    @brief Provides a writer to VTK files
*/

namespace viennagrid
{
  namespace io
  {

    template<typename ElementType>
    struct ValueTypeInformation;

    template<>
    struct ValueTypeInformation<double>
    {
      typedef double value_type;

      static std::string type_name() { return "Float32"; }
      static int num_components() { return 1; }
      static void write( std::ostream & os, value_type value )
      {
        os << value;
      }
    };

    template<>
    struct ValueTypeInformation< std::vector<double> >
    {
      typedef std::vector<double> value_type;

      static std::string type_name() { return "Float32"; }
      static int num_components() { return 3; }
      static void write( std::ostream & os, value_type const & value )
      {
        os << value[0] << " " << value[1] << " " << value[2];
      }
    };


    /////////////////// VTK export ////////////////////////////

    //helper: translate element tags to VTK-element types
    // (see: http://www.vtk.org/VTK/img/file-formats.pdf, page 9)

    /** @brief Main VTK writer class. Writes a mesh or a region to a file
     *
     * @tparam MeshType         Type of the ViennaGrid mesh. Must not be a region!
     * @tparam SegmentationType   Type of the ViennaGrid regionation. Default is the default regionation of MeshType
     */
    template<typename MeshType>
    class vtk_writer
    {
    protected:

      typedef MeshType mesh_type;
      typedef typename mesh_type::region_id_type region_id_type;

      typedef typename result_of::region_range<mesh_type>::type RegionRangeType;

      typedef typename result_of::point<mesh_type>::type   PointType;
      typedef typename result_of::coord<PointType>::type  CoordType;

      typedef typename result_of::element<mesh_type>::type           ElementType;
      typedef typename result_of::id<ElementType>::type                         CellIDType;

      typedef typename result_of::id<ElementType>::type                           VertexIDType;

      typedef typename mesh_type::const_region_type RegionType;


      typedef std::vector<viennagrid_numeric> vector_data_type;

      typedef base_dynamic_accessor<viennagrid_numeric, ElementType> BaseScalarAccessorType;
      typedef base_dynamic_accessor<vector_data_type, ElementType> BaseVectorAccessorType;

//       typedef base_dynamic_accessor<double, ElementType> VertexScalarBaseAccesor;
      typedef std::map< std::string, BaseScalarAccessorType * > VertexScalarOutputAccessorContainer;

//       typedef base_dynamic_accessor<vector_data_type, ElementType> VertexVectorBaseAccesor;
      typedef std::map< std::string, BaseVectorAccessorType * > VertexVectorOutputAccessorContainer;

//       typedef base_dynamic_accessor<double, ElementType> CellScalarBaseAccesor;
      typedef std::map< std::string, BaseScalarAccessorType * > CellScalarOutputAccessorContainer;

//       typedef base_dynamic_accessor<vector_data_type, ElementType> CellVectorBaseAccesor;
      typedef std::map< std::string, BaseVectorAccessorType * > CellVectorOutputAccessorContainer;

    protected:


      template<typename map_type>
      void clear_map( map_type & map )
      {
        for (typename map_type::iterator it = map.begin(); it != map.end(); ++it)
          delete it->second;

        map.clear();
      }

      void clear()
      {
        clear_map(vertex_scalar_data);
        clear_map(vertex_vector_data);

        clear_map(cell_scalar_data);
        clear_map(cell_vector_data);


        for (typename std::map< region_id_type, VertexScalarOutputAccessorContainer>::iterator it = region_vertex_scalar_data.begin(); it != region_vertex_scalar_data.end(); ++it)
          clear_map(it->second);

        for (typename std::map< region_id_type, VertexVectorOutputAccessorContainer>::iterator it = region_vertex_vector_data.begin(); it != region_vertex_vector_data.end(); ++it)
          clear_map(it->second);


        for (typename std::map< region_id_type, CellScalarOutputAccessorContainer>::iterator it = region_cell_scalar_data.begin(); it != region_cell_scalar_data.end(); ++it)
          clear_map(it->second);

        for (typename std::map< region_id_type, CellVectorOutputAccessorContainer>::iterator it = region_cell_vector_data.begin(); it != region_cell_vector_data.end(); ++it)
          clear_map(it->second);


        region_vertex_scalar_data.clear();
        region_vertex_vector_data.clear();

        region_cell_scalar_data.clear();
        region_cell_vector_data.clear();


        used_vertex_map.clear();
        vertex_to_index_map.clear();
        used_cell_map.clear();
      }

      /** @brief Writes the XML file header */
      void writeHeader(std::ofstream & writer)
      {
        writer << "<?xml version=\"1.0\"?>" << std::endl;
        writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
        writer << " <UnstructuredGrid>" << std::endl;
      }


      template<typename MeshRegionT>
      std::size_t preparePoints(MeshRegionT const & region, region_id_type region_id)
      {
        typedef typename viennagrid::result_of::const_element_range<MeshRegionT>::type    CellRangeType;
        typedef typename viennagrid::result_of::iterator<CellRangeType>::type             CellRangeIterator;

        typedef typename viennagrid::result_of::const_element_range<ElementType>::type    VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;

        std::map< VertexIDType, ElementType > & current_used_vertex_map = used_vertex_map[region_id];
        std::map< ElementType, VertexIDType > & current_vertex_to_index_map = vertex_to_index_map[region_id];


        CellRangeType cells(region, viennagrid::topologic_dimension(region));
        for (CellRangeIterator it = cells.begin(); it != cells.end(); ++it)
        {
          VertexOnCellRange vertices_on_cell(*it, 0);
          for (VertexOnCellIterator jt = vertices_on_cell.begin(); jt != vertices_on_cell.end(); ++jt)
          {
              typename std::map< VertexIDType, ElementType >::iterator kt = current_used_vertex_map.find( (*jt).id() );
              if (kt == current_used_vertex_map.end())
                current_used_vertex_map.insert( std::make_pair((*jt).id(), *jt) );


//                   typename std::map< ConstVertexType, VertexIDType >::iterator kt = current_vertex_to_index_map.find( jt.handle() );
//                   if (kt == current_vertex_to_index_map.end())
//                   {
// //                       current_vertex_to_index_map.insert( std::make_pair( jt.handle(), jt->id() ) );
//                       current_vertex_to_index_map.insert( std::make_pair( jt.handle(), index ) );
// //                       current_used_vertex_map[ index ] = jt.handle();
//                       current_used_vertex_map[ jt->id() ] = jt.handle();
//                       index++;
//                   }
          }
        }

        VertexIDType index = 0;
        for (typename std::map< VertexIDType, ElementType >::iterator it = current_used_vertex_map.begin(); it != current_used_vertex_map.end(); ++it)
          current_vertex_to_index_map.insert( std::make_pair( it->second, index++ ) );

        return current_vertex_to_index_map.size();
      }

      template<typename MeshRegionT>
      std::size_t prepareCells(MeshRegionT const & domseg, region_id_type region_id)
      {
        typedef typename viennagrid::result_of::const_element_range<MeshRegionT>::type     CellRange;
        typedef typename viennagrid::result_of::iterator<CellRange>::type                                         CellIterator;

        std::set< ElementType > & current_used_cells_map = used_cell_map[region_id];

        CellRange cells(domseg, topologic_dimension(domseg));
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
            {
              current_used_cells_map.insert(*cit);
            }

        return current_used_cells_map.size();
      }

      /** @brief Writes the vertices in the mesh */
      template <typename RegionT>
      void writePoints(RegionT const & domseg, std::ofstream & writer, region_id_type region_id)
      {
        std::map< VertexIDType, ElementType > & current_used_vertex_map = used_vertex_map[region_id];

        writer << "   <Points>" << std::endl;
        writer << "    <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

        for (typename std::map< VertexIDType, ElementType >::iterator it = current_used_vertex_map.begin(); it != current_used_vertex_map.end(); ++it)
        {
          const int dim = viennagrid::geometric_dimension(domseg);
          PointWriter::write(writer, viennagrid::get_point(domseg, it->second) );

          // add 0's for less than three dimensions
          for (int i = dim; i < 3; ++i)
            writer << " " << 0;

          writer << std::endl;
        }
        writer << std::endl;
        writer << "    </DataArray>" << std::endl;
        writer << "   </Points> " << std::endl;
      } //writePoints()

      /** @brief Writes the cells to the mesh */
      template <typename MeshRegionT>
      void writeCells(MeshRegionT const &, std::ofstream & writer, region_id_type region_id)
      {
        typedef typename viennagrid::result_of::const_element_range<ElementType>::type    VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;

        std::map< ElementType, VertexIDType > & current_vertex_to_index_map = vertex_to_index_map[region_id];

        writer << "   <Cells> " << std::endl;
        writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;

        std::set< ElementType > & current_used_cells_map = used_cell_map[region_id];
        for (typename std::set< ElementType >::iterator it = current_used_cells_map.begin(); it != current_used_cells_map.end(); ++it)

        {
          //step 1: Write vertex indices in ViennaGrid orientation to array:
//           ElementType const & cell = it->second; //viennagrid::dereference_handle(domseg, it->second);
//             ElementType const & cell = *cit;


          VertexOnCellRange vertices_on_cell(*it, 0);
          std::vector<VertexIDType> viennagrid_vertices( vertices_on_cell.size() );
          std::size_t j = 0;
          for (VertexOnCellIterator vocit = vertices_on_cell.begin();
              vocit != vertices_on_cell.end();
              ++vocit, ++j)
          {
            viennagrid_vertices[j] = current_vertex_to_index_map[*vocit];
          }

          //Step 2: Write the transformed connectivities:
//           detail::viennagrid_to_vtk_orientations<CellTag> reorderer;
          detail::viennagrid_to_vtk_orientations reorderer( (*it).tag() );
          for (std::size_t i=0; i<viennagrid_vertices.size(); ++i)
            writer << viennagrid_vertices[reorderer(i)] << " ";

          writer << std::endl;
        }

        writer << std::endl;
        writer << "    </DataArray>" << std::endl;

        writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;

        std::size_t offset = 0;
        for (typename std::set< ElementType >::iterator it = current_used_cells_map.begin(); it != current_used_cells_map.end(); ++it)
        {
          offset += viennagrid::vertices(*it).size();
          writer << offset << " ";
        }

//         for (element_tag_t cell_tag = cell_tag_begin(domseg); cell_tag != cell_tag_end(domseg); ++cell_tag)
//         {
//           for (std::size_t i = 1;
//                 //offsets <= current_used_cells_map.size();
//                 i <= viennagrid::elements(domseg, cell_tag).size();
//                 ++i)
//           {
//             offset += i *
//             writer << ( offsets * cell_tag.vertex_count() ) << " ";
//           }
//         }
        writer << std::endl;
        writer << "    </DataArray>" << std::endl;

        writer << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
        for (typename std::set< ElementType >::iterator it = current_used_cells_map.begin(); it != current_used_cells_map.end(); ++it)
        {
          offset += viennagrid::vertices(*it).size();
          writer << vtk_element_tag( (*it).tag() ) << " ";
        }

//         for (element_tag_t cell_tag = cell_tag_begin(domseg); cell_tag != cell_tag_end(domseg); ++cell_tag)
//         {
//           for (std::size_t offsets = 1;
//                 //offsets <= current_used_cells_map.size();
//               offsets <= viennagrid::elements(domseg, cell_tag).size();
//                 ++offsets)
//           {
//             writer << vtk_element_tag(cell_tag) << " ";
//   //           writer << detail::ELEMENT_TAG_TO_VTK_TYPE<CellTag>::value << " ";
//           }
//         }
        writer << std::endl;
        writer << "    </DataArray>" << std::endl;
        writer << "   </Cells>" << std::endl;
      }








      /** @brief Writes vector-valued data defined on vertices (points) to file */
      template <typename RegionT, typename IOAccessorType>
      void writePointData(RegionT const &, std::ofstream & writer, std::string const & name, IOAccessorType const & accessor, region_id_type region_id)
      {
        typedef typename IOAccessorType::value_type ValueType;

        writer << "    <DataArray type=\"" << ValueTypeInformation<ValueType>::type_name() << "\" Name=\"" << name <<
          "\" NumberOfComponents=\"" << ValueTypeInformation<ValueType>::num_components() << "\" format=\"ascii\">" << std::endl;

        std::map< VertexIDType, ElementType > & current_used_vertex_map = used_vertex_map[region_id];
        for (typename std::map< VertexIDType, ElementType >::iterator it = current_used_vertex_map.begin(); it != current_used_vertex_map.end(); ++it)
        {
          ValueTypeInformation<ValueType>::write(writer, accessor.get(it->second));
          writer << " ";
        }
        writer << std::endl;

        writer << "    </DataArray>" << std::endl;
      } //writePointDataScalar


      /** @brief Writes vector-valued data defined on vertices (points) to file */
      template <typename RegionT, typename IOAccessorType>
      void writeCellData(RegionT const &, std::ofstream & writer, std::string const & name, IOAccessorType const & accessor, region_id_type region_id)
      {
        typedef typename IOAccessorType::value_type ValueType;

        writer << "    <DataArray type=\"" << ValueTypeInformation<ValueType>::type_name() << "\" Name=\"" << name <<
          "\" NumberOfComponents=\"" << ValueTypeInformation<ValueType>::num_components() << "\" format=\"ascii\">" << std::endl;


        std::set< ElementType > & current_used_cells_map = used_cell_map[region_id];
        for (typename std::set< ElementType >::iterator it = current_used_cells_map.begin(); it != current_used_cells_map.end(); ++it)

        {
          //step 1: Write vertex indices in ViennaGrid orientation to array:
//           ElementType const & cell = viennagrid::dereference_handle(region, it->second);
//             ElementType const & cell = *cit;

          ValueTypeInformation<ValueType>::write(writer, accessor.get(*it));
          writer << " ";
        }
        writer << std::endl;

        writer << "    </DataArray>" << std::endl;
      } //writePointDataScalar



      /** @brief Writes the XML footer */
      void writeFooter(std::ofstream & writer)
      {
        writer << " </UnstructuredGrid>" << std::endl;
        writer << "</VTKFile>" << std::endl;
      }

    public:


      ~vtk_writer() { clear(); }

      /** @brief Triggers the write process to a XML file. Make sure that all data to be written to the file is already passed to the writer
       *
       * @param mesh_obj   The ViennaGrid mesh.
       * @param filename   The file to write to
       */
      void operator()(MeshType const & mesh_obj, std::string const & filename)
      {
        if (mesh_obj.region_count() <= 1)
        {
          std::stringstream ss;
          ss << filename << ".vtu";
          std::ofstream writer(ss.str().c_str());

          if (!writer)
            throw cannot_open_file_exception("* ViennaGrid: vtk_writer::operator(): File " + filename + ": Cannot open file!");

          writeHeader(writer);

          region_id_type tmp_id = region_id_type();

          std::size_t num_points = preparePoints(mesh_obj, tmp_id);
          prepareCells(mesh_obj, tmp_id);

          std::size_t cell_count = viennagrid::elements(mesh_obj, topologic_dimension(mesh_obj)).size();
//           for (element_tag_t cell_tag = cell_tag_begin(mesh_obj); cell_tag != cell_tag_end(mesh_obj); ++cell_tag)
//             cell_count += viennagrid::elements(mesh_obj, topologic_dimension(mesh_obj)).size();

          writer << "  <Piece NumberOfPoints=\""
                << num_points
                << "\" NumberOfCells=\""
                << cell_count //viennagrid::cells(mesh_obj).size()
                << "\">" << std::endl;

          writePoints(mesh_obj, writer, tmp_id);

          if (vertex_scalar_data.size() > 0 || vertex_vector_data.size() > 0)
          {
            writer << "   <PointData>" << std::endl;

              for (typename VertexScalarOutputAccessorContainer::const_iterator it = vertex_scalar_data.begin(); it != vertex_scalar_data.end(); ++it)
                writePointData( mesh_obj, writer, it->first, *(it->second), tmp_id );
              for (typename VertexVectorOutputAccessorContainer::const_iterator it = vertex_vector_data.begin(); it != vertex_vector_data.end(); ++it)
                writePointData( mesh_obj, writer, it->first, *(it->second), tmp_id );

            writer << "   </PointData>" << std::endl;
          }

          writeCells(mesh_obj, writer, tmp_id);
          if (cell_scalar_data.size() > 0 || cell_vector_data.size() > 0)
          {
            writer << "   <CellData>" << std::endl;

              for (typename CellScalarOutputAccessorContainer::const_iterator it = cell_scalar_data.begin(); it != cell_scalar_data.end(); ++it)
                writeCellData( mesh_obj, writer, it->first, *(it->second), tmp_id );
              for (typename CellVectorOutputAccessorContainer::const_iterator it = cell_vector_data.begin(); it != cell_vector_data.end(); ++it)
                writeCellData( mesh_obj, writer, it->first, *(it->second), tmp_id );

            writer << "   </CellData>" << std::endl;
          }

          writer << "  </Piece>" << std::endl;
          writeFooter(writer);

          clear();
          return;
        }

        //
        // Step 1: Write meta information
        //
        {
          std::stringstream ss;
          ss << filename << ".pvd";
          std::ofstream writer(ss.str().c_str());

          std::string short_filename = filename;
          std::string::size_type pos = filename.rfind("/");
          if (pos == std::string::npos)
            pos = filename.rfind("\\");   //A tribute to Windows

          if (pos != std::string::npos)
            short_filename = filename.substr(pos+1, filename.size());

          if (!writer){
            clear();
            throw cannot_open_file_exception("* ViennaGrid: vtk_writer::operator(): File " + filename + ": Cannot open file!");
          }

          writer << "<?xml version=\"1.0\"?>" << std::endl;
          writer << "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">" << std::endl;
          writer << "<Collection>" << std::endl;

          RegionRangeType regions(mesh_obj);
          for (typename RegionRangeType::iterator rit = regions.begin(); rit != regions.end(); ++rit)
          {
            writer << "    <DataSet part=\"" << (*rit).id() << "\" file=\"" << short_filename << "_" << (*rit).id() << ".vtu\" name=\"" << (*rit).id() << "\"/>" << std::endl;
          }

          writer << "  </Collection>" << std::endl;
          writer << "</VTKFile>" << std::endl;
          writer.close();
        }

        //
        // Step 2: Write regions to individual files
        //

        RegionRangeType regions(mesh_obj);
        for (typename RegionRangeType::iterator rit = regions.begin(); rit != regions.end(); ++rit)
        {
          RegionType region = *rit;

          std::stringstream ss;
          ss << filename << "_" << region.id() << ".vtu";
          std::ofstream writer(ss.str().c_str());
          writeHeader(writer);

          if (!writer)
          {
            clear();
            throw cannot_open_file_exception("* ViennaGrid: vtk_writer::operator(): File " + ss.str() + ": Cannot open file!");
          }

          std::size_t num_points = preparePoints(region, region.id());
          prepareCells(region, region.id());

          std::size_t cell_count = viennagrid::elements(region, topologic_dimension(region)).size();
//           std::size_t cell_count = 0;
//           for (element_tag_t cell_tag = cell_tag_begin(region); cell_tag != cell_tag_end(region); ++cell_tag)
//             cell_count += viennagrid::elements(region, cell_tag).size();

          writer << "  <Piece NumberOfPoints=\""
                << num_points
                << "\" NumberOfCells=\""
                << cell_count //viennagrid::cells(region).size()
                << "\">" << std::endl;

          writePoints(region, writer, region.id());


          VertexScalarOutputAccessorContainer const & current_region_vertex_scalar_data = region_vertex_scalar_data[ region.id() ];
          VertexVectorOutputAccessorContainer const & current_region_vertex_vector_data = region_vertex_vector_data[ region.id() ];

          if (vertex_scalar_data.size() > 0 || vertex_vector_data.size() > 0 || current_region_vertex_scalar_data.size() > 0 || current_region_vertex_vector_data.size() > 0)
          {
            writer << "   <PointData>" << std::endl;

            for (typename VertexScalarOutputAccessorContainer::const_iterator data_it = vertex_scalar_data.begin(); data_it != vertex_scalar_data.end(); ++data_it)
            {
              writePointData( region, writer, data_it->first, *(data_it->second), region.id() );
            }
            for (typename VertexVectorOutputAccessorContainer::const_iterator data_it = vertex_vector_data.begin(); data_it != vertex_vector_data.end(); ++data_it)
            {
              writePointData( region, writer, data_it->first, *(data_it->second), region.id() );
            }


            for (typename VertexScalarOutputAccessorContainer::const_iterator data_it = current_region_vertex_scalar_data.begin(); data_it != current_region_vertex_scalar_data.end(); ++data_it)
            {
              writePointData( region, writer, data_it->first, *(data_it->second), region.id() );
            }

            for (typename VertexVectorOutputAccessorContainer::const_iterator data_it = current_region_vertex_vector_data.begin(); data_it != current_region_vertex_vector_data.end(); ++data_it)
            {
              writePointData( region, writer, data_it->first, *(data_it->second), region.id() );
            }

            writer << "   </PointData>" << std::endl;
          }

          writeCells(region, writer, region.id());

          CellScalarOutputAccessorContainer const & current_region_cell_scalar_data = region_cell_scalar_data[ region.id() ];
          CellVectorOutputAccessorContainer const & current_region_cell_vector_data = region_cell_vector_data[ region.id() ];
          if (cell_scalar_data.size() > 0 || cell_vector_data.size() > 0 || current_region_cell_scalar_data.size() > 0 || current_region_cell_vector_data.size() > 0)
          {
            writer << "   <CellData>" << std::endl;

            for (typename CellScalarOutputAccessorContainer::const_iterator data_it = cell_scalar_data.begin(); data_it != cell_scalar_data.end(); ++data_it)
              writeCellData( region, writer, data_it->first, *(data_it->second), region.id() );
            for (typename CellVectorOutputAccessorContainer::const_iterator data_it = cell_vector_data.begin(); data_it != cell_vector_data.end(); ++data_it)
              writeCellData( region, writer, data_it->first, *(data_it->second), region.id() );


            for (typename CellScalarOutputAccessorContainer::const_iterator data_it = current_region_cell_scalar_data.begin(); data_it != current_region_cell_scalar_data.end(); ++data_it)
              writeCellData( region, writer, data_it->first, *(data_it->second), region.id() );
            for (typename CellVectorOutputAccessorContainer::const_iterator data_it = current_region_cell_vector_data.begin(); data_it != current_region_cell_vector_data.end(); ++data_it)
              writeCellData( region, writer, data_it->first, *(data_it->second), region.id() );

            writer << "   </CellData>" << std::endl;
          }

          writer << "  </Piece>" << std::endl;
          writeFooter(writer);
          writer.close();
        }

        clear();
      }


    private:


      template<typename ValueT, typename MapT, typename AccessorT>
      void add_to_container(MapT & map, AccessorT const & accessor, std::string const & quantity_name)
      {
        typename MapT::iterator it = map.find(quantity_name);
        if (it != map.end())
        {
          delete it->second;
          it->second = new dynamic_accessor_wrapper<base_dynamic_accessor<ValueT, ElementType>,
                                                    AccessorT>( accessor );
        }
        else
          map[quantity_name] = new dynamic_accessor_wrapper<base_dynamic_accessor<ValueT, ElementType>,
                                                            AccessorT>( accessor );
      }

      template<typename ValueT, typename MapT, typename AccessorT>
      void add_to_vertex_container(MapT & map, AccessorT const & accessor, std::string const & quantity_name)
      {
        add_to_container<ValueT>(map, accessor, quantity_name);
      }

      template<typename ValueT, typename MapT, typename AccessorT>
      void add_to_cell_container(MapT & map, AccessorT const & accessor, std::string const & quantity_name)
      {
        add_to_container<ValueT>(map, accessor, quantity_name);
      }

    public:


      /** @brief Register an accessor/field for scalar data on vertices with a given quantity name */
      template <typename AccessorT>
      void add_scalar_data_on_vertices(AccessorT const & accessor, std::string const & quantity_name)
      { add_to_vertex_container<viennagrid_numeric>(vertex_scalar_data, accessor, quantity_name); }

      /** @brief Register an accessor/field for scalar data on vertices for a given region ID with a given quantity name */
      template <typename AccessorT>
      void add_scalar_data_on_vertices(region_id_type region_id, AccessorT const & accessor, std::string const & quantity_name)
      { add_to_vertex_container<viennagrid_numeric>(region_vertex_scalar_data[region_id], accessor, quantity_name); }

      /** @brief Register an accessor/field for scalar data on vertices for a given region with a given quantity name */
      template <typename AccessorT>
      void add_scalar_data_on_vertices(RegionType const & region, AccessorT const & accessor, std::string const & quantity_name)
      { add_scalar_data_on_vertices(region.id(), accessor, quantity_name); }


      /** @brief Register an accessor/field for vector data on vertices with a given quantity name */
      template <typename AccessorType>
      void add_vector_data_on_vertices(AccessorType const & accessor, std::string const & quantity_name)
      { add_to_vertex_container<vector_data_type>(vertex_vector_data, accessor, quantity_name); }

      /** @brief Register an accessor/field for vector data on vertices for a given region ID with a given quantity name */
      template <typename AccessorType>
      void add_vector_data_on_vertices(region_id_type region_id, AccessorType const & accessor, std::string const & quantity_name)
      { add_to_vertex_container<vector_data_type>(region_vertex_vector_data[region_id], accessor, quantity_name); }

      /** @brief Register an accessor/field for vector data on vertices for a given region with a given quantity name */
      template <typename AccessorType>
      void add_vector_data_on_vertices(RegionType const & region, AccessorType const & accessor, std::string const & quantity_name)
      { add_vector_data_on_vertices(region.id(), accessor, quantity_name); }



      /** @brief Register an accessor/field for scalar data on cells with a given quantity name */
      template <typename AccessorType>
      void add_scalar_data_on_cells(AccessorType const accessor, std::string const & quantity_name)
      { add_to_cell_container<viennagrid_numeric>(cell_scalar_data, accessor, quantity_name); }

      /** @brief Register an accessor/field for scalar data on cells for a given region ID with a given quantity name */
      template <typename AccessorType>
      void add_scalar_data_on_cells(region_id_type region_id, AccessorType const & accessor, std::string const & quantity_name)
      { add_to_cell_container<viennagrid_numeric>(region_cell_scalar_data[region_id], accessor, quantity_name); }

      /** @brief Register an accessor/field for scalar data on cells for a given region with a given quantity name */
      template <typename AccessorType>
      void add_scalar_data_on_cells(RegionType const & region, AccessorType const & accessor, std::string const & quantity_name)
      { add_scalar_data_on_cells(region.id(), accessor, quantity_name); }


      /** @brief Register an accessor/field for vector data on cells with a given quantity name */
      template <typename AccessorType>
      void add_vector_data_on_cells(AccessorType const & accessor, std::string const & quantity_name)
      { add_to_cell_container<vector_data_type>(cell_vector_data, accessor, quantity_name); }

      /** @brief Register an accessor/field for vector data on cells for a given region ID with a given quantity name */
      template <typename AccessorType>
      void add_vector_data_on_cells(region_id_type region_id, AccessorType const & accessor, std::string const & quantity_name)
      { add_to_cell_container<vector_data_type>(region_cell_vector_data[region_id], accessor, quantity_name); }

      /** @brief Register an accessor/field for vector data on cells for a given region with a given quantity name */
      template <typename AccessorType>
      void add_vector_data_on_cells(RegionType const & region, AccessorType const & accessor, std::string const & quantity_name)
      { add_vector_data_on_cells(region.id(), accessor, quantity_name); }


    private:

      std::map< region_id_type, std::map< ElementType, VertexIDType> >             vertex_to_index_map;
      std::map< region_id_type, std::map< VertexIDType, ElementType> >             used_vertex_map;
      std::map< region_id_type, std::set< ElementType> >                           used_cell_map;


      VertexScalarOutputAccessorContainer          vertex_scalar_data;
      VertexVectorOutputAccessorContainer          vertex_vector_data;

      CellScalarOutputAccessorContainer          cell_scalar_data;
      CellVectorOutputAccessorContainer          cell_vector_data;

      std::map< region_id_type, VertexScalarOutputAccessorContainer > region_vertex_scalar_data;
      std::map< region_id_type, VertexVectorOutputAccessorContainer > region_vertex_vector_data;

      std::map< region_id_type, CellScalarOutputAccessorContainer >   region_cell_scalar_data;
      std::map< region_id_type, CellVectorOutputAccessorContainer >   region_cell_vector_data;
    };

    /** @brief Convenience function that exports a mesh to file directly. Does not export quantities */
    template < typename MeshType, typename SegmentationType >
    int export_vtk(MeshType const & mesh_obj, SegmentationType const & regionation, std::string const & filename)
    {
      vtk_writer<MeshType> vtk_writer;
      return vtk_writer(mesh_obj, regionation, filename);
    }

    /** @brief Convenience function that exports a mesh to file directly. Does not export quantities */
    template < typename MeshType >
    int export_vtk(MeshType const & mesh_obj, std::string const & filename)
    {
      vtk_writer<MeshType> vtk_writer;
      return vtk_writer(mesh_obj, filename);
    }


    /** @brief Registers scalar-valued data on vertices at the VTK writer. At most one data set is allowed.
      *
      * @tparam MeshT             The mesh type to be written
      * @tparam AccessorT    An accessor/field type holding scalar data
      * @param  writer              The VTK writer object for which the data should be registered
      * @param  accessor   The accessor/field object holding scalar data on vertices
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorT>
    vtk_writer<MeshT> & add_scalar_data_on_vertices(vtk_writer<MeshT> & writer,
                                                    AccessorT const accessor,
                                                    std::string const & quantity_name)
    {
      writer.add_scalar_data_on_vertices(accessor, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data on vertices at the VTK writer. At most one data set is allowed.
      *
      * @tparam MeshT             The mesh type to be written
      * @tparam AccessorT    An accessor/field type holding vector data
      * @param  writer              The VTK writer object for which the data should be registered
      * @param  accessor   The accessor/field object holding vector data on vertices
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorT>
    vtk_writer<MeshT> & add_vector_data_on_vertices(vtk_writer<MeshT> & writer,
                                                    AccessorT const accessor,
                                                    std::string const & quantity_name)
    {
      writer.add_vector_data_on_vertices(accessor, quantity_name);
      return writer;
    }

    /** @brief Registers scalar-valued data on vertices for a given region at the VTK writer. At most one data set is allowed.
      *
      * @tparam MeshT             The mesh type to be written
      * @tparam AccessorT    An accessor/field type holding scalar data
      * @param  writer              The VTK writer object for which the data should be registered
      * @param  region             The region for which the data is defined
      * @param  accessor   The accessor/field object holding scalar data on vertices
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorT>
    vtk_writer<MeshT> & add_scalar_data_on_vertices(vtk_writer<MeshT> & writer,
                                                    typename MeshT::region_type const & region,
                                                    AccessorT const accessor,
                                                    std::string const & quantity_name)
    {
      writer.add_scalar_data_on_vertices(region, accessor, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data on vertices for a given region at the VTK writer. At most one data set is allowed.
      *
      * @tparam MeshT             The mesh type to be written
      * @tparam SegmentationT       The regionation type to be written
      * @tparam AccessorT    An accessor/field type holding vector data
      * @param  writer              The VTK writer object for which the data should be registered
      * @param  region             The region for which the data is defined
      * @param  accessor   The accessor/field object holding vector data on vertices
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorT>
    vtk_writer<MeshT> & add_vector_data_on_vertices(vtk_writer<MeshT> & writer,
                                                    typename MeshT::region_type const & region,
                                                    AccessorT const accessor,
                                                    std::string const & quantity_name)
    {
      writer.add_vector_data_on_vertices(region, accessor, quantity_name);
      return writer;
    }





    /** @brief Registers scalar-valued data on cells at the VTK writer. At most one data set is allowed.
      *
      * @tparam MeshT             The mesh type to be written
      * @tparam AccessorT    An accessor/field type holding scalar data
      * @param  writer              The VTK writer object for which the data should be registered
      * @param  accessor   The accessor/field object holding scalar data on cells
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorT>
    vtk_writer<MeshT> & add_scalar_data_on_cells(vtk_writer<MeshT> & writer,
                                                 AccessorT const accessor,
                                                 std::string const & quantity_name)
    {
      writer.add_scalar_data_on_cells(accessor, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data on cells at the VTK writer. At most one data set is allowed.
      *
      * @tparam MeshT             The mesh type to be written
      * @tparam AccessorT    An accessor/field type holding vector data
      * @param  writer              The VTK writer object for which the data should be registered
      * @param  accessor   The accessor/field object holding vector data on cells
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorT>
    vtk_writer<MeshT> & add_vector_data_on_cells(vtk_writer<MeshT> & writer,
                                                 AccessorT const accessor,
                                                 std::string const & quantity_name)
    {
      writer.add_vector_data_on_cells(accessor, quantity_name);
      return writer;
    }

    /** @brief Registers scalar-valued data on cells for a given region at the VTK writer. At most one data set is allowed.
      *
      * @tparam MeshT             The mesh type to be written
      * @tparam AccessorT    An accessor/field type holding scalar data
      * @param  writer              The VTK writer object for which the data should be registered
      * @param  region             The region for which the data is defined
      * @param  accessor   The accessor/field object holding scalar data on cells
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorT>
    vtk_writer<MeshT> & add_scalar_data_on_cells(vtk_writer<MeshT> & writer,
                                                 typename MeshT::region_type const & region,
                                                 AccessorT const accessor,
                                                 std::string const & quantity_name)
    {
      writer.add_scalar_data_on_cells(region, accessor, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data on cells for a given region at the VTK writer. At most one data set is allowed.
      *
      * @tparam MeshT             The mesh type to be written
      * @tparam AccessorT    An accessor/field type holding vector data
      * @param  writer              The VTK writer object for which the data should be registered
      * @param  region             The region for which the data is defined
      * @param  accessor   The accessor/field object holding vector data on cells
      * @param  quantity_name       The quantity name within the VTK file
      */
    template <typename MeshT, typename AccessorT>
    vtk_writer<MeshT> & add_vector_data_on_cells(vtk_writer<MeshT> & writer,
                                                 typename MeshT::region_type const & region,
                                                 AccessorT const accessor,
                                                 std::string const & quantity_name)
    {
      writer.add_vector_data_on_cells(region, accessor, quantity_name);
      return writer;
    }


  } //namespace io
} //namespace viennagrid

#endif
