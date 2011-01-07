/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaMesh - The Vienna Mesh Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaMesh base directory
======================================================================= */


#ifndef VIENNAMESH_SEGMENT_HPP
#define VIENNAMESH_SEGMENT_HPP

#include "viennamesh/forwards.h"
#include "viennamesh/element.hpp"

namespace viennamesh
{
  
  /////////////// Segment is a container for elements ////////////////

  //helper: holds an ordered number of vertices
  //comparisons also take permutations into account
  template <typename ElementType, typename IDHandler = typename PointTag::IDHandler>
  struct ElementKeyStorageType
  {
    typedef ElementType *           ResultType;
  };

  template <typename ElementType>
  struct ElementKeyStorageType<ElementType, ProvideID>
  {
    typedef long          ResultType;
  };

  template <typename ElementType>
  class ElementKey
  {
      typedef typename ElementType::ElementTag            ElementTag;
      typedef typename ElementKeyStorageType<ElementType>::ResultType  StorageType;
    public:
      ElementKey( ElementType & el2) : vertexIDs(TopologyLevel<ElementTag, 0>::ElementNum)
      {
        typedef typename IteratorTypes<ElementType, 0>::ResultType         VertexIterator;
        long i = 0;
        for (VertexIterator vit = el2.template begin<0>();
              vit != el2.template end<0>(); ++vit, ++i)
          vertexIDs[i] = vit->getID();
        //sort it:
        std::sort(vertexIDs.begin(), vertexIDs.end());
      }

      ElementKey( const ElementKey & ek2) : vertexIDs(ek2.vertexIDs.size())
      {
        //std::cout << "Copy constructor ElementKey " << this << std::endl;
        for (typename std::vector<StorageType>::size_type i=0; i<ek2.vertexIDs.size(); ++i)
          vertexIDs[i] = ek2.vertexIDs[i];
      }

      bool operator < (ElementKey const & epc2) const
      {
        for (long i=0; i<TopologyLevel<ElementTag, 0>::ElementNum; ++i)
        {
          if ( vertexIDs[i] > epc2.vertexIDs[i] )
            return false;
          else if ( vertexIDs[i] < epc2.vertexIDs[i] )
            return true;
        }
        return false;
      }

      void print() const
      { 
        for (typename std::vector<StorageType>::const_iterator vit = vertexIDs.begin();
              vit != vertexIDs.end();
              ++vit)
          std::cout << *vit << " ";
        std::cout << std::endl;
      }

    private:
      std::vector< StorageType > vertexIDs;
  };


  //Segment type:
  template <typename T_Configuration, unsigned long levelnum, typename HandlingTag, bool specialHandling, typename MultigridTag>
  class segment {};


  template <typename T_Configuration, unsigned long levelnum, typename MultigridTag>
  class segment <T_Configuration, levelnum, TopoLevelFullHandling, false, MultigridTag> :
    public segment <T_Configuration, levelnum - 1>
  {
    typedef TopologyLevel<typename T_Configuration::CellTag, levelnum>       LevelSpecs;
    typedef segment <T_Configuration, levelnum - 1>      Base;

    typedef element<T_Configuration, typename LevelSpecs::ElementTag>      LevelElementType;
    typedef TopologyLevel<typename T_Configuration::CellTag, 0>       VertexSpecs;

    public:

      segment() : Base() {};

      template <long j>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, LevelElementType & elem, ElementOrientation * orientation, EqualType) {

        typedef typename std::map< ElementKey<LevelElementType>, LevelElementType >::iterator  ElementIterator;
        typedef typename IteratorTypes<LevelElementType, 0>::ResultType      VertexOnElementIterator;

        ElementKey<LevelElementType> epc(elem);
        //check whether already inserted:
        ElementIterator elit = elements.find(epc);
        //std::cout << "Candidate: "; elem.print_short();

        if (elit == elements.end())
        {
          //provide ID for element:
          elem.setID(elements.size());

          //std::cout << "ACCEPTED " << std::endl;

          //set default orientation:
          orientation->setDefaultOrientation();

          std::pair<ElementKey<LevelElementType>, LevelElementType> p(epc, elem);
          return &((elements.insert(p).first)->second);
        }

        //std::cout << "REJECTED" << std::endl;
        long i=0; long j=0;
        //set orientation:
        for (VertexOnElementIterator voeit = elem.template begin<0>();
              voeit != elem.template end<0>();
              ++voeit, ++i)
        {
            for (VertexOnElementIterator voeit2 = (elit->second).template begin<0>();
                  voeit2 != (elit->second).template end<0>();
                  ++voeit2, ++j)
            {
              if (voeit->getID() == voeit2->getID())
              {
                //orientation->setPermutation(i,j);   //local (elem) to global (elit->second)
                orientation->setPermutation(j,i);   //global (elit->second) to local (elem)
                break;
              }
            }
            j=0;
        }

        return &(elit->second);
      }
      
