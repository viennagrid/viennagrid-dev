/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_ELEMENT_HPP
#define VIENNAGRID_ELEMENT_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/celltags.hpp"
#include "viennagrid/iterators.hpp"

#include <vector>

namespace viennagrid
{
  
  
  /************** Level 1: Elements contained by a higher-level element *******/

  //local vertex numbering is in general different from global vertex numbering for lower-level topological elements
  //this permutator maps local numbers to global numbers
  //alternative implementation: vertexnum as template parameter
  class ElementOrientation
  {
    public:
      ElementOrientation() : permutator_(1) {};    //for std::vector-compatibility

      ElementOrientation(short vertexnum) : permutator_(vertexnum) {}

      void setDefaultOrientation()
      {
        for (unsigned short i=0; i<permutator_.size(); ++i)
          permutator_[i] = i;
      };

      short operator()(short in) const { return permutator_[in]; }

      void setPermutation(short index, short mappedTo) { permutator_[index] = mappedTo; };
      void resize(short vertexnum) { permutator_.resize(vertexnum); }

      void print() const
      {
        for (unsigned short i=0; i<permutator_.size(); ++i)
        {
          std::cout << i << "->" << permutator_[i] << ",";
        }
        std::cout << std::endl;
      }

    private:
      std::vector<short> permutator_;
  };

  template <typename T_Configuration,
              typename ElementTag,
              unsigned long levelnum,
              typename handling_tag = typename subcell_traits<ElementTag,levelnum>::handling_tag,
              bool LevelNull = (levelnum == 0)>
  class lower_level_holder  { };

  template <typename T_Configuration, typename ElementTag, unsigned long levelnum>
  class lower_level_holder <T_Configuration, ElementTag, levelnum, full_handling_tag, false> :
    public lower_level_holder < T_Configuration, ElementTag, levelnum - 1>
  {
    //requirements:
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    typedef subcell_traits<ElementTag, levelnum>                         LevelSpecs;
    typedef subcell_traits<typename LevelSpecs::element_tag, 0>  VertexOnElementSpecs;
    typedef lower_level_holder < T_Configuration, ElementTag, levelnum - 1 >      Base;

    typedef element<T_Configuration, typename LevelSpecs::element_tag>  LevelElementType;

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        //fill lower level first:
        Base::fill_level(dom);

        for (long i=0; i<LevelSpecs::num_elements; ++i)
          orientations_[i].resize(VertexOnElementSpecs::num_elements);

        subcell_filler<ElementTag, levelnum>::fill(&(elements_[0]),
                                                   &(Base::vertices_[0]),
                                                   &(orientations_[0]),
                                                   dom);
      }

      void print(long indent = 0) const
      {
        for (long j = 0; j<indent; ++j)
          std::cout << "   ";
        std::cout << "* Level " << levelnum << ": (FullHandling)" << std::endl;

        for (long i=0; i<LevelSpecs::num_elements; ++i)
        {
          std::cout << "Permutation (local to global) for element to come:" << std::endl;
          orientations_[i].print();
          elements_[i]->print(indent + 1);
        }
        Base::print(indent);
      }

    public:

      lower_level_holder( ) {};

      lower_level_holder( const lower_level_holder & llh) : Base (llh)
      {
        for (long i=0; i < LevelSpecs::num_elements; ++i)
          elements_[i] = llh.elements_[i];
      }

      /////////////////// access container: ////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag)
      { 
        return Base::template container<j>();
      }

      template <long j>
      typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(equal_tag)
      { 
        return &(elements_[0]);
      }

      template <long j>
      typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container()
      { 
        return container<j>( typename level_discriminator<levelnum, j>::result_type() );
      }
      
