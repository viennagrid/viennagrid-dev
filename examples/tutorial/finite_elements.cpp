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

#include "viennagrid/core.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/algorithm/refine.hpp"


// #include "viennacl/vector.hpp"
// #include "viennacl/matrix.hpp"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;

#define VIENNACL_WITH_UBLAS
#include "viennacl/linalg/cg.hpp"



template<typename PointT>
void inverse_affine(PointT const & m0, PointT const & m1, PointT const & t,
                    PointT & im0, PointT & im1, PointT & it)
{
  typedef typename viennagrid::result_of::coord<PointT>::type NumericType;

  NumericType a = m0[0];
  NumericType b = m1[0];
  NumericType c = m0[1];
  NumericType d = m1[1];

  im0.resize(2);
  im1.resize(2);

  NumericType det = a*d-b*c;
  im0[0] = d / det;
  im1[0] = -b / det;
  im0[1] = -c / det;
  im1[1] = a / det;

  it = -(im0*t[0]+im1*t[1]);
}


template<typename PointT>
PointT fe_grad(PointT const & p0, PointT const & p1, PointT const & p2)
{
  PointT A0;
  PointT A1;
  PointT it;

  inverse_affine( p1-p0, p2-p0, p0, A0, A1, it );

  return viennagrid::make_point( -A0[0]-A0[1], -A1[0]-A1[1] );
}


template<typename PointT>
PointT fe_grad(PointT const & p0, PointT const & p1, PointT const & p2, int index)
{
  if (index == 0)
    return fe_grad(p0, p1, p2);
  if (index == 1)
    return fe_grad(p1, p0, p2);
  if (index == 2)
    return fe_grad(p2, p0, p1);

  return 0;
}


template<typename PointT>
typename viennagrid::result_of::coord<PointT>::type triangle_contribution(PointT const & p0,
                                                                          PointT const & p1,
                                                                          PointT const & p2,
                                                                          int i0, int i1)
{
  typedef typename viennagrid::result_of::coord<PointT>::type NumericType;

  PointT pts[3];
  pts[0] = p0;
  pts[1] = p1;
  pts[2] = p2;

  PointT fe_grad0 = fe_grad(p0, p1, p2, i0);
  PointT fe_grad1 = fe_grad(p0, p1, p2, i1);

  NumericType triangle_volume = viennagrid::spanned_volume(p0, p1, p2);

  return viennagrid::inner_prod(fe_grad0, fe_grad1) * triangle_volume;
}


template<typename PointT>
typename viennagrid::result_of::coord<PointT>::type triangle_rhs_contribution(PointT const & p0,
                                                                              PointT const & p1,
                                                                              PointT const & p2)
{
  PointT centroid = (p0+p1+p2)/3.0;

  PointT A0;
  PointT A1;
  PointT it;

  inverse_affine( p1-p0, p2-p0, p0, A0, A1, it );

  return (1 - it[0] - it[1] - (A0[0]+A0[1])*centroid[0] - (A1[0]+A1[1])*centroid[1]) *
         viennagrid::spanned_volume(p0, p1, p2);
}




struct point_ordering
{
  template <typename PointType>
  bool operator()(PointType const & p1, PointType const & p2) const
  {
    if (p1[1] > p2[1])
      return true;
    if (p1[1] < p2[1])
      return false;

    return p1[0] < p2[0];
  }
};




