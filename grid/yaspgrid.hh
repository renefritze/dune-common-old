#ifndef __YASPGRID_HH__
#define __YASPGRID_HH__

#include<iostream>

#include"common/grid.hh"     // the grid base classes
#include"yaspgrid/grids.hh"  // the yaspgrid base classes

/*! \file yaspgrid.hh
  Yaspgrid stands for yet another structured parallel grid.
  It will implement the dune grid interface for structured grids with codim 0
  and dim, with arbitrary overlap, parallel features with two overlap
  models, periodic boundaries and fast a implementation allowing on-the-fly computations.
*/

namespace Dune {

/** @defgroup Yaspgrid Yet Another Structured Parallel Grid
    \ingroup GridCommon

  Yaspgrid stands for yet another structured parallel grid.
  It will implement the dune grid interface for structured grids with codim 0
  and dim, with arbitrary overlap, parallel features with two overlap
  models, periodic boundaries and fast a implementation allowing on-the-fly computations.


  History:
  - started on July 31, 2004 by PB based on abstractions developed in summer 2003
        @{
 */


//************************************************************************
/*! define name for floating point type used for coordinates in yaspgrid.
  You can change the type for coordinates by changing this single typedef.
 */
typedef double yaspgrid_ctype; 

static const yaspgrid_ctype yasptolerance=1E-13; // tolerance in coordinate computations

//************************************************************************
// forward declaration of templates

template<int dim, int dimworld>            class YaspElement;
template<int codim, int dim, int dimworld> class YaspEntity;
template<int dim, int dimworld>            class YaspGrid;
  //template <int dim, int dimworld>           class YaspBoundaryEntity;
  //template<int codim, int dim, int dimworld> class YaspLevelIterator;
  //template<int dim, int dimworld>            class YaspIntersectionIterator;
  //template<int dim, int dimworld>            class YaspHierarchicIterator;


//========================================================================
// The reference elements

/** Singleton holding reference elements */
template<int dim>
struct YaspReferenceElement {
  static Vec<dim,yaspgrid_ctype> midpoint;  // data neded for the refelem below
  static Vec<dim,yaspgrid_ctype> extension; // data needed for the refelem below
  static YaspElement<dim,dim> refelem;
};

// initialize static variable with bool constructor (which makes reference elements)
template<int dim>
YaspElement<dim,dim> YaspReferenceElement<dim>::refelem(YaspReferenceElement<dim>::midpoint,
														YaspReferenceElement<dim>::extension,
														true);
template<int dim>
Vec<dim,yaspgrid_ctype> YaspReferenceElement<dim>::midpoint;

template<int dim>
Vec<dim,yaspgrid_ctype> YaspReferenceElement<dim>::extension;

//========================================================================
/*!
  YaspElement realizes the concept of the geometric part of a mesh entity.

  We have specializations for dim==dimworld (elements), 
  dim = dimworld-1 (faces) and dim=0 (vertices).
  The general version throws a GridError on construction.
 */
//========================================================================

//! The general version implements dimworld==dimworld. If this is not the case an error is thrown
template<int dim,int dimworld> 
class YaspElement //: public ElementDefault<dim,dimworld,yaspgrid_ctype,YaspElement>
{
public:
  //! define type used for coordinates in grid module
  typedef yaspgrid_ctype ctype;

  //! return the element type identifier
  ElementType type ()           
  {
        switch (dim)
          {
          case 1: return line;
          case 2: return quadrilateral;
          case 3: return hexahedron;
          default: return unknown;
          }
  }

  //! return the number of corners of this element. Corners are numbered 0...n-1
  int corners ()                
  {
        return 1<<dim;
  }
  
  //! access to coordinates of corners. Index is the number of the corner 
  Vec<dim,yaspgrid_ctype>& operator[] (int i)
  {
	int bit=0;
	for (int k=0; k<dimworld; k++) // run over all directions in world
	  {
		if (k==missing)
		  {
			c(k) = midpoint(k);
			continue;
		  }
		//k is not the missing direction 
		if (i&(1<<bit))  // check whether bit is set or not
		  c(k) = midpoint(k)+0.5*extension(k); // bit is 1 in i
		else
		  c(k) = midpoint(k)-0.5*extension(k); // bit is 0 in i
		bit++; // we have processed a direction
	  }

	return c;
  }

