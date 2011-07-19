/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Markus Bina                        bina@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_IO_VTK_WRITER_GUARD
#define VIENNAGRID_IO_VTK_WRITER_GUARD

#include <fstream>
#include <sstream>
#include <iostream>
#include "viennagrid/domain.hpp"
#include "viennagrid/forwards.h"
#include "viennagrid/iterators.hpp"
#include "viennagrid/io/helper.hpp"
#include "viennagrid/io/vtk_tags.hpp"
//#include "viennagrid/algorithm/cell_normals.hpp"
#include "viennadata/api.hpp"

namespace viennagrid
{
  namespace io
  {
    //
    // helper: get id from a vertex relative to the segment or the domain:
    //
    struct vtk_get_id
    {
      template <typename VertexType, typename T>
      static long apply(VertexType const & v, T const & seg)
      {
        //obtain local segment numbering from ViennaData:
        return viennadata::access<segment_mapping_key<T>, long>(seg)(v);
      }
      
      //special handling for domain:
      template <typename VertexType, typename ConfigType>
      static long apply(VertexType const & v, viennagrid::domain<ConfigType> const & dom)
      {
        return v.getID();
      }
      
    };
    
    
    
    //
    // type erasure
    //
    template <typename ElementType>      //either cell type or vertex type
    class io_data_accessor_interface
    {
      public: 
        virtual std::string operator()(ElementType const & element, size_t segment_id) const = 0;
        
        virtual bool active_on_segment(size_t i) const { return true; }
        
        virtual io_data_accessor_interface<ElementType> * clone() const = 0;
    };

    
    
    //
    // Use case 1: Access data for all elements
    //
    template <typename ElementType, typename KeyType, typename DataType>
    class io_data_accessor_global : public io_data_accessor_interface<ElementType>
    {
      typedef io_data_accessor_global<ElementType, KeyType, DataType>    self_type;
      
      public:
        io_data_accessor_global(KeyType const & k) : key_(k) {}
        //vtk_data_accessor_global() {}

        std::string operator()(ElementType const & element, size_t segment_id) const
        {
          std::stringstream ss;
          ss << viennadata::access<KeyType, DataType>(key_)(element);
          return ss.str();
        }
        
        io_data_accessor_interface<ElementType> * clone() const { return new self_type(key_); }
        
      private:
        KeyType key_;
    };
    
    
    //
    // Use case 1: Access data for all elements
    //
    template <typename ElementType, typename KeyType, typename DataType>
    class io_data_accessor_segment_based : public io_data_accessor_interface<ElementType>
    {
      typedef io_data_accessor_segment_based<ElementType, KeyType, DataType>    self_type;
      
      public:
        io_data_accessor_segment_based(KeyType const & k) : key_(k) {}
        //vtk_data_accessor_global() {}

        std::string operator()(ElementType const & element, size_t segment_id) const
        {
          std::stringstream ss;
          ss << viennadata::access<KeyType, DataType>(key_)(element)[segment_id];
          return ss.str();
        }
        
        io_data_accessor_interface<ElementType> * clone() const { return new self_type(key_); }
        
      private:
        KeyType key_;
    };

    //
    // The final wrapper class for any IO implementations
    //
    template <typename ElementType>
    class io_data_accessor_wrapper
    {
      public:
        template <typename T>
        io_data_accessor_wrapper(T const * t) : functor_(t) {}
        
        io_data_accessor_wrapper() {}

        io_data_accessor_wrapper(const io_data_accessor_wrapper & other) : functor_(other.clone()) {}

        io_data_accessor_wrapper & operator=(io_data_accessor_wrapper & other)
        {
          functor_ = other.functor_;
          return *this;
        }
        
        std::string operator()(ElementType const & element, size_t segment_id = 0) const
        {
          return functor_->operator()(element, segment_id); 
        }
        
        io_data_accessor_interface<ElementType> * clone() const { return functor_->clone(); }

      private:
        std::auto_ptr< const io_data_accessor_interface<ElementType> > functor_;
    };
      
      
    
    
    
    /////////////////// VTK export ////////////////////////////

    //helper: translate element tags to VTK-element types
    // (see: http://www.vtk.org/VTK/img/file-formats.pdf, page 9)

    template < typename DomainType >
    class vtk_writer
    {
      protected:

        typedef typename DomainType::config_type                         DomainConfiguration;

        typedef typename DomainConfiguration::numeric_type               CoordType;
        typedef typename DomainConfiguration::dimension_tag              DimensionTag;
        typedef typename DomainConfiguration::cell_tag                   CellTag;

        typedef typename result_of::point_type<DomainConfiguration>::type                              PointType;
        typedef typename result_of::ncell_type<DomainConfiguration, 0>::type                           VertexType;
        typedef typename result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type     CellType;


        void writeHeader(std::ofstream & writer)
        {
          writer << "<?xml version=\"1.0\"?>" << std::endl;
          writer << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
          writer << " <UnstructuredGrid>" << std::endl;
        }

