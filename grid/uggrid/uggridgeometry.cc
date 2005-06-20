//****************************************************************
//
// --UGGridGeometry
//
//****************************************************************

#include <algorithm>

template <int mydim, int coorddim>
struct UGGridGeometryPositionAccess
{};

template <>
struct UGGridGeometryPositionAccess<0,3>
{
    static inline
    void get(TargetType<3,3>::T* target,
             int i,
             FieldVector<double, 3>& coord) {

#ifdef _3
        coord[0] = target->myvertex->iv.x[0];
        coord[1] = target->myvertex->iv.x[1];
        coord[2] = target->myvertex->iv.x[2];
#endif
    }

};


template <>
struct UGGridGeometryPositionAccess<3,3>
{
    static inline
    void get(TargetType<0,3>::T* target,
             int i,
             FieldVector<double, 3>& coord) {
#ifdef _3
        if (UG_NS<3>::Tag(target) == UG3d::HEXAHEDRON) {
            // Dune numbers the vertices of a hexahedron differently than UG.
            // The following two lines do the transformation
            const int renumbering[8] = {0, 1, 3, 2, 4, 5, 7, 6};
            i = renumbering[i];
        }
        
        UG3d::VERTEX* vertex = UG_NS<3>::Corner(target,i)->myvertex;
        
        for (int j=0; j<3; j++)
            coord[j] = vertex->iv.x[j];
#endif
    }

};

template <>
struct UGGridGeometryPositionAccess<0,2>
{
    static inline
    void get(TargetType<2,2>::T* target,
             int i,
             FieldVector<double, 2>& coord) {
#ifdef _2
        coord[0] = target->myvertex->iv.x[0];
        coord[1] = target->myvertex->iv.x[1];
#endif
    }

};

template <>
struct UGGridGeometryPositionAccess<2,2>
{
    static inline
    void get(TargetType<0,2>::T* target,
             int i,
             FieldVector<double, 2>& coord) {
#ifdef _2
    UG2d::VERTEX* vertex = UG_NS<2>::Corner(target,i)->myvertex;
    
    for (int j=0; j<2; j++)
        coord[j] = vertex->iv.x[j];
#endif
    }

};


template< int mydim, int coorddim, class GridImp>
inline GeometryType UGGridGeometry<mydim,coorddim,GridImp>::type() const
{

  switch (mydim)
  {
  case 0: return vertex;
  case 1: return line;
  case 2: 
#ifdef _2
      switch (UG_NS<coorddim>::Tag(target_)) {
      case UG2d::TRIANGLE:
          return triangle;
      case UG2d::QUADRILATERAL:
          return quadrilateral;
      default:
          DUNE_THROW(GridError, "UGGridGeometry::type():  ERROR:  Unknown type " 
                     << UG_NS<coorddim>::Tag(target_) << " found!");
      }
#endif
       
  case 3: 
      switch (UG_NS<coorddim>::Tag(target_)) {
#ifdef _3
      case UG3d::TETRAHEDRON:
          return tetrahedron;
      case UG3d::PYRAMID:
          return pyramid;
      case UG3d::PRISM:
          return prism;
      case UG3d::HEXAHEDRON:
          return hexahedron;
      default:
          DUNE_THROW(GridError, "UGGridGeometry::type():  ERROR:  Unknown type " 
                     << UG_NS<coorddim>::Tag(target_) << " found!");
#endif
      }
  }

  // Just to calm the compiler
  return tetrahedron;
}

template< int mydim, int coorddim, class GridImp>
inline int UGGridGeometry<mydim,coorddim,GridImp>::corners() const
{
    return UG_NS<coorddim>::Corners_Of_Elem(target_);
}


///////////////////////////////////////////////////////////////////////

template<int mydim, int coorddim, class GridImp>
inline const FieldVector<typename GridImp::ctype, coorddim>& UGGridGeometry<mydim,coorddim,GridImp>:: 
operator [](int i) const
{
    UGGridGeometryPositionAccess<mydim,coorddim>::get(target_, i, coord_[i]);
    return coord_[i];
}

