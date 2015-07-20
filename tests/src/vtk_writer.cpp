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

#include "viennagridpp/core.hpp"
#include "viennagridpp/algorithm/volume.hpp"
#include "viennagridpp/algorithm/circumcenter.hpp"
#include "viennagridpp/io/vtk_writer.hpp"
#include "viennagridpp/io/vtk_reader.hpp"
#include "viennagridpp/io/netgen_reader.hpp"



template<int dimension, typename CellTagT>
void test(std::string & infile, std::string & outfile)
{
  typedef viennagrid::mesh                                                            MeshType;
  typedef viennagrid::result_of::region<MeshType>::type                               RegionType;
  typedef viennagrid::result_of::id<RegionType>::type                                 RegionIDType;

  typedef typename viennagrid::result_of::element<MeshType>::type                     VertexType;
  typedef typename viennagrid::result_of::element<MeshType>::type                     CellType;

  typedef typename viennagrid::result_of::vertex_range<MeshType>::type                VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type             VertexIterator;

  typedef typename viennagrid::result_of::cell_range<MeshType>::type                  CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type               CellIterator;

  typedef typename viennagrid::result_of::vertex_range<RegionType>::type              RegionVertexContainer;
  typedef typename viennagrid::result_of::iterator<RegionVertexContainer>::type       RegionVertexIterator;

  typedef typename viennagrid::result_of::cell_range<RegionType>::type                RegionCellContainer;
  typedef typename viennagrid::result_of::iterator<RegionCellContainer>::type         RegionCellIterator;


  MeshType mesh;

  try{
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(mesh, infile);
  } catch (std::exception const & ex){
    std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
//   exit(EXIT_FAILURE);
  }



  std::deque<double>            vertex_double_data;
  std::deque< std::vector<double> >    vertex_vector_data;
  std::deque< std::vector<double> >    vertex_normal_data;

  typename viennagrid::result_of::accessor< std::deque<double>, VertexType >::type            vertex_double_data_accessor( vertex_double_data );
  typename viennagrid::result_of::accessor< std::deque< std::vector<double> >, VertexType >::type    vertex_vector_data_accessor( vertex_vector_data );
  typename viennagrid::result_of::accessor< std::deque< std::vector<double> >, VertexType >::type    vertex_normal_data_accessor( vertex_normal_data );


  std::deque<double>            cell_double_data;
  std::deque< std::vector<double> >    cell_vector_data;
  std::deque< std::vector<double> >    cell_normal_data;

  typename viennagrid::result_of::accessor< std::deque<double>, CellType >::type            cell_double_data_accessor( cell_double_data );
  typename viennagrid::result_of::accessor< std::deque< std::vector<double> >, CellType >::type    cell_vector_data_accessor( cell_vector_data );
  typename viennagrid::result_of::accessor< std::deque< std::vector<double> >, CellType >::type    cell_normal_data_accessor( cell_normal_data );

  // write global data to vertices
  VertexContainer vertices(mesh);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {
    vertex_double_data_accessor.set(*vit, viennagrid::get_point(*vit)[0]);
    vertex_vector_data_accessor.set(*vit, std::vector<double>(3));
    vertex_normal_data_accessor.set(*vit, std::vector<double>(3));
  }

  CellContainer cells(mesh);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    cell_double_data_accessor.set(*cit, viennagrid::circumcenter(*cit)[0]);
    cell_vector_data_accessor.set(*cit, std::vector<double>(3));
    cell_normal_data_accessor.set(*cit, std::vector<double>(3));
  }






  std::map<RegionIDType, std::deque<double> >            region_vertex_double_data;
  std::map<RegionIDType, std::deque< std::vector<double> > >    region_vertex_vector_data;
  std::map<RegionIDType, std::deque< std::vector<double> > >    region_vertex_normal_data;

  std::map<RegionIDType, std::deque<double> >            region_cell_double_data;
  std::map<RegionIDType, std::deque< std::vector<double> > >    region_cell_vector_data;
  std::map<RegionIDType, std::deque< std::vector<double> > >    region_cell_normal_data;





  // write region-based data
  int index = 0;

  typedef viennagrid::result_of::region_range<MeshType>::type RegionRangeType;
  typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

  RegionRangeType regions(mesh);
  for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit, ++index)
  {
    std::cout << "Writing region " << (int)(*rit).id() << std::endl;

    typename viennagrid::result_of::accessor< std::deque<double>, VertexType >::type            region_vertex_double_data_accessor( region_vertex_double_data[(*rit).id()] );
    typename viennagrid::result_of::accessor< std::deque< std::vector<double> >, VertexType >::type    region_vertex_vector_data_accessor( region_vertex_vector_data[(*rit).id()] );
    typename viennagrid::result_of::accessor< std::deque< std::vector<double> >, VertexType >::type    region_vertex_normal_data_accessor( region_vertex_normal_data[(*rit).id()] );

    RegionVertexContainer region_vertices(*rit);
    for (RegionVertexIterator vit = region_vertices.begin();
        vit != region_vertices.end();
        ++vit)
    {
      region_vertex_double_data_accessor.set(*vit, index+1);
      region_vertex_vector_data_accessor.set(*vit, std::vector<double>(3, index));
      region_vertex_normal_data_accessor.set(*vit, std::vector<double>(3, index));
    }


    typename viennagrid::result_of::accessor< std::deque<double>, CellType >::type            region_cell_double_data_accessor( region_cell_double_data[(*rit).id()] );
    typename viennagrid::result_of::accessor< std::deque< std::vector<double> >, CellType >::type    region_cell_vector_data_accessor( region_cell_vector_data[(*rit).id()] );
    typename viennagrid::result_of::accessor< std::deque< std::vector<double> >, CellType >::type    region_cell_normal_data_accessor( region_cell_normal_data[(*rit).id()] );




    RegionCellContainer region_cells(*rit);
    for (RegionCellIterator cit = region_cells.begin();
                      cit != region_cells.end();
                    ++cit)
    {
      region_cell_double_data_accessor.set(*cit, viennagrid::circumcenter(*cit)[0] + index);
      region_cell_vector_data_accessor.set(*cit, std::vector<double>(3, index));
      region_cell_normal_data_accessor.set(*cit, std::vector<double>(3, index));
    }
  }




  //now setup and run the VTK writer:

  viennagrid::io::vtk_writer<MeshType> vtk_writer;

  viennagrid::io::add_scalar_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(vertex_normal_data), "point_normal_global");

  for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit, ++index)
  {
    viennagrid::io::add_scalar_data_on_vertices(vtk_writer, *rit, viennagrid::make_accessor<VertexType>(region_vertex_double_data[(*rit).id()]), "point_scalar1_region");
    viennagrid::io::add_vector_data_on_vertices(vtk_writer, *rit, viennagrid::make_accessor<VertexType>(region_vertex_vector_data[(*rit).id()]), "point_vector_region");
    viennagrid::io::add_vector_data_on_vertices(vtk_writer, *rit, viennagrid::make_accessor<VertexType>(region_vertex_normal_data[(*rit).id()]), "point_normal_region");
  }

  viennagrid::io::add_scalar_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(cell_normal_data), "point_normal_global");

  for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit, ++index)
  {
    viennagrid::io::add_scalar_data_on_cells(vtk_writer, *rit, viennagrid::make_accessor<CellType>(region_cell_double_data[(*rit).id()]), "point_scalar1_region");
    viennagrid::io::add_vector_data_on_cells(vtk_writer, *rit, viennagrid::make_accessor<CellType>(region_cell_vector_data[(*rit).id()]), "point_vector_region");
    viennagrid::io::add_vector_data_on_cells(vtk_writer, *rit, viennagrid::make_accessor<CellType>(region_cell_normal_data[(*rit).id()]), "point_normal_region");
  }

  vtk_writer(mesh, outfile);




  //
  // Test for vtk reader: Read everything again
  //

  std::cout << "Reading and writing the whole data again... (pass 1)" << std::endl;
  MeshType mesh2;

  viennagrid::io::vtk_reader<MeshType> vtk_reader;


  std::deque<double>            pass1_vertex_double_data;
  std::deque< std::vector<double> >    pass1_vertex_vector_data;
  std::deque< std::vector<double> >    pass1_vertex_normal_data;

  std::deque<double>            pass1_cell_double_data;
  std::deque< std::vector<double> >    pass1_cell_vector_data;
  std::deque< std::vector<double> >    pass1_cell_normal_data;

  viennagrid::io::add_scalar_data_on_vertices(vtk_reader, viennagrid::make_accessor<VertexType>(pass1_vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_reader, viennagrid::make_accessor<VertexType>(pass1_vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_reader, viennagrid::make_accessor<VertexType>(pass1_vertex_normal_data), "point_normal_global");

  viennagrid::io::add_scalar_data_on_cells(vtk_reader, viennagrid::make_accessor<CellType>(pass1_cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_reader, viennagrid::make_accessor<CellType>(pass1_cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_reader, viennagrid::make_accessor<CellType>(pass1_cell_normal_data), "point_normal_global");

  vtk_reader(mesh2, outfile + ".pvd");


  viennagrid::io::add_scalar_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(pass1_vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(pass1_vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(pass1_vertex_normal_data), "point_normal_global");

  RegionRangeType regions2(mesh2);
  for (RegionRangeIterator rit = regions2.begin(); rit != regions2.end(); ++rit)
  {
    vtk_writer.add_scalar_data_on_vertices( *rit, vtk_reader.vertex_scalar_accessor("point_scalar1_region", *rit), "point_scalar1_region" );
    vtk_writer.add_vector_data_on_vertices( *rit, vtk_reader.vertex_vector_accessor("point_vector_region", *rit), "point_vector_region" );
    vtk_writer.add_vector_data_on_vertices( *rit, vtk_reader.vertex_vector_accessor("point_normal_region", *rit), "point_normal_region" );
  }

  viennagrid::io::add_scalar_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(pass1_cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(pass1_cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(pass1_cell_normal_data), "point_normal_global");

  for (RegionRangeIterator rit = regions2.begin(); rit != regions2.end(); ++rit)
  {
    vtk_writer.add_scalar_data_on_cells( *rit, vtk_reader.cell_scalar_accessor("point_scalar1_region", *rit), "point_scalar1_region" );
    vtk_writer.add_vector_data_on_cells( *rit, vtk_reader.cell_vector_accessor("point_vector_region", *rit), "point_vector_region" );
    vtk_writer.add_vector_data_on_cells( *rit, vtk_reader.cell_vector_accessor("point_normal_region", *rit), "point_normal_region" );
  }


  std::string outfile2 = outfile + "2";
  vtk_writer(mesh2, outfile2);







  std::cout << "Reading and writing the whole data again... (pass 2)" << std::endl;
  MeshType mesh3;

  std::deque<double>            pass2_vertex_double_data;
  std::deque< std::vector<double> >    pass2_vertex_vector_data;
  std::deque< std::vector<double> >    pass2_vertex_normal_data;

  std::deque<double>            pass2_cell_double_data;
  std::deque< std::vector<double> >    pass2_cell_vector_data;
  std::deque< std::vector<double> >    pass2_cell_normal_data;

  viennagrid::io::add_scalar_data_on_vertices(vtk_reader, viennagrid::make_accessor<VertexType>(pass2_vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_reader, viennagrid::make_accessor<VertexType>(pass2_vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_reader, viennagrid::make_accessor<VertexType>(pass2_vertex_normal_data), "point_normal_global");

  viennagrid::io::add_scalar_data_on_cells(vtk_reader, viennagrid::make_accessor<CellType>(pass2_cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_reader, viennagrid::make_accessor<CellType>(pass2_cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_reader, viennagrid::make_accessor<CellType>(pass2_cell_normal_data), "point_normal_global");

  vtk_reader(mesh3, outfile + "2.pvd");


  viennagrid::io::add_scalar_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(pass2_vertex_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(pass2_vertex_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_vertices(vtk_writer, viennagrid::make_accessor<VertexType>(pass2_vertex_normal_data), "point_normal_global");

  RegionRangeType regions3(mesh3);
  for (RegionRangeIterator rit = regions3.begin(); rit != regions3.end(); ++rit)
  {
    vtk_writer.add_scalar_data_on_vertices( *rit, vtk_reader.vertex_scalar_accessor("point_scalar1_region", *rit), "point_scalar1_region" );
    vtk_writer.add_vector_data_on_vertices( *rit, vtk_reader.vertex_vector_accessor("point_vector_region", *rit), "point_vector_region" );
    vtk_writer.add_vector_data_on_vertices( *rit, vtk_reader.vertex_vector_accessor("point_normal_region", *rit), "point_normal_region" );
  }

  viennagrid::io::add_scalar_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(pass2_cell_double_data), "point_scalar1_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(pass2_cell_vector_data), "point_vector_global");
  viennagrid::io::add_vector_data_on_cells(vtk_writer, viennagrid::make_accessor<CellType>(pass2_cell_normal_data), "point_normal_global");

  for (RegionRangeIterator rit = regions3.begin(); rit != regions3.end(); ++rit)
  {
    vtk_writer.add_scalar_data_on_cells( *rit, vtk_reader.cell_scalar_accessor("point_scalar1_region", *rit), "point_scalar1_region" );
    vtk_writer.add_vector_data_on_cells( *rit, vtk_reader.cell_vector_accessor("point_vector_region", *rit), "point_vector_region" );
    vtk_writer.add_vector_data_on_cells( *rit, vtk_reader.cell_vector_accessor("point_normal_region", *rit), "point_normal_region" );
  }


  std::string outfile3 = outfile + "3";
  vtk_writer(mesh3, outfile3);
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
  test<3, viennagrid::tetrahedron_tag>(infile, outfile);

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
