#ifndef __DUNE_OPERATOR_HH__
#define __DUNE_OPERATOR_HH__

#include "mapping.hh"

namespace Dune
{

template <typename DFieldType, typename RFieldType,
          typename DType , typename RType>
class Operator : public Mapping <DFieldType,RFieldType,DType,RType>
{
public:
  //! remember template parameters for derived classes  
  typedef DType DomainType;
  typedef RType  RangeType;
  typedef DFieldType DomainFieldType;
  typedef RFieldType RangeFieldType;
  
  //! apply operator, used by mapping 
  void apply ( const DomainType & arg, RangeType & dest ) const
  {
    std::cerr << "Operator::apply is called! \n"; 
    abort();
  }
  
  //! apply operator, used by mapping 
  void operator () ( const DomainType & arg, RangeType & dest ) const
  {
    std::cerr << "Operator::operator () is called! \n"; 
    abort();
  }
    
private:

}; // end class Operator 

} // end namespace Dune 

#endif
