//****************************************************************
//
// --UGGridElement 
// --Element 
//
//****************************************************************

// singleton holding reference elements
template<int dim> 
struct UGGridReferenceElement
{
  enum { dimension = dim };
  UGGridElement<dim,dim> refelem;

  UGGridReferenceElement () : refelem (true) {};
};

// initialize static variable with bool constructor 
// (which makes reference element)
// this sucks but for gcc we do a lot 
static UGGridReferenceElement<3> reftetrahedron;
static UGGridReferenceElement<3> refpyramid;
static UGGridReferenceElement<3> refprism;
static UGGridReferenceElement<3> refpinchedhexa;
static UGGridReferenceElement<3> refhexahedron;

static UGGridReferenceElement<2> reftriangle;
static UGGridReferenceElement<2> refquadrangle;

static UGGridReferenceElement<1> refline;


//****************************************************************
//
//  specialization of mapVertices 
//
//****************************************************************
//
// default, do nothing 
template< int dim, int dimworld>  
inline int UGGridElement<dim,dimworld>::mapVertices (int i) const  
{ 
  return i; 
}

template< int dim, int dimworld>
inline UGGridElement<dim,dimworld>:: 
UGGridElement(bool makeRefElement)
{
#if 0
  // make empty element 
  initGeom();

  // make coords for reference elements, spezial for different dim
  if(makeRefElement)
    makeRefElemCoords();
#endif
}



template< int dim, int dimworld>
inline ElementType UGGridElement<dim,dimworld>::type() 
{
    std::cerr << "UGGridElement::type() Not yet implemented\n";
  switch (dim)
  {
    case 1: return line;
    case 2: return triangle;
    case 3: return tetrahedron;

    default: return unknown;
  }
}

template< int dim, int dimworld>
inline int UGGridElement<dim,dimworld>::corners() 
{

    //#define CW_READ_STATIC(p,s,t)                   ReadCW(p,s ## CE)
    //#define TAG(p) CW_READ_STATIC(p,UG3d::TAG_,GENERAL_)
#define TAG(p) ReadCW(p, UG3d::TAG_CE)
#define CORNERS_OF_ELEM(p)(UG3d::element_descriptors[TAG(p)]->corners_of_elem)
    return CORNERS_OF_ELEM(target_);
#undef CORNERS_OF_ELEM
#undef TAG
    //#undef CW_READ_STATIC
}


///////////////////////////////////////////////////////////////////////
template< int dim, int dimworld>
inline Vec<dimworld,UGCtype>& UGGridElement<dim,dimworld>:: 
operator [](int i)
{
    switch (dim) {

    case 0: {
        UG3d::NODE* node_target = (UG3d::NODE*) target_;
        //printf("in operator[]: target %d\n", node_target);
        UG3d::VERTEX* vertex = node_target->myvertex;


#define OBJT(p) ReadCW(p, UG3d::OBJ_CE)
        bool vType = OBJT(vertex)== UG3d::IVOBJ;
#undef OBJT
        coord_(0,i) = vertex->iv.x[0];
        coord_(1,i) = vertex->iv.x[1];
        coord_(2,i) = vertex->iv.x[2];
        break;
    }
        
    }
    return coord_(i);
}


/** \todo It should be able to write this more concisely
    using partial spezialization.
*/
template<>
inline UGGridElement<3,3>& UGGridElement<3,3>:: 
refelem()
{
    /** \todo Speed this up! */
    switch (type()) {
    case tetrahedron:
        return reftetrahedron.refelem;
    default:
        std::cerr << "Unknown element type in refelem()\n";
    }

  return reftetrahedron.refelem;
}

template<>
inline UGGridElement<2,2>& UGGridElement<2,3>:: 
refelem()
{
    if (type() == triangle)
        return reftriangle.refelem;
    else
        return refquadrangle.refelem;
}

template<>
inline UGGridElement<1,1>& UGGridElement<1,3>:: 
refelem()
{
  return refline.refelem;
}

