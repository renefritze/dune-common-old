#ifndef DUNE_ALBERTGRID_HH
#define DUNE_ALBERTGRID_HH


#include <vector>

// if defined, then used Albert Index 
//#define REALINDEX 

#ifdef __ALBERTNAME__
#define ALBERT Albert:: 
namespace Albert 
{

/** @defgroup AlbertGrid AlbertGrid Module 

  This is one implementation of the grid interface providing 2d triangle 
  and 3d tetrahedra grids by using the FE Box ALBERT by Kunibert Siebert 
  and Alfred Schmidt.

  @{
 */
  
#else 
#define ALBERT
#endif
 
#ifndef __ALBERTNAME__
extern "C" 
{
#endif

// the original ALBERT Lib 
#include <albert.h>

// some extra functions for handling the Albert Mesh  
#include "albertgrid/albertextra.hh"

#ifdef ABS
#undef ABS
#endif
  
#ifdef MIN
#undef MIN
#endif
  
#ifdef MAX
#undef MAX
#endif
  
#ifndef __ALBERTNAME__
} // end extern "C"
#endif

#ifdef __ALBERTNAME__
}//end namespace Albert
#endif

#include "../common/misc.hh"
#include "../common/matvec.hh"
#include "../common/array.hh"
#include "common/grid.hh"

