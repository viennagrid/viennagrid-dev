#ifndef VIENNAGRID_SEGMENTATION_HPP
#define VIENNAGRID_SEGMENTATION_HPP

#include <limits>
#include "accessor.hpp"

#include "viennagrid/forwards.hpp"
#include "viennagrid/domain/topology.hpp"

// #include "viennagrid/algorithm/inner_prod.hpp"
// #include "viennagrid/algorithm/cross_prod.hpp"
// #include "viennagrid/algorithm/geometry.hpp"
// #include "viennagrid/algorithm/centroid.hpp"
// #include "viennagrid/domain/coboundary_iteration.hpp"


    
namespace viennagrid
{
    
    
    template<typename segmentation_type_>
    class segment_t
    {
    public:
        
        typedef segmentation_type_ segmentation_type;
        typedef typename segmentation_type::segment_id_type segment_id_type;
        typedef typename segmentation_type::domain_type domain_type;
        typedef typename segmentation_type::view_type view_type;
        
        segment_t( segmentation_type & segmentation_x, view_type & view_x, segment_id_type const & segment_id_) :
            segmentation_(&segmentation_x), view_(&view_x), segment_id(segment_id_) {}

        
        segment_id_type const & id() const { return segment_id; }
        
        
        segmentation_type & segmentation() { return *segmentation_; }
        segmentation_type const & segmentation() const { return *segmentation_; }
        
        view_type & view() { return *view_; }
        view_type const & view() const { return *view_; }
        
        
        bool operator<( const segment_t & rhs ) const { return segment_id < rhs.segment_id; }
        
    private:
        
        segmentation_type * segmentation_;
        view_type * view_;
        
        segment_id_type segment_id;
    };
    
    
    namespace result_of
    {
        template<typename segmentation_type, typename element_type_or_tag>
        struct element_range< segment_t<segmentation_type>, element_type_or_tag >
        {
            typedef typename element_range<typename segment_t<segmentation_type>::view_type, element_type_or_tag>::type type;
        };
        
        template<typename segmentation_type, typename element_type_or_tag>
        struct const_element_range< segment_t<segmentation_type>, element_type_or_tag >
        {
            typedef typename const_element_range<typename segment_t<segmentation_type>::view_type, element_type_or_tag>::type type;
        };
    }
    
    template<typename element_type_or_tag, typename segmentation_type>
    typename result_of::element_range<segment_t<segmentation_type>, element_type_or_tag>::type elements( segment_t<segmentation_type> & segment)
    {
        return elements<element_type_or_tag>( segment.view() );
    }
    
    template<typename element_type_or_tag, typename segmentation_type>
    typename result_of::const_element_range<segment_t<segmentation_type>, element_type_or_tag>::type elements(segment_t<segmentation_type> const & segment)
    {
        return elements<element_type_or_tag>( segment.view() );
    }
    
    
    template<typename segmentation_type, typename handle_type>
    typename storage::handle::result_of::value_type<handle_type>::type & dereference_handle( segment_t<segmentation_type> & segment, const handle_type & handle)
    { return dereference_handle( segment.view(), handle ); }
    
    template<typename segmentation_type, typename handle_type>
    typename storage::handle::result_of::value_type<handle_type>::type const & dereference_handle( segment_t<segmentation_type> const & segment, const handle_type & handle)
    { return dereference_handle( segment.view(), handle ); }
    
    
    
    template<typename segmentation_type, typename value_type>
    typename result_of::handle<typename segment_t<segmentation_type>::view_type, value_type>::type handle( segment_t<segmentation_type> & segment, value_type & element)
    { return handle( segment.view(), element ); }
    
    template<typename segmentation_type, typename value_type>
    typename result_of::const_handle<typename segment_t<segmentation_type>::view_type, value_type>::type handle( segment_t<segmentation_type> const & segment, value_type const & element)
    { return handle( segment.view(), element ); }
    
    
    
    
    
    template<typename container_type_>
    struct segment_interface_information_wrapper
    {
      typedef container_type_ container_type;
      segment_interface_information_wrapper() : change_counter(0) {}
      
        long change_counter;
        container_type container;
    };
    
    
    
    template<typename segment_id_type, typename container_type_>
    struct interface_information_wrapper
    {
        interface_information_wrapper() {}
        
        std::map< std::pair<segment_id_type,segment_id_type>, segment_interface_information_wrapper<container_type_> > interface_flags;
    };
    

    
    namespace result_of
    {
      template<typename WrappedConfigType>
      struct segmentation_domain_type
      {
        typedef typename WrappedConfigType::domain_type type;
      };
      
      template<typename WrappedConfigType>
      struct segmentation_view_type
      {
        typedef typename WrappedConfigType::view_type type;
      };
      
      template<typename WrappedConfigType>
      struct segmentation_segment_id_type
      {
        typedef typename WrappedConfigType::segment_id_type type;
      };
      
      template<typename WrappedConfigType>
      struct segmentation_appendix_type
      {
        typedef typename WrappedConfigType::appendix_type type;
      };
      
      template<typename WrappedConfigType>
      struct segmentation_view_container_tag
      {
        typedef typename WrappedConfigType::view_container_tag type;
      };
      
    }
    
    
    
    namespace config
    {
        template<typename domain_type_, typename view_type_, typename segment_id_type_, typename appendix_type_, typename view_container_tag_>
        struct segmentation_config_wrapper_t
        {
          typedef domain_type_ domain_type;
          typedef view_type_ view_type;
          typedef segment_id_type_ segment_id_type;
          typedef appendix_type_ appendix_type;
          typedef view_container_tag_ view_container_tag;
        };
    }
    
    
//     template<typename domain_type_, typename view_type_, typename segment_id_type_, typename appendix_type_ = viennameta::null_type, typename view_container_tag = viennagrid::storage::std_deque_tag>
    template<typename WrappedConfigType>
    class segmentation_t
    {
    public:
        
        typedef typename result_of::segmentation_domain_type<WrappedConfigType>::type domain_type;
        typedef typename result_of::segmentation_view_type<WrappedConfigType>::type view_type;
        typedef typename result_of::segmentation_segment_id_type<WrappedConfigType>::type segment_id_type;
        typedef typename result_of::segmentation_appendix_type<WrappedConfigType>::type appendix_type;
        typedef typename result_of::segmentation_view_container_tag<WrappedConfigType>::type view_container_tag;
        
//         typedef appendix_type_ appendix_type;
        
        typedef segmentation_t<WrappedConfigType> self_type;
        
        typedef segment_t<self_type> segment_type;
        
        typedef typename viennagrid::storage::result_of::container< view_type, view_container_tag >::type view_container_type;
        typedef std::map<segment_id_type, segment_type> segment_id_map_type;
        
        
        segmentation_t( domain_type & domain_x ) : highest_id(-1), domain_(&domain_x) {}
        
        
        
        domain_type & domain() { return *domain_; }
        domain_type const & domain() const { return *domain_; }
        
        appendix_type & appendix() { return appendix_; }
        appendix_type const & appendix() const { return appendix_; }
        
        
        bool segment_present( segment_id_type const & segment_id ) const { return segment_id_map.find(segment_id) != segment_id_map.end(); }       
        
        // create a new segment
        segment_type create_segment( segment_id_type const & segment_id )
        {
            typename segment_id_map_type::iterator it = segment_id_map.find(segment_id);
            
            if (it != segment_id_map.end())
                return it->second; // segment already is present
            
            view_segments.push_back( viennagrid::create_view( domain() ) );
            
            segment_type segment( *this, view_segments.back(), segment_id );
            
            segment_id_map.insert( std::make_pair(segment_id, segment) );
            
            if ( highest_id < segment_id )
                highest_id = segment_id;
            
            return segment;
        }
        