  /*! return reference element corresponding to this element. 
	  Usually, the implementation will store the finite
	  set of reference elements as global variables.
	  But why in the hell do we need this reference element?
  */
  static YaspElement<dim,dim>& refelem ()   
  {
	return YaspReferenceElement<dim>::refelem;
  }

  //! maps a local coordinate within reference element to global coordinate in element 
  Vec<dimworld,yaspgrid_ctype> global (const Vec<dim,yaspgrid_ctype>& local)
  {
	Vec<dimworld,yaspgrid_ctype> g;
	int bit=0;
	for (int k=0; k<dimworld; k++)
	  if (k==missing)
		g(k) = midpoint(k);
	  else
		{
		  g(k) = midpoint(k) + (local(bit)-0.5)*extension(k);
		  bit++;
		}
	return g;
  }

  //! maps a global coordinate within the element to a local coordinate in its reference element
  Vec<dim,yaspgrid_ctype> local (const Vec<dimworld,yaspgrid_ctype>& global)
  {
	Vec<dim,yaspgrid_ctype> l; // result
	int bit=0;
	for (int k=0; k<dimworld; k++)
	  if (k!=missing)
		{
		  l(bit) = (global(k)-midpoint(k))/extension(k) + 0.5;
		  bit++;
		}
	return l;
  }

  /*! determinant of the jacobian of the mapping
   */
  yaspgrid_ctype integration_element (const Vec<dim,yaspgrid_ctype>& local)
  {
	yaspgrid_ctype volume=1.0;
	for (int k=0; k<dim; k++) 
	  if (k!=missing) volume *= extension(k);
	return volume;
  }

  //! check whether local is inside reference element
  bool checkInside (const Vec<dim,yaspgrid_ctype>& local)
  {
	for (int i=0; i<dim; i++)
	  if (local(i)<-yasptolerance || local(i)>1+yasptolerance) return false;
	return true;
  }

  //! constructor from (storage for) midpoint and extension and missing direction number
  YaspElement (Vec<dimworld,yaspgrid_ctype>& p, Vec<dimworld,yaspgrid_ctype>& h, int m)
	: midpoint(p), extension(h), missing(m)
  {
	if (dimworld!=dim+1)
	  throw GridError("general YaspElement assumes dimworld=dim+1",__FILE__,__LINE__);
  }

  //! print function
  void print (std::ostream& s)
  {
        s << "YaspElement<"<<dim<<","<<dimworld<< "> ";
        s << "midpoint";
        for (int i=0; i<dimworld; i++) 
		  s << " " << midpoint[i];
		s << " extension";
        for (int i=0; i<dimworld; i++) 
		  s << " " << extension[i];
		s << " missing is " << missing;
  }

private:
  // the element is fully defined by its midpoint the extension
  // in each direction and the missing direction. 
  // References are used because this information
  // is known outside the element in many cases.
  // Note dimworld==dim+1
  Vec<dimworld,yaspgrid_ctype>& midpoint;   // the midpoint
  Vec<dimworld,yaspgrid_ctype>& extension;  // the extension
  int missing;                           // the missing, i.e. constant direction

  // In addition we need memory in order to return references.
  // Possibly we should change this in the interface ...
  Vec<dim,yaspgrid_ctype> c;             // a point
};



//! specialize for dim=dimworld, i.e. a volume element
template<int dim> 
class YaspElement<dim,dim> //: public ElementDefault<dim,dim,yaspgrid_ctype,YaspElement>
{
public:
  //! define type used for coordinates in grid module
  typedef yaspgrid_ctype ctype;

  //! return the element type identifier
  ElementType type ()           
  {
        switch (dim)
          {
          case 1: return line;
          case 2: return quadrilateral;
          case 3: return hexahedron;
          default: return unknown;
          }
  }

  //! return the number of corners of this element. Corners are numbered 0...n-1
  int corners ()                
  {
        return 1<<dim;
  }
  
  //! access to coordinates of corners. Index is the number of the corner 
  Vec<dim,yaspgrid_ctype>& operator[] (int i)
  {
	for (int k=0; k<dim; k++)
	  if (i&(1<<k))  
		c(k) = midpoint(k)+0.5*extension(k); // kth bit is 1 in i
	  else
		c(k) = midpoint(k)-0.5*extension(k); // kth bit is 0 in i
	return c;
  }