      template <long j, typename T>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, T & elem, ElementOrientation * orientation, LessType) {
        //std::cout << "operating on " << this << ": " << std::endl;
        //no handling: do nothing
        return Base::template add<j>(pos, elem, orientation);
      }

      template <long j, typename T>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, T & elem, ElementOrientation * orientation) {
        //std::cout << "operating on " << this << ": " << std::endl;
        //no handling: do nothing
        return add<j>(pos, elem, orientation, typename LevelDiscriminator<levelnum, j>::ResultType());
      }

      template <long j>
      long size(LessType) const { return Base::template size<j>(); }
      template <long j>
      long size(EqualType) const { return elements.size(); }
      template <long j>
      long size() const
      {
        return size<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }
      //void addBoundaryVertex(long pos) { vertices[pos].setOnBoundary(); }


      //begin:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(LessType)
      { 
        return Base::template begin<j>();
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(EqualType)
      { 
        typedef typename IteratorTypes< segment<T_Configuration>, j>::ResultType    IteratorType;
        return IteratorType(elements.begin());
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin()
      { 
        return begin<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }

      //end:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(LessType)
      { 
        return Base::template end<j>();
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(EqualType)
      { 
        typedef typename IteratorTypes< segment<T_Configuration>, j>::ResultType    IteratorType;
        return IteratorType(elements.end());
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end()
      { 
        return end<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }

    private:
      std::map< ElementKey<LevelElementType>, LevelElementType >   elements;
  };

  template <typename T_Configuration, unsigned long levelnum, typename MultigridTag>
  class segment <T_Configuration, levelnum, TopoLevelNoHandling, false, MultigridTag> :
    public segment <T_Configuration, levelnum - 1>
  {
    typedef TopologyLevel<typename T_Configuration::CellTag, levelnum>       LevelSpecs;
    typedef segment <T_Configuration, levelnum - 1>      Base;

    typedef element<T_Configuration, typename LevelSpecs::ElementTag>      LevelElementType;

    public:

      segment() : Base() {};

      //Force compiler error: no implementation for add at this level
      template <long j>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, LevelElementType & elem, ElementOrientation * orientation, EqualType) {

        typename TopologyLevel<typename T_Configuration::CellTag, j>::ERROR_NO_SEGMENT_HANDLING_AVAILABLE_FOR_THIS_ELEMENT_LEVEL   ErrorIndicator;
        //no handling: return Null-pointer
        return 0;
      }
      
      template <long j, typename T>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, T & elem, ElementOrientation * orientation, LessType) {
        //std::cout << "operating on " << this << ": " << std::endl;
        //no handling: do nothing
        return Base::template add<j>(pos, elem, orientation);
      }

      template <long j, typename T>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, T & elem, ElementOrientation * orientation) {
        //std::cout << "operating on " << this << ": " << std::endl;
        //no handling: do nothing
        return add<j>(pos, elem, orientation, typename LevelDiscriminator<levelnum, j>::ResultType());
      }

      template <long j>
      long size(LessType) const { return Base::template getElement<j>(); }
      template <long j>
      long size(EqualType) const { return 0; }
      template <long j>
      long size() const
      {
        return size<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }
      //void addBoundaryVertex(long pos) { vertices[pos].setOnBoundary(); }


/*      //begin:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(LessType)
      { 
        return Base::template begin<j>();
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(EqualType)
      { 
        //TODO: Use something like EmptyIterator here. What about emulation of this iterator?
        return false;
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin()
      { 
        return begin<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }

      //end:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(LessType)
      { 
        return Base::template end<j>();
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(EqualType)
      { 
        return false;
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end()
      { 
        return end<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }  */

  };

  //special treatment for vertices
  template <typename T_Configuration, typename HandlingTag>
  class segment <T_Configuration, 0uL, HandlingTag, true, FullMultigridTag>
    : public segment<T_Configuration, 0uL, HandlingTag, true, NoMultigridTag>
  {};

  template <typename T_Configuration, typename HandlingTag>
  class segment <T_Configuration, 0uL, HandlingTag, true, NoMultigridTag>
  {
    typedef typename DomainTypes<T_Configuration>::VertexType        LevelElementType;
    //typedef element<T_Configuration, typename VertexSpecs::ElementTag>   LevelElementType;
    typedef LevelElementType                                          VertexType;

    public:

      segment() : multigrid_enabled(false), multigrid_counter(0) {};

      template <long level>
      VertexType *
      add(long pos, LevelElementType const & elem, ElementOrientation * orientation)
      {
        unsigned long retindex;

        if (multigrid_enabled)
        {
          typename std::map< VertexType, unsigned long >::iterator  mapit = vertices_temp.find(elem);

          if (mapit == vertices_temp.end())
          {
            vertices_temp.insert( std::make_pair(elem, multigrid_counter) );
            vertices[multigrid_counter] = elem;
            vertices[multigrid_counter].setID(multigrid_counter);
            retindex = multigrid_counter; 
            ++multigrid_counter;
            //std::cout << "Set ID: " << multigrid_counter << std::endl;
          }
          else
          {
            //VertexType const *ret = &(mapit->first);
            retindex = mapit->second;
            //std::cout << "Vertex found!" << std::endl;
          }
        }
        else
        {
          vertices[pos] = elem;
          vertices[pos].setID(pos);
          retindex = pos;
        }

        if (retindex >= vertices.size())
          std::cout << "WARNING: " << retindex << " is invalid array index for vertices!" << std::endl;

        return &(vertices[retindex]);
      }

      template <long j>
      long size() const { return vertices.size(); }

      void reserveVertices(typename std::vector<VertexType>::size_type n){ vertices.resize(n); }

      const VertexType & getVertex(long pos) const { return vertices[pos]; }
      VertexType * getVertexAddress(long pos){ return &(vertices[pos]); }

      //begin:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin()
      { 
        return vertices.begin();
      }

      //end:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end()
      { 
        return vertices.end();
      }

      void enableMultigrid() { multigrid_enabled = true; }
      void refinementFinished()
      {
        //clear the temporary map:
        vertices_temp.clear();
        std::cout << "Refinement finished, new vertex number: " << multigrid_counter << std::endl;
      }

    private:
      std::map< VertexType, unsigned long >   vertices_temp;
      std::vector< VertexType >   vertices;
      bool multigrid_enabled;
      unsigned long multigrid_counter;
  };

  //special treatment for cells
  template <typename T_Configuration, unsigned long levelnum, typename HandlingTag>
  class segment <T_Configuration, levelnum, HandlingTag, true, FullMultigridTag> :
   public segment <T_Configuration, T_Configuration::CellTag::TopoLevel - 1>
  {
    typedef segment <T_Configuration, T_Configuration::CellTag::TopoLevel - 1>                                                         Base;
    typedef TopologyLevel<typename T_Configuration::CellTag, 0>       VertexSpecs;
    typedef typename DomainTypes<T_Configuration>::VertexType         VertexType;
    typedef TopologyLevel<typename T_Configuration::CellTag,
                            T_Configuration::CellTag::TopoLevel>             LevelSpecs;

    typedef typename DomainTypes<T_Configuration>::CellType            LevelElementType;

    public:
      typedef T_Configuration                                       Configuration;

      segment() : Base(), child_seg(0) {}
      ~segment()
      {
        //recursivly deletes child-segments
        if (child_seg != 0)
          delete child_seg;
      }

      template <long j>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, LevelElementType & elem, ElementOrientation * orientation, EqualType)
      {
        elements[pos] = elem;
        LevelElementType & insertedElement = elements[pos];

        //init cell:
        insertedElement.setID(pos);
        insertedElement.update_dt_dx();
        insertedElement.fill(*this);

        return &(insertedElement);
      }

      template <long j, typename T>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, T & elem, ElementOrientation * orientation, LessType) {
        //std::cout << "operating on " << this << ": " << std::endl;
        return Base::template add<j>(pos, elem, orientation);
      }

      template <long j, typename T>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, T & elem, ElementOrientation * orientation = 0 ) {
        //std::cout << "operating on " << this << ": " << std::endl;
        return add<j>(pos, elem, orientation, typename LevelDiscriminator<levelnum, j>::ResultType());
      }

      template <long j>
      long size(LessType) const { return Base::template size<j>(); }
      template <long j>
      long size(EqualType) const { return elements.size(); }
      template <long j>
      long size() const 
      {
        return size<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }
      //void addBoundaryVertex(long pos) { vertices[pos].setOnBoundary(); }

      void reserveCells(typename std::vector<VertexType>::size_type n){ elements.resize(n); }

