//*************************************************************************
//
//  --UGGridEntity 
//  --Entity 
//
//*************************************************************************


//
//  codim > 0
//
//*********************************************************************


template<int codim, int dim, class GridImp>
inline UGGridEntity < codim, dim ,GridImp>::
UGGridEntity(int level) : 
    geo_(),
    level_ (level)
{
}

template<int codim, int dim, class GridImp>
inline void UGGridEntity < codim, dim ,GridImp>::
setToTarget(typename TargetType<codim,dim>::T* target)
{
    target_ = target;
    geo_.setToTarget(target);
}

template<int codim, int dim, class GridImp>
inline void UGGridEntity < codim, dim ,GridImp>::
setToTarget(typename TargetType<codim,dim>::T* target, int level)
{
    target_ = target;
    level_  = level;
    geo_.setToTarget(target);
}

template<int codim, int dim, class GridImp>
inline int UGGridEntity < codim, dim ,GridImp>::
level() const
{
  return level_;
}


template<int codim, int dim, class GridImp>
inline int UGGridEntity < codim, dim ,GridImp>::
index() const
{
    std::cerr << "UGGridEntity <" << codim << ", " << dim 
              << ">::index() not implemented!\n";
    return -1;
}

#ifdef _3
template<class GridImp>
inline int UGGridEntity < 3, 3 ,GridImp>::
index()
{
    return target_->myvertex->iv.id;
}

template<>
inline int UGGridEntity < 0, 3 ,3 >::
index()
{
    return target_->ge.id;
}
#endif

#ifdef _2
#if 0
template<class GridImp>
inline int UGGridEntity < 2, 2 ,GridImp>::
index()
{
    return target_->myvertex->iv.id;
}

template<>
inline int UGGridEntity < 0, 2 ,2 >::
index()
{
    return target_->ge.id;
}
#endif
#endif

template< int codim, int dim, class GridImp>
inline const typename UGGridEntity<codim,dim,GridImp>::Geometry& 
UGGridEntity < codim, dim ,GridImp>::
geometry() const
{
  return geo_;
}



//************************************
//
//  --UGGridEntity codim = 0 
//  --Entity codim = 0 
//
//************************************
template< int dim, class GridImp>
inline bool UGGridEntity < 0, dim ,GridImp>::
mark( int refCount )
{
#ifdef _3
    if (!UG3d::EstimateHere(target_))
        return false;

    return UG3d::MarkForRefinement(target_, 
                                   UG3d::RED, 
                                   0);  // no user data
#else
    if (!UG2d::EstimateHere(target_))
        return false;
    
    return UG2d::MarkForRefinement(target_, 
                                   UG2d::RED, // red refinement rule
                                   0);  // no user data
#endif
}



//*****************************************************************8
// count
template <int codim, int dim, class GridImp> 
template <int cc> 
inline int UGGridEntity<codim,dim,GridImp>::count () const
{
    DUNE_THROW(GridError, "UGGridEntity<" << codim << ", " << dim 
               << ">::count() not implemented yet!");
    return -1;
}

template <int codim, int dim, class GridImp>
template <int cc>
inline int UGGridEntity<codim, dim, GridImp>::subIndex(int i) const
{
    DUNE_THROW(GridError, "UGGridEntity<" << codim << ", " << dim 
               << ">::subIndex(int i) not implemented yet!");
    return 0;
}


#if 0  // only for codim 0 ?!?
template <int codim, int dim, class GridImp> 
template <int cc>
inline UGGridLevelIterator<cc,All_Partition,GridImp> 
UGGridEntity<codim,dim,GridImp>::entity ( int i )
{
    DUNE_THROW(GridError, "UGGridEntity<" << codim << ", " << dim
               << ">::entity(int i) not implemented yet!");
    UGGridLevelIterator<cc,All_Partition,GridImp> tmp (level_);
  return tmp;
}
#endif


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//     Specializations for codim == 0                                     //
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

template< int dim, class GridImp>
inline AdaptationState UGGridEntity < 0, dim ,GridImp >::state() const
{
    std::cerr << "UGGridEntity::state() not yet implemented!\n";
    return NONE;
}
//*****************************************************************8
// count
template <int dim, class GridImp> 
template <int cc> 
inline int UGGridEntity<0,dim,GridImp>::count() const
{
    if (dim==3) {

        switch (cc) {
        case 0:
        return 1;
#ifdef _3
        case 1:
            return UG_NS<3>::Sides_Of_Elem(target_);
        case 2:
            return UG_NS<3>::Edges_Of_Elem(target_);
        case 3:
            return UG_NS<3>::Corners_Of_Elem(target_);
#endif
        } 

    } else {

        switch (cc) {
        case 0:
        return 1;
#ifdef _2
        case 1:
            return UG_NS<2>::Edges_Of_Elem(target_);
        case 2:
            return UG_NS<2>::Corners_Of_Elem(target_);
#endif
        } 

    }
  return -1;
}


