#ifndef VIENNAGRID_SEGMENTATION_HPP
#define VIENNAGRID_SEGMENTATION_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <limits>
#include "viennagrid/mesh/accessor.hpp"

#include "viennagrid/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element_deletion.hpp"


/** @file segmentation.hpp
    @brief Contains definition and implementation of segmentations and segments
*/


namespace viennagrid
{
  // doxygen doku in forwards.hpp
  template<typename WrappedConfigType>
  class segment_handle_t< segmentation_t<WrappedConfigType> >
  {
    friend class segmentation_t<WrappedConfigType>;

  public:

    typedef segmentation_t<WrappedConfigType> segmentation_type;
    typedef typename segmentation_type::segment_id_type segment_id_type;
    typedef typename segmentation_type::mesh_type mesh_type;
    typedef typename segmentation_type::view_type view_type;

  private:
    /** @brief Constructor, used by segmentation to create a segment */
    segment_handle_t( segmentation_type & segmentation_x,
                view_type & view_x,
                segment_id_type const & segment_id_) :
        segmentation_(&segmentation_x), view_(&view_x), segment_id(segment_id_) {}

  public:

    /** @brief Returns the ID of the segment
      *
      * @return   ID of the segment
      */
    segment_id_type const & id() const { return segment_id; }

    /** @brief Returns the segmentation in which the segment lives
      *
      * @return   A reference to the segmentation
      */
    segmentation_type & segmentation() { return *segmentation_; }
    /** @brief Returns the segmentation in which the segment lives, const version
      *
      * @return   A const reference to the segmentation
      */
    segmentation_type const & segmentation() const { return *segmentation_; }

    /** @brief Returns a view which represents the elements in the segment
      *
      * @return   A reference to the view
      */
    view_type & view() { return *view_; }
    /** @brief Returns a view which represents the elements in the segment, const version
      *
      * @return   A const reference to the view
      */
    view_type const & view() const { return *view_; }


    /** @brief operator< for ordering operations (e.g. used in std::map)
      *
      * @return   A const reference to the view
      */
    bool operator<( const segment_handle_t & rhs ) const { return segment_id < rhs.segment_id; }

  private:

    segmentation_type * segmentation_;
    view_type * view_;

    segment_id_type segment_id;
  };


  namespace result_of
  {
    template<typename SegmentationT>
    struct change_counter_type< segment_handle_t<SegmentationT> >
    {
      typedef typename change_counter_type< typename segment_handle_t<SegmentationT>::view_type >::type type;
    };

    // doxygen docu in mesh.hpp
    template<typename SegmentationType, typename element_type_or_tag>
    struct is_element_present< segment_handle_t<SegmentationType>, element_type_or_tag >
    {
      static const bool value = is_element_present< typename segment_handle_t<SegmentationType>::view_type, element_type_or_tag>::value;
    };

    template<typename SegmentationType, typename element_type_or_tag>
    struct is_element_present< const segment_handle_t<SegmentationType>, element_type_or_tag >
    {
      static const bool value = is_element_present<segment_handle_t<SegmentationType>, element_type_or_tag>::value;
    };


    // doxygen docu in forwards.hpp
    template<typename segmentation_type>
    struct element_collection< segment_handle_t<segmentation_type> >
    {
        typedef typename element_collection<typename segment_handle_t<segmentation_type>::view_type>::type type;
    };

    template<typename segmentation_type>
    struct element_collection< const segment_handle_t<segmentation_type> >
    {
        typedef typename element_collection<const typename segment_handle_t<segmentation_type>::view_type>::type type;
    };


    // doxygen docu in mesh.hpp
    template<typename segmentation_type>
    struct point< segment_handle_t<segmentation_type> >
    {
        typedef typename point<typename segment_handle_t<segmentation_type>::view_type>::type type;
    };


    // doxygen docu in mesh.hpp
    template<typename WrappedConfigT>
    struct segment_handle< segmentation_t<WrappedConfigT> >
    {
      typedef segment_handle_t< segmentation_t<WrappedConfigT> > type;
    };


    /** @brief Metafunction for obtaining the segment ID type from a segment or segmentation type
     *
     * @tparam SegmentationOrSegmentHandleT      The segmentation or segment type
     */
    template<typename SegmentationOrSegmentHandleT>
    struct segment_id;

    template<typename SegmentationType>
    struct segment_id< segment_handle_t<SegmentationType> >
    {
      typedef typename segment_handle_t<SegmentationType>::segment_id_type type;
    };

    template<typename WrappedConfigType>
    struct segment_id< segmentation_t<WrappedConfigType> >
    {
      typedef typename segmentation_t<WrappedConfigType>::segment_id_type type;
    };


    /** @brief Metafunction for obtaining a mesh view from a mesh. Elements can be directly given. If viennagrid::null_type is specified somewhere all following types will be ignored. The default type for all element types is viennagrid::null_type.
     *
     * @tparam MeshT              The host mesh type
     * @tparam Element0TypeOrTagT   The first element type which is present in the mesh view
     * @tparam Element1TypeOrTagT   The second element type which is present in the mesh view
     * @tparam Element2TypeOrTagT   The third element type which is present in the mesh view
     * @tparam Element3TypeOrTagT   The forth element type which is present in the mesh view
     * @tparam Element4TypeOrTagT   The fifth element type which is present in the mesh view
     * @tparam Element5TypeOrTagT   The sixth element type which is present in the mesh view
     * @tparam Element6TypeOrTagT   The seventh element type which is present in the mesh view
     * @tparam Element7TypeOrTagT   The eighth element type which is present in the mesh view
     * @tparam Element8TypeOrTagT   The nineth element type which is present in the mesh view
     * @tparam Element9TypeOrTagT   The tenth element type which is present in the mesh view
     */
    template<typename SegmentationT,
             typename Element0TypeOrTagT, typename Element1TypeOrTagT,
             typename Element2TypeOrTagT, typename Element3TypeOrTagT,
             typename Element4TypeOrTagT, typename Element5TypeOrTagT,
             typename Element6TypeOrTagT, typename Element7TypeOrTagT,
             typename Element8TypeOrTagT, typename Element9TypeOrTagT>
    struct mesh_view< segment_handle_t<SegmentationT>,
                      Element0TypeOrTagT, Element1TypeOrTagT, Element2TypeOrTagT, Element3TypeOrTagT, Element4TypeOrTagT,
                      Element5TypeOrTagT, Element6TypeOrTagT, Element7TypeOrTagT, Element8TypeOrTagT, Element9TypeOrTagT>
    {
      typedef typename segment_handle_t<SegmentationT>::view_type ViewType;

        typedef typename mesh_view<
            ViewType,
            Element0TypeOrTagT, Element1TypeOrTagT, Element2TypeOrTagT, Element3TypeOrTagT, Element4TypeOrTagT,
            Element5TypeOrTagT, Element6TypeOrTagT, Element7TypeOrTagT, Element8TypeOrTagT, Element9TypeOrTagT
        >::type type;
    };
  }



  /** @brief For internal use only */
  template<typename SegmentationType>
  bool is_obsolete( segment_handle_t<SegmentationType> const & segment, typename segment_handle_t<SegmentationType>::view_type::change_counter_type change_counter_to_check )
  { return is_obsolete(segment.view(), change_counter_to_check); }

