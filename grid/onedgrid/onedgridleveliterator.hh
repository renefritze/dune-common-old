#ifndef DUNE_ONE_D_GRID_LEVELITERATOR_HH
#define DUNE_ONE_D_GRID_LEVELITERATOR_HH

/** \file
 * \brief The OneDGridLevelIterator class
 */

#include <dune/common/dlist.hh>

namespace Dune {

template<int codim, class GridImp>
class OneDGridEntityPointer
  : public EntityPointerDefault <codim, GridImp, Dune::OneDGridEntityPointer<codim,GridImp> >
{
  enum { dim = GridImp::dimension };
public:
  typedef typename GridImp::template codim<codim>::Entity Entity;

  //! equality
  bool equals(const OneDGridEntityPointer<codim,GridImp>& other) const {
    return other.target_ == target_;
  }

  //! dereferencing
  Entity& dereference() const {return virtualEntity_;}
  
  //! ask for level of entity
  int level () const {return target_->level();}

protected:

    /** \brief Constructor from a given iterator */
    OneDGridEntityPointer(OneDEntityImp<dim-codim>* it) 
        /*: virtualEntity_()*/ {
        target_ = it;
        virtualEntity_.setToTarget(it);
    };
  
protected:
    mutable OneDEntityWrapper<codim,GridImp::dimension,GridImp> virtualEntity_;

    OneDEntityImp<dim-codim>* target_;
};


//**********************************************************************
//
// --OneDGridLevelIterator
// --LevelIterator
  /** \brief Iterator over all entities of a given codimension and level of a grid.
   * \ingroup OneDGrid
   */
template<int codim, PartitionIteratorType pitype, class GridImp>
class OneDGridLevelIterator :
    public OneDGridEntityPointer <codim, GridImp>,
    public LevelIteratorDefault <codim, pitype, GridImp, OneDGridLevelIterator>
{
public:
    enum {dim=GridImp::dimension};
    friend class OneDGridLevelIteratorFactory<codim>;
    friend class OneDGrid<dim,GridImp::dimensionworld>;
    friend class OneDGridEntity<codim,dim,GridImp>;
    friend class OneDGridEntity<0,dim,GridImp>;

    typedef typename GridImp::template codim<codim>::Entity Entity;

protected:

    /** \brief Constructor from a given iterator */
    OneDGridLevelIterator<codim,pitype, GridImp>(OneDEntityImp<dim-codim>* it)
      : OneDGridEntityPointer<codim, GridImp>(it)
    { 
    } 

public:

    //! prefix increment
    void increment() {
        this->target_ = this->target_->succ_;
        this->virtualEntity_.setToTarget(this->target_);
    }
};

}  // namespace Dune
  
#endif