template<>
inline UGGridElement<2,2>& UGGridElement<2,2>:: 
refelem()
{
    if (type() == triangle)
        return reftriangle.refelem;
    else
        return refquadrangle.refelem;
}

template<>
inline UGGridElement<1,1>& UGGridElement<1,2>:: 
refelem()
{
  return refline.refelem;
}


#if 0
template< int dim, int dimworld>
inline Vec<dimworld,albertCtype> AlbertGridElement<dim,dimworld>:: 
global(const Vec<dim>& local)
{
  // checked, works 
  
  // we calculate interal in barycentric coordinates  
  // fake the third local coordinate via localFake
  albertCtype localFake=1.0;
  albertCtype c;

  c = local.read(0);
  localFake -= c;
 
  // the initialize 
  // note that we have to swap the j and i 
  // in coord(j,i) means coord_(i)(j)  
  for(int j=0; j<dimworld; j++)
    globalCoord_(j) = c * coord_(j,0);
      
  // for all local coords 
  for (int i = 1; i < dim; i++)
  { 
    c = local.read(i);
    localFake -= c;
    for(int j=0; j<dimworld; j++)
      globalCoord_(j) += c * coord_(j,i);
  } 
  
  // for the last barycentric coord 
  for(int j=0; j<dimworld; j++)
    globalCoord_(j) += localFake * coord_(j,dim);

  std::cout << "*********************************\n";
  globalCoord_.print(std::cout , 2); std::cout << "\n";
  std::cout << "*********************************\n";
  
  return globalCoord_; 
}

template< int dim, int dimworld>
inline Vec<dim>& AlbertGridElement<dim,dimworld>:: 
local(const Vec<dimworld>& global)
{
  ALBERT REAL lambda[dim+1];
  
  tmpVec_ = localBary(global);
  
  // Umrechnen von baryzentrischen localen Koordinaten nach 
  // localen Koordinaten, 
  for(int i=0; i<dim; i++)
    localCoord_(i) = tmpVec_.read(i);
  
  return localCoord_; 
}

template <int dim, int dimworld> 
inline Vec<dim+1> AlbertGridElement<dim,dimworld>:: 
localBary(const Vec<dimworld>& global)
{
  std::cout << "localBary for dim != dimworld not implemented yet!";
  Vec<dim+1> tmp (0.0);  
  return tmp;
}

template <>
inline Vec<3> AlbertGridElement<2,2>:: 
localBary(const Vec<2>& global)
{
  enum { dim = 2};
  enum { dimworld = 2};
  
  ALBERT REAL edge[dim][dimworld], x[dimworld];
  ALBERT REAL x0, det, det0, det1, lmin;
  int j, k;
  Vec<dim+1,albertCtype> lambda;
  ALBERT REAL *v = NULL;

  /*
   * we got to solve the problem : 
   */
  /*
   * ( q1x q2x ) (lambda1) = (qx) 
   */
  /*
   * ( q1y q2y ) (lambda2) = (qy) 
   */
  /*
   * with qi=pi-p3, q=global-p3 
   */

  v = static_cast<ALBERT REAL *> (elInfo_->coord[0]);
  for (int j = 0; j < dimworld; j++)
  {
    x0 = elInfo_->coord[dim][j];
    x[j] = global.read(j) - x0;
    for (int i = 0; i < dim; i++)
      edge[i][j] = elInfo_->coord[i][j] - x0;
  }

  det = edge[0][0] * edge[1][1] - edge[0][1] * edge[1][0];

  det0 = x[0] * edge[1][1] - x[1] * edge[1][0];
  det1 = edge[0][0] * x[1] - edge[0][1] * x[0];

  if(ABS(det) < 1.E-20)
  {
    printf("det = %e; abort\n", det);
    abort();
  }

  // lambda is initialized here 
  lambda(0) = det0 / det;
  lambda(1) = det1 / det;
  lambda(2) = 1.0 - lambda(0) - lambda(1);

  return lambda;
}

