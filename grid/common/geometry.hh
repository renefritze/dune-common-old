#ifndef DUNE_GRID_GEOMETRY_HH
#define DUNE_GRID_GEOMETRY_HH

#include <dune/common/matvec.hh>
#include <dune/common/helpertemplates.hh>
#include <dune/common/exceptions.hh>

namespace Dune
{

//*****************************************************************************
//
// Geometry
// forwards the interface to the implementation
//
//*****************************************************************************

template<int mydim, int cdim, class GridImp, template<int,int,class> class GeometryImp>  
class Geometry {
  // save typing
  typedef typename GridImp::ctype ct;
protected:
  GeometryImp<mydim,cdim,GridImp> realGeometry;
public:
  //! know dimension
  enum { dimension=GridImp::dimension };
  //! know dimension of geometry
  enum { mydimension=mydim };
  //! know dimension of embedding coordsystem
  enum { coorddimension=cdim };
  //! know dimension of world
  enum { dimensionworld=GridImp::dimensionworld };
  //! define type used for coordinates in grid module
  typedef ct ctype;
  //! the Reference Geometry
  typedef Geometry<mydim,mydim,GridImp,GeometryImp> ReferenceGeometry;
  
  //! return the geometry type identifier
  GeometryType type () const { return realGeometry.type(); };

  //! return the number of corners of this geometry. Corners are numbered 0...n-1
  int corners () const { return realGeometry.corners(); };

  //! access to coordinates of corners. Index is the number of the corner 
  const FieldVector<ct, cdim>& operator[] (int i) const
    {
      return realGeometry[i];
    }

  /*! return reference geometry corresponding to this geometry. If this is
    a reference geometry then self is returned. A reference to a reference
    geometry is returned. Usually, the implementation will store the finite
    set of reference geometries as global variables.
  */
  static const ReferenceGeometry& refelem ()
    {
      return GeometryImp<mydim,mydim,GridImp>::refelem();
    }

  //! maps a local coordinate within reference geometry to global coordinate in geometry 
  FieldVector<ct, cdim> global (const FieldVector<ct, mydim>& local) const
    {
      return realGeometry.global(local);
    }

  //! maps a global coordinate within the geometry to a local coordinate in its reference geometry
  FieldVector<ct, mydim> local (const FieldVector<ct, cdim>& global) const
    {
      return realGeometry.local(global);
    }

  //! return true if the point in local coordinates lies inside the reference geometry
  bool checkInside (const FieldVector<ct, mydim>& local) const
    {
      return realGeometry.checkInside(local);
    }

  /*! Integration over a general geometry is done by integrating over the reference geometry
    and using the transformation from the reference geometry to the global geometry as follows:
    \f[\int\limits_{\Omega_e} f(x) dx = \int\limits_{\Omega_{ref}} f(g(l)) A(l) dl \f] where
    \f$g\f$ is the local to global mapping and \f$A(l)\f$ is the integration geometry. 

    For a general map \f$g(l)\f$ involves partial derivatives of the map (surface geometry of
    the first kind if \f$d=2,w=3\f$, determinant of the Jacobian of the transformation for
    \f$d=w\f$, \f$\|dg/dl\|\f$ for \f$d=1\f$).

    For linear geometries, the derivatives of the map with respect to local coordinates
    do not depend on the local coordinates and are the same over the whole geometry.

    For a structured mesh where all edges are parallel to the coordinate axes, the 
    computation is the length, area or volume of the geometry is very simple to compute.
 
    Each grid module implements the integration geometry with optimal efficieny. This
    will directly translate in substantial savings in the computation of finite geometry
    stiffness matrices.
   */
  ct integrationElement (const FieldVector<ct, mydim>& local) const
    {
      return realGeometry.integrationElement(local);
    }

  //! can only be called for mydim=cdim!
  const Mat<mydim,mydim,ct>& jacobianInverse (const FieldVector<ct, mydim>& local) const
    {
      assert(mydim == cdim);
      return realGeometry.jacobianInverse(local);
    }

  //! copy constructor from GeometryImp
  Geometry(const GeometryImp<mydim,cdim,GridImp> & e) : realGeometry(e) {};
};

//! Specialization of Geometry for mydim=0 (vertices)
template<int cdim, class GridImp, template<int,int,class> class GeometryImp>  
class Geometry<0,cdim,GridImp,GeometryImp> {
  // save typing
  typedef typename GridImp::ctype ct;
protected:
  GeometryImp<0,cdim,GridImp> realGeometry;
public:
  //! know dimension
  enum { dimension=GridImp::dimension };
  //! know dimension of geometry
  enum { mydimension=0 };
  //! know dimension of embedding coordsystem
  enum { coorddimension=cdim };
  //! know dimension of world
  enum { dimensionworld=GridImp::dimensionworld };
  //! define type used for coordinates in grid module
  typedef ct ctype;

