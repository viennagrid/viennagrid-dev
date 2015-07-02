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
#include <set>
#include <map>

#include <boost/lexical_cast.hpp>

#include "viennautils/exception.hpp"
#include "viennautils/dfise/parsing_error.hpp"
#include "viennautils/dfise/grid_reader.hpp"
#include "viennautils/dfise/data_reader.hpp"

#include "viennagridpp/mesh/element_creation.hpp"
#include "viennagridpp/algorithm/inclusion.hpp"
#include "viennagridpp/algorithm/distance.hpp"
#include "viennagridpp/quantity_field.hpp"

class P;

namespace viennagrid
{
namespace io
{

class dfise_text_reader
{
public:
  struct error : virtual viennautils::exception {};

  dfise_text_reader(std::string const & filename);

  void read_dataset(std::string const & filename);

  template<typename MeshT>
  bool to_viennagrid(MeshT const & mesh, std::vector<viennagrid::quantity_field> & quantity_fields, bool extrude_contacts = true);

private:
  struct RegionContact
  {
    std::string region_name_;
    std::vector<viennautils::dfise::grid_reader::ElementIndex> element_indices_;
  };

  viennautils::dfise::grid_reader grid_reader_;
  viennautils::dfise::data_reader data_reader_;

  static viennagrid_element_type to_viennagrid_element_type(viennautils::dfise::grid_reader::element_tag::type dfise_tag);

  template<typename PointT>
  static PointT normal_vector(PointT const & p0, PointT const & p1);