template <>
inline Vec<4> AlbertGridElement<3,3>:: 
localBary(const Vec<3>& global)
{
  enum { dim = 3};
  enum { dimworld = 3};
  
  ALBERT REAL edge[dim][dimworld], x[dimworld];
  ALBERT REAL x0, det, det0, det1, det2, lmin;
  Vec<dim+1,albertCtype> lambda;
  int j, k;

  //! we got to solve the problem : 
  //! ( q1x q2x q3x) (lambda1) (qx) 
  //! ( q1y q2y q3y) (lambda2) = (qy) 
  //! ( q1z q2z q3z) (lambda3) (qz) 
  //! with qi=pi-p3, q=xy-p3 

  for (int j = 0; j < dimworld; j++)
  {
    x0 = elInfo_->coord[dim][j];
    x[j] = global.read(j) - x0;
    for (int i = 0; i < dim; i++)
      edge[i][j] = elInfo_->coord[i][j] - x0;
  }

  det = edge[0][0] * edge[1][1] * edge[2][2]
    + edge[0][1] * edge[1][2] * edge[2][0]
    + edge[0][2] * edge[1][0] * edge[2][1]
    - edge[0][2] * edge[1][1] * edge[2][0]
    - edge[0][0] * edge[1][2] * edge[2][1]
    - edge[0][1] * edge[1][0] * edge[2][2];
  det0 = x[0] * edge[1][1] * edge[2][2]
    + x[1] * edge[1][2] * edge[2][0]
    + x[2] * edge[1][0] * edge[2][1]
    - x[2] * edge[1][1] * edge[2][0]
    - x[0] * edge[1][2] * edge[2][1] - x[1] * edge[1][0] * edge[2][2];
  det1 = edge[0][0] * x[1] * edge[2][2]
    + edge[0][1] * x[2] * edge[2][0]
    + edge[0][2] * x[0] * edge[2][1]
    - edge[0][2] * x[1] * edge[2][0]
    - edge[0][0] * x[2] * edge[2][1] - edge[0][1] * x[0] * edge[2][2];
  det2 = edge[0][0] * edge[1][1] * x[2]
    + edge[0][1] * edge[1][2] * x[0]
    + edge[0][2] * edge[1][0] * x[1]
    - edge[0][2] * edge[1][1] * x[0]
    - edge[0][0] * edge[1][2] * x[1] - edge[0][1] * edge[1][0] * x[2];
  if(ABS(det) < 1.E-20)
  {
    printf("det = %e; abort\n", det);
    abort();
    return (-2);
  }

  // lambda is initialized here
  lambda(0) = det0 / det;
  lambda(1) = det1 / det;
  lambda(2) = det2 / det;
  lambda(3) = 1.0 - lambda(0) - lambda(1) - lambda(2);

  return lambda;
}

// default implementation calls ALBERT routine 
template< int dim, int dimworld>
inline albertCtype AlbertGridElement<dim,dimworld>::elVolume () const 
{
  return ALBERT el_volume(elInfo_);
}

// volume of one Element, here triangle  
template <> 
inline albertCtype AlbertGridElement<2,2>::elVolume () const 
{
  enum { dim = 2 };
  enum { dimworld = 2 };
  const albertCtype volFac = 0.5;
  REAL        e1[dimworld], e2[dimworld], det;
  const REAL  *v0;

  v0 = elInfo_->coord[0];
  for (int i = 0; i < DIM_OF_WORLD; i++)
  {
    e1[i] = elInfo_->coord[1][i] - v0[i];
    e2[i] = elInfo_->coord[2][i] - v0[i];
  }

  det = e1[0]*e2[1] - e1[1]*e2[0];
  det = ABS(det);

  return volFac*det;
}

// volume of one Element, here therahedron  
template <> 
inline albertCtype AlbertGridElement<3,3>::elVolume () const 
{
  enum { dim = 3 };
  enum { dimworld = 3 };
  const albertCtype volFac = 1.0/6.0;

  REAL        e1[dimworld], e2[dimworld], e3[dimworld], det;
  const REAL  *v0;
  
  v0 = elInfo_->coord[0];
  for (int i = 0; i < dimworld; i++)
  {     
    e1[i] = elInfo_->coord[1][i] - v0[i];
    e2[i] = elInfo_->coord[2][i] - v0[i];
    e3[i] = elInfo_->coord[3][i] - v0[i];
  }

  det =   e1[0] * (e2[1]*e3[2] - e2[2]*e3[1])
        - e1[1] * (e2[0]*e3[2] - e2[2]*e3[0])
        + e1[2] * (e2[0]*e3[1] - e2[1]*e3[0]);
  det = ABS(det);

  return volFac*det;
}
  
