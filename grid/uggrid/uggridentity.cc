//*************************************************************************
//
//  --AlbertGridEntity 
//  --Entity 
//
//*************************************************************************


//
//  codim > 0
//
// The Element is prescribed by the EL_INFO struct of ALBERT MESH
// the pointer to this struct is set and get by setElInfo and
// getElInfo. 
//*********************************************************************8

#if 0
template<int codim, int dim, int dimworld>
inline void AlbertGridEntity < codim, dim ,dimworld >::
makeDescription()
{
  elInfo_ = NULL;
  builtgeometry_ = false;

  // to slow and not needed
  //geo_.initGeom();
}


template<int codim, int dim, int dimworld>
inline AlbertGridEntity < codim, dim ,dimworld >::
AlbertGridEntity(AlbertGrid<dim,dimworld> &grid, int level, 
      ALBERT TRAVERSE_STACK * travStack) : grid_(grid) 
  , level_ ( level )
  , geo_ (false)
{
  travStack_ = travStack;
  makeDescription();
}


template<int codim, int dim, int dimworld>
inline void AlbertGridEntity < codim, dim ,dimworld >::
setTraverseStack(ALBERT TRAVERSE_STACK * travStack)
{
  travStack_ = travStack;
}
#endif

template<int codim, int dim, int dimworld>
inline UGGridEntity < codim, dim ,dimworld >::
UGGridEntity(UGGrid<dim,dimworld> &grid, int level) : 
  grid_(grid)
, level_ (level)
, geo_ ( false )
{
#if 0
  travStack_ = NULL;
  makeDescription();
#endif
}

#if 0
template<int codim, int dim, int dimworld>
inline ALBERT EL_INFO* AlbertGridEntity < codim, dim ,dimworld >::
getElInfo() const
{
  return elInfo_;
}

template<int codim, int dim, int dimworld>
inline void AlbertGridEntity < codim, dim ,dimworld >::
setElInfo(ALBERT EL_INFO * elInfo, int elNum, unsigned char face,
          unsigned char edge, unsigned char vertex )
{
  elNum_ = elNum;
  face_ = face;
  edge_ = edge;
  vertex_ = vertex;
  elInfo_ = elInfo;
  builtgeometry_ = geo_.builtGeom(elInfo_,face,edge,vertex);
}
#endif

template<int codim, int dim, int dimworld>
inline int UGGridEntity < codim, dim ,dimworld >::
level()
{
  return level_;
}

#if 0
template<int codim, int dim, int dimworld>
inline int AlbertGridEntity < codim, dim ,dimworld >::
index()
{
  return elNum_;
}

template< int codim, int dim, int dimworld>
inline AlbertGridElement<dim-codim,dimworld>& 
AlbertGridEntity < codim, dim ,dimworld >::geometry()
{
  return geo_;
}

template<int codim, int dim, int dimworld>
inline Vec<dim,albertCtype>& 
AlbertGridEntity < codim, dim ,dimworld >::local()
{
  return localFatherCoords_;
}

template<int codim, int dim, int dimworld>
inline AlbertGridLevelIterator<0,dim,dimworld> 
AlbertGridEntity < codim, dim ,dimworld >::father()
{
  std::cout << "father not correctly implemented! \n";
  ALBERT TRAVERSE_STACK travStack;
  initTraverseStack(&travStack);

  travStack = (*travStack_);

  travStack.stack_used--;

  AlbertGridLevelIterator <0,dim,dimworld>
    it(grid_,travStack.elinfo_stack+travStack.stack_used,0,0,0,0);
  return it;
}


//************************************
//
//  --AlbertGridEntity codim = 0 
//  --0Entity codim = 0 
//
//************************************
template< int dim, int dimworld>
inline bool AlbertGridEntity < 0, dim ,dimworld >::
mark( int refCount )
{
  if(! hasChildren() )
  {
    elInfo_->el->mark = refCount;
    return true;
  }
  
  std::cout << "Element not marked!\n";
  return false;
}

