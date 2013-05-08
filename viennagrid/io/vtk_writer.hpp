#ifndef VIENNAGRID_IO_VTK_WRITER_GUARD
#define VIENNAGRID_IO_VTK_WRITER_GUARD

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


#include <fstream>
#include <sstream>
#include <iostream>

#include "viennagrid/domain/domain.hpp"

#include "viennagrid/forwards.hpp"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/vtk_common.hpp"
#include "viennagrid/io/data_accessor.hpp"
#include "viennadata/api.hpp"

/** @file vtk_writer.hpp
    @brief Provides a writer to VTK files
*/

namespace viennagrid
{
  namespace io
  {
    
    /////////////////// VTK export ////////////////////////////

    //helper: translate element tags to VTK-element types
    // (see: http://www.vtk.org/VTK/img/file-formats.pdf, page 9)

    /** @brief Main VTK writer class. Writes a domain or a segment to a file
     *
     * @tparam DomainType    Type of the ViennaGrid domain. Must not be a segment! 
     */
    template < typename DomainType, typename CellTypeOrTag >
    class vtk_writer
    {
      public:
      
      protected:

        //typedef typename DomainType::config_type                         DomainConfiguration;
        
        typedef typename result_of::point_type<DomainType>::type PointType;
        typedef typename result_of::coord_type<PointType>::type CoordType;
        enum { dim = traits::static_size<PointType>::value };
        
        typedef typename result_of::element_tag<CellTypeOrTag>::type CellTag;
        typedef typename result_of::element<DomainType, CellTag>::type CellType;

        //typedef typename DomainConfiguration::numeric_type               CoordType;
        //typedef typename DomainConfiguration::coordinate_system_tag      CoordinateSystemTag;
        //typedef typename DomainConfiguration::cell_tag                   CellTag;

        //typedef typename result_of::point<DomainConfiguration>::type                              PointType;
        typedef typename result_of::element<DomainType, vertex_tag>::type                           VertexType;
        typedef typename result_of::handle<DomainType, vertex_tag>::type          VertexHandleType;
        typedef typename result_of::const_handle<DomainType, vertex_tag>::type          ConstVertexHandleType;
        //typedef typename result_of::element<DomainConfiguration, CellTag::dim>::type     CellType;
        
        
//         typedef typename viennagrid::result_of::point_type<DomainType>::type    PointType;
//         typedef typename viennagrid::result_of::coord_type< PointType >::type      CoordType;
//         typedef CellType_                                                       CellType;
//         typedef typename CellType_::tag                                         CellTag;
//         
        //typedef typename result_of::element<DomainType, viennagrid::vertex_tag>::type          VertexType;
        

      protected:

        /** @brief Writes the XML file header */
        void writeHeader(std::ofstream & writer)
        {
          writer << "<?xml version=\"1.0\"?>" << std::endl;
          writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
          writer << " <UnstructuredGrid>" << std::endl;
        }
        
        
        template<typename SegmentType>
        unsigned int preparePoints(SegmentType const & segment, int seg_num)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, CellTag>::type     CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type                                         CellIterator;

          typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type      VertexOnCellRange;
          typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
          
          std::deque< ConstVertexHandleType > & current_used_vertices = used_vertices[seg_num];
          std::map< ConstVertexHandleType, std::size_t > & current_vertex_to_index_map = vertex_to_index_map[seg_num];
          
          std::size_t index = 0;
          CellRange cells = viennagrid::elements(segment);
//           std::cout << cells.size() << std::endl;
          for (CellIterator it = cells.begin(); it != cells.end(); ++it)
          {
              VertexOnCellRange vertices_on_cell = viennagrid::elements(*it);
              for (VertexOnCellIterator jt = vertices_on_cell.begin(); jt != vertices_on_cell.end(); ++jt)
              {
                  typename std::map< ConstVertexHandleType, std::size_t >::iterator kt = current_vertex_to_index_map.find( jt.handle() );
                  if (kt == current_vertex_to_index_map.end())
                  {
                      current_vertex_to_index_map.insert( std::make_pair( jt.handle(), index++ ) );
                      current_used_vertices.push_back( jt.handle() );
                  }
              }
          }
          
          return current_vertex_to_index_map.size();
        }