template< int dim, int dimworld>
inline albertCtype AlbertGridElement<dim,dimworld>:: 
integration_element (const Vec<dim,albertCtype>& local)
{
  // if inverse was built, volume was calced already 
  if(builtinverse_)
    return volume_; 

  volume_ = elVolume();
  return volume_;
}

template <>
inline Mat<1,1>& AlbertGridElement<1,2>:: 
Jacobian_inverse (const Vec<1,albertCtype>& local)
{
  std::cout << "Jaconbian_inverse for dim=1,dimworld=2 not implemented yet! \n";
  return Jinv_;
}

template< int dim, int dimworld>
inline Mat<dim,dim>& AlbertGridElement<dim,dimworld>:: 
Jacobian_inverse (const Vec<dim,albertCtype>& local)
{
  if(builtinverse_)
    return Jinv_;

  // builds the jacobian inverse and calculates the volume 
  builtJacobianInverse(local);
  return Jinv_;
}

// calc volume of face of tetrahedron
template <>
inline void AlbertGridElement<2,3>:: 
builtJacobianInverse(const Vec<2,albertCtype>& local)
{
  //std::cout << "To be implemented! \n";
  //abort();
  enum { dim = 2 };
  enum { dimworld = 3 };
  
  // is faster than the lower method 
  volume_ = 0.1; 
  builtinverse_ = true;
}

template< int dim, int dimworld>
inline void AlbertGridElement<dim,dimworld>:: 
builtJacobianInverse(const Vec<dim,albertCtype>& local)
{
  //std::cout << dim << " " << dimworld<< " Dim|Dimworld \n";
  enum { div = (dim < 3) ? 1 : 2 };
  // volFactor should be 0.5 for dim==2 and (1.0/6.0) for dim==3
  const albertCtype volFactor = static_cast<albertCtype> (0.5/div);
  
  ALBERT REAL lambda[dim+1][dimworld];

  // is faster than the lower method 
  volume_ = volFactor * ALBERT el_grd_lambda(elInfo_,lambda);
  for(int i=0; i<dim; i++)
  {
    for(int j=0; j<dimworld; j++)
      Jinv_(i,j) = lambda[i][j];
  }
  builtinverse_ = true;
}
#endif
#if 0
inline void AlbertGridElement<2,2>:: 
builtJacobianInverse(const Vec<2,albertCtype>& local)
{
  // volFactor should be 1/6, see ALBERT Doc
  const albertCtype volFactor = 0.5; 
  enum { dimworld = 2 };
  enum { dim = 2 };
  
  REAL        e1[dimworld], e2[dimworld], det, adet;
  const REAL  *v0;
  REAL        a11, a12, a21, a22;
  
  v0 = elInfo_->coord[0];
  for (int i = 0; i < dimworld; i++) 
  {
    e1[i] = elInfo_->coord[1][i] - v0[i];
    e2[i] = elInfo_->coord[2][i] - v0[i];
  }

  det = e1[0] * e2[1] - e1[1] * e2[0];
  adet = ABS(det);
  if (adet < 1.0E-25)
  {
    std::cout << "abs(det) = " << adet << "\n";
    Jinv_ = 0.0;
  }
  else
  {
    det = 1.0 / det;
    a11 =  e2[1] * det;       /* (a_ij) = A^{-T} */
    a21 = -e2[0] * det;
    a12 = -e1[1] * det;
    a22 =  e1[0] * det;

    Jinv_(1,0) = a11;
    Jinv_(1,1) = a21;
    Jinv_(2,0) = a12;
    Jinv_(2,1) = a22;
    Jinv_(0,0) = - Jinv_(1,0) - Jinv_(2,0);
    Jinv_(0,1) = - Jinv_(1,1) - Jinv_(2,1);
  }

  volume_ = volFactor * adet;
  builtinverse_ = true;
  return;
}
   