  //! return the geometry type identifier
  GeometryType type () const { return realGeometry.type(); };

  //! return the number of corners of this geometry. Corners are numbered 0...n-1
  int corners () const { return realGeometry.corners(); };

  //! access to coordinates of corners. Index is the number of the corner 
  const FieldVector<ct, cdim>& operator[] (int i) const
    {
      return realGeometry[i];
    }

  //! copy constructor from GeometryImp
  Geometry(const GeometryImp<0,cdim,GridImp> & e) : realGeometry(e) {};
};

//************************************************************************
// GEOMETRY Interface
//************************************************************************

/*!
  Geometry realizes the concept of the geometric part of a mesh entity.
  
  The geometric part of a mesh entity is a \f$d\f$-dimensional object in \f$\mathbf{R}^w\f$
  where \f$d\f$ corresponds the template parameter dim and \f$w\f$ corresponds to the
  template parameter dimworld.

  The \f$d\f$-dimensional object is a polyhedron given by a certain number of corners, which
  are vectors in \f$\mathbf{R}^w\f$.

  The member function global provides a map from a topologically equivalent polyhedron ("reference geometry")
  in \f$\mathbf{R}^d\f$ to the given polyhedron. This map can be inverted by the member function local, where
  an appropriate projection is applied first, when \f$d\neq w\f$.

  In the case of a structured mesh discretizing a generalized cube this map is linear
  and can be described as \f[ g(l) = s + \sum\limits_{i=0}^{d-1} l_ir^i\f] where \f$s\in\mathbf{R}^w\f$
  is a given position vector, the \f$r^i\in\mathbf{R}^w\f$ are given direction vectors and \f$l\in\mathbf{R}^d\f$
  is a local coordinate within the reference polyhedron. The direction vectors are assumed to be orthogonal
  with respect to the standard Euclidean inner product.

  The \f$d\f$-dimensional reference polyhedron is given
  by the points \f$\{ (x_0,\ldots,x_{d-1}) \ | \ x_i\in\{0,1\}\ \}\f$.
  
  In order to invert the map for a point \f$p\f$, we have to find a local coordinate \f$l\f$ such
  that \f$g(l)=p\f$. Of course this is only possible if \f$d=w\f$. In the general case \f$d\leq w\f$
  we determine \f$l\f$ such that
  \f[(s,r^k) + \sum\limits_{i=0}^{d-1} l_i (r^i,r^k) = (p,r^k) \ \ \ \forall k=0,\ldots,d-1. \f]

  The resulting system is diagonal since the direction vectors are required to be orthogonal.

  Geometries with dimension 0 (i.e. vertices) are defined through specialization.
 */
template<int mydim, int cdim, class GridImp, template<int,int,class> class GeometryImp>  
class GeometryInterface {
  // save typing
  typedef typename GridImp::ctype ct;
public:
  //! know dimension
  enum { dimension=GridImp::dimension };
  //! know dimension of geometry
  enum { mydimension=mydim };
  //! know dimension of embedding coordsystem
  enum { coorddimension=cdim };
  //! know dimension of world
  enum { dimensionworld=GridImp::dimensionworld };
  //! define type used for coordinates in grid module
  typedef ct ctype;
  //! the Reference Geometry
  typedef Geometry<mydim,mydim,GridImp,GeometryImp> ReferenceGeometry;

  //! return the geometry type identifier
  GeometryType type () const { return asImp().type(); }

  //! return the number of corners of this geometry. Corners are numbered 0...n-1
  int corners () const { return asImp().corners(); }

  //! access to coordinates of corners. Index is the number of the corner 
  const FieldVector<ct, cdim>& operator[] (int i) const
    {
      return asImp().operator[](i);
    }

  /*! return reference geometry corresponding to this geometry. If this is
    a reference geometry then self is returned. A reference to a reference
    geometry is returned. Usually, the implementation will store the finite
    set of reference geometries as global variables.
  */
  static const ReferenceGeometry& refelem ()
    {
      return GeometryImp<mydim,mydim,GridImp>::refelem();
    }

  //! maps a local coordinate within reference geometry to global coordinate in geometry 
  FieldVector<ct, cdim> global (const FieldVector<ct, mydim>& local) const
    {
      return asImp().global(local);
    }

  //! maps a global coordinate within the geometry to a local coordinate in its reference geometry
  FieldVector<ct, mydim> local (const FieldVector<ct, cdim>& global) const
    {
      return asImp().local(global);
    }

  //! return true if the point in local coordinates lies inside the reference geometry
  bool checkInside (const FieldVector<ct, mydim>& local) const
    {
      return asImp().checkInside(local);
    }

