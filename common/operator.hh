#ifndef __DUNE_OPERATOR_HH__
#define __DUNE_OPERATOR_HH__

#include "mapping.hh"

namespace Dune
{

template <typename FieldType, typename DomainType , typename RangeType>
class Operator : public Mapping <FieldType,DomainType,RangeType>
{
public:
  typedef DomainType Domain;
  typedef RangeType  Range;
  typedef FieldType  Field;

private:

};

}

#endif