inline void AlbertGridElement<3,3>:: 
builtJacobianInverse(const Vec<3,albertCtype>& local)
{
  // volFactor should be 1/6, see ALBERT Doc
  static const albertCtype volFactor = 1.0/6.0; 
  
  enum { dimworld = 3 };
  enum { dim = 3 };
  
  REAL        e1[dimworld], e2[dimworld], e3[dimworld];
  const REAL  *v0;
  REAL        det, adet;
  REAL        a11, a12, a13, a21, a22, a23, a31, a32, a33;

  v0 = elInfo_->coord[0];
  for (int i = 0; i < dimworld; i++) 
  {
    e1[i] = elInfo_->coord[1][i] - v0[i];
    e2[i] = elInfo_->coord[2][i] - v0[i];
    e3[i] = elInfo_->coord[3][i] - v0[i];
  }

  det =   e1[0] * (e2[1]*e3[2] - e2[2]*e3[1])
        - e1[1] * (e2[0]*e3[2] - e2[2]*e3[0])
        + e1[2] * (e2[0]*e3[1] - e2[1]*e3[0]);
  
  adet = ABS(det);
  if (adet < 1.0E-25)
  {
    std::cout << "abs(det) = " << adet << "\n";
    Jinv_ = 0.0;
  }
  else
  {
    det = 1.0 / det;
    a11 = (e2[1]*e3[2] - e2[2]*e3[1]) * det;    /* (a_ij) = A^{-T} */
    a12 = (e2[2]*e3[0] - e2[0]*e3[2]) * det;
    a13 = (e2[0]*e3[1] - e2[1]*e3[0]) * det;
    a21 = (e1[2]*e3[1] - e1[1]*e3[2]) * det;
    a22 = (e1[0]*e3[2] - e1[2]*e3[0]) * det;
    a23 = (e1[1]*e3[0] - e1[0]*e3[1]) * det;
    a31 = (e1[1]*e2[2] - e1[2]*e2[1]) * det;
    a32 = (e1[2]*e2[0] - e1[0]*e2[2]) * det;
    a33 = (e1[0]*e2[1] - e1[1]*e2[0]) * det;

    Jinv_(1,0) = a11;
    Jinv_(1,1) = a12;
    Jinv_(1,2) = a13;
    Jinv_(2,0) = a21;
    Jinv_(2,1) = a22;
    Jinv_(2,2) = a23;
    Jinv_(3,0) = a31;
    Jinv_(3,1) = a32;
    Jinv_(3,2) = a33;
    Jinv_(0,0) = -Jinv_(1,0) -Jinv_(2,0) -Jinv_(3,0);
    Jinv_(0,1) = -Jinv_(1,1) -Jinv_(2,1) -Jinv_(3,1);
    Jinv_(0,2) = -Jinv_(1,2) -Jinv_(2,2) -Jinv_(3,2);
  }

  volume_ = volFactor * adet;
  builtinverse_ = true;
  return;
}
#endif

#if 0
template <>
inline void AlbertGridElement<1,2>:: 
builtJacobianInverse(const Vec<1,albertCtype>& local)
{
  // volume is length of edge 
  Vec<2,albertCtype> vec = coord_(0) - coord_(1);
  volume_ = vec.norm2(); 

  builtinverse_ = true;
}

template<int dim, int dimworld>
inline bool AlbertGridElement <dim ,dimworld >::
checkInside(const Vec<dimworld> &global)
{
  Vec<dim+1> localCoords = localBary(global);

  // return true if point is inside element
  bool ret=true;
  
  // if one of the barycentric coordinates is negativ 
  // then the point must be outside of the element
  for(int i=0; i<dim+1; i++)
   if(localCoords(i) < 0.0) ret = false; 
  
  return ret;
}


#endif
