#ifndef __DUNE_DISCRETEFUNCTION_HH__
#define __DUNE_DISCRETEFUNCTION_HH__

#include "../grid/common/grid.hh"
#include "../common/function.hh"
#include "../common/functionspace.hh"
#include "discretefunctionspace.hh"

namespace Dune{

/** @defgroup DiscreteFunction The Interface for DiscreteFunctions
    
    The DiscreteFunction is resposible for the dof storage. This can be
    done in various ways an is left to the user. The user has to derive his
    own implemenation from the DiscreteFunctionDefault class. If some of
    the implementations in the default class are for his dof storage
    unefficient, then one can overload this functions.
  
  @{
 */

typedef std::basic_string <char> StringType;

template <typename T>
StringType typeIdentifier ()
{
  StringType tmp = "unknown";
  return tmp;
};

template <>
StringType typeIdentifier<float> ()
{
  StringType tmp = "float";
  return tmp;
};

template <>
StringType typeIdentifier<int> ()
{
  StringType tmp = "int";
  return tmp;
};

template <>
StringType typeIdentifier<double> ()
{
  StringType tmp = "double";
  return tmp;
};


//************************************************************************
//
//  --DiscreteFunctionInterface
//
//! This is the minimal interface of a discrete function which has to be
//! implemented. It contains an local function and a dof iterator which can 
//! iterator over all dofs of one level. Via the method access the local
//! dofs and basfunction can be accessed for a given entity.
//! 
//************************************************************************
template<class DiscreteFunctionSpaceType, 
         class DofIteratorImp, 
         template <class , class> class LocalFunctionIteratorImp , 
         class DiscreteFunctionImp >
class DiscreteFunctionInterface 
: public Function < DiscreteFunctionSpaceType, 
      DiscreteFunctionInterface <DiscreteFunctionSpaceType, 
      DofIteratorImp , LocalFunctionIteratorImp , DiscreteFunctionImp > > 
{
public:
  //! types that we sometimes need outside 
  typedef Function < DiscreteFunctionSpaceType,  
        DiscreteFunctionInterface <DiscreteFunctionSpaceType,  
              DofIteratorImp , LocalFunctionIteratorImp,  DiscreteFunctionImp > > FunctionType;

  typedef typename DiscreteFunctionSpaceType::Domain DomainType;
  typedef typename DiscreteFunctionSpaceType::Range RangeType;
  
  typedef typename DiscreteFunctionSpaceType::DomainField DomainFieldType;
  typedef typename DiscreteFunctionSpaceType::RangeField RangeFieldType;
  
  //! remember the template types 
  template <class GridIteratorType>
  struct Traits 
  {
    typedef typename 
      LocalFunctionIteratorImp<DiscreteFunctionImp,GridIteratorType>
      LocalFunctionIteratorType;
  };

  typedef typename DiscreteFunctionSpaceType::GridType GridType;
  typedef DofIteratorImp DofIteratorType;

  //* end of type declarations

  DiscreteFunctionInterface ( const DiscreteFunctionSpaceType &f ) 
    : FunctionType ( f ) {} ;

  template <class GridIteratorType>
  LocalFunctionIteratorImp<DiscreteFunctionImp,GridIteratorType> 
  localFunction ( GridIteratorType & it)
  {
    return asImp().localFunction( it );
  }

  //! the implementation of an iterator to iterate efficient over all dof
  //! on one level. 
  DofIteratorType dbegin ( int level ) 
  {
    return asImp().dbegin ( level );
  };

  //! the implementation of an iterator to iterate efficient over all dof
  //! on one level 
  DofIteratorType dend ( int level ) 
  {
    return asImp().dend ( level );
  };

private:
  // Barton-Nackman trick 
  DiscreteFunctionImp &asImp() 
  { 
    return static_cast<DiscreteFunctionImp&>(*this); 
  }
  const DiscreteFunctionImp &asImp() const 
  { 
    return static_cast<const DiscreteFunctionImp&>(*this); 
  }
};

//*************************************************************************
//
//  --DiscreteFunctionDefault
//  
//! Default implementation of the discrete function. This class provides 
//! is responsible for the dof storage. Different implementations of the
//! discrete function use different dof storage. 
//! The default implementation provides +=, -= ans so on operators and 
//! a DofIterator access, which can run over all dofs in an efficient way. 
//! Furthermore with an entity you can access an local function to evaluate
//! the discrete function by multiplying the dofs and the basefunctions. 
//! 
//*************************************************************************
template<class DiscreteFunctionSpaceType, 
        // class DofIteratorImp, class LocalFunctionImp, class DiscreteFunctionImp >
         class DofIteratorImp, 
         template <class , class> class LocalFunctionIteratorImp, 
         class DiscreteFunctionImp >
class DiscreteFunctionDefault 
: public DiscreteFunctionInterface  
  <DiscreteFunctionSpaceType, DofIteratorImp, 
  LocalFunctionIteratorImp, DiscreteFunctionImp > 
{ 

  typedef DiscreteFunctionInterface <DiscreteFunctionSpaceType, 
    DofIteratorImp, LocalFunctionIteratorImp, DiscreteFunctionImp >  DiscreteFunctionInterfaceType;

  enum { myId_ = 0 };  
public:
#if 0
  //! type declaration that we could need outside 
  typedef typename DiscreteFunctionSpaceType::RangeField RangeFieldType;
  typedef typename DiscreteFunctionSpaceType::DomainField DomainFieldType;
  typedef DofIteratorImp DofIteratorType;
#endif

  //! pass the function space to the interface class
  DiscreteFunctionDefault ( const DiscreteFunctionSpaceType & f ) :
    DiscreteFunctionInterfaceType ( f ) {};

  //! derived from Function 
  //! search for element which contains point x an evaluate 
  //! dof entity with en 
  void evaluate ( const DomainType & , RangeType &) const 
  {
    // search element  
  };


  //! evaluate an scalar product of the dofs of two DiscreteFunctions
  DiscreteFunctionSpaceType::RangeField scalarProductDofs( const DiscreteFunctionDefault &g );
  
  // assign 
  Vector<DiscreteFunctionSpaceType::RangeField> & 
  assign(const Vector<DiscreteFunctionSpaceType::RangeField> &g);
 
  // assign 
  Vector<DiscreteFunctionSpaceType::RangeField> & 
  operator = (const Vector<DiscreteFunctionSpaceType::RangeField> &g);

  // add
  Vector<DiscreteFunctionSpaceType::RangeField> & 
  operator += (const Vector<DiscreteFunctionSpaceType::RangeField> &g);
  
  // add
  void addScaled (const Vector<DiscreteFunctionSpaceType::RangeField> &g, 
             const  DiscreteFunctionSpaceType::RangeField &scalar);

  // substract
  Vector<DiscreteFunctionSpaceType::RangeField> & 
  operator -= (const Vector<DiscreteFunctionSpaceType::RangeField> &g);
 
  // multiply 
  Vector<DiscreteFunctionSpaceType::RangeField> & 
  operator *= (const DiscreteFunctionSpaceType::RangeField &scalar);

  Vector<DiscreteFunctionSpaceType::RangeField> & 
  operator /= (const DiscreteFunctionSpaceType::RangeField &scalar);

  // add 
  Vector<DiscreteFunctionSpaceType::RangeField> & 
  add(const Vector<DiscreteFunctionSpaceType::RangeField> &g ,
      DiscreteFunctionSpaceType::RangeField scalar );

  //! clear all dofs of a given level of the discrete function
  void clearLevel(int level ); 

  //! write disc func information file and write dofs to file+timestep
  //! this method use the write method of the implementation of the
  //! discrete function
  template <FileFormatType ftype>
  bool write(const char *filename, int timestep);

  //! same as write only read
  template <FileFormatType ftype>
  bool read(const char *filename, int timestep);
  
private:
  // Barton-Nackman trick 
  DiscreteFunctionImp &asImp() 
  { 
    return static_cast<DiscreteFunctionImp&>(*this); 
  }
  const DiscreteFunctionImp &asImp() const 
  { 
    return static_cast<const DiscreteFunctionImp&>(*this); 
  }

}; // end class DiscreteFunctionDefault 

/** @} end documentation group */


} // end namespace Dune

#include "discretefunction.cc"

#endif
