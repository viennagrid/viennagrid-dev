#ifndef VIENNAGRID_SEGMENTS_HPP
#define VIENNAGRID_SEGMENTS_HPP

#include "viennagrid/forwards.hpp"

namespace viennagrid
{

    template<typename base_type = int, base_type invalid_id = -1>
    struct segment_id_t
    {
    public:
        segment_id_t() : id(invalid_id) {}
        segment_id_t( base_type id_ ) : id(id_) {}
        
        bool is_valid() const { return id == invalid_id; }
        operator base_type() const { return id; }
        
    private:
        base_type id;
    };
    
    
    
    struct disjunct_segmentation_tag;
    struct overlapping_segmentation_tag;
    
    
    template<typename element_tag, unsigned int geometric_dimension, typename segmentation_type_tag, typename segment_id_type>
    struct element_segment_info_t;

    template<typename element_tag_, unsigned int geometric_dimension_, typename segment_id_type_>
    struct element_segment_info_t<element_tag_, geometric_dimension_, disjunct_segmentation_tag, segment_id_type_>
    {
        typedef segment_id_type_ segment_id_type;
        
        element_segment_info_t() {}
        element_segment_info_t( segment_id_type const & segment_id_ ) : segment_id(segment_id_) {}
        
        segment_id_type segment_id;
        
        template<typename segmentation_type>
        void add_segments( segmentation_type & segmentation ) const { segmentation.add_segment(segment_id); }
    };    
    
    template<typename segment_id_type_>
    struct element_segment_info_t<triangle_tag, 3, disjunct_segmentation_tag, segment_id_type_>
    {
        typedef segment_id_type_ segment_id_type;
        
        element_segment_info_t() {}
        element_segment_info_t( segment_id_type const & positive_orientation_segment_id_,
                                segment_id_type const & negative_orientation_segment_id_) :
                        positive_orientation_segment_id(positive_orientation_segment_id_), negative_orientation_segment_id(negative_orientation_segment_id_) {}
        
        segment_id_type positive_orientation_segment_id;
        segment_id_type negative_orientation_segment_id;
        
        template<typename segmentation_type>
        void add_segments( segmentation_type & segmentation ) const
        {
            segmentation.add_segment(positive_orientation_segment_id);
            segmentation.add_segment(negative_orientation_segment_id);
        }
    };
    
    
    
    template<typename segmentation_type, typename element_segment_info_type>
    void add_segments( segmentation_type & segmentation, element_segment_info_type const & segment_element_info )
    {
        segment_element_info.add_segments(segmentation);
    }
    
    
    
    namespace result_of
    {
        template<typename something>
        struct segment_id_type
        {
            typedef typename something::segment_id_type type;
        };
    }
    
    
    
    template<typename element_type_, typename element_segment_info_container_type_>
    class segmentation_t
    {
    public:
        typedef element_type_ element_type;
        typedef element_segment_info_container_type_ element_segment_info_container_type;
        
        typedef typename element_segment_info_container_type::value_type element_segment_info_type;
        
        typedef typename result_of::segment_id_type<element_segment_info_type>::type segment_id_type;
        
        
        element_segment_info_type const & segment_info( element_type const & element ) const
        {
            return viennagrid::look_up( segmentation, element );
        }
        
        void set_segment_info( element_type const & element, element_segment_info_type const & segment_element_info )
        {
            viennagrid::look_up( segmentation, element ) = segment_element_info;
            add_segments( *this, segment_element_info );
        }
        
        // TODO reference counting on segments -> unused segments disapear
        void add_segment( segment_id_type const & segment_id )
        {
            if (segment_id.is_valid())
                used_segments.insert(segment_id);
        }
        
        std::set<segment_id_type> const & segments() const { return used_segments; }
        
        
    private:
        
        element_segment_info_container_type segmentation;
        std::set<segment_id_type> used_segments;
    };
    

    namespace result_of
    {
        template<typename domain_type, typename element_type_or_tag, typename segmentation_type_tag = disjunct_segmentation_tag, typename segment_id_type = segment_id_t<> >
        struct element_segment_info
        {
            typedef typename viennagrid::result_of::point_type<domain_type>::type point_type;
            static const unsigned int geometric_dimension = viennagrid::result_of::geometric_dimension<domain_type>::value;
            typedef typename viennagrid::result_of::element_tag<element_type_or_tag>::type element_tag;
            
            typedef element_segment_info_t<element_tag, geometric_dimension, segmentation_type_tag, segment_id_type> type;
        };
        
        
        template<typename domain_type, typename element_type_or_tag, typename container_tag = storage::std_deque_tag,
            typename segmentation_type_tag = disjunct_segmentation_tag, typename segment_id_type = segment_id_t<> >
        struct segmentation
        {
            typedef typename element_segment_info<domain_type, element_type_or_tag, segmentation_type_tag, segment_id_type>::type element_segment_info_type;
            typedef typename storage::result_of::container< element_segment_info_type, container_tag >::type element_segment_info_container_type;
            typedef typename viennagrid::result_of::element<domain_type, element_type_or_tag>::type element_type;
            typedef segmentation_t<element_type, element_segment_info_container_type> type;
        };
    }    
}




#endif