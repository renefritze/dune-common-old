#ifndef __DUNE_FUNCTION_HH__
#define __DUNE_FUNCTION_HH__

#include "mapping.hh"


namespace Dune{


template< class FunctionSpaceType, class FunctionImp>
class Function : public Mapping <typename FunctionSpaceType::RangeField , typename FunctionSpaceType::Domain, typename FunctionSpaceType::Range > {

public:
  typedef typename FunctionSpaceType::Domain Domain ;
  typedef typename FunctionSpaceType::Range Range ;
  typedef typename FunctionSpaceType::GradientRange GradientRange;
  typedef typename FunctionSpaceType::HessianRange  HessianRange;
  typedef FunctionSpaceType FunctionSpace;

  Function ( const FunctionSpaceType & f ) : functionSpace_ (f) {} ;  

  void evaluate ( const Domain & , Range &) const ;

  const FunctionSpaceType &getFunctionSpace() const { return functionSpace_; }

protected:
  
  const FunctionSpaceType & functionSpace_;

};


}

#endif
