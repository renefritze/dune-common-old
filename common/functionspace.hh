#ifndef __DUNE_FUNCTIONSPACE_HH__
#define __DUNE_FUNCTIONSPACE_HH__

#include "matvec.hh"
#include "dynamictype.hh"

namespace Dune{

template< typename DomainFieldType, typename RangeFieldType, int n, int m >
class FunctionSpace : public DynamicType {
public:

  typedef DomainFieldType DomainField;
  typedef RangeFieldType  RangeField;
  
  typedef Mat < n, m, RangeField> JacobianRange;
  typedef Vec < m,  Mat< n, n, RangeField> > HessianRange ;

  typedef Vec<n, DomainField> Domain;
  typedef Vec<m, RangeField> Range;

  enum { DimDomain = n, DimRange = m};

  FunctionSpace ( int ident ) : DynamicType (ident){} ;

};

}

#endif
