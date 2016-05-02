#ifndef VIENNAGRID_IO_DFISE_TEXT_READER_HPP
#define VIENNAGRID_IO_DFISE_TEXT_READER_HPP

/* =======================================================================
   Copyright (c) 2011-2015, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <string>
#include <vector>
#include <utility>

#include "viennautils/exception.hpp"
#include "viennautils/dfise/parsing_error.hpp"
#include "viennautils/dfise/grid_reader.hpp"

#include "viennagrid/viennagrid.hpp"

namespace viennagrid
{
namespace io
{

class dfise_bnd_reader
{
public:
  struct error : virtual viennautils::exception {};

  dfise_bnd_reader(std::string const & filename);

  template<typename MeshT>
  bool to_viennagrid(MeshT const & mesh);

private:
  struct RegionContact
  {
    std::string region_name_;
    std::vector<viennautils::dfise::grid_reader::ElementIndex> element_indices_;
  };

  viennautils::dfise::grid_reader grid_reader_;

  static viennagrid_element_type to_viennagrid_element_type(viennautils::dfise::grid_reader::element_tag dfise_tag);
};

//------------------------------------------------------------------------------------------------
//              Implementation
//------------------------------------------------------------------------------------------------

inline dfise_bnd_reader::dfise_bnd_reader( std::string const & filename
                                         ) try
                                         : grd_bnd_reader_(filename)
{
}
catch(viennautils::dfise::parsing_error const & e)
{
  throw viennautils::make_exception<dfise_bnd_reader::error>(e.what());
}

template<typename PlcT>
bool dfise_bnd_reader::to_viennagrid(PlcT & plc)
{
  //TODO convert grid_bnd_reader_ data into ViennaGrid PLC
  //
  //to implement this, compare with dfise_grd_dat_reader and how it creates the mesh using the data in the grd_bnd_reader
  return true;
}

inline viennagrid_element_type dfise_bnd_reader::to_viennagrid_element_type(viennautils::dfise::grid_reader::element_tag dfise_tag)
{
  switch(dfise_tag)
  {
    case viennautils::dfise::grid_reader::element_tag_line:          return VIENNAGRID_ELEMENT_TYPE_LINE;
    case viennautils::dfise::grid_reader::element_tag_triangle:      return VIENNAGRID_ELEMENT_TYPE_TRIANGLE;
    case viennautils::dfise::grid_reader::element_tag_quadrilateral: return VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL;
    case viennautils::dfise::grid_reader::element_tag_polygon:       return VIENNAGRID_ELEMENT_TYPE_POLYGON;
    case viennautils::dfise::grid_reader::element_tag_tetrahedron:   return VIENNAGRID_ELEMENT_TYPE_TETRAHEDRON;
  }

  assert(false);
  return VIENNAGRID_ELEMENT_TYPE_NO_ELEMENT;
}

} //end of namespace io

} //end of namespace viennamesh

#endif