template<int mydim, int coorddim, class GridImp>
inline bool UGGridGeometry<mydim,coorddim,GridImp>:: 
checkInside(const FieldVector<UGCtype, coorddim> &loc) const
{
    switch (mydim) {

    case 0: // vertex
        return false;
    case 1: // line
        return 0 <= loc[0] && loc[0] <= 1;
    case 2: 
#ifdef _2
        switch (UG_NS<coorddim>::Tag(target_)) {
        case UG2d::TRIANGLE:
            return 0 <= loc[0] && 0 <= loc[1] && (loc[0]+loc[1])<=1;
        case UG2d::QUADRILATERAL:
            return 0 <= loc[0] && loc[0] <= 1
                && 0 <= loc[1] && loc[1] <= 1;
        default:
            DUNE_THROW(GridError, "UGGridGeometry::checkInside():  ERROR:  Unknown type " 
                       << UG_NS<coorddim>::Tag(target_) << " found!");
        }
#endif
        
    case 3: 
        switch (UG_NS<coorddim>::Tag(target_)) {
#ifdef _3
        case UG3d::TETRAHEDRON:
            return 0 <= loc[0] && 0 <= loc[1] && 0 <= loc[2]
                && (loc[0]+loc[1]+loc[2]) <= 1;
        case UG3d::PYRAMID:
            return 0 <= loc[0] && 0 <= loc[1] && 0 <= loc[2]
                && (loc[0]+loc[2]) <= 1
                && (loc[1]+loc[2]) <= 1;
        case UG3d::PRISM:
            return 0 <= loc[0] && 0 <= loc[1] 
                && (loc[0]+loc[1])<=1
                && 0 <= loc[2] && loc[2] <= 1;
        case UG3d::HEXAHEDRON:
            return 0 <= loc[0] && loc[0] <= 1
                && 0 <= loc[1] && loc[1] <= 1
                && 0 <= loc[2] && loc[2] <= 1;
        default:
            DUNE_THROW(GridError, "UGGridGeometry::checkInside():  ERROR:  Unknown type " 
                       << UG_NS<coorddim>::Tag(target_) << " found!");
#endif
        }
    }
    
}

template< int mydim, int coorddim, class GridImp>
inline FieldVector<typename GridImp::ctype, coorddim> UGGridGeometry<mydim,coorddim,GridImp>:: 
global(const FieldVector<UGCtype, mydim>& local) const
{
    FieldVector<UGCtype, coorddim> globalCoord;

    // coorddim*coorddim is an upper bound for the number of vertices
    UGCtype* cornerCoords[coorddim*coorddim];
    UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);

    // Actually do the computation
    UG_NS<coorddim>::Local_To_Global(corners(), cornerCoords, local, globalCoord);

    return globalCoord; 
}

// Specialization for dim==1, coorddim==2.  This is necessary
// because we specialized the whole class
template <class GridImp>
inline FieldVector<typename GridImp::ctype, 2> UGGridGeometry<1,2,GridImp>:: 
global(const FieldVector<typename GridImp::ctype, 1>& local) const
{
    FieldVector<UGCtype, 2> globalCoord;

    // I want operator +...  (sigh)
    globalCoord[0] = local[0]*coord_[1][0] + (1-local[0])*coord_[0][0];
    globalCoord[1] = local[0]*coord_[1][1] + (1-local[0])*coord_[0][1];

    return globalCoord; 
}