template< int dim, int dimworld>
inline bool AlbertGridEntity < 0, dim ,dimworld >::hasChildren()
{
  //return (elInfo_->el->child[0] != NULL); 
  return ( level_ != grid_.maxlevel() );
}
  
template< int dim, int dimworld>
inline int AlbertGridEntity < 0, dim ,dimworld >::refDistance()
{
  return 0;
}

//***************************

template< int dim, int dimworld>
inline void AlbertGridEntity < 0, dim ,dimworld >::
makeDescription()
{
  elInfo_ = NULL;
  builtgeometry_ = false;

  // not fast , and also not needed
  //geo_.initGeom();
}

template<int dim, int dimworld>
inline void AlbertGridEntity < 0, dim ,dimworld >::
setTraverseStack(ALBERT TRAVERSE_STACK * travStack)
{
  travStack_ = travStack;
}

//#endif
template< int dim, int dimworld>
inline UGGridEntity < 0, dim ,dimworld >::
UGGridEntity(UGGrid<dim,dimworld> &grid, int level) : grid_(grid) 
  , level_ (level)
  , vxEntity_ ( grid_ , NULL, 0, 0, 0, 0) 
  , geo_(false) , travStack_ (NULL) , elInfo_ (NULL) 
  , builtgeometry_ (false)
{
}
//#if 0
//*****************************************************************8
// count
template <int dim, int dimworld> template <int cc> 
inline int AlbertGridEntity<0,dim,dimworld>::count ()
{
  return (dim+1);
}
//! specialization only for codim == 2 , edges, 
//! a tetrahedron has always 6 edges 
template <> template <>
inline int AlbertGridEntity<0,3,3>::count<2> ()
{
  return 6;
}

// subIndex 
template <int dim, int dimworld> template <int cc>
inline int AlbertGridEntity<0,dim,dimworld>::subIndex ( int i )
{
  return entity<cc>(i)->index();
}

// subIndex 
template <> template <>
inline int AlbertGridEntity<0,2,2>::subIndex<2> ( int i )
{
  return grid_.indexOnLevel<2>(elInfo_->el->dof[i][0],level_);
}

// subIndex 
template <> template <>
inline int AlbertGridEntity<0,2,3>::subIndex<2> ( int i )
{
  return grid_.indexOnLevel<2>(elInfo_->el->dof[i][0],level_);
}

// subIndex 
template <> template <>
inline int AlbertGridEntity<0,3,3>::subIndex<3> ( int i )
{
  return grid_.indexOnLevel<3>(elInfo_->el->dof[i][0],level_);
}

// default is faces 
template <int dim, int dimworld> template <int cc>
inline AlbertGridLevelIterator<cc,dim,dimworld> 
AlbertGridEntity<0,dim,dimworld>::entity ( int i )
{
  AlbertGridLevelIterator<cc,dim,dimworld> tmp (grid_,elInfo_,
      grid_. template indexOnLevel<cc>( globalIndex() ,level_),i,0,0);
  return tmp;
}

template <> template <>
inline AlbertGridLevelIterator<2,3,3> 
AlbertGridEntity<0,3,3>::entity<2> ( int i )
{
  //enum { cc = 2 };
  int num = grid_.indexOnLevel<2>(globalIndex() ,level_);
  if(i < 3)
  { // 0,1,2 
    AlbertGridLevelIterator<2,3,3> tmp (grid_,elInfo_,num, 0,i,0);
    return tmp;
  }  
  else
  { // 3,4,5
    AlbertGridLevelIterator<2,3,3> tmp (grid_,elInfo_,num, i-2,1,0);
    return tmp;
  }
}

