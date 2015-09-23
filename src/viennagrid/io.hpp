#ifndef VIENNAGRID_IO_HPP
#define VIENNAGRID_IO_HPP

#include <map>
#include <string>

#include "viennagrid/viennagrid.h"

struct viennagrid_mesh_io_
{
public:

  viennagrid_mesh_io_() : mesh_(0), reference_counter(1) {}
  ~viennagrid_mesh_io_()
  {
    clear();
  }


  void clear()
  {
    for (std::map<std::string, viennagrid_quantity_field>::iterator it = quantity_fields_.begin();
                                                                    it != quantity_fields_.end();
                                                                  ++it)
    {
      viennagrid_quantity_field_release( (*it).second );
    }
    quantity_fields_.clear();

    set_mesh(0);
  }


  viennagrid_int quantity_field_count()
  {
    return quantity_fields_.size();
  }

  viennagrid_quantity_field get_quantity_field(viennagrid_int i)
  {
    if ((i < 0) || (i >= (viennagrid_int)quantity_fields_.size()))
      return 0;

    std::map<std::string, viennagrid_quantity_field>::iterator it = quantity_fields_.begin();
    std::advance(it, i);
    return (*it).second;
  }

  viennagrid_quantity_field get_quantity_field(std::string const & quantity_name)
  {
    std::map<std::string, viennagrid_quantity_field>::iterator it = quantity_fields_.find( quantity_name );
    if (it != quantity_fields_.end())
      return (*it).second;
    return 0;
  }

  void set_quantity_field(std::string const & quantity_name, viennagrid_quantity_field quantity_field)
  {
    std::map<std::string, viennagrid_quantity_field>::iterator it = quantity_fields_.find( quantity_name );
    if (it != quantity_fields_.end())
    {
      viennagrid_quantity_field_release( (*it).second );

      if (quantity_field)
        (*it).second = quantity_field;
      else
        quantity_fields_.erase(it);
    }
    else
    {
      if (quantity_field)
        quantity_fields_[quantity_name] = quantity_field;
    }

    if (quantity_field)
      viennagrid_quantity_field_retain( quantity_field );
  }

  void set_mesh(viennagrid_mesh mesh_in)
  {
    if (mesh_)
      viennagrid_mesh_release(mesh_);

    mesh_ = mesh_in;

    if (mesh_)
      viennagrid_mesh_retain(mesh_);
  }

  viennagrid_mesh get_mesh()
  {
    return mesh_;
  }


private:
  viennagrid_mesh mesh_;
  std::map<std::string, viennagrid_quantity_field> quantity_fields_;

public:
  viennagrid_int reference_counter;
};




viennagrid_error viennagrid_mesh_io_read_netgen(viennagrid_mesh_io mesh_io,
                                                const char * filename);

viennagrid_error viennagrid_mesh_io_write_vtk(viennagrid_mesh_io mesh_io,
                                             const char * filename);

viennagrid_error viennagrid_mesh_io_read_vtk(viennagrid_mesh_io mesh_io,
                                             std::string filename);

#endif