        segment_type create_segment() { return create_segment( ++highest_id ); }
        

        
        
        class iterator : public segment_id_map_type::iterator
        {
            typedef typename segment_id_map_type::iterator base;
        public:
            iterator() {}
            iterator(base const & foo) : base(foo) {}
            
            typedef segment_type value_type;
            typedef segment_type & reference;
            typedef segment_type * pointer;
            
            reference operator* () { return base::operator*().second; }
            const reference operator* () const { return base::operator*().second; }
        };        
        
        class const_iterator : public segment_id_map_type::const_iterator
        {
            typedef typename segment_id_map_type::const_iterator base;
        public:
            const_iterator() {}
            const_iterator(base const & foo) : base(foo) {}
            const_iterator(iterator & it) : base(it) {}
            
            typedef segment_type value_type;
            typedef segment_type const & reference;
            typedef segment_type const * pointer;
            
            reference operator* () const { return base::operator*().second; }
        };
        
        iterator begin() { return iterator(segment_id_map.begin()); }
        iterator end() { return iterator(segment_id_map.end()); }
        
        const_iterator cbegin() const { return const_iterator(segment_id_map.begin()); }
        const_iterator cend() const { return const_iterator(segment_id_map.end()); }
        
        const_iterator begin() const { return cbegin(); }
        const_iterator end() const { return cend(); }
        
        iterator find( segment_id_type const & segment_id ) { return iterator( segment_id_map.find(segment_id) ); }
        const_iterator find( segment_id_type const & segment_id ) const { return const_iterator( segment_id_map.find(segment_id) ); }
        
       
    private:
        
        segment_id_type highest_id;
        segment_id_map_type segment_id_map;
        
        view_container_type view_segments;
        
        appendix_type appendix_;
        
        domain_type * domain_;
    };
    
    
    
    
    
    
    
    template<typename segment_id_type_, typename element_segment_info_type_ = viennagrid::meta::null_type>
    struct element_segment_mapping
    {        
        typedef segment_id_type_ segment_id_type;
        typedef element_segment_info_type_ element_segment_info_type;
        
        element_segment_mapping() {}
        element_segment_mapping( segment_id_type const & segment_id_ ) : segment_id(segment_id_) {}
        
        segment_id_type segment_id;
        element_segment_info_type element_segment_info;
    };
    
    
    
    template<typename element_segment_mapping_type, typename container_tag = viennagrid::storage::std_deque_tag>
    struct segment_info_t
    {
        typedef typename element_segment_mapping_type::segment_id_type segment_id_type;
        typedef typename element_segment_mapping_type::element_segment_info_type element_segment_info_type;
        
        typedef typename viennagrid::storage::result_of::container<element_segment_mapping_type, container_tag>::type element_segment_mapping_container_type;
        element_segment_mapping_container_type element_segment_mapping_container;
    };
    
    
    
    
    
    struct element_segment_mapping_tag;
    
    
    
    namespace result_of
    {
        template<typename element_segment_info_container_typemap, typename segment_id_type, typename container_tag, typename segment_info_container_tag>
        struct segmentation_info_container_typemap;
        
        template<typename segment_id_type, typename container_tag, typename segment_info_container_tag>
        struct segmentation_info_container_typemap< viennagrid::meta::null_type, segment_id_type, container_tag, segment_info_container_tag >
        {
            typedef viennagrid::meta::null_type type;
        };
        
        template<typename element_tag, typename segment_info_type, typename tail, typename segment_id_type, typename container_tag, typename segment_info_container_tag>
        struct segmentation_info_container_typemap< viennagrid::meta::typelist_t<viennagrid::meta::static_pair<element_tag, segment_info_type>, tail>, segment_id_type, container_tag, segment_info_container_tag >
        {
            typedef viennagrid::meta::static_pair< element_tag, segment_info_type > key_type;
            typedef typename viennagrid::storage::result_of::container< std::pair<segment_id_type, segment_info_type>, segment_info_container_tag>::type segment_info_container;
            typedef typename viennagrid::storage::result_of::container<segment_info_container, container_tag>::type container_type;
            
            typedef viennagrid::meta::typelist_t<
                viennagrid::meta::static_pair<
                    element_tag,
                    container_type
                >,
                typename segmentation_info_container_typemap<tail, segment_id_type, container_tag, segment_info_container_tag>::type
            > type;
        };
        
        
 
        
        
        
        
        template<typename element_typelist, typename segment_id_type, typename container_tag = viennagrid::storage::std_deque_tag>
        struct trivial_segmentation_appendix;

        template<typename segment_id_type,typename container_tag>
        struct trivial_segmentation_appendix< viennagrid::meta::null_type, segment_id_type, container_tag >
        {
            typedef viennagrid::meta::null_type type;
        };
        
        template<typename element_type, typename tail, typename segment_id_type, typename container_tag>
        struct trivial_segmentation_appendix< viennagrid::meta::typelist_t<element_type, tail>, segment_id_type, container_tag >
        {
            typedef viennagrid::meta::typelist_t<
                viennagrid::meta::static_pair<
                    element_type,
                    typename viennagrid::storage::result_of::container<
                        segment_info_t< element_segment_mapping<segment_id_type> >,
                        container_tag
                    >::type
                >,
                typename trivial_segmentation_appendix<tail, segment_id_type, container_tag>::type
            > type;
        };
        
        
        
        
        template<typename domain_type,
                 typename view_type = typename viennagrid::result_of::domain_view<domain_type>::type,
                 typename segment_id_type = int,
                 typename appendix_type =
                    viennagrid::storage::collection_t<
                        typename viennagrid::meta::make_typemap<
                            element_segment_mapping_tag,
                            viennagrid::storage::collection_t<
                                typename trivial_segmentation_appendix<
                                    typename viennagrid::result_of::element_typelist<domain_type>::type,
                                    segment_id_type
                                >::type
                            >
                        >::type
                    >,
                typename view_container_tag = viennagrid::storage::std_deque_tag >
        struct segmentation
        {
          typedef config::segmentation_config_wrapper_t<domain_type, view_type, segment_id_type, appendix_type, view_container_tag> WrappedConfigType;
          
          typedef segmentation_t<WrappedConfigType> type;
        };
        
        
        
        
        template<typename domain_type,
                 typename view_type = typename viennagrid::result_of::domain_view<domain_type>::type,
                 typename segment_id_type = int,
                 typename appendix_type =
                 viennagrid::storage::collection_t<
                    typename viennameta::make_typemap<
                        element_segment_mapping_tag,
                        viennagrid::storage::collection_t<
                            typename viennameta::typemap::result_of::modify<
                                typename trivial_segmentation_appendix<
                                    typename viennagrid::result_of::element_typelist<domain_type>::type,
                                    segment_id_type
                                >::type,
                                viennameta::static_pair<
                                    typename viennagrid::result_of::element< domain_type, viennagrid::triangle_tag >::type,
                                    typename viennagrid::storage::result_of::container<
                                        segment_info_t< element_segment_mapping<segment_id_type, bool> >,
                                        viennagrid::storage::std_deque_tag
                                    >::type
                                >
                            >::type
                        >
                    >::type
                >,
                typename view_container_tag = viennagrid::storage::std_deque_tag >
        struct oriented_3d_hull_segmentation
        {
          typedef config::segmentation_config_wrapper_t<domain_type, view_type, segment_id_type, appendix_type, view_container_tag> WrappedConfigType;
          
