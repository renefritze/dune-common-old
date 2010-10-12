// $Id: fvector.hh 5262 2008-09-07 09:03:38Z christi $
#ifndef DUNE_MATVECTRAITS_HH
#define DUNE_MATVECTRAITS_HH

namespace Dune {

/**
   @addtogroup DenseMatVec
   \brief Type Traits to retrieve types associated with an implementation of Dune::DenseVector or Dune::DenseMatrix

   you have to specialize this class for every implementation of DenseVector or DenseMatrix.

   \code
   //! export the type of the derived class (e.g. FieldVector<K,SIZE>)
   typedef ... derived_type;
   //! export the type of the stored values
   typedef ... value_type;
   //! export the type representing the size information
   typedef ... size_type;
   \endcode

*/
template<class T>
struct DenseMatVecTraits {};

} // end namespace Dune

#endif // DUNE_FTRAITS_HH
