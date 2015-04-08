#include <algorithm>
#include <cstring>
#include <cassert>
#include <string>
#include <vector>
#include <map>

#include "viennagrid/quantity_field.h"



// quantities
struct viennagrid_quantity_field_t
{
  std::string name;
  std::string unit;
  viennagrid_dimension topologic_dimension;


  viennagrid_dimension values_dimension;
  viennagrid_int storage_layout;


  std::vector<viennagrid_numeric> dense_values;
  std::vector<bool> dense_valid_flags;

  std::map< viennagrid_int, std::vector<viennagrid_numeric> > sparse_values;


  viennagrid_int use_count;                                                           // initialized with 1
};




viennagrid_error viennagrid_quantity_field_make(viennagrid_quantity_field * quantity_field)
{
  *quantity_field = new viennagrid_quantity_field_t;

  (*quantity_field)->topologic_dimension = -1;

  (*quantity_field)->values_dimension = -1;
  (*quantity_field)->storage_layout = -1;

  (*quantity_field)->use_count = 1;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field quantity_field)
{
  ++quantity_field->use_count;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field)
{
  if (!quantity_field)
    return VIENNAGRID_SUCCESS;

  if ( --(quantity_field->use_count) <= 0 )
    delete quantity_field;

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_init(viennagrid_quantity_field quantity_field,
                                                viennagrid_dimension topologic_dimension,
                                                viennagrid_dimension values_dimension,
                                                viennagrid_int storage_layout)
{
  if ( (topologic_dimension == quantity_field->topologic_dimension) &&
       (storage_layout == quantity_field->storage_layout) &&
       (values_dimension == quantity_field->values_dimension) )
    return VIENNAGRID_SUCCESS;

  if ( (storage_layout != QUANTITY_FIELD_STORAGE_DENSE) && (storage_layout != QUANTITY_FIELD_STORAGE_DENSE) )
    return VIENNAGRID_INVALID_ARGUMENTS;

  quantity_field->topologic_dimension = topologic_dimension;

  quantity_field->values_dimension = values_dimension;
  quantity_field->storage_layout = storage_layout;

  quantity_field->dense_values.clear();
  quantity_field->dense_valid_flags.clear();
  quantity_field->sparse_values.clear();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_get_topologic_dimension(viennagrid_quantity_field quantity_field,
                                                                   viennagrid_dimension * topologic_dimension)
{
  *topologic_dimension = quantity_field->topologic_dimension;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_get_storage_layout(viennagrid_quantity_field quantity_field,
                                                              viennagrid_int * storage_layout)
{
  *storage_layout  = quantity_field->storage_layout;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_get_values_dimension(viennagrid_quantity_field quantity_field,
                                                                viennagrid_dimension * values_dimension)
{
  *values_dimension = quantity_field->values_dimension;
  return VIENNAGRID_SUCCESS;
}








viennagrid_error viennagrid_quantity_field_set_name(viennagrid_quantity_field quantity_field,
                                                const char * name)
{
//   if (quantity_field->name)
//     delete[] quantity_field->name;
//
//   if (!name)
//   {
//     quantity_field->name = NULL;
//     return VIENNAGRID_SUCCESS;
//   }
//
//   size_t string_length = strlen(name);
//   quantity_field->name = new char[string_length+1];
//
//   memcpy( quantity_field->name, name, (string_length+1)*sizeof(char) );

  if (!name)
    quantity_field->name.clear();
  else
    quantity_field->name = name;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_get_name(viennagrid_quantity_field quantity_field,
                                                const char ** name)
{
  if (name)
    *name = quantity_field->name.c_str();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_set_unit(viennagrid_quantity_field quantity_field,
                                                const char * unit)
{
//   if (quantity_field->unit)
//     delete[] quantity_field->unit;
//
//   if (!unit)
//   {
//     quantity_field->unit = NULL;
//     return VIENNAGRID_SUCCESS;
//   }
//
//
//   size_t string_length = strlen(unit);
//   quantity_field->unit = new char[string_length+1];
//
//   memcpy( quantity_field->unit, unit, (string_length+1)*sizeof(char) );

  if (!unit)
    quantity_field->unit.clear();
  else
    quantity_field->unit = unit;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_get_unit(viennagrid_quantity_field quantity_field,
                                                const char ** unit)
{
  if (unit)
    *unit = quantity_field->unit.c_str();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_resize(viennagrid_quantity_field quantity_field,
                                              viennagrid_int value_count)
{
  switch ( quantity_field->storage_layout )
  {
    case QUANTITY_FIELD_STORAGE_DENSE:
      quantity_field->dense_values.resize( quantity_field->values_dimension * value_count );
      quantity_field->dense_valid_flags.resize( value_count, false );
      break;
    case QUANTITY_FIELD_STORAGE_SPARSE:
      break;
    default:
      assert(false);
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_size(viennagrid_quantity_field quantity_field,
                                            viennagrid_int * value_count)
{
  if (value_count)
  {
    switch ( quantity_field->storage_layout )
    {
      case QUANTITY_FIELD_STORAGE_DENSE:
        *value_count = quantity_field->dense_values.size() / quantity_field->values_dimension;
        break;
      case QUANTITY_FIELD_STORAGE_SPARSE:
        *value_count = quantity_field->sparse_values.size();
        break;
      default:
        assert(false);
    }
  }

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_set_value(viennagrid_quantity_field quantity_field,
                                                 viennagrid_index element_id,
                                                 viennagrid_numeric * values)
{
  assert( 0 <= element_id );

  viennagrid_numeric * dst = NULL;

  switch ( quantity_field->storage_layout )
  {
    case QUANTITY_FIELD_STORAGE_DENSE:
      if ( element_id >= static_cast<viennagrid_index>(quantity_field->dense_valid_flags.size()) )
        viennagrid_quantity_field_resize(quantity_field, element_id+1);

      dst = &quantity_field->dense_values[0] + element_id*quantity_field->values_dimension;
      quantity_field->dense_valid_flags[element_id] = true;
      break;

    case QUANTITY_FIELD_STORAGE_SPARSE:
      dst = &quantity_field->sparse_values[element_id][0];
      break;

    default:
      assert(false);
  }

  std::copy(values, values+quantity_field->values_dimension, dst);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_get_value(viennagrid_quantity_field quantity_field,
                                                 viennagrid_index element_id,
                                                 viennagrid_numeric ** values)
{
  assert( 0 <= element_id );

  switch ( quantity_field->storage_layout )
  {
    case QUANTITY_FIELD_STORAGE_DENSE:
    {
      if ( (element_id < static_cast<viennagrid_index>(quantity_field->dense_valid_flags.size())) &&
           (quantity_field->dense_valid_flags[element_id]) )
      {
        *values = &quantity_field->dense_values[0] + element_id*quantity_field->values_dimension;
      }
      else
      {
        *values = NULL;
      }
      break;
    }

    case QUANTITY_FIELD_STORAGE_SPARSE:
    {
      std::map< viennagrid_int, std::vector<viennagrid_numeric> >::iterator it = quantity_field->sparse_values.find(element_id);
      if (it != quantity_field->sparse_values.end())
        *values = &(*it).second[0];
      else
        *values = NULL;
      break;
    }

    default:
      assert(false);
  }

  return VIENNAGRID_SUCCESS;
}

