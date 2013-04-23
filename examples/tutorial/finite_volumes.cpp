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
  #pragma warning( disable : 4355 )     //use of this in member initializer list
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif


#include <iostream>
#include <fstream>
#include <vector>

#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/io/netgen_reader.hpp"

#include "viennagrid/domain/coboundary_iteration.hpp"

//
// A very simple sparse matrix class
//
template <typename T>
class sparse_matrix
{
  public:
    sparse_matrix(std::size_t rows = 1, std::size_t cols = 1) : rows_(rows), cols_(cols) {}
    
    T & operator()(std::size_t i, std::size_t j) { return entries_[i][j]; }
    T const & operator()(std::size_t i, std::size_t j) const { return entries_[i][j]; }
    
    void resize(std::size_t rows, std::size_t cols)
    {
      rows_ = rows;
      cols_ = cols;
      entries_.resize(rows);
    }
    
  private:
    std::size_t rows_;
    std::size_t cols_;
    std::vector<std::map<std::size_t, T> >  entries_;
};

//
// The assembly routine for Poisson equation:  div( grad(psi) ) = 1
//
template <typename CellTypeOrTag, typename DomainType, typename MatrixType, typename VectorType>
void assemble(DomainType & domain,
              MatrixType & system_matrix,
              VectorType & load_vector)
{
  typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
  typedef typename viennagrid::result_of::element<DomainType, CellTag>::type CellType;
    
  //typedef typename DomainType::config_type           Config;
  //typedef typename Config::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type                         VertexType;
  typedef typename viennagrid::result_of::element<DomainType, viennagrid::line_tag>::type                         EdgeType;
  //typedef typename viennagrid::result_of::element<Config, CellTag>::type   CellType;
  
  typedef typename viennagrid::result_of::element_range<DomainType, viennagrid::vertex_tag>::type     VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type          VertexIterator;
  typedef typename viennagrid::result_of::handle_iterator<VertexContainer>::type          VertexHookIterator;

  //typedef typename viennagrid::result_of::element_range<VertexType, viennagrid::line_tag>::type     EdgeOnVertexContainer;
  typedef typename viennagrid::result_of::coboundary_range<DomainType, viennagrid::line_tag>::type EdgeOnVertexContainer;
  typedef typename viennagrid::result_of::iterator<EdgeOnVertexContainer>::type    EdgeOnVertexIterator;
  
  std::size_t current_dof = 0;
  
  //
  // Compute Voronoi info
  //
  viennagrid::apply_voronoi<CellTypeOrTag>(domain);
  
  //
  // Iterate over all vertices in the domain and enumerate degrees of freedom (aka. unknown indices)
  //
  VertexContainer vertices = viennagrid::elements<viennagrid::vertex_tag>(domain);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {  
    //boundary condition: Assuming homogeneous Dirichlet boundary conditions at x=0 and x=1
    //if ( (vit->point()[0] == 0) || (vit->point()[0] == 1) )
    if ( (viennagrid::point(domain, *vit)[0] == 0) || (viennagrid::point(domain, *vit)[0] == 1) )
      viennadata::access<std::string, long>("dof")(*vit) = -1;
    else
    {
      viennadata::access<std::string, long>("dof")(*vit) = current_dof;
      ++current_dof;
    }
  }
  
  std::cout << "Assigned degrees of freedom: " << current_dof << std::endl;
  
  //resize global system matrix and load vector to the number of unknowns:
  system_matrix.resize(current_dof, current_dof);
  load_vector.resize(current_dof);
  
  
  //        
  // Poisson equation assembly:  div( grad(psi) ) = 1
  //
  for (VertexHookIterator vhit = vertices.handle_begin();
        vhit != vertices.handle_end();
        ++vhit)
  {
    VertexType & vertex = viennagrid::dereference_handle(domain, *vhit);
    long row_index = viennadata::access<std::string, long>("dof")(vertex);
    
    //std::cout << vertex << " " << row_index << std::endl;
    
    if (row_index < 0) //this is a Dirichlet boundary condition
      continue;
    
    //EdgeOnVertexContainer edges = viennagrid::ncells<1>(*vit, domain);
      EdgeOnVertexContainer edges = viennagrid::coboundary_elements<viennagrid::line_tag>(domain, *vhit);
    for (EdgeOnVertexIterator eovit = edges.begin();
          eovit != edges.end();
          ++eovit)
    {
      VertexType const * other_vertex_ptr = &(viennagrid::elements<viennagrid::vertex_tag>(*eovit)[0]);
      if (other_vertex_ptr == &(vertex)) //one of the two vertices of the edge is different from *vit
        other_vertex_ptr = &(viennagrid::elements<viennagrid::vertex_tag>(*eovit)[1]);
      
      long col_index        = viennadata::access<std::string, long>("dof")(*other_vertex_ptr);
      double edge_len       = viennagrid::volume(*eovit, domain);
      double interface_area = viennadata::access<viennagrid::voronoi_interface_area_key, double>()(*eovit);
      
      //std::cout << "  " << *other_vertex_ptr << std::endl;
      //std::cout << "  " << col_index << " " << edge_len << " " << interface_area << std::endl;
      
      if (col_index >= 0)
        system_matrix(row_index, col_index) = - interface_area / edge_len;
      
      system_matrix(row_index, row_index) += interface_area / edge_len;
      
      //std::cout << "  " << system_matrix(row_index, col_index) << " " << system_matrix(row_index, row_index) << std::endl;
      //std::cout << std::endl;
      
      //Note: volume stored on edges consists of volumes of both adjacent boxes.
      load_vector[row_index] += viennadata::access<viennagrid::voronoi_box_volume_key, double>()(*eovit) / 2.0;
    } //for edges
  } //for vertices
  
} //assemble()


