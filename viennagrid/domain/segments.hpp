#ifndef VIENNAGRID_SEGMENTS_HPP
#define VIENNAGRID_SEGMENTS_HPP

#include "viennagrid/forwards.hpp"

namespace viennameta {
template<class first_, class second_ >
struct static_pair;}

namespace viennagrid
{
    template<typename base_type = int, base_type invalid_id = -1>
    struct segment_id_t
    {
    public:
        segment_id_t() : id(invalid_id) {}
        segment_id_t( base_type id_ ) : id(id_) {}
        
        bool is_valid() { return id == invalid_id; }
        operator base_type() const { return id; }
        
    private:
        base_type id;
    };
    
    
    
    struct disjunct_segmentation_tag;
    struct overlapping_segmentation_tag;
    
    
    template<typename element_tag, unsigned int geometric_dimension, typename segmentation_type_tag, typename segment_id_type>
    struct element_segment_info;

    template<typename segment_id_type>
    struct element_segment_info<line_tag, 1, disjunct_segmentation_tag, segment_id_type>
    {
        segment_id_type segment_id;
    };
    
    template<typename segment_id_type>
    struct element_segment_info<triangle_tag, 2, disjunct_segmentation_tag, segment_id_type>
    {
        segment_id_type segment_id;
    };
    
    template<typename segment_id_type>
    struct element_segment_info<tetrahedron_tag, 3, disjunct_segmentation_tag, segment_id_type>
    {
        segment_id_type segment_id;
    };
    
    template<typename segment_id_type>
    struct element_segment_info<triangle_tag, 3, disjunct_segmentation_tag, segment_id_type>
    {
        segment_id_type positive_orientation_segment_id;
        segment_id_type negative_orientation_segment_id;
    };
    

    
    template<typename element_segmentation_info_type_, typename segmentation_id_type_>
    struct segmentation_info
    {
        typedef element_segmentation_info_type_ element_segmentation_info_type;
        typedef segmentation_id_type_ segmentation_id_type;
        
        element_segmentation_info_type & operator() () { return default_segmentation; }
        element_segmentation_info_type const & operator() () const { return default_segmentation; }
        
        element_segmentation_info_type & operator[] ( segmentation_id_type const & id ) { return segmentations[id]; }
        element_segmentation_info_type const & operator() ( segmentation_id_type const & id ) const
        {
            typename std::map<segmentation_id_type, element_segmentation_info_type>::const_iterator it = segmentations.find(id);
            assert( it != segmentations.end() );
            return *it;
        }

        
        element_segmentation_info_type default_segmentation;
        std::map<segmentation_id_type, element_segmentation_info_type> segmentations;
    };
    
    
    
    
    
//     template<typename domain_type, typename element_type>
    
    
    
    
    
    
    
    
    namespace result_of
    {
        template<typename domain_type, typename element_type_or_tag,
            typename segmentation_type_tag = disjunct_segmentation_tag,
            typename segment_id_type = segment_id_t<>,
            typename segmentation_id_type = int
            >
        struct segment_info
        {
            typedef typename viennagrid::result_of::point_type<domain_type>::type point_type;
            static const unsigned int geometric_dimension = viennagrid::result_of::geometric_dimension<domain_type>::value;
            typedef typename viennagrid::result_of::element_tag<element_type_or_tag>::type element_tag;
            
            typedef element_segment_info<element_tag, geometric_dimension, segmentation_type_tag, segment_id_type> element_segment_info_type;
            
            typedef segmentation_info< element_segment_info_type, segmentation_id_type > type;
        };
        
        template<typename domain_type,
            typename segmentation_type_tag = disjunct_segmentation_tag,
            typename segment_id_type = segment_id_t<>,
            typename segmentation_id_type = int
            >
        struct cell_segment_info
        {
            typedef typename viennagrid::result_of::cell_type<domain_type>::type cell_type;
            typedef typename segment_info<domain_type, cell_type, segmentation_type_tag, segment_id_type, segmentation_id_type>::type type;
        };
        

        
        template<typename domain_config, typename segment_id_type = segment_id_t<>, typename  segmentation_id_type = int>
        struct cell_segmented_domain_config
        {
            typedef typename viennagrid::result_of::domain<domain_config>::type domain_type;
            typedef typename viennagrid::result_of::cell_type<domain_type>::type cell_type;
            typedef typename viennagrid::result_of::element_tag<cell_type>::type cell_tag;
            static const int geometric_dimension = viennagrid::result_of::geometric_dimension<domain_type>::value;
            
            typedef typename viennagrid::result_of::cell_segment_info<domain_type>::type cell_segment_information;
            
            typedef typename viennameta::typemap::result_of::insert_or_modify<
                    typename viennagrid::config::result_of::query_config<domain_config, viennagrid::config::metainfo_typelist_tag>::type,
                    viennameta::static_pair<cell_tag, cell_segment_information>
                >::type segmentation_metainfo_typemap;
            
            typedef typename viennameta::make_typemap<
                viennagrid::config::segment_id_type_tag, segment_id_type,
                viennagrid::config::segmentation_id_type_tag, segmentation_id_type,
                viennagrid::config::element_segmentation_tag, typename viennameta::make_typemap<
                                                                  cell_tag, cell_segment_information
                                                              >::type
            >::type segmentation_config_type;
            
            typedef typename viennameta::typemap::result_of::merge_overwrite<
                typename viennameta::typemap::result_of::insert_or_modify<
                    domain_config,
                    viennameta::static_pair<
                        viennagrid::config::metainfo_typelist_tag,
                        segmentation_metainfo_typemap
                    >
                >::type,
                segmentation_config_type
            >::type type;
        };

        
        
        template<typename something, typename element_type_or_tag>
        struct element_segment_information
        {
            typedef typename viennagrid::result_of::element_tag<element_type_or_tag>::type element_tag;
            typedef typename viennagrid::result_of::config<something>::type config_type;
            typedef typename viennagrid::config::result_of::query_config<config_type, viennagrid::config::element_segmentation_tag>::type element_segmentation_config;
            typedef typename viennagrid::config::result_of::query_config<element_segmentation_config, element_tag>::type type;
        };
    }
    
    
    template<typename domain_type, typename element_type>
    typename result_of::element_segment_information<domain_type, element_type>::type & segment_info( domain_type & domain, element_type const & element )
    {
        return viennagrid::look_up< typename result_of::element_segment_information<domain_type, element_type>::type >( domain, element );
    }
    
    template<typename domain_type, typename element_type>
    typename result_of::element_segment_information<domain_type, element_type>::type::element_segmentation_info_type & default_segment( domain_type & domain, element_type const & element )
    {
        return viennagrid::look_up< typename result_of::element_segment_information<domain_type, element_type>::type >( domain, element )();
    }
    
    
}




#endif