        /** @brief Writes the vertices in the domain */
        template <typename SegmentType>
        void writePoints(SegmentType const & segment, std::ofstream & writer, int seg_num)
        {
//           std::map< ConstVertexHandleType, std::size_t > & current_vertex_to_index_map = vertex_to_index_map[seg_num];
            std::deque< ConstVertexHandleType > & current_used_vertices = used_vertices[seg_num];
          
          writer << "   <Points>" << std::endl;
          writer << "    <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

          
//           VertexRange vertices = viennagrid::elements<vertex_tag>(segment);
//           for (VertexHandleIterator vit = vertices.handle_begin();
//               vit != vertices.handle_end();
//               ++vit)
//           std::cout << current_vertex_to_index_map.size() << std::endl;
          for (typename std::deque< ConstVertexHandleType >::iterator it = current_used_vertices.begin(); it != current_used_vertices.end(); ++it)
          {
            //PointWriter<dim>::write(writer, vit->point());
//             vertex_to_index_map[seg_num][*vit] = index++;
            
//             VertexType const & vertex = viennagrid::dereference_handle(segment, it->first);
            
            PointWriter<dim>::write(writer, viennagrid::point(segment, *it) );
//             std::cout << viennagrid::point(segment, it->first) << std::endl;

            // add 0's for less than three dimensions
              if (dim == 2)
                writer << " " << 0;
              if(dim == 1)
                writer << " " << 0 << " " << 0;

              writer << std::endl;
          }
          writer << std::endl;
          writer << "    </DataArray>" << std::endl;
          writer << "   </Points> " << std::endl;
        } //writePoints()

        /** @brief Writes the cells to the domain */
        template <typename DomainSegmentType>
        void writeCells(DomainSegmentType const & domseg, std::ofstream & writer, int seg_num)
        {
          typedef typename viennagrid::result_of::const_element_range<DomainSegmentType, CellTag>::type     CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type                                         CellIterator;

          typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type      VertexOnCellRange;
          typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
          typedef typename viennagrid::result_of::handle_iterator<VertexOnCellRange>::type         VertexHandleOnCellIterator;
          
          //vtk_vertex_id_repository<DomainSegmentType>  vertex_ids(domseg);
          
          writer << "   <Cells> " << std::endl;
          writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;
          CellRange cells = viennagrid::elements<CellTag>(domseg);
          for (CellIterator cit = cells.begin();
              cit != cells.end();
              ++cit)
          {
              //step 1: Write vertex indices in ViennaGrid orientation to array:
              std::vector<std::size_t> viennagrid_vertices(viennagrid::element_topology::boundary_cells<CellTag, vertex_tag>::num);
              VertexOnCellRange vertices_on_cell = viennagrid::elements<vertex_tag>(*cit);
              std::size_t j = 0;
              for (VertexHandleOnCellIterator vocit = vertices_on_cell.handle_begin();
                  vocit != vertices_on_cell.handle_end();
                  ++vocit, ++j)
              {
                viennagrid_vertices[j] = vertex_to_index_map[seg_num][*vocit];
                //&(*vocit);
              }
              
              //Step 2: Write the transformed connectivities:
              viennagrid_to_vtk_orientations<CellTag> reorderer;
              for (std::size_t i=0; i<viennagrid_vertices.size(); ++i)
              {
                //writer << viennagrid_vertices[reorderer(static_cast<long>(i))]->id() << " ";
                writer <<   viennagrid_vertices[reorderer(static_cast<long>(i))] << " ";
//                 writer << vertex_ids(*(viennagrid_vertices[reorderer(static_cast<long>(i))]), //convert between different orientations used within VTK and ViennaGrid
//                                      domseg) << " ";
              }
                
              
              writer << std::endl;
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;

            writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
            for (std::size_t offsets = 1;
                 offsets <= viennagrid::elements<CellTag>(domseg).size();
                 ++offsets)
            {
              writer << ( offsets * viennagrid::element_topology::boundary_cells<CellTag, vertex_tag>::num) << " ";
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;

            writer << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
            for (std::size_t offsets = 1;
                  offsets <= viennagrid::elements<CellTag>(domseg).size();
                  ++offsets)
            {
              writer << ELEMENT_TAG_TO_VTK_TYPE<CellTag>::value << " ";
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;
            writer << "   </Cells>" << std::endl;
        }

        /** @brief Writes scalar-valued data defined on vertices (points) to file */
        template <typename SegmentType>
        void writePointDataScalar(SegmentType const & segment, std::ofstream & writer, std::size_t seg_id = 0)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, vertex_tag>::type   VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
          
          if (vertex_data_scalar.size() > 0)
          {
            for (size_t i=0; i<vertex_data_scalar.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << vertex_data_scalar_names[i] << "\" format=\"ascii\">" << std::endl;
              
              VertexRange vertices = viennagrid::elements<vertex_tag>(segment);
              for (VertexIterator vit = vertices.begin();
                  vit != vertices.end();
                  ++vit)
              {
                writer << vertex_data_scalar[i](*vit, seg_id) << " ";
              }
              writer << std::endl;
              writer << "    </DataArray>" << std::endl;
            }
          }
        } //writePointDataScalar

        /** @brief Writes vector-valued data defined on vertices (points) to file */
        template <typename SegmentType>
        void writePointDataVector(SegmentType const & segment, std::ofstream & writer, std::size_t seg_id = 0)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, vertex_tag>::type   VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
          
          if (vertex_data_vector.size() > 0)
          {
            for (size_t i=0; i<vertex_data_vector.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << vertex_data_vector_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;
              
              VertexRange vertices = viennagrid::elements<vertex_tag>(segment);
              for (VertexIterator vit = vertices.begin();
                  vit != vertices.end();
                  ++vit)
              {
                writer << vertex_data_vector[i](*vit, seg_id) << " ";
              }
              writer << std::endl;
              writer << "    </DataArray>" << std::endl;
            }
          }
        } //writePointDataScalar

        // Write normal point data:
        /** @brief Writes vector-valued data (normals) defined on vertices (points) to file */
        template <typename SegmentType>
        void writePointDataNormal(SegmentType const & segment, std::ofstream & writer, std::size_t seg_id = 0)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, vertex_tag>::type   VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
          
          if (vertex_data_normal.size() > 0)
          {
            for (size_t i=0; i<vertex_data_normal.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << vertex_data_normal_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;
              
              VertexRange vertices = viennagrid::elements<vertex_tag>(segment);
              for (VertexIterator vit = vertices.begin();
                  vit != vertices.end();
                  ++vit)
              {
                writer << vertex_data_normal[i](*vit, seg_id) << " ";
              }
              writer << std::endl;
              writer << "    </DataArray>" << std::endl;
            }
          }
        } //writePointDataScalar