        template <typename SegmentType>
        void writePoints(SegmentType const & segment, std::ofstream & writer)
        {
          typedef typename viennagrid::result_of::const_ncell_container<SegmentType, 0>::type   VertexContainer;
          typedef typename viennagrid::result_of::iterator<VertexContainer>::type               VertexIterator;
          
          writer << "   <Points>" << std::endl;
          writer << "    <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

          VertexContainer vertices = viennagrid::ncells<0>(segment);
          for (VertexIterator vit = vertices.begin();
              vit != vertices.end();
              ++vit)
          {
            PointWriter<DimensionTag::value>::write(writer, vit->getPoint());

            // add 0's for less than three dimensions
              if (DimensionTag::value == 2)
                writer << " " << 0;
              if(DimensionTag::value == 1)
                writer << " " << 0 << " " << 0;

              writer << std::endl;
          }
          writer << std::endl;
          writer << "    </DataArray>" << std::endl;
          writer << "   </Points> " << std::endl;
        } //writePoints()

        template <typename SegmentType>
        void writeCells(SegmentType const & segment, std::ofstream & writer)
        {
          typedef typename viennagrid::result_of::const_ncell_container<SegmentType, CellTag::topology_level>::type     CellContainer;
          typedef typename viennagrid::result_of::iterator<CellContainer>::type                                         CellIterator;

          typedef typename viennagrid::result_of::const_ncell_container<CellType, 0>::type      VertexOnCellContainer;
          typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;
          
          writer << "   <Cells> " << std::endl;
          writer << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;
          CellContainer cells = viennagrid::ncells<CellTag::topology_level>(segment);
          for (CellIterator cit = cells.begin();
              cit != cells.end();
              ++cit)
          {
              VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(*cit);
              for (VertexOnCellIterator vocit = vertices_on_cell.begin();
                  vocit != vertices_on_cell.end();
                  ++vocit)
              {
                writer << vtk_get_id::apply(*vocit, segment) << " ";
              }
              writer << std::endl;
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;

            writer << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
            for (size_t offsets = 1;
                offsets <= segment.template size<CellTag::topology_level>();
                ++offsets)
            {
              writer << ( offsets * viennagrid::traits::subcell_desc<CellTag, 0>::num_elements) << " ";
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;

            writer << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
            for (size_t offsets = 1;
                  offsets <= segment.template size<CellTag::topology_level>();
                  ++offsets)
            {
              writer << ELEMENT_TAG_TO_VTK_TYPE<CellTag>::ReturnValue << " ";
            }
            writer << std::endl;
            writer << "    </DataArray>" << std::endl;
            writer << "   </Cells>" << std::endl;
        }

        template <typename SegmentType>
        void writePointDataScalar(SegmentType const & segment, std::ofstream & writer)
        {
          typedef typename viennagrid::result_of::const_ncell_container<SegmentType, 0>::type   VertexContainer;
          typedef typename viennagrid::result_of::iterator<VertexContainer>::type               VertexIterator;
          
          if (point_data_scalar.size() > 0)
          {
            writer << "   <PointData Scalars=\"scalars\">" << std::endl;
            for (size_t i=0; i<point_data_scalar.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << point_data_scalar_names[i] << "\" format=\"ascii\">" << std::endl;
              
              VertexContainer vertices = viennagrid::ncells<0>(segment);
              for (VertexIterator vit = vertices.begin();
                  vit != vertices.end();
                  ++vit)
              {
                writer << point_data_scalar[i](*vit, 0) << " ";
              }
              writer << std::endl;
              writer << "    </DataArray>" << std::endl;
            }
            writer << "   </PointData>"  << std::endl;
          }
        } //writePointData

        template <typename SegmentType>
        void writeCellDataNormals(SegmentType const & segment, std::ofstream & writer, std::size_t seg_id = 0)
        {
          typedef typename viennagrid::result_of::const_ncell_container<SegmentType, CellTag::topology_level>::type     CellContainer;
          typedef typename viennagrid::result_of::iterator<CellContainer>::type                                         CellIterator;
          
          if (cell_data_normal.size() > 0)
          {
            writer << "   <CellData Normals=\"cell_normals\">" << std::endl;
            
            for (size_t i=0; i<cell_data_normal.size(); ++i)
            {
              writer << "    <DataArray type=\"Float32\" Name=\"" << cell_data_normal_names[i] << "\" NumberOfComponents=\"3\" format=\"ascii\">" << std::endl;

              CellContainer cells = viennagrid::ncells<CellTag::topology_level>(segment);
              for (CellIterator cit = cells.begin();
                  cit != cells.end();
                  ++cit)
              {
                 viennagrid::io::PointWriter<3>::write(writer, cell_data_normal[i](*cit, seg_id));
                 writer << std::endl;
                //writer << cell_data_normal[i](*cit, seg_id) << std::endl;
              }
            }
            writer << "    </DataArray>" << std::endl;
            writer << "   </CellData>"  << std::endl;
          }
        } //writeCellData

        void writeFooter(std::ofstream & writer)
        {
          writer << " </UnstructuredGrid>" << std::endl;
          writer << "</VTKFile>" << std::endl;
        } 

      public:

        int writeDomain(DomainType const & domain, std::string const & filename)
        {
          return this->operator()(domain, filename);
        }

        int operator()(DomainType const & domain, std::string const & filename)
        {

          long segment_num = domain.segment_size();
          if (segment_num > 0)
          {
            //
            // Step 1: Write meta information
            //
            {
              std::stringstream ss;
              ss << filename << "_main.pvd";
              std::ofstream writer(ss.str().c_str());
              
              writer << "<?xml version=\"1.0\"?>" << std::endl;
              writer << "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">" << std::endl;
              writer << "<Collection>" << std::endl;
              
              
              for (long i = 0; i<segment_num; ++i)
                writer << "    <DataSet part=\"" << i << "\" file=\"" << filename << "_" << i << ".vtu\" name=\"Segment_" << i << "\"/>" << std::endl;
                
                
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
              
              typedef typename DomainType::segment_type                                             SegmentType;
              typedef typename viennagrid::result_of::const_ncell_container<SegmentType, 0>::type   VertexContainer;
              typedef typename viennagrid::result_of::iterator<VertexContainer>::type               VertexIterator;
              //std::cout << "Writing segment" << std::endl;
              //Segment & curSeg = *segit;

              SegmentType const & seg = domain.segment(i);
              
              //setting local segment numbering on vertices:
              long current_id = 0;
              VertexContainer vertices = viennagrid::ncells<0>(seg);
              for (VertexIterator vit = vertices.begin();
                  vit != vertices.end();
                  ++vit)
              {
                viennadata::access<segment_mapping_key<SegmentType>, long>(seg)(*vit) = current_id++;
              }

              writer << "  <Piece NumberOfPoints=\""
                    << seg.template size<0>()
                    << "\" NumberOfCells=\""
                    << seg.template size<CellTag::topology_level>()
                    << "\">" << std::endl;


              writePoints(seg, writer);
              writePointDataScalar(seg, writer);

              writeCells(seg, writer);
              writeCellDataNormals(seg, writer, i);

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
                  << domain.template size<0>()
                  << "\" NumberOfCells=\""
                  << domain.template size<CellTag::topology_level>()
                  << "\">" << std::endl;


            writePoints(domain, writer);
            writePointDataScalar(domain, writer);

            writeCells(domain, writer);
            writeCellDataNormals(domain, writer);

            writer << "  </Piece>" << std::endl;
            writeFooter(writer);

          }
          
          return EXIT_SUCCESS;
        }
        
        
        //
        // Add point data writer:
        //
        template <typename KeyType, typename DataType>
        void add_point_data_scalar(KeyType const & key, std::string name)
        {
          io_data_accessor_wrapper<VertexType> wrapper(new io_data_accessor_global<VertexType, KeyType, DataType>(key));
          point_data_scalar.push_back(wrapper);
          point_data_scalar_names.push_back(name);
        }

        template <typename T>
        void add_point_data_scalar(T const & accessor, std::string name)
        {
          io_data_accessor_wrapper<VertexType> wrapper(accessor.clone());
          point_data_scalar.push_back(wrapper);
          point_data_scalar_names.push_back(name);
        }

        //
        // Add cell data normals:
        //
        template <typename KeyType, typename DataType>
        void add_cell_data_normal(KeyType const & key, std::string name)
        {
          io_data_accessor_wrapper<CellType> wrapper(new io_data_accessor_global<CellType, KeyType, DataType>(key));
          cell_data_normal.push_back(wrapper);
          cell_data_normal_names.push_back(name);
        }        
        
        template <typename T>
        void add_cell_data_normal(T const & accessor, std::string name)
        {
          io_data_accessor_wrapper<CellType> wrapper(accessor.clone());
          cell_data_normal.push_back(wrapper);
          cell_data_normal_names.push_back(name);
        }

      private:
        std::vector< io_data_accessor_wrapper<VertexType> >    point_data_scalar;
        std::vector< std::string >                             point_data_scalar_names;
        
        std::vector< io_data_accessor_wrapper<CellType> >      cell_data_scalar;
        std::vector< std::string >                             cell_data_scalar_names;
        
        std::vector< io_data_accessor_wrapper<CellType> >      cell_data_vector;
        std::vector< std::string >                             cell_data_vector_names;
        
        std::vector< io_data_accessor_wrapper<CellType> >      cell_data_normal;
        std::vector< std::string >                             cell_data_normal_names;
        
    };

    template < typename DomainType > 
    int export_vtk(DomainType const& domain, std::string const & filename)
    {
 
      vtk_writer<DomainType> vtk_writer;
      return vtk_writer.writeDomain(domain, filename);
    }

  } //namespace io
} //namespace viennagrid

#endif
