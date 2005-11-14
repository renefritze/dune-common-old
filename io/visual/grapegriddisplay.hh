#ifndef DUNE_GRAPEGRIDDISPLAY_HH
#define DUNE_GRAPEGRIDDISPLAY_HH

#include "grape/grapeinclude.hh"

namespace Dune 
{
  
template<class GridType>
class GrapeGridDisplay 
{
  typedef GrapeGridDisplay < GridType > MyDisplayType;
  enum { dim = GridType::dimension };
  enum { dimworld = GridType::dimensionworld };

  // defined in griddisplay.hh 
  typedef typename GrapeInterface<dim,dimworld>::DUNE_ELEM DUNE_ELEM;
  
public:
  typedef GridType MyGridType;
  
  typedef typename GridType::Traits::template Codim<0>::LevelIterator LevelIteratorType ;
  typedef typename GridType::Traits::template Codim<0>::LeafIterator LeafIteratorType ;
  typedef typename GridType::Traits::LocalIdSet LocalIdSetType; 
  typedef typename GridType::Traits::LeafIndexSet LeafIndexSetType; 
    
  //typedef typename GridType::Traits::template Codim<0>::template Partition<Interior_Partition>:: LeafIterator LeafIteratorType;
protected:
  //! the grid we want to display 
  const GridType &grid_;

  //! leaf index set of the grid 
  const LeafIndexSetType & leafset_; 

  //! leaf index set of the grid 
  const LocalIdSetType & lid_; 

  //! my process number 
  const int myRank_;

  //! my interal level iterators 
  LevelIteratorType *myIt_;
  LevelIteratorType *myEndIt_;
 
  //! my internal leaf iterators 
  LeafIteratorType *myLeafIt_;
  LeafIteratorType *myLeafEndIt_;

  //! store the actual element pointer 
  DUNE_ELEM hel_;
  
  // no better way than this canot export HMESH structure to here
  //! pointer to hmesh 
  void *hmesh_;

public:
  //! Constructor, make a GrapeGridDisplay for given grid
  inline GrapeGridDisplay(const GridType &grid, const int myrank );

  //! Constructor, make a GrapeGridDisplay for given grid
  inline GrapeGridDisplay(const GridType &grid);

  //! Destructor for GrapeGridDisplay
  inline ~GrapeGridDisplay();

  //! Calls the display of the grid and draws the discrete function
  //! if discretefunction is NULL, then only the grid is displayed 
  inline void display();

  //! return rank of this display, for visualisation of parallel grid 
  int myRank () const { return myRank_; }

  //! return pointer to Grape Hmesh
  inline void * getHmesh();

  inline void addMyMeshToTimeScene(void * timescene, double time, int proc);
 
  //! return reference to Dune Grid
  inline const GridType& getGrid() const ;

  bool hasData () { return false; }

  // internal vec for local to global methods 
  FieldVector<double,dimworld> globalVec_;
  FieldVector<double,dim> localVec_;
 
private:  
  // generate hmesh 
  inline void * setupHmesh();

public:
//****************************************************************  
//
// --GrapeGridDisplay, Some Subroutines needed for display with GRAPE
// 
//****************************************************************  
  // update element from entity
  template <class GridIteratorType>
  inline int el_update (GridIteratorType *, DUNE_ELEM *) ;

  // update child element 
  template <class GridIteratorType>
  inline int child_update (GridIteratorType *, DUNE_ELEM *) ;

  template <class GridIteratorType>
  inline int child_n_update (GridIteratorType *, DUNE_ELEM *) ;

  // first and next macro element via LevelIterator level 0 
  inline int first_leaf (DUNE_ELEM * he) ;
  inline int next_leaf (DUNE_ELEM * he) ;
  
  // first and next macro element via LevelIterator level 0 
  inline int first_macro (DUNE_ELEM * he) ;
  inline int next_macro (DUNE_ELEM * he) ;

  // first and next child via HierarchicIterator with given maxlevel in Grape
  inline int first_child (DUNE_ELEM * he) ;
  inline int next_child (DUNE_ELEM * he) ;

  // fake function for copy iterator 
  inline static void * copy_iterator (const void * i) ;

  // local to world 
  inline void local2world (DUNE_ELEM * he, const double * c, double * w);

  // world to local 
  inline int world2local (DUNE_ELEM * he, const double * w, double * c);
 
  // check inside reference element 
  inline int checkWhetherInside (DUNE_ELEM * he, const double * w);

  //*********************************
  //  wrapper functions 
  //*********************************
  // local to world 
  inline static void ctow (DUNE_ELEM * he, const double * c, double * w);

  // world to local 
  inline static int wtoc (DUNE_ELEM * he, const double * w, double * c);
 
  // check inside reference element 
  inline static int check_inside (DUNE_ELEM * he, const double * w);

  // dito 
  template <class EntityType>
  inline int  checkInside(EntityType &en, const double * w);  
 
  // dito 
  template <class EntityType>
  inline int  world_to_local(EntityType &en, const double * w, double * c);  
  
  // dito
  template <class EntityType>
  inline void local_to_world(EntityType &en, const double * c, double * w);  
  
  // wrapper methods for first_child and next_child
  inline static int first_mac (DUNE_ELEM * he);
  inline static int next_mac (DUNE_ELEM * he);
  
  // wrapper methods for first_child and next_child
  inline static int fst_leaf (DUNE_ELEM * he);
  inline static int nxt_leaf (DUNE_ELEM * he);
  
  // wrapper methods for first_child and next_child
  inline static int fst_child (DUNE_ELEM * he);
  inline static int nxt_child (DUNE_ELEM * he);
  
}; // end class GrapeGridDisplay


/**************************************************************************/
/*  element types, see dune/grid/common/grid.hh */
enum GRAPE_ElementType
    {g_vertex=0,  g_line=1,  g_triangle=2,  g_quadrilateral=3,g_tetrahedron=4,
     g_pyramid=5, g_prism=6, g_hexahedron=7,g_iso_triangle=8, g_iso_quadrilateral=9,
     g_unknown=127};


//! convert new geometry types to old ones 
//! hack at this monment 
static inline GeometryType  geomTypeConvert ( GeometryType type , int dim )
{
  GeometryType t = type;
  if(t == simplex)
  {
    if(dim == 1) t = line;
    if(dim == 2) t = triangle;
    if(dim == 3) t = tetrahedron;
  }
  if(t == cube)
  {
    if(dim == 1) t = line;
    if(dim == 2) t = quadrilateral;
    if(dim == 3) t = hexahedron;
  }
  return t;
}

//! convert new geometry types to old ones 
//! hack at this monment 
static inline GRAPE_ElementType convertToGrapeType ( GeometryType type , int dim )
{
  GeometryType t = geomTypeConvert(type,dim);
  switch(t)
  {
    case vertex:        return g_vertex;
    case line:          return g_line;
    case triangle:      return g_triangle;
    case quadrilateral: return g_quadrilateral;
    case tetrahedron:   return g_tetrahedron;
    case pyramid:       return g_pyramid;
    case prism:         return g_prism;
    case hexahedron:    return g_hexahedron;
  }
  return g_unknown;
}


} // end namespace Dune


#include "grape/grapegriddisplay.cc"


#endif