        ///////////////// cells /////////////////////

        /** @brief Writes scalar-valued data defined on cells to file */
        template <typename SegmentType>
        void writeCellDataScalar(SegmentType const & segment, std::ofstream & writer, std::size_t seg_id = 0)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, CellTag>::type   CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type               CellIterator;
          
          if (cell_data_scalar.size() > 0)
          {
            for (size_t i=0; i<cell_data_scalar.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << cell_data_scalar_names[i] << "\" format=\"ascii\">" << std::endl;
              
              CellRange cells = viennagrid::elements<CellTag>(segment);
              for (CellIterator cit  = cells.begin();
                                cit != cells.end();
                              ++cit)
              {
                writer << cell_data_scalar[i](*cit, seg_id) << " ";
              }
              writer << std::endl;
              writer << "    </DataArray>" << std::endl;
            }
          }
        } //writeCellDataScalar

        /** @brief Writes vector-valued data defined on cells to file */
        template <typename SegmentType>
        void writeCellDataVector(SegmentType const & segment, std::ofstream & writer, std::size_t seg_id = 0)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, CellTag>::type   CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type               CellIterator;
          
          if (cell_data_vector.size() > 0)
          {
            for (size_t i=0; i<cell_data_vector.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << cell_data_vector_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;
              
              CellRange cells = viennagrid::elements<CellTag>(segment);
              for (CellIterator cit  = cells.begin();
                                cit != cells.end();
                              ++cit)
              {
                writer << cell_data_vector[i](*cit, seg_id) << " ";
              }
              writer << std::endl;
              writer << "    </DataArray>" << std::endl;
            }
          }
        } //writeCellDataVector

        /** @brief Writes vector-valued data (normals) defined on cells to file */
        template <typename SegmentType>
        void writeCellDataNormals(SegmentType const & segment,
                                  std::ofstream & writer,
                                  std::size_t seg_id = 0)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, CellTag>::type     CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type                                         CellIterator;
          
          if (cell_data_normal.size() > 0)
          {
            for (size_t i=0; i<cell_data_normal.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << cell_data_normal_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

              CellRange cells = viennagrid::elements<CellTag>(segment);
              for (CellIterator cit  = cells.begin();
                                cit != cells.end();
                              ++cit)
              {
                 //std::cout << "josef: " << cell_data_normal[i](*cit, seg_id) << std::endl;
                 //std::cout << cell_data_normal[i](*cit, seg_id) << std::endl;
                 //viennagrid::io::PointWriter<3>::write(writer, cell_data_normal[i](*cit, seg_id));
                 //viennautils::print()(cell_data_normal[i](*cit, seg_id), writer);
                 //viennautils::dumptype(cell_data_normal[i](*cit, seg_id));
                 //writer << std::endl;
                 writer << cell_data_normal[i](*cit, seg_id) << std::endl;
              }
              writer << "    </DataArray>" << std::endl;
            }
          }
        } //writeCellData

        /** @brief Writes the XML footer */
        void writeFooter(std::ofstream & writer)
        {
          writer << " </UnstructuredGrid>" << std::endl;
          writer << "</VTKFile>" << std::endl;
        } 

      public:

        /** @brief Triggers the write process to a XML file. Make sure that all data to be written to the file is already passed to the writer
         * 
         * @param domain     The ViennaGrid domain. Must not be a segment!
         * @param filename   The file to write to         
         */
        int operator()(DomainType const & domain, std::string const & filename)
        {
//             std::stringstream ss;
//             ss << filename;
//             
//             if (filename.rfind(".vtu") != filename.size()-4)
//                 ss << ".vtu";
//             std::ofstream writer(ss.str().c_str());

            std::ofstream writer(filename.c_str());
            writeHeader(writer);
            
            used_vertices.resize(1);
            vertex_to_index_map.resize(1);
            
            unsigned int num_points = preparePoints(domain, 0);
            
            writer << "  <Piece NumberOfPoints=\""
                   << num_points
                   << "\" NumberOfCells=\""
                   << viennagrid::elements<CellTag>(domain).size()
                   << "\">" << std::endl;

            writePoints(domain, writer, 0);
            
            if (vertex_data_scalar.size() + vertex_data_vector.size() + vertex_data_normal.size() > 0)
            {
              writer << "   <PointData ";
              if (vertex_data_scalar.size() > 0)
                writer << "Scalars=\"" << vertex_data_scalar_names[0] << "\" ";
              if (vertex_data_vector.size() > 0)
                writer << "Vectors=\"" << vertex_data_vector_names[0] << "\" ";
              if (vertex_data_normal.size() > 0)
                writer << "Normals=\"" << vertex_data_normal_names[0] << "\" ";
              writer << ">" << std::endl;
              writePointDataScalar(domain, writer);
              writePointDataVector(domain, writer);
              writePointDataNormal(domain, writer);
              writer << "   </PointData>" << std::endl;
            }

            writeCells(domain, writer, 0);
            if (cell_data_scalar.size() + cell_data_vector.size() + cell_data_normal.size() > 0)
            {
              writer << "   <CellData ";
              if (cell_data_scalar.size() > 0)
                writer << "Scalars=\"" << cell_data_scalar_names[0] << "\" ";
              if (cell_data_vector.size() > 0)
                writer << "Vectors=\"" << cell_data_vector_names[0] << "\" ";
              if (cell_data_normal.size() > 0)
                writer << "Normals=\"" << cell_data_normal_names[0] << "\" ";
              writer << ">" << std::endl;
              writeCellDataScalar(domain, writer);
              writeCellDataVector(domain, writer);
              writeCellDataNormals(domain, writer);
              writer << "   </CellData>" << std::endl;
            }

            writer << "  </Piece>" << std::endl;
            writeFooter(writer);
          
          return EXIT_SUCCESS;
        }
        
        
        
        
        template<typename SegmentContainerType>
        int operator()(DomainType const & domain, const SegmentContainerType & segments, std::string const & filename)
        {
            typedef typename SegmentContainerType::value_type SegmentType;

          long segment_num = segments.size();
          if (segment_num > 0)
          {
            //
            // Step 1: Write meta information
            //
            {
              std::stringstream ss;
              ss << filename << "_main.pvd";
              std::ofstream writer(ss.str().c_str());
              
              std::string short_filename = filename;
              std::string::size_type pos = filename.rfind("/");
              if (pos == std::string::npos)
                pos = filename.rfind("\\");   //A tribute to Windows

              if (pos != std::string::npos)
                short_filename = filename.substr(pos+1, filename.size());

              if (!writer){
                throw cannot_open_file_exception(filename);
                return EXIT_FAILURE;
              }
              
              writer << "<?xml version=\"1.0\"?>" << std::endl;
              writer << "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">" << std::endl;
              writer << "<Collection>" << std::endl;
              
              
              for (long i = 0; i<segment_num; ++i)
                writer << "    <DataSet part=\"" << i << "\" file=\"" << short_filename << "_" << i << ".vtu\" name=\"Segment_" << i << "\"/>" << std::endl;
                
                
              writer << "  </Collection>" << std::endl;
              writer << "</VTKFile>" << std::endl;
              writer.close();
            }
            
            
            used_vertices.resize( segment_num );
            vertex_to_index_map.resize( segment_num );
            
            //
            // Step 2: Write segments to individual files
            //
            for (long i = 0; i<segment_num; ++i)
            {
              std::stringstream ss;
              ss << filename << "_" << i << ".vtu";
              std::ofstream writer(ss.str().c_str());
              writeHeader(writer);

              if (!writer)
              {
                throw cannot_open_file_exception(ss.str());
                return EXIT_FAILURE;
              }
              
              //typedef typename DomainType::segment_type                                             SegmentType;
              typedef typename viennagrid::result_of::const_element_range<SegmentType, vertex_tag>::type   VertexRange;
              typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
              //std::cout << "Writing segment" << std::endl;
              //Segment & curSeg = *segit;

              SegmentType const & seg = *viennagrid::advance(segments.begin(), i);
              
              //setting local segment numbering on vertices:
              long current_id = 0;
              VertexRange vertices = viennagrid::elements<vertex_tag>(seg);
              for (VertexIterator vit = vertices.begin();
                  vit != vertices.end();
                  ++vit)
              {
                viennadata::access<segment_mapping_key<SegmentType>, long>(seg)(*vit) = current_id++;
              }
              
              unsigned int num_points = preparePoints(seg, i);

              writer << "  <Piece NumberOfPoints=\""
                    << num_points
                    << "\" NumberOfCells=\""
                    << viennagrid::elements<CellTag>(seg).size()
                    << "\">" << std::endl;


              writePoints(seg, writer, i);
              
              if (vertex_data_scalar.size() + vertex_data_vector.size() + vertex_data_normal.size() > 0)
              {
                writer << "   <PointData ";
                if (vertex_data_scalar.size() > 0)
                  writer << "Scalars=\"" << vertex_data_scalar_names[0] << "\" ";
                if (vertex_data_vector.size() > 0)
                  writer << "Vectors=\"" << vertex_data_vector_names[0] << "\" ";
                if (vertex_data_normal.size() > 0)
                  writer << "Normals=\"" << vertex_data_normal_names[0] << "\" ";
                writer << ">" << std::endl;
                writePointDataScalar(seg, writer, i);
                writePointDataVector(seg, writer, i);
                writePointDataNormal(seg, writer, i);
                writer << "   </PointData>" << std::endl;
              }

              writeCells(seg, writer, i);
              if (cell_data_scalar.size() + cell_data_vector.size() + cell_data_normal.size() > 0)
              {
                writer << "   <CellData ";
                if (cell_data_scalar.size() > 0)
                  writer << "Scalars=\"" << cell_data_scalar_names[0] << "\" ";
                if (cell_data_vector.size() > 0)
                  writer << "Vectors=\"" << cell_data_vector_names[0] << "\" ";
                if (cell_data_normal.size() > 0)
                  writer << "Normals=\"" << cell_data_normal_names[0] << "\" ";
                writer << ">" << std::endl;
                writeCellDataScalar(seg, writer, i);
                writeCellDataVector(seg, writer, i);
                writeCellDataNormals(seg, writer, i);
                writer << "   </CellData>" << std::endl;
              }

              writer << "  </Piece>" << std::endl;
              writeFooter(writer);
              writer.close();
            }
          }
          else
              (*this)(domain, filename);
          
          return EXIT_SUCCESS;
        }
        
        
        
        /** @brief Specify scalar-valued data for vertices to the writer. Prefer the free function add_scalar_data_on_vertices() 
         *
         * @tparam T       Anything that can be wrapped by a data_accessor_wrapper
         * @param  accessor The quantity accessor
         * @param  name    The quantity name that should appear in the VTK file
         */
        template <typename T>
        void add_scalar_data_on_vertices(T const & accessor, std::string name)
        {
          data_accessor_wrapper<VertexType> wrapper(accessor.clone());
          vertex_data_scalar.push_back(wrapper);
          vertex_data_scalar_names.push_back(name);
        }

        /** @brief Specify vector-valued data for vertices to the writer. Prefer the free function add_vector_data_on_vertices() 
         *
         * @tparam T       Anything that can be wrapped by a data_accessor_wrapper
         * @param  accessor The quantity accessor
         * @param  name    The quantity name that should appear in the VTK file
         */
        template <typename T>
        void add_vector_data_on_vertices(T const & accessor, std::string name)
        {
          data_accessor_wrapper<VertexType> wrapper(accessor.clone());
          vertex_data_vector.push_back(wrapper);
          vertex_data_vector_names.push_back(name);
        }

        /** @brief Specify vector-valued data (normals) for vertices to the writer. Prefer the free function add_normal_data_on_vertices() 
         *
         * @tparam T       Anything that can be wrapped by a data_accessor_wrapper
         * @param  accessor The quantity accessor
         * @param  name    The quantity name that should appear in the VTK file
         */
        template <typename T>
        void add_normal_data_on_vertices(T const & accessor, std::string name)
        {
          data_accessor_wrapper<VertexType> wrapper(accessor.clone());
          vertex_data_normal.push_back(wrapper);
          vertex_data_normal_names.push_back(name);
        }


        /** @brief Specify scalar-valued data for cells to the writer. Prefer the free function add_scalar_data_on_cells() 
         *
         * @tparam T       Anything that can be wrapped by a data_accessor_wrapper
         * @param  accessor The quantity accessor
         * @param  name    The quantity name that should appear in the VTK file
         */
        template <typename T>
        void add_scalar_data_on_cells(T const & accessor, std::string name)
        {
          data_accessor_wrapper<CellType> wrapper(accessor.clone());
          cell_data_scalar.push_back(wrapper);
          cell_data_scalar_names.push_back(name);
        }

        /** @brief Specify vector-valued data for cells to the writer. Prefer the free function add_vector_data_on_cells() 
         *
         * @tparam T       Anything that can be wrapped by a data_accessor_wrapper
         * @param  accessor The quantity accessor
         * @param  name    The quantity name that should appear in the VTK file
         */
        template <typename T>
        void add_vector_data_on_cells(T const & accessor, std::string name)
        {
          data_accessor_wrapper<CellType> wrapper(accessor.clone());
          cell_data_vector.push_back(wrapper);
          cell_data_vector_names.push_back(name);
        }


        /** @brief Specify vector-valued data (normals) for cells to the writer. Prefer the free function add_normal_data_on_cells() 
         *
         * @tparam T       Anything that can be wrapped by a data_accessor_wrapper
         * @param  accessor The quantity accessor
         * @param  name    The quantity name that should appear in the VTK file
         */
        template <typename T>
        void add_normal_data_on_cells(T const & accessor, std::string name)
        {
          data_accessor_wrapper<CellType> wrapper(accessor.clone());
          cell_data_normal.push_back(wrapper);
          cell_data_normal_names.push_back(name);
        }

      private:
          
        std::vector< std::map< ConstVertexHandleType, std::size_t > >            vertex_to_index_map;
        std::vector< std::deque< ConstVertexHandleType> >                        used_vertices;
          
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
        
    };

    /** @brief Convenience function that exports a domain to file directly. Does not export quantities */
    template < typename DomainType, typename CellTypeOrTag, typename SegmentContainerType > 
    int export_vtk(DomainType const& domain, const SegmentContainerType & segments, std::string const & filename)
    {
      vtk_writer<DomainType, CellTypeOrTag> vtk_writer;
      return vtk_writer(domain, segments, filename);
    }



    //
    // Convenience functions for adding vertex-based data
    //    
    
    /** @brief Registers scalar-valued data on vertices at the XML writer. 
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellType>
    vtk_writer<DomainType, CellType> & add_scalar_data_on_vertices(vtk_writer<DomainType, CellType> & writer,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      //typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::element<DomainType, viennagrid::vertex_tag>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new global_scalar_data_accessor<VertexType, KeyType, DataType>(key));
      writer.add_scalar_data_on_vertices(wrapper, quantity_name);
      return writer;
    }

    /** @brief Registers scalar-valued data on vertices at the XML writer. Data is segment based and might be discontinuous at segment boundaries.
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellType>
    vtk_writer<DomainType, CellType> & add_scalar_data_on_vertices_per_segment(vtk_writer<DomainType, CellType> & writer,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      //typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::element<DomainType, vertex_tag>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new segment_scalar_data_accessor<VertexType, KeyType, DataType>(key));
      writer.add_scalar_data_on_vertices(wrapper, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data on vertices at the XML writer. 
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellType>
    vtk_writer<DomainType, CellType> & add_vector_data_on_vertices(vtk_writer<DomainType, CellType> & writer,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      //typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::element<DomainType, vertex_tag>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new global_vector_data_accessor<VertexType, KeyType, DataType>(key));
      writer.add_vector_data_on_vertices(wrapper, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data on vertices at the XML writer. Data is segment based and might be discontinuous at segment boundaries.
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellType>
    vtk_writer<DomainType, CellType> & add_vector_data_on_vertices_per_segment(vtk_writer<DomainType, CellType> & writer,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      //typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::element<DomainType, vertex_tag>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new segment_vector_data_accessor<VertexType, KeyType, DataType>(key));
      writer.add_vector_data_on_vertices(wrapper, quantity_name);
      return writer;
    }


    /** @brief Registers vector-valued data (normals) on vertices at the XML writer. 
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellType>
    vtk_writer<DomainType, CellType> & add_normal_data_on_vertices(vtk_writer<DomainType, CellType> & writer,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
//       typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::element<DomainType, vertex_tag>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new global_vector_data_accessor<VertexType, KeyType, DataType>(key));
      writer.add_normal_data_on_vertices(wrapper, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data (normals) on vertices at the XML writer. Data is segment based and might be discontinuous at segment boundaries.
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellType>
    vtk_writer<DomainType, CellType> & add_normal_data_on_vertices_per_segment(vtk_writer<DomainType, CellType> & writer,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      //typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::element<DomainType, vertex_tag>::type      VertexType;
      
      data_accessor_wrapper<VertexType> wrapper(new segment_vector_data_accessor<VertexType, KeyType, DataType>(key));
      writer.add_normal_data_on_vertices(wrapper, quantity_name);
      return writer;
    }



    //
    // Convenience functions for adding cell-based data
    //



    /** @brief Registers scalar-valued data on cells at the XML writer. 
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellTypeOrTag>
    vtk_writer<DomainType, CellTypeOrTag> & add_scalar_data_on_cells(vtk_writer<DomainType, CellTypeOrTag> & writer,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename result_of::element<DomainType, CellTypeOrTag>::type CellType;
//       typedef typename DomainType::config_type                         DomainConfiguration;
//       typedef typename DomainConfiguration::cell_tag                   CellTag;
//       typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_scalar_data_accessor<CellType, KeyType, DataType>(key));
      writer.add_scalar_data_on_cells(wrapper, quantity_name);
      return writer;
    }
    
    
    template <typename KeyType, typename DataType, typename DomainType, typename CellTypeOrTag>
    vtk_writer<DomainType, CellTypeOrTag> & add_scalar_data_on_cells(vtk_writer<DomainType, CellTypeOrTag> & writer,
                                                      std::string quantity_name)
    {
      typedef typename result_of::element<DomainType, CellTypeOrTag>::type CellType;
//       typedef typename DomainType::config_type                         DomainConfiguration;
//       typedef typename DomainConfiguration::cell_tag                   CellTag;
//       typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_scalar_data_accessor_type_based_key<CellType, KeyType, DataType>());
      writer.add_scalar_data_on_cells(wrapper, quantity_name);
      return writer;
    }
    

    /** @brief Registers scalar-valued data on cells at the XML writer. Data is segment based and might be discontinuous at segment boundaries.
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellTypeOrTag>
    vtk_writer<DomainType, CellTypeOrTag> & add_scalar_data_on_cells_per_segment(vtk_writer<DomainType, CellTypeOrTag> & writer,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename result_of::element<DomainType, CellTypeOrTag>::type CellType;
//       typedef typename DomainType::config_type                         DomainConfiguration;
//       typedef typename DomainConfiguration::cell_tag                   CellTag;
//       typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new segment_scalar_data_accessor<CellType, KeyType, DataType>(key));
      writer.add_scalar_data_on_cells(wrapper, quantity_name);
      return writer;
    }

    // vector data
    /** @brief Registers vector-valued data on cells at the XML writer. 
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellTypeOrTag>
    vtk_writer<DomainType, CellTypeOrTag> & add_vector_data_on_cells(vtk_writer<DomainType, CellTypeOrTag> & writer,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename result_of::element<DomainType, CellTypeOrTag>::type CellType;
//       typedef typename DomainType::config_type                         DomainConfiguration;
//       typedef typename DomainConfiguration::cell_tag                   CellTag;
//       typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_vector_data_accessor<CellType, KeyType, DataType>(key));
      writer.add_vector_data_on_cells(wrapper, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data on cells at the XML writer. Data is segment based and might be discontinuous at segment boundaries.
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellTypeOrTag>
    vtk_writer<DomainType, CellTypeOrTag> & add_vector_data_on_cells_per_segment(vtk_writer<DomainType, CellTypeOrTag> & writer,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename result_of::element<DomainType, CellTypeOrTag>::type CellType;
//       typedef typename DomainType::config_type                         DomainConfiguration;
//       typedef typename DomainConfiguration::cell_tag                   CellTag;
//       typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new segment_vector_data_accessor<CellType, KeyType, DataType>(key));
      writer.add_vector_data_on_cells(wrapper, quantity_name);
      return writer;
    }

    // cell data
    /** @brief Registers vector-valued data (normals) on cells at the XML writer. 
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellTypeOrTag>
    vtk_writer<DomainType, CellTypeOrTag> & add_normal_data_on_cells(vtk_writer<DomainType, CellTypeOrTag> & writer,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename result_of::element<DomainType, CellTypeOrTag>::type CellType;
//       typedef typename DomainType::config_type                         DomainConfiguration;
//       typedef typename DomainConfiguration::cell_tag                   CellTag;
//       typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_vector_data_accessor<CellType, KeyType, DataType>(key));
      writer.add_normal_data_on_cells(wrapper, quantity_name);
      return writer;
    }

    /** @brief Registers vector-valued data (normals) on cells at the XML writer. Data is segment based and might be discontinuous at segment boundaries.
      *
      * @tparam KeyType     Type of the key used with ViennaData
      * @tparam DataType    Type of the data as used with ViennaData
      * @tparam DomainType  The ViennaGrid domain type
      * @param  writer      The XML writer object for which the data should be registered
      * @param  key         The key object for ViennaData
      * @param  quantity_name        The quantity name that should appear in the VTK file
      */
    template <typename KeyType, typename DataType, typename DomainType, typename CellTypeOrTag>
    vtk_writer<DomainType, CellTypeOrTag> & add_normal_data_on_cells_per_segment(vtk_writer<DomainType, CellTypeOrTag> & writer,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename result_of::element<DomainType, CellTypeOrTag>::type CellType;
//       typedef typename DomainType::config_type                         DomainConfiguration;
//       typedef typename DomainConfiguration::cell_tag                   CellTag;
//       typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new segment_vector_data_accessor<CellType, KeyType, DataType>(key));
      writer.add_normal_data_on_cells(wrapper, quantity_name);
      return writer;
    }





  } //namespace io
} //namespace viennagrid

#endif
