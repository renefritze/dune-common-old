#ifndef __DUNE_FUNCTION_HH__
#define __DUNE_FUNCTION_HH__

#include <dune/common/mapping.hh>
#include <dune/common/fvector.hh>

namespace Dune{
/** @defgroup FunctionCommon Functions
    @ingroup AlgebraCommon
    Functions are Mappings from \f$K^n\f$ into \f$L^m\f$ where 
    \f$K\f$ and \f$L\f$ are fields.
 */

/** @defgroup Function Function
   \ingroup FunctionCommon

  @{
 */


typedef int deriType;
  
    /** \brief Class representing a function
     * \todo Please doc me!
     */
template< class FunctionSpaceType, class FunctionImp>
class Function : public Mapping < typename FunctionSpaceType::DomainField,
    typename FunctionSpaceType::RangeField , typename FunctionSpaceType::Domain, typename FunctionSpaceType::Range > {

public:
  //! ???
  typedef typename FunctionSpaceType::Domain Domain ;
  //! ???
  typedef typename FunctionSpaceType::Range Range ;
  //! ???
  typedef typename FunctionSpaceType::JacobianRange JacobianRange;
  //! ???
  typedef typename FunctionSpaceType::HessianRange  HessianRange;
  //! ???
  typedef FunctionSpaceType FunctionSpace;

  //! Constructor
  Function (const FunctionSpaceType & f) : functionSpace_ (f) {} ;  

  //! application operator
  virtual void operator()(const Domain & arg, Range & dest) const {
    eval(arg,dest);
  }

  //! evaluate Function
  void eval(const Domain & arg, Range & dest) const {
    asImp().eval(arg, dest);
  }

  //! evaluate function and derivatives 
  template <int derivation>
  void evaluate  ( const FieldVector<deriType, derivation> &diffVariable, 
                   const Domain& arg, Range & dest) const { 
    asImp().evaluate(diffVariable, arg, dest);
  }

  //! Get access to the related function space
  const FunctionSpaceType& getFunctionSpace() const { return functionSpace_; }

protected:
  //! Barton-Nackman trick
  FunctionImp& asImp() { 
    return static_cast<FunctionImp&>(*this); 
  }
  const FunctionImp& asImp() const { 
    return static_cast<const FunctionImp&>(*this); 
  }
  
  //! The related function space
  const FunctionSpaceType & functionSpace_;

};

/** @} end documentation group */

}

#endif
