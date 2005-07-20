//************************************************************************
//
//  implementation of AlbertaGrid 
//
//  namespace Dune
//
//************************************************************************

namespace Dune
{

// because of gcc bug 14479 
#ifdef HAVE_ICC
#define TEMPPARAM2
#endif

static ALBERTA EL_INFO statElInfo[DIM+1];  

// singleton holding reference elements
template<int dim, class GridImp>
struct AlbertaGridReferenceGeometry
{
  AlbertaGridMakeableGeometry<dim,dim,GridImp> refelem;
  AlbertaGridReferenceGeometry () : refelem (true) {};
};

//****************************************************************
//
// --AlbertaGridGeometry 
// --Geometry 
//
//****************************************************************

//****************************************************************
//
//  specialization of mapVertices 
//
//****************************************************************
template <int md, int cd>
struct MapVertices
{
  static int mapVertices (int i, int face, int edge, int vertex)
  {
    return i;
  }
};

// faces in 2d 
// which vertices belong to which face
static const int localTriangleFaceNumber [3][2] = { {1,2} , {2,0} , {0,1} };
template <>
struct MapVertices<1,2>
{
  static int mapVertices (int i, int face, int edge, int vertex)
  {
    return localTriangleFaceNumber[face][i];
  }
};

// faces in 3d 
template <>
struct MapVertices<2,3>
{
  static int mapVertices (int i, int face, int edge, int vertex)
  {
    return ALBERTA AlbertHelp::localTetraFaceNumber[face][i];
  }
};

// edges in 3d 
// local numbers of vertices belonging to one edge 
static const int localEdgeNumber [6][2] = 
{ 
  {1,2} , {2,0} , {0,1} , // first three vertices like in 2d for faces(edges)
  {0,3} , {1,3} , {2,3}  // then all with the last vertex 
};
template <>
struct MapVertices<1,3>
{
  static int mapVertices (int i, int face, int edge, int vertex)
  {
    return localEdgeNumber[edge][i];
  }
};

// vertices in 2d and 3d 
template <int cd>
struct MapVertices<0,cd>
{
  static int mapVertices (int i, int face, int edge, int vertex)
  {
    return vertex;
  }
};

// default, do nothing 
template <int mydim, int cdim, class GridImp>
inline int AlbertaGridGeometry<mydim,cdim,GridImp>::mapVertices (int i) const  
{ 
  // there is a specialisation for each combination of mydim and coorddim 
  return MapVertices<mydim,cdim>::mapVertices(i,face_,edge_,vertex_); 
}

template <int mydim, int cdim, class GridImp>
inline AlbertaGridGeometry<mydim,cdim,GridImp>:: 
AlbertaGridGeometry(bool makeRefGeometry)
{
  // make empty element 
  initGeom();

  // make coords for reference elements, spezial for different dim
  if(makeRefGeometry)
    makeRefElemCoords();
}

template <int mydim, int cdim, class GridImp>
inline ALBERTA EL_INFO * AlbertaGridGeometry<mydim,cdim,GridImp>:: 
makeEmptyElInfo()
{
  ALBERTA EL_INFO * elInfo = &statElInfo[mydim]; 
  
  elInfo->mesh = 0;
  elInfo->el = 0;
  elInfo->parent = 0;
  elInfo->macro_el = 0;
  elInfo->level = 0;
#if DIM > 2
  elInfo->orientation = 0;
  elInfo->el_type = 0;
#endif
  
  for(int i =0; i<mydim+1; i++)
  {
    for(int j =0; j< cdim; j++)
    {
      elInfo->coord[i][j] = 0.0;
      elInfo->opp_coord[i][j] = 0.0;
    }
    elInfo->bound[i] = 0;
  }
  return elInfo;
}


template <> 
inline void AlbertaGridGeometry<2,2,const AlbertaGrid<2,2> >:: 
makeRefElemCoords()
{
  // make empty elInfo 
  elInfo_ = makeEmptyElInfo();

//*****************************************************************
//!   
/*!
   Dune reference element triangles (2d) 
   
    (0,1)
     2|\    coordinates and local node numbers 
      | \
      |  \    
     1|   \0
      |    \
      |     \
     0|______\1
    (0,0) 2  (1,0)
*/           
//*****************************************************************
 
  // set reference coordinates 
  coord_ = 0.0;

  // vertex 1 
  coord_[1][0] = 1.0; 

  // vertex 2
  coord_[2][1] = 1.0;
}

template <>
inline void AlbertaGridGeometry<3,3,const AlbertaGrid<3,3> >:: 
makeRefElemCoords()
{
  //! make ReferenzGeometry as default 
  elInfo_ = makeEmptyElInfo();
//*****************************************************************
//
//! reference element 3d
//!        z
//!        |     y
//        3|    /
// (0,0,1) |   /2 (0,1,0)
//         |  /
//         | /  
//         |/      (1,0,0)
//       0 ------------x
//      (0,0,0)     1
//!  
//
//*****************************************************************

  // set coordinates 
  coord_ = 0.0;

  // vertex 1 
  coord_[1][0] = 1.0; 
  
  // vertex 2 
  coord_[2][1] = 1.0;

  // vertex 3 
  coord_[3][2] = 1.0;
}
template <> 
inline void AlbertaGridGeometry<1,1,const AlbertaGrid<1,1> >:: 
makeRefElemCoords()
{
  //! make  Referenz Geometry as default
  elInfo_ = makeEmptyElInfo();

  // set reference coordinates 
  coord_ = 0.0;

  // vertex 1
  coord_[1] = 1.0;
}

template <int mydim, int cdim, class GridImp>
inline void AlbertaGridGeometry<mydim,cdim,GridImp>:: 
makeRefElemCoords()
{
  DUNE_THROW(AlbertaError, "No default implementation for this AlbertaGridGeometry!");
}

template <int mydim, int cdim, class GridImp>
inline void AlbertaGridGeometry<mydim,cdim,GridImp>:: 
initGeom()
{
  elInfo_ = 0;
  face_ = 0;
  edge_ = 0;
  vertex_ = 0;
  builtinverse_ = false;
  builtElMat_   = false;
  calcedDet_    = false;
}

// built Geometry 
template <int mydim, int cdim, class GridImp>
inline bool AlbertaGridGeometry<mydim,cdim,GridImp>:: 
builtGeom(ALBERTA EL_INFO *elInfo, int face, 
          int edge, int vertex)
{
  elInfo_ = elInfo;
  face_ = face;
  edge_ = edge;
  vertex_ = vertex;
  elDet_ = 0.0;
  builtinverse_ = false;
  builtElMat_   = false;
  calcedDet_    = false;

  if(elInfo_)
  {
    for(int i=0; i<mydim+1; i++)
    {
      const ALBERTA REAL_D & elcoord = elInfo_->coord[mapVertices(i)];
      for(int j=0; j<cdim; j++)
        coord_[i][j] = elcoord[j];
    }
    // geometry built 
    return true;
  }
  // geometry not built 
  return false;
}


// specialization yields speed up, because vertex_ .. is not copied 
template <>
inline bool AlbertaGridGeometry<2,2,const AlbertaGrid<2,2> >:: 
builtGeom(ALBERTA EL_INFO *elInfo, int face, 
          int edge, int vertex)
{
  enum { dim = 2 }; 
  enum { dimworld = 2 }; 

  elInfo_ = elInfo;
  elDet_ = 0.0;
  builtinverse_ = false;
  builtElMat_   = false;
  calcedDet_    = false;

  if(elInfo_)
  {
    for(int i=0; i<dim+1; i++)
    {
      const ALBERTA REAL_D & elcoord = elInfo_->coord[mapVertices(i)];
      for(int j=0; j<dimworld; j++)
        coord_[i][j] = elcoord[j];
    }
    // geometry built 
    return true;
  }
  // geometry not built 
  return false;
}

template <>
inline bool AlbertaGridGeometry<3,3,const AlbertaGrid<3,3> >:: 
builtGeom(ALBERTA EL_INFO *elInfo, int face, 
          int edge, int vertex)
{
  enum { dim = 3 }; 
  enum { dimworld = 3 }; 
  
  elInfo_ = elInfo;
  elDet_ = 0.0;
  builtinverse_ = false;
  builtElMat_   = false;
  calcedDet_    = false;
  
  if(elInfo_)
  {
    for(int i=0; i<dim+1; i++)
    {
      const ALBERTA REAL_D & elcoord = elInfo_->coord[mapVertices(i)];
      for(int j=0; j<dimworld; j++)
        coord_[i][j] = elcoord[j];
    }
    // geometry built 
    return true;
  }
  // geometry not built 
  return false;
}


// print the GeometryInformation
template <int mydim, int cdim, class GridImp>
inline void AlbertaGridGeometry<mydim,cdim,GridImp>::print (std::ostream& ss, int indent) const
{
  ss << "AlbertaGridGeometry<" << mydim << "," << cdim << "> = {\n";
  for(int i=0; i<corners(); i++)
  {
    ss << " corner " << i; 
    ss << ((*this)[i]); ss << "\n";
  }
  ss << "} \n";
}


template <int dim> struct AlbertaGridGeomType {
  static GeometryType type () { return unknown; }
};
template <> struct AlbertaGridGeomType<1> {
  static GeometryType type () { return line; }
};
template <> struct AlbertaGridGeomType<2> {
  static GeometryType type () { return triangle; }
};
template <> struct AlbertaGridGeomType<3> {
  static GeometryType type () { return tetrahedron; }
};

template <int mydim, int cdim, class GridImp>
inline GeometryType AlbertaGridGeometry<mydim,cdim,GridImp>::type() const
{
  return AlbertaGridGeomType<mydim>::type();
}

template <int mydim, int cdim, class GridImp>
inline int AlbertaGridGeometry<mydim,cdim,GridImp>::corners() const
{
  return (mydim+1);
}

///////////////////////////////////////////////////////////////////////
template <int mydim, int cdim, class GridImp>
inline const FieldVector<albertCtype, cdim>& AlbertaGridGeometry<mydim,cdim,GridImp>:: 
operator [](int i) const
{
  return coord_[i];
}

///////////////////////////////////////////////////////////////////////
template <int mydim, int cdim, class GridImp>
inline FieldVector<albertCtype, cdim>& AlbertaGridGeometry<mydim,cdim,GridImp>:: 
getCoordVec (int i) 
{
  return coord_[i];
}

template <class GridImp, int dim> struct AlbertaGridRefElem;
template <class GridImp> struct AlbertaGridRefElem<GridImp,1> {
  static const Dune::Geometry<1,1,GridImp,Dune::AlbertaGridGeometry> & refelem () 
  { 
    static AlbertaGridReferenceGeometry<1,GridImp> ref;
    return ref.refelem; 
  }
};

template <class GridImp> struct AlbertaGridRefElem<GridImp,2> {
  static const Dune::Geometry<2,2,GridImp,Dune::AlbertaGridGeometry> & refelem () 
  { 
    static AlbertaGridReferenceGeometry<2,GridImp> ref;
    return ref.refelem; 
  }
};
template <class GridImp> struct AlbertaGridRefElem<GridImp,3> {
  static const Dune::Geometry<3,3,GridImp,Dune::AlbertaGridGeometry> & refelem () 
  { 
    static AlbertaGridReferenceGeometry<3,GridImp> ref;
    return ref.refelem; 
  }
};

template <int mydim, int cdim, class GridImp>
inline const Dune::Geometry<mydim,mydim,GridImp,Dune::AlbertaGridGeometry> & 
AlbertaGridGeometry<mydim,cdim,GridImp>::refelem()
{
  return AlbertaGridRefElem<GridImp,mydim>::refelem();
}


template <class GridImp, int mydim, int cdim>
struct CalcElementMatrix
{
  enum { matdim = (mydim > 0) ? mydim : 1 }; 
  static bool calcElMatrix(const FieldMatrix<albertCtype,mydim+1,cdim> & coord,
                           FieldMatrix<albertCtype,matdim,matdim> & elMat)
  {
    char text [1024];
    sprintf(text,"AlbertaGridGeometry<%d,%d>::calcElMatrix: No default implementation",mydim,cdim);
    DUNE_THROW(AlbertaError, text);
    return false;
  }
};

template <class GridImp>
struct CalcElementMatrix<GridImp,2,2>
{
  enum { mydim  = 2 };
  enum { cdim   = 2 };
  enum { matdim = 2 }; 
  static bool calcElMatrix(const FieldMatrix<albertCtype,mydim+1,cdim> & coord,
                           FieldMatrix<albertCtype,matdim,matdim> & elMat)
  {
    //       column 0 , column 1
    // A = ( P1 - P0  , P2 - P0 )
    for (int i=0; i<mydim; i++) 
    {
      elMat[i][0] = coord[1][i] - coord[0][i];
      elMat[i][1] = coord[2][i] - coord[0][i];
    }
    return true;
  }
};

template <class GridImp>
struct CalcElementMatrix<GridImp,3,3>
{
  enum { mydim  = 3 };
  enum { cdim   = 3 };
  enum { matdim = 3 }; 
  static bool calcElMatrix(const FieldMatrix<albertCtype,mydim+1,cdim> & coord,
                           FieldMatrix<albertCtype,matdim,matdim> & elMat)
  {
    const FieldVector<albertCtype, cdim> & coord0 = coord[0];
    for(int i=0 ;i<mydim; i++)
    {
      elMat[i][0] = coord[1][i] - coord0[i];
      elMat[i][1] = coord[2][i] - coord0[i];
      elMat[i][2] = coord[3][i] - coord0[i];
    }
    return true;
  }
};

template <int mydim, int cdim, class GridImp>
inline void AlbertaGridGeometry<mydim,cdim,GridImp>::calcElMatrix () const
{
  // build mapping from reference element to actual element 
  builtElMat_ = CalcElementMatrix<GridImp,mydim,cdim>::calcElMatrix(coord_,elMat_);
}

template <int mydim, int cdim, class GridImp>
inline FieldVector<albertCtype, cdim> AlbertaGridGeometry<mydim,cdim,GridImp>:: 
global(const FieldVector<albertCtype, mydim>& local) const
{
  // 1hecked, works  
  
  // we calculate interal in barycentric coordinates  
  // fake the third local coordinate via localFake
  albertCtype c = local[0];
  albertCtype localFake=1.0-c;
 
  // the initialize 
  // note that we have to swap the j and i 
  // in coord(j,i) means coord_(i)(j)  
  for(int j=0; j<cdim; j++)
    globalCoord_[j] = c * coord_[1][j];
      
  // for all local coords 
  for (int i = 1; i < mydim; i++)
  {
    c = local[i];
    localFake -= c;
    for(int j=0; j<cdim; j++)
      globalCoord_[j] += c * coord_[i+1][j];
  }

  // for the last barycentric coord 
  for(int j=0; j<cdim; j++)
    globalCoord_[j] += localFake * coord_[0][j];

  return globalCoord_;
}

// uses the element matrix, because faster 
template<>
inline FieldVector<albertCtype, 2> AlbertaGridGeometry<2,2,const AlbertaGrid<2,2> >:: 
global(const FieldVector<albertCtype, 2>& local) const
{
  calcElMatrix();
  
  globalCoord_ = coord_[0];
  elMat_.umv(local,globalCoord_);
  
  return globalCoord_; 
}

template<>
inline FieldVector<albertCtype, 3> AlbertaGridGeometry<3,3,const AlbertaGrid<3,3> >:: 
global(const FieldVector<albertCtype, 3>& local) const
{
  calcElMatrix();
  
  globalCoord_ = coord_[0];
  elMat_.umv(local,globalCoord_);
  return globalCoord_; 
}


template <int mydim, int cdim, class GridImp>
inline FieldVector<albertCtype, mydim> AlbertaGridGeometry<mydim,cdim,GridImp>:: 
local(const FieldVector<albertCtype, cdim>& global) const
{
  char text [1024]; 
  assert(false);
  sprintf(text,"AlbertaGridGeometry<%d,%d>::local: dim != dimworld not implemented!",mydim,cdim);
  DUNE_THROW(AlbertaError, text);
  
  localCoord_ = 0.0;
  return localCoord_; 
}

template <>
inline FieldVector<albertCtype, 2> AlbertaGridGeometry<2,2,const AlbertaGrid<2,2> >:: 
local(const FieldVector<albertCtype, 2>& global) const
{
  if(!builtinverse_)
    buildJacobianInverse();
  
  enum { dim = 2 };
  
  for(int i=0; i<dim; i++) 
    globalCoord_[i] = global[i] - coord_[0][i];

  FMatrixHelp::multAssign(Jinv_,globalCoord_,localCoord_);
  
  return localCoord_; 
}

template <>
inline FieldVector<albertCtype, 3> AlbertaGridGeometry<3,3,const AlbertaGrid<3,3> >:: 
local(const FieldVector<albertCtype, 3>& global) const
{
  if(!builtinverse_)
    buildJacobianInverse();
  enum { dim = 3 };
  
  for(int i=0; i<dim; i++) 
    globalCoord_[i] = global[i] - coord_[0][i];

  FMatrixHelp::multAssign(Jinv_,globalCoord_,localCoord_);
  
  return localCoord_; 
}


// this method is for (dim==dimworld) = 2 and 3 
template <int mydim, int cdim, class GridImp>
inline void AlbertaGridGeometry<mydim,cdim,GridImp>:: 
buildJacobianInverse() const
{
  //******************************************************
  //
  //  the mapping is: 
  //
  //  F(T) = D where T is the reference element
  //  and D the actual element 
  //
  //  F(x) = A * x + b    with   A := ( P_0 , P_1 ) 
  //
  //  A consist of the column vectors P_0 and P_1 and 
  //  is calculated by the method calcElMatrix 
  //
  //******************************************************
 
  // calc A and stores it in elMat_  
  calcElMatrix();
  
  // Jinv = A^-1
  assert( builtElMat_ == true );
  elDet_ = std::abs( FMatrixHelp::invertMatrix(elMat_,Jinv_) );

  assert(elDet_ > 1.0E-25);
  calcedDet_ = true;
  builtinverse_ = true;
  return;
}

// calc volume of face of tetrahedron
template <>
inline void AlbertaGridGeometry<2,3,AlbertaGrid<2,3> >:: 
buildJacobianInverse() const
{
  enum { dim = 2 };
  enum { dimworld = 3 };
  
  // is faster than the lower method 
  DUNE_THROW(AlbertaError,"buildJacobianInverse<2,3> not correctly implemented!"); 
  elDet_ = 0.1; 
  builtinverse_ = true;
  calcedDet_ = true;
}

template <>
inline void AlbertaGridGeometry<1,2,const AlbertaGrid<1,2> >:: 
buildJacobianInverse() const
{
  DUNE_THROW(AlbertaError,"this method is not implemented!\n");
  
  // volume is length of edge 
  //FieldVector<albertCtype, 2> vec = coord_[0] - coord_[1];
  //elDet_ = vec.two_norm(); 

  //calcedDet_ = true;
  //builtinverse_ = true;
}

// default implementation calls ALBERTA routine 
template <int mydim, int cdim, class GridImp>
inline albertCtype AlbertaGridGeometry<mydim,cdim,GridImp>::elDeterminant () const
{
  DUNE_THROW(AlbertaError,"this method is not implemented !\n");
  return 0.0;
}

// determinat of one Geometry, here line  
template <> 
inline albertCtype AlbertaGridGeometry<1,2,const AlbertaGrid<2,2> >::elDeterminant () const
{
  // volume is length of edge 
  tmpZ_ = coord_[0] - coord_[1];
  return std::abs ( tmpZ_.two_norm() );
}

// determinat of one Geometry, here triangle  
template <> 
inline albertCtype AlbertaGridGeometry<2,2,const AlbertaGrid<2,2> >::elDeterminant () const
{
  calcElMatrix();
  return std::abs ( elMat_.determinant () );
}

// determinat of one Geometry, here triangle in 3d   
template <> 
inline albertCtype AlbertaGridGeometry<2,3,const AlbertaGrid<3,3> >::elDeterminant () const
{
  enum { dim = 3 };
  
  // create vectors of face 
  tmpV_ = coord_[1] - coord_[0]; 
  tmpU_ = coord_[2] - coord_[1]; 

  // calculate scaled outer normal 
  for(int i=0; i<dim; i++) 
  {
    tmpZ_[i] = (  tmpU_[(i+1)%dim] * tmpV_[(i+2)%dim] 
                - tmpU_[(i+2)%dim] * tmpV_[(i+1)%dim] );
  }

  // tmpZ is the same as 2.0 * the outer normal 
  return std::abs( tmpZ_.two_norm() );
}

// volume of one Geometry, here therahedron  
template <> 
inline albertCtype AlbertaGridGeometry<3,3,const AlbertaGrid<3,3> >::elDeterminant () const
{
  calcElMatrix();
  return std::abs ( elMat_.determinant () );
}
  
template <int mydim, int cdim, class GridImp>
inline albertCtype AlbertaGridGeometry<mydim,cdim,GridImp>:: 
integrationElement (const FieldVector<albertCtype, mydim>& local) const
{
  // if inverse was built, volume was calced already 
  if(calcedDet_)
    return elDet_; 

  elDet_ = elDeterminant();
  calcedDet_ = true;
  return elDet_;
}

template <int mydim, int cdim, class GridImp>
inline const FieldMatrix<albertCtype,mydim,mydim>& AlbertaGridGeometry<mydim,cdim,GridImp>:: 
jacobianInverse (const FieldVector<albertCtype, cdim>& global) const
{
  if(builtinverse_)
    return Jinv_;

  // builds the jacobian inverse and calculates the volume 
  buildJacobianInverse();
  return Jinv_;
}

//************************************************************************
//  checkMapping and checkInverseMapping are for checks of Jinv_
//************************************************************************
template <int mydim, int cdim, class GridImp>
inline bool AlbertaGridGeometry<mydim,cdim,GridImp>::checkInverseMapping (int loc) const
{
  DUNE_THROW(AlbertaError,"AlbertaGridGeometry::checkInverseMapping: no default implemantation!");
  return false;
}

template <>
inline bool AlbertaGridGeometry<2,2,const AlbertaGrid<2,2> >::checkInverseMapping (int loc) const
{
  // checks if F^-1 (x_i) == xref_i  
  enum { dim =2 };
  
  const FieldVector<albertCtype, dim> & coord    = coord_[loc];
  const FieldVector<albertCtype, dim> & refcoord = refelem()[loc];
  buildJacobianInverse();
  
  FieldVector<albertCtype, dim> tmp3 = coord - coord_[0];     
  FieldVector<albertCtype, dim> tmp2(0.0);
  
  Jinv_.umv(tmp3,tmp2);

  for(int j=0; j<dim; j++)
    if(std::abs(tmp2[j] - refcoord[j]) > 1e-15)
    {
      std::cout << "AlbertaGridGeometry<2,2,AlbertaGrid<2,2> >::checkInverseMapping: Mapping of coord " << loc << " incorrect! \n";
      return false;
    }
  return true;
}

template <>
inline bool AlbertaGridGeometry<3,3,const AlbertaGrid<3,3> >::checkInverseMapping (int loc) const
{
  // checks if F^-1 (x_i) == xref_i  
  enum { dim = 3 };
  
  const FieldVector<albertCtype, dim> & coord    = coord_[loc];
  const FieldVector<albertCtype, dim> & refcoord = refelem()[loc];
  buildJacobianInverse();
  
  FieldVector<albertCtype, dim> tmp3 = coord - coord_[0];     
  FieldVector<albertCtype, dim> tmp2(0.0);

  Jinv_.umv(tmp3,tmp2);

  for(int j=0; j<dim; j++)
    if(std::abs(tmp2[j] - refcoord[j]) > 1e-15)
    {
      std::cout << "AlbertaGridGeometry<3,3,AlbertaGrid<3,3> >::checkInverseMapping: Mapping of coord " << loc << " incorrect! \n";
      return false;
    }
  return true;
}


template <int mydim, int cdim, class GridImp>
inline bool AlbertaGridGeometry<mydim,cdim,GridImp>::checkMapping (int loc) const
{
  DUNE_THROW(AlbertaError,"AlbertaGridGeometry::checkMapping: no default implemantation!");
  return false;
}

template <>
inline bool AlbertaGridGeometry<2,2,const AlbertaGrid<2,2> >::checkMapping (int loc) const
{
  // checks the mapping 
  // F = Ax + P_0
  enum { dim =2 };
  
  calcElMatrix ();
  
  const FieldVector<albertCtype, dim> & coord    = coord_[loc];
  const FieldVector<albertCtype, dim> & refcoord = refelem()[loc];
  
  FieldVector<albertCtype, dim> tmp2 = coord_[0];
  elMat_.umv(refcoord,tmp2);     

  for(int j=0; j<dim; j++)
    if(tmp2[j] != coord[j])
    {
      std::cout << coord; std::cout << tmp2; std::cout << "\n";
      std::cout << "AlbertaGridGeometry<2,2,AlbertaGrid<2,2> >::checkMapping: Mapping of coord " << loc << " incorrect! \n";
      return false;
    }
  return true;
}

template <>
inline bool AlbertaGridGeometry<3,3,const AlbertaGrid<3,3> >::checkMapping (int loc) const
{
  // checks the mapping 
  // F = Ax + P_0
  
  enum { dim = 3 };
  
  calcElMatrix ();
  
  const FieldVector<albertCtype, dim> & coord    = coord_[loc];
  const FieldVector<albertCtype, dim> & refcoord = refelem()[loc];
  
  FieldVector<albertCtype, dim> tmp2 = coord_[0];
  elMat_.umv(refcoord,tmp2);     

  for(int j=0; j<dim; j++)
  {
    if(std::abs(tmp2[j] - coord[j]) > 1e-15)
    {
      std::cout << "Checking of " << loc << " not ok!\n";
      std::cout << coord; std::cout << refcoord;
      std::cout << tmp2; std::cout << "\n";
      std::cout << "AlbertaGridGeometry<3,3,AlbertaGrid<3,3> >::checkMapping: Mapping of coord " << loc << " incorrect! \n";
      return false;
    }
  }
  return true;
}

template <int mydim, int cdim, class GridImp>
inline bool AlbertaGridGeometry<mydim,cdim,GridImp>::
checkInside(const FieldVector<albertCtype, mydim> &local) const
{
  albertCtype sum = 0.0;
  
  for(int i=0; i<mydim; i++)
  {
    sum += local[i];
    if(local[i] < 0.0) 
    {
      if(std::abs(local[i]) > 1e-15) 
      {
        return false; 
      }
    }
  }
  
  if( sum > 1.0 ) 
  {
    if(sum > (1.0 + 1e-15))
      return false;
  }
   
  return true;
}

//*************************************************************************
//
//  --AlbertaGridEntity 
//  --Entity 
//
//*************************************************************************
//
//  codim > 0
//
// The Geometry is prescribed by the EL_INFO struct of ALBERTA MESH
// the pointer to this struct is set and get by setElInfo and
// getElInfo. 
//*********************************************************************8
template<int codim, int dim, class GridImp>
inline void AlbertaGridEntity<codim,dim,GridImp>::
makeDescription()
{
  elInfo_  = 0;
  element_ = 0;
  builtgeometry_ = false;
}

template<int codim, int dim, class GridImp>
inline AlbertaGridEntity<codim,dim,GridImp>::
AlbertaGridEntity(const GridImp &grid, int level, 
      ALBERTA TRAVERSE_STACK * travStack) 
  : grid_(grid) 
  , level_ ( level )
  , geo_ (false)
{
  travStack_ = travStack;
  makeDescription();
}


template<int codim, int dim, class GridImp>
inline void AlbertaGridEntity<codim,dim,GridImp>::
setTraverseStack(ALBERTA TRAVERSE_STACK * travStack)
{
  travStack_ = travStack;
}

template<int codim, int dim, class GridImp>
inline AlbertaGridEntity<codim,dim,GridImp>::
AlbertaGridEntity(const GridImp &grid, int level) : 
  grid_(grid)
, level_ (level)
, geo_ ( false )
, localFCoordCalced_(false)
{
  travStack_ = 0;
  makeDescription();
}

template<int codim, int dim, class GridImp>
inline ALBERTA EL_INFO* AlbertaGridEntity<codim,dim,GridImp>::
getElInfo() const
{
  return elInfo_;
}

template<int codim, int dim, class GridImp>
inline ALBERTA EL * AlbertaGridEntity<codim,dim,GridImp>::
getElement() const
{
  return element_;
}

template<int codim, int dim, class GridImp>
inline void AlbertaGridEntity<codim,dim,GridImp>::
setElInfo(ALBERTA EL_INFO * elInfo, int face,
          int edge, int vertex )
{
  face_ = face;
  edge_ = edge;
  vertex_ = vertex;
  elInfo_ = elInfo;
  if(elInfo_) 
    element_ = elInfo_->el;
  else 
    element_ = 0;
  builtgeometry_ = geo_.builtGeom(elInfo_,face,edge,vertex);
  localFCoordCalced_ = false;
}

template<int codim, int dim, class GridImp>
inline void AlbertaGridEntity<codim,dim,GridImp>::
setEntity(const AlbertaGridEntity<codim,dim,GridImp> & org)
{
  setElInfo(org.elInfo_,org.face_,org.edge_,org.vertex_);  
  setLevel(org.level_);
}

template<int codim, int dim, class GridImp>
inline void AlbertaGridEntity<codim,dim,GridImp>::
setLevel(int level) 
{
  level_ = level;
}

template<int codim, int dim, class GridImp>
inline int AlbertaGridEntity<codim,dim,GridImp>::
level() const
{
  return level_;
}

template<int codim, int dim, class GridImp>
inline int AlbertaGridEntity<codim,dim,GridImp>::
index() const
{
  const Entity en (*this);
  return grid_.levelIndexSet(en.level()).index(en);
}

// default 
template <class GridImp, int codim, int cdim> 
struct AlbertaGridBoundaryId
{
  static int boundaryId (const ALBERTA EL_INFO * elInfo, int face, int edge, int vertex)
  {
    return 0;
  }
};

// faces in 2d and 3d 
template <class GridImp> 
struct AlbertaGridBoundaryId<GridImp,1,3>
{
  static int boundaryId (const ALBERTA EL_INFO * elInfo, int face, int edge, int vertex)
  {
    return elInfo->boundary[face]->bound;
  }
};

template <class GridImp> 
struct AlbertaGridBoundaryId<GridImp,1,2>
{
  static int boundaryId (const ALBERTA EL_INFO * elInfo, int face, int edge, int vertex)
  {
    return elInfo->boundary[face]->bound;
  }
};

// vertices in 2d and 3d 
template <class GridImp, int dim> 
struct AlbertaGridBoundaryId<GridImp,dim,dim>
{
  static int boundaryId (const ALBERTA EL_INFO * elInfo, int face, int edge, int vertex)
  {
    return elInfo->bound[vertex];
  }
};

template <int codim, int dim, class GridImp> 
inline int AlbertaGridEntity<codim,dim,GridImp>::boundaryId() const
{
  return AlbertaGridBoundaryId<GridImp,codim,dim>::boundaryId(elInfo_,face_,edge_,vertex_);
}


// vertices in 2d and 3d 
template <class GridImp, int codim, int cdim> 
struct AlbertaGridFEVnum
{
  static int getFEVnum (int face, int edge, int vertex)
  {
    return face;
  }
};

template <class GridImp, int cdim> 
struct AlbertaGridFEVnum<GridImp,1,cdim> 
{
  static int getFEVnum (int face, int edge, int vertex)
  {
    return face;
  }
};

template <class GridImp, int cdim> 
struct AlbertaGridFEVnum<GridImp,cdim,cdim> 
{
  static int getFEVnum (int face, int edge, int vertex)
  {
    return vertex;
  }
};

template <class GridImp> 
struct AlbertaGridFEVnum<GridImp,2,3> 
{
  static int getFEVnum (int face, int edge, int vertex)
  {
    return edge;
  }
};

template<int codim, int dim, class GridImp>
inline int AlbertaGridEntity<codim,dim,GridImp>::
getFEVnum() const
{
  return AlbertaGridFEVnum<GridImp,codim,GridImp::dimensionworld>::getFEVnum(face_,edge_,vertex_);
}
template<int codim, int dim, class GridImp>
inline int AlbertaGridEntity<codim,dim,GridImp>::
globalIndex() const
{
  //assert(codim == dim);
  const Entity en (*this);
  return grid_.hierarchicIndexSet().index(en);
}

template<int cd, int dim, class GridImp>
inline const typename AlbertaGridEntity<cd,dim,GridImp>::Geometry & 
AlbertaGridEntity<cd,dim,GridImp>::geometry() const
{
  assert(builtgeometry_ == true);
  return geo_;
}

template<int codim, int dim, class GridImp>
inline typename AlbertaGridEntity<codim,dim,GridImp>::EntityPointer 
AlbertaGridEntity<codim,dim,GridImp>::ownersFather () const
{
  ALBERTA EL_INFO * fatherInfo = ALBERTA AlbertHelp::getFatherInfo(travStack_,elInfo_,level_);
  int fatherLevel = (level_ > 0) ? (level_-1) : 0;

  return AlbertaGridEntityPointer<0,GridImp> (grid_,travStack_,fatherLevel,fatherInfo,0,0,0);
}

/*
// coords for 2d 
static double fatherref [2][3][2] =
  {
    { {0.0,1.0},{0.0,0.0 }, {0.5,0.0 } }  ,
    { {1.0,0.0},{0.0,1.0 }, {0.5,0.0 } }
  };
*/

template<int codim, int dim, class GridImp>
inline FieldVector<albertCtype, dim>&
AlbertaGridEntity<codim,dim,GridImp>::positionInOwnersFather() const
{
  assert( codim == dim );
  if(!localFCoordCalced_)
  {
    EntityPointer vati = this->ownersFather();
    localFatherCoords_ = (*vati).geometry().local( this->geometry()[0] );
    localFCoordCalced_ = true;
/*
    // check 
    if((level_ > 0) && (dim == 2))
    {
      ALBERTA EL_INFO * fatty = ALBERTA AlbertHelp::getFatherInfo(travStack_,elInfo_,level_);
      int child = 0;
      if(elInfo_->el == fatty->el->child[1])
      {
        child = 1;
      }

      FieldVector<double,2> tmp(0.0);
      for(int j=0; j<2; j++) tmp[j] = fatherref[child][vertex_][j];
      if( (localFatherCoords_ - tmp).two_norm() > 1.0E-10) 
      {
        std::cout << localFatherCoords_ << " c|r " << tmp << " localF \n";
        assert(false); 
      }
    }
*/
  }
  return localFatherCoords_;
}


//************************************
//
//  --AlbertaGridEntity codim = 0 
//  --0Entity codim = 0 
//
template<int dim, class GridImp>
inline int AlbertaGridEntity <0,dim,GridImp>::
boundaryId() const
{
  return 0;
}

template<int dim, class GridImp>
inline AdaptationState AlbertaGridEntity <0,dim,GridImp>::
state() const
{
  assert( element_ && elInfo_ );
  assert( element_ == elInfo_->el );
  if( element_->mark < 0 )
  {
    return COARSEN; 
  }
  if( grid_.checkElNew( element_ ) )
  {
    return REFINED;   
  }

  return NONE;
}

template<int dim, class GridImp>
inline PartitionType AlbertaGridEntity <0,dim,GridImp>::
partitionType () const 
{
  return grid_.partitionType( elInfo_ );
}

template<int dim, class GridImp>
inline bool AlbertaGridEntity <0,dim,GridImp>::isLeaf() const
{
  assert( element_ && elInfo_ );
  assert( element_ == elInfo_->el );
  // if no child exists, then this element is leaf element 
  return (element_->child[0] == 0); 
}
  
//***************************

template<int dim, class GridImp>
inline void AlbertaGridEntity <0,dim,GridImp>::
makeDescription()
{
  elInfo_  = 0;
  element_ = 0;
  builtgeometry_ = false;
}

template<int dim, class GridImp>
inline void AlbertaGridEntity <0,dim,GridImp>::
setTraverseStack(ALBERTA TRAVERSE_STACK * travStack)
{
  travStack_ = travStack;
}

template<int dim, class GridImp>
inline AlbertaGridEntity <0,dim,GridImp>::
AlbertaGridEntity(const GridImp &grid, int level) 
  : grid_(grid) 
  , level_ (level)
  , travStack_ (0) , elInfo_ (0) 
  , fatherReLocal_(false)
  , geo_(false) 
  , builtgeometry_ (false)
{
}

//*****************************************************************
// count
template <class GridImp, int dim, int cc> struct AlbertaGridCount {
  static int count () { return dim+1; }
};
template <class GridImp> struct AlbertaGridCount<GridImp,3,2> {
  static int count () { return 6; }
};
 
template<int dim, class GridImp> template <int cc> 
inline int AlbertaGridEntity <0,dim,GridImp>::count () const 
{
  return AlbertaGridCount<GridImp,dim,cc>::count();
}

//*****************************************************************
// subIndex 
template<int dim, class GridImp> template <int cc>
inline int AlbertaGridEntity <0,dim,GridImp>::subIndex ( int i ) const
{
  const Entity en (*this);
  return grid_.hierarchicIndexSet().template subIndex<cc> (en,i);
  //return grid_.levelIndexSet(en.level()).template subIndex<cc> (en,i);
}

template <class GridImp, int dim, int cd> struct SubEntity;

// specialisation for elements  
template <class GridImp, int dim>
struct SubEntity<GridImp,dim,0>
{
  typedef typename AlbertaGridEntity <0,dim,GridImp>::template Codim<0>::EntityPointer EntityPointerType;
  static EntityPointerType entity(GridImp & grid, ALBERTA TRAVERSE_STACK * stack, 
      int level, ALBERTA EL_INFO * elInfo, int i )
  {
    return AlbertaGridEntityPointer<0,GridImp> (grid, stack , level ,elInfo, 0,0,0);
  }
};

// specialisation for faces 
template <class GridImp, int dim>
struct SubEntity<GridImp,dim,1>
{
  typedef typename AlbertaGridEntity <0,dim,GridImp>::template Codim<1>::EntityPointer EntityPointerType;
  static EntityPointerType entity(GridImp & grid, ALBERTA TRAVERSE_STACK * stack, 
      int level, ALBERTA EL_INFO * elInfo, int i )
  {
    return AlbertaGridEntityPointer<1,GridImp> (grid, stack , level ,elInfo, i,0,0);
  }
};

// specialisation for edges , only when dim == 3 
template <class GridImp>
struct SubEntity<GridImp,3,2>
{
  enum { dim = 3 };
  typedef typename AlbertaGridEntity <0,dim,GridImp>::template Codim<2>::EntityPointer EntityPointerType;
  static EntityPointerType entity(GridImp & grid, ALBERTA TRAVERSE_STACK * stack, 
      int level, ALBERTA EL_INFO * elInfo, int i )
  {
    return AlbertaGridEntityPointer<2,GridImp> (grid, stack , level ,elInfo, 0,i,0);
  }
};

// specialisation for vertices 
template <class GridImp, int dim>
struct SubEntity<GridImp,dim,dim>
{
  typedef typename AlbertaGridEntity <0,dim,GridImp>::template Codim<dim>::EntityPointer EntityPointerType;
  static EntityPointerType entity(GridImp & grid, ALBERTA TRAVERSE_STACK * stack, 
      int level, ALBERTA EL_INFO * elInfo, int i )
  {
    return AlbertaGridEntityPointer<dim,GridImp> (grid, stack , level ,elInfo, 0,0,i);
  }
};


// default is faces 
template <int dim, class GridImp>
template <int cc>
inline typename AlbertaGridEntity <0,dim,GridImp>::template Codim<cc>::EntityPointer
AlbertaGridEntity <0,dim,GridImp>::entity ( int i ) const
{
  return SubEntity<GridImp,dim,cc> :: entity(grid_,travStack_,level(),elInfo_,i);
}

template<int dim, class GridImp>
inline ALBERTA EL_INFO* AlbertaGridEntity <0,dim,GridImp>::
getElInfo() const
{
  return elInfo_;
}

template<int dim, class GridImp>
inline ALBERTA EL * AlbertaGridEntity <0,dim,GridImp>::
getElement() const
{
  return element_;
}

template<int dim, class GridImp>
inline int AlbertaGridEntity <0,dim,GridImp>::
level() const
{
  return level_;
}

template<int dim, class GridImp>
inline int AlbertaGridEntity <0,dim,GridImp>::
index() const
{
  const Entity en (*this);
  return grid_.levelIndexSet(level()).index(en);
}

template<int dim, class GridImp>
inline int AlbertaGridEntity <0,dim,GridImp>::
globalIndex() const
{
  return grid_.getElementNumber( elInfo_->el );
}

template<int dim, class GridImp>
inline void AlbertaGridEntity <0,dim,GridImp>::
setLevel(int actLevel)
{
  level_ = actLevel;
  assert( level_ >= 0);
}

template<int dim, class GridImp>
inline void AlbertaGridEntity <0,dim,GridImp>::
setElInfo(ALBERTA EL_INFO * elInfo, int face, int edge, int vertex )
{
  // in this case the face, edge and vertex information is not used,
  // because we are in the element case
  elInfo_ = elInfo;
  if(elInfo_) 
    element_ = elInfo_->el;
  else 
    element_ = 0;
  builtgeometry_ = geo_.builtGeom(elInfo_,face,edge,vertex);
}

template<int dim, class GridImp>
inline void AlbertaGridEntity<0,dim,GridImp>::
setEntity(const AlbertaGridEntity<0,dim,GridImp> & org)
{
  setElInfo(org.elInfo_);  
  setTraverseStack(org.travStack_);
  setLevel(org.level());
}

template<int dim, class GridImp>
inline const typename AlbertaGridEntity <0,dim,GridImp>::Geometry & 
AlbertaGridEntity <0,dim,GridImp>::geometry() const
{
  assert(builtgeometry_ == true);
  return geo_;
}


template<int dim, class GridImp>
inline typename AlbertaGridEntity <0,dim,GridImp>::EntityPointer 
AlbertaGridEntity <0,dim,GridImp>::father() const
{
  ALBERTA EL_INFO * fatherInfo = ALBERTA AlbertHelp::getFatherInfo(travStack_,elInfo_,level_);
  int fatherLevel = (level_ > 0) ? (level_-1) : 0;

  return AlbertaGridEntityPointer<0,GridImp> (grid_,travStack_,fatherLevel,fatherInfo,0,0,0);
}

template< int dim, class GridImp >
inline const typename AlbertaGridEntity <0,dim,GridImp>::Geometry & 
AlbertaGridEntity <0,dim,GridImp>::geometryInFather() const
{
  //AlbertaGridLevelIterator<0,dim,dimworld> daddy = father();
  const Geometry & daddy = (*father()).geometry();  

  fatherReLocal_.initGeom();
  // compute the local coordinates in father refelem
  for(int i=0; i<fatherReLocal_.corners(); i++)
    fatherReLocal_.getCoordVec(i) = daddy.local(this->geometry()[i]);
  
  return fatherReLocal_;
}
// end AlbertaGridEntity

//*******************************************************************
//
//  --EntityPointer
//  --EnPointer 
//
//*******************************************************************
template<int codim, class GridImp >
inline AlbertaGridEntityPointer<codim,GridImp> ::
  AlbertaGridEntityPointer(const GridImp & grid,
      int level,  ALBERTA EL_INFO *elInfo,int face,int edge,int vertex)
  : grid_(grid)
  , entity_ ( grid_.template getNewEntity<codim> ( level ) )
  , done_ (false)
{
  assert( entity_ );
  (*entity_).setElInfo(elInfo,face,edge,vertex);
  (*entity_).setLevel(level);
}     

template<int codim, class GridImp >
inline AlbertaGridEntityPointer<codim,GridImp> ::
 AlbertaGridEntityPointer(const GridImp & grid, ALBERTA TRAVERSE_STACK * stack ,
      int level,  ALBERTA EL_INFO *elInfo,int face,int edge,int vertex)
  : grid_(grid)
  , entity_ ( grid_.template getNewEntity<codim> ( level ) )
  , done_ (false)
{
  assert( entity_ );
  (*entity_).setElInfo(elInfo,face,edge,vertex);
  (*entity_).setTraverseStack(stack);
  (*entity_).setLevel(level);
}     

template<int codim, class GridImp >
inline AlbertaGridEntityPointer<codim,GridImp> ::
  AlbertaGridEntityPointer(const GridImp & grid, int level , bool end )
  : grid_(grid) 
  , entity_ ( grid_.template getNewEntity<codim> (level) )
  , done_ (end) 
{
  if(done_) this->done();
}

template<int codim, class GridImp >
inline AlbertaGridEntityPointer<codim,GridImp> ::
  AlbertaGridEntityPointer(const AlbertaGridEntityPointerType & org)
  : grid_(org.grid_)
  , entity_ ( grid_.template getNewEntity<codim> ( org.entity_->level() ) )
{
  (*entity_).setEntity( *(org.entity_) );
}

template<int codim, class GridImp >
inline AlbertaGridEntityPointer<codim,GridImp> :: ~AlbertaGridEntityPointer()
{
  grid_.freeEntity( entity_ );
}

template<int codim, class GridImp >
inline void AlbertaGridEntityPointer<codim,GridImp>::done ()
{
  // sets entity pointer in the status of an end iterator 
  (*entity_).removeElInfo();
  done_ = true;
}

template<int codim, class GridImp >
inline bool AlbertaGridEntityPointer<codim,GridImp>::
equals (const AlbertaGridEntityPointer<codim,GridImp>& i) const
{
  ALBERTA EL * e1 = (*entity_).getElement();
  ALBERTA EL * e2 = (*(i.entity_)).getElement();
  return ((e1 == e2 ) && (done_ == i.done_));
}

template<int codim, class GridImp >
inline typename AlbertaGridEntityPointer<codim,GridImp>::Entity &
AlbertaGridEntityPointer<codim,GridImp>::dereference () const
{
  assert(entity_);
  return (*entity_);
}

template<int codim, class GridImp >
inline int AlbertaGridEntityPointer<codim,GridImp>::level () const
{
  assert(entity_);
  return (*entity_).level();
}




//***************************************************************
//
//  --AlbertaGridHierarchicIterator
//  --HierarchicIterator
//
//***************************************************************
template< class GridImp >
inline void AlbertaGridHierarchicIterator<GridImp>::
makeIterator()
{
  virtualEntity_.setTraverseStack(0);
  virtualEntity_.setElInfo(0,0,0,0);
}

template< class GridImp >
inline AlbertaGridHierarchicIterator<GridImp>::
AlbertaGridHierarchicIterator(const GridImp & grid,
                              int actLevel,
                              int maxLevel) 
  : AlbertaGridEntityPointer<0,GridImp> (grid,actLevel,true)  
  , level_ (actLevel) 
  , maxlevel_ (maxLevel) 
  , virtualEntity_(*(this->entity_))
  , end_ (true)
{
  makeIterator();
}

template< class GridImp >
inline AlbertaGridHierarchicIterator<GridImp>::
AlbertaGridHierarchicIterator(const GridImp & grid, 
  ALBERTA TRAVERSE_STACK *travStack,int actLevel, int maxLevel) 
  : AlbertaGridEntityPointer<0,GridImp> (grid,actLevel,false)  
  , level_ (actLevel)
  , maxlevel_ ( maxLevel)
  , virtualEntity_( *(this->entity_) )
  , end_ (false)
{
  if(travStack)
  {
    // get new ALBERTA TRAVERSE STACK 
    manageStack_.makeItNew(true);
    
    ALBERTA TRAVERSE_STACK *stack = manageStack_.getStack();

    // cut old traverse stack, kepp only actual element 
    cutHierarchicStack(stack, travStack);

    // set new traverse level
    if(maxlevel_ < 0) 
    {
      std::cout << "WARNING: maxlevel < 0 in AlbertaGridHierarchicIterator! \n";
      // this means, we go until leaf level 
      stack->traverse_fill_flag = CALL_LEAF_EL | stack->traverse_fill_flag;
      // exact here has to stand Grid->maxlevel, but is ok anyway
      maxlevel_ = this->grid_.maxlevel(); //123456789;
    }
    // set new traverse level
    stack->traverse_level = maxlevel_;

    virtualEntity_.setTraverseStack(stack);
    // Hier kann ein beliebiges Geometry uebergeben werden,
    // da jedes AlbertGeometry einen Zeiger auf das Macroelement
    // enthaelt.
    virtualEntity_.setElInfo(recursiveTraverse(stack));

    // set new level 
    virtualEntity_.setLevel(level_);
  }
  else
  {
    std::cout << "Warning: travStack == NULL in HierarchicIterator(travStack,travLevel) \n";
    makeIterator();
  }
}

template< class GridImp >
inline AlbertaGridHierarchicIterator<GridImp>::
AlbertaGridHierarchicIterator(const AlbertaGridHierarchicIterator<GridImp> & org)
  : AlbertaGridEntityPointer<0,GridImp> (org.grid_,org.level(), org.end_ )
  , level_ ( org.level_ )
  , maxlevel_ ( org.maxlevel_ )
  , virtualEntity_( *(this->entity_) )
  , manageStack_ ( org.manageStack_ )
{
  if( org.virtualEntity_.getElInfo() )
    virtualEntity_.setEntity( org.virtualEntity_ );
  else 
    this->done();
}

template< class GridImp >
inline void AlbertaGridHierarchicIterator< GridImp >::increment()
{
  ALBERTA EL_INFO * nextinfo = recursiveTraverse(manageStack_.getStack());
  if(!nextinfo) 
  {
    this->done();
    return;
  }
  
  virtualEntity_.setElInfo( nextinfo );
  // set new actual level, calculated by recursiveTraverse 
  virtualEntity_.setLevel(level_);
  return ;
}

template< class GridImp >
inline ALBERTA EL_INFO * 
AlbertaGridHierarchicIterator<GridImp>::
recursiveTraverse(ALBERTA TRAVERSE_STACK * stack)
{
    // see function 
    // static EL_INFO *traverse_leaf_el(TRAVERSE_STACK *stack)
    // Common/traverse_nr_common.cc, line 392 
    ALBERTA EL * el=0;

    if(!stack->elinfo_stack)
    {
      /* somethin' wrong */
      return 0;
    }
    else
    {
      // go up until we can go down again
      el = stack->elinfo_stack[stack->stack_used].el;
      
      // stack->stack_used is actual element in stack 
      // stack->info_stack[stack->stack_used] >= 2
      //    means the two children has been visited
      while((stack->stack_used > 0) &&
      ((stack->info_stack[stack->stack_used] >= 2) 
       || (el->child[0] == 0) 
       || ( stack->traverse_level <=
      (stack->elinfo_stack+stack->stack_used)->level)) )
      {
        stack->stack_used--;
        el = stack->elinfo_stack[stack->stack_used].el;
        level_ = stack->elinfo_stack[stack->stack_used].level;
      }
      
      // goto next father is done by other iterator and not our problem  
      if(stack->stack_used < 1)
      {
        return 0;
      }
    }
    
    // go down next child 
    if(el->child[0] && (stack->traverse_level > 
          (stack->elinfo_stack+stack->stack_used)->level) )
    {
      if(stack->stack_used >= stack->stack_size - 1)
        ALBERTA enlargeTraverseStack(stack);

      int i = stack->info_stack[stack->stack_used];
      el = el->child[i];
      stack->info_stack[stack->stack_used]++;
      
      // new: go down maxlevel, but fake the elements 
      level_++;
      this->grid_.fillElInfo(i, level_, stack->elinfo_stack+stack->stack_used,
                        stack->elinfo_stack+stack->stack_used+1 ,true);
      //ALBERTA fill_elinfo(i, stack->elinfo_stack + stack->stack_used,
      //  stack->elinfo_stack + (stack->stack_used + 1));
        
      stack->stack_used++;
      stack->info_stack[stack->stack_used] = 0;
    }
    else 
    { 
      return 0;
    }

  return (stack->elinfo_stack + stack->stack_used);
}  // recursive traverse over all childs 

// end AlbertaGridHierarchicIterator
//***************************************************************
//
//  --AlbertaGridBoundaryEntity
//  --BoundaryEntity
//
//***************************************************************

template< class GridImp >
inline AlbertaGridBoundaryEntity<GridImp>::
AlbertaGridBoundaryEntity () : _geom (false) , _elInfo ( 0 ),
  _neigh (-1) {}

template< class GridImp >
inline int AlbertaGridBoundaryEntity<GridImp>::id () const
{
  assert(_elInfo->boundary[_neigh] != 0);
  return _elInfo->boundary[_neigh]->bound;
}

template< class GridImp >
inline bool AlbertaGridBoundaryEntity<GridImp>::hasGeometry () const
{
  return _geom.builtGeom(_elInfo,0,0,0);
}

template< class GridImp >
inline const typename AlbertaGridBoundaryEntity<GridImp>::Geometry & 
AlbertaGridBoundaryEntity<GridImp>::geometry () const
{
  return _geom;
}

template< class GridImp >
inline void AlbertaGridBoundaryEntity<GridImp>::
setElInfo (ALBERTA EL_INFO * elInfo, int nb)
{
  _neigh = nb;
  if(elInfo)
    _elInfo = elInfo;
  else
    _elInfo = 0;
}

//***************************************************************
//
//  --AlbertaGridIntersectionIterator
//  --IntersectionIterator
//
//***************************************************************

// these object should be generated with new by Entity, because
// for a LevelIterator we only need one virtualNeighbour Entity, which is
// given to the Neighbour Iterator, we need a list of Neighbor Entitys
template< class GridImp >
inline void AlbertaGridIntersectionIterator<GridImp>::freeObjects () const
{
  if(fakeNeigh_) 
    grid_.interSelfProvider_.freeObjectEntity(fakeNeigh_);
  
  if(neighGlob_) 
    grid_.interNeighProvider_.freeObjectEntity(neighGlob_);

  if(boundaryEntity_) 
  {
    grid_.interBndProvider_.freeObjectEntity(boundaryEntity_);
    boundaryEntity_ = 0;
  }

  if(neighElInfo_) 
    elinfoProvider.freeObjectEntity(neighElInfo_);
}

template< class GridImp >
inline AlbertaGridIntersectionIterator<GridImp>::~AlbertaGridIntersectionIterator ()
{
  freeObjects();
}

template< class GridImp >
inline AlbertaGridIntersectionIterator<GridImp>::
AlbertaGridIntersectionIterator(const GridImp & grid,int level) : 
  AlbertaGridEntityPointer<0,GridImp> (grid,level,false),
  level_ (level), 
  neighborCount_ (dim+1),
  virtualEntity_ ( *(this->entity_) ),
  elInfo_ (0),
  fakeNeigh_ (0),
  neighGlob_ (0),
  boundaryEntity_ (0),
  neighElInfo_ (0) 
{
}


template< class GridImp >
inline AlbertaGridIntersectionIterator<GridImp>::AlbertaGridIntersectionIterator
(const GridImp & grid, int level, ALBERTA EL_INFO *elInfo )
  : AlbertaGridEntityPointer<0,GridImp> (grid,level,false)
  , level_ (level), neighborCount_ (0) 
  , builtNeigh_ (false) 
  , virtualEntity_ ( *(this->entity_) ) 
  , elInfo_ ( elInfo ) 
  , fakeNeigh_ (0) 
  , neighGlob_ (0)  
  , boundaryEntity_ (0) 
  , neighElInfo_ ( elinfoProvider.getNewObjectEntity() ) 
{
}

// copy constructor 
template< class GridImp >
inline AlbertaGridIntersectionIterator<GridImp>::AlbertaGridIntersectionIterator 
(const AlbertaGridIntersectionIterator<GridImp> & org) 
  : AlbertaGridEntityPointer<0,GridImp> (org.grid_,org.level(), (org.elInfo_) ? false : true )
  , level_(org.level_) 
  , neighborCount_(org.neighborCount_)
  , builtNeigh_ (false) 
  , virtualEntity_ ( *(this->entity_) ) 
  , elInfo_ ( org.elInfo_ ) 
  , fakeNeigh_ (0) 
  , neighGlob_ (0)  
  , boundaryEntity_ (0) 
  , neighElInfo_ ( (elInfo_) ? elinfoProvider.getNewObjectEntity() : 0 ) 
{
}

// assignment operator  
template< class GridImp >
inline  AlbertaGridIntersectionIterator<GridImp> & 
AlbertaGridIntersectionIterator<GridImp>::operator = (const AlbertaGridIntersectionIterator<GridImp> & org) 
{
  assert( false );
  
  // only assign iterators from the same grid 
  assert( &this->grid_ == &(org.grid_));
  level_ =  org.level_;
  neighborCount_ = org.neighborCount_;
  elInfo_ = org.elInfo_; 
  builtNeigh_ = false;
  assert( elInfo_ );
}

template< class GridImp >
inline void AlbertaGridIntersectionIterator<GridImp>::increment()
{
  builtNeigh_ = false;
  // is like go to the next neighbour
  neighborCount_++;

  // (dim+1) is neigbourCount for end iterators 
  if(neighborCount_ > dim) 
  {
    this->done();
  }
  return ;
}

template< class GridImp >
inline typename AlbertaGridIntersectionIterator<GridImp>::EntityPointer
AlbertaGridIntersectionIterator<GridImp>::outside () const 
{
  if(!builtNeigh_)
  {
    virtualEntity_.setLevel(level_);

    assert( elInfo_ );
    assert( neighElInfo_ );
    // just copy elInfo and then set some values 
    std::memcpy(neighElInfo_,elInfo_,sizeof(ALBERTA EL_INFO));
    
    setupVirtEn();
  }
  return AlbertaGridEntityPointer<0, GridImp>(this->grid_, 
                                              (int) neighElInfo_->level,
                                              neighElInfo_,
                                              0, 0, 0);
  //return virtualEntity_;
}

template< class GridImp >
inline typename AlbertaGridIntersectionIterator<GridImp>::EntityPointer 
AlbertaGridIntersectionIterator<GridImp>::inside () const {
  assert(elInfo_);
  return AlbertaGridEntityPointer<0, GridImp>(this->grid_,
                                              (int) elInfo_->level,
                                              elInfo_,
                                              0, 0, 0);
} 

template< class GridImp >
inline typename AlbertaGridIntersectionIterator<GridImp>::BoundaryEntity & 
AlbertaGridIntersectionIterator<GridImp>::boundaryEntity () const
{
  if(!boundaryEntity_) 
  {
    boundaryEntity_  = this->grid_.interBndProvider_.getNewObjectEntity();
  } 

  assert( boundaryEntity_ );
  (*boundaryEntity_).setElInfo(elInfo_,neighborCount_);
  return (*boundaryEntity_);
}

template< class GridImp >
inline bool AlbertaGridIntersectionIterator<GridImp>::boundary() const
{
  return (elInfo_->boundary[neighborCount_] != 0);
}

template< class GridImp >
inline bool AlbertaGridIntersectionIterator<GridImp>::neighbor() const
{
  return (elInfo_->neigh[neighborCount_] != 0);
}

template<class GridImp>
inline const typename AlbertaGridIntersectionIterator<GridImp>::NormalVecType &
AlbertaGridIntersectionIterator<GridImp>::unitOuterNormal (const LocalCoordType & local) const
{
  // calculates the outer_normal
  unitNormal_  = this->outerNormal(local);
  unitNormal_ *= (1.0/unitNormal_.two_norm());
  
  return unitNormal_; 
}

template<class GridImp>
inline const typename AlbertaGridIntersectionIterator<GridImp>::NormalVecType &
AlbertaGridIntersectionIterator<GridImp>::integrationOuterNormal (const LocalCoordType & local) const
{
  return this->outerNormal(local); 
}


template< class GridImp >
inline const typename AlbertaGridIntersectionIterator<GridImp>::NormalVecType & 
AlbertaGridIntersectionIterator<GridImp>::outerNormal(const LocalCoordType & local) const
{
  calcOuterNormal();
  return outNormal_;
}

template< class GridImp > 
inline void AlbertaGridIntersectionIterator<GridImp>:: calcOuterNormal() const
{
  std::cout << "outer_normal() not correctly implemented yet! \n";
  assert(false);
  for(int i=0; i<dimworld; i++)
    outNormal_[i] = 0.0;

  return ; 
}

template <>
inline void 
AlbertaGridIntersectionIterator<const AlbertaGrid<2,2> >::calcOuterNormal () const
{
  // seems to work   
  ALBERTA REAL_D *coord = elInfo_->coord;

  outNormal_[0] = -(coord[(neighborCount_+1)%3][1] - coord[(neighborCount_+2)%3][1]); 
  outNormal_[1] =   coord[(neighborCount_+1)%3][0] - coord[(neighborCount_+2)%3][0];

  return ;
}

template <> 
inline void AlbertaGridIntersectionIterator<const AlbertaGrid<3,3> >:: 
calcOuterNormal () const
{
  enum { dim = 3 };
  // rechne Kreuzprodukt der Vectoren aus   
  ALBERTA REAL_D *coord = elInfo_->coord;
  
  // in this case the orientation is negative, therefore multiply with -1
#if DIM == 3
  const albertCtype val = (elInfo_->orientation > 0) ? 0.5 : -0.5;
#else
  const albertCtype val = 0.5;
#endif 

  // neighborCount_ is the local face number 
  const int * localFaces = ALBERTA AlbertHelp::localTetraFaceNumber[neighborCount_]; 
  for(int i=0; i<dim; i++) 
  {
    tmpV_[i] = coord[localFaces[1]][i] - coord[localFaces[0]][i]; 
    tmpU_[i] = coord[localFaces[2]][i] - coord[localFaces[1]][i]; 
  }

  // outNormal_ has length 3 
  for(int i=0; i<dim; i++) 
    outNormal_[i] = tmpU_[(i+1)%dim] * tmpV_[(i+2)%dim] 
                  - tmpU_[(i+2)%dim] * tmpV_[(i+1)%dim];

  outNormal_ *= val;

  return ;
}

template< class GridImp >
inline typename AlbertaGridIntersectionIterator<GridImp>::LocalGeometry & 
AlbertaGridIntersectionIterator<GridImp>::
intersectionSelfLocal () const
{
  //std::cout << "\nintersection_self_local not checked until now! \n";
  if(!fakeNeigh_)
    fakeNeigh_ = this->grid_.interSelfProvider_.getNewObjectEntity();

  fakeNeigh_->builtGeom(elInfo_,neighborCount_,0,0);
  return (*fakeNeigh_);
}

template< class GridImp >
inline typename AlbertaGridIntersectionIterator<GridImp>::Geometry & 
AlbertaGridIntersectionIterator<GridImp>::
intersectionGlobal () const
{
  if(!neighGlob_)
    neighGlob_ = this->grid_.interNeighProvider_.getNewObjectEntity();

  if(neighGlob_->builtGeom(elInfo_,neighborCount_,0,0))
    return (*neighGlob_);
  else 
  {
    DUNE_THROW(AlbertaError, "intersection_self_global: error occured!");
  }
  return (*neighGlob_);
}

template< class GridImp >
inline typename AlbertaGridIntersectionIterator<GridImp>::LocalGeometry & 
AlbertaGridIntersectionIterator<GridImp>::intersectionNeighborLocal () const
{
  std::cout << "intersection_neighbor_local not checked until now! \n";
  if(!fakeNeigh_)
    fakeNeigh_ = this->grid_.interSelfProvider_.getNewObjectEntity();

  if(fakeNeigh_->builtGeom(neighElInfo_,neighborCount_,0,0)) 
    return (*fakeNeigh_);
  else 
  {
    DUNE_THROW(AlbertaError, "intersection_neighbor_local: error occured!");
  }
  return (*fakeNeigh_);

}

template< class GridImp >
inline int AlbertaGridIntersectionIterator<GridImp>::
numberInSelf () const
{
  return neighborCount_;
}

template< class GridImp >
inline int AlbertaGridIntersectionIterator<GridImp>::
numberInNeighbor () const
{
  return elInfo_->opp_vertex[neighborCount_];
}

// setup neighbor element with the information of elInfo_
template< class GridImp >
inline void AlbertaGridIntersectionIterator<GridImp>::setupVirtEn() const
{
  // set the neighbor element as element
  neighElInfo_->el = elInfo_->neigh[neighborCount_];
#if DIM==3
  neighElInfo_->orientation = elInfo_->orientation;
#endif

  int vx = elInfo_->opp_vertex[neighborCount_];
  
  for(int i=0; i<dimworld; i++) 
    neighElInfo_->coord[vx][i] = elInfo_->opp_coord[neighborCount_][i];
    
  for(int i=1; i<dim+1; i++)
  {
    int nb = (((neighborCount_-i)%(dim+1)) +dim+1)%(dim+1);
    for(int j=0; j<dimworld; j++)
      neighElInfo_->coord[(vx+i)%(dim+1)][j] = elInfo_->coord[nb][j];
  }
  /* works, tested many times */
  
  virtualEntity_.setElInfo(neighElInfo_);
  builtNeigh_ = true;
}
// end IntersectionIterator


//*******************************************************
// 
// --AlbertaGridTreeIterator
// --TreeIterator
// --LevelIterator
// 
//*******************************************************

//***********************************************************
//  some template specialization of goNextEntity
//***********************************************************
// default implementation, go next elInfo
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
goNextEntity(ALBERTA TRAVERSE_STACK *stack,ALBERTA EL_INFO *elinfo_old)
{
  // to be revised , use specialisation for speedup
  if(codim == 0) return goNextElInfo(stack,elinfo_old);
  if(codim == 1) return goNextFace(stack,elinfo_old);
  if((codim == 2) && (GridImp::dimension ==3)) return goNextEdge(stack,elinfo_old);
  if(codim == GridImp::dimension) return goNextVertex(stack,elinfo_old);

  DUNE_THROW(AlbertaError,"worng codimension and dimension in goNExtEntity of AlbertaGridTreeIterator \n");
  return 0;
}
//***************************************

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline void AlbertaGridTreeIterator<codim,pitype,GridImp>::
makeIterator()
{
  level_ = 0;
  enLevel_ = 0;
  vertex_ = 0;
  face_ = 0;
  edge_ = 0;
  vertexMarker_ = 0;

  virtualEntity_.setTraverseStack(0);
  virtualEntity_.setElInfo(0,0,0,0);
}

// Make LevelIterator with point to element from previous iterations
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline AlbertaGridTreeIterator<codim,pitype,GridImp>::
AlbertaGridTreeIterator(const GridImp & grid, int travLevel,int proc, bool leafIt ) 
  : AlbertaGridEntityPointer<codim,GridImp> (grid,travLevel,true) // true means end iterator 
  , level_   (travLevel)
  , enLevel_ (travLevel)
  , virtualEntity_(*(this->entity_))
  , face_(0)
  , edge_ (0)
  , vertex_ (0)
  , vertexMarker_(0) 
  , leafIt_(leafIt) , proc_(proc)
{
  makeIterator();
}

// Make LevelIterator with point to element from previous iterations
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline AlbertaGridTreeIterator<codim,pitype,GridImp>::
AlbertaGridTreeIterator(const AlbertaGridTreeIterator<codim,pitype,GridImp> & org)
  : AlbertaGridEntityPointer<codim,GridImp> (org.grid_,org.level_, (org.vertexMarker_) ? false : true)
  , level_   (org.level_)
  , enLevel_ (org.enLevel_)
  , virtualEntity_(*(this->entity_))
  , manageStack_ ()
  //, manageStack_ ( org.manageStack_ )
  , face_(org.face_)
  , edge_ (org.edge_)
  , vertex_ ( org.vertex_)
  , vertexMarker_(org.vertexMarker_) 
  , leafIt_(org.leafIt_) , proc_(org.proc_)
{
  if(vertexMarker_) 
  { 
    // if vertexMarker is not NULL then we have a real iterator 
    manageStack_.makeItNew(true);
    ALBERTA TRAVERSE_STACK * stack = manageStack_.getStack();
    ALBERTA copyTraverseStack( stack , org.manageStack_.getStack() );
        
    virtualEntity_.setTraverseStack( stack );
    /// get the actual used enInfo 
    ALBERTA EL_INFO * elInfo = stack->elinfo_stack+stack->stack_used;
    
    virtualEntity_.setElInfo( elInfo,face_,edge_,vertex_ );
    virtualEntity_.setLevel( enLevel_ );

    assert( virtualEntity_.globalIndex() == org.virtualEntity_.globalIndex());
    
   // virtualEntity_.setTraverseStack(manageStack_.getStack());
   // virtualEntity_.setEntity( *(org.entity_) );
  }
}

// Make LevelIterator with point to element from previous iterations
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline AlbertaGridTreeIterator<codim,pitype,GridImp>::
AlbertaGridTreeIterator(const GridImp & grid, TRAVERSE_STACK * stack, 
    int level,  ALBERTA EL_INFO *elInfo,int face,int edge,int vertex) 
  : AlbertaGridEntityPointer<codim,GridImp> (grid,level,elInfo,face,edge,vertex)
  , level_ (level) 
  , enLevel_(level) 
  , virtualEntity_(*(this->entity_)) 
  , face_ ( face ) ,  edge_ ( edge ), vertex_ ( vertex ) 
  , leafIt_(false) ,  proc_(-1)
  , vertexMarker_(0) 
  , vertex_ (0)
  , face_(0)
  , edge_ (0)
{
  assert(stack);
  virtualEntity_.setTraverseStack(stack);

  if(elInfo)
  {
    // diese Methode muss neu geschrieben werden, da man 
    // die ParentElement explizit speichern moechte. 
    virtualEntity_.setElInfo(elInfo,face_,edge_,vertex_);
  }
}

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline AlbertaGridTreeIterator<codim,pitype,GridImp>::
AlbertaGridTreeIterator(const GridImp & grid, 
  AlbertaMarkerVector * vertexMark, 
  int travLevel, int proc, bool leafIt) 
  : AlbertaGridEntityPointer<codim,GridImp> (grid,travLevel,false)
  , level_ (travLevel) , enLevel_(travLevel) 
  , virtualEntity_(*(this->entity_)) 
  , face_(0)
  , edge_ (0)
  , vertex_ (0)
  , vertexMarker_(0) 
  , leafIt_(leafIt), proc_(proc)
{
  ALBERTA MESH * mesh = this->grid_.getMesh();

  if( mesh && ((travLevel >= 0) && (travLevel <= this->grid_.maxlevel())) )
  {
    vertexMarker_ = vertexMark;
    
    ALBERTA FLAGS travFlags = FILL_ANY; //FILL_COORDS | FILL_NEIGH;
   
    // CALL_LEAF_EL is not used anymore
    travFlags = travFlags | CALL_LEAF_EL_LEVEL;

    // get traverse_stack
    manageStack_.makeItNew(true);
    
    virtualEntity_.setTraverseStack(manageStack_.getStack());

    // diese Methode muss neu geschrieben werden, da man 
    // die ParentElement explizit speichern moechte. 
    ALBERTA EL_INFO* elInfo = 
      goFirstElement(manageStack_.getStack(), mesh, travLevel,travFlags);

    virtualEntity_.setElInfo(elInfo,face_,edge_,vertex_);
    virtualEntity_.setLevel( enLevel_ );
  }
  else
  {
    // create empty iterator 
    makeIterator();
  }
}

// gehe zum naechsten Element, wie auch immer
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline void AlbertaGridTreeIterator<codim,pitype,GridImp>::increment()
{
  ALBERTA EL_INFO * nextinfo = goNextEntity(manageStack_.getStack(),virtualEntity_.getElInfo());
  
  if(!nextinfo) 
  {
    this->done();
    return ;
  }
  
  virtualEntity_.setElInfo( nextinfo , face_, edge_, vertex_); 
  virtualEntity_.setLevel( enLevel_ );

  return ;
}

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
goNextFace(ALBERTA TRAVERSE_STACK *stack, ALBERTA EL_INFO *elInfo)
{
  // go next Element, if face_ > numberOfVertices, then go to next elInfo 
  face_++;
  if(face_ >= (dim+1)) // dim+1 Faces
  {
    elInfo = goNextElInfo(stack, elInfo);
    face_ = 0;
  }

  if(!elInfo)
    return elInfo;  // if no more Faces, return
  
  if( (elInfo->neigh[face_]) &&
      (this->grid_.getElementNumber(elInfo->el) > this->grid_.getElementNumber(elInfo->neigh[face_])))
  {
    // if reachedFace before, go next 
    elInfo = goNextFace(stack,elInfo);
  }

  return elInfo;
}

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
goNextEdge(ALBERTA TRAVERSE_STACK *stack, ALBERTA EL_INFO *elInfo)
{
  // go next Element, Edge 0
  // treat Edge like Faces
  edge_++;
  if(edge_ >= 6) // in 3d only 6 Edges
  {
    elInfo = goNextElInfo(stack, elInfo);
    edge_ = 0;
  }
  
  if(!elInfo) return 0;  // if no more Edges, return

  // go next, if Vertex is not treated on this Element 
  if(vertexMarker_->edgeNotOnElement(elInfo->el,
        this->grid_.getElementNumber(elInfo->el),level_,
        this->grid_.getEdgeNumber(elInfo->el,edge_)))
  {
    elInfo = goNextEdge(stack,elInfo);
  }

  return elInfo;
}

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
goNextVertex(ALBERTA TRAVERSE_STACK *stack, ALBERTA EL_INFO *elInfo)
{
  // go next Element, Vertex 0
  // treat Vertices like Faces
  vertex_++;
  if(vertex_ >= (dim+1)) // dim+1 Vertices
  {
    elInfo = goNextElInfo(stack, elInfo);
    vertex_ = 0;
  }
  
  if(!elInfo) return 0;  // if no more Vertices, return

  // go next, if Vertex is not treated on this Element 
  if(vertexMarker_->notOnThisElement(elInfo->el,
        this->grid_.getElementNumber(elInfo->el),level_,
        this->grid_.getVertexNumber(elInfo->el,vertex_)))
  {
    elInfo = goNextVertex(stack,elInfo);
  }

  return elInfo;
}

/*
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline typename AlbertaGridTreeIterator<codim,pitype,GridImp>::Entity & 
AlbertaGridTreeIterator<codim,pitype,GridImp>::dereference () const
{
  assert(virtualEntity_.getElInfo() != 0);
  return virtualEntity_;
}
*/

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
goFirstElement(ALBERTA TRAVERSE_STACK *stack,ALBERTA MESH *mesh, int level, 
     ALBERTA FLAGS fill_flag)
{   
  FUNCNAME("goFirstElement");

  if (!stack)
  {
    ALBERTA_ERROR("no traverse stack\n");
    return(nil);
  }

  stack->traverse_mesh      = mesh;
  stack->traverse_level     = level;
  stack->traverse_fill_flag = fill_flag;

  if (stack->stack_size < 1)
    enlargeTraverseStack(stack);

  for (int i=0; i<stack->stack_size; i++)
    stack->elinfo_stack[i].fill_flag = fill_flag & FILL_ANY;

  stack->elinfo_stack[0].mesh = stack->elinfo_stack[1].mesh = mesh;

  if (fill_flag & CALL_LEAF_EL_LEVEL) 
  {
    ALBERTA_TEST_EXIT(level >= 0)("invalid level: %d\n",level);
  }

  stack->traverse_mel = 0;
  stack->stack_used   = 0;
  stack->el_count     = 0;

  // go to first enInfo, therefore goNextElInfo for all codims 
  return(goNextElInfo(stack,0));
}


// --travNext
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
goNextElInfo(ALBERTA TRAVERSE_STACK *stack, ALBERTA EL_INFO *elinfo_old)
{
  FUNCNAME("goNextElInfo");
  ALBERTA EL_INFO       *elinfo = 0;

  if (stack->stack_used)
  {
    ALBERTA_TEST_EXIT(elinfo_old == stack->elinfo_stack+stack->stack_used)
      ("invalid old elinfo\n");
  }
  else
  {
    ALBERTA_TEST_EXIT(elinfo_old == nil)("invalid old elinfo != nil\n");
  }

  PartitionIteratorType pt = pitype;
  if(this->grid_.myRank() < 0) pt = All_Partition;

  switch (pt)
  {
    // walk only over macro_elements that belong to this processor 
    case Interior_Partition :
    {
      // overloaded traverse_leaf_el_level, is not implemened in ALBERTA yet
      elinfo = traverseElLevelInteriorBorder(stack);

      // if leafIt_ == false go to elements only on desired level 
      if((elinfo) && (!leafIt_))
      {
        if(elinfo->level == stack->traverse_level)
          okReturn_ = true;

        while(!okReturn_)
        {
          elinfo = traverseElLevelInteriorBorder(stack);
          if(!elinfo) okReturn_ = true;
        }
        stack->el_count++;
      }
      
      // set new level for Entity  
      if((elinfo) && (leafIt_)) enLevel_ = elinfo->level;
      
      return(elinfo);
    }

    // Walk over all macro_elements on this grid 
    case All_Partition:
    {
      // overloaded traverse_leaf_el_level, is not implemened in ALBERTA yet
      elinfo = traverseElLevel(stack);

      //std::cout << elinfo << " elf | leaf " << leafIt_ << "\n";

      // if leafIt_ == false go to elements only on desired level 
      if((elinfo) && (!leafIt_))
      {
        if(elinfo->level == stack->traverse_level)
          okReturn_ = true;

        while(!okReturn_)
        {
          elinfo = traverseElLevel(stack);
          if(!elinfo) okReturn_ = true;
        }
        stack->el_count++;
      }
      
      // set new level for Entity  
      if((elinfo) && (leafIt_)) enLevel_ = elinfo->level;
      
      return(elinfo);
    }

    // walk over ghost elements, if proc == -1 then over all ghosts
    case Ghost_Partition:
    {
      // overloaded traverse_leaf_el_level, is not implemened in ALBERTA yet
      elinfo = traverseElLevelGhosts(stack);

      // if leafIt_ == false go to elements only on desired level 
      if((elinfo) && (!leafIt_))
      {
        if(elinfo->level == stack->traverse_level)
          okReturn_ = true;

        while(!okReturn_)
        {
          elinfo = traverseElLevelGhosts(stack);
          if(!elinfo) okReturn_ = true;
        }
        stack->el_count++;
      }

      // check neighbours 
      if(elinfo)
      {
        // here we have the interior element, now check the neighbours
        for(int i=0; i<dim+1; i++)
        {
          ALBERTA EL * neigh = NEIGH(elinfo->el,elinfo)[i];
          if(neigh)
          {
            if(this->grid_.getOwner(neigh) == this->grid_.myRank())
            {
              return elinfo;
            }
          }
        }
        return goNextElInfo(stack,elinfo);
      }

      // set new level for Entity  
      if((elinfo) && (leafIt_)) enLevel_ = elinfo->level;
      
      return(elinfo);
    }

    // walk over interior elements which have ghosts as neighbour 
    case InteriorBorder_Partition:
    {
      // overloaded traverse_leaf_el_level, is not implemened in ALBERTA yet
      elinfo = traverseElLevelInteriorBorder(stack);

      // if leafIt_ == false go to elements only on desired level 
      if((elinfo) && (!leafIt_))
      {
        if(elinfo->level == stack->traverse_level)
          okReturn_ = true;

        while(!okReturn_)
        {
          elinfo = traverseElLevelInteriorBorder(stack);
          if(!elinfo) okReturn_ = true;
        }
        stack->el_count++;
      }

      if(elinfo)
      {
        // here we have the interior element, now check the neighbours
        for(int i=0; i<dim+1; i++)
        {
          ALBERTA EL * neigh = NEIGH(elinfo->el,elinfo)[i];
          if(neigh)
          {
            if(((proc_ == -1) && (this->grid_.getOwner(neigh) != this->grid_.myRank()))||
               ((proc_ != -1) && (this->grid_.getOwner(neigh) == proc_) ))
            {
              return elinfo;
            }
          }
        }
        // we found not the one, so go next 
        return goNextElInfo(stack,elinfo);
      }

      // set new level for Entity  
      if((elinfo) && (leafIt_)) enLevel_ = elinfo->level;
      
      return elinfo;
    }
    // default iterator type no supported
    default:
    {
      DUNE_THROW(AlbertaError, "AlbertaGridTreeIterator::goNextEntity: Unsupported IteratorType!");
      return 0;
    }
  } // end switch
}

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
traverseElLevel(ALBERTA TRAVERSE_STACK *stack)
{
  FUNCNAME("traverseElLevel");
  ALBERTA EL *el;
  int i;
  okReturn_ = false;

  if (stack->stack_used == 0)   /* first call */
  {
    if(proc_ >= 0)
    {
      ALBERTA MACRO_EL * mel = stack->traverse_mesh->first_macro_el;
      while((this->grid_.getOwner(mel->el) != this->grid_.myRank() 
              && this->grid_.isNoElement(mel)))
      {
        mel = mel->next; 
        if(!mel) break;
      }
      stack->traverse_mel = mel;
    }
    else 
    {
      stack->traverse_mel = stack->traverse_mesh->first_macro_el;
    }
    if (stack->traverse_mel == nil)  return(nil);

    stack->stack_used = 1;
    
    ALBERTA fillMacroInfo(stack, stack->traverse_mel,
        stack->elinfo_stack+stack->stack_used,level_);

    stack->info_stack[stack->stack_used] = 0;

    el = stack->elinfo_stack[stack->stack_used].el;
    if ((el == nil) || (el->child[0] == nil)) {
      return(stack->elinfo_stack+stack->stack_used);
    }
  }
  else
  {
    el = stack->elinfo_stack[stack->stack_used].el;

    /* go up in tree until we can go down again */
    while((stack->stack_used > 0) &&
    ((stack->info_stack[stack->stack_used] >= 2) || (el->child[0]==nil)
      || ( stack->traverse_level <=
          (stack->elinfo_stack+stack->stack_used)->level)) )
    {
      stack->stack_used--;
      el = stack->elinfo_stack[stack->stack_used].el;
    }
    /* goto next macro element */
    if (stack->stack_used < 1)
    {
      ALBERTA MACRO_EL * mel = stack->traverse_mel->next;
      if(mel && (proc_ >= 0))
      {
        while((this->grid_.getOwner(mel->el) != this->grid_.myRank()
               && this->grid_.isNoElement(mel)))
        {
          mel = mel->next;
          if(!mel) break;
        }
        stack->traverse_mel = mel;
      }
      stack->traverse_mel = mel;
      if (stack->traverse_mel == nil)  return(nil);

      stack->stack_used = 1;

      ALBERTA fillMacroInfo(stack, stack->traverse_mel,
          stack->elinfo_stack+stack->stack_used,level_);

      stack->info_stack[stack->stack_used] = 0;

      el = stack->elinfo_stack[stack->stack_used].el;
      if ((el == nil) || (el->child[0] == nil))
      {
        return(stack->elinfo_stack+stack->stack_used);
      }
    }
  }

  /* go down tree until leaf oder level*/
  while (el->child[0] &&
   ( stack->traverse_level > (stack->elinfo_stack+stack->stack_used)->level))
  {
    if(stack->stack_used >= stack->stack_size-1)
      enlargeTraverseStack(stack);

    i = stack->info_stack[stack->stack_used];
    el = el->child[i];
    stack->info_stack[stack->stack_used]++;

    this->grid_.fillElInfo(i, level_, stack->elinfo_stack+stack->stack_used,
                     stack->elinfo_stack+stack->stack_used+1, false , leafIt_);

    stack->stack_used++;

    ALBERTA_TEST_EXIT(stack->stack_used < stack->stack_size)
      ("stack_size=%d too small, level=(%d,%d)\n",
       stack->stack_size, stack->elinfo_stack[stack->stack_used].level);

    stack->info_stack[stack->stack_used] = 0;

    if(stack->traverse_level == (stack->elinfo_stack+stack->stack_used)->level)
    {
      okReturn_ = true;
    }
  }

  return(stack->elinfo_stack+stack->stack_used);
}

// iterate over interior elements
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
traverseElLevelInteriorBorder(ALBERTA TRAVERSE_STACK *stack)
{
  FUNCNAME("traverseElLevelInteriorBorder");
  ALBERTA EL *el;
  int i;
  okReturn_ = false;
      
  if (stack->stack_used == 0)   /* first call */
  {   
    ALBERTA MACRO_EL * mel = stack->traverse_mesh->first_macro_el;
    while(this->grid_.getOwner(mel->el) != this->grid_.myRank())
    {
      mel = mel->next;
      if(!mel) break;
    }
    stack->traverse_mel = mel;
    if (stack->traverse_mel == nil)  return(nil);

    stack->stack_used = 1;

    ALBERTA fillMacroInfo(stack, stack->traverse_mel,
        stack->elinfo_stack+stack->stack_used, level_ );

    stack->info_stack[stack->stack_used] = 0;

    el = stack->elinfo_stack[stack->stack_used].el;
    if ((el == nil) || (el->child[0] == nil)) {
      return(stack->elinfo_stack+stack->stack_used);
    }
  }
  else
  {
    el = stack->elinfo_stack[stack->stack_used].el;

    /* go up in tree until we can go down again */
    while((stack->stack_used > 0) &&
    ((stack->info_stack[stack->stack_used] >= 2) || (el->child[0]==nil)
      || ( stack->traverse_level <=
          (stack->elinfo_stack+stack->stack_used)->level)) )
    {
      stack->stack_used--;
      el = stack->elinfo_stack[stack->stack_used].el;
    }
    /* goto next macro element */
    if (stack->stack_used < 1)
    {
      ALBERTA MACRO_EL * mel = stack->traverse_mel->next;
      if(mel)
      {
        while(this->grid_.getOwner(mel->el) != this->grid_.myRank())
        {
          mel = mel->next;
          if(!mel) break;
        }
      }
      stack->traverse_mel = mel;
      if (stack->traverse_mel == nil)  return(nil);

      stack->stack_used = 1;

      ALBERTA fillMacroInfo(stack, stack->traverse_mel,
          stack->elinfo_stack+stack->stack_used,level_);

      stack->info_stack[stack->stack_used] = 0;

      el = stack->elinfo_stack[stack->stack_used].el;
      if ((el == nil) || (el->child[0] == nil))
      {
        return(stack->elinfo_stack+stack->stack_used);
      }
    }
  }

  /* go down tree until leaf or level reached */
  while (el->child[0] &&
   ( stack->traverse_level > (stack->elinfo_stack+stack->stack_used)->level))
  {
    if(stack->stack_used >= stack->stack_size-1)
      enlargeTraverseStack(stack);

    i = stack->info_stack[stack->stack_used];
    el = el->child[i];
    stack->info_stack[stack->stack_used]++;

    this->grid_.fillElInfo(i, level_, stack->elinfo_stack+stack->stack_used,
                    stack->elinfo_stack+stack->stack_used+1, false, leafIt_);

    stack->stack_used++;

    ALBERTA_TEST_EXIT(stack->stack_used < stack->stack_size)
      ("stack_size=%d too small, level=(%d,%d)\n",
       stack->stack_size, stack->elinfo_stack[stack->stack_used].level);

    stack->info_stack[stack->stack_used] = 0;
    if(stack->traverse_level == (stack->elinfo_stack+stack->stack_used)->level)
    {
      okReturn_ = true;
    }
  }

  return(stack->elinfo_stack+stack->stack_used);

}

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA MACRO_EL * AlbertaGridTreeIterator<codim,pitype,GridImp>::
nextGhostMacro(ALBERTA MACRO_EL * oldmel)
{
  ALBERTA MACRO_EL * mel = oldmel;
  if(mel)
  {
    while( (!this->grid_.isGhost(mel)) )
    {
      mel = mel->next;
      if(!mel) break;
    }
  }
  return mel;
}

template<int codim, PartitionIteratorType pitype, class GridImp>   
inline ALBERTA EL_INFO * AlbertaGridTreeIterator<codim,pitype,GridImp>::
traverseElLevelGhosts(ALBERTA TRAVERSE_STACK *stack)
{
  FUNCNAME("traverseElLevelGhosts");
  ALBERTA EL *el;
  int i;
  okReturn_ = false;

  if (stack->stack_used == 0)   /* first call */
  {
    stack->traverse_mel = nextGhostMacro(stack->traverse_mesh->first_macro_el);
    if (stack->traverse_mel == nil)  return(nil);

    stack->stack_used = 1;

    ALBERTA fillMacroInfo(stack, stack->traverse_mel,
        stack->elinfo_stack+stack->stack_used,level_);

    stack->info_stack[stack->stack_used] = 0;

    el = stack->elinfo_stack[stack->stack_used].el;
    if ((el == nil) || (el->child[0] == nil)) {
      return(stack->elinfo_stack+stack->stack_used);
    }
  }
  else
  {
    el = stack->elinfo_stack[stack->stack_used].el;

    /* go up in tree until we can go down again */
    while((stack->stack_used > 0) &&
         ((stack->info_stack[stack->stack_used] >= 2) || (el->child[0]==nil)
      || ( stack->traverse_level <=
          (stack->elinfo_stack+stack->stack_used)->level)) )
      // Aenderung hier 
    {
      stack->stack_used--;
      el = stack->elinfo_stack[stack->stack_used].el;
    }
    /* goto next macro element */
    if (stack->stack_used < 1) {

      ALBERTA MACRO_EL * mel = nextGhostMacro(stack->traverse_mel->next);
      if(!mel) return 0;

      stack->traverse_mel = mel;

      stack->stack_used = 1;

      ALBERTA fillMacroInfo(stack, stack->traverse_mel,
          stack->elinfo_stack+stack->stack_used,level_);

      stack->info_stack[stack->stack_used] = 0;

      el = stack->elinfo_stack[stack->stack_used].el;
      if ((el == nil) || (el->child[0] == nil))
      {
        return(stack->elinfo_stack+stack->stack_used);
      }
    }
  }

  /* go down tree until leaf oder level*/
  while (el->child[0] && (this->grid_.getOwner(el) >= 0) &&
      ( stack->traverse_level > (stack->elinfo_stack+stack->stack_used)->level))
  {
    if(stack->stack_used >= stack->stack_size-1)
      enlargeTraverseStack(stack);

    i = stack->info_stack[stack->stack_used];
    el = el->child[i];

    stack->info_stack[stack->stack_used]++;

    // go next possible element, if not ghost  
    if( this->grid_.getOwner(el) < 0)
      return traverseElLevelGhosts(stack);

    this->grid_.fillElInfo(i, level_, stack->elinfo_stack+stack->stack_used,
                     stack->elinfo_stack+stack->stack_used+1, false);

    stack->stack_used++;

    ALBERTA_TEST_EXIT(stack->stack_used < stack->stack_size)
      ("stack_size=%d too small, level=(%d,%d)\n",
       stack->stack_size, stack->elinfo_stack[stack->stack_used].level);

    stack->info_stack[stack->stack_used] = 0;

    if(stack->traverse_level == (stack->elinfo_stack+stack->stack_used)->level)
    {
      okReturn_ = true;
    }
  }

  return(stack->elinfo_stack+stack->stack_used);

}

/*
template<int codim, PartitionIteratorType pitype, class GridImp>   
inline int AlbertaGridTreeIterator<codim,pitype,GridImp>::level() const
{
  return (manageStack_.getStack())->stack_used;
}
*/

//*************************************************************************
//  end AlbertaGridTreeIterator
//*************************************************************************

template <int dim, class GridImp>
inline AlbertaGridHierarchicIterator<GridImp> 
AlbertaGridEntity <0,dim,GridImp>::hbegin(int maxlevel) const
{
  // Kopiere alle Eintraege des stack, da man im Stack weiterlaeuft und
  // sich deshalb die Werte anedern koennen, der elinfo_stack bleibt jedoch
  // der gleiche, deshalb kann man auch nur nach unten, d.h. zu den Kindern
  // laufen
  return AlbertaGridHierarchicIterator<GridImp> (grid_,travStack_,level(),maxlevel);  
}

template <int dim, class GridImp>
inline AlbertaGridHierarchicIterator<GridImp> 
AlbertaGridEntity <0,dim,GridImp>::hend(int maxlevel) const
{
  AlbertaGridHierarchicIterator<GridImp> it(grid_,level(),maxlevel);  
  return it;
}

template <int dim, class GridImp>
inline AlbertaGridIntersectionIterator<GridImp> 
AlbertaGridEntity <0,dim,GridImp>::ibegin() const
{
  AlbertaGridIntersectionIterator<GridImp> it(grid_,level(),elInfo_);  
  return it;
}

template <int dim, class GridImp>
inline AlbertaGridIntersectionIterator<GridImp> 
AlbertaGridEntity <0,dim,GridImp>::iend() const
{
  AlbertaGridIntersectionIterator<GridImp> it(grid_,level());  
  return it;
}

//*********************************************************************
//
//  AlbertMarkerVertex
//
//*********************************************************************
inline bool AlbertaMarkerVector::
notOnThisElement(ALBERTA EL * el, int elIndex, int level, int vertex) 
{
  return (vec_[level][ vertex ] != elIndex);
}

inline bool AlbertaMarkerVector::
edgeNotOnElement(ALBERTA EL * el, int elIndex, int level, int edgenum) 
{
  return (edgevec_[level][ edgenum ] != elIndex);
}

template <class GridType, int dim> 
struct MarkEdges 
{
  inline static void mark(GridType & grid , Array<int> & vec, const ALBERTA EL * el, int count, int elindex)
  {
  }
};

// only for 3d calc edges markers 
template <class GridType> 
struct MarkEdges<GridType,3> 
{
  inline static void mark(GridType & grid , Array<int> & vec, const ALBERTA EL * el, int count, int elindex)
  {
    for(int i=0; i<count; i++)
    {
      int num = grid.hierarchicIndexSet().getIndex(el ,i, Int2Type<2>() );  
      if( vec[num] == -1 ) vec[num] = elindex;
    }
  }
};

template <class GridType>
inline void AlbertaMarkerVector::markNewVertices(GridType &grid)
{
  enum { dim      = GridType::dimension };
  enum { dimworld = GridType::dimensionworld };
  
  int nvx = grid.hierarchicIndexSet().size(dim);
#if DIM == 3
  int edg = grid.hierarchicIndexSet().size(dim-1);
#endif
  
  for(int level=0; level <= grid.maxlevel(); level++)
  {
    Array<int> & vec     = vec_[level];
    if(vec.size()     < nvx) vec.resize( nvx + vxBufferSize_ );

#if DIM == 3
    Array<int> & edgevec = edgevec_[level];
    if(edgevec.size() < edg) edgevec.resize( edg + vxBufferSize_ );
#endif
  
    for(int i=0; i<vec.size(); i++) vec[i] = -1;

    //typedef AlbertaGridMakeableEntity<0,dim,const GridType> MakeableEntityImp;
    typedef typename GridType::template Codim<0>::LevelIterator LevelIteratorType;
    LevelIteratorType endit = grid.template lend<0> (level);
    for(LevelIteratorType it = grid.template lbegin<0> (level); it != endit; ++it)
    {
      int elindex = grid.hierarchicIndexSet().index(*it); 
      for(int local=0; local<dim+1; local++)
      { 
        int num = (grid.template getRealEntity<0> (*it)).getElInfo()->el->dof[local][0]; // vertex num
        if( vec[num] == -1 ) vec[num] = elindex;
      }
    
#if DIM == 3  
      // mark edges for this element 
      MarkEdges<GridType,dim>::mark(grid,edgevec,
        (grid.template getRealEntity<0> (*it)).getElInfo()->el, 
        (grid.template getRealEntity<0> (*it)).template count<2> (), elindex );
#endif
    }
    // remember the number of entity on level and codim = 0
  }
  up2Date_ = true;
}

inline void AlbertaMarkerVector::print()
{
  for(int l=0; l<MAXL; l++)
  {
    if(vec_[l].size() > 0)
    {
      Array<int> & vec = vec_[l];
      printf("\nEntries %d \n",vec.size());
      for(int i=0; i<vec.size(); i++)
        printf("Vx %d visited on Element %d \n",i,vec[i]);
    }
  }
}

//***********************************************************************
// 
// --AlbertaGrid
// --Grid
// 
//***********************************************************************
template < int dim, int dimworld >
inline AlbertaGrid < dim, dimworld >::AlbertaGrid() : 
 mesh_ (0), maxlevel_ (0) , wasChanged_ (false)
  , isMarked_ (false) 
  , nv_ (dim+1) , dof_ (0) , myRank_ (0)
  , hIndexSet_(*this,maxHierIndex_)
  , levelIndexVec_(MAXL) 
  , leafIndexSet_ (0)
{
  for(unsigned int i=0; i<levelIndexVec_.size(); i++) levelIndexVec_[i] = 0;
  vertexMarker_ = new AlbertaMarkerVector ();

  for(int i=0; i<AlbertHelp::numOfElNumVec; i++) dofvecs_.elNumbers[i] = 0;
  dofvecs_.elNewCheck = 0;
  dofvecs_.owner      = 0;
}

template < int dim, int dimworld >
inline void AlbertaGrid < dim, dimworld >::initGrid(int proc)
{
  ALBERTA AlbertHelp::getDofVecs(&dofvecs_);
  ALBERTA AlbertHelp::setDofVec ( dofvecs_.owner, -1 );

  // dont delete dofs on higher levels 
  mesh_->preserve_coarse_dofs = 1;
  
  calcExtras();

  wasChanged_ = true;
  isMarked_ = false;

  macroVertices_.resize( mesh_->n_vertices );
  
  ALBERTA AlbertHelp::initProcessor(mesh_,proc);

  calcExtras();
}

template < int dim, int dimworld >
inline AlbertaGrid < dim, dimworld >::AlbertaGrid(const char *MacroTriangFilename) : 
 mesh_ (0), maxlevel_ (0) , wasChanged_ (false)
  , isMarked_ (false) 
  , nv_ (dim+1) , dof_ (0) , myRank_ (-1) 
  , hIndexSet_(*this,maxHierIndex_)
  , levelIndexVec_(MAXL) 
  , leafIndexSet_ (0)
{
  assert(dimworld == DIM_OF_WORLD);
  assert(dim      == DIM);

  for(unsigned int i=0; i<levelIndexVec_.size(); i++) levelIndexVec_[i] = 0;
  
  bool makeNew = true;
  { 
    std::fstream file (MacroTriangFilename,std::ios::in);
    if(!file) DUNE_THROW(AlbertaIOError,"could not open grid file " << MacroTriangFilename);
      
    std::basic_string <char> str,str1;
    file >> str1; str = str1.assign(str1,0,3);
    // With that Albert MacroTriang starts DIM or DIM_OF_WORLD
    if (str != "DIM") makeNew = false;
    file.close();
  }

  vertexMarker_ = new AlbertaMarkerVector ();
  ALBERTA AlbertHelp::initIndexManager_elmem_cc(indexStack_);

  if(makeNew)
  {
    mesh_ = ALBERTA get_mesh("AlbertaGrid", ALBERTA AlbertHelp::initDofAdmin, 
                    ALBERTA AlbertHelp::initLeafData); 
    ALBERTA read_macro(mesh_, MacroTriangFilename, ALBERTA AlbertHelp::initBoundary);

    initGrid(0);
  }
  else 
  {
    double time = 0.0;
    GrapeDataIO < AlbertaGrid <dim,dimworld> > dataIO;
    dataIO.readGrid ( *this, MacroTriangFilename,time,0);
  }
}

template < int dim, int dimworld >
inline AlbertaGrid < dim, dimworld >::
AlbertaGrid(AlbertaGrid<dim,dimworld> & oldGrid, int proc) :
  mesh_ (0), maxlevel_ (0) , wasChanged_ (false)
  , isMarked_ (false) 
  , nv_ (dim+1) , dof_ (0), myRank_ (proc)  
  , hIndexSet_(*this,maxHierIndex_)
  , levelIndexVec_(MAXL) 
  , leafIndexSet_ (0)
{
  assert(dimworld == DIM_OF_WORLD);
  assert(dim      == DIM);

  for(unsigned int i=0; i<levelIndexVec_.size(); i++) levelIndexVec_[i] = 0;
  assert(dim == 2);
  
  ALBERTA MESH * oldMesh = oldGrid.getMesh();

  vertexMarker_ = new AlbertaMarkerVector ();
  ALBERTA AlbertHelp::initIndexManager_elmem_cc(indexStack_);

  DUNE_THROW(AlbertaError,"To be revised!");
}

template < int dim, int dimworld >
inline void AlbertaGrid < dim, dimworld >::removeMesh()
{
  for(unsigned int i=0; i<levelIndexVec_.size(); i++)
    if(levelIndexVec_[i]) delete levelIndexVec_[i]; 

  if(leafIndexSet_) delete leafIndexSet_;

  if(vertexMarker_)  delete vertexMarker_;
  
  for(int i=0; i<AlbertHelp::numOfElNumVec; i++)
    if(dofvecs_.elNumbers[i])  ALBERTA free_dof_int_vec(dofvecs_.elNumbers[i]);

  if(dofvecs_.elNewCheck) ALBERTA free_dof_int_vec(dofvecs_.elNewCheck);
  if(dofvecs_.owner )     ALBERTA free_dof_int_vec(dofvecs_.owner);

#if DIM == 3
  if(mesh_) 
  {
    // because of bug in Alberta 1.2 , here until bug fixed  
    RC_LIST_EL * rclist = ALBERTA get_rc_list(mesh_);
    rclist = 0;
  }
#endif
  if(mesh_) ALBERTA free_mesh(mesh_);
}

// Desctructor 
template < int dim, int dimworld >
inline AlbertaGrid < dim, dimworld >::~AlbertaGrid()
{
  removeMesh();
}

template < int dim, int dimworld > 
template<int codim, PartitionIteratorType pitype>
inline typename AlbertaGrid<dim, dimworld>::Traits::template Codim<codim>::template Partition<pitype>::LevelIterator
AlbertaGrid < dim, dimworld >::lbegin (int level, int proc) const
{
  if((dim == codim) || ((dim == 3) && (codim == 2)) ) 
  {
    if( ! (*vertexMarker_).up2Date() ) vertexMarker_->markNewVertices(*this);
  }
  return AlbertaGridLevelIterator<codim,pitype,const MyType> (*this,vertexMarker_,level,proc);
}

template < int dim, int dimworld > template<int codim, PartitionIteratorType pitype>
inline typename AlbertaGrid<dim, dimworld>::Traits::template Codim<codim>::template Partition<pitype>::LevelIterator
AlbertaGrid < dim, dimworld >::lend (int level, int proc ) const
{
  return AlbertaGridLevelIterator<codim,pitype,const MyType> ((*this),level,proc);
}

template < int dim, int dimworld > template<int codim>
inline typename AlbertaGrid<dim, dimworld>::Traits::template Codim<codim>::template Partition<All_Partition>::LevelIterator
AlbertaGrid < dim, dimworld >::lbegin (int level, int proc) const
{
  if((dim == codim) || ((dim == 3) && (codim == 2)) ) 
  {
    if( ! (*vertexMarker_).up2Date() ) vertexMarker_->markNewVertices(*this);
  }
  return AlbertaGridLevelIterator<codim,All_Partition,const MyType> (*this,vertexMarker_,level,proc);
}

template < int dim, int dimworld > template<int codim>
inline typename AlbertaGrid<dim, dimworld>::Traits::template Codim<codim>::template Partition<All_Partition>::LevelIterator
AlbertaGrid < dim, dimworld >::lend (int level, int proc ) const
{
  return AlbertaGridLevelIterator<codim,All_Partition,const MyType> ((*this),level,proc);
}

template < int dim, int dimworld >
template<int codim, PartitionIteratorType pitype>
inline typename AlbertaGrid<dim,dimworld>::Traits::template Codim<codim>::template Partition<pitype>::LeafIterator  
AlbertaGrid < dim, dimworld >::leafbegin (int level, int proc ) const
{
  return AlbertaGridLeafIterator<codim, pitype, const MyType> (*this,vertexMarker_,level,proc);
}

template < int dim, int dimworld >
template<int codim>
inline typename AlbertaGrid<dim,dimworld>::Traits::template Codim<codim>::LeafIterator  
AlbertaGrid < dim, dimworld >::leafbegin (int level, int proc ) const {
  return leafbegin<codim, All_Partition>(level, proc);
}


template < int dim, int dimworld >
template<int codim, PartitionIteratorType pitype>
inline typename AlbertaGrid<dim,dimworld>::Traits::template Codim<codim>::template Partition<pitype>::LeafIterator  
AlbertaGrid < dim, dimworld >::leafbegin () const {
  return leafbegin<codim, pitype>(maxlevel_, -1);
}

template < int dim, int dimworld >
template<int codim>
inline typename AlbertaGrid<dim,dimworld>::Traits::template Codim<codim>::LeafIterator  
AlbertaGrid < dim, dimworld >::leafbegin () const {
  return leafbegin<codim, All_Partition>(maxlevel_, -1);
}


template < int dim, int dimworld >
template<int codim, PartitionIteratorType pitype>
inline typename AlbertaGrid<dim,dimworld>::Traits::template Codim<codim>::template Partition<pitype>::LeafIterator 
AlbertaGrid < dim, dimworld >::leafend (int level, int proc ) const
{
  return AlbertaGridLeafIterator<codim, pitype, const MyType> (*this,level,proc);
}

template < int dim, int dimworld >
template<int codim>
inline typename AlbertaGrid<dim,dimworld>::Traits::template Codim<codim>::LeafIterator 
AlbertaGrid < dim, dimworld >::leafend (int level, int proc ) const {
  return leafend<codim, All_Partition>(level, proc);
}

template < int dim, int dimworld >
template<int codim, PartitionIteratorType pitype>
inline typename AlbertaGrid<dim,dimworld>::Traits::template Codim<codim>::template Partition<pitype>::LeafIterator 
AlbertaGrid < dim, dimworld >::leafend () const {
  return leafend<codim, pitype>(maxlevel_, -1);
}

template < int dim, int dimworld >
template<int codim>
inline typename AlbertaGrid<dim,dimworld>::Traits::template Codim<codim>::LeafIterator 
AlbertaGrid < dim, dimworld >::leafend () const {
  return leafend<codim, All_Partition>(maxlevel_, -1);
}

template < int dim, int dimworld >
inline typename AlbertaGrid<dim,dimworld>::LeafIterator  
AlbertaGrid < dim, dimworld >::leafbegin (int level, int proc ) const
{
  return AlbertaGridLeafIterator<0, All_Partition, const MyType> (*this,vertexMarker_,level,proc);
}

template < int dim, int dimworld >
inline typename AlbertaGrid<dim,dimworld>::LeafIterator  
AlbertaGrid < dim, dimworld >::leafbegin () const {
  return leafbegin<0, All_Partition>(maxlevel_, -1);
}

template < int dim, int dimworld >
inline typename AlbertaGrid<dim,dimworld>::LeafIterator 
AlbertaGrid < dim, dimworld >::leafend (int level, int proc ) const
{
  return AlbertaGridLeafIterator<0, All_Partition, const MyType> (*this,level,proc);
}

template < int dim, int dimworld >
inline typename AlbertaGrid<dim,dimworld>::LeafIterator 
AlbertaGrid < dim, dimworld >::leafend () const {
  return leafend<0, All_Partition>(maxlevel_, -1);
}

//**************************************
//  refine and coarsen methods
//**************************************
//  --Adaptation
template < int dim, int dimworld >
inline bool AlbertaGrid < dim, dimworld >::
globalRefine(int refCount)
{
  typedef LeafIterator LeafIt;
  LeafIt endit = this->leafend(this->maxlevel());

  assert(refCount >= 0);
  for(int i=0; i<refCount; i++)
  {
    // mark all interior elements 
    for(LeafIt it = this->leafbegin(this->maxlevel()); it != endit; ++it)
    {
      this->mark(refCount,*it);
    }

    // mark all ghosts
    for(LeafIt it = leafbegin(maxlevel(),Ghost_Partition); it != endit; ++it)
    {
      this->mark(refCount,*it);
    }

    this->adapt();
    this->postAdapt();
  }
  return wasChanged_;
}

template < int dim, int dimworld >
inline bool AlbertaGrid < dim, dimworld >::preAdapt()
{
  return isMarked_;
}

template < int dim, int dimworld >
inline bool AlbertaGrid < dim, dimworld >::postAdapt()
{
  isMarked_ = false;
  return wasChanged_;
}


template < int dim, int dimworld >
template <class EntityType>
inline int AlbertaGrid < dim, dimworld >::owner(const EntityType & en) const
{
  return this->getOwner( (this->template getRealEntity<0>(en)).getElInfo()->el );
}

template < int dim, int dimworld >
inline int AlbertaGrid < dim, dimworld >::getOwner (ALBERTA EL *el) const
{
  // if element is new then entry in dofVec is 1 
  return ownerVec_ [el->dof[dof_][nv_]];
}

template < int dim, int dimworld >
inline bool AlbertaGrid < dim, dimworld >::isGhost(const ALBERTA MACRO_EL *mel) const
{
  assert((mel->index >= 0) && (mel->index < ghostFlag_.size()));
  return ghostFlag_[mel->index] == 1;
}

template < int dim, int dimworld >
inline bool AlbertaGrid < dim, dimworld >::isNoElement(const ALBERTA MACRO_EL *mel) const
{
  if(myRank() < 0) return false;
  assert((mel->index >= 0) && (mel->index < ghostFlag_.size()));
  return ghostFlag_[mel->index] == -1;
}

template < int dim, int dimworld >
inline void AlbertaGrid < dim, dimworld >::createGhosts()
{
  assert(myRank_ >= 0);
  if(ghostFlag_.size() < mesh_->n_macro_el) ghostFlag_.resize(mesh_->n_macro_el);
  for(ALBERTA MACRO_EL * mel = mesh_->first_macro_el; mel; mel = mel->next)
  {
    int own = getOwner(mel->el);
    //assert(own >= 0);

    int gh = 0;
    if(own != myRank_)
    {
      gh = -1;
      for(int i=0; i<dim+1; i++)
      {
        ALBERTA MACRO_EL * neigh = mel->neigh[i];
        if(neigh)
        {
          if(getOwner(neigh->el) == myRank_)
          {
            gh = 1;
            //std::cout << "Mark el " << mel->index << " as Ghost\n";
          }
        }
      }
    }
    assert((mel->index >= 0) && (mel->index < ghostFlag_.size()));
    ghostFlag_[mel->index] = gh;
  }

  /*
  std::cout << "CreateGhost of proc " << myRank_ << "\n";
  for(ALBERTA MACRO_EL * mel = mesh_->first_macro_el; mel; mel = mel->next)
  {
    std::cout << "Owner = " << getOwner(mel->el) << " flag = " << ghostFlag_[mel->index] << "\n";
  }
  */

  calcExtras ();
  //std::cout << "ende CreateGhost of proc " << myRank_ << "\n";
}


template < int dim, int dimworld >
inline void AlbertaGrid < dim, dimworld >::
unpackAll( ObjectStreamType & os ) 
{         
  //         global index, map to levels , for each level OS 
  std:: map < int , ObjectStreamType > elmap;

  int buff;
  int newmxl = 0;
  os.readObject( buff );
  //std::cout << buff << " Read buff \n";
  if(buff == ENDOFSTREAM ) return ;
  else
  {
    assert(buff == BEGINELEMENT );
    while( buff == BEGINELEMENT )
    {
      os.readObject( buff ); // read elnum 
      int elnum = buff;
      //std::cout << "Unpack el = " << elnum << "\n";
      os.readObject(buff); // read refine info  
      if(buff == BEGINELEMENT ) continue;
      if(buff == ENDOFSTREAM  ) break;
      if(buff == 1) // means that macro element has children 
      {
        //std::cout << "Read level info = " << buff << "\n";
        if(elmap.find(elnum) == elmap.end())
        {
          ObjectStreamType elstr;
          elmap[elnum] = elstr;
        }
        ObjectStreamType & elstr = elmap[elnum];

        os.readObject(buff); // read level 
        while((buff != ENDOFSTREAM) && (buff != BEGINELEMENT ))
        {
          if(buff < 0) newmxl = std::max( newmxl, std::abs( buff ));
          elstr.writeObject( buff );
          os.readObject( buff );
        }
      }
    }
  }
  std:: map < int , std::map < int , int > > elmap2;
  typedef std :: map < int , int > HierMap ;
  {
    {
      // now refine grid 
      typedef typename Traits::template Codim<0>::LevelIterator LevelIteratorType;
      LevelIteratorType endit = this->template lend<0> (0);
      for(LevelIteratorType it = this->template lbegin<0> (0);
          it != endit ; ++it )
      {
        int id = it->globalIndex();
        if(elmap.find(id) != elmap.end())
        {
          std::map < int , int > hiertree;
          elmap2[id] = hiertree;
          if(it->isLeaf()) this->mark(1,(*it));
        }
      }
    }

    this->preAdapt();
    this->adapt();
    this->postAdapt();

    typedef std :: map < int , int > HierMap ;

    for(int l=1; l<=newmxl; l++)
    {
      //std::cout << "Begin on Level l = " << l << "\n";
      // now refine grid 
      typedef typename Traits::template Codim<0>::LevelIterator LevelIteratorType;
      LevelIteratorType endit  = this->template lend<0>   (0);
      for(LevelIteratorType it = this->template lbegin<0> (0);
          it != endit ; ++it )
      {
        int id = it->globalIndex();
        //std::cout << "Begin LevelIter it = " << id << "\n";
        if(elmap.find(id) != elmap.end())
        {
          int buff;
          // build a new hier tree 
          ObjectStreamType & levstr = elmap[id];
          try {
            levstr.readObject( buff );
          }
          catch (ObjectStreamType :: EOFException)
          {
            continue;
          }
          assert( buff < 0);
          assert( std::abs( buff ) == l );

          HierMap  & hiertree = elmap2[id];
          typedef typename Traits:: template Codim<0> :: Entity :: HierarchicIterator HierIt;

          // Hier muss eine ineinandergeschateltes HierarchiIt kommen.

          typedef typename Traits:: template Codim<0> :: Entity EntityType;
          typedef typename Traits:: template Codim<0> :: EntityPointer EntityPointer;

          hiertree[id] = 1;

          HierIt hendit = it->hend(l);
          for(HierIt hit = it->hbegin(l); hit != hendit; ++hit)
          {
            if(hit->level() != l) continue;
            // if father isnt in tree then we dont do anything here
            EntityPointer vati = hit->father();
            if( hiertree.find( vati->globalIndex() ) == hiertree.end()) continue;

            int mark;
            //try {
            levstr.readObject ( mark );
            //}
            //catch (ObjectStreamType :: EOFException) {}
            if(mark == 1)
            {
              hiertree[hit->globalIndex()] = mark;
              if(hit->isLeaf()) this->mark(1,(*hit));
            }
          }
          //std::cout << "Hier it done \n";
        }
      }

      //std::cout << "Begin Adapt \n";
      this->preAdapt();
      this->adapt();
      this->postAdapt();
    }
  }
  

}

template < int dim, int dimworld >
template <class EntityType>
inline void AlbertaGrid < dim, dimworld >::
packAll( ObjectStreamType & os, EntityType & en  )  
{         
  assert( en.level() == 0 ); // call only on macro elements 
  os.writeObject( BEGINELEMENT );
  os.writeObject( en.globalIndex ());
  //std::cout << "Pack el = " << globalIndex () << "\n";
  //std::cout << isLeaf() << " children? \n"; 
  
  if(! (en.isLeaf()) )
  {
    int mxl = this->maxlevel();
    os.writeObject( 1 ); // this element should be refined 
    
    for(int l=1;l<mxl; l++)
    { 
      os.writeObject( -l ); // store level in negative form 
      // to distinguish between mark and level 
      // walk only over desired level 
      typedef typename EntityType :: HierarchicIterator HierIt;
      HierIt endit  = en.hend(l);
      for(HierIt it = en.hbegin(l); it != endit; ++it)
      {
        if(it->level() != l) continue;
        os.writeObject( (it->isLeaf()) ? STOPHERE : REFINEEL );

        // mark element for coarsening 
        this->mark( -1, (*it) ) ;
      }
    }
  }
  //std::cout << "Done with element !\n";
  return ;
}

template < int dim, int dimworld >
template <class EntityType>
inline void AlbertaGrid < dim, dimworld >::
packBorder ( ObjectStreamType & os, EntityType & en  ) 
{         
  assert( en.level() == 0 ); // call only on macro elements 
  os.writeObject( BEGINELEMENT );
  os.writeObject( en.globalIndex ());

  if(! (en.isLeaf()) )
  {
    int mxl = this->maxlevel();
    os.writeObject( 1 ); // this element should be refined 

    for(int l=1;l<mxl; l++)
    {
      os.writeObject( -l ); // store level in negative form 
      // to distinguish between mark and level 
      typedef typename EntityType :: HierarchicIterator HierIt;
      HierIt endit  = en.hend(l);
      for(HierIt it = en.hbegin(l); it != endit; ++it)
      {
        if(it->level() != l) continue;
        if((en.partitionType() != BorderEntity) || (it->isLeaf()))
        {
          os.writeObject( STOPHERE );
        }
        else
          os.writeObject( 1 );
      }
    }
  }
  return ;
}

template<int dim, int dimworld>
inline bool AlbertaGrid < dim, dimworld >:: 
mark( int refCount , typename Traits::template Codim<0>::EntityPointer & ep ) const
{
  return this->mark(refCount,*ep);
}

template <int dim, int dimworld>
template <class EntityType>
inline int AlbertaGrid < dim, dimworld >:: 
getMark( const EntityType & ep ) const 
{
  return (this->template getRealEntity<0>(ep)).getElInfo()->el->mark;
}

template<int dim, int dimworld>
inline bool AlbertaGrid < dim, dimworld >:: 
mark( int refCount , const typename Traits::template Codim<0>::Entity & ep ) const
{
  ALBERTA EL_INFO * elInfo = (this->template getRealEntity<0>(ep)).getElInfo();
  if(!elInfo) return false;
  assert(elInfo);

  if( ep.isLeaf() )
  {
    // we can not mark for coarsening if already marked for refinement
    if((refCount < 0) && (elInfo->el->mark > 0))
    {
      //dverb << "WARNING:  AlbertaGrid::mark: Could not mark element for coarsening, it was marked for refinement before! in: " << __FILE__ << "  line: " << __LINE__ << "\n";
      return false;
    }

    if( refCount > 0)
    {
      elInfo->el->mark = 1;
      return true;
    }
    if( refCount < 0)
    {
      this->setMark ( true );
      elInfo->el->mark = -1;
      return true;
    }
  }
  //dwarn << "WARNING: in AlbertaGrid<"<<dim<<","<<dimworld<<">::mark("<<refCount<<",EP &) : called on non LeafEntity! in: " << __FILE__ << " line: "<< __LINE__ << "\n"; 
  elInfo->el->mark = 0;
  return false;
}


template < int dim, int dimworld >
inline bool AlbertaGrid < dim, dimworld >::adapt()
{
  unsigned char flag;
  bool refined = false;
  wasChanged_ = false;
 
  // set global pointer to index manager in elmem.cc
  ALBERTA AlbertHelp::initIndexManager_elmem_cc( indexStack_ );
  ALBERTA AlbertHelp::clearDofVec ( dofvecs_.elNewCheck );

  flag = ALBERTA AlbertRefine ( mesh_ );
  refined = (flag == 0) ? false : true;
 
  if(isMarked_) // true if a least on element is marked for coarseing
    flag = ALBERTA AlbertCoarsen( mesh_ );

  if(!refined)
  {
    wasChanged_ = (flag == 0) ? false : true;
  }
  else 
    wasChanged_ = true;

  if(wasChanged_)
  {
    calcExtras();
    isMarked_ = false;
  }

  ALBERTA AlbertHelp::setElOwnerNew(mesh_, dofvecs_.owner);

  // remove global pointer in elmem.cc
  ALBERTA AlbertHelp::removeIndexManager_elmem_cc();

  return refined;
}

template < int dim, int dimworld >
template <class DofManagerType, class RestrictProlongOperatorType>
inline bool AlbertaGrid < dim, dimworld >::
adapt(DofManagerType &, RestrictProlongOperatorType &, bool verbose)
{
  unsigned char flag;
  bool refined = false;
  wasChanged_ = false;

  std::cerr << "Method adapt 2 not implemented! in: " << __FILE__ << " line: " << __LINE__ << "\n";
  abort();
 /* 
 
  // set global pointer to index manager in elmem.cc
  ALBERTA AlbertHelp::initIndexManager_elmem_cc( indexStack_ );
  ALBERTA AlbertHelp::clearDofVec ( dofvecs_.elNewCheck );

  flag = ALBERTA AlbertRefine ( mesh_ );
  refined = (flag == 0) ? false : true;
 
  if(isMarked_) // true if a least on element is marked for coarseing
    flag = ALBERTA AlbertCoarsen( mesh_ );

  if(!refined)
  {
    wasChanged_ = (flag == 0) ? false : true;
  }
  else 
    wasChanged_ = true;

  if(wasChanged_)
  {
    calcExtras();
    isMarked_ = false;
  }

  ALBERTA AlbertHelp::setElOwnerNew(mesh_, dofvecs_.owner);

  // remove global pointer in elmem.cc
  ALBERTA AlbertHelp::removeIndexManager_elmem_cc();
  */
  return refined;
}

template < int dim, int dimworld >
inline void AlbertaGrid < dim, dimworld >::setMark (bool isMarked) const
{
  isMarked_ = isMarked;
}

template < int dim, int dimworld >
inline bool AlbertaGrid < dim, dimworld >::checkElNew (ALBERTA EL *el) const 
{
  // if element is new then entry in dofVec is 1 
  return (elNewVec_[el->dof[dof_][nv_]] > 0);
}

template < int dim, int dimworld >
template <class EntityType> 
inline bool AlbertaGrid < dim, dimworld >::
partition( int proc , EntityType & en ) 
{
  return this->setOwner( (this->template getRealEntity<0>(en)).getElInfo()->el , proc );
}

template < int dim, int dimworld >
inline bool AlbertaGrid < dim, dimworld >::setOwner (ALBERTA EL *el, int proc)  
{
  // if element is new then entry in dofVec is 1 
  int dof = el->dof[dof_][nv_];
  if(ownerVec_ [dof] < 0)
  {
    ownerVec_ [dof] = proc;
    return true;
  }
  return false;
}

template < int dim, int dimworld >
inline PartitionType AlbertaGrid < dim, dimworld >::
partitionType (ALBERTA EL_INFO *elinfo) const
{
  int owner = getOwner(elinfo->el);

  // if processor number == myRank ==> InteriorEntity or BorderEntity
  if(owner == myRank()) 
  {
    for(int i=0; i<dim+1; i++)
    {
      ALBERTA EL * neigh = NEIGH(elinfo->el,elinfo)[i];
      if(neigh) 
      {
        if(getOwner(neigh) != myRank())
          return BorderEntity;
      }
    }  

    // if no GhostNeighbor, then we have real InteriorEntity
    return InteriorEntity;
  }
  
  // if processor number != myProcossor ==> GhostEntity
  if((owner >= 0) && (owner != myRank())) return GhostEntity;

  DUNE_THROW(AlbertaError, "Unsupported PartitionType");
  
  return OverlapEntity;
}
template < int dim, int dimworld >
inline int AlbertaGrid < dim, dimworld >::maxlevel() const
{
  return maxlevel_;
}

template < int dim, int dimworld >
inline int AlbertaGrid < dim, dimworld >::global_size (int codim) const
{
  if(codim == dim) 
    return mesh_->n_vertices;
  // at this moment only for codim=0 and codim=dim
  //assert((codim == dim) || (codim == 0));
  return indexStack_[codim].getMaxIndex()+1;
}

template < int dim, int dimworld >
inline int AlbertaGrid < dim, dimworld >::size (int level, int codim) const
{
  return this->levelIndexSet(level).size(codim); 
}

template < int dim, int dimworld > 
inline void AlbertaGrid < dim, dimworld >::arrangeDofVec()
{
  hIndexSet_.updatePointers(dofvecs_);

  elNewVec_ = (dofvecs_.elNewCheck)->vec;  assert(elNewVec_);
  ownerVec_ = (dofvecs_.owner)->vec;       assert(ownerVec_);
  elAdmin_ = dofvecs_.elNumbers[0]->fe_space->admin;

  // see Albert Doc. , should stay the same 
  const_cast<int &> (nv_)  = elAdmin_->n0_dof[CENTER];
  const_cast<int &> (dof_) = elAdmin_->mesh->node[CENTER];
}


template < int dim, int dimworld > 
inline int AlbertaGrid < dim, dimworld >::getElementNumber ( ALBERTA EL * el ) const
{
  return hIndexSet_.getIndex(el,0,Int2Type<dim>());
};

template < int dim, int dimworld > 
inline int AlbertaGrid < dim, dimworld >::getEdgeNumber ( ALBERTA EL * el , int i ) const
{
  assert(dim == 3);
  return hIndexSet_.getIndex(el,i,Int2Type<dim-1>());
};

template < int dim, int dimworld > 
inline int AlbertaGrid < dim, dimworld >::getVertexNumber ( ALBERTA EL * el , int vx ) const
{
  return hIndexSet_.getIndex(el,vx,Int2Type<0>());
};

template < int dim, int dimworld > 
inline void AlbertaGrid < dim, dimworld >::calcExtras ()
{
  // store pointer to numbering vectors and so on 
  arrangeDofVec ();
 
  // determine new maxlevel and mark neighbours 
  maxlevel_ = ALBERTA AlbertHelp::calcMaxLevel(mesh_);

  // calculate the new maximal index used, this value+1 is then used as size 
  for(int i=0; i<ALBERTA AlbertHelp::numOfElNumVec; i++)
    maxHierIndex_[i] = indexStack_[i].getMaxIndex();

  maxHierIndex_[dim] = mesh_->n_vertices;

  // unset up2Dat status, if lbegin is called then this status is updated 
  vertexMarker_->unsetUp2Date();

  // if levelIndexSet exists, then update now 
  for(unsigned int i=0; i<levelIndexVec_.size(); i++)
    if(levelIndexVec_[i]) (*levelIndexVec_[i]).calcNewIndex();

  if( leafIndexSet_ ) (*leafIndexSet_).resize();

  // we have a new grid 
  wasChanged_ = true;
}

template < int dim, int dimworld >  template <GrapeIOFileFormatType ftype> 
inline bool AlbertaGrid < dim, dimworld >::
writeGrid (const std::basic_string<char> filename, albertCtype time ) const
{
  // use only with xdr as filetype 
  assert(ftype == xdr); 
  return writeGridXdr (filename , time ); 
}

template < int dim, int dimworld >  template <GrapeIOFileFormatType ftype> 
inline bool AlbertaGrid < dim, dimworld >::
readGrid (const std::basic_string<char> filename, albertCtype &time )
{
  switch(ftype)
  { 
    case xdr   : return readGridXdr   (filename , time );
    case ascii : return readGridAscii (filename , time );         
    default : {
                DUNE_THROW(AlbertaError,"wrong FileType in AlbertaGrid::readGrid!"); 
                return false;
              }
  }
}

template < int dim, int dimworld > 
inline bool AlbertaGrid < dim, dimworld >::
writeGridXdr (const std::basic_string<char> filename, albertCtype time ) const
{
  std::ostringstream ownerfile;
  if(filename.size() > 0)
  {
    ownerfile << filename;
    ownerfile << "_own";
  }
  else 
    DUNE_THROW(AlbertaIOError, "no filename given in writeGridXdr ");

  // strore element numbering to file 
  for(int i=0; i<AlbertHelp::numOfElNumVec; i++)
  {
    std::ostringstream elnumfile;
    elnumfile << filename;
    elnumfile << "_num_c" << i;
    ALBERTA write_dof_int_vec_xdr(dofvecs_.elNumbers[i],elnumfile.str().c_str());
  }

  if(myRank() >= 0)
  {
    int val = -1;
    int entry = ALBERTA AlbertHelp::saveMyProcNum(dofvecs_.owner,myRank(),val);
   
    ALBERTA write_dof_int_vec_xdr(dofvecs_.owner,ownerfile.str().c_str());

    // set old value of owner vec
    dofvecs_.owner->vec[entry] = val;
  }
  
  // use write_mesh_xdr, but works not correctly 
  return static_cast<bool> (ALBERTA write_mesh_xdr (mesh_ , filename.c_str(), time) );
}

template < int dim, int dimworld > 
inline bool AlbertaGrid < dim, dimworld >::
readGridXdr (const std::basic_string<char> filename, albertCtype & time )
{
  // use read_mesh_xdr, but works not correctly 
  mesh_ = (ALBERTA read_mesh_xdr (filename.c_str(), &time , ALBERTA AlbertHelp::initLeafData , 
                                ALBERTA AlbertHelp::initBoundary) );
  if (mesh_ == 0)
    DUNE_THROW(AlbertaIOError, "could not open grid file " << filename);
  
  // read element numbering from file 
  std::ostringstream ownerfile;
  if(filename.size() > 0)
  {
    ownerfile << filename;
    ownerfile << "_own";
  }
  else 
    return false;

  for(int i=0; i<AlbertHelp::numOfElNumVec; i++)
  {
    std::ostringstream elnumfile;
    elnumfile << filename;
    elnumfile << "_num_c" << i;
    std::cout << elnumfile.str() << " filename " << "\n";
    dofvecs_.elNumbers[i] = ALBERTA read_dof_int_vec_xdr(elnumfile.str().c_str(), mesh_ , 0 );
  }

  // if owner file exists, read it 
  {
    dofvecs_.owner = 0;
    FILE * file=0;
    const char * ownfile = ownerfile.str().c_str();
      
    file = fopen(ownfile,"r");
    if(file) 
    {
      fclose(file);
      dofvecs_.owner = ALBERTA read_dof_int_vec_xdr(ownfile, mesh_ , 0 );
      const_cast<int &> (myRank_) = ALBERTA AlbertHelp::restoreMyProcNum(dofvecs_.owner);
    }
  }
  
  // make the rest of the dofvecs 
  ALBERTA AlbertHelp::makeTheRest(&dofvecs_);

  arrangeDofVec();
  
  // calc maxlevel and indexOnLevel and so on 
  calcExtras();
  
  // set el_index of index manager to max element index 
  for(int i=0; i<ALBERTA AlbertHelp::numOfElNumVec; i++)
  {
    int maxIdx = ALBERTA AlbertHelp::calcMaxIndex( dofvecs_.elNumbers[i] );
    indexStack_[i].setMaxIndex(maxIdx);
  }
  
  return true;
}

template < int dim, int dimworld > 
inline bool AlbertaGrid < dim, dimworld >::readGridAscii 
(const std::basic_string<char> filename, albertCtype & time )
{
  removeMesh(); // delete all objects 
  
  mesh_ = ALBERTA get_mesh("AlbertaGrid", ALBERTA AlbertHelp::initDofAdmin, 
                  ALBERTA AlbertHelp::initLeafData); 
  ALBERTA read_macro(mesh_, filename.c_str(), ALBERTA AlbertHelp::initBoundary);
  
  if( !readParameter(filename,"Time",time) )
    time = 0.0;
    
  vertexMarker_ = new AlbertaMarkerVector (); assert(vertexMarker_);
  ALBERTA AlbertHelp::initIndexManager_elmem_cc(indexStack_);

  initGrid(myRank_);
  return true;
}

// if defined some debugging test were made that reduce the performance
// so they were switch off normaly 

//#define DEBUG_FILLELINFO
//*********************************************************************
//  fillElInfo 2D
//*********************************************************************
#define CALC_COORD
typedef U_CHAR ALBERTA_CHAR;

template<int dim, int dimworld>
inline void AlbertaGrid<dim,dimworld >::
firstNeigh(const int ichild, const ALBERTA EL_INFO *elinfo_old, 
           ALBERTA EL_INFO *elinfo, const bool leafLevel) const
{
  // old stuff 
  const ALBERTA REAL_D * old_opp_coord  = elinfo_old->opp_coord;
  const ALBERTA REAL_D * old_coord      = elinfo_old->coord;

  // new stuff 
  ALBERTA ALBERTA_CHAR * opp_vertex     = elinfo->opp_vertex;
  ALBERTA EL ** neigh = NEIGH(el,elinfo);
  ALBERTA REAL_D * opp_coord = elinfo->opp_coord;

  assert(neigh != 0);
  assert(neigh == NEIGH(el,elinfo));

  const int onechi = 1-ichild;
  
  // first nb 0 of new elinfo
  {
    ALBERTA EL * nb = NEIGH(elinfo_old->el,elinfo_old)[2];
    if(nb)
    {
      // if NULL then nonconforme refinement 
      assert(nb->child[0] != 0);
      ALBERTA EL * nextNb = nb->child[onechi];
      opp_vertex[ichild] = onechi;
#ifdef CALC_COORD
      for(int i=0; i<dimworld; i++)
      opp_coord[ichild][i]  = old_opp_coord[2][i]; 
#endif
      // if we have children, we could go down 
      if(nextNb->child[0])
      { 
        // if level of neighbour to small, do down once more 
        // but only one level down because of conformity 
        if( leafLevel ) 
        {
          nextNb = nextNb->child[ichild];
          opp_vertex[ichild] = 2;
#ifdef CALC_COORD
          for(int i=0; i<dimworld; i++)
            opp_coord[ichild][i] += old_coord[ichild][i]; 
          for(int i=0; i<dimworld; i++)
            opp_coord[ichild][i] *= 0.5; 
#endif
        }
      }
      neigh[ichild] = nextNb;  
    }
    else 
    {
      // if no neighbour then children have no neighbour 
      neigh[ichild] = 0;
    }
  }
}

template<int dim, int dimworld>
inline void AlbertaGrid<dim,dimworld >::
secondNeigh(const int ichild, const ALBERTA EL_INFO *elinfo_old, 
            ALBERTA EL_INFO *elinfo, const bool leafLevel) const
{
  // old stuff 
  const ALBERTA REAL_D * old_coord      = elinfo_old->coord;

  // new stuff 
  ALBERTA ALBERTA_CHAR * opp_vertex     = elinfo->opp_vertex;
  ALBERTA EL ** neigh = NEIGH(el,elinfo);
  ALBERTA REAL_D * opp_coord = elinfo->opp_coord;

  assert(neigh != 0);
  assert(neigh == NEIGH(el,elinfo));

  const int onechi = 1-ichild;
  // nb 1 of new elinfo, always the same 
  {
    ALBERTA EL * nextNb = elinfo_old->el->child[onechi]; 
    opp_vertex[onechi] = ichild;
#ifdef CALC_COORD
    for(int i=0; i<dimworld; i++)
      opp_coord[onechi][i]  = old_coord[onechi][i]; 
#endif        
    // check if children exists
    if(nextNb->child[0] )
    {
      if( leafLevel )
      {
        nextNb = nextNb->child[onechi];
        opp_vertex[onechi] = 2;
#ifdef CALC_COORD
        for(int i=0; i<dimworld; i++)
          opp_coord[onechi][i] += old_coord[2][i];
        for(int i=0; i<dimworld; i++)
          opp_coord[onechi][i] *= 0.5;
#endif
      }
    }
    neigh[onechi] = nextNb; 
  }
}

template<int dim, int dimworld>
inline void AlbertaGrid<dim,dimworld >::
thirdNeigh(const int ichild, const ALBERTA EL_INFO *elinfo_old, 
           ALBERTA EL_INFO *elinfo, const bool leafLevel) const
{
  // old stuff 
  const ALBERTA ALBERTA_CHAR * old_opp_vertex = elinfo_old->opp_vertex;
  const ALBERTA REAL_D * old_opp_coord  = elinfo_old->opp_coord;
  const ALBERTA REAL_D * old_coord      = elinfo_old->coord;

  // new stuff 
  ALBERTA ALBERTA_CHAR * opp_vertex     = elinfo->opp_vertex;
  ALBERTA EL ** neigh = NEIGH(el,elinfo);
  ALBERTA REAL_D * opp_coord = elinfo->opp_coord;

  assert(neigh != 0);
  assert(neigh == NEIGH(el,elinfo));

  const int onechi = 1-ichild;
  // nb 2 of new elinfo 
  {
    ALBERTA EL * nb = NEIGH(elinfo_old->el,elinfo_old)[onechi];
    if(nb)
    {
      const int vx = old_opp_vertex[onechi];
      opp_vertex[2] = vx;
#ifdef CALC_COORD
      for(int i=0; i<dimworld; i++)
        opp_coord[2][i] = old_opp_coord[onechi][i]; 
#endif  
      if((vx == 2) || (nb->child[0] == 0))
      {
        // means the neighbour has the same refinement edge like our child 
        neigh[2] = nb;
      }
      else 
      {
        assert(nb->child[0] != 0);
        neigh[2] = nb->child[1-vx];
        opp_vertex[2] = 2;
#ifdef CALC_COORD
        const int vxind = (vx == ichild) ? ichild : 2;
        for(int i=0; i<dimworld; i++)
          opp_coord[2][i] += old_coord[vxind][i]; 
        for(int i=0; i<dimworld; i++)
          opp_coord[2][i] *= 0.5; 
#endif        
      }
    }
    else 
    {
       // if no neighbour then children have no neighbour 
       neigh[2] = 0;
    }
  }

}

template<int dim, int dimworld>
inline void AlbertaGrid<dim,dimworld >::
fillElInfo(int ichild, int actLevel , const ALBERTA EL_INFO *elinfo_old, 
    ALBERTA EL_INFO *elinfo, bool hierarchical, bool leaf) const
{
#if 0
  // the alberta version of filling an EL_INFO structure
  ALBERTA fill_elinfo(ichild,elinfo_old,elinfo);
#else 
    
  // old stuff 
  ALBERTA EL * el_old = elinfo_old->el;
  assert(el_old != 0);
  assert(el_old->child[0] != 0);
  
  // new stuff 
  // set new element
  ALBERTA EL * el  = el_old->child[ichild];
  elinfo->el = el;

  ALBERTA FLAGS   fill_flag = elinfo_old->fill_flag;

  elinfo->macro_el  = elinfo_old->macro_el;
  elinfo->fill_flag = fill_flag;
  elinfo->mesh      = elinfo_old->mesh;
  elinfo->parent    = el_old;
  elinfo->level     = elinfo_old->level + 1;
  
  // calculate the coordinates 
  if (fill_flag & FILL_COORDS)
  {
    if (el_old->new_coord) 
    {
      for (int j = 0; j < dimworld; j++)
        elinfo->coord[2][j] = el_old->new_coord[j];
    }
    else 
    {
      for (int j = 0; j < dimworld; j++)
        elinfo->coord[2][j] =
          0.5 * (elinfo_old->coord[0][j] + elinfo_old->coord[1][j]);
    }

    // set the other coord 
    for (int j = 0; j < dimworld; j++) 
    {
      elinfo->coord[ichild  ][j] = elinfo_old->coord[2][j];
      elinfo->coord[1-ichild][j] = elinfo_old->coord[ichild][j];
    }
  }

  // calculate neighbours 
  if(fill_flag & FILL_NEIGH)
  {
    // allow to go down on neighbour more than once 
    // if the following condition is satisfied 
    const bool leafLevel = ((el->child[0] == 0) && (elinfo->level < actLevel));
    firstNeigh (ichild,elinfo_old,elinfo,leafLevel);
    secondNeigh(ichild,elinfo_old,elinfo,leafLevel);
    thirdNeigh (ichild,elinfo_old,elinfo,leafLevel);
  }

  // boundary calculation 
  if (fill_flag & FILL_BOUND)
  {
    if (elinfo_old->boundary[2])
      elinfo->bound[2] = elinfo_old->boundary[2]->bound;
    else
      elinfo->bound[2] = INTERIOR;

    elinfo->bound[ichild]   = elinfo_old->bound[2];
    elinfo->bound[1-ichild] = elinfo_old->bound[ichild];
    elinfo->boundary[ichild] = elinfo_old->boundary[2];
    elinfo->boundary[1-ichild] = nil;
    elinfo->boundary[2] = elinfo_old->boundary[1-ichild];
  }

#endif
} // end Grid::fillElInfo 2D


//***********************************************************************
// fillElInfo 3D
//***********************************************************************
#if DIM == 3
template <> 
inline void AlbertaGrid<3,3>::
fillElInfo(int ichild, int actLevel , const ALBERTA EL_INFO *elinfo_old, 
    ALBERTA EL_INFO *elinfo, bool hierarchical, bool leaf) const
{
  enum { dim = 3 };
  enum { dimworld = 3 };

#if 1
  ALBERTA fill_elinfo(ichild,elinfo_old,elinfo);
#else 
  static S_CHAR child_orientation[3][2] = {{1,1}, {1,-1}, {1,-1}};
 
  int     k;
  int     el_type=0;                                 /* el_type in {0,1,2} */
  int     ochild=0;                     /* index of other child = 1-ichild */
  int     *cv=nil;                     /* cv = child_vertex[el_type][ichild] */
  int     (*cvg)[4]=nil;                      /* cvg = child_vertex[el_type] */
  int     *ce;                       /* ce = child_edge[el_type][ichild] */
  int     iedge;
  EL      *nb, *nbk, **neigh_old;
  EL      *el_old = elinfo_old->el;
  FLAGS   fill_flag = elinfo_old->fill_flag;
  DOF    *dof;
#if !NEIGH_IN_EL
  int     ov;
  EL      **neigh;
  FLAGS   fill_opp_coords;
  U_CHAR  *opp_vertex;
#endif

  TEST_EXIT(el_old->child[0])("missing child?\n");  /* Kuni 22.08.96 */

  elinfo->el        = el_old->child[ichild];
  elinfo->macro_el  = elinfo_old->macro_el;
  elinfo->fill_flag = fill_flag;
  elinfo->mesh      = elinfo_old->mesh;
  elinfo->parent    = el_old;
  elinfo->level     = elinfo_old->level + 1;
#if !NEIGH_IN_EL
  elinfo->el_type   = (elinfo_old->el_type + 1) % 3;
#endif

  REAL_D * opp_coord = elinfo->opp_coord;
  REAL_D * coord = elinfo->coord;

  const REAL_D * old_coord = elinfo_old->coord;
  const REAL_D * oldopp_coord = elinfo_old->opp_coord;


  TEST_EXIT(elinfo->el)("missing child %d?\n", ichild);

  if (fill_flag) {
    el_type = EL_TYPE(elinfo_old->el, elinfo_old);
    cvg = child_vertex[el_type];
    cv = cvg[ichild];
    ochild = 1-ichild;
  }

  if (fill_flag & FILL_COORDS)
  {
    for (int i=0; i<3; i++) {
      for (int j = 0; j < dimworld; j++) {
  coord[i][j] = old_coord[cv[i]][j];
      }
    }
    if (el_old->new_coord)
      for (int j = 0; j < dimworld; j++) 
  coord[3][j] = el_old->new_coord[j];
    else
      for (int j = 0; j < dimworld; j++)
  coord[3][j] = 0.5*
    (old_coord[0][j] + old_coord[1][j]);
  }


#if NEIGH_IN_EL
  if (fill_flag & FILL_OPP_COORDS)
  {
    neigh_old = el_old->neigh;

    /*----- nb[0] is other child -------------------------------------------*/

/*    if (nb = el_old->child[ochild]) {        old version    */
    if (el_old->child[0]  &&  (nb = el_old->child[ochild])) /* Kuni 22.08.96*/
    {        
      if (nb->child[0]) {     /* go down one level for direct neighbour */
  k = cvg[ochild][1];
  if (nb->new_coord)
    for (int j = 0; j < dimworld; j++)
      opp_coord[0][j] = nb->new_coord[j];
  else
    for (int j = 0; j < dimworld; j++)
      opp_coord[0][j] = 0.5*
        (old_coord[ochild][j] + old_coord[k][j]);
      }
      else {
  for (int j = 0; j < dimworld; j++) {
    opp_coord[0][j] = old_coord[ochild][j];
  }
      }
    }
    else {
      ERROR_EXIT("no other child");
    }


/*----- nb[1],nb[2] are childs of old neighbours nb_old[cv[i]] ----------*/

    for (int i=1; i<3; i++) 
    { 
      if (nb = neigh_old[cv[i]]) 
      {
  TEST_EXIT(nb->child[0])("nonconforming triangulation\n");

  for (k=0; k<2; k++) {     /* look at both children of old neighbour */

    nbk = nb->child[k];
    if (nbk->dof[0] == el_old->dof[ichild]) {
      dof = nb->dof[el_old->opp_vertex[cv[i]]];      /* opp. vertex */
      if (dof == nbk->dof[1]) {
        if (nbk->child[0]) {
    if (nbk->new_coord)
      for (int j = 0; j < dimworld; j++)
        opp_coord[i][j] = nbk->new_coord[j];
    else
      for (int j = 0; j < dimworld; j++)
        opp_coord[i][j] = 0.5* 
          (oldopp_coord[cv[i]][j] + old_coord[ichild][j]);
    break;
        }
      }
      else {
        TEST_EXIT(dof == nbk->dof[2])("opp_vertex not found\n");
      }

      for (int j = 0; j < dimworld; j++) {
        opp_coord[i][j] = oldopp_coord[cv[i]][j];
      }
      break;
    }
    
  } /* end for k */
  TEST_EXIT(k<2)("child not found with vertex\n");

      }
    }  /* end for i */


/*----- nb[3] is old neighbour neigh_old[ochild] ------------------------*/

    if (neigh_old[ochild]) {
      for (int j = 0; j < dimworld; j++) {
  opp_coord[3][j] = oldopp_coord[ochild][j];
      }
    }

  }

#else  /* ! NEIGH_IN_EL */

  if (fill_flag & (FILL_NEIGH | FILL_OPP_COORDS))
  {
    neigh      = elinfo->neigh;
    neigh_old  = (EL **) elinfo_old->neigh;
    opp_vertex = (U_CHAR *) &(elinfo->opp_vertex);
    fill_opp_coords = (fill_flag & FILL_OPP_COORDS);

/*----- nb[0] is other child --------------------------------------------*/

/*    if (nb = el_old->child[ochild])   old version  */
    if (el_old->child[0]  &&  (nb = el_old->child[ochild])) /*Kuni 22.08.96*/
    {
      if (nb->child[0])
      {     /* go down one level for direct neighbour */
  if (fill_opp_coords)
  {
    if (nb->new_coord)
    {
      for (int j = 0; j < dimworld; j++)
        opp_coord[0][j] = nb->new_coord[j];
    }
    else
    {
      k = cvg[ochild][1];
      for (int j = 0; j < dimworld; j++)
        opp_coord[0][j] = 0.5*
    (old_coord[ochild][j] + old_coord[k][j]);
    }
  }
  neigh[0]      = nb->child[1];
  opp_vertex[0] = 3;
      }
      else {
  if (fill_opp_coords) {
    for (int j = 0; j < dimworld; j++) {
      opp_coord[0][j] = old_coord[ochild][j];
    }
  }
  neigh[0]      = nb;
  opp_vertex[0] = 0;
      }
    }
    else {
      ERROR_EXIT("no other child");
      neigh[0] = nil;
    }


/*----- nb[1],nb[2] are childs of old neighbours nb_old[cv[i]] ----------*/

    for (int i=1; i<3; i++)
    { 
      if ((nb = neigh_old[cv[i]])) 
      {
  TEST_EXIT(nb->child[0])("nonconforming triangulation\n");

  for (k=0; k<2; k++)  /* look at both childs of old neighbour */
  {       
    nbk = nb->child[k];
    if (nbk->dof[0] == el_old->dof[ichild]) {
      dof = nb->dof[elinfo_old->opp_vertex[cv[i]]]; /* opp. vertex */
      if (dof == nbk->dof[1]) 
      {
        ov = 1;
        if (nbk->child[0])
        {
    if (fill_opp_coords)
    {
      if (nbk->new_coord)
        for (int j = 0; j < dimworld; j++)
          opp_coord[i][j] = nbk->new_coord[j];
      else
        for (int j = 0; j < dimworld; j++)
          opp_coord[i][j] = 0.5*
      (oldopp_coord[cv[i]][j]
       + old_coord[ichild][j]);
    }
    neigh[i]      = nbk->child[0];
    opp_vertex[i] = 3;
    break;
        }
      }
      else
      {
        TEST_EXIT(dof == nbk->dof[2])("opp_vertex not found\n");
        ov = 2;
      }

      if (fill_opp_coords)
      {
        for (int j = 0; j < dimworld; j++)
        {
    opp_coord[i][j] = oldopp_coord[cv[i]][j];
        }
      }
      neigh[i]      = nbk;
      opp_vertex[i] = ov;
      break;
    }
    
  } /* end for k */
  TEST_EXIT(k<2)("child not found with vertex\n");
      }
      else 
      {
  neigh[i] = nil;
      }
    }  /* end for i */


/*----- nb[3] is old neighbour neigh_old[ochild] ------------------------*/

    if ((neigh[3] = neigh_old[ochild]))
    {
      opp_vertex[3] = elinfo_old->opp_vertex[ochild];
      if (fill_opp_coords) {
  for (int j = 0; j < dimworld; j++) {
    opp_coord[3][j] = oldopp_coord[ochild][j];
  }
      }
    }
  }
#endif

  if (fill_flag & FILL_BOUND)
  {
    for (int i = 0; i < 3; i++)
    {
      elinfo->bound[i] = elinfo_old->bound[cv[i]];
    }
    elinfo->bound[3] = GET_BOUND(elinfo_old->boundary[N_FACES+0]);

    elinfo->boundary[0] = nil;
    elinfo->boundary[1] = elinfo_old->boundary[cv[1]];
    elinfo->boundary[2] = elinfo_old->boundary[cv[2]];
    elinfo->boundary[3] = elinfo_old->boundary[ochild];

    ce = child_edge[el_type][ichild];
    for (iedge=0; iedge<4; iedge++) {
      elinfo->boundary[N_FACES+iedge] = 
                   elinfo_old->boundary[N_FACES+ce[iedge]];
    }
    for (iedge=4; iedge<6; iedge++) {
      int i = 5 - cv[iedge-3];                /* old vertex opposite new edge */
      elinfo->boundary[N_FACES+iedge] = elinfo_old->boundary[i];
    }
  }


  if (elinfo->fill_flag & FILL_ORIENTATION) {
    elinfo->orientation =
      elinfo_old->orientation * child_orientation[el_type][ichild];
  }
#endif
}
#endif

template < int dim, int dimworld >
inline void AlbertaGrid < dim, dimworld >::setNewCoords
(const FieldVector<albertCtype, dimworld> & trans, const albertCtype scalar)
{
    static FieldVector<albertCtype, dimworld> trans_(0.0);
  static albertCtype scalar_ (1.0);

  for(int i=0; i<macroVertices_.size(); i++)
    macroVertices_[i] = 0;

  for(ALBERTA MACRO_EL * mel = mesh_->first_macro_el; mel; mel=mel->next)
  {
    for(int i=0; i<dim+1; i++)
    {
      int dof = mel->el->dof[i][0];
      // visit each coord only once 
      if( macroVertices_[dof] != 1)
      {
        macroVertices_[dof] = 1;
        for(int j=0; j<dimworld; j++)
        {
          mel->coord[i][j] -= trans_[j];
          mel->coord[i][j] /= scalar_;

          mel->coord[i][j] *= scalar;
          mel->coord[i][j] += trans[j];
        }
      }
    }
  }

  for (int i=0; i<dimworld; i++)
    trans_[i] = trans[i];

  scalar_ = scalar;
}

} // namespace Dune