  /*! return reference element corresponding to this element. 
	  Usually, the implementation will store the finite
	  set of reference elements as global variables.
	  But why in the hell do we need this reference element?
  */
  static YaspElement<dim,dim>& refelem ()   
  {
	return YaspReferenceElement<dim>::refelem;
  }

  //! maps a local coordinate within reference element to global coordinate in element 
  Vec<dim,yaspgrid_ctype> global (const Vec<dim,yaspgrid_ctype>& local)
  {
	Vec<dim,yaspgrid_ctype> g;
	for (int k=0; k<dim; k++) 
	  g(k) = midpoint(k) + (local(k)-0.5)*extension(k);
	return g;
  }

  //! maps a global coordinate within the element to a local coordinate in its reference element
  Vec<dim,yaspgrid_ctype> local (const Vec<dim,yaspgrid_ctype>& global)
  {
	Vec<dim,yaspgrid_ctype> l; // result
	for (int k=0; k<dim; k++) 
	  l(k) = (global(k)-midpoint(k))/extension(k) + 0.5;
	return l;
  }

  /*! determinant of the jacobian of the mapping
   */
  yaspgrid_ctype integration_element (const Vec<dim,yaspgrid_ctype>& local)
  {
	yaspgrid_ctype volume=1.0;
	for (int k=0; k<dim; k++) volume *= extension(k);
	return volume;
  }

  //! can only be called for dim=dim!
  Mat<dim,dim>& Jacobian_inverse (const Vec<dim,yaspgrid_ctype>& local)
  {
	for (int i=0; i<dim; ++i) 
	  {
		Jinv(i) = 0.0;                // set column to zero
		Jinv(i,i) = 1.0/extension(i); // set diagonal element
	  }
	return Jinv;
  }
  
  //! check whether local is inside reference element
  bool checkInside (const Vec<dim,yaspgrid_ctype>& local)
  {
	for (int i=0; i<dim; i++)
	  if (local(i)<-yasptolerance || local(i)>1+yasptolerance) return false;
	return true;
  }

  //! constructor from (storage for) midpoint and extension
  YaspElement (Vec<dim,yaspgrid_ctype>& p, Vec<dim,yaspgrid_ctype>& h)
	: midpoint(p), extension(h)
  {}

  //! constructor from (storage for) midpoint and extension initializing reference element
  YaspElement (Vec<dim,yaspgrid_ctype>& p, Vec<dim,yaspgrid_ctype>& h, bool b)
	: midpoint(p), extension(h)
  {
	for (int i=0; i<dim; i++)
	  {
		midpoint[i] = 0.5;
		extension[i] = 1.0;
	  }
  }

  //! print function
  void print (std::ostream& s)
  {
        s << "YaspElement<"<<dim<<","<<dim<< "> ";
        s << "midpoint";
        for (int i=0; i<dim; i++) 
		  s << " " << midpoint[i];
		s << " extension";
        for (int i=0; i<dim; i++) 
		  s << " " << extension[i];
  }

private:
  // the element is fully defined by midpoint and the extension
  // in each direction. References are used because this information
  // is known outside the element in many cases.
  // Note dim==dimworld
  Vec<dim,yaspgrid_ctype>& midpoint;   // the midpoint
  Vec<dim,yaspgrid_ctype>& extension;  // the extension

  // In addition we need memory in order to return references.
  // Possibly we should change this in the interface ...
  Mat<dim,dim,yaspgrid_ctype> Jinv;       // the jacobian inverse
  Vec<dim,yaspgrid_ctype> c;           // a point
};




//! specialization for dim=0, this is a vertex
template<int dimworld> 
class YaspElement<0,dimworld> //: public ElementDefault<0,dimworld,yaspgrid_ctype,YaspElement>
{
public:
  //! define type used for coordinates in grid module
  typedef yaspgrid_ctype ctype;
  
  //! return the element type identifier
  ElementType type ()             
  {
	return vertex;
  }
  
  //! return the number of corners of this element. Corners are numbered 0...n-1
  int corners ()                  
  {
	return 1;
  }     

