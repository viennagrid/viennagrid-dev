#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/bnd_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennagrid/io/mphtxt_writer.hpp"

int main()
{

  viennagrid::triangular_3d_mesh mesh;
  viennagrid::triangular_3d_segmentation segmentation(mesh);


  viennagrid::io::bnd_reader reader;

  reader( mesh, segmentation, "../data/test.bnd" );

  {
    viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> writer;
    writer( mesh, segmentation, "bnd_test.vtk" );
  }

  {
    viennagrid::io::mphtxt_writer writer;
    writer( mesh, segmentation, "bnd_test.mphtxt" );
  }

}
