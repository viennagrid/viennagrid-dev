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

#include "viennautils/dfise/grid_reader.hpp"

#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/algorithm/inclusion.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/quantity_field.hpp"

namespace viennagrid
{
namespace io
{

class dfise_text_reader
{
public:
  dfise_text_reader(std::string const & filename);

  template<typename MeshT>
  bool to_viennagrid(MeshT const & mesh, std::vector<viennagrid::quantity_field> & quantity_fields, bool extrude_contacts = true);

private:
  viennautils::dfise::grid_reader grid_reader_;

  static viennagrid_element_tag to_viennagrid_element_tag(viennautils::dfise::element_tag::type dfise_tag);

  template<typename PointT>
  static PointT normal_vector(PointT const & p0, PointT const & p1);

  template<typename PointT>
  static PointT normal_vector(PointT const & p0, PointT const & p1, PointT const & p2);
};

//------------------------------------------------------------------------------------------------
//              Implementation
//------------------------------------------------------------------------------------------------

inline dfise_text_reader::dfise_text_reader(std::string const & filename) : grid_reader_(filename)
{
}

viennagrid_element_tag dfise_text_reader::to_viennagrid_element_tag(viennautils::dfise::element_tag::type dfise_tag)
{
  using viennautils::dfise::element_tag;
  switch(dfise_tag)
  {
    case element_tag::line:          return VIENNAGRID_ELEMENT_TAG_LINE;
    case element_tag::triangle:      return VIENNAGRID_ELEMENT_TAG_TRIANGLE;
    case element_tag::quadrilateral: return VIENNAGRID_ELEMENT_TAG_QUADRILATERAL;
  }
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

template<typename MeshT>
bool dfise_text_reader::to_viennagrid(MeshT const & mesh, std::vector<viennagrid::quantity_field>& quantity_fields, bool extrude_contacts)
{
  using viennautils::dfise::grid_reader;
  
  typedef typename viennagrid::result_of::point<MeshT>::type PointType;
  typedef typename viennagrid::result_of::element<MeshT>::type VertexType;
  
  struct RegionContact
  {
    std::string region_name_;
    std::vector<grid_reader::ElementIndex> element_indices_;
  };
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
      for (int j = 0; j < dimension; ++j)
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
    cell_dimension = std::max(cell_dimension, viennagrid_topological_dimension(to_viennagrid_element_tag(it->tag_)));
  }

  RegionContactMap region_contacts;
  
  grid_reader::RegionMap const & regions = grid_reader_.get_regions();

  for (grid_reader::RegionMap::const_iterator region_it = regions.begin(); region_it != regions.end(); ++region_it)
  {
    std::string region_name = region_it->first;
    //dfise regions can come in a quoted form i.e. "region_name" and those extra quotes are useless or even harmful in other formats, thus we strip them
    if (region_name.front() == '"')
    {
      region_name = region_name.erase(0,1);
    }
    if (region_name.back() == '"')
    {
      region_name.pop_back();
    }

    std::vector<grid_reader::ElementIndex> const & element_indices = region_it->second.element_indices_;
    for (std::vector<grid_reader::ElementIndex>::const_iterator element_it = element_indices.begin(); element_it != element_indices.end(); ++element_it)
    {
      grid_reader::element const & e = dfise_elements[*element_it];

      //TODO manual TRIANGULATION should be kicked out asap
      if(e.tag_ == viennautils::dfise::element_tag::quadrilateral)
      {
        //TRIANGULATION
        std::vector<VertexType> cell_vertices(3);
        cell_vertices[0] = vertices[e.vertex_indices_[0]];
        cell_vertices[1] = vertices[e.vertex_indices_[1]];
        cell_vertices[2] = vertices[e.vertex_indices_[2]];
        
        viennagrid::make_element( mesh.get_make_region(region_name),
                                viennagrid::element_tag_t::from_internal(VIENNAGRID_ELEMENT_TAG_TRIANGLE),
                                cell_vertices.begin(), cell_vertices.end() );
        
        cell_vertices[0] = vertices[e.vertex_indices_[2]];
        cell_vertices[1] = vertices[e.vertex_indices_[3]];
        cell_vertices[2] = vertices[e.vertex_indices_[0]];
        
        viennagrid::make_element( mesh.get_make_region(region_name),
                                viennagrid::element_tag_t::from_internal(VIENNAGRID_ELEMENT_TAG_TRIANGLE),
                                cell_vertices.begin(), cell_vertices.end() );
      }
      else
      {
        std::vector<VertexType> cell_vertices(e.vertex_indices_.size());
        for (std::size_t i = 0; i < e.vertex_indices_.size(); ++i)
        {
          cell_vertices[i] = vertices[e.vertex_indices_[i]];
        }

        if (viennagrid_topological_dimension(to_viennagrid_element_tag(e.tag_)) == cell_dimension)
        {
          viennagrid::make_element( mesh.get_make_region(region_name),
                                    viennagrid::element_tag_t::from_internal(to_viennagrid_element_tag(e.tag_)),
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
  
  std::vector<std::pair<VertexType, grid_reader::VertexIndex> > new_vertices;

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

        viennagrid_element_tag contact_tag;
        if (e.tag_ == VIENNAGRID_ELEMENT_TAG_LINE)
        {
          PointType p0 = viennagrid::get_point( vertices[e.vertex_indices_[0]] );
          PointType p1 = viennagrid::get_point( vertices[e.vertex_indices_[1]] );

          center = (p0+p1)/2;
          normal = normal_vector(p0, p1);

          size = viennagrid::distance(center, p0);
          contact_tag = VIENNAGRID_ELEMENT_TAG_TRIANGLE;
        }
        else
        {
          //TODO
          throw std::string("not supported");
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
        new_vertices.push_back(std::pair<VertexType, grid_reader::VertexIndex>(new_vertex, e.vertex_indices_[0]));
        cell_vertices.push_back( new_vertex );

        viennagrid::make_element( mesh.get_make_region(rc_it->second.region_name_),
                                  viennagrid::element_tag_t::from_internal(contact_tag),
                                  cell_vertices.begin(), cell_vertices.end() );
        }
    }
  }
  
  /*
  for (VertexDataMap::const_iterator vd_it = vertex_data_.begin(); vd_it != vertex_data_.end(); ++vd_it)
  {
    viennagrid::quantity_field qf;
    qf.set_name( vd_it->first.substr(1, vd_it->first.size()-2) );
    qf.set_topologic_dimension( 0 );
    qf.set_values_dimension( 1 );
    qf.resize(vertices.size() + new_vertices.size());
    
    for (size_t i = 0; i < vertices.size(); ++i)
    {
      qf.set(vertices[i], vd_it->second[i]);
    }
    
    for (size_t i = 0; i < new_vertices.size(); ++i)
    {
      qf.set(new_vertices[i].first, vd_it->second[new_vertices[i].second]);
    }
    
    quantity_fields.push_back(qf);
  }
  */
  return true;
}

} //end of namespace io

} //end of namespace viennamesh

#endif