  /** @brief For internal use only */
  template<typename SegmentationType>
  void update_change_counter( segment_handle_t<SegmentationType> & segment, typename segment_handle_t<SegmentationType>::view_type::change_counter_type & change_counter_to_update )
  { update_change_counter(segment.view(), change_counter_to_update); }

  /** @brief For internal use only */
  template<typename SegmentationType>
  void increment_change_counter( segment_handle_t<SegmentationType> & segment )
  { increment_change_counter(segment.view()); }



  /** @brief For internal use only */
  template<typename SegmentationType>
  typename segment_handle_t<SegmentationType>::view_type::element_collection_type & element_collection( segment_handle_t<SegmentationType> & segment)
  { return element_collection( segment.view() ); }

  /** @brief For internal use only */
  template<typename SegmentationType>
  typename segment_handle_t<SegmentationType>::view_type::element_collection_type const & element_collection( segment_handle_t<SegmentationType> const & segment)
  { return element_collection( segment.view() ); }


  /** @brief For internal use only */
  template<typename SegmentationType>
  typename segment_handle_t<SegmentationType>::view_type::inserter_type & inserter(segment_handle_t<SegmentationType> & segment)
  { return inserter( segment.view() ); }

  /** @brief For internal use only */
  template<typename SegmentationType>
  typename segment_handle_t<SegmentationType>::view_type::inserter_type const & inserter(segment_handle_t<SegmentationType> const & segment)
  { return inserter( segment.view() ); }


  /** @brief For internal use only */
  template<typename SegmentationType>
  typename segment_handle_t<SegmentationType>::view_type::inserter_type::id_generator_type & id_generator(segment_handle_t<SegmentationType> & segment)
  { return id_generator( segment.view() ); }

  /** @brief For internal use only */
  template<typename SegmentationType>
  typename segment_handle_t<SegmentationType>::view_type::inserter_type::id_generator_type const & id_generator(segment_handle_t<SegmentationType> const & segment)
  { return id_generator( segment.view() ); }


  // doxygen docu in forwards.hpp
  template<typename element_type_or_tag, typename segmentation_type>
  typename result_of::element_range<segment_handle_t<segmentation_type>, element_type_or_tag>::type elements( segment_handle_t<segmentation_type> & segment)
  { return elements<element_type_or_tag>( segment.view() ); }

  // doxygen docu in forwards.hpp
  template<typename element_type_or_tag, typename segmentation_type>
  typename result_of::const_element_range<segment_handle_t<segmentation_type>, element_type_or_tag>::type elements(segment_handle_t<segmentation_type> const & segment)
  { return elements<element_type_or_tag>( segment.view() ); }


  // doxygen docu in mesh.hpp
  template<typename ElementTypeOrTag, typename SegmentationType>
  typename viennagrid::result_of::id<
    typename viennagrid::result_of::element< segment_handle_t<SegmentationType>, ElementTypeOrTag>::type
  >::type id_upper_bound( segment_handle_t<SegmentationType> const & segment )
  { return id_upper_bound<ElementTypeOrTag>( segment.view() ); }



  /** @brief Creates a view out of a mesh using the mesh_proxy object
    *
    * @tparam MeshOrSegmentHandleT    The mesh or segment type from which the mesh view is created
    * @param  mesh              The mesh or segment object from which the mesh view is created
    * @return                     a mesh_proxy object holding the host mesh/segment object, can be assigned to a mesh_t object
    */
  template<typename SegmentationT>
  mesh_proxy< typename SegmentationT::view_type > make_view(segment_handle_t<SegmentationT> & segment)
  {
      return mesh_proxy<typename SegmentationT::view_type>( segment.view() );
  }



  /** @brief Function for obtaining the point from a vertex
    *
    * @tparam SegmentationType    Segmentation type of the segment type
    * @param  segment             The host segment object
    * @param  vertex              The vertex of which the point is obtained
    * @return                     A reference to the point
    */
  template<typename SegmentationType>
  typename result_of::point< segment_handle_t<SegmentationType> >::type & point(segment_handle_t<SegmentationType> & segment, typename result_of::vertex< segment_handle_t<SegmentationType> >::type & vertex)
  { return point( segment.view(), vertex ); }

  /** @brief Function for obtaining the point from a vertex, const version
    *
    * @tparam SegmentationType    Segmentation type of the segment type
    * @param  segment             The host segment object
    * @param  vertex              The vertex of which the point is obtained
    * @return                     A const reference to the point
    */
  template<typename SegmentationType>
  typename result_of::point< segment_handle_t<SegmentationType> >::type const & point( segment_handle_t<SegmentationType> const & segment, typename result_of::vertex< segment_handle_t<SegmentationType> >::type const & vertex)
  { return point( segment.view(), vertex ); }

  /** @brief Function for obtaining the point from a vertex
    *
    * @tparam SegmentationType    Segmentation type of the segment type
    * @param  segment             The host segment object
    * @param  vertex_handle       A handle to the vertex of which the point is obtained
    * @return                     A reference to the point
    */
  template<typename SegmentationType>
  typename result_of::point< segment_handle_t<SegmentationType> >::type & point(segment_handle_t<SegmentationType> & segment, typename result_of::vertex_handle< segment_handle_t<SegmentationType> >::type vertex)
  { return point( segment.view(), vertex ); }

  /** @brief Function for obtaining the point from a vertex, const version
    *
    * @tparam SegmentationType    Segmentation type of the segment type
    * @param  segment             The host segment object
    * @param  vertex_handle       A handle to the vertex of which the point is obtained
    * @return                     A const reference to the point
    */
  template<typename SegmentationType>
  typename result_of::point< segment_handle_t<SegmentationType> >::type const & point( segment_handle_t<SegmentationType> const & segment, typename result_of::const_vertex_handle< segment_handle_t<SegmentationType> >::type vertex)
  { return point( segment.view(), vertex ); }








  namespace result_of
  {
    /** @brief For internal use only */
    template<typename WrappedConfigType>
    struct segmentation_mesh_type
    {
      typedef typename WrappedConfigType::mesh_type type;
    };

    /** @brief For internal use only */
    template<typename WrappedConfigType>
    struct segmentation_view_type
    {
      typedef typename WrappedConfigType::view_type type;
    };

    /** @brief For internal use only */
    template<typename WrappedConfigType>
    struct segmentation_segment_id_type
    {
      typedef typename WrappedConfigType::segment_id_type type;
    };

    /** @brief For internal use only */
    template<typename WrappedConfigType>
    struct segmentation_appendix_type
    {
      typedef typename WrappedConfigType::appendix_type type;
    };

    /** @brief For internal use only */
    template<typename WrappedConfigType>
    struct segmentation_view_container_tag
    {
      typedef typename WrappedConfigType::view_container_tag type;
    };

  }



  namespace config
  {
    /** @brief For internal use only */
    template<typename mesh_type_, typename view_type_, typename segment_id_type_, typename appendix_type_, typename view_container_tag_>
    struct segmentation_config_wrapper_t
    {
      typedef mesh_type_ mesh_type;
      typedef view_type_ view_type;
      typedef segment_id_type_ segment_id_type;
      typedef appendix_type_ appendix_type;
      typedef view_container_tag_ view_container_tag;
    };
  }


  template<typename WrappedConfigType>
  class segmentation_t
  {
  public:

    /** @brief The mesh type to which the segmentation references */
    typedef typename result_of::segmentation_mesh_type<WrappedConfigType>::type mesh_type;
    /** @brief The view type in a segment */
    typedef typename result_of::segmentation_view_type<WrappedConfigType>::type view_type;
    /** @brief The segment ID type */
    typedef typename result_of::segmentation_segment_id_type<WrappedConfigType>::type segment_id_type;
    /** @brief For internal use only */
    typedef typename result_of::segmentation_appendix_type<WrappedConfigType>::type appendix_type;
    /** @brief For internal use only */
    typedef typename result_of::segmentation_view_container_tag<WrappedConfigType>::type view_container_tag;

    /** @brief For internal use only */
    typedef segmentation_t<WrappedConfigType> self_type;

    /** @brief The segment type of the segmentation */
    typedef segment_handle_t<self_type> segment_handle_type;

    /** @brief For internal use only */
    typedef typename viennagrid::storage::result_of::container< view_type, view_container_tag >::type view_container_type;
    /** @brief For internal use only */
    typedef std::map<segment_id_type, segment_handle_type> segment_id_map_type;


    /** @brief Constructor
      *
      * @param mesh_x   The mesh object to which the segment references
      */
    segmentation_t( mesh_type & mesh_x ) : highest_id(-1), mesh_(&mesh_x) { all_elements_ = viennagrid::make_view(*mesh_); }


    /** @brief Returns the mesh to which the segmentation is associated
      *
      * @return   A reference to the mesh
      */
    mesh_type & mesh() { return *mesh_; }
    /** @brief Returns the mesh to which the segmentation is associated, const version
      *
      * @return   A const reference to the mesh
      */
    mesh_type const & mesh() const { return *mesh_; }

    /** @brief Returns the appenix of the segmentation, for internal use only
      *
      * @return   A reference to the appendix
      */
    appendix_type & appendix() { return appendix_; }
    /** @brief Returns the appenix of the segmentation, const version, for internal use only
      *
      * @return   A const reference to the appendix
      */
    appendix_type const & appendix() const { return appendix_; }


    /** @brief Queries if a segment with given ID is present within the segmentation
      *
      * @param segment_id   The ID of the segment to search
      * @return             True if the segment is present, false otherwise
      */
    bool segment_present( segment_id_type const & segment_id ) const { return segment_id_map.find(segment_id) != segment_id_map.end(); }


    /** @brief Returns the segment with the given ID, will fail if no segment with segment_id is present
      *
      * @param segment_id   The ID of the segment to search
      * @return             A const reference to the segment with the given ID
      */
    segment_handle_type const & get_segment( segment_id_type const & segment_id ) const
    {
        typename segment_id_map_type::const_iterator it = segment_id_map.find(segment_id);
        assert( it != segment_id_map.end() );
        return it->second; // segment already is present
    }

    /** @brief Returns the segment with the given ID, will create a new segment if no segment with the given ID is present
      *
      * @param segment_id   The ID of the segment to search
      * @return             A reference to the segment with the given ID
      */
    segment_handle_type & get_make_segment( segment_id_type const & segment_id )
    {
        typename segment_id_map_type::iterator it = segment_id_map.find(segment_id);

        if (it != segment_id_map.end())
            return it->second; // segment already is present

        view_segments.push_back( viennagrid::make_view( mesh() ) );

        segment_handle_type segment( *this, view_segments.back(), segment_id );

        if ( highest_id < segment_id )
            highest_id = segment_id;

        return (segment_id_map.insert( std::make_pair(segment_id, segment) ).first)->second;
    }

    /** @brief Creates a new segment with an automatic assigned ID
      *
      * @return             A reference to newly created segment
      */
    segment_handle_type & make_segment() { return get_make_segment( ++highest_id ); }

    /** @brief Calls get_make_segment with segment_id
      *
      * @param segment_id   The ID of the segment to search
      * @return             A reference to newly created segment
      */
    segment_handle_type & operator()( segment_id_type const & segment_id ) { return get_make_segment(segment_id); }
    /** @brief Calls get_make_segment with segment_id
      *
      * @param segment_id   The ID of the segment to search
      * @return             A reference to newly created segment
      */
    segment_handle_type & operator[]( segment_id_type const & segment_id ) { return get_make_segment(segment_id); }

    /** @brief Calls get_segment with segment_id
      *
      * @param segment_id   The ID of the segment to search
      * @return             A const reference to newly created segment
      */
    segment_handle_type const & operator()( segment_id_type const & segment_id ) const { return get_segment(segment_id); }
    /** @brief Calls get_segment with segment_id
      *
      * @param segment_id   The ID of the segment to search
      * @return             A const reference to newly created segment
      */
    segment_handle_type const & operator[]( segment_id_type const & segment_id ) const { return get_segment(segment_id); }

    /** @brief Returns the heighest segment ID
      *
      * @return             The highest segment ID
      */
    segment_id_type id_upper_bound() const { return highest_id; }


    /** @brief Iterator for iterating over all segments */
    class iterator : public segment_id_map_type::iterator
    {
      typedef typename segment_id_map_type::iterator base;
    public:
      iterator() {}
      iterator(base const & foo) : base(foo) {}

      typedef segment_handle_type           value_type;
      typedef segment_handle_type &         reference;
      typedef segment_handle_type const &   const_reference;
      typedef segment_handle_type *         pointer;
      typedef segment_handle_type const *   const_pointer;

      reference       operator* ()       { return base::operator*().second; }
      const_reference operator* () const { return base::operator*().second; }

      pointer       operator->()       { return &(operator* ()); }
      const_pointer operator->() const { return &(operator* ()); }
    };

    /** @brief Const iterator for iterating over all segments */
    class const_iterator : public segment_id_map_type::const_iterator
    {
      typedef typename segment_id_map_type::const_iterator base;
    public:
      const_iterator() {}
      const_iterator(base const & foo) : base(foo) {}
      const_iterator(iterator const & it) : base(it) {}

      typedef segment_handle_type value_type;
      typedef segment_handle_type const & reference;
      typedef segment_handle_type const * pointer;

      reference operator* () const { return base::operator*().second; }

      pointer operator->() const { return &(operator* ()); }
    };

    /** @brief Returns an iterator pointing to the first segment
      *
      * @return    An iterator pointing to the first segment
      */
    iterator begin() { return iterator(segment_id_map.begin()); }
    /** @brief Returns an iterator pointing to the end of the segments
      *
      * @return    An iterator pointing to the end of the segments
      */
    iterator end() { return iterator(segment_id_map.end()); }

    /** @brief Returns a const iterator pointing to the first segment
      *
      * @return    A const iterator pointing to the first segment
      */
    const_iterator cbegin() const { return const_iterator(segment_id_map.begin()); }
    /** @brief Returns a const iterator pointing to the end of the segments
      *
      * @return    A const iterator pointing to the end of the segments
      */
    const_iterator cend() const { return const_iterator(segment_id_map.end()); }

    /** @brief Returns a const iterator pointing to the first segment, const version
      *
      * @return    A const iterator pointing to the first segment
      */
    const_iterator begin() const { return cbegin(); }
    /** @brief Returns a const iterator pointing to the end of the segments, const version
      *
      * @return    A const iterator pointing to the end of the segments
      */
    const_iterator end() const { return cend(); }

    /** @brief Returns the number of segments in the segmentation
      *
      * @return    The number of segments
      */
    std::size_t size() const { return segment_id_map.size(); }
    /** @brief Queries if the segmentation is empty
      *
      * @return    True if there are no segments, false otherwise
      */
    bool empty() const { return segment_id_map.empty(); }