namespace Dune 
{
/** @defgroup AlbertGrid AlbertGrid Module 

  This is one implementation of the grid interface providing 2d triangle 
  and 3d tetrahedra grids by using the FE Box ALBERT ( ALBERT 
  was written by Kunibert Siebert 
  and Alfred Schmidt).

  @{
 */
  

// i.e. double or float 
typedef ALBERT REAL albertCtype;

// forward declarations 
class AlbertMarkerVector;

template<int codim, int dim, int dimworld> class AlbertGridEntity;
template<int codim, int dim, int dimworld> class AlbertGridLevelIterator;

template<int dim, int dimworld>            class AlbertGridElement;
template<int dim, int dimworld>            class AlbertGridBoundaryEntity;
template<int dim, int dimworld>            class AlbertGridHierarchicIterator;
template<int dim, int dimworld>            class AlbertGridNeighborIterator;
template<int dim, int dimworld>            class AlbertGrid;

template <class Object> class AlbertGridMemory;

// singleton holding reference elements
template<int dim> struct AlbertGridReferenceElement;

//**********************************************************************
//
// --AlbertGridElement
/*!
  Defines the geometry part of a mesh entity. Works for all dimensions, element types and dime
  of world. Provides reference element and mapping between local and global coordinates.
  The element may have different implementations because the mapping can be
  done more efficient for structured meshes than for unstructured meshes.

  dim: An element is a polygonal in a hyperplane of dimension dim. 0 <= dim <= 3 is typically 
  dim=0 is a point.

  dimworld: Each corner is a point with dimworld coordinates.
*/

template<int dim, int dimworld>  
class AlbertGridElement : 
public ElementDefault <dim,dimworld,albertCtype,AlbertGridElement>
{ 
  friend class AlbertGridBoundaryEntity<dim,dimworld>;
public:
  //! know dimension
  enum { dimension=dim };

  //! know dimension of world
  enum { dimensionworld=dimworld };
  
  //! know dimension of world
  enum { dimbary=dim+1};

  //! for makeRefElement == true a Element with the coordinates of the 
  //! reference element is made 
  AlbertGridElement(bool makeRefElement=false);

  //! return the element type identifier
  //! line , triangle or tetrahedron, depends on dim 
  ElementType type ();

  //! return the number of corners of this element. Corners are numbered 0...n-1
  int corners ();

  //! access to coordinates of corners. Index is the number of the corner 
  Vec<dimworld,albertCtype>& operator[] (int i);

  /*! return reference element corresponding to this element. If this is
    a reference element then self is returned.
  */
  static AlbertGridElement<dim,dim>& refelem ();

  //! maps a local coordinate within reference element to 
  //! global coordinate in element 
  Vec<dimworld,albertCtype>& global (const Vec<dim,albertCtype>& local);
  
  //! maps a global coordinate within the element to a 
  //! local coordinate in its reference element
  Vec<dim,albertCtype>& local (const Vec<dimworld,albertCtype>& global);
  
  //! returns true if the point is in the current element
  bool checkInside(const Vec<dimworld,albertCtype> &global);

  /*! 
    Copy from sgrid.hh:

    Integration over a general element is done by integrating over the reference element
    and using the transformation from the reference element to the global element as follows:
    \f[\int\limits_{\Omega_e} f(x) dx = \int\limits_{\Omega_{ref}} f(g(l)) A(l) dl \f] where
    \f$g\f$ is the local to global mapping and \f$A(l)\f$ is the integration element. 

    For a general map \f$g(l)\f$ involves partial derivatives of the map (surface element of
    the first kind if \f$d=2,w=3\f$, determinant of the Jacobian of the transformation for
    \f$d=w\f$, \f$\|dg/dl\|\f$ for \f$d=1\f$).

    For linear elements, the derivatives of the map with respect to local coordinates
    do not depend on the local coordinates and are the same over the whole element.

    For a structured mesh where all edges are parallel to the coordinate axes, the 
    computation is the length, area or volume of the element is very simple to compute.
 
    Each grid module implements the integration element with optimal efficieny. This
    will directly translate in substantial savings in the computation of finite element
    stiffness matrices.
   */

  // A(l) 
  albertCtype integration_element (const Vec<dim,albertCtype>& local);

  //! can only be called for dim=dimworld!
  Mat<dim,dim>& Jacobian_inverse (const Vec<dim,albertCtype>& local);

  //***********************************************************************
  //  Methods that not belong to the Interface, but have to be public
  //***********************************************************************
  //! generate the geometry for the ALBERT EL_INFO 
  bool builtGeom(ALBERT EL_INFO *elInfo, unsigned char face, 
                 unsigned char edge, unsigned char vertex);
  // init geometry with zeros 
  void initGeom();

  //! print internal data
  void print (std::ostream& ss, int indent);
private:

  // calc the local barycentric coordinates 
  template <int dimbary>
  Vec<dimbary,albertCtype>& localB (const Vec<dimworld,albertCtype>& global)
  {
    localBary_ = localBary(global);
    return localBary_;
  }

  //! built the reference element
  void makeRefElemCoords();
  
  //! built the jacobian inverse and store the volume 
  void builtJacobianInverse (const Vec<dim,albertCtype>& local); 
  
  //! maps a barycentric coordinate within element to global coordinate in element 
  Vec<dimworld,albertCtype> globalBary (const Vec<dim+1,albertCtype>& local);

  //! maps a global coordinate within the elements local barycentric
  //! koordinates 
  Vec<dim+1,albertCtype> localBary (const Vec<dimworld,albertCtype>& global);

  // template method for map the vertices of EL_INFO to the actual 
  // coords with face_,edge_ and vertex_ , needes for operator []
  template <int cc> int mapVertices (int i) const 
  {
    return i; 
  }  

  // codim == 1, Faces
  template <> int mapVertices<1> (int i) const 
  {
    // N_VERTICES (from ALBERT) = dim +1 
    // std::cout << "Its me! \n";
    //return ((face_ + 1 + i) % (dimension+1)); 
    return ((face_ + 1 + i) % (dimworld+1)); 
    
  }  

#if DIM == 3
  // codim == 2, dim > 2, Edges 
  template <> int mapVertices<2 << dimworld> (int i) const 
  {
    // N_VERTICES (from ALBERT) = dim +1 
    return ((face_+1)+ (edge_+1) +i)% (dimension+1);
  };  
#else
  template <> int mapVertices<2> (int i) const 
  {
    // N_VERTICES (from ALBERT) = dim +1 
    return ((face_+1)+ (vertex_+1) +i)% (dimension+1);
  }  
#endif

  template <> int mapVertices<3> (int i) const 
  {
    // N_VERTICES (from ALBERT) = dim +1 = 4
    return ((face_+1)+ (edge_+1) +(vertex_+1) +i) %( 4 );
  }  

 
  //! the vertex coordinates 
  Mat<dimworld,dim+1,albertCtype> coord_;

  //! storage for global coords 
  Vec<dimworld,albertCtype> globalCoord_;

  //! storage for local coords
  Vec<dim,albertCtype> localCoord_;

  //! storage for barycentric coords 
  Vec<dimbary,albertCtype> localBary_;
  
  
  ALBERT EL_INFO * makeEmptyElInfo();
  
  ALBERT EL_INFO * elInfo_;
  
  //! Which Face of the Element 0...dim+1
  unsigned char face_;
  
  //! Which Edge of the Face of the Element 0...dim
  unsigned char edge_;
  
  //! Which Edge of the Face of the Element 0...dim-1
  unsigned char vertex_;

  //! is true if Jinv_ and volume_ is calced
  bool builtinverse_;
  Mat<dim,dim,albertCtype> Jinv_;  //!< storage for inverse of jacobian
  albertCtype volume_; //!< storage of element volume
    
  //Vec<dimworld,albertCtype> outerNormal_;
};


//**********************************************************************
//
// --AlbertGridEntity
// --Entity
//
/*! 
  A Grid is a container of grid entities. An entity is parametrized by the codimension.
  An entity of codimension c in dimension d is a d-c dimensional object.

  Here: the general template
 */
template<int codim, int dim, int dimworld>
class AlbertGridEntity : 
public EntityDefault <codim,dim,dimworld,albertCtype,
              AlbertGridEntity,AlbertGridElement,AlbertGridLevelIterator,
              AlbertGridNeighborIterator,AlbertGridHierarchicIterator>
{
  friend class AlbertGrid < dim , dimworld >;
  friend class AlbertGridEntity < 0, dim, dimworld>;
  friend class AlbertGridLevelIterator < codim, dim, dimworld>;
  //friend class AlbertGridLevelIterator < dim, dim, dimworld>;
public:
  //! know your own codimension
  enum { codimension=codim };

  //! know your own dimension
  enum { dimension=dim };

  //! know your own dimension of world
  enum { dimensionworld=dimworld };

  //! level of this element
  int level ();

  //! index is unique and consecutive per level and codim 
  //! used for access to degrees of freedom
  int index ();

  AlbertGridEntity(AlbertGrid<dim,dimworld> &grid, int level, 
                   ALBERT TRAVERSE_STACK * travStack);
  
  AlbertGridEntity(AlbertGrid<dim,dimworld> &grid, int level);

  //! geometry of this entity
  AlbertGridElement<dim-codim,dimworld>& geometry ();

  /*! Intra-element access to entities of codimension cc > codim. Return number of entities
    with codimension cc.
   */

#if 0
  //!< Default codim 1 Faces and codim == dim Vertices 
  template <int cc> int count () { return ((dim-codim)+1); };   

  //! Provide access to mesh entity i of given codimension. Entities
  //!  are numbered 0 ... count<cc>()-1
  template <int cc> AlbertGridLevelIterator<cc,dim,dimworld>& entity (int i)
  {
    AlbertGridLevelIterator<cc,dim,dimworld> vxEntity_(grid_,NULL,0,0,0,0);
    vxEntity_.virtualEntity_.setElInfo(elInfo_,elInfo_->el->dof[((face_ + 1) + i)%(dimension+1)][0],0,0,((face_ + 1) + i)%(dimension+1));
    return vxEntity_;  
  }; 
#endif
  /*! Location of this vertex within a mesh entity of codimension 0 on the coarse grid.
    This can speed up on-the-fly interpolation for linear conforming elements
    Possibly this is sufficient for all applications we want on-the-fly.
  */
  AlbertGridLevelIterator<0,dim,dimworld> father ();

  //! local coordinates within father
  Vec<dim,albertCtype>& local ();
private: 
  // methods for setting the infos from the albert mesh
  void setTraverseStack (ALBERT TRAVERSE_STACK *travStack);
  void setElInfo (ALBERT EL_INFO *elInfo, int elNum, unsigned char face,
                  unsigned char edge, unsigned char vertex );
  // needed for the LevelIterator 
  ALBERT EL_INFO *getElInfo () const;

  // returns the global vertex number as default 
  int globalIndex() { return elInfo_->el->dof[vertex_][0]; }

  // private Methods
  void makeDescription();

  AlbertGrid<dim,dimworld> &grid_;

  // private Members
  ALBERT EL_INFO *elInfo_;
  ALBERT TRAVERSE_STACK * travStack_;

  //! the cuurent geometry
  AlbertGridElement<dim-codim,dimworld> geo_;
  bool builtgeometry_;         //!< true if geometry has been constructed

  //! for vertex access, to be revised, filled on demand
  //AlbertGridLevelIterator<dim,dim,dimworld> vxEntity_;

  Vec<dim,albertCtype> localFatherCoords_; 

  //! element number 
  int elNum_;

  //! level
  int level_;

  //! Which Face of the Element 
  unsigned char face_;

  //! Which Edge of the Face of the Element 
  unsigned char edge_;
  
  //! Which Vertex of the Edge of the Face of the Element 
  unsigned char vertex_;
};

/*! 
  A Grid is a container of grid entities. An entity is parametrized by the codimension.
  An entity of codimension c in dimension d is a d-c dimensional object.

  Entities of codimension 0 ("elements") are defined through template specialization. Note
  that this specialization has an extended interface compared to the general case

  Entities of codimension 0  allow to visit all neighbors, where
  a neighbor is an entity of codimension 0 which has a common entity of codimension 1 with the
  These neighbors are accessed via an iterator. This allows the implementation of
  non-matching meshes. The number of neigbors may be different from the number of faces/edges
  of an element!
 */
//***********************
//  
//  --AlbertGridEntity
//  --0Entity
//
//***********************
template<int dim, int dimworld>
class AlbertGridEntity<0,dim,dimworld> : 
public EntityDefault<0,dim,dimworld,albertCtype,AlbertGridEntity,AlbertGridElement,
              AlbertGridLevelIterator,AlbertGridNeighborIterator,
              AlbertGridHierarchicIterator>
{
  friend class AlbertGrid < dim , dimworld >;
  friend class AlbertMarkerVector;
  friend class AlbertGridNeighborIterator < dim, dimworld>;
  friend class AlbertGridHierarchicIterator < dim, dimworld>;
  friend class AlbertGridLevelIterator <0,dim,dimworld>;
public:
  typedef AlbertGridNeighborIterator<dim,dimworld> NeighborIterator; 
  typedef AlbertGridHierarchicIterator<dim,dimworld> HierarchicIterator; 
  
  //! know your own codimension
  enum { codimension=0 };

  //! know your own dimension
  enum { dimension=dim };

  //! know your own dimension of world
  enum { dimensionworld=dimworld };

  //! Destructor, needed perhaps needed for deleteing faceEntity_ and
  //! edgeEntity_ , see below
  //! there are only implementations for dim==dimworld 2,3
  ~AlbertGridEntity() {};

  //! Constructor, real information is set via setElInfo method
  AlbertGridEntity(AlbertGrid<dim,dimworld> &grid, int level);

  //! level of this element
  int level ();

  //! index is unique and consecutive per level and codim used for access to degrees of freedo
  int index ();

  //! geometry of this entity
  AlbertGridElement<dim,dimworld>& geometry ();

  /*! Intra-element access to entities of codimension cc > codim. Return number of entities
    with codimension cc.
   */
  //!< Default codim 1 Faces and codim == dim Vertices 
  template<int cc> int count () { return (dim+1); };   

  //! specialization only for codim == 2 , edges, 
  //! a tetrahedron has always 6 edges 
  template<> int count<2 << dim> () { return 6; };   

  //! return index of sub entity with codim = cc and local number i
  //! i.e. return global number of vertex i
  template <int cc> int subIndex (int i)
  {
    return (*entity<cc>(i)).index(); 
  }

  template <> int subIndex<dim> (int i)
  {
    return grid_.indexOnLevel<dim>(elInfo_->el->dof[i][0],level_);
  }

  //! Provide access to mesh entity i of given codimension. Entities
  //!  are numbered 0 ... count<cc>()-1
  template<int cc> AlbertGridLevelIterator<cc,dim,dimworld> entity (int i)
  {
    return faceEntity(i); 
  }; 

  template<> AlbertGridLevelIterator<2<<dim,dim,dimworld> 
  entity< 2<<dim > (int i)
  {
    return edgeEntity(i); 
  }; 
  
  template<> AlbertGridLevelIterator<dim,dim,dimworld> entity<dim> (int i)
  {
    vxEntity_.virtualEntity_.setElInfo(elInfo_,elInfo_->el->dof[i][0],0,0,i);
    return vxEntity_;  
  }; 

  /*! Intra-level access to neighboring elements. A neighbor is an entity of codimension 0
    which has an entity of codimension 1 in commen with this entity. Access to neighbors
    is provided using iterators. This allows meshes to be nonmatching. Returns iterator
    referencing the first neighbor. */
  AlbertGridNeighborIterator<dim,dimworld> nbegin ();

  //! Reference to one past the last neighbor
  AlbertGridNeighborIterator<dim,dimworld> nend ();

  //! returns true if Entity has children 
  bool hasChildren () 
  {
    //return (elInfo_->el->child[0] != NULL); 
    return ( level_ != grid_.maxlevel() ); 
  };

  //! return number of layers far from refined elements of this level 
  int refDistance () 
  {
    return 0;
  }

  //! Inter-level access to father element on coarser grid. 
  //! Assumes that meshes are nested.
  AlbertGridLevelIterator<0,dim,dimworld> father ();

  /*! Location of this element relative to the reference element 
    of the father. This is sufficient to interpolate all 
    dofs in conforming case. Nonconforming may require access to 
    neighbors of father and computations with local coordinates.
    On the fly case is somewhat inefficient since dofs  are visited 
    several times. If we store interpolation matrices, this is tolerable. 
    We assume that on-the-fly implementation of numerical algorithms 
    is only done for simple discretizations. Assumes that meshes are nested.
  */
  AlbertGridElement<dim,dim>& father_relative_local ();
  
  /*! Inter-level access to son elements on higher levels<=maxlevel.
    This is provided for sparsely stored nested unstructured meshes.
    Returns iterator to first son.
  */
  AlbertGridHierarchicIterator<dim,dimworld> hbegin (int maxlevel);
  
  //! Returns iterator to one past the last son
  AlbertGridHierarchicIterator<dim,dimworld> hend (int maxlevel);

  //***************************************************************
  //  Interface for Adaptation
  //***************************************************************
  
  //! marks an element for refCount refines. if refCount is negative the
  //! element is coarsend -refCount times
  //! mark returns true if element was marked, otherwise false
  bool mark( int refCount ); 
  
private: 
  // called from HierarchicIterator, because only this 
  // class changes the level of the entity, otherwise level is set by
  // Constructor 
  void setLevel ( int actLevel );  
  
  // face, edge and vertex only for codim > 0, in this
  // case just to supply the same interface
  void setTraverseStack (ALBERT TRAVERSE_STACK *travStack);
  void setElInfo (ALBERT EL_INFO *elInfo,
                  int elNum = 0, 
                  unsigned char face = 0,
                  unsigned char edge = 0,
                  unsigned char vertex = 0 );
  // needed for LevelIterator to compare 
  ALBERT EL_INFO *getElInfo () const;

  // return the global unique index in mesh 
  int globalIndex() { return elInfo_->el->index; }

  //! make a new AlbertGridEntity 
  void makeDescription();

  //! specialization of entity 
  AlbertGridLevelIterator<1,dim,dimworld>& faceEntity (int i); 
  AlbertGridLevelIterator<2,dim,dimworld>& edgeEntity (int i); 

  //! the corresponding grid 
  AlbertGrid<dim,dimworld> &grid_;

  //! for vertex access, to be revised, filled on demand
  AlbertGridLevelIterator<dim,dim,dimworld> vxEntity_;

  //! Because face and edge access is not often needed as vertex access,
  //! these objects are created on demand
  //! for the face access, __created__ on demand
  AlbertGridLevelIterator<1,dim,dimworld> * faceEntity_;

  //! only needed if dim == 3, __created__ on demand
  AlbertGridLevelIterator<dim-1,dim,dimworld> * edgeEntity_;

  //! the cuurent geometry
  AlbertGridElement<dim,dimworld> geo_;
  bool builtgeometry_;  //!< true if geometry has been constructed

  //! pointer to the real Albert element data
  ALBERT EL_INFO *elInfo_;

  //! the level of the entity
  int level_;
  
  //! pointer to the Albert TRAVERSE_STACK data
  ALBERT TRAVERSE_STACK * travStack_;

  AlbertGridElement <dim,dim> fatherReLocal_;
}; // end of AlbertGridEntity codim = 0

//**********************************************************************
//
// --AlbertGridHierarchicIterator
// --HierarchicIterator
/*!
  Mesh entities of codimension 0 ("elements") allow to visit all entities of
  codimension 0 obtained through nested, hierarchic refinement of the entity.
  Iteration over this set of entities is provided by the HIerarchicIterator,
  starting from a given entity.
  This is redundant but important for memory efficient implementations of unstru
  hierarchically refined meshes.
 */

template<int dim, int dimworld>
class AlbertGridHierarchicIterator :
public HierarchicIteratorDefault <dim,dimworld,albertCtype,
                          AlbertGridHierarchicIterator,AlbertGridEntity>
{
public:
  //! know your own dimension
  enum { dimension=dim };

  //! know your own dimension of world
  enum { dimensionworld=dimworld };
 
#if 1
  //! the normal Constructor
  AlbertGridHierarchicIterator(AlbertGrid<dim,dimworld> &grid,
    ALBERT TRAVERSE_STACK *travStack, int actLevel, int maxLevel);

  //! the default Constructor
  AlbertGridHierarchicIterator(AlbertGrid<dim,dimworld> &grid,
        int actLevel,int maxLevel);
#else 
  //! the normal Constructor
  AlbertGridHierarchicIterator(AlbertGrid<dim,dimworld> &grid,
    ALBERT TRAVERSE_STACK *travStack, int travLevel);

  //! the default Constructor
  AlbertGridHierarchicIterator(AlbertGrid<dim,dimworld> &grid);
#endif
  
  //! prefix increment
  AlbertGridHierarchicIterator& operator ++();

  //! postfix increment
  AlbertGridHierarchicIterator& operator ++(int i);

  //! equality
  bool operator== (const AlbertGridHierarchicIterator& i) const;

  //! inequality
  bool operator!= (const AlbertGridHierarchicIterator& i) const;

  //! dereferencing
  AlbertGridEntity<0,dim,dimworld>& operator*();

  //! arrow
  AlbertGridEntity<0,dim,dimworld>* operator->();

private:
  //! implement with virtual element
  AlbertGridEntity<0,dim,dimworld> virtualEntity_;

  //! know the grid were im comming from
  AlbertGrid<dim,dimworld> &grid_;

  //! the actual Level of this Hierarichic Iterator 
  int level_;

  //! max level to go down 
  int maxlevel_;

  //! we need this for Albert traversal, and we need ManageTravStack, which
  //! does count References when copied
  ALBERT ManageTravStack manageStack_;

  //! The nessesary things for Albert
  ALBERT EL_INFO * recursiveTraverse(ALBERT TRAVERSE_STACK * stack);

  //! make empty HierarchicIterator
  void makeIterator();
};


#define NEIGH_DEBUG
template<int dim, int dimworld>  
class AlbertGridBoundaryEntity 
: public BoundaryEntityDefault <dim,dimworld,albertCtype,
          AlbertGridElement,AlbertGridBoundaryEntity>
{ 
  friend class AlbertGridNeighborIterator<dim,dimworld>;
public:
  AlbertGridBoundaryEntity () : _geom (false) , _elInfo ( NULL ), 
  _neigh (-1) {};

  //! return type of boundary , i.e. Neumann, Dirichlet ... 
  BoundaryType type () 
  {
#ifdef NEIGH_DEBUG
    if(_elInfo->boundary[_neigh] == NULL) 
    {
      std::cerr << "No Boundary, fella! \n";
      abort();
    }
#endif
    return (( _elInfo->boundary[_neigh]->bound < 0 ) ? Neumann : Dirichlet ); 
  }

  //! return identifier of boundary segment, number 
  int id ()
  {
#ifdef NEIGH_DEBUG
    if(_elInfo->boundary[_neigh] == NULL) 
    {
      std::cerr << "No Boundary, fella! \n";
      abort();
    }
#endif
    return _elInfo->boundary[_neigh]->bound; 
  }

  //! return true if geometry of ghost cells was filled  
  bool hasGeometry () { return _geom.builtGeom(_elInfo,0,0,0); }

  //! return geometry of the ghost cell 
  AlbertGridElement<dim,dimworld> geometry () 
  {
    return _geom;
  }

private: 
  // set elInfo
  void setElInfo(ALBERT EL_INFO * elInfo, int nb)
  {
    _neigh = nb;
    if(elInfo)
      _elInfo = elInfo; 
    else 
      _elInfo = NULL;
  };

  int _neigh;
  
  // AlbertGrid<dim,dimworld> & _grid;
  AlbertGridElement<dim,dimworld> _geom;
 
  // cooresponding el_info 
  ALBERT EL_INFO * _elInfo;

};
//**********************************************************************
//
// --AlbertGridNeighborIterator
// --NeighborIterator
/*!
  Mesh entities of codimension 0 ("elements") allow to visit all neighbors, wh
  a neighbor is an entity of codimension 0 which has a common entity of codimens
  These neighbors are accessed via a NeighborIterator. This allows the implement
  non-matching meshes. The number of neigbors may be different from the number o
  of an element!
 */
template<int dim, int dimworld>
class AlbertGridNeighborIterator : 
public  NeighborIteratorDefault <dim,dimworld,albertCtype,
                         AlbertGridNeighborIterator,AlbertGridEntity,
                         AlbertGridElement, AlbertGridBoundaryEntity>
{
public:
  //! know your own dimension
  enum { dimension=dim };
  
  //! know your own dimension of world
  enum { dimensionworld=dimworld };
  
  //! prefix increment
  AlbertGridNeighborIterator& operator ++();

  //! postfix increment
  AlbertGridNeighborIterator& operator ++(int i);

  //! The default Constructor 
  AlbertGridNeighborIterator(AlbertGrid<dim,dimworld> &grid,int level);

  //! The Constructor 
  AlbertGridNeighborIterator(AlbertGrid<dim,dimworld> &grid,int level,
          ALBERT EL_INFO *elInfo);
  
  //! The Destructor 
  ~AlbertGridNeighborIterator();

  //! equality
  bool operator== (const AlbertGridNeighborIterator& i) const;

  //! inequality
  bool operator!= (const AlbertGridNeighborIterator& i) const;

  //! access neighbor, dereferencing 
  AlbertGridEntity<0,dim,dimworld>& operator*();

  //! access neighbor, arrow
  AlbertGridEntity<0,dim,dimworld>* operator->();

  //! return true if intersection is with boundary. \todo connection with 
  //! boundary information, processor/outer boundary
  bool boundary ();

  //! return true if across the edge an neighbor on this level exists
  bool neighbor (); 

  //! return information about the Boundary 
  AlbertGridBoundaryEntity<dim,dimworld> & boundaryEntity (); 
      
  //! return unit outer normal, this should be dependent on local 
  //! coordinates for higher order boundary 
  Vec<dimworld,albertCtype>& unit_outer_normal (Vec<dim-1,albertCtype>& local);

  //! return unit outer normal, if you know it is constant use this function instead
  Vec<dimworld,albertCtype>& unit_outer_normal ();
 
  //! intersection of codimension 1 of this neighbor with element where
  //! iteration started. 
  //! Here returned element is in LOCAL coordinates of the element
  //! where iteration started.
  AlbertGridElement<dim-1,dim>& intersection_self_local ();

  //! intersection of codimension 1 of this neighbor with element where iteration started. 
  //! Here returned element is in GLOBAL coordinates of the element where iteration started.
  AlbertGridElement<dim-1,dimworld>& intersection_self_global ();

  //! local number of codim 1 entity in self where intersection is contained in 
  int number_in_self ();

  //! intersection of codimension 1 of this neighbor with element where iteration started. 
  //! Here returned element is in LOCAL coordinates of neighbor
  AlbertGridElement<dim-1,dim>& intersection_neighbor_local ();

  //! intersection of codimension 1 of this neighbor with element where iteration started. 
  //! Here returned element is in LOCAL coordinates of neighbor
  AlbertGridElement<dim-1,dimworld>& intersection_neighbor_global ();

  //! local number of codim 1 entity in neighbor where intersection is contained
  int number_in_neighbor ();
  
  //! return outer normal, this should be dependent on local 
  //! coordinates for higher order boundary 
  Vec<dimworld,albertCtype>& outer_normal (Vec<dim-1,albertCtype>& local);

  //! return unit outer normal, if you know it is constant use this function instead
  Vec<dimworld,albertCtype>& outer_normal ();

private:
  //**********************************************************
  //  private methods 
  //**********************************************************

  // makes empty neighElInfo
  void initElInfo(ALBERT EL_INFO * elInfo); 
  // calc the Neighbor neigh out of elInfo information
  void setNeighInfo(ALBERT EL_INFO * elInfo, ALBERT EL_INFO * neighInfo, int neigh);

  //! know the grid were im comming from
  AlbertGrid<dim,dimworld> &grid_;

  //! the actual level
  int level_;
  
  //! implement with virtual element
  //! Most of the information can be generated from the ALBERT EL_INFO 
  //! therefore this element is only created on demand. 
  bool builtNeigh_;
  AlbertGridEntity<0,dim,dimworld> *virtualEntity_;
 
  // for memory management 
  AlbertGrid<dim,dimworld>::EntityProvider::ObjectEntity *manageObj_;
  AlbertGrid<dim,dimworld>::IntersectionSelfProvider::ObjectEntity  *manageInterEl_;
  AlbertGrid<dim,dimworld>::IntersectionNeighProvider::ObjectEntity *manageNeighEl_;

  //! vector storing the outer normal 
  //Vec<dimworld,albertCtype> outerNormal_;

  //! pointer to element holding the self_local and self_global information.
  //! This element is created on demand.
  AlbertGridElement<dim-1,dim> *fakeNeigh_;
 
  //! pointer to element holding the neighbor_global and neighbor_local 
  //! information. This element is created on demand.
  AlbertGridElement<dim-1,dimworld> *neighGlob_;

  //! BoundaryEntity
  AlbertGridBoundaryEntity<dim,dimworld> _boundaryEntity;
  
  //! pointer to the EL_INFO struct storing the real element information
  ALBERT EL_INFO * elInfo_;

  //! EL_INFO th store the information of the neighbor if needed
  ALBERT EL_INFO neighElInfo_;
  ALBERT EL boundEl_;
 
  //! count on which neighbor we are lookin' at
  int neighborCount_;
};



//**********************************************************************
//
// --AlbertGridLevelIterator
// --LevelIterator
/*!
 Enables iteration over all entities of a given codimension and level of a grid.
 */
template<int codim, int dim, int dimworld>
class AlbertGridLevelIterator : 
public LevelIteratorDefault <codim,dim,dimworld,albertCtype,
          AlbertGridLevelIterator,AlbertGridEntity>
{
  friend class AlbertGridEntity<2,dim,dimworld>;
  friend class AlbertGridEntity<1,dim,dimworld>;
  friend class AlbertGridEntity<0,dim,dimworld>;
  friend class AlbertGrid < dim , dimworld >;
public:

  //friend class AlbertGrid<dim,dimworld>;
  //! know your own codimension
  enum { codimension=dim };

  //! know your own dimension
  enum { dimension=dim };

  //! know your own dimension of world
  enum { dimensionworld=dimworld };

  //! Constructor
  AlbertGridLevelIterator(AlbertGrid<dim,dimworld> &grid, int travLevel);
  
  //! Constructor
  AlbertGridLevelIterator(AlbertGrid<dim,dimworld> &grid,
        ALBERT EL_INFO *elInfo,int elNum = 0 , int face=0, int edge=0,int vertex=0);
  
  //! Constructor
  AlbertGridLevelIterator(AlbertGrid<dim,dimworld> &grid, 
          AlbertMarkerVector * vec ,int travLevel);
  
  //! prefix increment
  AlbertGridLevelIterator<codim,dim,dimworld>& operator ++();

  //! postfix increment
  AlbertGridLevelIterator<codim,dim,dimworld>& operator ++(int i);

  //! equality
  bool operator== (const AlbertGridLevelIterator<codim,dim,dimworld>& i) const;

  //! inequality
  bool operator!= (const AlbertGridLevelIterator<codim,dim,dimworld>& i) const;

  //! dereferencing
  AlbertGridEntity<codim,dim,dimworld>& operator*() ;

  //! arrow
  AlbertGridEntity<codim,dim,dimworld>* operator->() ;
  
  //! ask for level of entity
  int level ();

private:
  // private Methods
  void makeIterator();


  ALBERT EL_INFO * goFirstElement(ALBERT TRAVERSE_STACK *stack,
                            ALBERT MESH *mesh,
                           int level, ALBERT FLAGS fill_flag);
  ALBERT EL_INFO * traverseLeafElLevel(ALBERT TRAVERSE_STACK * stack);
 
  // the default is, go to next elInfo
  template <int cc>  ALBERT EL_INFO * 
  goNextEntity(ALBERT TRAVERSE_STACK *stack,ALBERT EL_INFO *elinfo_old)
  {
    return goNextElInfo(stack,elinfo_old); 
  };

  template <>  ALBERT EL_INFO * 
  goNextEntity<1 << dim>(ALBERT TRAVERSE_STACK *stack,ALBERT EL_INFO *elinfo_old)
  {
     return goNextFace(stack,elinfo_old);
  };
  
  template <>  ALBERT EL_INFO * 
  goNextEntity<2 << dim>(ALBERT TRAVERSE_STACK *stack,ALBERT EL_INFO *elinfo_old)
  {
     return goNextEdge(stack,elinfo_old);
  };
  
  template <>  ALBERT EL_INFO * 
  goNextEntity<dim>(ALBERT TRAVERSE_STACK *stack,ALBERT EL_INFO *elinfo_old)
  {
     return goNextVertex(stack,elinfo_old);
  };
  
  // the real go next methods
  ALBERT EL_INFO * goNextElInfo(ALBERT TRAVERSE_STACK *stack,ALBERT EL_INFO *elinfo_old);
  ALBERT EL_INFO * goNextFace(ALBERT TRAVERSE_STACK *stack,
                              ALBERT EL_INFO *elInfo);
  ALBERT EL_INFO * goNextEdge(ALBERT TRAVERSE_STACK *stack,
                              ALBERT EL_INFO *elInfo);
  ALBERT EL_INFO * goNextVertex(ALBERT TRAVERSE_STACK *stack,
                              ALBERT EL_INFO *elInfo);


  //! the grid were it all comes from
  AlbertGrid<dim,dimworld> &grid_; 

  // private Members
  AlbertGridEntity<codim,dim,dimworld> virtualEntity_;

  // contains ALBERT traversal stack 
  ALBERT ManageTravStack manageStack_;

  //! element number 
  int elNum_; 

  //! level 
  int level_;
    
  //! which face, edge and vertex are we watching of an elInfo
  unsigned char face_;
  unsigned char edge_;
  unsigned char vertex_;

  // knows on which element a point is viewed
  AlbertMarkerVector * vertexMarker_;
  
};


  
//**********************************************************************
//
// --AlbertGrid
//
//**********************************************************************
template <int dim, int dimworld>
class AlbertGrid : public GridDefault  < dim, dimworld,
                          albertCtype,AlbertGrid,
                          AlbertGridLevelIterator,AlbertGridEntity>
{
  friend class AlbertGridEntity <0,dim,dimworld>;
  //friend class AlbertGridEntity <1,dim,dimworld>;
  //friend class AlbertGridEntity <1 << dim-1 ,dim,dimworld>;
  friend class AlbertGridEntity <dim,dim,dimworld>;

  friend class AlbertMarkerVector;

  // friends because of fillElInfo
  friend class AlbertGridLevelIterator<0,dim,dimworld>;
  friend class AlbertGridLevelIterator<1,dim,dimworld>;
  friend class AlbertGridLevelIterator<2,dim,dimworld>;
  friend class AlbertGridLevelIterator<3,dim,dimworld>;
  friend class AlbertGridHierarchicIterator<dim,dimworld>;

  friend class AlbertGridNeighborIterator<dim,dimworld>;

  //! AlbertGrid is only implemented for 2 and 3 dimension
  //! for 1d use SGrid or SimpleGrid 
  CompileTimeChecker<dimworld != 1>   Do_not_use_AlbertGrid_for_1d_Grids;   

  //! At this time AlbertGrid only supports the dim == dimworld case
  CompileTimeChecker<dimworld == dim> Only_dim_equal_dimworld_is_implemented;   
  
//**********************************************************
// The Interface Methods
//**********************************************************
public: 
  typedef AlbertGridReferenceElement<dim> ReferenceElement;
  
  //! know your own dimension
  enum { dimension=dim };

  //! know your own dimension of world
  enum { dimensionworld=dimworld }; 

  enum { numCodim = dim+1 };

  //! Constructor which reads an Albert Macro Triang file 
  //! or given GridFile 
  AlbertGrid(const char* macroTriangFilename);
  
  //! empty Constructor 
  AlbertGrid();
 
  //! Desctructor 
  ~AlbertGrid();
   
  //! Return maximum level defined in this grid. Levels are numbered
  //! 0 ... maxlevel with 0 the coarsest level.  
  int maxlevel() const;

  //! Iterator to first entity of given codim on level
  template<int codim>
  AlbertGridLevelIterator<codim,dim,dimworld> lbegin (int level);

  //! one past the end on this level
  template<int codim>
  AlbertGridLevelIterator<codim,dim,dimworld> lend (int level);

  //! number of grid entities per level and codim
  int size (int level, int codim) ;
  int size (int level, int codim) const; 

  //! refine all positive marked leaf entities 
  //! return true if the grid was changed
  bool refine  ( );
  
  //! coarsen all negative marked leaf entities 
  //! return true if the grid was changed
  bool coarsen ( );

  GridIdentifier type () { return AlbertGrid_Id; };
  
//**********************************************************
// End of Interface Methods
//**********************************************************

  //! uses the interface, mark on entity and refineLocal 
  bool globalRefine(int refCount);

  // write Grid to file
  //void writeGrid(int level=-1);

  // write Grid to file in specified FileFormatType 
  template <FileFormatType ftype>
  bool writeGrid( const char * filename, albertCtype time );
  
  // read Grid from file filename and store time of mesh in time 
  template <FileFormatType ftype>
  bool readGrid( const char * filename, albertCtype & time );

  albertCtype getTime () const { return time_; };
  
private:
  // make the calculation of indexOnLevel and so on.
  // extra method because of Reihenfolge
  void calcExtras(); 
 
  // read and write mesh_ via ALBERT routines 
  bool writeGridXdr  ( const char * filename, albertCtype time );
  bool writeGridUSPM ( const char * filename, albertCtype time, int level );
  bool readGridXdr   ( const char * filename, albertCtype & time );
    
  //! access to mesh pointer, needed by some methods
  ALBERT MESH* getMesh () const { return mesh_; }; 

  // pointer to an Albert Mesh, which contains the data
  ALBERT MESH *mesh_;

  // number of maxlevel of the mesh
  int maxlevel_;

  // true if grid was refined
  bool wasChanged_; 
  
  // number of entitys of each level an codim 
  Array<int> size_;

  // remember on which level an ALBERT EL lives, is needed for the new
  // fillElInfo method that takes the level of an element into account
  // for calculation of the neighbor realtions 
  std::vector<int> neighOnLevel_;

  // this method is new fill_elinfo from ALBERT but here the neighbor
  // relations are calced diffrent, on ervery level there are neighbor
  // realtions ( in ALBERT only on leaf level ), so we needed a new
  // fill_elinfo. 
  void fillElInfo(int ichild, int actLevel ,const ALBERT EL_INFO *elinfo_old, 
        ALBERT EL_INFO *elinfo, bool hierachical ) const;
  
  // needed for VertexIterator, mark on which element a vertex is treated 
  AlbertMarkerVector * vertexMarker_; 

  //*********************************************************
  // Methods for mapping the global Index to local on Level
  // contains the index on level for each unique el->index of Albert
  Array<int> levelIndex_[dim+1];
  void makeNewSize(Array<int> &a, int newNumberOfEntries);
  void markNew();
  //**********************************************************
    
  //! map the global index from the Albert Mesh to the local index on Level
  template <int codim>
  int indexOnLevel(int globalIndex, int level ) ;

  //! map the global index from the Albert Mesh to the local index on Level
  template <>
  int indexOnLevel<dim>(int globalIndex, int level )  
  {
    return levelIndex_[dim][level*mesh_->n_vertices + globalIndex]; 
  };

  //! actual time of Grid
  albertCtype time_;

//***********************************************************************
//  MemoryManagement for Entitys and Elements 
//**********************************************************************

  // organize the memory management for entitys used by the NeighborIterator
  template <class Object> 
  class MemoryProvider 
  {
  public:
    typedef Object ObjectType;
    
    struct ObjectEntity
    {
      ObjectEntity *next;
      Object       *item;
    };

    // freeEntity_ = NULL
    MemoryProvider() : freeEntity_ (NULL) {} 

    // call deleteEntity 
    ~MemoryProvider ();

    // delete recursive all free ObjectEntitys 
    void deleteEntity(ObjectEntity *obj);

    // i.e. return pointer to Entity
    ObjectEntity *getNewObjectEntity(AlbertGrid<dim,dimworld> &grid, int level);

    // i.e. get pointer to element 
    ObjectEntity *getNewObjectElement();

    // free, move element to stack 
    void freeObjectEntity (ObjectEntity *obj);

    private:    
    ObjectEntity  *freeEntity_;
  };

  typedef MemoryProvider< AlbertGridEntity<0,dim,dimworld > > EntityProvider;
  typedef MemoryProvider< AlbertGridElement<dim-1,dimworld> > IntersectionSelfProvider; 
  typedef MemoryProvider< AlbertGridElement<dim-1,dim> >      IntersectionNeighProvider;

  EntityProvider            entityProvider_;
  IntersectionSelfProvider  interSelfProvider_;
  IntersectionNeighProvider interNeighProvider_;

}; // end Class AlbertGridGrid



// Class to mark the Vertices on the leaf level
// to visit every vertex only once
// for the LevelIterator codim == dim
class AlbertMarkerVector 
{
  friend class AlbertGrid<2,2>;
  friend class AlbertGrid<3,3>;
public:
  AlbertMarkerVector () {} ;

  bool notOnThisElement(ALBERT EL * el, int level , int vertex);

  template <class Grid>
  void markNewVertices(Grid &grid);
  void print();

private:
  // built in array to mark on which element a vertex is reached
  Array<int> vec_;
  // number of vertices 
  int numVertex_;
};

/** @} end documentation group */


}; // namespace Dune

#include "albertgrid/agmemory.hh"
#include "albertgrid/albertgrid.cc"

#endif
