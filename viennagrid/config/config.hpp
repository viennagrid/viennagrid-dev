#ifndef VIENNAGRID_CONFIG_HPP
#define VIENNAGRID_CONFIG_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/meta/typemap.hpp"
#include "viennagrid/storage/forwards.hpp"
#include "../meta/utils.hpp"

namespace viennagrid
{

    namespace config
    {
        struct topology_config_tag;

        struct element_id_tag;
        struct element_container_tag;
        struct element_boundary_storage_layout_tag;
        struct element_appendix_type_tag;

        struct id_generator_tag;
        struct continuous_id_generator_tag;

        struct coboundary_container_tag;
        struct coboundary_view_container_tag;

        struct neighbour_container_tag;
        struct neighbour_view_container_tag;


        struct boundary_information_container_tag;

        struct interface_information_container_tag;

        struct vector_type_tag;

        struct mesh_change_counter_tag;


        namespace result_of
        {
            template<typename ConfigEntryT>
            struct unpack
            {
                typedef typename ConfigEntryT::second type;
            };

            template<>
            struct unpack<viennagrid::meta::not_found>
            {
                typedef viennagrid::meta::not_found type;
            };



            template<typename ConfigT, typename DefaultT,
                     typename SearchTag0T = viennagrid::meta::null_type, typename SearchTag1T = viennagrid::meta::null_type, typename SearchTag2T = viennagrid::meta::null_type,
                     typename SearchTag3T = viennagrid::meta::null_type, typename SearchTag4T = viennagrid::meta::null_type, typename SearchTag5T = viennagrid::meta::null_type,
                     typename SearchTag6T = viennagrid::meta::null_type, typename SearchTag7T = viennagrid::meta::null_type, typename SearchTag8T = viennagrid::meta::null_type,
                     typename SearchTag9T = viennagrid::meta::null_type>
            struct query;

            template<typename DefaultT, typename SearchTag0T, typename SearchTag1T, typename SearchTag2T, typename SearchTag3T, typename SearchTag4T,
                                    typename SearchTag5T, typename SearchTag6T, typename SearchTag7T, typename SearchTag8T, typename SearchTag9T>
            struct query<viennagrid::meta::not_found, DefaultT, SearchTag0T, SearchTag1T, SearchTag2T, SearchTag3T, SearchTag4T,
                                                                                    SearchTag5T, SearchTag6T, SearchTag7T, SearchTag8T, SearchTag9T>
            {
              typedef DefaultT type;
            };



            template<typename HeadT, typename TailT, typename DefaultT, typename SearchTag0T, typename SearchTag2T, typename SearchTag3T, typename SearchTag4T,
                                    typename SearchTag5T, typename SearchTag6T, typename SearchTag7T, typename SearchTag8T, typename SearchTag9T>
            struct query<viennagrid::meta::typelist_t<HeadT, TailT>, DefaultT, SearchTag0T, viennagrid::meta::null_type, SearchTag2T, SearchTag3T, SearchTag4T,
                                                               SearchTag5T, SearchTag6T, SearchTag7T, SearchTag8T, SearchTag9T>
            {
              typedef viennagrid::meta::typelist_t<HeadT, TailT> ConfigType;
              typedef typename unpack<typename viennagrid::meta::typemap::result_of::find<ConfigType, SearchTag0T>::type >::type EntryType;
              typedef typename viennagrid::meta::IF<
                  viennagrid::meta::EQUAL<EntryType, viennagrid::meta::not_found>::value,
                  DefaultT,
                  EntryType>::type type;
            };


            template<typename ConfigT, typename DefaultT, typename SearchTag1T, typename SearchTag2T, typename SearchTag3T, typename SearchTag4T,
                                    typename SearchTag5T, typename SearchTag6T, typename SearchTag7T, typename SearchTag8T, typename SearchTag9T>
            struct query<ConfigT, DefaultT, viennagrid::meta::null_type, SearchTag1T, SearchTag2T, SearchTag3T, SearchTag4T,
                                                                               SearchTag5T, SearchTag6T, SearchTag7T, SearchTag8T, SearchTag9T>
            {
              typedef DefaultT type;
            };



            template<typename ConfigT, typename DefaultT,
                     typename SearchTag0T, typename SearchTag1T, typename SearchTag2T,
                     typename SearchTag3T, typename SearchTag4T, typename SearchTag5T,
                     typename SearchTag6T, typename SearchTag7T, typename SearchTag8T,
                     typename SearchTag9T>
            struct query
            {
              typedef typename unpack<typename viennagrid::meta::typemap::result_of::find<ConfigT, SearchTag0T>::type >::type EntryType;
              typedef typename query<
                    EntryType, DefaultT,
                    SearchTag1T, SearchTag2T, SearchTag3T, SearchTag4T, SearchTag5T, SearchTag6T, SearchTag7T, SearchTag8T, SearchTag9T
                  >::type type;
            };

        }
    }

}


#endif