      //const:
      template <long j>
      const typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag) const
      { 
        return Base::template container<j>();
      }

      template <long j>
      const typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(equal_tag) const
      { 
        return &(elements_[0]);
      }

      template <long j>
      const typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container() const
      { 
        return container<j>( typename level_discriminator<levelnum, j>::result_type() );
      }
      
      
      
      
      

      ////////////////// orientation: ////////////////////
      template <long j>
      ElementOrientation const &
      getLevelOrientation(long index, less_tag)
      { 
        return Base::template getLevelOrientation<j>(index);
      }

      template <long j>
      ElementOrientation const &
      getLevelOrientation(long index, equal_tag) 
      { 
        return orientations_[index];
      }

      template <long j>
      ElementOrientation const &
      getLevelOrientation(long index)
      { 
        return getLevelOrientation<j>( index, typename level_discriminator<levelnum, j>::result_type() );
      }

    private: 
      LevelElementType * elements_[LevelSpecs::num_elements];
      ElementOrientation orientations_[LevelSpecs::num_elements];
  };

  template <typename T_Configuration, typename ElementTag, unsigned long levelnum>
  class lower_level_holder <T_Configuration, ElementTag, levelnum, no_handling_tag, false> :
    public lower_level_holder < T_Configuration, ElementTag, levelnum - 1 >
  {
    //requirements:
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    //typedef typename DomainTypes<T_Configuration>::segment_type        SegmentType;
    typedef subcell_traits<ElementTag, levelnum>                                LevelSpecs;
    typedef lower_level_holder < T_Configuration, ElementTag, levelnum - 1 >      Base;

    typedef element<T_Configuration, typename LevelSpecs::element_tag>  LevelElementType;

    protected:
      template <typename DomainType>
      void fill_level(DomainType & dom)
      {
        //fill lower topological levels only:
        Base::fill_level(dom);

/*
        facets_[0] = seg.addFacet(Base::vertices_[0], Base::vertices_[1], Base::vertices_[3]);
        facets_[0].first->fillEdges(seg);
              ...... and so on .........
*/
      }

      void print(long indent = 0) const
      {
        for (long j = 0; j<indent; ++j)
          std::cout << "   ";
        std::cout << "* Level " << levelnum << ": (NoHandling)" << std::endl;

        Base::print(indent);
      }

    public:

      lower_level_holder( ) {};

      lower_level_holder( const lower_level_holder & llh) : Base (llh) {}
      
      //////////////////// container ///////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag)
      { 
        return Base::template container<j>();
      }

      template <long j>
      typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container()
      { 
        return container<j>( typename level_discriminator<levelnum, j>::result_type() );
      }


      //const:
      template <long j>
      const typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag) const
      { 
        return Base::template container<j>();
      }

      template <long j>
      const typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container() const
      { 
        return container<j>( typename level_discriminator<levelnum, j>::result_type() );
      }

  };


  //at level 0, i.e. vertex level, recursion ends:
  template <typename T_Configuration, typename ElementTag, typename handling_tag>
  class lower_level_holder <T_Configuration, ElementTag, 0, handling_tag, true> 
  {
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    //typedef typename DomainTypes<T_Configuration>::segment_type               SegmentType;
    typedef subcell_traits<ElementTag, 0>                                      LevelSpecs;

    typedef element<T_Configuration, typename LevelSpecs::element_tag>         VertexType;
    typedef typename result_of::point_type<T_Configuration>::type              PointType;

    typedef typename result_of::iterator< element<T_Configuration, ElementTag>, 0>::type         VertexIterator;

    protected:
      //end recursion:
      template <typename DomainType>
      void fill_level(DomainType & dom) {}

      void print(long indent = 0) const
      {
        for (long i = 0; i<indent; ++i)
          std::cout << "   ";
        std::cout << "* Level " << 0 << ": " << std::endl;

        for (long i = 0; i<LevelSpecs::num_elements; ++i)
        {
          for (long j = 0; j<=indent; ++j)
            std::cout << "   ";
          vertices_[i]->print();
        }
      }

      void print_short() const
      {
        std::cout << "* Vertices: " << std::endl;

        for (long i = 0; i<LevelSpecs::num_elements; ++i)
        {
          std::cout << "Vertex " << i << " (" << vertices_[i] << "): "; vertices_[i]->print();
        }
      }

    public:
      lower_level_holder() {};

      lower_level_holder( const lower_level_holder & llh)
      {
        for (long i=0; i < LevelSpecs::num_elements; ++i)
          vertices_[i] = llh.vertices_[i];
      }

      PointType const & getPoint(long index) const { return (vertices_[index])->getPoint(); }

      ////////////////// container access: /////////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container()
      { 
        return &(vertices_[0]);
      }

      //const:
      template <long j>
      const typename result_of::element_container< element<T_Configuration, ElementTag>, j, T_Configuration::cell_tag::topology_level>::type *
      container() const
      { 
        return &(vertices_[0]);
      }


    protected:
      VertexType * vertices_[LevelSpecs::num_elements];
  };

  ////////////// LAYER 3: Multigrid capabilities //////////
  template <typename T_Configuration, typename ElTag, typename MultigridTag = typename T_Configuration::multigrid_tag>
  class multigrid_layer
  {  };

  /*
  template <typename T_Configuration, typename ElTag>
  class multigrid_layer<T_Configuration, ElTag, full_multigrid_tag> :
    public lower_level_holder < T_Configuration, ElTag, ElTag::topology_level - 1>
  {
      typedef lower_level_holder < T_Configuration, ElTag, ElTag::topology_level - 1>                     Base;
      typedef element<T_Configuration, ElTag>                       ElementType;

    public:
      //typedef typename std::list<ElementType *>::iterator         ChildIterator;
      typedef childit< element<T_Configuration, ElTag> >            ChildIterator;

      multigrid_layer() : Base() {}
      multigrid_layer(const multigrid_layer & mgl) : Base(mgl) {}

      ChildIterator getChildrenBegin() { return ChildIterator(children.begin()); }
      ChildIterator getChildrenEnd() { return ChildIterator(children.end()); }

      long child_num() { return children.size(); }

      template <typename SegmentType>
      void refine_uniformly(long & cell_id, SegmentType & seg)
      {
        //delegate work to ElementTag:
        ElTag::refine_uniformly( &(Base::vertices_[0]), children, cell_id, seg);
      }

    private:
      //pointers to children:
      std::list<ElementType *>  children;
  }; */

  //if no multigrid-handling is needed, no implementation is needed :-)
  template <typename T_Configuration, typename ElTag>
  class multigrid_layer<T_Configuration, ElTag, no_multigrid_tag> :
    public lower_level_holder < T_Configuration, ElTag, ElTag::topology_level - 1>
  {
      typedef lower_level_holder < T_Configuration, ElTag, ElTag::topology_level - 1>             Base;

    public:
      multigrid_layer() : Base() {}
      multigrid_layer(const multigrid_layer & mgl) : Base(mgl) {}

  };

  ////////////// LAYER 4: Top Level configuration //////////
  template <typename T_Configuration,
            typename ElTag>
  class element :
      public multigrid_layer < T_Configuration, ElTag > ,
      public cell_traits<ElTag>::id_handler
  {
      typedef typename T_Configuration::numeric_type                   ScalarType;
      typedef multigrid_layer < T_Configuration, ElTag >               Base;
      typedef typename result_of::point_type<T_Configuration>::type              PointType;
      typedef typename result_of::ncell_type<T_Configuration, 0>::type       VertexType;
      typedef subcell_traits<ElTag, 0>                                 VertexSpecs;

    public:
      typedef T_Configuration                                       config_type;
      typedef ElTag                                                 element_tag;

      element ( ) : onBoundary(false) {};   //construction of "template" for this type

      element (const element & e2) : Base(e2), onBoundary(e2.onBoundary)
      {
        //std::cout << "Copy constructor Element " << this << std::endl;
        this->setID(e2.getID());
      };

      template <typename DomainType>
      void fill(DomainType & dom)
      {
        Base::fill_level(dom);
      }

      void print(long indent = 0) const
      {
        for (long j = 0; j<indent; ++j)
          std::cout << "   ";
        std::cout << "---- " << ElTag::name() << " " << this << " ---------" << std::endl;
        Base::print(indent + 1);
      }

      void print_short() const
      {
        std::cout << "---- " << ElTag::name() << " " << this << " ---------" << std::endl;
        Base::print_short();
      }

      VertexType & getVertex(int pos) const { return *(Base::vertices_[pos]); }

      void setVertices(VertexType **vertices)
      {
        for(int i=0; i<VertexSpecs::num_elements; i++)
        {
          Base::vertices_[i] = vertices[i];
          //std::cout << i << " ";
        }
        //std::cout << std::endl;
      }

      /*PointType getMidPoint() const
      {
        PointType retPoint;
        for(int i=0; i<VertexSpecs::num_elements; i++)
        {
          retPoint += Base::vertices_[i]->getPoint();
        }
        retPoint /= VertexSpecs::num_elements;
        return retPoint;
      }*/


      /*bool operator==(element & c2)
      {
        //simple algorithm that checks each vertex for a matching vertex in c2
        //For a small number of vertices this is not too bad.
        bool found = false;
        for (int i=0; i<subcell_traits<ElementTag,0>::num_elements; ++i)
        {
          for (int j=0; j<subcell_traits<ElementTag,0>::num_elements; ++j)
          {
            if (Base::vertices_[i] == c2.vertices_[j])
            {
              found = true;
              break;
            }
          }
          if (found){
            found = false;
            continue;
          }
          //not found:
          return false;
        }
        //all vertices found:
        return true;
      } //operator== */

      ///////////////////// container ////////////////////
      
      //non-const:
      template <long j>
      typename result_of::element_container< element<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag)
      { 
        return Base::template container<j>();
      }

      template <long j>
      typename result_of::element_container< element<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container()
      { 
        return container<j>(less_tag());
      }


      //const
      template <long j>
      const typename result_of::element_container< element<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container(less_tag) const
      { 
        return Base::template container<j>();
      }

      template <long j>
      const typename result_of::element_container< element<T_Configuration, element_tag>, j, T_Configuration::cell_tag::topology_level>::type *
      container() const
      { 
        return container<j>(less_tag());
      }


    private:
      bool onBoundary;

  };

  //vertex-type needs separate treatment:
  template <typename T_Configuration>
  class element <T_Configuration, point_tag> :
    public cell_traits<point_tag>::id_handler
  {
      typedef typename result_of::point_type<T_Configuration>::type          PointType;
      typedef typename T_Configuration::numeric_type                         CoordType;
      typedef typename result_of::ncell_type<T_Configuration, 0>::type       VertexType;
      typedef typename T_Configuration::cell_tag                             CellTag;

    public:
      typedef T_Configuration                                       config_type;
      typedef point_tag                                             element_tag;
      typedef typename cell_traits<point_tag>::id_handler           id_handler;

      element() { };

      element(PointType const & p, long id = -1) : point_(p)
      {
        this->setID(id);
      };

      element(const element & e2)
      {
        //std::cout << "Copy constructor Element (Vertex) " << this << std::endl;
        point_ = e2.point_;
        this->setID(e2.id_);
      }

      element & operator=(const element & other)
      {
        point_ = other.point_;
        return *this;
      }

      PointType & getPoint() { return point_; }
      PointType const & getPoint() const { return point_; }

      void print(long indent = 0) const
      {
        for (long j = 0; j<indent; ++j)
          std::cout << "   ";
        std::cout << "Vertex (" << this << ") " << this->getID() << ": ";
        std::cout << point_ << std::endl;
      }

      void print_short() const
      {
        std::cout << "Vertex (" << this << ") " << this->getID() << ": ";
        std::cout << point_ << std::endl;
      }

      void printAddress() const{
        std::cout << this << std::endl;
      }

      //compare function for operator== (depends on ID-Handling!)
      //bool compare(VertexType & v2, NoID &)
      //{
      //  return v2.point_ == point_;
      //}

      //bool compare(VertexType & v2, ProvideID &)
      //{
      //  return v2.id_ == this->id_;
      //}

      bool operator<(const element e2) const { return point_ < e2.point_; }

      //PointType getMidPoint() { return getPoint(); }

      //bool operator==(VertexType & v2)
      //{
      //  return compare(v2, subcell_traits<typename T_Configuration::cell_tag,0>::id_handler() );
      //}
    private:
      PointType point_;
  };


}


#endif