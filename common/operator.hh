#ifndef DUNE_OPERATOR_HH
#define DUNE_OPERATOR_HH

#include "mapping.hh"

namespace Dune
{
/** @defgroup OperatorCommon Operators
  @ingroup AlgebraCommon
  Operators are mappings from function spaces into function spaces.
 */

/** @defgroup Operator Operator
    @ingroup OperatorCommon

  @{
 */

/** \brief An abstract operator
 *
 * \todo Please doc me!
 */
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

  //! Application operator  
  virtual void operator() (const DomainType& arg, RangeType& dest) const = 0;
 
private:
  //! Helper function for Mapping
  //! With this function, a combined mapping can choose the right application
  //! operator (i.e. the one from Mapping itself, or from Function/Operator)
  //! \note: Do not override this definition
  virtual void apply (const DomainType& arg, RangeType& dest) const {
    operator()(arg, dest);
  }
}; // end class Operator 

/** @} end documentation group */

} // end namespace Dune 

#endif
