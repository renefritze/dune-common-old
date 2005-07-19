#ifndef DUNE_AGMEMORY_HH
#define DUNE_AGMEMORY_HH

#include <stack>

namespace Dune
{

//! organize the memory management for entitys used by the NeighborIterator
template <class Object>
class AGMemoryProvider
{
  std::stack < Object * > objStack_;

  typedef AGMemoryProvider < Object > MyType;
public:
  typedef Object ObjectType;

  //! delete all objects stored in stack 
  AGMemoryProvider() {};

  //! call deleteEntity 
  ~AGMemoryProvider ();

  //! create object with empty constructor  
  Object * getNewObjectEntity(int l = 0)
  {
  if( objStack_.empty() )
  {
    return ( new Object () ); 
  }
  else
  {
    ObjectType * obj = objStack_.top();
    objStack_.pop();
    return obj;
  }
  }

  //! i.e. return pointer to Entity
  template <class GridType>
  ObjectType * getNewObjectEntity(const GridType &grid, int level);

  //! free, move element to stack, returns NULL 
  void freeObjectEntity (ObjectType * obj);

private:
  //! do not copy pointers  
  AGMemoryProvider(const AGMemoryProvider<Object> & org) {}
};


//************************************************************************
//
//  AGMemoryProvider implementation
//
//************************************************************************
template <class Object> template <class GridType>
inline typename AGMemoryProvider<Object>::ObjectType * 
AGMemoryProvider<Object>::getNewObjectEntity
(const GridType &grid, int level )
{
  if( objStack_.empty() )
  {
    return ( new Object (grid,level) ); 
  }
  else
  {
    ObjectType * obj = objStack_.top();
    objStack_.pop();
    return obj;
  }
}

/*
template <class Object> template <class GridType>
inline Object * AGMemoryProvider<Object>::getNewObjectEntity(int l)
{
  if( objStack_.empty() )
  {
    return ( new Object () ); 
  }
  else
  {
    ObjectType * obj = objStack_.top();
    objStack_.pop();
    return obj;
  }
}
*/

template <class Object>
inline AGMemoryProvider<Object>::~AGMemoryProvider()
{
  while ( !objStack_.empty() )
  {
    ObjectType * obj = objStack_.top();
    objStack_.pop();
    if( obj ) delete obj;
  }
}

template <class Object>
inline void AGMemoryProvider<Object>::freeObjectEntity(Object * obj)
{
  objStack_.push( obj );
}

typedef AGMemoryProvider < ALBERTA EL_INFO > ElInfoProvider;
static ElInfoProvider elinfoProvider;

} //end namespace 

#endif 
