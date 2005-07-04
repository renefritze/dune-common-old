#include "geometry.hh"
#include "entity.hh"
#include "grid.hh"
#include "faceutility.hh"

namespace Dune {

/************************************************************************************
  ###
   #     #    #   #####  ######  #####    ####   ######   ####      #     #####
   #     ##   #     #    #       #    #  #       #       #    #     #       #
   #     # #  #     #    #####   #    #   ####   #####   #          #       #
   #     #  # #     #    #       #####        #  #       #          #       #
   #     #   ##     #    #       #   #   #    #  #       #    #     #       #
  ###    #    #     #    ######  #    #   ####   ######   ####      #       #
************************************************************************************/

// --IntersectionIterator 
template<class GridImp>
inline ALU3dGridIntersectionIterator<GridImp> :: 
ALU3dGridIntersectionIterator(const GridImp & grid,
                              ALU3DSPACE HElementType *el, 
                              int wLevel,bool end) : 
  ALU3dGridEntityPointer<0,GridImp> ( grid , wLevel, end ),
  connector_(),
  geoProvider_(0),
  intersectionGlobal_(0),
  intersectionSelfLocal_(0),
  intersectionNeighborLocal_(0),
  item_(0),
  bndEntity_(0),
  nFaces_(el? el->nFaces() : 0),
  walkLevel_(wLevel),
  index_(0),
  generatedGlobalGeometry_(false),
  generatedLocalGeometries_(false)
{
  if (!end) {
    intersectionGlobal_ = 
      this->grid_.geometryProvider_.getNewObjectEntity(this->grid_,walkLevel_);
    intersectionSelfLocal_ =
      this->grid_.geometryProvider_.getNewObjectEntity(this->grid_,walkLevel_);
    intersectionNeighborLocal_ =
      this->grid_.geometryProvider_.getNewObjectEntity(this->grid_,walkLevel_);
    bndEntity_ = 
      this->grid_.bndProvider_.getNewObjectEntity( this->grid_ , walkLevel_ );
    first(*el,wLevel);
  } else {
    this->done();
  }
}

template<class GridImp>
inline void ALU3dGridIntersectionIterator<GridImp> :: 
first (ALU3DSPACE HElementType & elem, int wLevel) 
{
  item_  = static_cast<GEOElementType *> (&elem);

  // Get first face
  const GEOFaceType* firstFace = getFace(*item_, index_);
  if (canGoDown(*firstFace)) {
    firstFace = firstFace->down();
  }

  // Store the face in the connector
  setNewFace(*firstFace);

  // * temporary output
  //outputElementInfo();
  //outputFaceInfo();
}

template <class GridImp>
inline void ALU3dGridIntersectionIterator<GridImp>::
initGeometryProvider() const {
  if (!geoProvider_) {
    geoProvider_ = new GeometryInfoType(*connector_);
  }
}

// copy constructor 
template<class GridImp>
inline ALU3dGridIntersectionIterator<GridImp> :: 
ALU3dGridIntersectionIterator(const ALU3dGridIntersectionIterator<GridImp> & org) : 
  ALU3dGridEntityPointer<0,GridImp> (org.grid_ , 
                                     org.walkLevel_ , 
                                     (org.item_) ? false : true ),
  connector_(),
  geoProvider_(org.geoProvider_ ? 
               new GeometryInfoType(*org.geoProvider_) : 0),
  intersectionGlobal_(0),
  intersectionSelfLocal_(0),
  intersectionNeighborLocal_(0),
  item_(org.item_),
  bndEntity_(0), // this is only set if this is not an end iterator
  nFaces_(org.nFaces_),
  walkLevel_(org.walkLevel_),
  generatedGlobalGeometry_(false),
  generatedLocalGeometries_(false)
{
  if(org.item_) { // else it's a end iterator 
    connector_      = FaceInfoPointer(new FaceInfoType(*org.connector_)),
    item_           = org.item_;
    index_          = org.index_;
    intersectionGlobal_ = 
      org.intersectionGlobal_ ? 
      this->grid_.geometryProvider_.getNewObjectEntity(this->grid_, walkLevel_)
      : 0;
    intersectionSelfLocal_ = 
      org.intersectionSelfLocal_ ? 
      this->grid_.geometryProvider_.getNewObjectEntity(this->grid_, walkLevel_)
      : 0;
    intersectionNeighborLocal_ = 
      org.intersectionNeighborLocal_ ? 
      this->grid_.geometryProvider_.getNewObjectEntity(this->grid_, walkLevel_)
      : 0;
    bndEntity_ = 
      (org.bndEntity_) ? 
      this->grid_.bndProvider_.getNewObjectEntity( this->grid_ , walkLevel_ ) 
      : 0;
    
  } else {
    this->done();
  }
}

template<class GridImp>
inline ALU3dGridIntersectionIterator<GridImp> :: 
~ALU3dGridIntersectionIterator() {
  if (intersectionGlobal_) {
    this->grid_.geometryProvider_.freeObjectEntity(intersectionGlobal_);
    intersectionGlobal_ = 0;
  }
  
  if (intersectionSelfLocal_) {
    this->grid_.geometryProvider_.freeObjectEntity(intersectionSelfLocal_);
    intersectionSelfLocal_ = 0;
  }
  
  if (intersectionNeighborLocal_) {
    this->grid_.geometryProvider_.freeObjectEntity(intersectionNeighborLocal_);
    intersectionNeighborLocal_ = 0;
  }

  if(bndEntity_) {
    this->grid_.bndProvider_.freeObjectEntity( bndEntity_ );
    bndEntity_ = 0;
  }

  if (geoProvider_) {
    delete geoProvider_;
    geoProvider_ = 0;
  }
}

template<class GridImp>
inline void ALU3dGridIntersectionIterator<GridImp> :: increment () 
{
  assert(item_);
  
  const GEOFaceType * nextFace = 0;
  
  // When neighbour element is refined, try to get the next child on the face
  if (connector_->conformanceState() == FaceInfoType::REFINED_OUTER) {
    nextFace = connector_->face().next();
      
    // There was a next child face...
    if (nextFace) {
      setNewFace(*nextFace);
      // * temporary output
      //outputFaceInfo();
      return; // we found what we were looking for...
    }
  } // end if

  // Next face number of starting element
  ++index_;
  
  // When the face number is larger than the number of faces an element
  // can have, we've reached the end...
  if (index_ >= nFaces_) {
    delete geoProvider_;
    geoProvider_ = 0;

    this->done();
    return;
  }

  // ... else we can take the next face
  nextFace = getFace(connector_->innerEntity(), index_);
  assert(nextFace);

  // Check whether we need to go down first
  //if (nextFace has children which need to be visited)
  if (canGoDown(*nextFace)) {
    nextFace = nextFace->down();
    assert(nextFace);
  }

  setNewFace(*nextFace);
  // * temporary output
  //outputFaceInfo();
  return;
}
 
template<class GridImp>
inline typename ALU3dGridIntersectionIterator<GridImp>::Entity & 
ALU3dGridIntersectionIterator<GridImp>::dereference () const
{
#ifdef _ALU3DGRID_PARALLEL_
  if(connector_->isGhostBnd())
  {
    BNDFaceType * ghost = const_cast<BNDFaceType *>(&connector_->boundaryFace());
    if( connector_->boundaryFace().level () != connector_->boundaryFace().ghostLevel() ) 
      ghost = static_cast<BNDFaceType *>(ghost->up());
      
    this->entity_->setGhost(const_cast<BNDFaceType &>(connector_->boundaryFace()) );
  }
  else 
#endif
  {
    this->entity_->setElement(const_cast<GEOElementType&>(connector_->outerEntity())); 
  }
  return ALU3dGridEntityPointer<0,GridImp>::dereference();
}

template<class GridImp>
inline bool ALU3dGridIntersectionIterator<GridImp> :: boundary () const
{
  return (connector_->outerBoundary());
}

template<class GridImp>
inline bool ALU3dGridIntersectionIterator<GridImp>::neighbor () const
{
  return !(this->boundary());
}

template<class GridImp>
inline int ALU3dGridIntersectionIterator<GridImp>::numberInSelf () const
{
  assert(ElementTopo::dune2aluFace(index_) == 
         connector_->innerALUFaceIndex());
  return index_;
}


template <class GridImp>
inline const typename ALU3dGridIntersectionIterator<GridImp>::LocalGeometry &
ALU3dGridIntersectionIterator<GridImp>::intersectionSelfLocal() const {
  buildLocalGeometries();
  return *intersectionSelfLocal_;
}

template<class GridImp>
inline int ALU3dGridIntersectionIterator<GridImp>::numberInNeighbor () const
{
  return ElementTopo::alu2duneFace(connector_->outerALUFaceIndex());
}

template <class GridImp>
inline const typename ALU3dGridIntersectionIterator<GridImp>::LocalGeometry &
ALU3dGridIntersectionIterator<GridImp>::intersectionNeighborLocal() const {
  assert(!boundary());
  buildLocalGeometries();
  return *intersectionNeighborLocal_;
}

template<class GridImp>
inline typename ALU3dGridIntersectionIterator<GridImp>::NormalType
ALU3dGridIntersectionIterator<GridImp>::
integrationOuterNormal(const FieldVector<alu3d_ctype, dim-1>& local) const
{
  return this->outerNormal(local);
}

template<class GridImp>
inline typename ALU3dGridIntersectionIterator<GridImp>::NormalType 
ALU3dGridIntersectionIterator<GridImp>::
outerNormal(const FieldVector<alu3d_ctype, dim-1>& local) const
{
  assert(item_ != 0);
  initGeometryProvider();
  return geoProvider_->outerNormal(local);
}
  
template<class GridImp>
inline typename ALU3dGridIntersectionIterator<GridImp>::NormalType 
ALU3dGridIntersectionIterator<GridImp>::
unitOuterNormal(const FieldVector<alu3d_ctype, dim-1>& local) const
{
  NormalType unitOuterNormal_  = this->outerNormal(local);
  unitOuterNormal_ *= (1.0/unitOuterNormal_.two_norm()); 
  return unitOuterNormal_;
}

template<class GridImp>
inline const typename ALU3dGridIntersectionIterator<GridImp>::Geometry &
ALU3dGridIntersectionIterator<GridImp>::intersectionGlobal () const
{
  buildGlobalGeometry();
  return *intersectionGlobal_;
}

template<class GridImp>
inline const typename ALU3dGridIntersectionIterator<GridImp>::BoundaryEntity &
ALU3dGridIntersectionIterator<GridImp>::boundaryEntity () const
{
  assert(boundary());
  assert(item_); // make sure that this is not an end iterator
  const BNDFaceType& bnd = connector_->boundaryFace();
  int id = bnd.bndtype(); // ids are positive
  bndEntity_->setId( -id );
  return *bndEntity_;
}

template <class GridImp>
void ALU3dGridIntersectionIterator<GridImp>::buildGlobalGeometry() const {
  initGeometryProvider();
  intersectionGlobal_->buildGeom(geoProvider_->intersectionGlobal());
  generatedGlobalGeometry_ = true;
}

template <class GridImp>
void ALU3dGridIntersectionIterator<GridImp>::buildLocalGeometries() const {
  initGeometryProvider();
  intersectionSelfLocal_->buildGeom(geoProvider_->intersectionSelfLocal());
  if (!connector_->outerBoundary()) {
    intersectionNeighborLocal_->buildGeom(geoProvider_->intersectionNeighborLocal());
  }
  generatedLocalGeometries_ = true;
}

template <class GridImp>
inline const ALU3dImplTraits<tetra>::GEOFaceType*
ALU3dGridIntersectionIterator<GridImp>::
getFace(const ALU3DSPACE GEOTetraElementType& elem, int index) const {
  assert(index >= 0 && index < numFaces);
  return elem.myhface3(ElementTopo::dune2aluFace(index));
}

template <class GridImp>
inline const ALU3dImplTraits<hexa>::GEOFaceType*
ALU3dGridIntersectionIterator<GridImp>::
getFace(const ALU3DSPACE GEOHexaElementType& elem, int index) const {
  assert(index >= 0 && index < numFaces);
  return elem.myhface4(ElementTopo::dune2aluFace(index));
}

template <class GridImp>
void ALU3dGridIntersectionIterator<GridImp>::
setNewFace(const GEOFaceType& newFace) {
  connector_ = 
    FaceInfoPointer(new FaceInfoType(newFace, 
                                     item_->twist(ElementTopo::dune2aluFace(index_))));
 
  if (geoProvider_) {
    delete geoProvider_;
    geoProvider_ = 0;
  }
}

template <class GridImp>
bool ALU3dGridIntersectionIterator<GridImp>::
canGoDown(const GEOFaceType& nextFace) const {
  return (item_->level() < walkLevel_ && item_->leaf() && nextFace.down());
}

template <class GridImp>
void ALU3dGridIntersectionIterator<GridImp>::outputElementInfo() const {
  std::cout << "Starting outputElementInfo\n";
  // output element corner coordinates
  std::cout << "Element corner coordinates" << std::endl;
  for (int i = 0; i < numVertices; ++i) {
    printToScreen(ElementTopo::alu2duneVertex(i), i, 
                  convert2FV(item_->myvertex(i)->Point()));
  }
  
  // output midpoint of faces
  /* too specific for hexa
  std::cout << "Midpoint of faces" << std::endl;
  FieldVector<alu3d_ctype, 2> mid(0.5);
  for (int i = 0; i < numFaces; ++i) {
    NormalType c0 = 
      convert2FV(item_->myvertex(i, FaceTopo::dune2aluVertex(0))->Point());
    NormalType c1 = 
      convert2FV(item_->myvertex(i, FaceTopo::dune2aluVertex(1))->Point());
    NormalType c2 = 
      convert2FV(item_->myvertex(i, FaceTopo::dune2aluVertex(2))->Point());
    NormalType c3 = 
      convert2FV(item_->myvertex(i, FaceTopo::dune2aluVertex(3))->Point());

    BilinearSurfaceMapping biMap(c0, c1, c2, c3);
    NormalType result;
    biMap.map2world(mid, result);

    printToScreen(ElementTopo::alu2duneFace(i), i, result);
  } // end for

  // output element reference faces - as calculated in Dune
  std::cout << "Corner indices (from ReferenceTopologySet)" << std::endl;
  for (int i = 0; i < numFaces; ++i) {
    std::cout << "-- Face " << i << "--\n";
    const int* result;
    int n;
    ReferenceTopologySet::getSubEntities< 1, 3 >(hexahedron,
                                                 i,
                                                 result,
                                                 n);
    for (int j = 0; j < n; ++j) {
      printToScreen(result[j], ElementTopo::dune2aluVertex(result[j]), j);
    } // end for
  } // end for
  */
  // output element reference faces - compared with data from Alu
  std::cout << "Corner indices (from ALU)" << std::endl;
  for (int i = 0; i < numFaces; ++i) {
    std::cout << "-- Face " << i << "--\n";
    for (int j = 0; j < numVerticesPerFace; ++j) {
      int aluIdx = GEOElementType::prototype[i][j];
      printToScreen(ElementTopo::alu2duneVertex(aluIdx), aluIdx, j);
    }
  }

  std::cout << "Ending outputElementInfo\n" << std::endl;
}

template <class GridImp>
void  ALU3dGridIntersectionIterator<GridImp>::outputFaceInfo() const {
  std::cout << "Starting outputFaceInfo\n";
  // output face index (inner, outer)
  std::cout << "Inner twist" << std::endl;
  printToScreen(index_, 
                connector_->innerALUFaceIndex(),
                connector_->innerTwist());

  assert(index_ == ElementTopo::alu2duneFace(connector_->innerALUFaceIndex()));
  
  std::cout << "Outer twist" << std::endl;
  printToScreen(-1,
                connector_->outerALUFaceIndex(),
                connector_->outerTwist());
  
  // output corner coordinates
  std::cout << "Face corner coordinates (ALU face)" << std::endl;
  const GEOFaceType& face = connector_->face();
  for (int i = 0; i < numVerticesPerFace; ++i) {
    printToScreen(FaceTopo::alu2duneVertex(i), i, 
                  convert2FV(face.myvertex(i)->Point()));
  }

  std::cout << "Face corner coordinates (intersectionGlobal)" << std::endl;
  const Geometry& interGlobal = intersectionGlobal();
  for (int i = 0; i < numVerticesPerFace; ++i) {
    printToScreen(i, FaceTopo::dune2aluVertex(i), interGlobal[i]);
  }

  std::cout << "Ending outputFaceInfo\n" << std::endl;
}

template <class GridImp>
void ALU3dGridIntersectionIterator<GridImp>::
printToScreen(int duneIdx, int aluIdx) const {
  printToScreen(duneIdx, aluIdx, "-");
}

template <class GridImp>
template <typename T>
void ALU3dGridIntersectionIterator<GridImp>::
printToScreen(int duneIdx, int aluIdx, const T& info) const {
  std::cout << duneIdx << ", " << aluIdx << ": " << info << "\n";
}

template <class GridImp>
typename ALU3dGridIntersectionIterator<GridImp>::NormalType
ALU3dGridIntersectionIterator<GridImp>::
convert2FV(const alu3d_ctype (&p)[3]) const {
  NormalType result;
  result[0] = p[0];
  result[1] = p[1];
  result[2] = p[2];
  return result;
}


/*************************************************************************
 #       ######  #    #  ######  #          #     #####  ######  #####
 #       #       #    #  #       #          #       #    #       #    #
 #       #####   #    #  #####   #          #       #    #####   #    #
 #       #       #    #  #       #          #       #    #       #####
 #       #        #  #   #       #          #       #    #       #   #
 ######  ######    ##    ######  ######     #       #    ######  #    #
*************************************************************************/
//--LevelIterator 
template<int codim, PartitionIteratorType pitype, class GridImp >
inline ALU3dGridLevelIterator<codim,pitype,GridImp> :: 
  ALU3dGridLevelIterator(const GridImp & grid, int level,bool end)
  : ALU3dGridEntityPointer<codim,GridImp> ( grid ,level,end)
  , index_(-1) 
  , level_(level)
{
  if(!end) 
  {
    IteratorType * it = new IteratorType ( this->grid_ , level_ );
    iter_.store( it );
    
    (*iter_).first();
    if(!(*iter_).done())
    {
      assert((*iter_).size() > 0);
      index_=0;
      myEntity().reset( level_ );
      myEntity().setElement( (*iter_).item()); 
    }
  }
  else
    this->done();
}

template<int codim, PartitionIteratorType pitype, class GridImp >
inline ALU3dGridLevelIterator<codim,pitype,GridImp> :: 
  ALU3dGridLevelIterator(const ALU3dGridLevelIterator<codim,pitype,GridImp> & org ) 
  : ALU3dGridEntityPointer<codim,GridImp> ( org.grid_,org.level_,(org.index_ < 0) ? true : false )
  , index_( org.index_ ) 
  , level_( org.level_ )
  , iter_ ( org.iter_ )
{
  if(index_ >= 0)
  {
    myEntity().reset( level_ );
    myEntity().setElement( (*iter_).item());
  }
}

template<int codim, PartitionIteratorType pitype, class GridImp >
inline void ALU3dGridLevelIterator<codim,pitype,GridImp> :: increment () 
{
  // if assertion is thrown then end iterator was forgotten or didnt stop 
  assert(index_ >= 0);

  (*iter_).next();
  index_++;
  if ((*iter_).done()) 
  {
    index_ = -1; 
    this->done();
    return ;
  }
   
  myEntity().setElement( (*iter_).item()); 
  return ;
}

//*******************************************************************
//
//  LEAFITERATOR 
//
//--LeafIterator 
//*******************************************************************
template<class GridImp>
inline ALU3dGridLeafIterator<GridImp> :: 
  ALU3dGridLeafIterator(const GridImp &grid, int level, 
      bool end, const int nlinks, PartitionIteratorType pitype) 
  : ALU3dGridEntityPointer <0,GridImp> ( grid,level,end) 
  , index_(-1) 
  , level_(level)
  , pitype_ (pitype)
{
  if(!end) 
  {
#ifdef _ALU3DGRID_PARALLEL_
    if(pitype == Ghost_Partition) 
    {
      std::cout << "Erstelle GhostIterator \n";
      typedef ALU3DSPACE ALU3dGridLeafIteratorWrapper<0,Ghost_Partition> GhostIterator; 
      IterInterface * it = new GhostIterator ( this->grid_, level_, nlinks );  
      iter_.store( it );
    }
    else if(pitype == All_Partition) 
    {
      std::cout << "Erstelle All Partition Iterator! \n";
      typedef ALU3DSPACE ALU3dGridLeafIteratorWrapper<0,All_Partition> AllIterator; 
      IterInterface * it = new AllIterator ( this->grid_, level_, nlinks );  
      iter_.store( it );
    }
    else 
#endif
    {
      IteratorType * it = new IteratorType ( this->grid_ , level_ );
      iter_.store( it );
    }
    
    (*iter_).first();
    if(!(*iter_).done()) // else iterator empty
    {
      assert((*iter_).size() > 0);
      index_=0;
      myEntity().reset( level_ );
      ALU3DSPACE LeafValType & item = (*iter_).item();
      //myEntity().setElement( (*iter_).item());
      if( item.first )
        myEntity().setElement( * item.first );
      else 
        myEntity().setGhost( * item.second );
    }
  }
  else 
    this->done();
}

template<class GridImp>
inline ALU3dGridLeafIterator<GridImp> ::
ALU3dGridLeafIterator(const ALU3dGridLeafIterator<GridImp> &org)
 : ALU3dGridEntityPointer <0,GridImp> ( org.grid_,org.level_,(org.index_ < 0) ? true : false )
 , index_(org.index_) 
 , level_(org.level_)
 , iter_ ( org.iter_ ) 
 , pitype_(org.pitype_)
{ 
  if(index_ >= 0)
  {
    myEntity().reset( level_ );
    ALU3DSPACE LeafValType & item = (*iter_).item();
    if( item.first )
      myEntity().setElement( * item.first );
    else 
      myEntity().setGhost( * item.second );
    //myEntity().setElement( iter_->item() );
  }
}

template<class GridImp> 
inline void ALU3dGridLeafIterator<GridImp> :: increment () 
{
  // if assertion is thrown then end iterator was forgotten or didnt stop 
  assert(index_  >= 0);

  (*iter_).next();
  index_++;
  
  if((*iter_).done()) 
  {
    index_ = -1;
    this->done(); 
    return ;
  }
   
  ALU3DSPACE LeafValType & item = (*iter_).item();
  //myEntity().setElement( (*iter_).item());
  if( item.first )
    myEntity().setElement( * item.first );
  else 
    myEntity().setGhost( * item.second );
  return ;
}

/************************************************************************************
#     #
#     #     #    ######  #####      #     #####  ######  #####
#     #     #    #       #    #     #       #    #       #    #
#######     #    #####   #    #     #       #    #####   #    #
#     #     #    #       #####      #       #    #       #####
#     #     #    #       #   #      #       #    #       #   #
#     #     #    ######  #    #     #       #    ######  #    #
************************************************************************************/
// --HierarchicIterator 
template <class GridImp>
inline ALU3dGridHierarchicIterator<GridImp> :: 
  ALU3dGridHierarchicIterator(const GridImp & grid , 
   const ALU3DSPACE HElementType & elem, int maxlevel ,bool end)
  : ALU3dGridEntityPointer<0,GridImp> ( grid, elem ) 
  , elem_(elem) , item_(0) , maxlevel_(maxlevel) 
{
  if (!end) 
  {
    item_ = const_cast<ALU3DSPACE HElementType *> (this->elem_.down());
    if(item_) 
    {
      // we have children and they lie in the disired level range 
      if(item_->level() <= maxlevel_)
      {
        myEntity().reset( maxlevel_ );
        myEntity().setElement(*item_);
      }
      else 
      { // otherwise do nothing 
        item_ = 0;
        this->done();
      }
    }
    else 
    {
      this->done();
    }
  }
}

template <class GridImp>
inline ALU3dGridHierarchicIterator<GridImp> :: 
ALU3dGridHierarchicIterator(const ALU3dGridHierarchicIterator<GridImp> & org)
  : ALU3dGridEntityPointer<0,GridImp> (org.grid_,org.elem_)
  , elem_ (org.elem_) , item_(org.item_) , maxlevel_(org.maxlevel_) 
{
  if(item_) 
  {
    myEntity().reset( maxlevel_ );
    myEntity().setElement(*item_);
  }
  else 
    this->done();
}

template <class GridImp>
inline ALU3DSPACE HElementType * ALU3dGridHierarchicIterator<GridImp>::
goNextElement(ALU3DSPACE HElementType * oldelem ) 
{
  // strategy is:
  // - go down as far as possible and then over all children 
  // - then go to father and next and down again 
  
  ALU3DSPACE HElementType * nextelem = oldelem->down();
  if(nextelem)
  {
    if(nextelem->level() <= maxlevel_)
      return nextelem;
  }
    
  nextelem = oldelem->next();
  if(nextelem) 
  {
    if(nextelem->level() <= maxlevel_) 
      return nextelem;
  }

  nextelem = oldelem->up();
  if(nextelem == &elem_) return 0;

  while( !nextelem->next() )
  {
    nextelem = nextelem->up();
    if(nextelem == &elem_) return 0;
  }

  if(nextelem) nextelem = nextelem->next();

  return nextelem;
}

template <class GridImp>
inline void ALU3dGridHierarchicIterator<GridImp> :: increment () 
{
  assert(item_   != 0);

  item_ = goNextElement( item_ );
  if(!item_) 
  {
    this->done();
    return ;
  }

  myEntity().setElement(*item_);
  return ;
}

} // end namespace Dune
