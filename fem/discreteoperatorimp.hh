#ifndef __DUNE_DISCRETEOPERATOR_HH__
#define __DUNE_DISCRETEOPERATOR_HH__

#include <dune/fem/common/discreteoperator.hh>
#include <dune/fem/common/combinedoperator.hh>

namespace Dune{


// Note: Range has to have Vector structure as well.
template <class DiscreteFunctionType, class LocalOperatorImp >
class DiscreteOperator 
//: public DiscreteOperatorDefault 
//    < DiscreteFunctionType , LocalOperatorImp, 
//      DiscreteOperator <DiscreteFunctionType,LocalOperatorImp > > 
: public Operator <typename DiscreteFunctionType::RangeFieldType,
    DiscreteFunctionType , DiscreteFunctionType>
{  
  typedef typename DiscreteFunctionType::FunctionSpaceType::RangeField RangeFieldType;
    
public:
  //! get LocalOperator 
  DiscreteOperator (LocalOperatorImp &op, bool leaf=false ) 
    : localOp_ ( op ) , leaf_ (leaf), level_ (-1) , prepared_ (false) {};
 
  //! remember time step size 
  void prepare ( int level , const Domain &Arg, Range &Dest, 
                 Range *tmp , RangeFieldType & a, RangeFieldType & b) 
  {
    level_ = level;
    localOp_.prepareGlobal(level,Arg,Dest,tmp,a,b);
    prepared_ = true;
  }
 
  //! remember time step size 
  virtual void prepareG ( int level , const Domain &Arg, Range &Dest, 
                 Range *tmp , RangeFieldType & a, RangeFieldType & b) const
  {
    level_ = level;
    localOp_.prepareGlobal(level,Arg,Dest,tmp,a,b);
    prepared_ = true;
  }
 
  //! go over all Entitys and call the LocalOperator.applyLocal Method 
  //! Note that the LocalOperator can be an combined Operator 
  //! Domain and Range are defined through class Operator
  void apply ( const Domain &Arg, Range &Dest ) const 
  {
    if(!prepared_)
    {
      std::cerr << "DiscreteOperator::apply: I was not prepared! \n";
      abort();    
    }
    
    // useful typedefs
    typedef typename DiscreteFunctionType::FunctionSpace FunctionSpaceType;
    typedef typename FunctionSpaceType::GridType GridType;
    // the corresponding grid 
    FunctionSpaceType & functionSpace_= Dest.getFunctionSpace();
    GridType &grid = functionSpace_.getGrid();
    
    if(leaf_)
    {
      std::cout << "using  Leaf! \n";
      typedef typename GridType::LeafIterator LeafIterator;
  
      // make run through grid 
      LeafIterator it     = grid.leafbegin ( level_ );
      LeafIterator endit  = grid.leafend   ( level_ );
      applyOnGrid( it, endit , Arg, Dest );

    }
    else 
    {
      typedef typename GridType::Traits<0>::LevelIterator LevelIterator;

      // make run through grid 
      LevelIterator it = grid.template lbegin<0>( level_ );
      LevelIterator endit = grid.template lend<0>  ( level_ );
      applyOnGrid( it, endit , Arg, Dest );
    }
  }
  
  //! finalize the operation 
  void finalize ( int level , const Domain &Arg, Range &Dest, 
                  Range *tmp , RangeFieldType & a, RangeFieldType & b)
  {
    prepared_ = false;
    localOp_.finalizeGlobal(level,Arg,Dest,tmp,a,b);
  }
  
  //! finalize the operation 
  virtual void finalizeG ( int level , const Domain &Arg, Range &Dest, 
                  Range *tmp , RangeFieldType & a, RangeFieldType & b) const
  {
    prepared_ = false;
    localOp_.finalizeGlobal(level,Arg,Dest,tmp,a,b);
  }
 
  //! apply the operator 
  void operator()( const Domain &Arg, Range &Dest ) const   
  {
    apply(Arg,Dest);
  }

private:
  template <class GridIteratorType>
  void applyOnGrid ( GridIteratorType &it, GridIteratorType &endit,
                     const Domain &Arg, Range &Dest ) const 
  {
      // erase destination function
      Dest.clearLevel ( level_ );

      // run through grid and apply the local operator
      for( it ; it != endit; ++it )
      {
        localOp_.prepareLocal (it , Arg, Dest);
        localOp_.applyLocal   (it , Arg, Dest);
        localOp_.finalizeLocal(it , Arg, Dest);
      }
  }

  mutable bool prepared_;

  //! if true use LeafIterator else LevelIterator
  mutable bool leaf_;
  
  //! Level on which we operate 
  mutable int level_;
 
  //! Operator which is called on each entity
  LocalOperatorImp & localOp_;
};

}

#endif
