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

#include "viennagrid/viennagrid.hpp"


inline std::string random_name()
{
  std::string name;
  name.resize( rand() % 10 + 2 );

  for (std::string::size_type i = 0; i < name.size(); ++i)
    name[i] = 'a' + rand() % 26;

  return name;
}


void test(int region_count, int test_count)
{
  typedef viennagrid::mesh MeshType;
  for (int i = 0; i < test_count; ++i)
  {
    MeshType mesh;

    std::map<std::string, int> segment_names;
    while (static_cast<int>(segment_names.size()) != region_count)
      segment_names[random_name()] = rand() % 90 + 10;

    for (std::map<std::string, int>::const_iterator sit = segment_names.begin(); sit != segment_names.end(); ++sit)
    {
      for (int j = 0; j < sit->second; ++j)
        viennagrid::make_vertex( mesh.get_or_create_region(sit->first) );
    }


    for (std::map<std::string, int>::const_iterator sit = segment_names.begin(); sit != segment_names.end(); ++sit)
    {
      if (static_cast<int>(viennagrid::vertices( mesh.get_region(sit->first) ).size()) != sit->second)
      {
        std::cerr << "Number of vertices in a named region is NOT equal to created vertices" << std::endl;
        std::cerr << "Vertex count created: " << sit->second << std::endl;
        std::cerr << "Vertex count in region: " << viennagrid::vertices( mesh.get_region(sit->first) ).size() << std::endl;
        std::cerr << "FAILED!" << std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }
}

int main()
{
  srand (static_cast<unsigned int>(time(NULL)));

  int region_count = 10;
  int test_count = 10;

  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "--- Testing---" << std::endl;
  test(region_count, test_count);

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
