//#define THREEDIM
#define TWODIM
//#define ONEDIM

  struct NewDomainTesting
  {
    typedef double                                              CoordType;
    #ifdef THREEDIM
    typedef ThreeDimensionsTag                                  DimensionTag;
    typedef TetrahedronTag                                       CellTag;
    #endif
    #ifdef TWODIM
    typedef TwoDimensionsTag                                    DimensionTag;
    typedef TriangleTag                                         CellTag;
    #endif
    #ifdef ONEDIM
    typedef OneDimensionTag                                     DimensionTag;
    typedef LineTag                                             CellTag;
    #endif

    //multigrid:
    //typedef FullMultigridTag                                    MultigridTag;
    typedef NoMultigridTag                                    MultigridTag;

    //Read boundary from file?
    //yes: use BoundaryKeyType<id>
    //no:  use NoBoundaryRead
    //typedef BoundaryKeyType<11>                                  BoundaryReadTag;
    typedef NoBoundaryRead                                  BoundaryReadTag;
  };
 