    /** @brief Finds a segment with a given segment ID
      *
      * @param    segment_id    The segment ID of the segment to find
      * @return                 An iterator pointing segment with the given ID, if no segment was found it points to end()
      */
    iterator find( segment_id_type const & segment_id ) { return iterator( segment_id_map.find(segment_id) ); }
    /** @brief Finds a segment with a given segment ID, const version
      *
      * @param    segment_id    The segment ID of the segment to find
      * @return                 A const iterator pointing segment with the given ID, if no segment was found it points to end()
      */
    const_iterator find( segment_id_type const & segment_id ) const { return const_iterator( segment_id_map.find(segment_id) ); }

    /** @brief Completely clears a segmentations
      *
      */
    void clear()
    {
      highest_id = -1;
      segment_id_map.clear();
      view_segments.clear();
      appendix_ = appendix_type();
    }


    view_type & all_elements() { return all_elements_; }
    view_type const & all_elements() const { return all_elements_; }

  private:

    view_type all_elements_;

    segment_id_type highest_id;
    segment_id_map_type segment_id_map;

    view_container_type view_segments;

    appendix_type appendix_;

    mesh_type * mesh_;
  };




  /** @brief For internal use only */
  template<typename WrappedConfigT>
  typename segmentation_t<WrappedConfigT>::view_type::element_collection_type &
  element_collection( segmentation_t<WrappedConfigT> & segmentation)
  { return element_collection( segmentation.all_elements() ); }

  /** @brief For internal use only */
  template<typename WrappedConfigT>
  typename segmentation_t<WrappedConfigT>::view_type::element_collection_type const &
  element_collection( segmentation_t<WrappedConfigT> const & segmentation)
  { return element_collection( segmentation.all_elements() ); }


  template<typename ElementTypeOrTagT, typename WrappedConfigT>
  typename result_of::element_range<segmentation_t<WrappedConfigT>, ElementTypeOrTagT>::type
  elements( segmentation_t<WrappedConfigT> & segmentation)
  { return elements<ElementTypeOrTagT>( segmentation.all_elements() ); }

  // doxygen docu in forwards.hpp
  template<typename ElementTypeOrTagT, typename WrappedConfigT>
  typename result_of::const_element_range<segmentation_t<WrappedConfigT>, ElementTypeOrTagT>::type
  elements(segmentation_t<WrappedConfigT> const & segmentation)
  { return elements<ElementTypeOrTagT>( segmentation.all_elements() ); }




  namespace result_of
  {
    // doxygen docu in mesh.hpp
    template<typename WrappedConfigType, typename element_type_or_tag>
    struct is_element_present< segmentation_t<WrappedConfigType>, element_type_or_tag >
    {
      static const bool value = is_element_present< typename segmentation_t<WrappedConfigType>::mesh_type, element_type_or_tag>::value;
    };

    template<typename WrappedConfigType, typename element_type_or_tag>
    struct is_element_present< const segmentation_t<WrappedConfigType>, element_type_or_tag >
    {
      static const bool value = is_element_present<segmentation_t<WrappedConfigType>, element_type_or_tag>::value;
    };


    // doxygen docu in forwards.hpp
    template<typename segmentation_type>
    struct element_collection< segmentation_t<segmentation_type> >
    {
      typedef typename element_collection<typename segmentation_t<segmentation_type>::view_type>::type type;
    };

    template<typename segmentation_type>
    struct element_collection< const segmentation_t<segmentation_type> >
    {
      typedef typename element_collection<const typename segmentation_t<segmentation_type>::view_type>::type type;
    };


    // doxygen docu in mesh.hpp
    template<typename segmentation_type>
    struct point< segmentation_t<segmentation_type> >
    {
      typedef typename point<typename segmentation_t<segmentation_type>::mesh_type>::type type;
    };



    // doxygen docu in forwards.hpp
    template<typename segmentation_type, typename element_type_or_tag>
    struct element< segmentation_t<segmentation_type>, element_type_or_tag >
    {
      typedef typename element<typename segmentation_t<segmentation_type>::view_type, element_type_or_tag>::type type;
    };

    // doxygen docu in forwards.hpp
    template<typename segmentation_type, typename element_type_or_tag>
    struct handle< segmentation_t<segmentation_type>, element_type_or_tag >
    {
      typedef typename handle<typename segmentation_t<segmentation_type>::view_type, element_type_or_tag>::type type;
    };

    // doxygen docu in forwards.hpp
    template<typename segmentation_type, typename element_type_or_tag>
    struct const_handle< segmentation_t<segmentation_type>, element_type_or_tag >
    {
      typedef typename const_handle<typename segmentation_t<segmentation_type>::view_type, element_type_or_tag>::type type;
    };


    // doxygen docu in forwards.hpp
    template<typename segmentation_type, typename element_type_or_tag>
    struct element_range< segmentation_t<segmentation_type>, element_type_or_tag >
    {
      typedef typename element_range<typename segmentation_t<segmentation_type>::view_type, element_type_or_tag>::type type;
    };

    // doxygen docu in forwards.hpp
    template<typename segmentation_type, typename element_type_or_tag>
    struct const_element_range< segmentation_t<segmentation_type>, element_type_or_tag >
    {
      typedef typename const_element_range<typename segmentation_t<segmentation_type>::view_type, element_type_or_tag>::type type;
    };

    // doxygen docu in forwards.hpp
    template<typename segmentation_type>
    struct cell_tag< segmentation_t<segmentation_type> >
    {
      typedef typename cell_tag< typename segmentation_t<segmentation_type>::view_type >::type type;
    };
  }




  /** @brief For internal use only */
  template<typename segment_id_type_, typename segment_element_info_type_ = viennagrid::null_type>
  struct element_segment_mapping
  {
    typedef segment_id_type_ segment_id_type;
    typedef segment_element_info_type_ segment_element_info_type;

    element_segment_mapping() {}
    element_segment_mapping( segment_id_type const & segment_id_ ) : segment_id(segment_id_) {}

    segment_id_type segment_id;
    segment_element_info_type segment_element_info;
  };



  template<typename element_segment_mapping_type, typename container_tag = viennagrid::storage::std_deque_tag>
  struct segment_info_t
  {
    typedef typename element_segment_mapping_type::segment_id_type segment_id_type;
    typedef typename element_segment_mapping_type::segment_element_info_type segment_element_info_type;

    typedef typename viennagrid::storage::result_of::container<element_segment_mapping_type, container_tag>::type element_segment_mapping_container_type;


    class const_iterator : public element_segment_mapping_container_type::const_iterator
    {
      typedef typename element_segment_mapping_container_type::const_iterator base;
    public:
      const_iterator() {}
      const_iterator(base const & foo) : base(foo) {}

      typedef segment_id_type           value_type;
      typedef segment_id_type &         reference;
      typedef segment_id_type const &   const_reference;
      typedef segment_id_type *         pointer;
      typedef segment_id_type const *   const_pointer;

      const_reference operator* () const { return base::operator*().segment_id; }
      const_pointer operator->() const { return &(operator* ()); }
    };

    const_iterator cbegin() const { return const_iterator(element_segment_mapping_container.begin()); }
    const_iterator cend()   const { return const_iterator(element_segment_mapping_container.end()); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end()   const { return cend(); }

    bool empty() const { return element_segment_mapping_container.empty(); }


    element_segment_mapping_container_type element_segment_mapping_container;
  };