// specialization for vertices 
template <> template <>
inline AlbertGridLevelIterator<2,2,2> 
AlbertGridEntity<0,2,2>::entity<2> ( int i )
{
  std::cout << "entity<2> ,2,2 !\n";
  // we are looking at vertices 
  //enum { cc = dimension };
  enum { cc = 2 };
  AlbertGridLevelIterator<cc,2,2> 
    tmp (grid_,elInfo_, grid_.indexOnLevel<cc>( elInfo_->el->dof[i][0],level_), 
         0,0,i);
  return tmp; 
}
// specialization for vertices 
template <> template <>
inline AlbertGridLevelIterator<2,2,3> 
AlbertGridEntity<0,2,3>::entity<2> ( int i )
{
  // we are looking at vertices 
  //enum { cc = dimension };
  enum { cc = 2 };
  AlbertGridLevelIterator<cc,2,3> 
    tmp (grid_,elInfo_, grid_.indexOnLevel<cc>( elInfo_->el->dof[i][0],level_), 
         0,0,i);
  return tmp; 
}
// specialization for vertices 
template <> template <>
inline AlbertGridLevelIterator<3,3,3> 
AlbertGridEntity<0,3,3>::entity<3> ( int i )
{
  // we are looking at vertices 
  enum { cc = 3 };
  //enum { cc = dimension };
  AlbertGridLevelIterator<cc,3,3> 
    tmp (grid_,elInfo_, grid_.indexOnLevel<cc>( elInfo_->el->dof[i][0],level_), 
         0,0,i);
  return tmp; 
}
//***************************

template<int dim, int dimworld>
inline ALBERT EL_INFO* AlbertGridEntity < 0 , dim ,dimworld >::
getElInfo() const
{
  return elInfo_;
}

template<int dim, int dimworld>
inline int AlbertGridEntity < 0, dim ,dimworld >::
level()
{
  return level_;
  //return elInfo_->level;
}

template<int dim, int dimworld>
inline int AlbertGridEntity < 0, dim ,dimworld >::
index()
{
  return grid_.template indexOnLevel<0>( globalIndex() , level_ );
}

template< int dim, int dimworld>
inline void AlbertGridEntity < 0, dim ,dimworld >::
setLevel(int actLevel)
{
  level_ = actLevel;
}

template< int dim, int dimworld>
inline void AlbertGridEntity < 0, dim ,dimworld >::
setElInfo(ALBERT EL_INFO * elInfo, int elNum,  unsigned char face,
          unsigned char edge, unsigned char vertex )
{
  // in this case the face, edge and vertex information is not used,
  // because we are in the element case
  elInfo_ = elInfo;
  builtgeometry_ = geo_.builtGeom(elInfo_,face,edge,vertex);
}

template< int dim, int dimworld>
inline AlbertGridElement<dim,dimworld>& 
AlbertGridEntity < 0, dim ,dimworld >::geometry()
{
  assert(builtgeometry_ == true);
  return geo_;
}


template< int dim, int dimworld>
inline AlbertGridLevelIterator<0,dim,dimworld> 
AlbertGridEntity < 0, dim ,dimworld >::father()
{
  ALBERT EL_INFO * fatherInfo = NULL;
  int fatherLevel = level_-1;
  // if this level > 0 return father = elInfoStack -1, 
  // else return father = this 
  if(level_ > 0)
    fatherInfo = &travStack_->elinfo_stack[travStack_->stack_used-1];
  else
  {
    fatherInfo = elInfo_;
    fatherLevel = 0;
  }

  // new LevelIterator with EL_INFO one level above 
  AlbertGridLevelIterator <0,dim,dimworld>
    it(grid_,fatherInfo,grid_.template indexOnLevel<0>(fatherInfo->el->index,fatherLevel),0,0,0);
  return it;  
}

template< int dim, int dimworld>
inline AlbertGridElement<dim,dim>& 
AlbertGridEntity < 0, dim ,dimworld >::father_relative_local()
{
  //AlbertGridLevelIterator<0,dim,dimworld> daddy = father();
  AlbertGridElement<dim,dimworld> daddy = (*father()).geometry();  

  fatherReLocal_.initGeom();
  // compute the local coordinates in father refelem
  for(int i=0; i<fatherReLocal_.corners(); i++)
    fatherReLocal_[i] = daddy.local(geometry()[i]);
  
  std::cout << "\nfather_realtive_local not tested yet! \n";
  return fatherReLocal_;
}
#endif
// end AlbertGridEntity
