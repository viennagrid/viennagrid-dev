/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
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

#include "refinement-common.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

template <typename MeshT>
void test(std::string & infile, std::string & outfile)
{
  typedef typename viennagrid::result_of::segmentation<MeshT>::type           SegmentationType;

  typedef typename viennagrid::result_of::point<MeshT>::type          PointType;
  typedef typename viennagrid::result_of::cell<MeshT>::type   CellType;

  typedef typename viennagrid::result_of::cell_range<MeshT>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;

  MeshT mesh;
  SegmentationType segmentation(mesh);

  try{
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(mesh, segmentation, infile);
  } catch (...){
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }


  double input_mesh_volume = viennagrid::volume(mesh);
  double input_mesh_surface = mesh_surface(mesh);

  std::cout << "Volume of input mesh: " << input_mesh_volume << std::endl;
  std::cout << "Surface of input mesh: " << input_mesh_surface << std::endl;

  std::vector<bool> cell_refinement_tag_container1;
  typename viennagrid::result_of::field<std::vector<bool>, CellType>::type cell_refinement_tag_field1(cell_refinement_tag_container1);

  //Testing: Tag some cells with centroid at x \in [2,3]:
  CellContainer cells(mesh);
  for (CellIterator cit  = cells.begin();
                    cit != cells.end();
                  ++cit)
  {
    PointType centroid = viennagrid::centroid(*cit);

    if ( (centroid[0] >= 2.0)
        && (centroid[0] <= 3.0)
        && (centroid[1] >= 0.0)
        && (centroid[1] <= 1.0))
    {
      cell_refinement_tag_field1(*cit) = true;
//       viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*cit) = true;
    }
  }

  MeshT refined_mesh;
  SegmentationType refined_segmentation(refined_mesh);
  viennagrid::cell_refine( mesh, segmentation, refined_mesh, refined_segmentation, cell_refinement_tag_field1 );


//   Mesh refined_mesh;
//   refined_mesh = viennagrid::refine(mesh, viennagrid::local_refinement_tag());

  std::cout << "Volume of refined mesh: " << viennagrid::volume(refined_mesh) << std::endl;
  std::cout << "Surface of refined mesh: " << mesh_surface(refined_mesh) << std::endl;

  if ( fabs(viennagrid::volume(refined_mesh) - input_mesh_volume) / input_mesh_volume > 1e-3 )
  {
    std::cerr << "Error in check: Mesh volumes mismatch!" << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( fabs(mesh_surface(refined_mesh) - input_mesh_surface) / input_mesh_surface > 1e-3 )
  {
    std::cerr << "Error in check: Mesh surfaces mismatch!" << std::endl;
    exit(EXIT_FAILURE);
  }

  //
  // Second pass:
  //
  std::vector<bool> cell_refinement_tag_container2;
  typename viennagrid::result_of::field<std::vector<bool>, CellType>::type cell_refinement_tag_field2(cell_refinement_tag_container2);

  CellContainer cells_refined(refined_mesh);
  for (CellIterator cit  = cells_refined.begin();
                    cit != cells_refined.end();
                  ++cit)
  {
    PointType centroid = viennagrid::centroid(*cit);

    if ( (centroid[0] >= 2.0)
        && (centroid[0] <= 3.0)
        && (centroid[1] >= 0.0)
        && (centroid[1] <= 1.0))
    {
      cell_refinement_tag_field2(*cit) = true;
//       viennadata::access<viennagrid::refinement_key, bool>(viennagrid::refinement_key())(*cit) = true;
    }
  }

  MeshT double_refined_mesh;
  SegmentationType double_refined_segmentation(double_refined_mesh);
  viennagrid::cell_refine( refined_mesh, refined_segmentation, double_refined_mesh, double_refined_segmentation, cell_refinement_tag_field2 );

  std::cout << "Volume of double refined mesh: " << viennagrid::volume(double_refined_mesh) << std::endl;
  std::cout << "Surface of double refined mesh: " << mesh_surface(double_refined_mesh) << std::endl;

  if ( fabs(viennagrid::volume(double_refined_mesh) - input_mesh_volume) / input_mesh_volume > 1e-3 )
  {
    std::cerr << "Error in check: Mesh volumes mismatch!" << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( fabs(mesh_surface(double_refined_mesh) - input_mesh_surface) / input_mesh_surface > 1e-3 )
  {
    std::cerr << "Error in check: Mesh surfaces mismatch!" << std::endl;
    exit(EXIT_FAILURE);
  }

  //test writers:
  std::cout << "Writing meshs..." << std::endl;
  viennagrid::io::vtk_writer<MeshT> my_vtk_writer;
  my_vtk_writer(mesh, segmentation, outfile + ".vtu");
  my_vtk_writer(refined_mesh, refined_segmentation, outfile + "_refined.vtu");
  my_vtk_writer(double_refined_mesh, double_refined_segmentation, outfile + "_double_refined.vtu");

  return;

}


int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::string path = "../../examples/data/";

  std::string infile = path + "sshape2d.mesh";
  std::string outfile = "out_sshape2d"; // without ending

  test<viennagrid::triangular_2d_mesh>(infile, outfile);

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