  /** @brief For internal use only */
  struct element_segment_mapping_tag;

  template<typename SegmentationT>
  typename viennagrid::storage::result_of::value_type<
          typename SegmentationT::appendix_type,
          element_segment_mapping_tag
      >::type & element_segment_mapping_collection( SegmentationT & segmentation )
  {
    return viennagrid::storage::collection::get<element_segment_mapping_tag>( segmentation.appendix() );
  }

  template<typename SegmentationT>
  typename viennagrid::storage::result_of::value_type<
          typename SegmentationT::appendix_type,
          element_segment_mapping_tag
      >::type const & element_segment_mapping_collection( SegmentationT const & segmentation )
  {
    return viennagrid::storage::collection::get<element_segment_mapping_tag>( segmentation.appendix() );
  }


  template<typename segment_handle_type>
  typename viennagrid::storage::result_of::value_type<
          typename segment_handle_type::segmentation_type::appendix_type,
          element_segment_mapping_tag
      >::type & element_segment_mapping_collection( segment_handle_type & segment )
  {
    return element_segment_mapping_collection( segment.segmentation() );
  }

  template<typename segment_handle_type>
  typename viennagrid::storage::result_of::value_type<
          typename segment_handle_type::segmentation_type::appendix_type,
          element_segment_mapping_tag
      >::type const & element_segment_mapping_collection( segment_handle_type const & segment )
  {
    return element_segment_mapping_collection( segment.segmentation() );
  }




  /** @brief For internal use only */
  template<typename container_type_, typename ChangeCounterType>
  struct segment_interface_information_wrapper
  {
    typedef container_type_ container_type;
    typedef ChangeCounterType change_counter_type;

    segment_interface_information_wrapper() : seg0_change_counter(0), seg1_change_counter(0) {}

    change_counter_type seg0_change_counter;
    change_counter_type seg1_change_counter;

    container_type container;
  };


  /** @brief For internal use only */
  template<typename segment_id_type, typename container_type_, typename ChangeCounterType>
  struct interface_information_wrapper
  {
    typedef container_type_ container_type;
    typedef segment_interface_information_wrapper<container_type, ChangeCounterType> segment_interface_information_wrapper_type;
    typedef std::pair<segment_id_type, segment_id_type> key_type;
    typedef std::map< key_type, segment_interface_information_wrapper_type > map_type;

    interface_information_wrapper() {}

    template<typename segment_handle_type>
    segment_interface_information_wrapper_type & get_interface_wrapper_impl( segment_handle_type const & seg0, segment_handle_type const & seg1 ) const
    {
      assert( seg0.id() != seg1.id() );

      key_type key( std::min(seg0.id(), seg1.id()), std::max(seg0.id(), seg1.id()) );

      return interface_flags[ key ];
    }

    template<typename segment_handle_type>
    segment_interface_information_wrapper_type & get_interface_wrapper( segment_handle_type const & seg0, segment_handle_type const & seg1 )
    { return get_interface_wrapper_impl(seg0, seg1); }

    template<typename segment_handle_type>
    segment_interface_information_wrapper_type const & get_interface_wrapper( segment_handle_type const & seg0, segment_handle_type const & seg1 ) const
    { return get_interface_wrapper_impl(seg0, seg1); }

    mutable map_type interface_flags;
  };



  template<typename element_tag, typename segment_handle_type>
  typename viennagrid::storage::result_of::value_type<
      typename viennagrid::storage::result_of::value_type<
          typename segment_handle_type::segmentation_type::appendix_type,
          interface_information_collection_tag
      >::type,
      element_tag
  >::type::segment_interface_information_wrapper_type &
  interface_information_collection( segment_handle_type & seg0, segment_handle_type & seg1 )
  { return viennagrid::storage::collection::get<element_tag>( viennagrid::storage::collection::get<interface_information_collection_tag>( seg0.segmentation().appendix() ) ).get_interface_wrapper(seg0, seg1); }

  template<typename element_tag, typename segment_handle_type>
  typename viennagrid::storage::result_of::value_type<
      typename viennagrid::storage::result_of::value_type<
          typename segment_handle_type::segmentation_type::appendix_type,
          interface_information_collection_tag
      >::type,
      element_tag
  >::type::segment_interface_information_wrapper_type const &
  interface_information_collection( segment_handle_type const & seg0, segment_handle_type const & seg1 )
  { return viennagrid::storage::collection::get<element_tag>( viennagrid::storage::collection::get<interface_information_collection_tag>( seg0.segmentation().appendix() ) ).get_interface_wrapper(seg0, seg1); }




  namespace result_of
  {
    /** @brief For internal use only */
    template<typename segment_id_type, typename interface_information_container_tag, typename ChangeCounterType, typename element_taglist>
    struct interface_information_collection_typemap_impl;

    template<typename segment_id_type, typename interface_information_container_tag, typename ChangeCounterType>
    struct interface_information_collection_typemap_impl<segment_id_type, interface_information_container_tag, ChangeCounterType, viennagrid::null_type>
    {
      typedef viennagrid::null_type type;
    };

    template<typename segment_id_type, typename interface_information_container_tag, typename ChangeCounterType, typename element_tag, typename tail>
    struct interface_information_collection_typemap_impl<segment_id_type, interface_information_container_tag, ChangeCounterType, viennagrid::meta::typelist_t<element_tag, tail> >
    {
      typedef typename storage::result_of::container<bool, interface_information_container_tag >::type base_container;

      typedef viennagrid::meta::typelist_t<
          viennagrid::meta::static_pair<
              element_tag,
              interface_information_wrapper<segment_id_type, base_container, ChangeCounterType>
          >,
          typename interface_information_collection_typemap_impl<segment_id_type, interface_information_container_tag, ChangeCounterType, tail>::type
      > type;
    };


    template<typename element_taglist, typename segment_id_type, typename interface_information_container_tag, typename ChangeCounterType>
    struct interface_information_collection_typemap
    {
      typedef typename cell_tag_from_typelist<element_taglist>::type cell_tag;
      typedef typename viennagrid::meta::typelist::result_of::erase< element_taglist, cell_tag>::type element_typelist_without_cell_tag;

      typedef typename interface_information_collection_typemap_impl<segment_id_type, interface_information_container_tag, ChangeCounterType, element_typelist_without_cell_tag>::type type;
    };
  }



  namespace result_of
  {
    /** @brief For internal use only */
    template<typename segment_element_info_container_typemap, typename segment_id_type, typename container_tag, typename segment_info_container_tag>
    struct segmentation_info_container_typemap;

    template<typename segment_id_type, typename container_tag, typename segment_info_container_tag>
    struct segmentation_info_container_typemap< viennagrid::null_type, segment_id_type, container_tag, segment_info_container_tag >
    {
      typedef viennagrid::null_type type;
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


    /** @brief For internal use only */
    template<typename element_typelist, typename segment_id_type, typename container_tag = viennagrid::storage::std_deque_tag>
    struct trivial_segmentation_appendix;

    template<typename segment_id_type,typename container_tag>
    struct trivial_segmentation_appendix< viennagrid::null_type, segment_id_type, container_tag >
    {
      typedef viennagrid::null_type type;
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



