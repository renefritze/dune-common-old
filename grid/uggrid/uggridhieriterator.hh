#ifndef DUNE_UGHIERITERATOR_HH
#define DUNE_UGHIERITERATOR_HH

/** \file
 * \brief The UGGridHierarchicIterator class
 */

#include <dune/common/stack.hh>

namespace Dune {

//**********************************************************************
//
// --UGGridHierarchicIterator
// --HierarchicIterator
  /** \brief Iterator over the descendants of an entity.
   * \ingroup UGGrid
  Mesh entities of codimension 0 ("elements") allow to visit all entities of
  codimension 0 obtained through nested, hierarchic refinement of the entity.
  Iteration over this set of entities is provided by the HIerarchicIterator,
  starting from a given entity.
  This is redundant but important for memory efficient implementations of unstru
  hierarchically refined meshes.
 */

template<class GridImp>
class UGGridHierarchicIterator :
        public Dune::UGGridEntityPointer <0,GridImp>,
        public HierarchicIteratorDefault <GridImp,UGGridHierarchicIterator>
{

    friend class UGGridEntity<0,GridImp::dimension,GridImp>;

    // Either UG3d::ELEMENT or UG2d:ELEMENT
    typedef typename TargetType<0,GridImp::dimension>::T UGElementType;

    // Stack entry
    struct StackEntry {
        UGElementType* element;
        int level;
    };

public:
    typedef typename GridImp::template Codim<0>::Entity Entity;

    //! the default Constructor
    UGGridHierarchicIterator(int maxLevel);

    void increment();

  //! max level to go down 
  int maxlevel_;

    Stack<StackEntry> elemStack;
    
};

    // Include class method definitions
#include "uggridhieriterator.cc"

}  // end namespace Dune

#endif
