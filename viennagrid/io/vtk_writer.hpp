#ifndef VIENNAGRID_IO_VTK_WRITER_GUARD
#define VIENNAGRID_IO_VTK_WRITER_GUARD

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


#include <fstream>
#include <sstream>
#include <iostream>
#include "viennagrid/domain.hpp"
#include "viennagrid/forwards.h"
#include "viennagrid/iterators.hpp"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/vtk_common.hpp"
#include "viennagrid/io/data_accessor.hpp"
//#include "viennagrid/algorithm/cell_normals.hpp"
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
    template < typename DomainType >
    class vtk_writer
    {
      public:
      
      protected:

        typedef typename DomainType::config_type                         DomainConfiguration;

        typedef typename DomainConfiguration::numeric_type               CoordType;
        typedef typename DomainConfiguration::coordinate_system_tag      CoordinateSystemTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;

        typedef typename result_of::point<DomainConfiguration>::type                              PointType;
        typedef typename result_of::ncell<DomainConfiguration, 0>::type                           VertexType;
        typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;

      protected:

        /** @brief Writes the XML file header */
        void writeHeader(std::ofstream & writer)
        {
          writer << "<?xml version=\"1.0\"?>" << std::endl;
          writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
          writer << " <UnstructuredGrid>" << std::endl;
        }

        /** @brief Writes the vertices in the domain */
        template <typename SegmentType>
        void writePoints(SegmentType const & segment, std::ofstream & writer)
        {
          typedef typename viennagrid::result_of::const_ncell_range<SegmentType, 0>::type   VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
          
          writer << "   <Points>" << std::endl;
          writer << "    <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

          VertexRange vertices = viennagrid::ncells<0>(segment);
          for (VertexIterator vit = vertices.begin();
              vit != vertices.end();
              ++vit)
          {
            PointWriter<CoordinateSystemTag::dim>::write(writer, vit->point());

            // add 0's for less than three dimensions
              if (CoordinateSystemTag::dim == 2)
                writer << " " << 0;
              if(CoordinateSystemTag::dim == 1)
                writer << " " << 0 << " " << 0;

              writer << std::endl;
          }
          writer << std::endl;
          writer << "    </DataArray>" << std::endl;
          writer << "   </Points> " << std::endl;
        } //writePoints()

        /** @brief Writes the cells to the domain */
        template <typename DomainSegmentType>
        void writeCells(DomainSegmentType const & domseg, std::ofstream & writer)
        {
          typedef typename viennagrid::result_of::const_ncell_range<DomainSegmentType, CellTag::dim>::type     CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type                                         CellIterator;

          typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type      VertexOnCellRange;
          typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
          
          vtk_vertex_id_repository<DomainSegmentType>  vertex_ids(domseg);
          
          writer << "   <Cells> " << std::endl;
          writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;
          CellRange cells = viennagrid::ncells<CellTag::dim>(domseg);
          for (CellIterator cit = cells.begin();
              cit != cells.end();
              ++cit)
          {
              //step 1: Write vertex indices in ViennaGrid orientation to array:
              std::vector<VertexType const *> viennagrid_vertices(viennagrid::topology::bndcells<CellTag, 0>::num);
              VertexOnCellRange vertices_on_cell = viennagrid::ncells<0>(*cit);
              std::size_t j = 0;
              for (VertexOnCellIterator vocit = vertices_on_cell.begin();
                  vocit != vertices_on_cell.end();
                  ++vocit, ++j)
              {
                viennagrid_vertices[j] = &(*vocit);
              }
              
              //Step 2: Write the transformed connectivities:
              viennagrid_to_vtk_orientations<CellTag> reorderer;
              for (std::size_t i=0; i<viennagrid_vertices.size(); ++i)
              {
                writer << vertex_ids(*(viennagrid_vertices[reorderer(static_cast<long>(i))]), //convert between different orientations used within VTK and ViennaGrid
                                     domseg) << " ";
              }
                
              
              writer << std::endl;
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;

            writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
            for (std::size_t offsets = 1;
                 offsets <= viennagrid::ncells<CellTag::dim>(domseg).size();
                 ++offsets)
            {
              writer << ( offsets * viennagrid::topology::bndcells<CellTag, 0>::num) << " ";
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;

            writer << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
            for (std::size_t offsets = 1;
                  offsets <= viennagrid::ncells<CellTag::dim>(domseg).size();
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
          typedef typename viennagrid::result_of::const_ncell_range<SegmentType, 0>::type   VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
          
          if (vertex_data_scalar.size() > 0)
          {
            for (size_t i=0; i<vertex_data_scalar.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << vertex_data_scalar_names[i] << "\" format=\"ascii\">" << std::endl;
              
              VertexRange vertices = viennagrid::ncells<0>(segment);
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
          typedef typename viennagrid::result_of::const_ncell_range<SegmentType, 0>::type   VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
          
          if (vertex_data_vector.size() > 0)
          {
            for (size_t i=0; i<vertex_data_vector.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << vertex_data_vector_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;
              
              VertexRange vertices = viennagrid::ncells<0>(segment);
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
          typedef typename viennagrid::result_of::const_ncell_range<SegmentType, 0>::type   VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
          
          if (vertex_data_normal.size() > 0)
          {
            for (size_t i=0; i<vertex_data_normal.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << vertex_data_normal_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;
              
              VertexRange vertices = viennagrid::ncells<0>(segment);
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
          typedef typename viennagrid::result_of::const_ncell_range<SegmentType, CellTag::dim>::type   CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type               CellIterator;
          
          if (cell_data_scalar.size() > 0)
          {
            for (size_t i=0; i<cell_data_scalar.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << cell_data_scalar_names[i] << "\" format=\"ascii\">" << std::endl;
              
              CellRange cells = viennagrid::ncells(segment);
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
          typedef typename viennagrid::result_of::const_ncell_range<SegmentType, CellTag::dim>::type   CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type               CellIterator;
          
          if (cell_data_vector.size() > 0)
          {
            for (size_t i=0; i<cell_data_vector.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << cell_data_vector_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;
              
              CellRange cells = viennagrid::ncells(segment);
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
          typedef typename viennagrid::result_of::const_ncell_range<SegmentType, CellTag::dim>::type     CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type                                         CellIterator;
          
          if (cell_data_normal.size() > 0)
          {
            for (size_t i=0; i<cell_data_normal.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << cell_data_normal_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

              CellRange cells = viennagrid::ncells<CellTag::dim>(segment);
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

          long segment_num = domain.segments().size();
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
              
              typedef typename DomainType::segment_type                                             SegmentType;
              typedef typename viennagrid::result_of::const_ncell_range<SegmentType, 0>::type   VertexRange;
              typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;
              //std::cout << "Writing segment" << std::endl;
              //Segment & curSeg = *segit;

              SegmentType const & seg = domain.segments()[i];
              
              //setting local segment numbering on vertices:
              long current_id = 0;
              VertexRange vertices = viennagrid::ncells<0>(seg);
              for (VertexIterator vit = vertices.begin();
                  vit != vertices.end();
                  ++vit)
              {
                viennadata::access<segment_mapping_key<SegmentType>, long>(seg)(*vit) = current_id++;
              }

              writer << "  <Piece NumberOfPoints=\""
                    << viennagrid::ncells<0>(seg).size()
                    << "\" NumberOfCells=\""
                    << viennagrid::ncells<CellTag::dim>(seg).size()
                    << "\">" << std::endl;


              writePoints(seg, writer);
              
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

              writeCells(seg, writer);
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
          {
            std::stringstream ss;
            ss << filename << ".vtu";
            std::ofstream writer(ss.str().c_str());
            writeHeader(writer);
            
            writer << "  <Piece NumberOfPoints=\""
                   << viennagrid::ncells<0>(domain).size()
                   << "\" NumberOfCells=\""
                   << viennagrid::ncells<CellTag::dim>(domain).size()
                   << "\">" << std::endl;


            writePoints(domain, writer);
            
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

            writeCells(domain, writer);
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

          }
          
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
    template < typename DomainType > 
    int export_vtk(DomainType const& domain, std::string const & filename)
    {
      vtk_writer<DomainType> vtk_writer;
      return vtk_writer(domain, filename);
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_scalar_data_on_vertices(vtk_writer<DomainType> & writer,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_scalar_data_on_vertices_per_segment(vtk_writer<DomainType> & writer,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_vector_data_on_vertices(vtk_writer<DomainType> & writer,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_vector_data_on_vertices_per_segment(vtk_writer<DomainType> & writer,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_normal_data_on_vertices(vtk_writer<DomainType> & writer,
                                                         KeyType const & key,
                                                         std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_normal_data_on_vertices_per_segment(vtk_writer<DomainType> & writer,
                                                                     KeyType const & key,
                                                                     std::string quantity_name)
    {
      typedef typename DomainType::config_type                             DomainConfiguration;
      typedef typename result_of::ncell<DomainConfiguration, 0>::type      VertexType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_scalar_data_on_cells(vtk_writer<DomainType> & writer,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new global_scalar_data_accessor<CellType, KeyType, DataType>(key));
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_scalar_data_on_cells_per_segment(vtk_writer<DomainType> & writer,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_vector_data_on_cells(vtk_writer<DomainType> & writer,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_vector_data_on_cells_per_segment(vtk_writer<DomainType> & writer,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_normal_data_on_cells(vtk_writer<DomainType> & writer,
                                                      KeyType const & key,
                                                      std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
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
    template <typename KeyType, typename DataType, typename DomainType>
    vtk_writer<DomainType> & add_normal_data_on_cells_per_segment(vtk_writer<DomainType> & writer,
                                                                  KeyType const & key,
                                                                  std::string quantity_name)
    {
      typedef typename DomainType::config_type                         DomainConfiguration;
      typedef typename DomainConfiguration::cell_tag                   CellTag;
      typedef typename result_of::ncell<DomainConfiguration, CellTag::dim>::type     CellType;
      
      data_accessor_wrapper<CellType> wrapper(new segment_vector_data_accessor<CellType, KeyType, DataType>(key));
      writer.add_normal_data_on_cells(wrapper, quantity_name);
      return writer;
    }





  } //namespace io
} //namespace viennagrid

#endif