    /** @brief Metafunction for obtaining a segmentation type for a mesh type and with settings. Segment element information is not present (see segment_element_info for more information)
     *
     * @tparam MeshT          The base mesh type to which the segmentation is associated
     * @tparam ViewT            The mesh view type representing the referenced elements, default ist default mesh view from MeshT
     * @tparam SegmentIDType    The ID type for segments, default is int
     * @tparam AppendixType     The appendix type, for internal use only, don't change default type unless you know what you are doing :)
     */
    template<typename MeshT,
             typename ViewT = typename viennagrid::result_of::mesh_view<MeshT>::type,
             typename SegmentIDType = int,
             typename AppendixType =
                viennagrid::storage::collection_t<
                    typename viennagrid::meta::make_typemap<
                        element_segment_mapping_tag,
                        viennagrid::storage::collection_t<
                            typename trivial_segmentation_appendix<
                                typename viennagrid::result_of::element_typelist<MeshT>::type,
                                SegmentIDType
                            >::type
                        >,


                        interface_information_collection_tag,
                        viennagrid::storage::collection_t<
                          typename viennagrid::result_of::interface_information_collection_typemap<
                            typename viennagrid::result_of::element_taglist<MeshT>::type,
                            SegmentIDType,
                            viennagrid::storage::std_vector_tag,
                            typename viennagrid::result_of::change_counter_type<MeshT>::type
                          >::type
                        >

                    >::type
                >,
            typename view_container_tag = viennagrid::storage::std_deque_tag >
    struct segmentation
    {
      typedef config::segmentation_config_wrapper_t<MeshT, ViewT, SegmentIDType, AppendixType, view_container_tag> WrappedConfigType;

      typedef segmentation_t<WrappedConfigType> type;
    };



    /** @brief Metafunction for obtaining a segmentation type for a 3D hull mesh type and with settings. Segment element information is a bool (see segment_element_info for more information)
     *
     * @tparam MeshT          The base mesh type to which the segmentation is associated
     * @tparam ViewT            The mesh view type representing the referenced elements, default ist default mesh view from MeshT
     * @tparam SegmentIDType    The ID type for segments, default is int
     * @tparam AppendixType     The appendix type, for internal use only, don't change default type unless you know what you are doing :)
     */
    template<typename MeshT,
             typename ViewT = typename viennagrid::result_of::mesh_view<MeshT>::type,
             typename SegmentIDType = int,
             typename AppendixType =
              viennagrid::storage::collection_t<
                typename viennagrid::meta::make_typemap<
                    element_segment_mapping_tag,
                    viennagrid::storage::collection_t<
                        typename viennagrid::meta::typemap::result_of::modify<
                            typename trivial_segmentation_appendix<
                                typename viennagrid::result_of::element_typelist<MeshT>::type,
                                SegmentIDType
                            >::type,
                            viennagrid::meta::static_pair<
                                typename viennagrid::result_of::element< MeshT, viennagrid::triangle_tag >::type,
                                typename viennagrid::storage::result_of::container<
                                    segment_info_t< element_segment_mapping<SegmentIDType, bool> >,
                                    viennagrid::storage::std_deque_tag
                                >::type
                            >
                        >::type
                    >,

                    interface_information_collection_tag,
                    viennagrid::storage::collection_t<
                      typename viennagrid::result_of::interface_information_collection_typemap<
                        typename viennagrid::result_of::element_taglist<MeshT>::type,
                        SegmentIDType,
                        viennagrid::storage::std_vector_tag,
                        typename viennagrid::result_of::change_counter_type<MeshT>::type
                      >::type
                    >

                >::type
            >,
            typename view_container_tag = viennagrid::storage::std_deque_tag >
    struct oriented_3d_hull_segmentation
    {
      typedef config::segmentation_config_wrapper_t<MeshT, ViewT, SegmentIDType, AppendixType, view_container_tag> WrappedConfigType;

      typedef segmentation_t<WrappedConfigType> type;
    };

  }




  /** @brief For internal use only */
  template<typename segment_handle_type, typename element_segment_mapping_type, typename container_tag>
  bool is_in_segment( segment_handle_type const & segment, segment_info_t<element_segment_mapping_type, container_tag> const & segment_info )
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

  /** @brief For internal use only */
  template< typename segment_handle_type, typename accessor_type, typename element_type >
  bool is_in_segment( segment_handle_type const & segment, accessor_type const accessor, element_type const & element )
  {
    return is_in_segment( segment, accessor(element) );
  }

