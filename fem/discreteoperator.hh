#ifndef __DUNE_DISCRETEOPERATOR_HH__
#define __DUNE_DISCRETEOPERATOR_HH__

#include "../common/operator.hh"
#include "combinedoperator.hh"

namespace Dune{


// Note: Range has to have Vector structure as well.
template <class DiscreteFunctionType, class LocalOperatorImp >
class DiscreteOperator 
: public Operator <typename DiscreteFunctionType::Traits<0>::RangeField,
    DiscreteFunctionType , DiscreteFunctionType> 
{  
public:
  typedef DiscreteOperator <DiscreteFunctionType, LocalOperatorImp > MyType;
  typedef typename DiscreteFunctionType::Traits<0>::RangeField Field;
  typedef typename DiscreteFunctionType::FunctionSpace::
    GridType::Traits<0>::Entity  EntityType;

  //! get LocalOperator 
  DiscreteOperator (LocalOperatorImp &op ) : _localOp ( op ) {};
   
  //! go over all Entitys and call the LocalOperator.applyLocal Method 
  //! Note that the LocalOperator can be an combined Operator 
  //! Domain and Range are defined through class Operator
  void apply ( const Domain &Arg, Range &Dest ) const   
  {
    typedef typename DiscreteFunctionType::FunctionSpace FunctionSpaceType;
    typedef typename FunctionSpaceType::GridType GridType;
    typedef typename GridType::Traits<0>::LevelIterator LevelIterator;

    const typename FunctionSpaceType & functionSpace_= Dest.getFunctionSpace();
    GridType &grid = const_cast<GridType &> (functionSpace_.getGrid());
    
    //std::cout << "DiscreteOperator::operator element wise () \n";
  
    LevelIterator levit = grid.lbegin<0>( grid.maxlevel() );
    LevelIterator endit = grid.lend<0> ( grid.maxlevel() );
    Range tmp( Dest );

    Dest.clear();

    //const_cast<DiscreteFunctionType &> (Arg).print();

    _localOp.prepare(&tmp);
    for( levit ; levit != endit; ++levit )
    {
      //std::cout << "Call applyLocal on element " << levit->index() << "\n";
      _localOp.applyLocal(*levit , Arg, Dest);
    }
  }
  
  void operator()( const Domain &Arg, Range &Dest )   
  {
    apply(Arg,Dest);
  }

private:
  LocalOperatorImp & _localOp;
};

//#include "mapping.cc"

}

#endif
