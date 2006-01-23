//****************************************************************
//
// --UGGridGeometry
//
//****************************************************************

#include <algorithm>


///////////////////////////////////////////////////////////////////////
//
// General implementation of UGGridGeometry mydim-> coorddim
//
///////////////////////////////////////////////////////////////////////


template< int mydim, int coorddim, class GridImp>
inline NewGeometryType UGGridGeometry<mydim,coorddim,GridImp>::type() const
{
  switch (mydim)
  {
  case 0: return NewGeometryType(NewGeometryType::cube,0); 
  case 1: return NewGeometryType(NewGeometryType::cube,1);
  case 2: 

      switch (UG_NS<coorddim>::Tag(target_)) {
      case UG2d::TRIANGLE:
          return NewGeometryType(NewGeometryType::simplex,2);
      case UG2d::QUADRILATERAL:
          return NewGeometryType(NewGeometryType::cube,2);
      default:
          DUNE_THROW(GridError, "UGGridGeometry::type():  ERROR:  Unknown type " 
                     << UG_NS<coorddim>::Tag(target_) << " found!");
      }
       
  case 3: 
      switch (UG_NS<coorddim>::Tag(target_)) {

      case UG3d::TETRAHEDRON:
          return NewGeometryType(NewGeometryType::simplex,3);
      case UG3d::PYRAMID:
          return NewGeometryType(NewGeometryType::pyramid,3);
      case UG3d::PRISM:
          return NewGeometryType(NewGeometryType::prism,3);
      case UG3d::HEXAHEDRON:
          return NewGeometryType(NewGeometryType::cube,3);
      default:
          DUNE_THROW(GridError, "UGGridGeometry::type():  ERROR:  Unknown type " 
                     << UG_NS<coorddim>::Tag(target_) << " found!");

      }
  }

}

template< int mydim, int coorddim, class GridImp>
inline int UGGridGeometry<mydim,coorddim,GridImp>::corners() const
{
    return UG_NS<coorddim>::Corners_Of_Elem(target_);
}


template<int mydim, int coorddim, class GridImp>
inline const FieldVector<typename GridImp::ctype, coorddim>& UGGridGeometry<mydim,coorddim,GridImp>:: 
operator [](int i) const
{
    // This geometry is a vertex
    if (mydim==0) {
        assert(i==0);
        // The cast onto typename UGTypes<coorddim>::Node*
        // is only correct if this geometry represents a vertex.  But this is so since
        // we are within an if (mydim==0) clause.
        for (int i=0; i<coorddim; i++)
            coord_[0][i] = ((typename UGTypes<coorddim>::Node*)target_)->myvertex->iv.x[i];

        return coord_[0];
    }

    // ////////////////////////////////
    //  This geometry is an element
    // ////////////////////////////////
    assert(mydim==coorddim);

    // Renumber the vertices of Dune numbering to UG numbering
    if (mydim==3 && type().isHexahedron()) {
        // Dune numbers the vertices of a hexahedron differently than UG.
        // The following two lines do the transformation
        const int renumbering[8] = {0, 1, 3, 2, 4, 5, 7, 6};
        i = renumbering[i];
    }

    if (mydim==2 && type().isQuadrilateral()) {
        // Dune numbers the vertices of a quadrilateral differently than UG.
        // The following two lines do the transformation
        const int renumbering[4] = {0, 1, 3, 2};
        i = renumbering[i];
    }

    if (mode_==element_mode) {
        typename UGTypes<coorddim>::Node* corner = UG_NS<coorddim>::Corner(((typename UGTypes<coorddim>::Element*)target_),i);
        for (int j=0; j<coorddim; j++)
            coord_[i][j] = corner->myvertex->iv.x[j];
    }

  return coord_[i];
}

