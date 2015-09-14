#include "dynamic_sizeof.hpp"
#include "common.hpp"
#include "buffer.hpp"
#include "element_buffer.hpp"
#include "element_handle_buffer.hpp"
#include "mesh.hpp"
#include "mesh_hierarchy.hpp"
#include "region.hpp"


namespace viennautils
{

  namespace detail
  {
    template<typename IndexT, typename T, typename PointerT, typename ConstPointerT>
    struct dynamic_sizeof_impl< dense_packed_multibuffer<IndexT, T, PointerT, ConstPointerT> >
    {
      static long size(dense_packed_multibuffer<IndexT, T, PointerT, ConstPointerT> const & dpm)
      {
        return dynamic_sizeof(dpm.get_values()) + dynamic_sizeof(dpm.get_offsets());
      }
    };




    template<typename IndexT, typename T, typename PointerT, typename ConstPointerT>
    struct dynamic_sizeof_impl< sparse_packed_multibuffer<IndexT, T, PointerT, ConstPointerT> >
    {
      static long size(sparse_packed_multibuffer<IndexT, T, PointerT, ConstPointerT> const & spm)
      {
        return dynamic_sizeof(spm.get_values()) + dynamic_sizeof(spm.get_offsets());
      }
    };


    template<>
    struct dynamic_sizeof_impl<viennagrid_element_buffer>
    {
      static long size(viennagrid_element_buffer const & eb)
      {
        long size = 0;

        size += dynamic_sizeof( eb.topological_dimension );

        size += dynamic_sizeof( eb.element_types );

        size += dynamic_sizeof( eb.parents );
        size += dynamic_sizeof( eb.aux_data_ );

        size += dynamic_sizeof( eb.region_buffer );
        size += dynamic_sizeof( eb.packed_region_buffer );

        size += dynamic_sizeof( eb.boundary_ids );

        size += dynamic_sizeof( eb.element_map );
        size += dynamic_sizeof( eb.mesh_hierarchy );

        return size;
      }
    };



    template<>
    struct dynamic_sizeof_impl<viennagrid_element_handle_buffer>
    {
      static long size(viennagrid_element_handle_buffer const & ehb)
      {
        long size = 0;

        size += dynamic_sizeof( ehb.ids_ );
        size += dynamic_sizeof( ehb.coboundary_ids );
        size += dynamic_sizeof( ehb.neighbor_ids );

        return size;
      }
    };



    template<typename T, typename SizeT>
    struct dynamic_sizeof_impl< short_vector<T, SizeT> >
    {
      static long size(short_vector<T, SizeT> const & sv)
      {
        long size = sv.size()*sizeof(T);
        for (typename short_vector<T, SizeT>::size_type i = 0; i != sv.size(); ++i)
          size += dynamic_sizeof(sv[i]);
        return size;
      }
    };


    template<>
    struct dynamic_sizeof_impl<viennagrid_mesh_>
    {
      static long size(viennagrid_mesh_ const & m)
      {
        long size = 0;

        size += dynamic_sizeof( m.hierarchy_ );
        size += dynamic_sizeof( m.parent_ );
        size += dynamic_sizeof( m.children );

        size += dynamic_sizeof( m.mesh_children_map );
        size += dynamic_sizeof( m.name_ );
        size += dynamic_sizeof( m.element_handle_buffers );
        size += dynamic_sizeof( m.boundary_elements_ );
        size += dynamic_sizeof( m.boundary_elements_change_counter );

        return size;
      }
    };

    template<>
    struct dynamic_sizeof_impl<viennagrid_region_>
    {
      static long size(viennagrid_region_ const & r)
      {
        long size = 0;

        size += dynamic_sizeof( r.name_ );
        size += dynamic_sizeof( r.id_ );
        size += dynamic_sizeof( r.boundary_elements_ );
        size += dynamic_sizeof( r.boundary_elements_change_counter );
        size += dynamic_sizeof( r.hierarchy_ );

        return size;
      }
    };



    long dynamic_sizeof_impl<viennagrid_mesh_hierarchy_>::size(viennagrid_mesh_hierarchy_ const & mh)
    {
      long size = 0;

      size += dynamic_sizeof( mh.boundary_layout_ );

      long element_buffers_size = dynamic_sizeof( mh.element_buffers );
      size += element_buffers_size;

      size += dynamic_sizeof( mh.geometric_dimension_ );
      size += dynamic_sizeof( mh.cell_dimension_ );

      size += dynamic_sizeof( mh.meshes_ );
      long mesh_size = 0;
      for (std::size_t i = 0; i != mh.meshes_.size(); ++i)
        mesh_size += dynamic_sizeof( *(mh.meshes_[i]) );
      size += mesh_size;

      size += dynamic_sizeof( mh.root_ );

      long vertex_buffer_size = dynamic_sizeof( mh.vertex_buffer );
      size += vertex_buffer_size;

      size += dynamic_sizeof( mh.regions );
      long region_size = 0;
      for (std::size_t i = 0; i != mh.regions.size(); ++i)
        region_size += dynamic_sizeof( *(mh.regions[i]) );
      size += region_size;

      size += dynamic_sizeof( mh.region_id_mapping );
      size += dynamic_sizeof( mh.change_counter_ );

      return size;
    }



  }
}
