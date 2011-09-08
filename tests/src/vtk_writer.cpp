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

#ifdef _MSC_VER      //Visual Studio complains about potentially dangerous things, which are perfectly legal in our context
  #pragma warning( disable : 4355 )
#endif

#include <iostream>
#include <ostream>

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/netgen_reader.hpp"

//dummy class for streaming three vector components
template <unsigned int dim>
class my_vector 
{
  public:
    my_vector() { val[0] = 0; val[1] = 1; val[2] = 0; }
    my_vector(double v) { val[0] = v; val[1] = 0; val[2] = 0; }
    
    double & operator[](std::size_t i) 
    {
      return val[i];
    }

    double const & operator[](std::size_t i) const
    {
      return val[i];      
    }

  private:
    double val[3];
};


//dummy class for streaming normals
template <unsigned int dim>
class my_normal 
{
  public:
    my_normal() { val[0] = 0; val[1] = 1; val[2] = 0; }
    my_normal(double v) { val[0] = v; val[1] = 0; val[2] = 0; }
    
    double & operator[](std::size_t i) 
    {
      double norm = sqrt(val[0] * val[0] + val[1] * val[1] + val[2] * val[2]);
      if (norm > 0)
      {
        val[0] /= norm;
        val[1] /= norm;
        val[2] /= norm;
      }
      return val[i];
    }

    double operator[](std::size_t i) const
    {
      double norm = sqrt(val[0] * val[0] + val[1] * val[1] + val[2] * val[2]);
      if (norm > 0)
        return val[i] / norm;
      return val[i];
    }
    
  private:
    double val[3];
};

template <typename VTKType>
void setup_vtk_object(VTKType & my_vtk)
{
  //point data
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_vtk, "vtk_data", "point_scalar1_global");
  viennagrid::io::add_vector_data_on_vertices<std::string, my_vector<3> >(my_vtk, "vtk_data", "point_vector_global");
  viennagrid::io::add_normal_data_on_vertices<std::string, my_normal<3> >(my_vtk, "vtk_data", "point_normal_global");

  viennagrid::io::add_scalar_data_on_vertices_per_segment<std::string, 
                                                          std::map<std::size_t, double>
                                                         >(my_vtk, "vtk_data", "point_scalar1_segment");
  viennagrid::io::add_vector_data_on_vertices_per_segment<std::string, 
                                                          std::map<std::size_t, my_vector<3> >
                                                         >(my_vtk, "vtk_data", "point_vector_segment");
  viennagrid::io::add_normal_data_on_vertices_per_segment<std::string, 
                                                          std::map<std::size_t, my_normal<3> > 
                                                         >(my_vtk, "vtk_data", "point_normal_segment");
  
  //cell data
  viennagrid::io::add_scalar_data_on_cells<std::string, double>(my_vtk, "vtk_data", "cell_scalar1_global");
  viennagrid::io::add_vector_data_on_cells<std::string, my_vector<3> >(my_vtk, "vtk_data", "cell_vector_global");
  viennagrid::io::add_normal_data_on_cells<std::string, my_normal<3> >(my_vtk, "vtk_data", "cell_normal_global");

  viennagrid::io::add_scalar_data_on_cells_per_segment<std::string, 
                                                       std::map<std::size_t, double> 
                                                      >(my_vtk, "vtk_data", "cell_scalar1_segment");
  viennagrid::io::add_vector_data_on_cells_per_segment<std::string, 
                                                       std::map<std::size_t, my_vector<3> > 
                                                      >(my_vtk, "vtk_data", "cell_vector_segment");
  viennagrid::io::add_normal_data_on_cells_per_segment<std::string, 
                                                       std::map<std::size_t, my_normal<3> > 
                                                      >(my_vtk, "vtk_data", "cell_normal_segment");
  
}