  /*! Integration over a general geometry is done by integrating over the reference geometry
    and using the transformation from the reference geometry to the global geometry as follows:
    \f[\int\limits_{\Omega_e} f(x) dx = \int\limits_{\Omega_{ref}} f(g(l)) A(l) dl \f] where
    \f$g\f$ is the local to global mapping and \f$A(l)\f$ is the integration geometry. 

    For a general map \f$g(l)\f$ involves partial derivatives of the map (surface geometry of
    the first kind if \f$d=2,w=3\f$, determinant of the Jacobian of the transformation for
    \f$d=w\f$, \f$\|dg/dl\|\f$ for \f$d=1\f$).

    For linear geometries, the derivatives of the map with respect to local coordinates
    do not depend on the local coordinates and are the same over the whole geometry.

    For a structured mesh where all edges are parallel to the coordinate axes, the 
    computation is the length, area or volume of the geometry is very simple to compute.
 
    Each grid module implements the integration geometry with optimal efficieny. This
    will directly translate in substantial savings in the computation of finite geometry
    stiffness matrices.
   */
  ct integrationElement (const FieldVector<ct, mydim>& local) const
    {
      return asImp().integrationElement(local);
    }

  //! can only be called for dim=dimworld!
  const Mat<cdim,cdim,ct>& jacobianInverse (const FieldVector<ct, mydim>& local) const
    {
      return asImp().jacobianInverse(local);
    }

private:
  //!  Barton-Nackman trick 
  GeometryImp<mydim,cdim,GridImp>& asImp () {return static_cast<GeometryImp<mydim,cdim,GridImp>&>(*this);}
  const GeometryImp<mydim,cdim,GridImp>& asImp () const {return static_cast<const GeometryImp<mydim,cdim,GridImp>&>(*this);}
};

//! Specialization of Geometry for dim=0 (vertices)
template<int cdim, class GridImp, template<int,int,class> class GeometryImp>  
class GeometryInterface<0,cdim,GridImp,GeometryImp> {
  // save typing
  typedef typename GridImp::ctype ct;
public:
  //! know dimension
  enum { dimension=GridImp::dimension };
  //! know dimension of geometry
  enum { mydimension=0 };
  //! know dimension of embedding coordsystem
  enum { coorddimension=cdim };
  //! know dimension of world
  enum { dimensionworld=GridImp::dimensionworld };
  //! define type used for coordinates in grid module
  typedef ct ctype;

  //! return the geometry type identifier
  GeometryType type () const { return asImp().type(); }

  //! return the number of corners of this geometry. Corners are numbered 0...n-1
  int corners () const { return asImp().corners(); }

  //! access to coordinates of corners. Index is the number of the corner 
  const FieldVector<ct, cdim>& operator[] (int i) const
    {
      return asImp().operator[](i);
    }

private:
  //!  Barton-Nackman trick 
  GeometryImp<0,cdim,GridImp>& asImp () {return static_cast<GeometryImp<0,cdim,GridImp>&>(*this);}
  const GeometryImp<0,cdim,GridImp>& asImp () const {return static_cast<const GeometryImp<0,cdim,GridImp>&>(*this);}
};

//*************************************************************************
//
// --GeometryDefault 
// 
//! Default implementation for class Geometry
//
//*************************************************************************
template<int mydim, int cdim, class GridImp, template<int,int,class> class GeometryImp>  
class GeometryDefault
  : public GeometryInterface <mydim,cdim,GridImp,GeometryImp>
{
  // save typing
  typedef typename GridImp::ctype ct;
private:
  //!  Barton-Nackman trick 
  GeometryImp<mydim,cdim,GridImp>& asImp () {return static_cast<GeometryImp<mydim,cdim,GridImp>&>(*this);}
  const GeometryImp<mydim,cdim,GridImp>& asImp () const {return static_cast<const GeometryImp<mydim,cdim,GridImp>&>(*this);}
}; // end GeometryDefault 

//! Default implementation for class Geometry (vertex)
template<int cdim, class GridImp, template<int,int,class> class GeometryImp>  
class GeometryDefault <0,cdim,GridImp,GeometryImp> 
  : public GeometryInterface <0,cdim,GridImp,GeometryImp>
{
  // save typing
  typedef typename GridImp::ctype ct;
private:
  //!  Barton-Nackman trick 
  GeometryImp<0,cdim,GridImp>& asImp () {return static_cast<GeometryImp<0,cdim,GridImp>&>(*this);}
  const GeometryImp<0,cdim,GridImp>& asImp () const {return static_cast<const GeometryImp<0,cdim,GridImp>&>(*this);}
}; // end GeometryDefault, dim = 0

}

#endif // DUNE_GRID_GEOMETRY_HH
