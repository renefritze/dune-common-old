#ifndef __DUNE_ALUMEMORY_HH__
#define __DUNE_ALUMEMORY_HH__

#include <stack>

//! organize the memory management for entitys used by the NeighborIterator
template <class Object>
class MemoryProvider
{
  std::stack < Object * > objStack_;
public:
  typedef Object ObjectType;

  //! freeEntity_ = NULL
  MemoryProvider() {};

  //! call deleteEntity 
  ~MemoryProvider ();

  //! i.e. return pointer to Entity
  template <class GridType>
  ObjectType * getNewObjectEntity(const GridType &grid, int level);

  //! free, move element to stack, returns NULL 
  void freeObjectEntity (ObjectType * obj);

private:
  //! do not copy pointers  
  MemoryProvider(const MemoryProvider<Object> & org) {}
};


//************************************************************************
//
//  MemoryProvider implementation
//
//************************************************************************
template <class Object> template <class GridType>
inline typename MemoryProvider<Object>::ObjectType *
MemoryProvider<Object>::getNewObjectEntity
(const GridType &grid, int level )
{
  if( objStack_.empty() )
  {
    ObjectType * obj = new Object (grid,level);
    return obj;
  }
  else
  {
    ObjectType * obj = objStack_.top();
    objStack_.pop();
    return obj;
  }
}

template <class Object>
inline MemoryProvider<Object>::~MemoryProvider()
{
  /*
  while ( !objStack_.empty() )
  {
    ObjectType * obj = objStack_.top();
    objStack_.pop();
    if( obj ) delete obj;
  }
  */
}

template <class Object>
inline void MemoryProvider<Object>::freeObjectEntity(ObjectType * obj)
{
  objStack_.push( obj );
}

#endif 
