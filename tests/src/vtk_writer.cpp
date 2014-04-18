/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include <iostream>
#include <ostream>

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/netgen_reader.hpp"



template <typename MeshType>
void test(std::string & infile, std::string & outfile)
{

  typedef typename viennagrid::result_of::segmentation<MeshType>::type           SegmentationType;
  typedef typename viennagrid::result_of::segment_handle<SegmentationType>::type           SegmentHandleType;
  typedef typename SegmentHandleType::segment_id_type SegmentIDType;

  typedef typename viennagrid::result_of::vertex<MeshType>::type       VertexType;
  typedef typename viennagrid::result_of::cell<MeshType>::type   CellType;

  typedef typename viennagrid::result_of::vertex_range<MeshType>::type           VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;

  typedef typename viennagrid::result_of::cell_range<MeshType>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;

  typedef typename viennagrid::result_of::vertex_range<SegmentHandleType>::type                 SegmentVertexContainer;
  typedef typename viennagrid::result_of::iterator<SegmentVertexContainer>::type        SegmentVertexIterator;

  typedef typename viennagrid::result_of::cell_range<SegmentHandleType>::type     SegmentCellContainer;
  typedef typename viennagrid::result_of::iterator<SegmentCellContainer>::type                    SegmentCellIterator;


  MeshType mesh;
  SegmentationType segmentation(mesh);

  try{
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(mesh, segmentation, infile);
  } catch (std::exception const & ex){
    std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
//   exit(EXIT_FAILURE);
  }



  std::deque<double>            vertex_double_data;
  std::deque< std::vector<double> >    vertex_vector_data;
  std::deque< std::vector<double> >    vertex_normal_data;

  typename viennagrid::result_of::field< std::deque<double>, VertexType >::type            vertex_double_data_field( vertex_double_data );
  typename viennagrid::result_of::field< std::deque< std::vector<double> >, VertexType >::type    vertex_vector_data_field( vertex_vector_data );
  typename viennagrid::result_of::field< std::deque< std::vector<double> >, VertexType >::type    vertex_normal_data_field( vertex_normal_data );


  std::deque<double>            cell_double_data;
  std::deque< std::vector<double> >    cell_vector_data;
  std::deque< std::vector<double> >    cell_normal_data;

  typename viennagrid::result_of::field< std::deque<double>, CellType >::type            cell_double_data_field( cell_double_data );
  typename viennagrid::result_of::field< std::deque< std::vector<double> >, CellType >::type    cell_vector_data_field( cell_vector_data );
  typename viennagrid::result_of::field< std::deque< std::vector<double> >, CellType >::type    cell_normal_data_field( cell_normal_data );

  // write global data to vertices
  VertexContainer vertices(mesh);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {
    vertex_double_data_field(*vit) = viennagrid::point(*vit)[0];
    vertex_vector_data_field(*vit) = std::vector<double>(3);
    vertex_normal_data_field(*vit) = std::vector<double>(3);
  }

  CellContainer cells(mesh);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    cell_double_data_field(*cit) = viennagrid::circumcenter(*cit)[0];
    cell_vector_data_field(*cit) = std::vector<double>(3);
    cell_normal_data_field(*cit) = std::vector<double>(3);
  }






  std::map<SegmentIDType, std::deque<double> >            segment_vertex_double_data;
  std::map<SegmentIDType, std::deque< std::vector<double> > >    segment_vertex_vector_data;
  std::map<SegmentIDType, std::deque< std::vector<double> > >    segment_vertex_normal_data;

  std::map<SegmentIDType, std::deque<double> >            segment_cell_double_data;
  std::map<SegmentIDType, std::deque< std::vector<double> > >    segment_cell_vector_data;
  std::map<SegmentIDType, std::deque< std::vector<double> > >    segment_cell_normal_data;





  // write segment-based data
  int index = 0;
  for (typename SegmentationType::iterator it = segmentation.begin(); it != segmentation.end(); ++it, ++index)
  {
    std::cout << "Writing segment " << it->id() << std::endl;

    typename viennagrid::result_of::field< std::deque<double>, VertexType >::type            segment_vertex_double_data_field( segment_vertex_double_data[it->id()] );
    typename viennagrid::result_of::field< std::deque< std::vector<double> >, VertexType >::type    segment_vertex_vector_data_field( segment_vertex_vector_data[it->id()] );
    typename viennagrid::result_of::field< std::deque< std::vector<double> >, VertexType >::type    segment_vertex_normal_data_field( segment_vertex_normal_data[it->id()] );

    typename viennagrid::result_of::field< std::deque<double>, CellType >::type            segment_cell_double_data_field( segment_cell_double_data[it->id()] );
    typename viennagrid::result_of::field< std::deque< std::vector<double> >, CellType >::type    segment_cell_vector_data_field( segment_cell_vector_data[it->id()] );
    typename viennagrid::result_of::field< std::deque< std::vector<double> >, CellType >::type    segment_cell_normal_data_field( segment_cell_normal_data[it->id()] );


    SegmentVertexContainer segment_vertices(*it);
    for (SegmentVertexIterator vit = segment_vertices.begin();
        vit != segment_vertices.end();
        ++vit)
    {
      segment_vertex_double_data_field(*vit) = index+1;
      segment_vertex_vector_data_field(*vit) = std::vector<double>(3, index);
      segment_vertex_normal_data_field(*vit) = std::vector<double>(3, index);
    }

    SegmentCellContainer segment_cells(*it);
    for (SegmentCellIterator cit = segment_cells.begin();
                      cit != segment_cells.end();
                    ++cit)
    {
      segment_cell_double_data_field(*cit) = viennagrid::circumcenter(*cit)[0] + index;
      segment_cell_vector_data_field(*cit) = std::vector<double>(3, index);
      segment_cell_normal_data_field(*cit) = std::vector<double>(3, index);
    }
  }




  //now setup and run the VTK writer:

  viennagrid::io::vtk_writer<MeshType> vtk_writer;

  viennagrid::io::add_scalar_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(vertex_normal_data), "point_normal_global");

  for (typename SegmentationType::iterator it = segmentation.begin(); it != segmentation.end(); ++it)
  {
    viennagrid::io::add_scalar_data_on_vertices(vtk_writer, *it, viennagrid::make_field<VertexType>(segment_vertex_double_data[it->id()]), "point_scalar1_segment");
    viennagrid::io::add_vector_data_on_vertices(vtk_writer, *it, viennagrid::make_field<VertexType>(segment_vertex_vector_data[it->id()]), "point_vector_segment");
    viennagrid::io::add_vector_data_on_vertices(vtk_writer, *it, viennagrid::make_field<VertexType>(segment_vertex_normal_data[it->id()]), "point_normal_segment");
  }

  viennagrid::io::add_scalar_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(cell_normal_data), "point_normal_global");

  for (typename SegmentationType::iterator it = segmentation.begin(); it != segmentation.end(); ++it)
  {
    viennagrid::io::add_scalar_data_on_cells(vtk_writer, *it, viennagrid::make_field<CellType>(segment_cell_double_data[it->id()]), "point_scalar1_segment");
    viennagrid::io::add_vector_data_on_cells(vtk_writer, *it, viennagrid::make_field<CellType>(segment_cell_vector_data[it->id()]), "point_vector_segment");
    viennagrid::io::add_vector_data_on_cells(vtk_writer, *it, viennagrid::make_field<CellType>(segment_cell_normal_data[it->id()]), "point_normal_segment");
  }

  vtk_writer(mesh, segmentation, outfile);




  //
  // Test for vtk reader: Read everything again
  //

  std::cout << "Reading and writing the whole data again... (pass 1)" << std::endl;
  MeshType mesh2;
  SegmentationType segmentation2(mesh2);

  viennagrid::io::vtk_reader<MeshType> vtk_reader;


  std::deque<double>            pass1_vertex_double_data;
  std::deque< std::vector<double> >    pass1_vertex_vector_data;
  std::deque< std::vector<double> >    pass1_vertex_normal_data;

  std::deque<double>            pass1_cell_double_data;
  std::deque< std::vector<double> >    pass1_cell_vector_data;
  std::deque< std::vector<double> >    pass1_cell_normal_data;

  viennagrid::io::add_scalar_data_on_vertices(vtk_reader, viennagrid::make_field<VertexType>(pass1_vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_reader, viennagrid::make_field<VertexType>(pass1_vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_reader, viennagrid::make_field<VertexType>(pass1_vertex_normal_data), "point_normal_global");

  viennagrid::io::add_scalar_data_on_cells(vtk_reader, viennagrid::make_field<CellType>(pass1_cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_reader, viennagrid::make_field<CellType>(pass1_cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_reader, viennagrid::make_field<CellType>(pass1_cell_normal_data), "point_normal_global");

  vtk_reader(mesh2, segmentation2, outfile + "_main.pvd");


  viennagrid::io::add_scalar_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(pass1_vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(pass1_vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(pass1_vertex_normal_data), "point_normal_global");

  for (typename SegmentationType::iterator it = segmentation2.begin(); it != segmentation2.end(); ++it)
  {
    vtk_writer.add_scalar_data_on_vertices( *it, vtk_reader.vertex_scalar_field("point_scalar1_segment", *it), "point_scalar1_segment" );
    vtk_writer.add_vector_data_on_vertices( *it, vtk_reader.vertex_vector_field("point_vector_segment", *it), "point_vector_segment" );
    vtk_writer.add_vector_data_on_vertices( *it, vtk_reader.vertex_vector_field("point_normal_segment", *it), "point_normal_segment" );
  }

  viennagrid::io::add_scalar_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(pass1_cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(pass1_cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(pass1_cell_normal_data), "point_normal_global");

  for (typename SegmentationType::iterator it = segmentation2.begin(); it != segmentation2.end(); ++it)
  {
    vtk_writer.add_scalar_data_on_cells( *it, vtk_reader.cell_scalar_field("point_scalar1_segment", *it), "point_scalar1_segment" );
    vtk_writer.add_vector_data_on_cells( *it, vtk_reader.cell_vector_field("point_vector_segment", *it), "point_vector_segment" );
    vtk_writer.add_vector_data_on_cells( *it, vtk_reader.cell_vector_field("point_normal_segment", *it), "point_normal_segment" );
  }


  std::string outfile2 = outfile + "2";
  vtk_writer(mesh2, segmentation2, outfile2);







  std::cout << "Reading and writing the whole data again... (pass 2)" << std::endl;
  MeshType mesh3;
  SegmentationType segmentation3(mesh3);

  std::deque<double>            pass2_vertex_double_data;
  std::deque< std::vector<double> >    pass2_vertex_vector_data;
  std::deque< std::vector<double> >    pass2_vertex_normal_data;

  std::deque<double>            pass2_cell_double_data;
  std::deque< std::vector<double> >    pass2_cell_vector_data;
  std::deque< std::vector<double> >    pass2_cell_normal_data;

  viennagrid::io::add_scalar_data_on_vertices(vtk_reader, viennagrid::make_field<VertexType>(pass2_vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_reader, viennagrid::make_field<VertexType>(pass2_vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_reader, viennagrid::make_field<VertexType>(pass2_vertex_normal_data), "point_normal_global");

  viennagrid::io::add_scalar_data_on_cells(vtk_reader, viennagrid::make_field<CellType>(pass2_cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_reader, viennagrid::make_field<CellType>(pass2_cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_reader, viennagrid::make_field<CellType>(pass2_cell_normal_data), "point_normal_global");

  vtk_reader(mesh3, segmentation3, outfile + "2_main.pvd");


  viennagrid::io::add_scalar_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(pass2_vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(pass2_vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_field<VertexType>(pass2_vertex_normal_data), "point_normal_global");

  for (typename SegmentationType::iterator it = segmentation3.begin(); it != segmentation3.end(); ++it)
  {
    vtk_writer.add_scalar_data_on_vertices( *it, vtk_reader.vertex_scalar_field("point_scalar1_segment", *it), "point_scalar1_segment" );
    vtk_writer.add_vector_data_on_vertices( *it, vtk_reader.vertex_vector_field("point_vector_segment", *it), "point_vector_segment" );
    vtk_writer.add_vector_data_on_vertices( *it, vtk_reader.vertex_vector_field("point_normal_segment", *it), "point_normal_segment" );
  }

  viennagrid::io::add_scalar_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(pass2_cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(pass2_cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_field<CellType>(pass2_cell_normal_data), "point_normal_global");

  for (typename SegmentationType::iterator it = segmentation3.begin(); it != segmentation3.end(); ++it)
  {
    vtk_writer.add_scalar_data_on_cells( *it, vtk_reader.cell_scalar_field("point_scalar1_segment", *it), "point_scalar1_segment" );
    vtk_writer.add_vector_data_on_cells( *it, vtk_reader.cell_vector_field("point_vector_segment", *it), "point_vector_segment" );
    vtk_writer.add_vector_data_on_cells( *it, vtk_reader.cell_vector_field("point_normal_segment", *it), "point_normal_segment" );
  }


  std::string outfile3 = outfile + "3";
  vtk_writer(mesh3, segmentation3, outfile3);
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::string path = "../examples/data/";

  std::string infile = path + "twocubes.mesh";
  std::string outfile = "vtk_writer"; // without ending

  std::cout << "Running VTK writer on tetrahedron mesh... " << std::endl;
  test<viennagrid::tetrahedral_3d_mesh>(infile, outfile);

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