          typedef segmentation_t<WrappedConfigType> type;
        };
        
    }
    
    
    
    
    template<typename segment_type>
    typename viennagrid::storage::result_of::value_type<
            typename segment_type::segmentation_type::appendix_type,
            element_segment_mapping_tag 
        >::type & element_segment_mapping_collection( segment_type & segment )
    {
        return viennagrid::storage::collection::get<element_segment_mapping_tag>( segment.segmentation().appendix() );
    }
    
    template<typename segment_type>
    typename viennagrid::storage::result_of::value_type<
            typename segment_type::segmentation_type::appendix_type,
            element_segment_mapping_tag 
        >::type const & element_segment_mapping_collection( segment_type const & segment )
    {
        return viennagrid::storage::collection::get<element_segment_mapping_tag>( segment.segmentation().appendix() );
    }
    
    
    

    
    
    
    
    
    
    template<typename segment_type, typename element_segment_mapping_type, typename container_tag>
    bool is_in_segment( segment_type const & segment, segment_info_t<element_segment_mapping_type, container_tag> const & segment_info )
    {
        typedef typename segment_info_t<element_segment_mapping_type, container_tag>::element_segment_mapping_container_type element_segment_mapping_container_type;
        
        for (typename element_segment_mapping_container_type::const_iterator it = segment_info.element_segment_mapping_container.begin();
             it != segment_info.element_segment_mapping_container.end();
             ++it)
        {
            if (it->segment_id == segment.id()) return true;
        }
            
        return false;
    }
    
    template< typename segment_type, typename accessor_type, typename element_type >
    bool is_in_segment( segment_type const & segment, accessor_type const accessor, element_type const & element )
    {
        return is_in_segment( segment, accessor(element) );
    }
    
