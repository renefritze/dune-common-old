//*******************************************************
// 
// --UGGridLevelIterator
// --LevelIterator
// 
//*******************************************************


// Make LevelIterator with point to element from previous iterations
template<int codim, int dim, int dimworld, PartitionIteratorType pitype>
inline UGGridLevelIterator<codim,dim,dimworld,pitype>::
UGGridLevelIterator(int travLevel) : virtualEntity_(0), level_ (travLevel)
{
    target_ = NULL;

    virtualEntity_.setToTarget(NULL);
}


#ifdef _3
template<>
inline UGGridLevelIterator < 3,3,3, All_Partition >& 
UGGridLevelIterator < 3,3,3,All_Partition >::operator++()
{

    target_ = target_->succ;
    
    setToTarget(target_);
    virtualEntity_.elNum_++;
    
    return (*this);
}

template<>
inline UGGridLevelIterator < 0,3,3, All_Partition >& 
UGGridLevelIterator < 0,3,3, All_Partition >::operator++()
{
    setToTarget(target_->ge.succ);
    virtualEntity_.elNum_++;
    return (*this);
}
#endif

#ifdef _2
template<>
inline UGGridLevelIterator < 2,2,2,All_Partition >& 
UGGridLevelIterator < 2,2,2,All_Partition>::operator++()
{

    target_ = target_->succ;
    
    setToTarget(target_);
    virtualEntity_.elNum_++;
    
    return (*this);
}

template<>
inline UGGridLevelIterator < 0,2,2,All_Partition >& 
UGGridLevelIterator < 0,2,2,All_Partition >::operator++()
{
    setToTarget(target_->ge.succ);
    virtualEntity_.elNum_++;
    return (*this);
}
#endif

// gehe zum i Schritte weiter , wie auch immer
template<int codim, int dim, int dimworld, PartitionIteratorType pitype>
inline UGGridLevelIterator < codim,dim,dimworld, pitype >& 
UGGridLevelIterator < codim,dim,dimworld,pitype >::operator++()
{
    printf("LevelIterator< %d, %d, %d >::operator++  Unimplemented case!\n",
           codim, dim, dimworld);
    return (*this);
}


template<int codim, int dim, int dimworld, PartitionIteratorType pitype>
inline bool UGGridLevelIterator<codim,dim,dimworld,pitype >::
operator ==(const UGGridLevelIterator<codim,dim,dimworld,pitype > &I) const 
{
    return target_==I.target_;
}

template<int codim, int dim, int dimworld, PartitionIteratorType pitype>
inline bool UGGridLevelIterator < codim,dim,dimworld,pitype >::
operator !=(const UGGridLevelIterator< codim,dim,dimworld,pitype > & I) const
{
    return !((*this)==I); 
}

template<int codim, int dim, int dimworld, PartitionIteratorType pitype>
inline UGGridEntity<codim, dim, dimworld> & 
UGGridLevelIterator< codim,dim,dimworld,pitype >::operator *()
{
  return virtualEntity_;
}

template<int codim, int dim, int dimworld, PartitionIteratorType pitype>
inline UGGridEntity< codim,dim,dimworld >* 
UGGridLevelIterator< codim,dim,dimworld,pitype >::operator ->()
{
  return &virtualEntity_;
}

template<int codim, int dim, int dimworld, PartitionIteratorType pitype>
inline int UGGridLevelIterator<codim,dim,dimworld,pitype >::level()
{
  return level_;
}
