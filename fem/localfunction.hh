#ifndef __DUNE_LOCALFUNCTION_HH__
#define __DUNE_LOCALFUNCTION_HH__

namespace Dune {
//****************************************************************************
//
//  --LocalFunctionInterface 
//
//! The LocalFunctionInterface is the Interface to local function which
//! form the discrete Function 
//
//****************************************************************************
template < class DiscreteFunctionSpaceType, class LocalFunctionImp > 
class LocalFunctionInterface 
{
public:

  //! this are the types for the derived classes 
  typedef typename DiscreteFunctionSpaceType::RangeField  RangeField;
  typedef typename DiscreteFunctionSpaceType::Domain      Domain;
  typedef typename DiscreteFunctionSpaceType::Range       Range;

  //! access to dof number num, all dofs of the local function
  RangeField& operator [] (int num) 
  {
    return asImp().operator [] ( num );
  }

  //! return the number of local dof of this local function 
  int numberOfDofs () 
  { 
    return asImp().numberOfDofs ();
  };

  //! evaluate the local function on x and return ret 
  template <class EntityType>
  void evaluate (EntityType &en, const Domain & x, Range & ret)
  {
    asImp().evaluate(en,x,ret);
  }

private:
  //! Barton-Nackman trick 
  LocalFunctionImp & asImp() 
  { 
    return static_cast<LocalFunctionImp&>(*this);
  }
  const LocalFunctionImp &asImp() const  
  {
    return static_cast<const LocalFunctionImp&>(*this);
  }
}; // end LocalFunctionInterface

//************************************************************************
//
//  --LocalFunctionDefault 
//
//! The Interface to the dune programmer, use this class to derive 
//! the own implementation. But only the methods declared in the interface
//! class must be implemented 
//
//************************************************************************
template < class DiscreteFunctionSpaceType, class LocalFunctionImp > 
class LocalFunctionDefault : public LocalFunctionInterface <
DiscreteFunctionSpaceType , LocalFunctionImp > 
{
public: 
  // notin' 
}; // end LocalFunctionDefault

//-------------------------------------------------------------------------

//**************************************************************************
//
//  --LocalFunctionIteratorInterface
//
//! Interface for iterators to iterate over local functions 
//
//**************************************************************************
template < class DiscFunctionSpaceType , class
LocalFunctionIteratorImp , class LocalFunctionImp > 
class LocalFunctionIteratorInterface
{
public:
  //! know the object typ for iteration
  typedef LocalFunctionImp LocalFunctionType;
  
  //! return reference to local function 
  LocalFunctionType & operator *() 
  { 
    return localFunc_; 
  };
 
  //! go next local function 
  LocalFunctionIteratorImp& operator++ () 
  {
    return asImp(). operator ++ ();
  };

  //! go next i steps   
  LocalFunctionIteratorImp& operator++ (int i) 
  {
    return asImp().operator ++ ( i );
  };
 
  //! compare with other iterators 
  bool operator == (const LocalFunctionIteratorImp & I )
  {
    return asImp().operator == (I);
  }
  
  //! compare with other iterators 
  bool operator != (const LocalFunctionIteratorImp & I )
  {
    return asImp().operator != (I);
  }

private:
  //! Barton-Nackman trick 
  LocalFunctionIteratorImp & asImp() 
  { 
    return static_cast<LocalFunctionIteratorImp&>(*this);
  }
  const LocalFunctionIteratorImp &asImp() const  
  {
    return static_cast<const LocalFunctionIteratorImp&>(*this);
  }
}; // end class LocalFunctionIteratorInterface 


//*************************************************************************
//
//  --LocalFunctionIteratorDefault 
//  
//! Defaultimplementation  
//
//*************************************************************************
template < class DiscFunctionSpaceType , class
LocalFunctionIteratorImp , class LocalFunctionImp > 
class LocalFunctionIteratorDefault 
: public LocalFunctionIteratorInterface < DiscFunctionSpaceType ,
  LocalFunctionIteratorImp , LocalFunctionImp > 
{
public:
  // notin' 
}; // end class LocalFunctionIteratorDefault 


} // end namespace Dune 

#endif
