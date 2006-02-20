#ifndef DUNE_UGGRIDLEVELITERATOR_HH
#define DUNE_UGGRIDLEVELITERATOR_HH

#include <dune/grid/uggrid/uggridentitypointer.hh>

/** \file
 * \brief The UGGridLevelIterator class
 */

namespace Dune {

    // Forward declaration
    template <int codim, PartitionIteratorType PiType, class GridImp>
    class UGGridLevelIteratorFactory;

//**********************************************************************
//
// --UGGridLevelIterator
// --LevelIterator
  /** \brief Iterator over all entities of a given codimension and level of a grid.
   * \ingroup UGGrid
   */
    template<int codim, PartitionIteratorType pitype, class GridImp>
class UGGridLevelIterator : 
        public Dune::UGGridEntityPointer <codim,GridImp>,
        public LevelIteratorDefaultImplementation <codim,pitype,GridImp,UGGridLevelIterator>
{
    enum {dim = GridImp::dimension};

    friend class UGGridEntity<codim,GridImp::dimension,GridImp>;
    friend class UGGridEntity<0,    GridImp::dimension,GridImp>;

    template <int codim_, PartitionIteratorType PiType_, class GridImp_>
    friend class UGGridLevelIteratorFactory;

public:

    typedef typename GridImp::template Codim<codim>::Entity Entity;

  //! Constructor
    explicit UGGridLevelIterator(int travLevel)
    {
        this->virtualEntity_.setToTarget(0,travLevel);
    }

  //! Constructor
    explicit UGGridLevelIterator(typename TargetType<codim,dim>::T* target, int level)
    {
        this->virtualEntity_.setToTarget(target, level);
    }

    //! prefix increment
    void increment() {
        this->virtualEntity_.setToTarget(UG_NS<GridImp::dimension>::succ(this->virtualEntity_.getTarget()));
    }

};

}  // namespace Dune
  
#endif