    template<typename segment_type, typename ElementTag, typename WrappedConfigType>
    bool is_in_segment( segment_type const & segment, element_t<ElementTag, WrappedConfigType> const & element )
    {
        typedef element_t<ElementTag, WrappedConfigType> element_type;
        return is_in_segment( segment, accessor::dense_container_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
    }
    
    
    
    
    
    template<typename segment_type, typename element_segment_mapping_type, typename container_tag>
    typename element_segment_mapping_type::element_segment_info_type * element_segment_info( segment_type const & segment, segment_info_t<element_segment_mapping_type, container_tag> & segment_info )
    {
        typedef typename segment_info_t<element_segment_mapping_type, container_tag>::element_segment_mapping_container_type element_segment_mapping_container_type;
        
        for (typename element_segment_mapping_container_type::iterator it = segment_info.element_segment_mapping_container.begin();
             it != segment_info.element_segment_mapping_container.end();
             ++it)
        {
            if (it->segment_id == segment.id())
                return &(it->element_segment_info);
        }
            
        return NULL;
    }
    
    template<typename segment_type, typename element_segment_mapping_type, typename container_tag>
    typename element_segment_mapping_type::element_segment_info_type const * element_segment_info( segment_type const & segment, segment_info_t<element_segment_mapping_type, container_tag> const & segment_info )
    {
        typedef typename segment_info_t<element_segment_mapping_type, container_tag>::element_segment_mapping_container_type element_segment_mapping_container_type;
        
        for (typename element_segment_mapping_container_type::iterator it = segment_info.element_segment_mapping_container.begin();
             it != segment_info.element_segment_mapping_container.end();
             ++it)
        {
            if (it->segment_id == segment.id())
                return &(it->element_segment_info);
        }
            
        return NULL;
    }
    
    template< typename segment_type, typename accessor_type, typename element_type >
    typename accessor_type::value_type::element_segment_info_type * element_segment_info( segment_type const & segment, accessor_type accessor, element_type const & element )
    {
        return element_segment_info( segment, accessor(element) );
    }
    
    template<typename segment_type, typename ElementTag, typename WrappedConfigType>
    typename viennagrid::storage::result_of::container_of<
        typename viennagrid::storage::result_of::value_type<
            typename segment_type::segmentation_type::appendix_type,
            element_segment_mapping_tag 
        >::type,
       element_t<ElementTag, WrappedConfigType>
    >::type::value_type::element_segment_info_type const *
        element_segment_info( segment_type const & segment, element_t<ElementTag, WrappedConfigType> const & element )
    {
        typedef element_t<ElementTag, WrappedConfigType> element_type;        
        return element_segment_info( segment, accessor::dense_container_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
    }
    
    template<typename segment_type, typename ElementTag, typename WrappedConfigType>
    typename viennagrid::storage::result_of::container_of<
        typename viennagrid::storage::result_of::value_type<
            typename segment_type::segmentation_type::appendix_type,
            element_segment_mapping_tag 
        >::type,
       element_t<ElementTag, WrappedConfigType>
    >::type::value_type::element_segment_info_type *
        element_segment_info( segment_type & segment, element_t<ElementTag, WrappedConfigType> const & element )
    {
        typedef element_t<ElementTag, WrappedConfigType> element_type;        
//         return element_segment_info( segment, accessor::dense_container_accessor<element_type>( segment.segmentation().appendix() ), element );
        return element_segment_info( segment, accessor::dense_container_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
    }
    
    
    
    
    
    
    template<typename segment_type, typename element_segment_mapping_type, typename container_tag>
    void add_to_segment( segment_type & segment,
                         segment_info_t<element_segment_mapping_type, container_tag> & segment_info
                       )
    {
        typedef typename segment_info_t<element_segment_mapping_type, container_tag>::element_segment_mapping_container_type element_segment_mapping_container_type;
        
        for (typename element_segment_mapping_container_type::const_iterator it = segment_info.element_segment_mapping_container.begin();
             it != segment_info.element_segment_mapping_container.end();
             ++it)
        {
            if (it->segment_id == segment.id()) return;
        }
        
        segment_info.element_segment_mapping_container.push_back( element_segment_mapping_type(segment.id()) );
    }
    
    
    template< typename segment_type, typename accessor_type, typename element_type >
    void add_to_segment( segment_type & segment, accessor_type accessor, element_type & element )
    {
        return add_to_segment( segment, accessor(element) );
    }
    
    
    template<typename segment_type, typename ElementTag, typename WrappedConfigType>
    void add_to_segment( segment_type & segment, element_t<ElementTag, WrappedConfigType> & element )
    {
        typedef element_t<ElementTag, WrappedConfigType> element_type;
        viennagrid::elements<element_type>( segment.view() ).insert_handle( viennagrid::handle( segment.segmentation().domain(), element ) );
        add_to_segment( segment, accessor::dense_container_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
    }
    
    
    
    
    
    template<typename segment_type, typename element_segment_mapping_type, typename container_tag>
    bool erase_from_segment( segment_type const & segment, segment_info_t<element_segment_mapping_type, container_tag> & segment_info )
    {
        typedef typename segment_info_t<element_segment_mapping_type, container_tag>::element_segment_mapping_container_type element_segment_mapping_container_type;
        
        for (typename element_segment_mapping_container_type::iterator it = segment_info.element_segment_mapping_container.begin();
             it != segment_info.element_segment_mapping_container.end();
             ++it)
        {
            if (it->segment_id == segment.id())
            {
                segment_info.element_segment_mapping_container.erase(it);
                return true;
            }
        }
            
        return false;
    }
    
    template< typename segment_type, typename accessor_type, typename element_type >
    bool erase_from_segment( segment_type const & segment, accessor_type accessor, element_type const & element )
    {
        return erase_from_segment( segment, accessor(element) );
    }
    
    template<typename segment_type, typename ElementTag, typename WrappedConfigType>
    bool erase_from_segment( segment_type & segment, element_t<ElementTag, WrappedConfigType> & element )
    {
        typedef element_t<ElementTag, WrappedConfigType> element_type;
        viennagrid::elements<element_type>( segment.view() ).erase_handle( viennagrid::handle( segment.segmentation().domain(), element ) );
        return erase_from_segment( segment, accessor::dense_container_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
    }
    
    
    
    
//     template<typename accessor_type, typename element_type>
//     typename accessor_type::value_type & segment_information( accessor_type & segmentation, element_type const & element )
//     {
//         viennagrid::storage::collection::get< typename element_type::tag >( segmentation.segment_metainfo ).resize( element.id().get() );
//         return viennagrid::storage::collection::get< typename element_type::tag >( segmentation.segment_metainfo )[ element.id().get() ];
//     }
    
    

    
    
//     template<>
//     struct element_info_segment_t
//     {
//         
//     };
//     
//     template<typename domain_type>
//     struct element_info_segmentation_t
//     {
//         
//         
//         domain_type & domain;
//     };
//     
//     
//     
//     
//     
//     
//     
//     
//     
//     
//     template<typename base_type = int, base_type invalid_id = -1>
//     struct segment_id_t
//     {
//     public:
//         segment_id_t() : id(invalid_id) {}
//         segment_id_t( base_type id_ ) : id(id_) {}
//         
//         bool is_valid() const { return id != invalid_id; }
//         operator base_type() const { return id; }
//         
//     private:
//         base_type id;
//     };
//     
//     
//     
//     struct disjunct_segmentation_tag;
//     struct overlapping_segmentation_tag;
//     
//     
//     template<typename element_tag, unsigned int geometric_dimension, typename segmentation_type_tag, typename segment_id_type>
//     struct element_segment_info_t;
// 
//     template<typename element_tag_, unsigned int geometric_dimension_, typename segment_id_type_>
//     struct element_segment_info_t<element_tag_, geometric_dimension_, disjunct_segmentation_tag, segment_id_type_>
//     {
//         typedef segment_id_type_ segment_id_type;
//         
//         element_segment_info_t() {}
//         element_segment_info_t( segment_id_type const & segment_id_ ) : segment_id(segment_id_) {}
//         
//         segment_id_type segment_id;
//         
//         bool in_segment( segment_id_type const & segment_id_ ) const
//         {
//             return (segment_id == segment_id_);
//         }
//         
//         operator segment_id_type() const { return segment_id; }
//         element_segment_info_t & operator=( segment_id_type const & segment_id_ ) { segment_id = segment_id_; return *this; }
//         
//         template<typename segmentation_type>
//         void add_segments( segmentation_type & segmentation ) const { segmentation.add_segment(segment_id); }
//     };    
//     
//     template<typename segment_id_type_>
//     struct element_segment_info_t<triangle_tag, 3, disjunct_segmentation_tag, segment_id_type_>
//     {
//         typedef segment_id_type_ segment_id_type;
//         
//         element_segment_info_t() {}
//         element_segment_info_t( segment_id_type const & positive_orientation_segment_id_,
//                                 segment_id_type const & negative_orientation_segment_id_) :
//                         positive_orientation_segment_id(positive_orientation_segment_id_), negative_orientation_segment_id(negative_orientation_segment_id_) {}
//         
//         segment_id_type positive_orientation_segment_id;
//         segment_id_type negative_orientation_segment_id;
//         
//         bool in_segment( segment_id_type const & segment_id ) const
//         {
//             return (segment_id == positive_orientation_segment_id) || (segment_id == negative_orientation_segment_id);
//         }
//         
//         bool faces_outward_on_segment( segment_id_type const & segment_id ) const
//         {
//             return (segment_id == positive_orientation_segment_id);
//         }
//         
//         template<typename segmentation_type>
//         void add_segments( segmentation_type & segmentation ) const
//         {
//             segmentation.add_segment(positive_orientation_segment_id);
//             segmentation.add_segment(negative_orientation_segment_id);
//         }
//     };
//     
//     
//     
//     template<typename element_tag, unsigned int geometric_dimension, typename segmentation_type_tag, typename segment_id_type>
//     bool in_segment( element_segment_info_t<element_tag, geometric_dimension, segmentation_type_tag, segment_id_type> const & segment_info, segment_id_type segment_id )
//     {
//         return segment_info.in_segment(segment_id);
//     }
//     
//     template<typename segmentation_type, typename element_type, typename base_type, base_type invalid_id>
//     bool in_segment( segmentation_type const & segmentation, element_type const & element, segment_id_t<base_type, invalid_id> segment_id )
//     {
//         return in_segment(segmentation.segment_info(element), segment_id);
//     }
//     
//     template<typename segment_id_type_>
//     bool faces_outward_on_segment( element_segment_info_t<triangle_tag, 3, disjunct_segmentation_tag, segment_id_type_> const & segment_info, segment_id_type_ segment_id )
//     {
//         return (segment_id == segment_info.positive_orientation_segment_id);
//     }
//     
//     template<typename segmentation_type, typename element_type, typename base_type, base_type invalid_id>
//     bool faces_outward_on_segment( segmentation_type const & segmentation, element_type const & element, segment_id_t<base_type, invalid_id> segment_id )
//     {
//         return faces_outward_on_segment(segmentation.segment_info(element), segment_id);
//     }
//     
//     
//     
//     template<typename segmentation_type, typename element_segment_info_type>
//     void add_segments( segmentation_type & segmentation, element_segment_info_type const & segment_element_info )
//     {
//         segment_element_info.add_segments(segmentation);
//     }
//     
//     
//     
//     namespace result_of
//     {
//         template<typename something>
//         struct segment_id_type
//         {
//             typedef typename something::segment_id_type type;
//         };
//     }
//     
//     
//     
//     template<typename element_type_, typename element_segment_info_container_type_>
//     class segmentation_t
//     {
//     public:
//         typedef element_type_ element_type;
//         
//         typedef element_segment_info_container_type_ element_segment_info_container_type;
//         typedef typename element_segment_info_container_type::value_type element_segment_info_type;
//         
//         typedef typename result_of::segment_id_type<element_segment_info_type>::type segment_id_type;
//         typedef typename std::set<segment_id_type> segment_ids_container_type;
//         
//         
//         template<typename domain_type>
//         void init( domain_type & domain )
//         {
//             segmentation.resize( viennagrid::id_upper_bound<element_type>(domain).get() );
// //             segmentation.resize( viennagrid::elements<element_type>( domain ).size()  );
//         }
//         
//         
//         element_segment_info_type const & segment_info( element_type const & element ) const
//         {
//             return viennagrid::look_up( segmentation, element );
//         }
//         
//         void set_segment_info( element_type const & element, element_segment_info_type const & segment_element_info )
//         {
//             viennagrid::look_up( segmentation, element ) = segment_element_info;
//             add_segments( *this, segment_element_info );
//         }
//         
//         // TODO reference counting on segments -> unused segments disapear
//         void add_segment( segment_id_type const & segment_id )
//         {
//             if (segment_id.is_valid())
//                 used_segments.insert(segment_id);
//         }
//         
//         segment_ids_container_type const & segments() const { return used_segments; }
//         
//         
//     private:
//         
//         element_segment_info_container_type segmentation;
//         segment_ids_container_type used_segments;
//     };
//     
//     
//     
//     
//     
//     
// 
//     template<typename segment_type, typename element_type>
//     bool is_in_segment( segment_type const & segment, element_type const & element );
//     
//     template<typename segment_type, typename element_type, typename segment_meta_info>
//     segment_meta_info get_segment_metainfo( segment_type const & segment, element_type const & element );
// 
//     template<typename segment_type, typename element_type, typename segment_meta_info>
//     void set_segment_metainfo( segment_type const & segment, element_type const & element, segment_meta_info const & metainfo );
//     
//     template<typename segment_type, typename element_type>
//     void add_element_to_segment( segment_type const & segment, element_type const & element );
//     
//     template<typename segment_type, typename element_type>
//     void erase_element_from_segment( segment_type const & segment, element_type const & element );
//     
//     namespace result_of
//     {
//         template<typename segment_type>
//         struct element_range<segment_type>;
//     }
//     
//     template<typename segment_type>
//     typename result_of::element_range<segment_type>:type elements( segment_type const & segment );
//     
//     
//     
//     
//     
//     
//     
//     
//     template<typename segment_id_type_ = segment_id_t<> >
//     class dummy_segmentation
//     {
//     public:
//         typedef segment_id_type_ segment_id_type;
//         
//         typedef segment_id_type element_segment_info_type;
//         typedef typename storage::static_array<segment_id_type, 1> segment_ids_container_type;
//         
//         dummy_segmentation( segment_id_type segment_id_ = segment_id_type(0) ) : segment_id(segment_id_)
//         {
//             used_segments[0] = segment_id;
//         }
//         
//         template<typename domain_type>
//         void init( domain_type & domain );
//         
//         template<typename element_type>
//         element_segment_info_type const segment_info( element_type const & element ) const
//         { return segment_id; }
//         
//         template<typename element_type>
//         void set_segment_info( element_type const & element, element_segment_info_type const & segment_element_info ) {}
//         
//         void add_segment( segment_id_type const & segment_id_ )
//         { assert( segment_id == segment_id_ ); }
//         
//         segment_ids_container_type const & segments() const { return used_segments; }
//         
//         
//     private:
//         const segment_id_type segment_id;
//         segment_ids_container_type used_segments;
//     };
//     
//     
//     template<typename base_type, base_type invalid_id>
//     bool in_segment( segment_id_t<base_type, invalid_id> const & segment_info, segment_id_t<base_type, invalid_id> segment_id )
//     {
//         return segment_info == segment_id;
//     }
//     
//     template<typename base_type, base_type invalid_id>
//     bool faces_outward_on_segment( segment_id_t<base_type, invalid_id> const & segment_info, segment_id_t<base_type, invalid_id> segment_id )
//     {
//         return segment_info == segment_id;
//     }
//     
//     
// 
//     namespace result_of
//     {
//         template<typename domain_type, typename element_type_or_tag, typename segmentation_type_tag = disjunct_segmentation_tag, typename segment_id_type = segment_id_t<> >
//         struct element_segment_info
//         {
//             typedef typename viennagrid::result_of::point_type<domain_type>::type point_type;
//             static const unsigned int geometric_dimension = viennagrid::result_of::geometric_dimension<domain_type>::value;
//             typedef typename viennagrid::result_of::element_tag<element_type_or_tag>::type element_tag;
//             
//             typedef element_segment_info_t<element_tag, geometric_dimension, segmentation_type_tag, segment_id_type> type;
//         };
//         
//         
//         template<typename domain_type, typename element_type_or_tag = typename viennagrid::result_of::cell_type<domain_type>::type,
//             typename container_tag = storage::std_deque_tag,
//             typename segmentation_type_tag = disjunct_segmentation_tag, typename segment_id_type = segment_id_t<> >
//         struct segmentation
//         {
//             typedef typename element_segment_info<domain_type, element_type_or_tag, segmentation_type_tag, segment_id_type>::type element_segment_info_type;
//             typedef typename storage::result_of::container< element_segment_info_type, container_tag >::type element_segment_info_container_type;
//             typedef typename viennagrid::result_of::element<domain_type, element_type_or_tag>::type element_type;
//             typedef segmentation_t<element_type, element_segment_info_container_type> type;
//         };
//     }
//     
//     
//     
//     
//     
//     template<typename domain_type, typename segmentation_type, typename view_container_type>
//     void split_in_views( domain_type & domain, segmentation_type const & segmentation, view_container_type & views )
//     {
//         typedef typename view_container_type::value_type view_type;
//         typedef typename viennagrid::result_of::cell_type<domain_type>::type cell_type;
//         typedef typename viennagrid::result_of::cell_range<domain_type>::type cell_range_type;
//         typedef typename viennagrid::result_of::iterator<cell_range_type>::type cell_range_iterator;
//         
//         typedef typename segmentation_type::segment_id_type segment_id_type;
//         typedef typename segmentation_type::segment_ids_container_type segment_ids_container_type;
//         segment_ids_container_type const & segments = segmentation.segments();
//         
//         views.resize( segments.size() );
//         unsigned int segment_index = 0;
//         for (typename segment_ids_container_type::iterator seg_it = segments.begin(); seg_it != segments.end(); ++seg_it, ++segment_index)
//             views[segment_index] = viennagrid::create_view<view_type>(domain);
// 
//         cell_range_type cells = viennagrid::elements(domain);
//         for (cell_range_iterator it = cells.begin(); it != cells.end(); ++it)
//         {
//             segment_index = 0;
//             for (typename segment_ids_container_type::iterator seg_it = segments.begin(); seg_it != segments.end(); ++seg_it, ++segment_index)
//             {
//                 segment_id_type segment_id = *seg_it;
//                 
//                 if ( viennagrid::in_segment( segmentation, *it, segment_id ) )
//                     viennagrid::add_handle( views[segment_index], domain, it.handle() );
//             }
//         }
//     }
//     
//     template<typename segment_container_type, typename seed_point_container>
//     void extract_seed_points( segment_container_type const & segments, seed_point_container & seed_points )
//     {
//         typedef typename segment_container_type::value_type segment_type;
//         typedef typename viennagrid::result_of::const_cell_range<segment_type>::type cell_range_type;
//         typedef typename viennagrid::result_of::iterator<cell_range_type>::type cell_range_iterator;
//         typedef typename viennagrid::result_of::point_type<segment_type>::type point_type;
//         
//         int segment_index = 0;
//         for (typename segment_container_type::const_iterator it = segments.begin(); it != segments.end(); ++it, ++segment_index)
//         {
//             cell_range_type cells = viennagrid::elements(*it);
//             point_type centroid = viennagrid::centroid( *it, cells[0] );
//             seed_points.push_back( std::make_pair(segment_index, centroid) );
//         }
//     }
//     
//     
//     
//     namespace segmentation
//     {
//         template<typename point_type>
//         bool triangle_ray_intersect(point_type const & r, point_type d, point_type const & A, point_type const & B, point_type const & C)
//         {
//             point_type b = B-A; // vector from A to B
//             point_type c = C-A; // vector from A to C
//             
//             point_type rhs = r-A;
//             
//             // solve the equation:
//             // r + lambda*d = A + beta*b + gamma*c
//             // beta*b + gamma*c - lambda*d = r-A
//             // (b c -d) (beta gamma lambda)^T = r-A
//             // (beta gamma lambda)^T = (b c -d)^-1 (r-A)
// 
//             double det = viennagrid::geometry::determinant( b, c, -d );
//             
// //             std::cout << "      det = " << det << std::endl;
//             
//             if ( std::abs(det) < 1e-6)
//             {
//                 
//                 point_type n = viennagrid::cross_prod( B-A, C-A );
//                 
//                 point_type center = (A+B+C)/3.0;
//                 if( std::abs( viennagrid::inner_prod( n, r-center ) ) < 1e-6 )
//                     return true;    // r lies on triangle plane, TODO behandeln: kreuzt strahl dreieck?
//                 else
//                     return false;
//             }
//             else
//             {
//                 point_type im[3]; // inverse matrix (b c -d)^-1
//                 
//                 d = -d; // invert for simplicity
//                 
//                 im[0][0] =   c[1]*d[2] - c[2]*d[1];
//                 im[1][0] = -(b[1]*d[2] - b[2]*d[1]);
//                 im[2][0] =   b[1]*c[2] - b[2]*c[1];
//                 
//                 im[0][1] = -(c[0]*d[2] - c[2]*d[0]);
//                 im[1][1] =   b[0]*d[2] - b[2]*d[0];
//                 im[2][1] = -(b[0]*c[2] - b[2]*c[0]);
//                 
//                 im[0][2] =   c[0]*d[1] - c[1]*d[0];
//                 im[1][2] = -(b[0]*d[1] - b[1]*d[0]);
//                 im[2][2] =   b[0]*c[1] - b[1]*c[0];
//                 
//                 double beta = viennagrid::inner_prod( rhs, im[0] );
//                 double gamma = viennagrid::inner_prod( rhs, im[1] );
//                 double lambda = viennagrid::inner_prod( rhs, im[2] );
//                 
//                 if (det < 0)
//                 {
//                     det = -det;
//                     beta = -beta;
//                     gamma = -gamma;
//                     lambda = -lambda;
//                 }
// 
//                 
//                 
//                 double alpha = det - beta - gamma;
//                 
// //                 std::cout << "      alpha = " << alpha << std::endl;
// //                 std::cout << "      beta = " << beta << std::endl;
// //                 std::cout << "      gamma = " << gamma << std::endl;
// //                 std::cout << "      lambda = " << lambda << std::endl;
//                 
//                 
// //                                 std::cout << "    Found intersection: " << alpha/det << " " << beta/det << " " << gamma/det << " " << lambda/det << std::endl;
//                 double offset = det * 1e-6;
//                 double lower = 0 - offset;
//                 double upper = det + offset;
//                                 
//                 if ( (alpha >= lower ) && (beta >= lower) && (gamma >= lower) && (alpha <= upper) && (beta <= upper) && (gamma <= upper) && (lambda >= lower) && (lambda <= upper))
//                 {       
// //                     std::cout << "  Triangle: " << A << " / " << B << " / " << C << std::endl;
// //                     std::cout << "    Found intersection: " << alpha/det << " " << beta/det << " " << gamma/det << " " << lambda/det << std::endl;
//                     return true;
//                 }
//                 else
//                     return false;
//             }
//             
//         }
//         
//         
//         
//         // adds an element and its neighbour elements to a segment
//         //   this method is called recursively on all valid neighbour elements
//         //   if a line is shared by more than 2 triangles, the triangle with the smallest inward angle is selected; this ensures a valid segment complex
//         //   the triangle with the smallest angle is calculated in this way:
//         //     for each boundary line set up a coordinate system in 2D where
//         //       the y-vector is the inverse normal of the current triangle
//         //       the x-vector is the cross product of the shared line vector and the normal of the current triangle
//         //     project the vector from line center to the neighbour triangle center onto this cooridanite system
//         //     calculate the oriented angle between the positive x-axis (=vector to the current triangle) and the projected vector to the neighbour triangle
//         //   the neighbour triangle with the smallest angle is selected
// 
//         template<typename domain_type, typename visited_state_container_type, typename segmentation_type, typename triangle_handle_type>
//         void mark_facing_shortes_angle( domain_type const & domain, visited_state_container_type & visited_state, segmentation_type & segmentation, unsigned int segment_id, triangle_handle_type triangle_handle, bool triangle_faces_outward )
//         {
//             typedef typename viennagrid::result_of::point_type<domain_type>::type point_type;
//             typedef typename viennagrid::result_of::coord_type<point_type>::type coord_type;
//             
//             typedef typename viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
//             typedef typename viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type triangle_type;
//             typedef typename viennagrid::result_of::const_handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
//             
//             typedef viennagrid::storage::static_array<vertex_handle_type, 3> handle_array;
//             
//             triangle_type const & triangle = viennagrid::dereference_handle(domain, triangle_handle);
//             
//             // if this triangle has already been visited -> nothing to do
//             if (viennagrid::look_up(visited_state, triangle))
// //             if (viennamesh::was_visited(triangle))
//                 return;
//             
//             // add this triangle to the segment
// //             viennamesh::add_face_to_segment( domain, triangle, segment_id, triangle_faces_outward );
//             {
//                 typedef typename segmentation_type::element_segment_info_type element_segment_info_type;
//                 element_segment_info_type current = segmentation.segment_info( triangle );
//                 if (triangle_faces_outward)
//                     current.positive_orientation_segment_id = segment_id;
//                 else
//                     current.negative_orientation_segment_id = segment_id;
//                 segmentation.set_segment_info( triangle, current );
//             }
//             
//             // ... and set the visited flag
//             viennagrid::look_up(visited_state, triangle) = true;
// //             viennamesh::set_visited(triangle);
//             
//             handle_array vtx;
//             vtx[0] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(0);
//             vtx[1] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(1);
//             vtx[2] = viennagrid::elements<viennagrid::vertex_tag>(triangle).handle_at(2);
//             
//             
//             viennagrid::storage::static_array<point_type,3> p;
//             p[0] = viennagrid::point( domain, vtx[0] );
//             p[1] = viennagrid::point( domain, vtx[1] );
//             p[2] = viennagrid::point( domain, vtx[2] );
//             
//             // calculate the center of the triangle
//             point_type center = (p[0]+p[1]+p[2])/3.0;
//             
//             // ... and its normal
//             point_type normal = viennagrid::cross_prod( p[1]-p[0], p[2]-p[0] );
//             // correcting the normal direction
//             if (!triangle_faces_outward) normal = -normal;
//             // normalizing the normal vector
//             normal /= viennagrid::norm_2(normal);
//             
//             
//         //     std::cout << " --- Triangle " << std::endl;
//         //     std::cout << "    adding to segment " << segment_id  << std::endl;
//         //     std::cout << "   " << p[0] << std::endl;
//         //     std::cout << "   " << p[1] << std::endl;
//         //     std::cout << "   " << p[2] << std::endl;
//         //     std::cout << "   normal = " << normal << std::endl;
//             
//             
//             
//             typedef typename viennagrid::result_of::const_element_range<triangle_type, viennagrid::line_tag>::type lines_on_triangle_range_type;
//             typedef typename viennagrid::result_of::handle_iterator<lines_on_triangle_range_type>::type lines_on_triangle_range_iterator;
//             
//             // iterating over all boundary lines
//             lines_on_triangle_range_type lines = viennagrid::elements<viennagrid::line_tag>( triangle );
//             for ( lines_on_triangle_range_iterator lit = lines.begin(); lit != lines.end(); ++lit )
//             {
//                 line_type const & line = viennagrid::dereference_handle( domain, *lit );
//                 
//                 viennagrid::storage::static_array<vertex_handle_type, 2> lvtx;
//                 lvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(line).handle_at(0);
//                 lvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(line).handle_at(1);
//                 
//                 viennagrid::storage::static_array<point_type, 2> lp;
//                 lp[0] = viennagrid::point( domain, lvtx[0] );
//                 lp[1] = viennagrid::point( domain, lvtx[1] );
//                 
//                 
//                 // calculating the line vector
//                 point_type line_vector = lp[1]-lp[0];
//                 // ... and normalizing it
//                 line_vector /= viennagrid::norm_2(line_vector);
//                 
//                 // calculating the center of the boundary line
//                 point_type line_center = (lp[0]+lp[1]) / 2.0;
//                 // calculate the vector facing towards the current triangle
//                 point_type line_to_triangle_vector = viennagrid::cross_prod( -normal, line_vector );
//                 // ... and normalizing it
//                 line_to_triangle_vector /= viennagrid::norm_2(line_to_triangle_vector);
//                 
//                 // check and correnct the orentietion of the vector facing towards the current triangle
//                 if (viennagrid::inner_prod(line_to_triangle_vector, center - line_center) < 0)
//                     line_to_triangle_vector = -line_to_triangle_vector;
//    
//                 
// //                 std::cout << "  line: " << lp[0] << " - " << lp[1] << std::endl;
//                 
//                 
//                 typedef typename viennagrid::result_of::const_coboundary_range<domain_type, viennagrid::triangle_tag>::type neighbour_range_type;
//                 typedef typename viennagrid::result_of::handle_iterator<neighbour_range_type>::type neighbour_handle_iterator;
//                 
//                 
//                 neighbour_range_type neighbour_triangles = viennagrid::create_coboundary_elements<viennagrid::triangle_tag>(domain, *lit);
//                 
// //                 if ( neighbour_triangles.size() != 2 )
// //                 {
//         //             std::cout << "    INFO - more than two triangle share the line" << std::endl;
//         //             std::cout << "ERROR there is something wrong... A line has not 2 coboundary triangles: " << neighbour_triangles.size() << std::endl;
//         //             std::cout << viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[0]) << " / " <<
//         //                         viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[1])  << " / " <<
//         //                         viennagrid::point(domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[2]) << std::endl;
//         //             return ;
// //                 }
//                 
//                 
//                 // smallest angle, triangle with smallest angle and facing outward flag of this triangle
//                 coord_type smallest_angle = std::numeric_limits<coord_type>::max();
//                 triangle_handle_type smallest_angle_triangle;
//                 bool smallest_angle_triangle_faces_outward;
//                 
//                 // iterating over all coboundary triangles of the current line
//                 for (neighbour_handle_iterator it = neighbour_triangles.handle_begin(); it != neighbour_triangles.handle_end(); ++it)
//                 {
//                     triangle_handle_type handle = *it;
//                     triangle_type const & neighbour_triangle = viennagrid::dereference_handle(domain, handle);
//                     
//         //             std::cout << "  neighbour " << neighbour_triangle << std::endl;
//                     
//                     // is the coboundary triangle the current triangle -> skipping
//                     if (handle == triangle_handle)
//                         continue;
//                     
//         //             if (viennamesh::was_visited(neighbour_triangle))
//         //                 continue;
//                     
//                     handle_array nvtx;
//                     nvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(0);
//                     nvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(1);
//                     nvtx[2] = viennagrid::elements<viennagrid::vertex_tag>(neighbour_triangle).handle_at(2);
//                     
//                     viennagrid::storage::static_array<point_type,3> np;
//                     np[0] = viennagrid::point( domain, nvtx[0] );
//                     np[1] = viennagrid::point( domain, nvtx[1] );
//                     np[2] = viennagrid::point( domain, nvtx[2] );
// 
//                     // calculating the center of the neighbour triangle
//                     point_type neighbour_center = (np[0]+np[1]+np[2])/3.0;
//                     // calculating the vector from the line center towards the neighbour triangle
//                     point_type line_to_neighbour_triangle_vector = neighbour_center - line_center;
//                     // ... and normalizing it
//                     line_to_neighbour_triangle_vector /= viennagrid::norm_2(line_to_neighbour_triangle_vector);
//                     
//                     // projecting the vector facing to the neighbour triangle onto the 2D coordinate system
//                     coord_type x = viennagrid::inner_prod( line_to_triangle_vector, line_to_neighbour_triangle_vector );
//                     coord_type y = viennagrid::inner_prod( -normal, line_to_neighbour_triangle_vector );
//                     
//                     // normalizing the 2D vector
//                     coord_type tmp = std::sqrt( x*x + y*y );
//                     x /= tmp;
//                     y /= tmp;
//                     
//                     
//                     
//                     // calculate the angle
//                     coord_type angle_center_vectors = std::acos( x );
//                     // calculate the corrected oriented angle; if y < 0 the angle has to be corrected
//                     coord_type oriented_angle = (y > 0) ? angle_center_vectors : 2.0*M_PI - angle_center_vectors;
//                     
//         //             std::cout << "   --- Neighbour Triangle " << std::endl;
//         //             std::cout << "     " << np[0] << std::endl;
//         //             std::cout << "     " << np[1] << std::endl;
//         //             std::cout << "     " << np[2] << std::endl;
//         //             std::cout << "     normal = " << neighbour_normal << std::endl;
//         // //             std::cout << "     line_vector = " << line_vector << std::endl;
//         //             std::cout << "     vector triangle = " << line_to_triangle_vector << std::endl;
//         //             std::cout << "     vector neighbour triangle = " << line_to_neighbour_triangle_vector << std::endl;
//         //             std::cout << "     angle orientation = " << angle_orientation << std::endl;
//         //             std::cout << "     angle center vectors = " << angle_center_vectors << std::endl;
//         //             std::cout << "     orientated angle = " << oriented_angle << std::endl;
//                     
//                     
//                     // if the corrent angle is smaller than the best -> yay! better choice found
//                     if (oriented_angle < smallest_angle)
//                     {
//                         smallest_angle = oriented_angle;
//                         smallest_angle_triangle = handle;
//                         
//                         typename handle_array::iterator vtx_it;
//                         typename handle_array::iterator nvtx_it;
//                         
//                         for (vtx_it = vtx.begin(); vtx_it != vtx.end(); ++vtx_it)
//                         {
//                             nvtx_it = std::find(nvtx.begin(), nvtx.end(), *vtx_it);
//                             if (nvtx_it != nvtx.end())
//                                 break;
//                         }
//                         
// 
//                         // topology check
//                         if ( ( *viennagrid::geometry::circular_next(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_next(nvtx_it, nvtx.begin(), nvtx.end()) ) ||
//                             ( *viennagrid::geometry::circular_prev(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_prev(nvtx_it, nvtx.begin(), nvtx.end()) ) )
//                         {
//                             smallest_angle_triangle_faces_outward = !triangle_faces_outward;
//                             
//                         }
//                         else if ( ( *viennagrid::geometry::circular_next(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_prev(nvtx_it, nvtx.begin(), nvtx.end()) ) ||
//                             ( *viennagrid::geometry::circular_prev(vtx_it, vtx.begin(), vtx.end()) == *viennagrid::geometry::circular_next(nvtx_it, nvtx.begin(), nvtx.end()) ) )
//                         {
//                             smallest_angle_triangle_faces_outward = triangle_faces_outward;
//                         }
//                         else
//                         {
//                             std::cout << "Something went wrong..." << std::endl;
//                         }
//                     }
//                 }
//                 
//                 // is a triangle found -> call mark_facing_shortes_angle recursively
//                 if (smallest_angle != std::numeric_limits<coord_type>::max())
//                 {
// //                     triangle_type & best = viennagrid::dereference_handle(domain, smallest_angle_triangle);
// //                     
// //                     handle_array nvtx;
// //                     nvtx[0] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(0);
// //                     nvtx[1] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(1);
// //                     nvtx[2] = viennagrid::elements<viennagrid::vertex_tag>(best).handle_at(2);
// //                     
// //                     viennagrid::storage::static_array<point_type,3> np;
// //                     np[0] = viennagrid::point( domain, nvtx[0] );
// //                     np[1] = viennagrid::point( domain, nvtx[1] );
// //                     np[2] = viennagrid::point( domain, nvtx[2] );
//                     
//         //             std::cout << "   Best neighbour triangle" << std::endl;
//         //             std::cout << "       " << np[0] << std::endl;
//         //             std::cout << "       " << np[1] << std::endl;
//         //             std::cout << "       " << np[2] << std::endl;
//         //             std::cout << "       faces outward = " << smallest_angle_triangle_faces_outward << std::endl;
//                     
//                     mark_facing_shortes_angle( domain, visited_state, segmentation, segment_id, smallest_angle_triangle, smallest_angle_triangle_faces_outward );
//                 }
//                 
//             }
//         }
//         
//         
//         
//         // this function detects and marks a face segment based on a seed point within it
//         //   is searches for all triangle which vector from its center towards the seed points does not intersect any other triangle
//         //   these triangles is for sure in the this segment
//         //   mark_facing_shortes_angle is called on those triangles to complete the segments
//         
//         template< typename domain_type, typename segmentation_type, typename vector_type >
//         void detect_and_mark_face_segment( domain_type const & domain, segmentation_type & segmentation_, unsigned int segment_index, vector_type seed_point )
//         {
//             typedef typename viennagrid::result_of::point_type<domain_type>::type point_type;
//             typedef typename viennagrid::result_of::coord_type<point_type>::type coord_type;
//             
//             typedef typename viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type triangle_type;
//             typedef typename viennagrid::result_of::handle<domain_type, viennagrid::triangle_tag>::type triangle_handle_type;
//             typedef typename viennagrid::result_of::const_element_range<domain_type, viennagrid::triangle_tag>::type triangle_range_type;
//             typedef typename viennagrid::result_of::handle_iterator<triangle_range_type>::type triangle_range_handle_iterator;
//             typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
//             
//             segmentation_.init( domain );
//             std::deque<bool> visited_state( viennagrid::id_upper_bound<triangle_type>(domain).get(), false );
//             
//             // iteratin over all triangles
//             triangle_range_type triangles = viennagrid::elements<viennagrid::triangle_tag>( domain );
//             for (triangle_range_handle_iterator it = triangles.handle_begin(); it != triangles.handle_end(); ++it)
//             {
//                 triangle_type const & triangle = viennagrid::dereference_handle( domain, *it );
//                 
//                 // has this triangle already been visited? -> skipping
//                 if (viennagrid::look_up(visited_state, triangle))
// //                 if (viennamesh::was_visited(triangle))
//                     continue;
//                 
//                 point_type const & p0 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[0] );
//                 point_type const & p1 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[1] );
//                 point_type const & p2 = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(triangle)[2] );
//                 
// //                 std::cout << std::endl << std::endl << std::endl;
// //                 std::cout << "Triangle: " << p0 << " / " << p1 << " / " << p2 << std::endl;
//                 
//                 // calculating the center of the triangle
//                 point_type r = (p0+p1+p2)/3.0;
//                 
//                 // calculating the normale vector of the triangle
//                 point_type n = viennagrid::cross_prod( p1-p0, p2-p0 );
//                 // ... and normalizing it
//                 n /= viennagrid::norm_2(n);
//                 
//                 // calculating the ray vector from the center of the triangle the the seed point
//                 point_type d = seed_point - r;
//                 
// //                 std::cout << " Seed point: " << seed_point << std::endl;
// //                 std::cout << " Triangle center: " << r << std::endl;
// //                 std::cout << " vector to seed point: " << d << std::endl;
//                 
//                 // projecting the normalized ray vector onto the normal vector
//                 coord_type p = viennagrid::inner_prod( d, n ) / viennagrid::norm_2(d);
//                 
//                 // if the projection if near zero (happens when the ray vector and the triangle are co-linear) -> skip this triangle
//                 if ( std::abs(p) < 1e-6 )
//                 {
//     //                 std::cout << " Line from triangle center to seed point is orthogonal to triangle normal -> skipping" << std::endl;
//                     continue;
//                 }
//                 
//                 // if the projection if negative, the triangle faces outward
//                 bool faces_outward = p < 0;
//                 
// //                 bool is_inside = false;
//                 // iterating over all triangles and check for intersection
//                 triangle_range_iterator jt = triangles.begin();
//                 for (; jt != triangles.end(); ++jt)
//                 {
//                     if (it == jt) continue; // no self intersect test
//                     
//                     triangle_type const & to_test = *jt;
//                     
//                     point_type const & A = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(to_test)[0] );
//                     point_type const & B = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(to_test)[1] );
//                     point_type const & C = viennagrid::point( domain, viennagrid::elements<viennagrid::vertex_tag>(to_test)[2] );
//                     
// //                     std::cout << "  Triangle intersection test: " << A << " / " << B << " / " << C << std::endl;
//                     
//                     if (segmentation::triangle_ray_intersect(r, d, A, B, C)) // TODO: scale by bounding box to ensure a ray outside the mesh
//                     {
// //                         std::cout << "    INTERSECTION!" << std::endl;
//                         break;
//                     }
//                     else
//                     {
// //                         std::cout << "    no intersection" << std::endl;
//                     }
//                 }
//                 
//                 // if there was no intersection -> mark this triangle and all neighbour triangles recursively
//                 if (jt == triangles.end())
//                 {
//                     if (!faces_outward) n = -n;
// //                     std::cout << " YAY! triangle has visible line to seed point " << seed_point << " " << n << std::endl;
//                     
//     //                 viennamesh::add_face_to_segment(triangle, segment_index, faces_outward);
//                     segmentation::mark_facing_shortes_angle( domain, visited_state, segmentation_, segment_index, *it, faces_outward );
//     //                 break;
//                 }
//             }        
//             
//             
// //             for (triangle_range_handle_iterator it = triangles.handle_begin(); it != triangles.handle_end(); ++it)
// //             {
// //                 viennamesh::clear_visited( viennagrid::dereference_handle( domain, *it ) );
// //             }
//         }
//         
//     }
//     
//     
//     
//     template< typename domain_type, typename segmentation_type, typename seed_point_iterator_type >
//     void mark_face_segments( domain_type const & domain, segmentation_type & segmentation_, seed_point_iterator_type seed_points_it, seed_point_iterator_type const & seed_points_end )
//     {
//         for ( ; seed_points_it != seed_points_end; ++seed_points_it)
//             segmentation::detect_and_mark_face_segment(domain, segmentation_, seed_points_it->first, seed_points_it->second);
//     }
    
}




#endif
