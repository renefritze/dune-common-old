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
         class DofIteratorImp, class LocalFunctionImp , 
         class DiscreteFunctionImp >
class DiscreteFunctionInterface 
: public Function < DiscreteFunctionSpaceType, 
      DiscreteFunctionInterface <DiscreteFunctionSpaceType, 
      DofIteratorImp , LocalFunctionImp , DiscreteFunctionImp > > 
{
public:
  // just for readability 
  typedef Function < DiscreteFunctionSpaceType,  
        DiscreteFunctionInterface <DiscreteFunctionSpaceType,  
              DofIteratorImp , LocalFunctionImp,  DiscreteFunctionImp > > FunctionType;
  //! remember the template types 
  template <int cc>
  struct Traits 
  {
    typedef typename DiscreteFunctionSpaceType::GridType GridType;
    typedef typename DiscreteFunctionSpaceType::Domain Domain;
    typedef typename DiscreteFunctionSpaceType::Range Range;
    typedef typename DiscreteFunctionSpaceType::RangeField RangeField;
  };

  typedef typename DiscreteFunctionSpaceType::GridType GridType;
  typedef DofIteratorImp DofIteratorType;
  typedef LocalFunctionImp LocalFunctionType;

  DiscreteFunctionInterface ( const DiscreteFunctionSpaceType &f ) 
    : FunctionType ( f ) {} ;


  //! access to the local function. Local functions can only be accessed
  //! for an existing entity. 
  template <class EntityType> 
  LocalFunctionType & access (EntityType & en )
  {
    return asImp().access(en);
  }
  
  //! access to the local function. Local functions can only be accessed
  //! for an existing entity. 
  void done (LocalFunctionType & lf )
  {
    return asImp().done(lf);
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
         class DofIteratorImp, class LocalFunctionImp, class DiscreteFunctionImp >
class DiscreteFunctionDefault 
: public DiscreteFunctionInterface  
  <DiscreteFunctionSpaceType, DofIteratorImp, 
  LocalFunctionImp, DiscreteFunctionImp > 
{ 

  typedef DiscreteFunctionInterface <DiscreteFunctionSpaceType, 
    DofIteratorImp, LocalFunctionImp, DiscreteFunctionImp >  DiscreteFunctionInterfaceType;

  enum { myId_ = 0 };  
public:
  //! remember the used types 
  template <int cc>
  struct Traits 
  {
    typedef typename DiscreteFunctionSpaceType::Domain Domain;
    typedef typename DiscreteFunctionSpaceType::Range Range;
    typedef typename DiscreteFunctionSpaceType::RangeField RangeField;
    typedef typename DiscreteFunctionSpaceType::DomainField DomainField;
  };
  
  typedef typename DiscreteFunctionSpaceType::RangeField RangeField;
  typedef typename DiscreteFunctionSpaceType::DomainField DomainField;
  typedef DofIteratorImp DofIteratorType;

  //! pass the function space to the interface class
  DiscreteFunctionDefault ( const DiscreteFunctionSpaceType & f ) :
    DiscreteFunctionInterfaceType ( f ) {};

  //! derived from Function 
  //! search for element which contains point x an evaluate 
  //! dof entity with en 
  void evaluate ( const typename Traits<0>::Domain & , 
                        typename Traits<0>::Range &) const 
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

  template <class EntityType> 
  void assignLocal(EntityType &en, const DiscreteFunctionImp &g,
      const DiscreteFunctionSpaceType::RangeField &scalar)
  {
    std::cout << "AssignLocal \n"; 
  }
  
  template <class EntityType> 
  void addLocal(EntityType &en, const DiscreteFunctionImp &g,
      const DiscreteFunctionSpaceType::RangeField &scalar)
  {
    std::cout << "AddLocal \n"; 
  }
  
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