template< int mydim, int coorddim, class GridImp>
inline FieldVector<typename GridImp::ctype, coorddim> UGGridGeometry<mydim,coorddim,GridImp>:: 
global(const FieldVector<UGCtype, mydim>& local) const
{
  FieldVector<UGCtype, coorddim> globalCoord;

  if (mode_==element_mode)
	{
	  // coorddim*coorddim is an upper bound for the number of vertices
	  UGCtype* cornerCoords[coorddim*coorddim];
	  UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);
	  
	  // Actually do the computation
	  UG_NS<coorddim>::Local_To_Global(corners(), cornerCoords, local, globalCoord);
	}
  else
	{
	  UG_NS<coorddim>::Local_To_Global(corners(), cornerpointers_, local, globalCoord);	  
	}

  return globalCoord; 
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
  
  if (mode_==element_mode)
	{
	  // coorddim*coorddim is an upper bound for the number of vertices
	  UGCtype* cornerCoords[coorddim*coorddim];
	  UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);

	  // Actually do the computation
	  /** \todo Why is this const_cast necessary? */
	  UG_NS<coorddim>::GlobalToLocal(corners(), const_cast<const double**>(cornerCoords), global_c, localCoords);
	}
  else
	{
	  // Actually do the computation
	  /** \todo Why is this const_cast necessary? */
	  UG_NS<coorddim>::GlobalToLocal(corners(), const_cast<const double**>(cornerpointers_), global_c, localCoords);
	}

  // Copy result into array
  for (int i=0; i<mydim; i++)
	result[i] = localCoords[i];

  return result;
}

template<int mydim, int coorddim, class GridImp>
inline bool UGGridGeometry<mydim,coorddim,GridImp>:: 
checkInside(const FieldVector<UGCtype, mydim> &loc) const
{
    switch (mydim) {

    case 0: // vertex
        return false;
    case 1: // line
        return 0 <= loc[0] && loc[0] <= 1;
    case 2: 

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
        
    case 3: 
        switch (UG_NS<coorddim>::Tag(target_)) {

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

        }
    }
    
}

  
template< int mydim, int coorddim, class GridImp>
inline typename GridImp::ctype UGGridGeometry<mydim,coorddim,GridImp>:: 
integrationElement (const FieldVector<typename GridImp::ctype, mydim>& local) const
{
    return std::abs(1/jacobianInverseTransposed(local).determinant());
}


template< int mydim, int coorddim, class GridImp>
inline const FieldMatrix<typename GridImp::ctype, mydim,mydim>& UGGridGeometry<mydim,coorddim, GridImp>:: 
jacobianInverseTransposed (const FieldVector<typename GridImp::ctype, mydim>& local) const
{
  if (mode_==element_mode)
	{
	  // coorddim*coorddim is an upper bound for the number of vertices
	  // compile array of pointers to corner coordinates
	  UGCtype* cornerCoords[coorddim*coorddim];
	  UG_NS<coorddim>::Corner_Coordinates(target_, cornerCoords);
	  
	  // compute the transformation onto the reference element (or vice versa?)
	  UG_NS<coorddim>::Transformation(corners(), cornerCoords, local, jac_inverse_); 
	}
  else
	{
	  // compute the transformation onto the reference element (or vice versa?)
	  UG_NS<coorddim>::Transformation(corners(), cornerpointers_, local, jac_inverse_); 
	}

  return jac_inverse_;
}



///////////////////////////////////////////////////////////////////////
//
// The specializations 1->2, 2->3
// (only methods that are not yet defined in header file
//
///////////////////////////////////////////////////////////////////////



// Specialization for dim==1, coorddim==2.  This is necessary
// because we specialized the whole class
template <class GridImp>
inline FieldVector<typename GridImp::ctype, 2> UGGridGeometry<1,2,GridImp>:: 
global(const FieldVector<typename GridImp::ctype, 1>& local) const
{
    FieldVector<UGCtype, 2> globalCoord;

    /** \todo Rewrite this once there are expression templates */
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

    if (elementType_.isSimplex()) {

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