  /** @brief Queries if element is within a given segment
    *
    * @tparam SegmentHandleT        The segment type which might include the element
    * @tparam ElementTagT     The element tag of the element which might be included in the segment
    * @tparam WrappedConfigT  The wrapped config of the element which might be included in the segment
    * @param  segment         The segment object which might include the element
    * @param  element         The element object which might be included in the segment
    * @return                 True if the element is in the segment, false otherwise
    */
  template<typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
  bool is_in_segment( SegmentHandleT const & segment, element_t<ElementTagT, WrappedConfigT> const & element )
  {
    typedef element_t<ElementTagT, WrappedConfigT> element_type;
    return is_in_segment( segment, viennagrid::make_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
  }


  /** @brief Defines a range of segment IDs which represents segments in which an element is included, use result_of::segment_id_range for defining this type
    *
    * @tparam SegmentInfoT        For internal use only
    */
  template<typename SegmentInfoT>
  struct segment_id_range_t
  {
  public:

    segment_id_range_t(SegmentInfoT & segment_info) : segment_info_(&segment_info) {}

    /** @brief An iterator for iterating over all segment IDs */
    typedef typename SegmentInfoT::const_iterator         iterator;
    /** @brief An iterator for iterating over all segment IDs */
    typedef typename SegmentInfoT::const_iterator   const_iterator;

    /** @brief Returns an iterator pointing to the first segment ID
      *
      * @return    An iterator pointing to the first segment ID
      */
    iterator begin() { return segment_info_->begin(); }
    /** @brief Returns an iterator pointing to the end of the segment IDs
      *
      * @return    An iterator pointing to the end of the segment IDs
      */
    iterator end() { return segment_info_->end(); }

    /** @brief Returns a const iterator pointing to the first segment ID
      *
      * @return    A const iterator pointing to the first segment ID
      */
    const_iterator cbegin() { return segment_info_->cbegin(); }
    /** @brief Returns a const iterator pointing to the end of the segment IDs
      *
      * @return    A const iterator pointing to the end of the segment IDs
      */
    const_iterator cend() { return segment_info_->cend(); }

    /** @brief Returns a const iterator pointing to the first segment ID, const version
      *
      * @return    A const iterator pointing to the first segment ID
      */
    const_iterator begin() const { return segment_info_->begin(); }
    /** @brief Returns a const iterator pointing to the end of the segment IDs, const version
      *
      * @return    A const iterator pointing to the end of the segment IDs
      */
    const_iterator end() const { return segment_info_->end(); }

    /** @brief Returns a const iterator pointing to the first segment ID
      *
      * @return    A const iterator pointing to the first segment ID
      */
    bool empty() { return segment_info_->empty(); }

  private:
    SegmentInfoT * segment_info_;
  };

  namespace result_of
  {
    /** @brief Defines a range of segment IDs which represents segments in which an element is included
      *
      * @tparam SegmentOrSegmentationT        The host segmentation/segment type
      * @tparam ElementT                      The element type for which the segment id range is queried
      */
    template<typename SegmentOrSegmentationT, typename ElementT>
    struct segment_id_range;

    template<typename SegmentationT, typename ElementT>
    struct segment_id_range
    {
      typedef typename viennagrid::storage::result_of::value_type<
          typename SegmentationT::appendix_type,
          element_segment_mapping_tag
      >::type ElementSegmentMappingCollectionType;

      typedef typename viennagrid::storage::result_of::container_of<
        ElementSegmentMappingCollectionType,
        ElementT
      >::type ElementSegmentMappingContainerType;

      typedef segment_id_range_t<const typename ElementSegmentMappingContainerType::value_type> type;
    };

    template<typename SegmentationT, typename ElementT>
    struct segment_id_range< segment_handle_t<SegmentationT>, ElementT >
    {
      typedef typename segment_id_range<SegmentationT, ElementT>::type type;
    };
  }

  /** @brief Obtains the segment ID range of an element within a segmentation. The segment ID range holds all IDs of segments in which the element is included.
    *
    * @tparam SegmentationT   The host segmentation type
    * @tparam ElementTagT     The element tag of the element for which the segment ID range is obtained
    * @tparam WrappedConfigT  The wrapped config of the element for which the segment ID range is obtained
    * @param  segmentation    The host segmentation object
    * @param  element         The element object for which the segment ID range is obtained
    * @return                 The segment ID range
    */
  template<typename SegmentationT, typename ElementTagT, typename WrappedConfigT>
  typename result_of::segment_id_range< SegmentationT, element_t<ElementTagT, WrappedConfigT> >::type segment_ids( SegmentationT const & segmentation, element_t<ElementTagT, WrappedConfigT> const & element )
  {
    typedef typename result_of::segment_id_range< SegmentationT, element_t<ElementTagT, WrappedConfigT> >::type SegmentIDRangeType;
    typedef element_t<ElementTagT, WrappedConfigT> ElementType;
    return SegmentIDRangeType( viennagrid::make_accessor<ElementType>( element_segment_mapping_collection(segmentation) )(element) );
  }




  /** @brief For internal use only */
  template<typename segment_handle_type, typename element_segment_mapping_type, typename container_tag>
  typename element_segment_mapping_type::segment_element_info_type *
  segment_element_info( segment_handle_type const & segment, segment_info_t<element_segment_mapping_type, container_tag> & segment_info )
  {
    typedef typename segment_info_t<element_segment_mapping_type, container_tag>::element_segment_mapping_container_type element_segment_mapping_container_type;

    for (typename element_segment_mapping_container_type::iterator it = segment_info.element_segment_mapping_container.begin();
          it != segment_info.element_segment_mapping_container.end();
          ++it)
    {
      if (it->segment_id == segment.id())
        return &(it->segment_element_info);
    }

    return NULL;
  }

  /** @brief For internal use only */
  template<typename segment_handle_type, typename element_segment_mapping_type, typename container_tag>
  typename element_segment_mapping_type::segment_element_info_type const *
  segment_element_info( segment_handle_type const & segment, segment_info_t<element_segment_mapping_type, container_tag> const & segment_info )
  {
    typedef typename segment_info_t<element_segment_mapping_type, container_tag>::element_segment_mapping_container_type element_segment_mapping_container_type;

    for (typename element_segment_mapping_container_type::const_iterator it = segment_info.element_segment_mapping_container.begin();
          it != segment_info.element_segment_mapping_container.end();
          ++it)
    {
      if (it->segment_id == segment.id())
        return &(it->segment_element_info);
    }

    return NULL;
  }

  /** @brief For internal use only */
  template< typename segment_handle_type, typename accessor_type, typename element_type >
  typename accessor_type::value_type::segment_element_info_type *
  segment_element_info( segment_handle_type & segment, accessor_type accessor, element_type const & element )
  {
    return segment_element_info( segment, accessor(element) );
  }

  /** @brief For internal use only */
  template< typename segment_handle_type, typename accessor_type, typename element_type >
  typename accessor_type::value_type::segment_element_info_type const *
  segment_element_info( segment_handle_type const & segment, const accessor_type accessor, element_type const & element )
  {
    return segment_element_info( segment, accessor(element) );
  }

  /** @brief Returns a pointer to additional segment element information. Segment element information is meta information for a combination of a segment and an element, e.g. the orientation of a triangle in a segment in a 3D hull mesh
    *
    * @tparam SegmentHandleT        The segment type
    * @tparam ElementTagT     The element tag of the element
    * @tparam WrappedConfigT  The wrapped config of the element
    * @param  segment         The segment object
    * @param  element         The element object
    * @return                 A pointer to the segment element information type
    */
  template<typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
  typename viennagrid::storage::result_of::container_of<
      typename viennagrid::storage::result_of::value_type<
          typename SegmentHandleT::segmentation_type::appendix_type,
          element_segment_mapping_tag
      >::type,
      element_t<ElementTagT, WrappedConfigT>
  >::type::value_type::segment_element_info_type const *
  segment_element_info( SegmentHandleT const & segment, element_t<ElementTagT, WrappedConfigT> const & element )
  {
    typedef element_t<ElementTagT, WrappedConfigT> element_type;
    return segment_element_info( segment, viennagrid::make_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
  }

  /** @brief Returns a pointer to additional segment element information. Segment element information is meta information for a combination of a segment and an element, e.g. the orientation of a triangle in a segment in a 3D hull mesh. Const version
    *
    * @tparam SegmentHandleT        The segment type
    * @tparam ElementTagT     The element tag of the element
    * @tparam WrappedConfigT  The wrapped config of the element
    * @param  segment         The segment object
    * @param  element         The element object
    * @return                 A const pointer to the segment element information type
    */
  template<typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
  typename viennagrid::storage::result_of::container_of<
      typename viennagrid::storage::result_of::value_type<
          typename SegmentHandleT::segmentation_type::appendix_type,
          element_segment_mapping_tag
      >::type,
      element_t<ElementTagT, WrappedConfigT>
  >::type::value_type::segment_element_info_type *
  segment_element_info( SegmentHandleT & segment, element_t<ElementTagT, WrappedConfigT> const & element )
  {
    typedef element_t<ElementTagT, WrappedConfigT> element_type;
    return segment_element_info( segment, viennagrid::make_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
  }


  /** @brief For internal use only */
  template<typename segment_handle_type, typename element_segment_mapping_type, typename container_tag>
  void add(segment_handle_type & segment,
           segment_info_t<element_segment_mapping_type, container_tag> & segment_info)
  {
    typedef typename segment_info_t<element_segment_mapping_type, container_tag>::element_segment_mapping_container_type element_segment_mapping_container_type;

    for (typename element_segment_mapping_container_type::const_iterator it = segment_info.element_segment_mapping_container.begin();
          it != segment_info.element_segment_mapping_container.end();
          ++it)
    {
      if (it->segment_id == segment.id()) return;
    }

    segment_info.element_segment_mapping_container.push_back( element_segment_mapping_type(segment.id()) );
    increment_change_counter( segment.view() );
  }

  /** @brief For internal use only */
  template< typename segment_handle_type, typename accessor_type, typename element_type >
  void add( segment_handle_type & segment, accessor_type accessor, element_type & element )
  {
    return add( segment, accessor(element) );
  }


  /** @brief Adds a vertex to a segment
    *
    * @tparam SegmentHandleT        The segment type
    * @tparam WrappedConfigT  The wrapped config of the vertex
    * @param  segment         The segment object
    * @param  vertex          The vertex object
    */
  template<typename SegmentHandleT, typename WrappedConfigT>
  void add( SegmentHandleT & segment, element_t<vertex_tag, WrappedConfigT> & vertex )
  {
    typedef element_t<vertex_tag, WrappedConfigT> element_type;
    viennagrid::elements<element_type>( segment.view() ).insert_unique_handle( viennagrid::handle( segment.segmentation().mesh(), vertex ) );
    viennagrid::elements<element_type>( segment.segmentation().all_elements() ).insert_unique_handle( viennagrid::handle( segment.segmentation().mesh(), vertex ) );
    add( segment, viennagrid::make_accessor<element_type>( element_segment_mapping_collection(segment) ), vertex );
  }

  /** @brief Adds an element to a segment, all boundary elements are added recursively
    *
    * @tparam SegmentHandleT        The segment type
    * @tparam ElementTagT     The element tag of the element
    * @tparam WrappedConfigT  The wrapped config of the element
    * @param  segment         The segment object to which the elemen is added
    * @param  element         The element object to be added
    */
  template<typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
  void add( SegmentHandleT & segment, element_t<ElementTagT, WrappedConfigT> & element )
  {
    typedef element_t<ElementTagT, WrappedConfigT> element_type;
    viennagrid::elements<element_type>( segment.view() ).insert_unique_handle( viennagrid::handle( segment.segmentation().mesh(), element ) );
    viennagrid::elements<element_type>( segment.segmentation().all_elements() ).insert_unique_handle( viennagrid::handle( segment.segmentation().mesh(), element ) );
    add( segment, viennagrid::make_accessor<element_type>( element_segment_mapping_collection(segment) ), element );

    // recursively adding facet elements; view containers has to be std::set
    typedef typename viennagrid::result_of::facet_range< element_type >::type FacetRangeType;
    typedef typename viennagrid::result_of::iterator< FacetRangeType >::type FacetRangeIterator;

    FacetRangeType facets = viennagrid::elements( element );
    for (FacetRangeIterator it = facets.begin(); it != facets.end(); ++it)
      add(segment, *it);
  }




  /** @brief For internal use only */
  template<typename SegmentHandleT, typename ElementSegmentMappingT, typename ContainerTagT>
  void erase( SegmentHandleT & segment, segment_info_t<ElementSegmentMappingT, ContainerTagT> & segment_info )
  {
    typedef typename segment_info_t<ElementSegmentMappingT, ContainerTagT>::element_segment_mapping_container_type element_segment_mapping_container_type;

    for (typename element_segment_mapping_container_type::iterator it = segment_info.element_segment_mapping_container.begin();
                                                                   it != segment_info.element_segment_mapping_container.end();
                                                                 ++it)
    {
      if (it->segment_id == segment.id())
      {
        segment_info.element_segment_mapping_container.erase(it);
        increment_change_counter( segment.view() );
        return;
      }
    }
  }

  /** @brief For internal use only */
  template< typename SegmentHandleT, typename AccessorT, typename ElementT >
  void erase( SegmentHandleT & segment, AccessorT accessor, ElementT const & element )
  {
    return erase( segment, accessor(element) );
  }


//   template<typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
//   bool non_recursive_erase( SegmentHandleT & segment, element_t<ElementTagT, WrappedConfigT> & element )
//   {
//     typedef element_t<ElementTagT, WrappedConfigT> element_type;
//     viennagrid::elements<element_type>( segment.view() ).erase_handle( viennagrid::handle( segment.segmentation().mesh(), element ) );
//     return erase( segment, viennagrid::make_accessor<element_type>( element_segment_mapping_collection(segment) ), element );
//   }

  template<typename SegmentHandleT>
  struct non_recursive_erase_functor
  {
    non_recursive_erase_functor(SegmentHandleT & segment_) : segment(segment_) {}

    template<typename ElementT>
    void operator() ( ElementT & element )
    {
      // erasing element from segment view
      viennagrid::elements<ElementT>( segment.view() ).erase_handle( viennagrid::handle( segment.segmentation().mesh(), element ) );
      // erasing element from segment-element information
      erase( segment, viennagrid::make_accessor<ElementT>( element_segment_mapping_collection(segment) ), element );

      if (segment_ids(segment.segmentation(), element).empty())
        viennagrid::elements<ElementT>( segment.segmentation().all_elements() ).erase_handle( viennagrid::handle( segment.segmentation().mesh(), element ) );
    }

    SegmentHandleT & segment;
  };




  /** @brief Deletes an element from a segment. The runtime of this function is linear in the number of elements of the element type to delete
    *
    * @tparam SegmentHandleT        The segment type
    * @tparam ElementTagT     The element tag of the element type to be deleted
    * @tparam WrappedConfigT  The wrapped config of the element type to be deleted
    * @param  segment         The segment object from which the element is to be deleted
    * @param  element         The element object to be deleted from the segment
    * @return                 True if the element is deleted from the segment, false otherwise
    */
  template<typename SegmentHandleT, typename ElementTagT, typename WrappedConfigT>
  void erase( SegmentHandleT & segment, element_t<ElementTagT, WrappedConfigT> & element )
  {
    typedef typename viennagrid::result_of::mesh_view<SegmentHandleT>::type ToEraseViewT;
    ToEraseViewT elements_to_erase = viennagrid::make_view(segment);
    viennagrid::mark_referencing_elements( segment, elements_to_erase, viennagrid::handle(segment, element) );

    non_recursive_erase_functor<SegmentHandleT> functor( segment );
    viennagrid::for_each(elements_to_erase, functor);
  }




  /** @brief For internal use only */
  template<bool generate_id, bool call_callback, typename SegmentationType, typename ElementTag, typename WrappedConfigType>
  std::pair<
              typename viennagrid::storage::result_of::container_of<
                  typename result_of::element_collection< segment_handle_t<SegmentationType> >::type,
                  viennagrid::element_t<ElementTag, WrappedConfigType>
              >::type::handle_type,
              bool
          >
  push_element( segment_handle_t<SegmentationType> & segment, viennagrid::element_t<ElementTag, WrappedConfigType> const & element)
  {
    std::pair<
            typename viennagrid::storage::result_of::container_of<
                typename result_of::element_collection< segment_handle_t<SegmentationType> >::type,
                viennagrid::element_t<ElementTag, WrappedConfigType>
            >::type::handle_type,
            bool
        > result = push_element<generate_id, call_callback>( segment.view(), element );

    add( segment, viennagrid::dereference_handle(segment, result.first) );
    return result;
  }




  /** @brief Adds a single element handle (no boundary elements/handles) to a view if it isn't already present in that view. The runtime of this function is linear in the number of elements present in the view.
    *
    * @tparam ViewT    The view/segment type
    * @tparam HandleT           The handle to be added
    * @param  view_or_segment   The view/segment object
    * @param  handle            The handle object to be inserted
    */
  template<typename ViewT, typename HandleT>
  void add_single_handle( ViewT & view_or_segment, HandleT handle )
  {
    typedef typename storage::handle::result_of::value_type<HandleT>::type value_type;
    value_type & element = dereference_handle(view_or_segment, handle);

    typedef typename viennagrid::result_of::element_range< ViewT, value_type >::type range_type;
    typedef typename viennagrid::result_of::iterator<range_type>::type iterator_type;

    iterator_type it = find( view_or_segment, element.id() );
    if ( it == elements<value_type>(view_or_segment).end() )
      viennagrid::storage::collection::get<value_type>( element_collection(view_or_segment) ).insert_handle( handle );

    viennagrid::increment_change_counter(view_or_segment);
  }
}




#endif
