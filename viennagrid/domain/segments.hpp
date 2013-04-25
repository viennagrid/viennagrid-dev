#ifndef VIENNAGRID_SEGMENTS_HPP
#define VIENNAGRID_SEGMENTS_HPP

#include <string>
#include "viennagrid/forwards.hpp"

namespace viennagrid
{
    
    struct disjunct_segmentation_tag;
    struct overlapping_segmentation_tag;
    
    
    template<typename segmentation_id_type_ = std::string>
    class segmentation_t
    {
    public:
        typedef segmentation_id_type_ segmentation_id_type;
        
        segmentation_t(segmentation_id_type const & id_) : id (id_) {}
        
    private:
        
        segmentation_id_type id;
    };
    
    
    
    
    template<typename cell_tag, typename element_tag, typename segmentation_type_tag, typename segment_id_type>
    struct element_segment_info
    {
        std::set<segment_id_type> segment_ids;
    };
    
    template<typename cell_tag, typename segment_id_type>
    struct element_segment_info<cell_tag, cell_tag, disjunct_segmentation_tag, segment_id_type>
    {
        segment_id_type segment_id;
    };
    
    template<typename cell_tag, typename segment_id_type>
    struct element_segment_info<cell_tag, triangle_tag, disjunct_segmentation_tag, segment_id_type>
    {
        segment_id_type positive_orientation_segment_id;
        segment_id_type negative_orientation_segment_id;
    };
    

    
    
//     template<typename domain_type, typename segmentation_id_type, typename element_type>
    
    
    
    
    
    
    namespace result_of
    {
        template<typename domain_type>
        struct segment_info
        {
            
        };
        
        
        
//         template<typename cell_tag, typename element_tag, typename segmentation_type_tag, typename segment_id_type = long>
//         struct segment_info
//         {
//             typedef element_segment_info<cell_tag, element_tag, segmentation_type_tag, segment_id_type> type;
//         };
//         
//         template<typename segmentation_id_type, typename cell_tag, typename element_tag, typename segmentation_type_tag, typename segment_id_type = long>
//         struct segmentation_info
//         {
//             typedef std::map< segmentation_id_type, segment_info<cell_tag, element_tag, segmentation_type_tag, segment_id_type> > type;
//         };
        
        
    }
    
    
}




#endif