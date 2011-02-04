/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_DOMAIN_CONFIGS_HPP
#define VIENNAGRID_DOMAIN_CONFIGS_HPP

namespace viennagrid
{
namespace config
{
struct u22
{
  typedef double                                numeric_type;
  typedef viennagrid::two_dimensions_tag        dimension_tag;
  typedef viennagrid::triangle_tag              cell_tag;
  typedef viennagrid::no_multigrid_tag          multigrid_tag;
};

struct u32
{
  typedef double                                numeric_type;
  typedef viennagrid::three_dimensions_tag      dimension_tag;
  typedef viennagrid::triangle_tag              cell_tag;
  typedef viennagrid::no_multigrid_tag          multigrid_tag;
};

struct u33
{
  typedef double                                numeric_type;
  typedef viennagrid::three_dimensions_tag      dimension_tag;
  typedef viennagrid::tetrahedron_tag           cell_tag;
  typedef viennagrid::no_multigrid_tag          multigrid_tag;
};
}
}


#endif
