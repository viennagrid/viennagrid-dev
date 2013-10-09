#ifndef VIENNAGRID_CONFIG_ID_GENERATOR_CONFIG_HPP
#define VIENNAGRID_CONFIG_ID_GENERATOR_CONFIG_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/storage/id_generator.hpp"

namespace viennagrid
{
  namespace result_of
  {
    template<typename mesh_config, typename id_generator_tag>
    struct id_generator_impl;

    template<typename mesh_config>
    struct id_generator_impl<mesh_config, config::continuous_id_generator_tag>
    {
      typedef typename viennagrid::storage::result_of::continuous_id_generator_config_from_mesh_config<mesh_config>::type typemap;
      typedef storage::continuous_id_generator<typemap> type;
    };

    template<typename WrappedConfig>
    struct id_generator
    {
      typedef typename config::result_of::query<WrappedConfig, config::continuous_id_generator_tag, config::id_generator_tag>::type id_generator_tag;
      typedef typename id_generator_impl<WrappedConfig, id_generator_tag>::type type;
    };
  }
}

#endif
