#ifndef DUNE_UGGRIDLEVELITERATOR_HH
#define DUNE_UGGRIDLEVELITERATOR_HH



//**********************************************************************
//
// --UGGridLevelIterator
// --LevelIterator
/*!
 Enables iteration over all entities of a given codimension and level of a grid.
 */
template<int codim, int dim, int dimworld>
class UGGridLevelIterator : 
    public LevelIteratorDefault <codim,dim,dimworld, UGCtype,
                                 UGGridLevelIterator,UGGridEntity>
{
  friend class UGGridEntity<2,dim,dimworld>;
  friend class UGGridEntity<1,dim,dimworld>;
  friend class UGGridEntity<0,dim,dimworld>;
  friend class UGGrid < dim , dimworld >;
public:

  //friend class UGGrid<dim,dimworld>;

  //! Constructor
  UGGridLevelIterator(UGGrid<dim,dimworld> &grid, int travLevel);

#if 0  
  //! Constructor
  UGGridLevelIterator(UGGrid<dim,dimworld> &grid,
        ALBERT EL_INFO *elInfo,int elNum = 0 , int face=0, int edge=0,int vertex=0);
#endif

  //! Constructor
  UGGridLevelIterator(UGGrid<dim,dimworld> &grid, 
          UGMarkerVector * vec ,int travLevel);
  
  //! prefix increment
  UGGridLevelIterator<codim,dim,dimworld>& operator ++();

  //! postfix increment
  UGGridLevelIterator<codim,dim,dimworld>& operator ++(int i);

  //! equality
  bool operator== (const UGGridLevelIterator<codim,dim,dimworld>& i) const;

  //! inequality
  bool operator!= (const UGGridLevelIterator<codim,dim,dimworld>& i) const;

  //! dereferencing
  UGGridEntity<codim,dim,dimworld>& operator*() ;

  //! arrow
  UGGridEntity<codim,dim,dimworld>* operator->() ;
  
  //! ask for level of entity
  int level ();

private:
  // private Methods
  void makeIterator();

#if 0
  ALBERT EL_INFO * goFirstElement(ALBERT TRAVERSE_STACK *stack,
                            ALBERT MESH *mesh,
                           int level, ALBERT FLAGS fill_flag);
  ALBERT EL_INFO * traverseLeafElLevel(ALBERT TRAVERSE_STACK * stack);
 
  // the default is, go to next elInfo
  //template <int cc>  
  ALBERT EL_INFO * goNextEntity(ALBERT TRAVERSE_STACK *stack,ALBERT EL_INFO *elinfo_old);
  
  // the real go next methods
  ALBERT EL_INFO * goNextElInfo(ALBERT TRAVERSE_STACK *stack,ALBERT EL_INFO *elinfo_old);
  ALBERT EL_INFO * goNextFace(ALBERT TRAVERSE_STACK *stack,
                              ALBERT EL_INFO *elInfo);
  ALBERT EL_INFO * goNextEdge(ALBERT TRAVERSE_STACK *stack,
                              ALBERT EL_INFO *elInfo);
  ALBERT EL_INFO * goNextVertex(ALBERT TRAVERSE_STACK *stack,
                              ALBERT EL_INFO *elInfo);
#endif

  //! the grid were it all comes from
  UGGrid<dim,dimworld> &grid_; 

  // private Members
  UGGridEntity<codim,dim,dimworld> virtualEntity_;

#if 0
  // contains ALBERT traversal stack 
  ALBERT ManageTravStack manageStack_;
#endif

  //! element number 
  int elNum_; 

  //! level 
  int level_;
    
  //! which face, edge and vertex are we watching of an elInfo
  unsigned char face_;
  unsigned char edge_;
  unsigned char vertex_;

  // knows on which element a point is viewed
  UGMarkerVector * vertexMarker_;
  

    void* target;
};
  
#endif
