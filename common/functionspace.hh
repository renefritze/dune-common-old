#ifndef DUNE_FUNCTIONSPACE_HH
#define DUNE_FUNCTIONSPACE_HH

#include <dune/common/fmatrix.hh>
#include "dynamictype.hh"

namespace Dune{

/** @defgroup FunctionSpace FunctionSpace
  @ingroup Function
  This provides the interfaces for discrete function spaces. 
  
  @{
 */

/** \brief An arbitrary function space
    Base class for specific function spaces.
*/
template< typename DomainFieldImp, typename RangeFieldImp, int n, int m >
class FunctionSpace : public DynamicType {
public:

/** Intrinsic type used for values in the domain field (usually a double) */
  typedef DomainFieldImp DomainFieldType;
/** Intrinsic type used for values in the range field (usually a double) */
  typedef RangeFieldImp  RangeFieldType;
  
/** \todo Please doc me! */
    typedef FieldMatrix <RangeFieldImp, m, n> JacobianRangeType;
/** \todo Please doc me! */
  typedef FieldVector<FieldMatrix<RangeFieldImp, n, n>, m> HessianRangeType;

/** Type of domain vector (using type of domain field) */
    typedef FieldVector<DomainFieldImp, n> DomainType;
/** Type of range vector (using type of range field) */
    typedef FieldVector<RangeFieldImp, m> RangeType;

/** Remember the dimensions of the domain and range field */
  enum { DimDomain = n, DimRange = m};

/** Constructor taking an identifier */
  FunctionSpace ( int ident ) : DynamicType (ident){} ;

};

  template <class FunctionSpaceImp>
  struct ToScalarFunctionSpace {};

  template <
    class DomainFieldImp, class RangeFieldImp, int dimDomain, int dimRange>
  struct ToScalarFunctionSpace<
    FunctionSpace<DomainFieldImp, RangeFieldImp, dimDomain, dimRange> >
  {
    typedef FunctionSpace<DomainFieldImp, RangeFieldImp, dimDomain, 1> Type;
  };

/** @} end documentation group */

}

#endif