template <int dim, class GridImp>
template <int cc>
inline int UGGridEntity<0, dim, GridImp>::subIndex(int i) const
{
    assert(i>=0 && i<count<cc>());

    if (cc!=dim)
        DUNE_THROW(GridError, "UGGrid::subIndex isn't implemented for cc != dim");

    if (geometry().type()==hexahedron) {
        // Dune numbers the vertices of a hexahedron differently than UG.
        // The following two lines do the transformation
        const int renumbering[8] = {0, 1, 3, 2, 4, 5, 7, 6};
        i = renumbering[i];
    }

    typename TargetType<dim,dim>::T* node = UG_NS<dim>::Corner(target_,i);
    return node->myvertex->iv.id;
}


template <int dim, class GridImp> 
template <int cc>
inline typename GridImp::template codim<cc>::EntityPointer
UGGridEntity<0,dim,GridImp>::entity ( int i ) const
{
    assert(i>=0 && i<count<cc>());

    if (cc!=dim)
        DUNE_THROW(GridError, "UGGrid::subIndex isn't implemented for cc != dim");

    if (geometry().type()==hexahedron) {
        // Dune numbers the vertices of a hexahedron differently than UG.
        // The following two lines do the transformation
        const int renumbering[8] = {0, 1, 3, 2, 4, 5, 7, 6};
        i = renumbering[i];
    }

    typename TargetType<cc,dim>::T* subEntity = UGGridSubEntityFactory<cc,dim>::get(target_,i);
    UGGridLevelIterator<cc,All_Partition,GridImp> it (level_);
    it.setToTarget(subEntity, level_);
    return it;
}

template<int dim, class GridImp>
inline UGGridEntity < 0, dim ,GridImp >::
UGGridEntity(int level) : 
    //geo_(),
    level_ (level)
{
}

template<int dim, class GridImp>
inline void UGGridEntity < 0, dim ,GridImp >::
setToTarget(typename TargetType<0,dim>::T* target, int level)
{
    target_ = target;
    level_  = level;
    geo_.setToTarget(target);
}

template<int dim, class GridImp>
inline void UGGridEntity < 0, dim ,GridImp >::
setToTarget(typename TargetType<0,dim>::T* target)
{
    target_ = target;
    geo_.setToTarget(target);
}

template<int dim, class GridImp>
inline UGGridIntersectionIterator<GridImp> 
UGGridEntity < 0, dim ,GridImp >::ibegin() const
{
    UGGridIntersectionIterator<GridImp> it;

    // Set intersection iterator to zeroth neighbor of target_
    it.setToTarget(target_, 0);

    return it;
}


template< int dim, class GridImp>
inline UGGridIntersectionIterator<GridImp> 
UGGridEntity < 0, dim ,GridImp >::iend() const
{
    UGGridIntersectionIterator<GridImp> it;

    it.setToTarget(NULL, -1);

    return it;
}

template<int dim, class GridImp>
inline UGGridHierarchicIterator<GridImp> 
UGGridEntity < 0, dim ,GridImp >::hbegin(int maxlevel) const
{
    UGGridHierarchicIterator<GridImp> it(level(), maxlevel);

    if (level()<maxlevel) {

        // The 30 is the macro MAX_SONS from ug/gm/gm.h
        UGElementType* sonList[30];
#ifdef _2
        UG2d::GetSons(target_,sonList);
#else
        UG3d::GetSons(target_,sonList);
#endif

#ifdef _2
#define NSONS(p) UG2d::ReadCW(p, UG2d::NSONS_CE)
#else
#define NSONS(p) UG3d::ReadCW(p, UG3d::NSONS_CE)
#endif
        // Load sons of entity into the iterator
        for (unsigned int i=0; i<NSONS(target_); i++) {
            typename UGGridHierarchicIterator<GridImp>::StackEntry se;
            se.element = sonList[i];
            se.level   = level()+1;
            it.elemStack.push(se);
        }
#undef NSONS

    }

    if (it.elemStack.empty()) {
        it.virtualEntity_.setToTarget(0);
    } else {
        // Set intersection iterator to first son
        it.virtualEntity_.setToTarget(it.elemStack.top().element, it.elemStack.top().level);
    }

    return it;
}


template< int dim, class GridImp>
inline UGGridHierarchicIterator<GridImp> 
UGGridEntity < 0, dim ,GridImp >::hend(int maxlevel) const
{
    UGGridHierarchicIterator<GridImp> it(level(), maxlevel);

    it.target_ = 0;

    return it;
}


template<int dim, class GridImp>
inline int UGGridEntity < 0, dim ,GridImp >::
level() const
{
  return level_;
}

template< int dim, class GridImp>
inline const typename UGGridEntity<0,dim,GridImp>::Geometry& 
UGGridEntity < 0, dim ,GridImp >::
geometry() const
{
  return geo_;
}


template<int dim, class GridImp>
inline UGGridLevelIterator<0,All_Partition,GridImp> 
UGGridEntity < 0, dim, GridImp>::father() const
{
    UGGridLevelIterator<0,All_Partition,GridImp> it(level()-1);
    it.setToTarget(UG_NS<dim>::EFather(target_));
    return it;
}

/** \todo Method not implemented yet! */
template<int dim, class GridImp>
inline UGGridGeometry<dim,dim,GridImp>& 
UGGridEntity < 0, dim, GridImp>::father_relative_local()
{
    DUNE_THROW(GridError, "UGGridEntity < 0, dim, dimworld>::father_relative_local() not implemented yet!");
    return fatherReLocal_;
}
