#ifndef __DUNE_FUNCTION_HH__
#define __DUNE_FUNCTION_HH__

#include "../common/mapping.hh"

namespace Dune{


template<class DiscreteFunctionSpaceType, class LocalFunctionType>
class DiscreteFunction : Function < DiscreteFunctionSpaceType, 
      DiscreteFunction<DiscreteFunctionSpaceType,LocalFunctionType> > 
{

public:
  DiscreteFunction ( const DiscreteFunctionSpaceType & f) 
    : Function < Field, DiscreteFunctionSpaceType>( f )  {} ;  

  void evaluate ( const Domain & , Range &) const ;

  template <class EvalEntityType>
  void evaluate ( const EvalEntityType &, const Domain &, Range & );

  void gradient ( const Domain &, GradientRange &) const;

  void hessian  ( const Domain &, HessianRange &) const;


private:

  
};

#include "discretefunction.cc" 

}

#endif