  //! access to coordinates of corners. Index is the number of the corner 
  Vec<dimworld,yaspgrid_ctype>& operator[] (int i)
  {
	return position;
  }
  
  //! constructor
  YaspElement (Vec<dimworld,yaspgrid_ctype>& p) : position(p)
  {
  }

  //! print function
  void print (std::ostream& s)
  {
        s << "YaspElement<"<<0<<","<<dimworld<< "> ";
        s << "position " << position;
  }
  
private:
  Vec<dimworld,yaspgrid_ctype>& position; //!< where the vertex is
};

// operator<< for all YaspElements
template <int dim, int dimworld>
inline std::ostream& operator<< (std::ostream& s, YaspElement<dim,dimworld>& e)
{
  e.print(s);
  return s;
}

//========================================================================
/*!
  YaspEntity realizes the concept a mesh entity.

  We have specializations for codim==0 (elements) and 
  codim=dim (vertices).
  The general version throws a GridError.
 */
//========================================================================


template<int codim, int dim, int dimworld> 
class YaspEntity //:  public EntityDefault <codim,dim,dimworld,yaspgrid_ctype,YaspEntity,YaspElement,
                 //				   YaspLevelIterator,YaspIntersectionIterator,YaspHierarchicIterator>
{
public:
  //! define type used for coordinates in grid module
  typedef yaspgrid_ctype ctype;

  //! level of this element
  int level () 
  {
	throw GridError("YaspEntity not implemented",__FILE__,__LINE__);
  }

  //! index is unique and consecutive per level and codim used for access to degrees of freedom
  int index () 
  {
	throw GridError("YaspEntity not implemented",__FILE__,__LINE__);
  }
  
  //! geometry of this entity
  YaspElement<dim-codim,dimworld>& geometry () 
  {
	throw GridError("YaspEntity not implemented",__FILE__,__LINE__);
  }
};


// specialization for codim=0
template<int dim, int dimworld> 
class YaspEntity<0,dim,dimworld> //:  public EntityDefault <0,dim,dimworld,yaspgrid_ctype,YaspEntity,YaspElement,
								 //	   YaspLevelIterator,YaspIntersectionIterator,YaspHierarchicIterator>
{
public:
  typedef typename MultiYGrid<dim,yaspgrid_ctype>::YGridLevelIterator YGLI;
  typedef typename YGrid<dim,yaspgrid_ctype>::TransformingIterator TI;

  //! define type used for coordinates in grid module
  typedef yaspgrid_ctype ctype;

  // constructor
  YaspEntity (YGLI& g, TI& it)
	: _it(it), _g(g), _element(it.position(),it.meshsize())
  {
  }
  
  //! level of this element
  int level () {return _g.level();}

  //! index is unique and consecutive per level
  int index () {return _it.index();}

  //! geometry of this entity
  YaspElement<dim,dimworld>& geometry () {return _element;}

  /*! Intra-element access to entities of codimension cc > codim. Return number of entities
	with codimension cc.
  */
  template<int cc> 
  int count ()
  {
	if (cc==dim) return 1<<dim;
	if (cc==1)   return 2*dim;
  }  

private:
  TI& _it;                            // position in the grid level
  YGLI& _g;                           // access to grid level
  YaspElement<dim,dimworld> _element; // the element geometry
};


//========================================================================
/*!
  YaspLevelIterator enables iteration over entities of one grid level

  We have specializations for codim==0 (elements) and 
  codim=dim (vertices).
  The general version throws a GridError.
 */
//========================================================================

// the general version
template<int codim, int dim, int dimworld>
class YaspLevelIterator //: public LevelIteratorDefault<codim,dim,dimworld,yaspgrid_ctype,YaspLevelIterator,YaspEntity>
{
  YaspLevelIterator () 
  {
	throw GridError("YaspLevelIterator not implemented",__FILE__,__LINE__);
  }
};

// specialization for codim==0 -- the elements
template<int dim, int dimworld>
class YaspLevelIterator<0,dim,dimworld> //: public LevelIteratorDefault<0,dim,dimworld,yaspgrid_ctype,YaspLevelIterator,YaspEntity>
{
public:
  typedef typename MultiYGrid<dim,yaspgrid_ctype>::YGridLevelIterator YGLI;
  typedef typename YGrid<dim,yaspgrid_ctype>::TransformingIterator TI;

