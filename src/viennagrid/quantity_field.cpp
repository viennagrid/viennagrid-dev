#include <algorithm>
#include <cstring>
#include <cassert>
#include <string>
#include <vector>
#include <map>

#include "viennagrid/viennagrid.h"
#include "common.hpp"



// quantities
struct viennagrid_quantity_field_t
{
  viennagrid_quantity_field_t() : topological_dimension(-1), size_of_value(-1), storage_layout(-1), reference_counter(1) {}

  std::string name;
  std::string unit;
  viennagrid_dimension topological_dimension;


  viennagrid_int size_of_value;
  viennagrid_int storage_layout;


  std::vector<char> dense_values;
  std::vector<bool> dense_valid_flags;

  std::map< viennagrid_int, char * > sparse_values;


  viennagrid_int size() const
  {
    switch ( storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
        return dense_values.size() / size_of_value;
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
        return sparse_values.size();
    }

    return -1;
  }

  void resize(viennagrid_int value_count)
  {
    switch ( storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
      {
        dense_values.resize( size_of_value * value_count );
        dense_valid_flags.resize( value_count, false );
        break;
      }
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
        break;
      default:
        assert(false);
    }
  }

  void set(viennagrid_int element_id, void * value)
  {
    void * dst = NULL;

    switch ( storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
      {
        if ( element_id >= static_cast<viennagrid_int>(dense_valid_flags.size()) )
          resize(element_id+1);

        dst = &dense_values[0] + element_id*size_of_value;
        dense_valid_flags[element_id] = true;

        break;
      }

      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
      {
        std::map< viennagrid_int, char* >::iterator it = sparse_values.find(element_id);
        if (it == sparse_values.end())
          it = sparse_values.insert( std::make_pair(element_id, new char[size_of_value]) ).first;

        dst = (*it).second;

        break;
      }
    }

    if (value && dst)
      memcpy(dst, value, size_of_value);
  }

  void * get(viennagrid_int element_id)
  {
    switch ( storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
      {
        if ( (element_id < static_cast<viennagrid_int>(dense_valid_flags.size())) &&
            (dense_valid_flags[element_id]) )
        {
          return &dense_values[0] + element_id*size_of_value;
        }

        break;
      }

      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
      {
        std::map< viennagrid_int, char* >::iterator it = sparse_values.find(element_id);
        if (it != sparse_values.end())
        {
          return &(*it).second[0];
        }

        break;
      }
    }

    return NULL;
  }


  void clear()
  {
    for (std::map< viennagrid_int, char * >::iterator it = sparse_values.begin(); it != sparse_values.end(); ++it)
    {
      delete[] (*it).second;
    }
  }

public:
  viennagrid_int reference_counter;
};












viennagrid_error viennagrid_quantity_field_create(viennagrid_quantity_field * quantity_field)
{
  if (quantity_field)
    *quantity_field = new viennagrid_quantity_field_t;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field quantity_field)
{
  retain(quantity_field);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field)
{
  release(quantity_field);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_init(viennagrid_quantity_field quantity_field,
                                                viennagrid_dimension topological_dimension,
                                                viennagrid_int size_of_value,
                                                viennagrid_int storage_layout)
{
  if (!quantity_field)                                                return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if ((storage_layout != VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE) &&
      (storage_layout != VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE)  ) return VIENNAGRID_ERROR_INVALID_STORAGE_LAYOUT;

  if ( (topological_dimension == quantity_field->topological_dimension) &&
       (storage_layout == quantity_field->storage_layout) &&
       (size_of_value == quantity_field->size_of_value) )
    return VIENNAGRID_SUCCESS;

  quantity_field->topological_dimension = topological_dimension;

  quantity_field->size_of_value = size_of_value;
  quantity_field->storage_layout = storage_layout;

  quantity_field->dense_values.clear();
  quantity_field->dense_valid_flags.clear();
  quantity_field->sparse_values.clear();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_topological_dimension_get(viennagrid_quantity_field quantity_field,
                                                                     viennagrid_dimension * topological_dimension)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (topological_dimension)
    *topological_dimension = quantity_field->topological_dimension;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_storage_layout_get(viennagrid_quantity_field quantity_field,
                                                              viennagrid_int * storage_layout)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (!storage_layout)
    *storage_layout  = quantity_field->storage_layout;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_size_of_value_get(viennagrid_quantity_field quantity_field,
                                                             viennagrid_int * size_of_value)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (size_of_value)
    *size_of_value = quantity_field->size_of_value;

  return VIENNAGRID_SUCCESS;
}








viennagrid_error viennagrid_quantity_field_name_set(viennagrid_quantity_field quantity_field,
                                                    const char * name)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (!name)
    quantity_field->name.clear();
  else
    quantity_field->name = name;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_name_get(viennagrid_quantity_field quantity_field,
                                                    const char ** name)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (name)
    *name = quantity_field->name.c_str();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_unit_set(viennagrid_quantity_field quantity_field,
                                                    const char * unit)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (!unit)
    quantity_field->unit.clear();
  else
    quantity_field->unit = unit;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_unit_get(viennagrid_quantity_field quantity_field,
                                                    const char ** unit)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (unit)
    *unit = quantity_field->unit.c_str();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_resize(viennagrid_quantity_field quantity_field,
                                                  viennagrid_int value_count)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if (value_count < 0) return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  quantity_field->resize(value_count);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_size(viennagrid_quantity_field quantity_field,
                                                viennagrid_int * value_count)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (value_count)
    *value_count = quantity_field->size();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_value_set(viennagrid_quantity_field quantity_field,
                                                     viennagrid_int element_id,
                                                     void * value)
{
  if (!quantity_field)                                            return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if ((element_id < 0) || (element_id >= quantity_field->size())) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  quantity_field->set(element_id, value);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_value_get(viennagrid_quantity_field quantity_field,
                                                     viennagrid_int element_id,
                                                     void ** values)
{
  if (!quantity_field)                                            return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if ((element_id < 0) || (element_id >= quantity_field->size())) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  if (values)
    *values = quantity_field->get(element_id);

  return VIENNAGRID_SUCCESS;
}

