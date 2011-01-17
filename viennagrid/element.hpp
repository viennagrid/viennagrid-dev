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
#include "viennagrid/iterators.h"

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
              typename HandlingTag = typename TopologyLevel<ElementTag,levelnum>::HandlingTag,
              bool LevelNull = (levelnum == 0)>
  class lower_level_holder  { };

  template <typename T_Configuration, typename ElementTag, unsigned long levelnum>
  class lower_level_holder <T_Configuration, ElementTag, levelnum, topology_level_full_handling, false> :
    public lower_level_holder < T_Configuration, ElementTag, levelnum - 1>
  {
    //requirements:
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    typedef typename DomainTypes<T_Configuration>::segment_type        SegmentType;
    typedef TopologyLevel<ElementTag, levelnum>                         LevelSpecs;
    typedef TopologyLevel<typename LevelSpecs::ElementTag, 0>  VertexOnElementSpecs;
    typedef lower_level_holder < T_Configuration, ElementTag, levelnum - 1 >      Base;

    typedef element<T_Configuration, typename LevelSpecs::ElementTag>  LevelElementType;

    protected:
      void fillLevel(SegmentType & seg)
      {
        //fill lower level first:
        Base::fillLevel(seg);

        for (long i=0; i<LevelSpecs::ElementNum; ++i)
          orientations_[i].resize(VertexOnElementSpecs::ElementNum);

        //Filling has to be done in CellTag:
        TopologyLevel<ElementTag, levelnum>::fill(&(elements_[0]),
                                                  &(Base::vertices_[0]),
                                                  &(orientations_[0]),
                                                  seg);
      }

      void print(long indent = 0) const
      {
        for (long j = 0; j<indent; ++j)
          std::cout << "   ";
        std::cout << "* Level " << levelnum << ": (FullHandling)" << std::endl;

        for (long i=0; i<LevelSpecs::ElementNum; ++i)
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
        for (long i=0; i < LevelSpecs::ElementNum; ++i)
          elements_[i] = llh.elements_[i];
      }

      //provide iterators:
      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin(LessType)
      { 
        return Base::template begin<j>();
      }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin(EqualType)
      { 
        typedef typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
            LevelIterator;
        return LevelIterator(elements_);
      }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin()
      { 
        return begin<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }


      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end(LessType)
      { 
        return Base::template end<j>();
      }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end(EqualType)
      { 
        typedef typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType   LevelIterator;
        return LevelIterator(elements_ + LevelSpecs::ElementNum);
      }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end()
      { 
        return end<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }

      //orientation:
      template <long j>
      ElementOrientation const &
      getLevelOrientation(long index, LessType)
      { 
        return Base::template getLevelOrientation<j>(index);
      }

      template <long j>
      ElementOrientation const &
      getLevelOrientation(long index, EqualType) 
      { 
        return orientations_[index];
      }

      template <long j>
      ElementOrientation const &
      getLevelOrientation(long index)
      { 
        return getLevelOrientation<j>( index, typename LevelDiscriminator<levelnum, j>::ResultType() );
      }

    private: 
      LevelElementType * elements_[LevelSpecs::ElementNum];
      ElementOrientation orientations_[LevelSpecs::ElementNum];
  };

  template <typename T_Configuration, typename ElementTag, unsigned long levelnum>
  class lower_level_holder <T_Configuration, ElementTag, levelnum, topology_level_no_handling, false> :
    public lower_level_holder < T_Configuration, ElementTag, levelnum - 1 >
  {
    //requirements:
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    typedef typename DomainTypes<T_Configuration>::segment_type        SegmentType;
    typedef TopologyLevel<ElementTag, levelnum>                                LevelSpecs;
    typedef lower_level_holder < T_Configuration, ElementTag, levelnum - 1 >      Base;

    typedef element<T_Configuration, typename LevelSpecs::ElementTag>  LevelElementType;

    protected:
      void fillLevel(SegmentType & seg)
      {
        //fill lower topological levels only:
        Base::fillLevel(seg);

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

      //provide iterators:
      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin(LessType)
      { 
        return Base::template begin<j>();
      }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin(EqualType)
      { 
        typedef typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
          LevelIterator;

        LevelElementType * elements_[LevelSpecs::ElementNum];
        //TODO: Set up elements here (via CellTag)
        return LevelIterator(elements_);
      }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin()
      { 
        return begin<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }


      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end(LessType)
      { 
        return Base::template end<j>();
      }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end(EqualType)
      { 
        typedef
          typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
            LevelIterator;

        LevelElementType * elements_[LevelSpecs::ElementNum];
        //TODO: Set up endpoint here (via CellTag)
        return LevelIterator(elements_ + LevelSpecs::ElementNum);
      }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end()
      { 
        return end<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }
  };


  //at level 0, i.e. vertex level, recursion ends:
  template <typename T_Configuration, typename ElementTag, typename HandlingTag>
  class lower_level_holder <T_Configuration, ElementTag, 0, HandlingTag, true> 
  {
    //array of pointers to elements of class 'levelnum' and a integer representing the orientation within the cell relative to the element it points to.
    typedef typename DomainTypes<T_Configuration>::segment_type        SegmentType;
    typedef TopologyLevel<ElementTag, 0>                                LevelSpecs;

    typedef element<T_Configuration, typename LevelSpecs::ElementTag>  VertexType;
    typedef typename DomainTypes<T_Configuration>::point_type        PointType;

    typedef typename IteratorTypes< element<T_Configuration, ElementTag>, 0>::ResultType                                          VertexIterator;

    template <typename FEMConfig, typename CellType, typename IntegrationDomain,
                bool map_u, typename BFTag>
    friend class MappingIterator;

    protected:
      //end recursion:
      void fillLevel(SegmentType & seg) {};

      void print(long indent = 0) const
      {
        for (long i = 0; i<indent; ++i)
          std::cout << "   ";
        std::cout << "* Level " << 0 << ": " << std::endl;

        for (long i = 0; i<LevelSpecs::ElementNum; ++i)
        {
          for (long j = 0; j<=indent; ++j)
            std::cout << "   ";
          vertices_[i]->print();
        }
      }

      void print_short() const
      {
        std::cout << "* Vertices: " << std::endl;

        for (long i = 0; i<LevelSpecs::ElementNum; ++i)
        {
          std::cout << "Vertex " << i << " (" << vertices_[i] << "): "; vertices_[i]->print();
        }
      }

    public:
      lower_level_holder() {};

      lower_level_holder( const lower_level_holder & llh)
      {
        for (long i=0; i < LevelSpecs::ElementNum; ++i)
          vertices_[i] = llh.vertices_[i];
      }

      PointType const & getPoint(long index) const { return (vertices_[index])->getPoint(); }

      //provides level iterators
      //Note: There is no LevelDiscriminator-thingy done here, because partly it is already protected by higher levels.
      template <long j>
      VertexIterator begin() { return VertexIterator(vertices_); }

      template <long j>
      VertexIterator end()   { return VertexIterator(vertices_ + LevelSpecs::ElementNum); }

    protected:
      VertexType * vertices_[LevelSpecs::ElementNum];
  };

  ////////////// LAYER 3: Multigrid capabilities //////////
  template <typename T_Configuration, typename ElTag, typename MultigridTag = typename T_Configuration::multigrid_tag>
  class multigrid_layer
  {  };

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

      long getChildNum() { return children.size(); }

      template <typename SegmentType>
      void refineUniformly(long & cell_id, SegmentType & seg)
      {
        //delegate work to ElementTag:
        ElTag::refineUniformly( &(Base::vertices_[0]), children, cell_id, seg);
      }

    private:
      //pointers to children:
      std::list<ElementType *>  children;
  };

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
  template <typename T_Configuration, typename ElTag>
  class element :
      public multigrid_layer < T_Configuration, ElTag > ,
      public ElTag::IDHandler
  {
      typedef typename T_Configuration::numeric_type                   ScalarType;
      typedef multigrid_layer < T_Configuration, ElTag >              Base;
      typedef typename DomainTypes<T_Configuration>::point_type      PointType;
      typedef typename DomainTypes<T_Configuration>::segment_type    SegmentType;
      typedef typename DomainTypes<T_Configuration>::vertex_type     VertexType;
      typedef TopologyLevel<ElTag, 0>                                 VertexSpecs;

    public:
      typedef T_Configuration                                       Configuration;
      typedef ElTag                                                 ElementTag;

      element ( ) : onBoundary(false) {};   //construction of "template" for this type

      element (const element & e2) : Base(e2), onBoundary(e2.onBoundary)
      {
        //std::cout << "Copy constructor Element " << this << std::endl;
        this->setID(e2.getID());
      };

      void fill(SegmentType & seg)
      {
        Base::fillLevel(seg);
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
        for(int i=0; i<VertexSpecs::ElementNum; i++)
        {
          Base::vertices_[i] = vertices[i];
          //std::cout << i << " ";
        }
        //std::cout << std::endl;
      }

      PointType getMidPoint() const
      {
        PointType retPoint;
        for(int i=0; i<VertexSpecs::ElementNum; i++)
        {
          retPoint += Base::vertices_[i]->getPoint();
        }
        retPoint /= VertexSpecs::ElementNum;
        return retPoint;
      }


      bool operator==(element & c2)
      {
        //simple algorithm that checks each vertex for a matching vertex in c2
        //For a small number of vertices this is not too bad.
        bool found = false;
        for (int i=0; i<TopologyLevel<ElementTag,0>::ElementNum; ++i)
        {
          for (int j=0; j<TopologyLevel<ElementTag,0>::ElementNum; ++j)
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
      } //operator==

      bool isOnBoundary() const { return onBoundary; }

      //toggles boundary flag. Boundary detection is done using facet iteration: Facets that belong to one cell only belong to the boundary. All lower level elements on that facet also lie on the boundary!
      void toggleOnBoundary() { onBoundary = !onBoundary; }

      //Provide a cell-on-cell-iterator, which is just the "this"-pointer
      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin(LessType)
      { return Base::template begin<j>(); }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin(EqualType)
      { return this; }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      begin()
      { 
        return begin<j>(typename LevelDiscriminator<ElementTag::topology_level, j>::ResultType());
      }


      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end(LessType)
      { return Base::template end<j>(); }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end(EqualType)
      { return this + 1; }

      template <long j>
      typename IteratorTypes< element<T_Configuration, ElementTag>, j>::ResultType
      end()
      { 
        return end<j>( typename LevelDiscriminator<ElementTag::topology_level, j>::ResultType() );
      }

    private:
      bool onBoundary;

  };

  //vertex-type needs separate treatment:
  template <typename T_Configuration>
  class element <T_Configuration, point_tag> :
    public point_tag::IDHandler
  {
      typedef typename DomainTypes<T_Configuration>::point_type     PointType;
      typedef typename T_Configuration::numeric_type               CoordType;
      typedef typename DomainTypes<T_Configuration>::vertex_type    VertexType;
      typedef typename T_Configuration::cell_tag                    CellTag;

    public:
      typedef T_Configuration                                       Configuration;
      typedef point_tag                                              ElementTag;
      typedef typename point_tag::IDHandler                        IDHandler;

      element ( ) : onBoundary(false) { };   //construction of "template" for this type

      element(PointType const & p, long id = -1) : point_(p), onBoundary(false)
      {
        this->setID(id);
      };

      element(const element & e2) : onBoundary(e2.onBoundary)
      {
        //std::cout << "Copy constructor Element (Vertex) " << this << std::endl;
        point_ = e2.point_;
        this->setID(e2.id_);
      }

      //~vertex_t(){
      //std::cout << "Freeing vertex: " << this << std::endl;
      //}

      PointType & getPoint() { return point_; }
      //PointType & getPoint() const { return point_; }

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

      void init_dt_dx() { }

      //compare function for operator== (depends on ID-Handling!)
      bool compare(VertexType & v2, NoID &)
      {
        return v2.point_ == point_;
      }

      bool compare(VertexType & v2, ProvideID &)
      {
        return v2.id_ == this->id_;
      }

      bool operator<(const element e2) const { return point_ < e2.point_; }

      PointType getMidPoint() { return getPoint(); }

      bool operator==(VertexType & v2)
      {
        return compare(v2, TopologyLevel<typename T_Configuration::cell_tag,0>::IDHandler() );
      }

      bool isOnBoundary() const { return onBoundary; }

      //toggles boundary flag. Boundary detection is done using facet iteration: Facets that belong to one cell only belong to the boundary. All lower level elements on that facet also lie on the boundary!
      void toggleOnBoundary() { onBoundary = !onBoundary; }

      //Provide a cell-on-cell-iterator, which is just the "this"-pointer
      template <long j>
      typename IteratorTypes< element<T_Configuration, point_tag>, j>::ResultType
      begin()
      { 
        return this;
      }


      template <long j>
      typename IteratorTypes< element<T_Configuration, point_tag>, j>::ResultType
      end(EqualType)
      { return this + 1; }

      template <long j>
      typename IteratorTypes< element<T_Configuration, point_tag>, j>::ResultType
      end()
      { 
        return this + 1;
      }


    private:
      PointType point_;
      bool onBoundary;
  };


}


#endif