  //! constructor
  YaspLevelIterator (YGLI g, TI it) : _g(g), _it(it), _entity(_g,_it) 
  {  }

  //! prefix increment
  YaspLevelIterator<0,dim,dimworld>& operator++()
  {
	++_it;
	return *this;
  }

  //! equality
  bool operator== (const YaspLevelIterator<0,dim,dimworld>& i) const
  {
	return _it==i._it;
  }

  //! inequality
  bool operator!= (const YaspLevelIterator<0,dim,dimworld>& i) const
  {
	return (_it!=i._it);
  }

  //! dereferencing
  YaspEntity<0,dim,dimworld>& operator*()
  {
	return _entity;
  }

  //! arrow
  YaspEntity<0,dim,dimworld>* operator->()
  {
	return &_entity;
  }

  //! ask for level of entity
  int level () {return _g.level();}

private:
  YGLI _g;                            // access to grid level
  TI _it;                             // position in the grid level
  YaspEntity<0,dim,dimworld> _entity; //!< virtual entity
};


//************************************************************************
/*!
  A Grid is a container of grid entities. Given a dimension dim these entities have a 
  codimension codim with 0 <= codim <= dim. 

  The Grid is assumed to be hierachically refined and nested. It enables iteration over
  entities of a given level and codimension.

  The grid can consist of several subdomains and it can be non-matching.

  All information is provided to allocate degrees of freedom in appropriate vector
  data structures (which are not part of this module).
 */
template<int dim, int dimworld>
class YaspGrid //: public GridDefault<dim,dimworld,yaspgrid_ctype,YaspGrid,YaspLevelIterator,YaspEntity>
{
public:
  //! maximum number of levels allowed
  enum { MAXL=64 };

  //! define type used for coordinates in grid module
  typedef yaspgrid_ctype ctype;

  //! shorthand for base class data types
  typedef MultiYGrid<dim,ctype> YMG;
  typedef typename MultiYGrid<dim,ctype>::YGridLevelIterator YGLI;

  //! return GridIdentifierType of Grid, i.e. SGrid_Id or AlbertGrid_Id ... 
  GridIdentifier type() const { return YaspGrid_Id; };
  
  /*! Constructor for a YaspGrid, they are all forwarded to the base class
	@param comm MPI communicator where this mesh is distributed to
	@param L extension of the domain
	@param s number of cells on coarse mesh in each direction
    @param periodic tells if direction is periodic or not
    @param size of overlap on coarsest grid (same in all directions)
  */
  YaspGrid (MPI_Comm comm, Dune::Vec<dim,ctype> L, Dune::Vec<dim,int> s, Dune::Vec<dim,bool> periodic, int overlap)
			: _mg(comm,L,s,periodic,overlap)
  {  }

  /*! Return maximum level defined in this grid. Levels are numbered
	0 ... maxlevel with 0 the coarsest level.
  */
  int maxlevel() const {return _mg.maxlevel();} // delegate
    
  //! refine the grid refCount times. What about overlap?
  void globalRefine (int refCount)
  {
	bool b=false;
	if (refCount>0) b=true;
	_mg.refine(b);
  }

  //! Iterator to first entity of given codim on level
  template<int cd>
  YaspLevelIterator<cd,dim,dimworld> lbegin (int level)
  {
	YGLI g = _mg.begin(level);
	if (cd==0) // the elements
	  {
		return YaspLevelIterator<cd,dim,dimworld>(g,g.cell_overlap().tbegin());
	  }
	throw GridError("YaspLevelIterator with this codim not implemented",__FILE__,__LINE__);	
  }

  //! one past the end on this level
  template<int cd>
  YaspLevelIterator<cd,dim,dimworld> lend (int level)
  {
	YGLI g = _mg.begin(level);
	if (cd==0) // the elements
	  {
		return YaspLevelIterator<cd,dim,dimworld>(g,g.cell_overlap().tend());
	  }
	throw GridError("YaspLevelIterator with this codim not implemented",__FILE__,__LINE__);	
  }

private:
  YMG _mg;
};



/** @} end documentation group */

} // end namespace


#endif