int main()
{
  typedef viennagrid::point_t<double, viennagrid::cartesian_cs<3> > PointType;  //use this for a 3d example
  //typedef viennagrid::point_t<double, viennagrid::cartesian_cs<2> > PointType;  //use this for a 2d example
  typedef viennagrid::result_of::geometric_domain_config< viennagrid::tetrahedron_tag, PointType, viennagrid::storage::id_handle_tag >::type DomainConfig;
  typedef viennagrid::result_of::geometric_domain< DomainConfig >::type Domain;
  typedef viennagrid::result_of::geometric_view<Domain>::type Segment;
  
  typedef viennagrid::tetrahedron_tag CellTag;
    
    
  //typedef viennagrid::config::tetrahedral_3d                      ConfigType;  //use this for a 3d example
  //typedef viennagrid::config::triangular_2d                       ConfigType;  //use this for a 2d example
  //typedef viennagrid::result_of::domain<ConfigType>::type         Domain;
  
  std::cout << "----------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaGrid tutorial: Finite Volumes using ViennaGrid --" << std::endl;
  std::cout << "----------------------------------------------------------" << std::endl;
  std::cout << std::endl;
  
  Domain domain;
  std::vector<Segment> segments;
  
  viennagrid::io::netgen_reader<CellTag> reader;
  #ifdef _MSC_VER      //Visual Studio builds in a subfolder
  std::string path = "../../examples/data/";
  #else
  std::string path = "../examples/data/";
  #endif
  reader(domain, segments, path + "cube48.mesh"); //use this for a 3d example
  //reader(domain, path + "square32.mesh"); //use this for a 2d example
  
  sparse_matrix<double> system_matrix;
  std::vector<double> load_vector;
  
  assemble<CellTag>(domain, system_matrix, load_vector);
  
  //
  // Next step: Solve here using a linear algebra library, e.g. ViennaCL. (not included in this tutorial to avoid external dependencies)
  // Instead, we print the matrix
  std::cout << std::endl << "System matrix: " << std::endl;
  for (std::size_t i=0; i<load_vector.size(); ++i)
  {
    std::cout << "Row " << i << ": ";
    for (std::size_t j=0; j<load_vector.size(); ++j)
    {
      if (system_matrix(i,j) != 0)
        std::cout << "(" << j << ", " << system_matrix(i,j) << "), ";
    }
    std::cout << std::endl;
  }
  
  std::cout << std::endl << "Load vector: " << std::endl;
  for (std::size_t i=0; i<load_vector.size(); ++i)
    std::cout << load_vector[i] << " " << std::endl;
  std::cout << std::endl;
  
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}