int main()
{
  //
  // Define the necessary types:
  //

  typedef viennagrid::mesh_hierarchy_t MeshHierarchyType;
  typedef viennagrid::result_of::mesh<MeshHierarchyType>::type MeshType;
  typedef viennagrid::result_of::region<MeshType>::type RegionType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::element<MeshType>::type           ElementType;

  typedef viennagrid::result_of::element_range<MeshType>::type      ElementRange;
  typedef viennagrid::result_of::iterator<ElementRange>::type          ElementIterator;

  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Setup of a mesh with two segments -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the mesh and the segmentation and create 2 segments:
  //
  MeshHierarchyType hierarchy;
  MeshType mesh = hierarchy.root();


  ElementType v0 = viennagrid::make_vertex(mesh, viennagrid::make_point(0, 0) );
  ElementType v1 = viennagrid::make_vertex(mesh, viennagrid::make_point(1, 0) );
  ElementType v2 = viennagrid::make_vertex(mesh, viennagrid::make_point(0, 1) );
  ElementType v3 = viennagrid::make_vertex(mesh, viennagrid::make_point(1, 1) );



  ElementType tri0 = viennagrid::make_triangle(mesh, v0, v1, v2);
  ElementType tri1 = viennagrid::make_triangle(mesh, v2, v1, v3);



  MeshType refined1 = mesh.make_child();
  viennagrid::cell_refine_uniformly(mesh, refined1);

  MeshType refined2 = refined1.make_child();
  viennagrid::cell_refine_uniformly(refined1, refined2);

  MeshType refined3 = refined2.make_child();
  viennagrid::cell_refine_uniformly(refined2, refined3);

  MeshType refined4 = refined3.make_child();
  viennagrid::cell_refine_uniformly(refined3, refined4);

  MeshType refined5 = refined4.make_child();
  viennagrid::cell_refine_uniformly(refined4, refined5);

  MeshType refined6 = refined5.make_child();
  viennagrid::cell_refine_uniformly(refined5, refined6);

  MeshType fem_mesh = refined6;



  int vertex_count = viennagrid::vertices(fem_mesh).size();

  std::map<PointType, ElementType, point_ordering> vertex_reorder;
  std::map<int, ElementType> index_to_vertex_map;

  int num_unknowns = 0;
  std::vector<int> index_container(vertex_count, -1);
  viennagrid::result_of::accessor< std::vector<int>, ElementType >::type index(index_container);

  ElementRange vertices(fem_mesh, 0);
  for (ElementIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
  {
    PointType p = viennagrid::get_point(*vit);

    if (p[0] < 0.51 && p[1] < 0.51)
      continue;

    if (viennagrid::is_boundary(fem_mesh, *vit))
      continue;

    vertex_reorder[p] = *vit;
  }

  for (std::map<PointType, ElementType, viennagrid::point_less>::iterator it = vertex_reorder.begin(); it != vertex_reorder.end(); ++it)
  {
    index_to_vertex_map[num_unknowns] = it->second;
    index(it->second) = num_unknowns++;
  }


  std::cout << "Number of unknows: " << num_unknowns << std::endl;


  ublas::matrix<double> A = ublas::zero_matrix<double>(num_unknowns, num_unknowns);
  ublas::vector<double> rhs = ublas::zero_vector<double>(num_unknowns);


  ElementRange cells(fem_mesh, 2);
  for (ElementIterator cit0 = cells.begin();
                       cit0 != cells.end();
                     ++cit0)
  {
    typedef viennagrid::result_of::vertex_range<ElementType>::type VertexOnElementRangeType;
    typedef viennagrid::result_of::iterator<VertexOnElementRangeType>::type VertexOnElementRangeIterator;


    PointType tp[3];
    tp[0] = viennagrid::get_point( viennagrid::vertices(*cit0)[0] );
    tp[1] = viennagrid::get_point( viennagrid::vertices(*cit0)[1] );
    tp[2] = viennagrid::get_point( viennagrid::vertices(*cit0)[2] );

    for (int i0 = 0; i0 < 3; ++i0)
      for (int i1 = 0; i1 < 3; ++i1)
      {
        int matrix_i0 = index( viennagrid::vertices(*cit0)[i0] );
        int matrix_i1 = index( viennagrid::vertices(*cit0)[i1] );

        if (matrix_i0 < 0 || matrix_i1 < 0)
          continue;

        double contrib = triangle_contribution(tp[0], tp[1], tp[2], i0, i1);
        A(matrix_i0, matrix_i1) += contrib;
      }


    PointType d0;
    PointType d1;
    PointType up = viennagrid::make_point(0,0,1);

    int i;

    i = index(viennagrid::vertices(*cit0)[0]);
    if (i >= 0)
      rhs(i) += triangle_rhs_contribution(tp[0], tp[1], tp[2]);

    i = index(viennagrid::vertices(*cit0)[1]);
    if (i >= 0)
      rhs(i) = triangle_rhs_contribution(tp[1], tp[0], tp[2]);

    i = index(viennagrid::vertices(*cit0)[2]);
    if (i >= 0)
      rhs(i) += triangle_rhs_contribution(tp[2], tp[0], tp[1]);
  }




//   std::cout << A << std::endl;
//   std::cout << rhs << std::endl;


  ublas::vector<double> result = viennacl::linalg::solve(A, rhs, viennacl::linalg::cg_tag());

  std::vector<double> result_data(vertices.size(), 0);
  viennagrid::result_of::accessor< std::vector<double>, ElementType >::type results(result_data);

  for (std::map<int, ElementType>::iterator it = index_to_vertex_map.begin(); it != index_to_vertex_map.end(); ++it)
    results(it->second) = result(it->first);





  viennagrid::io::vtk_writer<MeshType> writer;
  writer.add_scalar_data_on_vertices( results, "results" );
  writer(fem_mesh, "finite_elements");


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