template <typename ConfigType>
void test(std::string & infile, std::string & outfile)
{

  typedef typename viennagrid::result_of::domain<ConfigType>::type           Domain;
  typedef typename ConfigType::cell_tag            CellTag;
  typedef viennagrid::segment_t<ConfigType>        Segment;
  
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                       CellTag::dim>::type   CellType;

  typedef typename viennagrid::result_of::ncell_range<Domain, 0>::type           VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::dim>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;

  typedef typename viennagrid::result_of::ncell_range<Segment, 0>::type                 SegmentVertexContainer;
  typedef typename viennagrid::result_of::iterator<SegmentVertexContainer>::type        SegmentVertexIterator;

  typedef typename viennagrid::result_of::ncell_range<Segment, CellTag::dim>::type     SegmentCellContainer;
  typedef typename viennagrid::result_of::iterator<SegmentCellContainer>::type                    SegmentCellIterator;

  
  Domain domain;
  //domain.create_segments(2);
  
   //Segment & seg = domain.add();
  
  try{
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(domain, infile);
  } catch (std::exception const & ex){
    std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
//   exit(EXIT_FAILURE);
  }

  
  // write global data to vertices
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {
    viennadata::access<std::string, double>("vtk_data")(*vit) = vit->point()[0];
    viennadata::access<std::string, my_vector<3> >("vtk_data")(*vit) = my_vector<3>();
    viennadata::access<std::string, my_normal<3> >("vtk_data")(*vit) = my_normal<3>();
  }
  
  CellContainer cells = viennagrid::ncells(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    viennadata::access<std::string, double>("vtk_data")(*cit) = viennagrid::circumcenter(*cit)[0];
    viennadata::access<std::string, my_vector<3> >("vtk_data")(*cit) = my_vector<3>();
    viennadata::access<std::string, my_normal<3> >("vtk_data")(*cit) = my_normal<3>();
  }

  // write segment-based data
  typename Domain::segment_container & segments = domain.segments();
  for (std::size_t i=0; i<segments.size(); ++i)
  {
    std::cout << "Writing segment " << i << std::endl;
    SegmentVertexContainer vertices = viennagrid::ncells<0>(segments[i]);
    for (SegmentVertexIterator vit = vertices.begin();
        vit != vertices.end();
        ++vit)
    {
      viennadata::access<std::string, 
                         std::map<std::size_t, double> >("vtk_data")(*vit)[i] = i+1;
      viennadata::access<std::string,
                         std::map<std::size_t, my_vector<3> > >("vtk_data")(*vit)[i] = my_vector<3>(i);
      viennadata::access<std::string,
                         std::map<std::size_t, my_normal<3> > >("vtk_data")(*vit)[i] = my_normal<3>(i);
    }
    
    SegmentCellContainer cells = viennagrid::ncells(segments[i]);
    for (SegmentCellIterator cit = cells.begin();
                      cit != cells.end();
                    ++cit)
    {
      viennadata::access<std::string,
                         std::map<std::size_t, double> >("vtk_data")(*cit)[i] = viennagrid::circumcenter(*cit)[0] + i;
      viennadata::access<std::string,
                         std::map<std::size_t, my_vector<3> > >("vtk_data")(*cit)[i] = my_vector<3>(i);
      viennadata::access<std::string,
                         std::map<std::size_t, my_normal<3> > >("vtk_data")(*cit)[i] = my_normal<3>(i);
    }
  }
                                                 

  //now setup and run the VTK writer:
  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
  setup_vtk_object(my_vtk_writer);
  my_vtk_writer(domain, outfile);
  
  
  //
  // Test for vtk reader: Read everything again
  //
  
  std::cout << "Reading and writing the whole data again... (pass 1)" << std::endl;
  Domain domain2;

  viennagrid::io::vtk_reader<Domain> my_vtk_reader;
  setup_vtk_object(my_vtk_reader);
  my_vtk_reader(domain2, outfile + "_main.pvd");
  
  std::string outfile2 = outfile + "2";
  my_vtk_writer(domain2, outfile2);
  
  std::cout << "Reading and writing the whole data again... (pass 2)" << std::endl;
  Domain domain3;

  my_vtk_reader(domain3, outfile + "_main.pvd");
  
  std::string outfile3 = outfile + "3";
  my_vtk_writer(domain3, outfile3);
}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  std::string infile = path + "twocubes.mesh";
  std::string outfile = "vtk_writer"; // without ending
  
  std::cout << "Running VTK writer on tetrahedron domain... " << std::endl;
  test<viennagrid::config::tetrahedral_3d>(infile, outfile);
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
