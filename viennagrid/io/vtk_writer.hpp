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

/** @file vtk_writer.hpp
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
      static void write( std::ostream & os, value_type value ) { os << value; }
    };

    template<>
    struct ValueTypeInformation< std::vector<double> >
    {
      typedef std::vector<double> value_type;

      static std::string type_name() { return "Float32"; }
      static int num_components() { return 3; }
      static void write( std::ostream & os, value_type const & value ) { os << value[0] << " " << value[1] << " " << value[2]; }
    };


    /////////////////// VTK export ////////////////////////////

    //helper: translate element tags to VTK-element types
    // (see: http://www.vtk.org/VTK/img/file-formats.pdf, page 9)

    /** @brief Main VTK writer class. Writes a domain or a segment to a file
     *
     * @tparam DomainType    Type of the ViennaGrid domain. Must not be a segment!
     */
    template < typename DomainType, typename SegmentationType = typename viennagrid::result_of::segmentation<DomainType>::type >
    class vtk_writer
    {
      public:

      protected:

        typedef typename SegmentationType::segment_id_type segment_id_type;

        typedef typename result_of::point<DomainType>::type PointType;
        typedef typename result_of::coord<PointType>::type CoordType;
        enum { dim = traits::static_size<PointType>::value };

        typedef typename result_of::cell_tag<DomainType>::type CellTag;
        typedef typename result_of::element<DomainType, CellTag>::type CellType;

        typedef typename result_of::element<DomainType, vertex_tag>::type                           VertexType;
        typedef typename result_of::handle<DomainType, vertex_tag>::type          VertexHandleType;
        typedef typename result_of::const_handle<DomainType, vertex_tag>::type          ConstVertexHandleType;

        typedef typename SegmentationType::segment_type SegmentType;


        typedef std::vector<double> vector_data_type;

        typedef std::map< std::string, base_dynamic_accessor_t<double, VertexType> * > VertexScalarOutputAccessorContainer;
        typedef std::map< std::string, base_dynamic_accessor_t<vector_data_type, VertexType> * > VertexVectorOutputAccessorContainer;

        typedef std::map< std::string, base_dynamic_accessor_t<double, CellType> * > CellScalarOutputAccessorContainer;
        typedef std::map< std::string, base_dynamic_accessor_t<vector_data_type, CellType> * > CellVectorOutputAccessorContainer;

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


          for (typename std::map< segment_id_type, VertexScalarOutputAccessorContainer>::iterator it = segment_vertex_scalar_data.begin(); it != segment_vertex_scalar_data.end(); ++it)
            clear_map(it->second);

          for (typename std::map< segment_id_type, VertexVectorOutputAccessorContainer>::iterator it = segment_vertex_vector_data.begin(); it != segment_vertex_vector_data.end(); ++it)
            clear_map(it->second);


          for (typename std::map< segment_id_type, CellScalarOutputAccessorContainer>::iterator it = segment_cell_scalar_data.begin(); it != segment_cell_scalar_data.end(); ++it)
            clear_map(it->second);

          for (typename std::map< segment_id_type, CellVectorOutputAccessorContainer>::iterator it = segment_cell_vector_data.begin(); it != segment_cell_vector_data.end(); ++it)
            clear_map(it->second);


          segment_vertex_scalar_data.clear();
          segment_vertex_vector_data.clear();

          segment_cell_scalar_data.clear();
          segment_cell_vector_data.clear();


          used_vertices.clear();
          vertex_to_index_map.clear();
        }

        /** @brief Writes the XML file header */
        void writeHeader(std::ofstream & writer)
        {
          writer << "<?xml version=\"1.0\"?>" << std::endl;
          writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
          writer << " <UnstructuredGrid>" << std::endl;
        }


        template<typename SegmentType>
        unsigned int preparePoints(SegmentType const & segment, segment_id_type seg_id)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, CellTag>::type     CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type                                         CellIterator;

          typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type      VertexOnCellRange;
          typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;

          std::deque< ConstVertexHandleType > & current_used_vertices = used_vertices[seg_id];
          std::map< ConstVertexHandleType, std::size_t > & current_vertex_to_index_map = vertex_to_index_map[seg_id];

          std::size_t index = 0;
          CellRange cells = viennagrid::elements(segment);

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
        void writePoints(SegmentType const & segment, std::ofstream & writer, segment_id_type seg_id)
        {
          std::deque< ConstVertexHandleType > & current_used_vertices = used_vertices[seg_id];

          writer << "   <Points>" << std::endl;
          writer << "    <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

          typedef typename viennagrid::result_of::const_vertex_range<SegmentType>::type VertexRange;
          typedef typename viennagrid::result_of::iterator<VertexRange>::type VertexIterator;

          for (typename std::deque< ConstVertexHandleType >::iterator it = current_used_vertices.begin(); it != current_used_vertices.end(); ++it)
          {
            PointWriter<dim>::write(writer, viennagrid::point(segment, *it) );

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
        void writeCells(DomainSegmentType const & domseg, std::ofstream & writer, segment_id_type seg_id)
        {
          typedef typename viennagrid::result_of::const_element_range<DomainSegmentType, CellTag>::type     CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type                                         CellIterator;

          typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type      VertexOnCellRange;
          typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
          typedef typename viennagrid::result_of::handle_iterator<VertexOnCellRange>::type         VertexHandleOnCellIterator;

          std::map< ConstVertexHandleType, std::size_t > & current_vertex_to_index_map = vertex_to_index_map[seg_id];

          writer << "   <Cells> " << std::endl;
          writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;
          CellRange cells = viennagrid::elements<CellTag>(domseg);
          for (CellIterator cit = cells.begin();
              cit != cells.end();
              ++cit)
          {
              //step 1: Write vertex indices in ViennaGrid orientation to array:
              std::vector<std::size_t> viennagrid_vertices(viennagrid::boundary_elements<CellTag, vertex_tag>::num);
              VertexOnCellRange vertices_on_cell = viennagrid::elements<vertex_tag>(*cit);
              std::size_t j = 0;
              for (VertexHandleOnCellIterator vocit = vertices_on_cell.handle_begin();
                  vocit != vertices_on_cell.handle_end();
                  ++vocit, ++j)
              {
                viennagrid_vertices[j] = current_vertex_to_index_map[*vocit];
              }

              //Step 2: Write the transformed connectivities:
              viennagrid_to_vtk_orientations<CellTag> reorderer;
              for (std::size_t i=0; i<viennagrid_vertices.size(); ++i)
                writer <<   viennagrid_vertices[reorderer(static_cast<long>(i))] << " ";

              writer << std::endl;
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;

            writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
            for (std::size_t offsets = 1;
                 offsets <= viennagrid::elements<CellTag>(domseg).size();
                 ++offsets)
            {
              writer << ( offsets * viennagrid::boundary_elements<CellTag, vertex_tag>::num) << " ";
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








        /** @brief Writes vector-valued data defined on vertices (points) to file */
        template <typename SegmentType, typename IOAccessorType>
        void writePointData(SegmentType const & segment, std::ofstream & writer, std::string const & name, IOAccessorType const & accessor, long seg_id)
        {
          typedef typename IOAccessorType::value_type ValueType;

          writer << "    <DataArray type=\"" << ValueTypeInformation<ValueType>::type_name() << "\" Name=\"" << name <<
            "\" NumberOfComponents=\"" << ValueTypeInformation<ValueType>::num_components() << "\" format=\"ascii\">" << std::endl;

          std::deque< ConstVertexHandleType > & current_used_vertices = used_vertices[ seg_id ];
          for (typename std::deque< ConstVertexHandleType >::iterator it = current_used_vertices.begin(); it != current_used_vertices.end(); ++it)
          {
            ValueTypeInformation<ValueType>::write(writer, accessor( viennagrid::dereference_handle(segment, *it) ));
            writer << " ";
          }
          writer << std::endl;

          writer << "    </DataArray>" << std::endl;
        } //writePointDataScalar


        /** @brief Writes vector-valued data defined on vertices (points) to file */
        template <typename SegmentType, typename IOAccessorType>
        void writeCellData(SegmentType const & segment, std::ofstream & writer, std::string const & name, IOAccessorType const & accessor)
        {
          typedef typename viennagrid::result_of::const_element_range<SegmentType, CellTag>::type   CellRange;
          typedef typename viennagrid::result_of::iterator<CellRange>::type                         CellIterator;

          typedef typename IOAccessorType::value_type ValueType;

          writer << "    <DataArray type=\"" << ValueTypeInformation<ValueType>::type_name() << "\" Name=\"" << name <<
            "\" NumberOfComponents=\"" << ValueTypeInformation<ValueType>::num_components() << "\" format=\"ascii\">" << std::endl;

          CellRange cells = viennagrid::elements(segment);
          for (CellIterator it = cells.begin(); it != cells.end(); ++it)
          {
            ValueTypeInformation<ValueType>::write(writer, accessor( *it));
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
         * @param domain     The ViennaGrid domain. Must not be a segment!
         * @param filename   The file to write to
         */
        int operator()(DomainType const & domain, std::string const & filename)
        {
            std::ofstream writer(filename.c_str());

            if (!writer){
              throw cannot_open_file_exception(filename);
              clear();
              return EXIT_FAILURE;
            }

            writeHeader(writer);

            segment_id_type tmp_id = segment_id_type();

            unsigned int num_points = preparePoints(domain, tmp_id);

            writer << "  <Piece NumberOfPoints=\""
                   << num_points
                   << "\" NumberOfCells=\""
                   << viennagrid::elements<CellTag>(domain).size()
                   << "\">" << std::endl;

            writePoints(domain, writer, tmp_id);

            if (vertex_scalar_data.size() > 0 || vertex_vector_data.size() > 0)
            {
              writer << "   <PointData>" << std::endl;

                for (typename VertexScalarOutputAccessorContainer::const_iterator it = vertex_scalar_data.begin(); it != vertex_scalar_data.end(); ++it)
                  writePointData( domain, writer, it->first, *(it->second), tmp_id );
                for (typename VertexVectorOutputAccessorContainer::const_iterator it = vertex_vector_data.begin(); it != vertex_vector_data.end(); ++it)
                  writePointData( domain, writer, it->first, *(it->second), tmp_id );

              writer << "   </PointData>" << std::endl;
            }

            writeCells(domain, writer, tmp_id);
            if (cell_scalar_data.size() > 0 || cell_vector_data.size() > 0)
            {
              writer << "   <CellData>" << std::endl;

                for (typename CellScalarOutputAccessorContainer::const_iterator it = cell_scalar_data.begin(); it != cell_scalar_data.end(); ++it)
                  writeCellData( domain, writer, it->first, *(it->second) );
                for (typename CellVectorOutputAccessorContainer::const_iterator it = cell_vector_data.begin(); it != cell_vector_data.end(); ++it)
                  writeCellData( domain, writer, it->first, *(it->second) );

              writer << "   </CellData>" << std::endl;
            }

            writer << "  </Piece>" << std::endl;
            writeFooter(writer);

          clear();
          return EXIT_SUCCESS;
        }


        int operator()(DomainType const & domain, SegmentationType const & segmentation, std::string const & filename)
        {
            if (segmentation.empty()) return (*this)(domain, filename);

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
                clear();
                return EXIT_FAILURE;
              }

              writer << "<?xml version=\"1.0\"?>" << std::endl;
              writer << "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">" << std::endl;
              writer << "<Collection>" << std::endl;

              for (typename SegmentationType::const_iterator it = segmentation.begin(); it != segmentation.end(); ++it)
              {
                SegmentType const & seg = *it;
                writer << "    <DataSet part=\"" << seg.id() << "\" file=\"" << short_filename << "_" << seg.id() << ".vtu\" name=\"Segment_" << seg.id() << "\"/>" << std::endl;
              }

              writer << "  </Collection>" << std::endl;
              writer << "</VTKFile>" << std::endl;
              writer.close();
            }

            //
            // Step 2: Write segments to individual files
            //

            for (typename SegmentationType::const_iterator it = segmentation.begin(); it != segmentation.end(); ++it)
            {
              SegmentType const & seg = *it;

              std::stringstream ss;
              ss << filename << "_" << seg.id() << ".vtu";
              std::ofstream writer(ss.str().c_str());
              writeHeader(writer);

              if (!writer)
              {
                clear();
                throw cannot_open_file_exception(ss.str());
                return EXIT_FAILURE;
              }

              typedef typename viennagrid::result_of::const_element_range<SegmentType, vertex_tag>::type   VertexRange;
              typedef typename viennagrid::result_of::iterator<VertexRange>::type               VertexIterator;

              unsigned int num_points = preparePoints(seg, seg.id());

              writer << "  <Piece NumberOfPoints=\""
                    << num_points
                    << "\" NumberOfCells=\""
                    << viennagrid::elements<CellTag>(seg).size()
                    << "\">" << std::endl;

              writePoints(seg, writer, seg.id());


              VertexScalarOutputAccessorContainer const & current_segment_vertex_scalar_data = segment_vertex_scalar_data[ seg.id() ];
              VertexVectorOutputAccessorContainer const & current_segment_vertex_vector_data = segment_vertex_vector_data[ seg.id() ];

              if (vertex_scalar_data.size() > 0 || vertex_vector_data.size() > 0 || current_segment_vertex_scalar_data.size() > 0 || current_segment_vertex_vector_data.size() > 0)
              {
                writer << "   <PointData>" << std::endl;

                for (typename VertexScalarOutputAccessorContainer::const_iterator it = vertex_scalar_data.begin(); it != vertex_scalar_data.end(); ++it)
                  writePointData( seg, writer, it->first, *(it->second), seg.id() );
                for (typename VertexVectorOutputAccessorContainer::const_iterator it = vertex_vector_data.begin(); it != vertex_vector_data.end(); ++it)
                  writePointData( seg, writer, it->first, *(it->second), seg.id() );


                for (typename VertexScalarOutputAccessorContainer::const_iterator it = current_segment_vertex_scalar_data.begin(); it != current_segment_vertex_scalar_data.end(); ++it)
                  writePointData( seg, writer, it->first, *(it->second), seg.id() );

                for (typename VertexVectorOutputAccessorContainer::const_iterator it = current_segment_vertex_vector_data.begin(); it != current_segment_vertex_vector_data.end(); ++it)
                  writePointData( seg, writer, it->first, *(it->second), seg.id() );

                writer << "   </PointData>" << std::endl;
              }

              writeCells(seg, writer, seg.id());

              CellScalarOutputAccessorContainer const & current_segment_cell_scalar_data = segment_cell_scalar_data[ seg.id() ];
              CellVectorOutputAccessorContainer const & current_segment_cell_vector_data = segment_cell_vector_data[ seg.id() ];
              if (cell_scalar_data.size() > 0 || cell_vector_data.size() > 0 || current_segment_cell_scalar_data.size() > 0 || current_segment_cell_vector_data.size() > 0)
              {
                writer << "   <CellData>" << std::endl;

                for (typename CellScalarOutputAccessorContainer::const_iterator it = cell_scalar_data.begin(); it != cell_scalar_data.end(); ++it)
                  writeCellData( seg, writer, it->first, *(it->second) );
                for (typename CellVectorOutputAccessorContainer::const_iterator it = cell_vector_data.begin(); it != cell_vector_data.end(); ++it)
                  writeCellData( seg, writer, it->first, *(it->second) );


                for (typename CellScalarOutputAccessorContainer::const_iterator it = current_segment_cell_scalar_data.begin(); it != current_segment_cell_scalar_data.end(); ++it)
                  writeCellData( seg, writer, it->first, *(it->second) );
                for (typename CellVectorOutputAccessorContainer::const_iterator it = current_segment_cell_vector_data.begin(); it != current_segment_cell_vector_data.end(); ++it)
                  writeCellData( seg, writer, it->first, *(it->second) );

                writer << "   </CellData>" << std::endl;
              }

              writer << "  </Piece>" << std::endl;
              writeFooter(writer);
              writer.close();
            }

          clear();
          return EXIT_SUCCESS;
        }



    private:


      template<typename MapType, typename AccessorType>
      void add_to_container(MapType & map, AccessorType const accessor, std::string const & name)
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


        /** @brief Specify scalar-valued data for vertices to the writer. Prefer the free function add_scalar_data_on_vertices()
         *
         * @tparam T       Anything that can be wrapped by a data_accessor_wrapper
         * @param  accessor The quantity accessor
         * @param  name    The quantity name that should appear in the VTK file
         */
        template <typename AccessorType>
        void add_scalar_data_on_vertices(AccessorType const accessor, std::string const & name)
        { add_to_container(vertex_scalar_data, accessor, name); }

        template <typename AccessorType>
        void add_scalar_data_on_vertices(segment_id_type seg_id, AccessorType const accessor, std::string const &  name)
        { add_to_container(segment_vertex_scalar_data[seg_id], accessor, name); }

        template <typename AccessorType>
        void add_scalar_data_on_vertices(SegmentType const & segment, AccessorType const accessor, std::string const &  name)
        { add_scalar_data_on_vertices(segment.id(), accessor, name); }


        template <typename AccessorType>
        void add_vector_data_on_vertices(AccessorType const accessor, std::string const & name)
        { add_to_container(vertex_vector_data, accessor, name); }

        template <typename AccessorType>
        void add_vector_data_on_vertices(segment_id_type seg_id, AccessorType const accessor, std::string const &  name)
        { add_to_container(segment_vertex_vector_data[seg_id], accessor, name); }

        template <typename AccessorType>
        void add_vector_data_on_vertices(SegmentType const & segment, AccessorType const accessor, std::string const &  name)
        { add_vector_data_on_vertices(segment.id(), accessor, name); }



        /** @brief Specify scalar-valued data for cells to the writer. Prefer the free function add_scalar_data_on_cells()
         *
         * @tparam T       Anything that can be wrapped by a data_accessor_wrapper
         * @param  accessor The quantity accessor
         * @param  name    The quantity name that should appear in the VTK file
         */
        template <typename AccessorType>
        void add_scalar_data_on_cells(AccessorType const accessor, std::string name)
        { add_to_container(cell_scalar_data, accessor, name); }

        template <typename AccessorType>
        void add_scalar_data_on_cells(segment_id_type seg_id, AccessorType const accessor, std::string const &  name)
        { add_to_container(segment_cell_scalar_data[seg_id], accessor, name); }

        template <typename AccessorType>
        void add_scalar_data_on_cells(SegmentType const & segment, AccessorType const accessor, std::string const &  name)
        { add_scalar_data_on_cells(segment.id(), accessor, name); }


        template <typename AccessorType>
        void add_vector_data_on_cells(AccessorType const accessor, std::string name)
        { add_to_container(cell_vector_data, accessor, name); }

        template <typename AccessorType>
        void add_vector_data_on_cells(segment_id_type seg_id, AccessorType const accessor, std::string const &  name)
        { add_to_container(segment_cell_vector_data[seg_id], accessor, name); }

        template <typename AccessorType>
        void add_vector_data_on_cells(SegmentType const & segment, AccessorType const accessor, std::string const &  name)
        { add_vector_data_on_cells(segment.id(), accessor, name); }


      private:

        std::map< segment_id_type, std::map< ConstVertexHandleType, std::size_t > >            vertex_to_index_map;
        std::map< segment_id_type, std::deque< ConstVertexHandleType> >                        used_vertices;


        VertexScalarOutputAccessorContainer          vertex_scalar_data;
        VertexVectorOutputAccessorContainer          vertex_vector_data;

        CellScalarOutputAccessorContainer          cell_scalar_data;
        CellVectorOutputAccessorContainer          cell_vector_data;

        std::map< segment_id_type, VertexScalarOutputAccessorContainer > segment_vertex_scalar_data;
        std::map< segment_id_type, VertexVectorOutputAccessorContainer > segment_vertex_vector_data;

        std::map< segment_id_type, CellScalarOutputAccessorContainer >   segment_cell_scalar_data;
        std::map< segment_id_type, CellVectorOutputAccessorContainer >   segment_cell_vector_data;
    };

    /** @brief Convenience function that exports a domain to file directly. Does not export quantities */
    template < typename DomainType, typename SegmentationType >
    int export_vtk(DomainType const & domain, SegmentationType const & segmentation, std::string const & filename)
    {
      vtk_writer<DomainType> vtk_writer;
      return vtk_writer(domain, segmentation, filename);
    }

    template < typename DomainType >
    int export_vtk(DomainType const & domain, std::string const & filename)
    {
      vtk_writer<DomainType> vtk_writer;
      return vtk_writer(domain, filename);
    }

  } //namespace io
} //namespace viennagrid

#endif
