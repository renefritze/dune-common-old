#ifndef __DUNE_QUADRATURE_HH__
#define __DUNE_QUADRATURE_HH__

#include <vector>

#include "../common/dynamictype.hh"
#include "basefunctions.hh"

template< class FunctionSpaceType> class BaseFunctionSet;


namespace Dune{

template< class FunctionSpaceType >
class Quadrature: public DynamicType {

  typedef typename FunctionSpaceType::Domain DomainType ; 
  typedef typename FunctionSpaceType::RangeField RangeFieldType ; 
    
  friend class BaseFunctionSet < FunctionSpaceType >  ;

public:
    
  Quadrature ( int ident ) : DynamicType (ident) ;

  int getNumberOfQuadPoints () const ;

  const RangeFieldType&  getQuadratureWeights ( int ) const ;

  const DomainType&  getQuadraturePoints (int ) const ;
    
private:
  int numberOfPoints_ ;

  std::vector< RangeFieldType > weights_ ;
  std::vector< DomainType > points_ ;

}; // end class Quadrature 

} // end namespace Dune 

#endif
