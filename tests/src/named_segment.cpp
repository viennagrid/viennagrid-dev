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

//***********************************************
// Define the input-file format
//***********************************************

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>

#include "viennagrid/forwards.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"


inline std::string random_name()
{
  std::string name;
  name.resize( rand() % 10 + 2 );

  for (std::string::size_type i = 0; i < name.size(); ++i)
    name[i] = 'a' + rand() % 26;

  return name;
}


template<typename MeshT, typename SegmentationT>
void test(int segment_count, int test_count)
{
  for (int i = 0; i < test_count; ++i)
  {
    MeshT mesh;
    SegmentationT segmentation(mesh);

    std::map<std::string, int> segment_names;
    while (static_cast<int>(segment_names.size()) != segment_count)
      segment_names[random_name()] = rand() % 90 + 10;

    for (std::map<std::string, int>::const_iterator sit = segment_names.begin(); sit != segment_names.end(); ++sit)
    {
      for (int j = 0; j < sit->second; ++j)
        viennagrid::make_vertex( segmentation(sit->first) );
    }


    for (std::map<std::string, int>::const_iterator sit = segment_names.begin(); sit != segment_names.end(); ++sit)
    {
      if (static_cast<int>(viennagrid::vertices( segmentation(sit->first) ).size()) != sit->second)
      {
        std::cerr << "Numbers of vertices in a named segment is equal to created vertices" << std::endl;
        std::cerr << "FAILED!" << std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }
}

int main()
{
  srand (static_cast<unsigned int>(time(NULL)));

  int segment_count = 10;
  int test_count = 10;

  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "--- Testing with triangular_2d ---" << std::endl;
  test<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(segment_count, test_count);
  std::cout << "--- Testing with tetrahedral_3d ---" << std::endl;
  test<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(segment_count, test_count);

  std::cout << "--- Testing with quadrilateral_2d ---" << std::endl;
  test<viennagrid::quadrilateral_2d_mesh, viennagrid::quadrilateral_2d_segmentation>(segment_count, test_count);
  std::cout << "--- Testing with hexahedral_3d ---" << std::endl;
  test<viennagrid::hexahedral_3d_mesh, viennagrid::hexahedral_3d_segmentation>(segment_count, test_count);

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
