#ifndef DUNE_UGGRIDLEVELITERATOR_HH
#define DUNE_UGGRIDLEVELITERATOR_HH


namespace Dune {

//**********************************************************************
//
// --UGGridLevelIterator
// --LevelIterator
/*!
 Enables iteration over all entities of a given codimension and level of a grid.
 */


template<int codim, int dim, int dimworld, PartitionIteratorType pitype>
class UGGridLevelIterator : 
    public LevelIteratorDefault <codim,dim,dimworld, pitype, UGCtype,
                                 UGGridLevelIterator,UGGridEntity>
{
  friend class UGGridEntity<2,dim,dimworld>;
  friend class UGGridEntity<1,dim,dimworld>;
  friend class UGGridEntity<0,dim,dimworld>;
  friend class UGGrid < dim , dimworld >;
public:

  //friend class UGGrid<dim,dimworld>;

  //! Constructor
  explicit UGGridLevelIterator(int travLevel);

  //! prefix increment
  UGGridLevelIterator<codim,dim,dimworld,pitype>& operator ++();

  //! equality
  bool operator== (const UGGridLevelIterator<codim,dim,dimworld, pitype>& i) const;

  //! inequality
  bool operator!= (const UGGridLevelIterator<codim,dim,dimworld, pitype>& i) const;

  //! dereferencing
  UGGridEntity<codim,dim,dimworld>& operator*() ;

  //! arrow
  UGGridEntity<codim,dim,dimworld>* operator->() ;
  
  //! ask for level of entity
  int level ();

private:
  // private Methods
  void makeIterator();

    void setToTarget(typename TargetType<codim,dim>::T* target) {
        target_ = target;
        virtualEntity_.setToTarget(target);
    }

    void setToTarget(typename TargetType<codim,dim>::T* target, int level) {
        target_ = target;
        level_  = level;
        virtualEntity_.setToTarget(target, level);
    }

  // private Members
  UGGridEntity<codim,dim,dimworld> virtualEntity_;

  //! element number 
  int elNum_; 

  //! level 
  int level_;
    
    typename TargetType<codim,dim>::T* target_;

};

}  // namespace Dune
  
#endif