// Specialization for dim==2, coorddim==3.  This is necessary
// because we specialized the whole class
template <class GridImp>
inline FieldVector<typename GridImp::ctype, 3> UGGridGeometry<2,3,GridImp>:: 
global(const FieldVector<typename GridImp::ctype, 2>& local) const
{

    FieldVector<UGCtype, 3> result;

    if (elementType_ == triangle) {

        for (int i=0; i<3; i++)
            result[i] = (1.0-local[0]-local[1])*coord_[0][i]  
                + local[0]*coord_[1][i] 
                + local[1]*coord_[2][i];

    } else {

        // quadrilateral

        for (int i=0; i<3; i++)
            result[i] = (1.0-local[0])*(1.0-local[1])*coord_[0][i]
                + local[0]*(1.0-local[1])*coord_[1][i]                    
                + local[0]*local[1]*coord_[2][i]
                + (1.0-local[0])*local[1]*coord_[3][i];

    }

    return result;

}


// Maps a global coordinate within the element to a 
// local coordinate in its reference element
template< int mydim, int coorddim, class GridImp>
inline FieldVector<typename GridImp::ctype, mydim> UGGridGeometry<mydim,coorddim, GridImp>::
local (const FieldVector<typename GridImp::ctype, coorddim>& global) const
{
    FieldVector<UGCtype, mydim> result;
    UGCtype localCoords[mydim];

    // Copy input ADT into C-style array
    UGCtype global_c[coorddim];
    for (int i=0; i<coorddim; i++)
        global_c[i] = global[i];

    // coorddim*coorddim is an upper bound for the number of vertices
    UGCtype* cornerCoords[coorddim*coorddim];
    UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);

    // Actually do the computation
    /** \todo Why is this const_cast necessary? */
#ifdef _2
    UG2d::UG_GlobalToLocal(corners(), const_cast<const double**>(cornerCoords), global_c, localCoords);
#else
    UG3d::UG_GlobalToLocal(corners(), const_cast<const double**>(cornerCoords), global_c, localCoords);
#endif

    // Copy result into array
    for (int i=0; i<mydim; i++)
        result[i] = localCoords[i];

    return result;
}

  
template< int mydim, int coorddim, class GridImp>
inline typename GridImp::ctype UGGridGeometry<mydim,coorddim,GridImp>:: 
integrationElement (const FieldVector<typename GridImp::ctype, mydim>& local) const
{
    return std::abs(1/jacobianInverse(local).determinant());
}

template <class GridImp>
inline typename GridImp::ctype UGGridGeometry<1,2,GridImp>::
integrationElement (const FieldVector<typename GridImp::ctype, 1>& local) const
{
    FieldVector<UGCtype, 2> diff = coord_[0];
    diff -= coord_[1];
    return diff.two_norm();
}

template <class GridImp>
inline typename GridImp::ctype UGGridGeometry<2,3,GridImp>::
integrationElement (const FieldVector<typename GridImp::ctype, 2>& local) const
{
    FieldVector<UGCtype, 3> normal;
    FieldVector<UGCtype, 3> ba = coord_[1] - coord_[0];
    FieldVector<UGCtype, 3> ca = coord_[2] - coord_[0];

#define V3_VECTOR_PRODUCT(A,B,C) {(C)[0] = (A)[1]*(B)[2] - (A)[2]*(B)[1];\
                                     (C)[1] = (A)[2]*(B)[0] - (A)[0]*(B)[2];\
                                     (C)[2] = (A)[0]*(B)[1] - (A)[1]*(B)[0];} 

    V3_VECTOR_PRODUCT(ba, ca, normal);
#undef V3_VECTOR_PRODUCT

    return normal.two_norm();
}

template< int mydim, int coorddim, class GridImp>
inline const FieldMatrix<typename GridImp::ctype, mydim,mydim>& UGGridGeometry<mydim,coorddim, GridImp>:: 
jacobianInverse (const FieldVector<typename GridImp::ctype, mydim>& local) const
{
    // coorddim*coorddim is an upper bound for the number of vertices
    UGCtype* cornerCoords[coorddim*coorddim];
    UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);

    // compute the transformation onto the reference element (or vice versa?)
    UG_NS<coorddim>::Transformation(corners(), cornerCoords, local, jac_inverse_); 
    return jac_inverse_;
}