//       const LevelElementType & getCell(long pos) const { return elements[pos]; }
//       LevelElementType & getCell(long pos){ return elements[pos]; }

      //begin:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(LessType, long refinement_level = 0)
      { 
        return Base::template begin<j>();
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(EqualType, long refinement_level = 0)
      { 
        typedef typename IteratorTypes< segment<T_Configuration>, j>::ResultType  LevelIterator;
        return LevelIterator(elements.begin(), refinement_level);
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(long refinement_level = 0)
      { 
        return begin<j>( typename LevelDiscriminator<levelnum, j>::ResultType(), refinement_level );
      }

      //end:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(LessType, long refinement_level = 0)
      { 
        return Base::template end<j>();
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(EqualType, long refinement_level = 0)
      { 
        typedef typename IteratorTypes< segment<T_Configuration>, j>::ResultType  LevelIterator;
        return LevelIterator(elements.end(), refinement_level);
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(long refinement_level = 0)
      { 
        return end<j>( typename LevelDiscriminator<levelnum, j>::ResultType(), refinement_level );
      }

      //provide a child segment:
//       segment * getChild()
//       {
//         if (child_seg == 0)
//           child_seg = new segment();
//         return child_seg;
//       }

      //refine segment
      void refineUniformly()
      {
        typedef typename std::vector< LevelElementType >::iterator     LevelIterator;

        //refine this segment only if no child is around:
        if (child_seg == 0)
        {
          //set up new segment
          child_seg = new segment();
          std::cout << "Reserving memory for " << Base::template size<0>() << "+" << 
                                    size<1>() << " vertices" << std::endl;
          child_seg->reserveVertices( Base::template size<0>() + 
                                      size<1>() );
          std::cout << "Reserving memory for " << pow(2, T_Configuration::CellTag::TopoLevel) * elements.size() << " cells" << std::endl;
          child_seg->reserveCells( pow(2, T_Configuration::CellTag::TopoLevel) * elements.size() );


          child_seg->enableMultigrid();

          //fill with vertices (to induce some sort of red-black-ordering):
          typedef typename IteratorTypes<segment, 0>::ResultType      VertexIterator;

          long vertex_added = 0;
          for (VertexIterator vit = this->template begin<0>();
                vit != this->template end<0>();
                ++vit, ++vertex_added)
            child_seg->add<0>(0, *vit);

          std::cout << "Added " << vertex_added << " vertices from parent segment" << std::endl;

          //fill with cells:
          long cell_id = 0;

          for (LevelIterator lit = elements.begin();
                lit != elements.end();
                ++lit)
          {
            //lit->print_short();
            lit->refineUniformly(cell_id, *child_seg);
          }

          child_seg->refinementFinished();
        }
        else
          child_seg->refineUniformly();

      }

      segment & getRefinedSegment(long level)
      {
        if (level > 0 && child_seg != 0)
         return child_seg->getRefinedSegment(level-1);
        else
         return *this;
      };

      const segment & getRefinedSegment(long level) const
      {
        if (level > 0 && child_seg != 0)
         return child_seg->getRefinedSegment(level-1);
        else
         return *this;
      };
      
    private:
      std::vector< LevelElementType > elements;
      segment * child_seg;
  };

  //special treatment for cells
  template <typename T_Configuration, unsigned long levelnum, typename HandlingTag>
  class segment <T_Configuration, levelnum, HandlingTag, true, NoMultigridTag> :
   public segment <T_Configuration, T_Configuration::CellTag::TopoLevel - 1>
  {
    typedef segment <T_Configuration, T_Configuration::CellTag::TopoLevel - 1>                                                         Base;
    typedef TopologyLevel<typename T_Configuration::CellTag, 0>       VertexSpecs;
    typedef typename DomainTypes<T_Configuration>::VertexType         VertexType;
    typedef TopologyLevel<typename T_Configuration::CellTag,
                            T_Configuration::CellTag::TopoLevel>             LevelSpecs;

    typedef typename DomainTypes<T_Configuration>::CellType            LevelElementType;

    public:
      typedef T_Configuration                                       Configuration;

      template <long j>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, LevelElementType & elem, ElementOrientation * orientation, EqualType)
      {
        elements[pos] = elem;
        LevelElementType & insertedElement = elements[pos];

        //init cell:
        insertedElement.setID(pos);
        insertedElement.fill(*this);

        return &(insertedElement);
      }

      template <long j, typename T>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, T & elem, ElementOrientation * orientation, LessType) {
        //std::cout << "operating on " << this << ": " << std::endl;
        return Base::template add<j>(pos, elem, orientation);
      }

      template <long j, typename T>
      element<T_Configuration,
              typename TopologyLevel<typename T_Configuration::CellTag, j>::ElementTag> *
      add(long pos, T & elem, ElementOrientation * orientation = 0 ) {
        //std::cout << "operating on " << this << ": " << std::endl;
        return add<j>(pos, elem, orientation, typename LevelDiscriminator<levelnum, j>::ResultType());
      }

      template <long j>
      long size(LessType) const { return Base::template size<j>(); }
      template <long j>
      long size(EqualType) const { return elements.size(); }
      template <long j>
      long size() const
      {
        return size<j>( typename LevelDiscriminator<levelnum, j>::ResultType() );
      }
      //void addBoundaryVertex(long pos) { vertices[pos].setOnBoundary(); }

      void reserveCells(typename std::vector<VertexType>::size_type n){ elements.resize(n); };

//       const LevelElementType & getCell(long pos) const { return elements[pos]; }
//       LevelElementType & getCell(long pos){ return elements[pos]; }

      //begin:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(LessType, long refinement_level = 0)
      { 
        return Base::template begin<j>();
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(EqualType, long refinement_level = 0)
      { 
        typedef typename IteratorTypes< segment<T_Configuration>, j>::ResultType  LevelIterator;
        return LevelIterator(elements.begin());
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      begin(long refinement_level = 0)
      { 
        return begin<j>( typename LevelDiscriminator<levelnum, j>::ResultType(), refinement_level );
      }

      //end:
      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(LessType, long refinement_level = 0)
      { 
        return Base::template end<j>();
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(EqualType, long refinement_level = 0)
      { 
        typedef typename IteratorTypes< segment<T_Configuration>, j>::ResultType  LevelIterator;
        return LevelIterator(elements.end());
      }

      template <long j>
      typename IteratorTypes< segment<T_Configuration>, j>::ResultType
      end(long refinement_level = 0)
      { 
        return end<j>( typename LevelDiscriminator<levelnum, j>::ResultType(), refinement_level );
      }

      segment & getRefinedSegment(long level) {  return *this;  };
      const segment & getRefinedSegment(long level) const {  return *this; };

    private:
      std::vector< LevelElementType > elements;
  };
  
  
}
#endif