  template<typename PointT>
  static PointT normal_vector(PointT const & p0, PointT const & p1, PointT const & p2);
};

//------------------------------------------------------------------------------------------------
//              Implementation
//------------------------------------------------------------------------------------------------

inline dfise_text_reader::dfise_text_reader( std::string const & filename
                                           ) try
                                           : grid_reader_(filename)
                                           , data_reader_(grid_reader_)
{
}
catch(viennautils::dfise::parsing_error const & e)
{
  throw viennautils::make_exception<dfise_text_reader::error>(e.what());
}

inline void dfise_text_reader::read_dataset(std::string const & filename)
{
  try
  {
    data_reader_.read(filename);
  }
  catch(viennautils::dfise::parsing_error const & e)
  {
    throw viennautils::make_exception<dfise_text_reader::error>(e.what());
  }
}

template<typename MeshT>
bool dfise_text_reader::to_viennagrid(MeshT const & mesh, std::vector<viennagrid::quantity_field>& quantity_fields, bool extrude_contacts)
{
  using viennautils::dfise::grid_reader;
  using viennautils::dfise::data_reader;

  typedef typename viennagrid::result_of::point<MeshT>::type PointType;
  typedef typename viennagrid::result_of::element<MeshT>::type VertexType;

  typedef std::map<std::string, RegionContact> RegionContactMap;

  std::vector<VertexType> vertices;

  {
    //create vertices from the vector returned by the grid_reader_.get_vertices() method which
    // actually stores the coordinates of the vertices in sequential order:
    // e.g. in 2d: grid_reader_.get_vertices() = (x_0, y_0, x_1, y_1, ...)
    //      in 3d: grid_reader_.get_vertices() = (x_0, y_0, z_0, x_1, ...)

    //TODO get_translate() and get_transform() is still ignored here

    unsigned int dimension = grid_reader_.get_dimension();
    vertices.resize(grid_reader_.get_vertices().size() / dimension);
    for (typename std::vector<VertexType>::size_type i = 0; i < vertices.size(); ++i)
    {
      PointType point(dimension);
      for (unsigned int j = 0; j < dimension; ++j)
      {
        point[j] = grid_reader_.get_vertices()[dimension*i+j];
      }

      vertices[i] = viennagrid::make_vertex(mesh, point);
    }
  }

  grid_reader::ElementVector const & dfise_elements = grid_reader_.get_elements();

  //establish maximum cell dimension
  viennagrid_dimension cell_dimension = -1;
  for (grid_reader::ElementVector::const_iterator it = dfise_elements.begin(); it != dfise_elements.end(); ++it)
  {
    cell_dimension = std::max(cell_dimension, viennagrid_topological_dimension(to_viennagrid_element_type(it->tag_)));
  }

  RegionContactMap region_contacts;

  grid_reader::RegionMap const & regions = grid_reader_.get_regions();

  for (grid_reader::RegionMap::const_iterator region_it = regions.begin(); region_it != regions.end(); ++region_it)
  {
    std::string region_name = region_it->first;

    std::vector<grid_reader::ElementIndex> const & element_indices = region_it->second.element_indices_;
    for ( std::vector<grid_reader::ElementIndex>::const_iterator element_it = element_indices.begin()
        ; element_it != element_indices.end()
        ; ++element_it
        )
    {
      grid_reader::element const & e = dfise_elements[*element_it];

      //TODO manual TRIANGULATION should be kicked out asap
      if(e.tag_ == viennautils::dfise::grid_reader::element_tag::quadrilateral)
      {
        //TRIANGULATION
        std::vector<VertexType> cell_vertices(3);
        cell_vertices[0] = vertices[e.vertex_indices_[0]];
        cell_vertices[1] = vertices[e.vertex_indices_[1]];
        cell_vertices[2] = vertices[e.vertex_indices_[2]];

        viennagrid::make_element( mesh.get_make_region(region_name),
                                viennagrid::element_tag::from_internal(VIENNAGRID_ELEMENT_TYPE_TRIANGLE),
                                cell_vertices.begin(), cell_vertices.end() );

        cell_vertices[0] = vertices[e.vertex_indices_[2]];
        cell_vertices[1] = vertices[e.vertex_indices_[3]];
        cell_vertices[2] = vertices[e.vertex_indices_[0]];

        viennagrid::make_element( mesh.get_make_region(region_name),
                                viennagrid::element_tag::from_internal(VIENNAGRID_ELEMENT_TYPE_TRIANGLE),
                                cell_vertices.begin(), cell_vertices.end() );
      }
      else
      {
        std::vector<VertexType> cell_vertices(e.vertex_indices_.size());
        for (std::size_t i = 0; i < e.vertex_indices_.size(); ++i)
        {
          cell_vertices[i] = vertices[e.vertex_indices_[i]];
        }

        if (viennagrid_topological_dimension(to_viennagrid_element_type(e.tag_)) == cell_dimension)
        {
          viennagrid::make_element( mesh.get_make_region(region_name),
                                    viennagrid::element_tag::from_internal(to_viennagrid_element_type(e.tag_)),
                                    cell_vertices.begin(), cell_vertices.end() );
        }
        else
        {
          region_contacts[region_name].region_name_ = region_name + "_contact";
          region_contacts[region_name].element_indices_.push_back(*element_it);
        }
      }
    }
  }

  typedef std::multimap<grid_reader::VertexIndex, VertexType> NewVertexMap;
  NewVertexMap new_vertices;

  //TODO merge this code with sentaurus tdr_reader! TODO
  if (extrude_contacts)
  {
    for (typename RegionContactMap::const_iterator rc_it = region_contacts.begin(); rc_it != region_contacts.end(); ++rc_it)
    {
      for (std::size_t i = 0; i < rc_it->second.element_indices_.size(); ++i)
      {
        grid_reader::element const & e = dfise_elements[rc_it->second.element_indices_[i]];
//             std::vector<VertexType> handles = rc->second.vertex_handles[i];

        PointType center;
        PointType normal;
        double size = 0;

        viennagrid_element_type contact_type;
        if (e.tag_ == VIENNAGRID_ELEMENT_TYPE_LINE)
        {
          PointType p0 = viennagrid::get_point( vertices[e.vertex_indices_[0]] );
          PointType p1 = viennagrid::get_point( vertices[e.vertex_indices_[1]] );

          center = (p0+p1)/2;
          normal = normal_vector(p0, p1);

          size = viennagrid::distance(center, p0);
          contact_type = VIENNAGRID_ELEMENT_TYPE_TRIANGLE;
        }
        else
        {
          throw viennautils::make_exception<dfise_text_reader::error>("currently only line contacts are supported");
        }

        normal /= viennagrid::norm_2(normal);
        size /= 2;
        normal *= size;

        PointType other_vertex = center + normal;

        typedef typename viennagrid::result_of::cell_range<MeshT>::type CellRangeType;
        typedef typename viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;

        CellRangeType cells(mesh);
        CellIteratorType cit = cells.begin();
        for (; cit != cells.end(); ++cit)
        {
          if ( viennagrid::is_inside(*cit, other_vertex) )
            break;
        }

        if (cit != cells.end())
          other_vertex = center - normal;

        std::vector<VertexType> cell_vertices;
        for (std::vector<grid_reader::VertexIndex>::size_type i = 0; i < e.vertex_indices_.size(); ++i)
        {
          cell_vertices.push_back(vertices[e.vertex_indices_[i]]);
        }
        VertexType new_vertex = viennagrid::make_vertex(mesh, other_vertex);
        new_vertices.insert(std::pair<grid_reader::VertexIndex, VertexType>(e.vertex_indices_[0], new_vertex));
        cell_vertices.push_back( new_vertex );

        viennagrid::make_element( mesh.get_make_region(rc_it->second.region_name_),
                                  viennagrid::element_tag::from_internal(contact_type),
                                  cell_vertices.begin(), cell_vertices.end() );
        }
    }
  }

  for ( data_reader::CompleteDatasetMap::const_iterator dataset_it = data_reader_.get_complete_datasets().begin()
      ; dataset_it != data_reader_.get_complete_datasets().end()
      ; ++dataset_it
      )
  {
    size_t value_dimension = dataset_it->second.first;
    viennagrid::quantity_field qf(0, value_dimension, VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE);
    std::string quantity_name = dataset_it->first;
    qf.set_name(quantity_name);
    qf.resize(vertices.size() + new_vertices.size());

    data_reader::ValueVector const & values = dataset_it->second.second;

    for (size_t i = 0; i < vertices.size(); ++i)
    {
      qf.set(vertices[i], &values[i*value_dimension]);
    }

    for (typename NewVertexMap::const_iterator it = new_vertices.begin(); it != new_vertices.end(); ++it)
    {
      qf.set(it->second, &values[it->first*value_dimension]);
    }

    quantity_fields.push_back(qf);
  }

  for ( data_reader::PartialDatasetMap::const_iterator dataset_it = data_reader_.get_partial_datasets().begin()
      ; dataset_it != data_reader_.get_partial_datasets().end()
      ; ++dataset_it
      )
  {
    size_t value_dimension = dataset_it->second.first;
    viennagrid::quantity_field qf(0, value_dimension, VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE);
    std::string quantity_name = dataset_it->first;
    qf.set_name(quantity_name);

    data_reader::VertexIndexVector const & vertex_indices = dataset_it->second.second.first;
    data_reader::ValueVector const & values = dataset_it->second.second.second;
    for (size_t i = 0; i < vertex_indices.size(); ++i)
    {
      qf.set(vertex_indices[i], &values[i*value_dimension]);

      //set the same value for all new vertices that were "derived" of the current vertex
      for ( typename NewVertexMap::const_iterator new_it = new_vertices.find(vertex_indices[i])
          ; new_it != new_vertices.end() && new_it->first == vertex_indices[i]
          ; ++new_it
          )
      {
        qf.set(new_it->second, &values[i*value_dimension]);
      }
    }
    quantity_fields.push_back(qf);
  }

  return true;
}

inline viennagrid_element_type dfise_text_reader::to_viennagrid_element_type(viennautils::dfise::grid_reader::element_tag::type dfise_tag)
{
  switch(dfise_tag)
  {
    case viennautils::dfise::grid_reader::element_tag::line:          return VIENNAGRID_ELEMENT_TYPE_LINE;
    case viennautils::dfise::grid_reader::element_tag::triangle:      return VIENNAGRID_ELEMENT_TYPE_TRIANGLE;
    case viennautils::dfise::grid_reader::element_tag::quadrilateral: return VIENNAGRID_ELEMENT_TYPE_QUADRILATERAL;
  }

  assert(false);
  return viennautils::dfise::grid_reader::element_tag::line;
}

template<typename PointT>
PointT dfise_text_reader::normal_vector(PointT const & p0, PointT const & p1)
{
  PointT line = p1-p0;
  std::swap(line[0], line[1]);
  line[0] = -line[0];
  return line;
}

template<typename PointT>
PointT dfise_text_reader::normal_vector(PointT const & p0, PointT const & p1, PointT const & p2)
{
  return viennagrid::cross_prod( p1-p0, p2-p0 );
}

} //end of namespace io

} //end of namespace viennamesh

#endif
