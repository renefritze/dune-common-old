#ifndef __DUNE_DFADAPT_HH__
#define __DUNE_DFADAPT_HH__

#include "dune/common/array.hh"
#include "common/discretefunction.hh"
#include "common/fastbase.hh"
#include "common/localfunction.hh"
#include "common/dofiterator.hh"

#include <fstream>
#include <rpc/xdr.h>

namespace Dune{

template <class DiscreteFunctionSpaceType >    class LocalFunctionAdapt;
template <class DiscreteFunctionType, class GridIteratorType >
                                               class LocalFunctionAdaptIterator;
template <class DofType, class DofArrayType >  class DofIteratorAdapt;

//! defined in dofmanager.hh
template <class T>                             class DofArray;

//**********************************************************************
//
//  --DFAdapt 
//
//! this is one special implementation of a discrete function using an
//! array for storing the dofs.  
//!
//**********************************************************************
template<class DiscreteFunctionSpaceType>
class DFAdapt 
: public DiscreteFunctionDefault < DiscreteFunctionSpaceType, 
           DofIteratorAdapt < typename DiscreteFunctionSpaceType::RangeField ,
           DofArray< typename DiscreteFunctionSpaceType::RangeField > > , 
           LocalFunctionAdaptIterator ,
           DFAdapt <DiscreteFunctionSpaceType> > 
{
public:
  typedef typename DiscreteFunctionSpaceType::RangeField RangeFieldType;
  typedef DofArray< RangeFieldType > DofArrayType;

private:
  typedef DiscreteFunctionDefault < DiscreteFunctionSpaceType,
            DofIteratorAdapt < typename DiscreteFunctionSpaceType::RangeField,DofArrayType  > ,
            LocalFunctionAdaptIterator ,
            DFAdapt <DiscreteFunctionSpaceType > >
  DiscreteFunctionDefaultType;

  enum { myId_ = 0};
  
public:
  typedef DofIteratorAdapt<typename DiscreteFunctionSpaceType::RangeField,
          DofArrayType > DofIteratorType;

  typedef typename DiscreteFunctionSpaceType::MemObjectType MemObjectType;
  
  typedef DFAdapt <DiscreteFunctionSpaceType> DiscreteFunctionType;          
  typedef LocalFunctionAdapt < DiscreteFunctionSpaceType > LocalFunctionType;

  typedef DiscreteFunctionSpaceType FunctionSpaceType;
  
  //! Constructor make empty DFAdapt 
  DFAdapt (const char * name, DiscreteFunctionSpaceType & f );
       
  //! Constructor make Discrete Function for all or leaf level  
  DFAdapt (const char * name, DiscreteFunctionSpaceType & f, 
                 int level , int codim , bool leaf ) ;
  
  //! Constructor make Discrete Function for all or leaf level  
  DFAdapt (const DFAdapt <DiscreteFunctionSpaceType> & df); 

  //! delete stack of free local functions belonging to this discrete
  //! function 
  ~DFAdapt ();
 
  DiscreteFunctionType & argument    () { return *this; }
  const DiscreteFunctionType & argument () const { return *this; }
  DiscreteFunctionType & destination () { return *this; }
        
  // ***********  Interface  *************************
  //! return object of type LocalFunctionType 
  LocalFunctionAdapt<DiscreteFunctionSpaceType> newLocalFunction ( );
 
  //! update LocalFunction to given Entity en  
  template <class EntityType> 
  void localFunction ( EntityType &en, 
  LocalFunctionAdapt<DiscreteFunctionSpaceType> & lf); 

  // we use the default implementation 
  DofIteratorType dbegin ( int level );
  
  //! points behind the last dof of type cc
  DofIteratorType dend   ( int level );

  // the const versions 
  // we use the default implementation 
  const DofIteratorType dbegin ( int level ) const;
  
  //! points behind the last dof of type cc
  const DofIteratorType dend   ( int level ) const;

  //! return if allLevels are used 
  bool allLevels () { return true; }

  //! set all dofs to zero  
  void clearLevel( int level );
  void clear( );

  //! set all dof to value x 
  void set( RangeFieldType x ); 
  void setLevel( RangeFieldType x, int level ); 

  void addScaled (int level, const DFAdapt <DiscreteFunctionSpaceType> & g,
      const RangeFieldType &scalar); 
  
  void addScaled (const DFAdapt <DiscreteFunctionSpaceType> & g,
      const RangeFieldType &scalar); 
  
  template <class EntityType>
  void addScaledLocal (EntityType &en, 
      const DFAdapt <DiscreteFunctionSpaceType> & g,
      const RangeFieldType &scalar); 
 
  //! add g to this on local entity
  template <class EntityType>
  void addLocal (EntityType &it, 
      const DFAdapt <DiscreteFunctionSpaceType> & g); 
  
  //! add g to this on local entity
  template <class EntityType>
  void substractLocal (EntityType &it, 
      const DFAdapt <DiscreteFunctionSpaceType> & g); 
  
  template <class EntityType>
  void setLocal (EntityType &it, const RangeFieldType &scalar);
  
  //! print all dofs 
  void print(std::ostream& s, int level); 

  //! write leaf data to file in USPM format for Grape 
  bool write_USPM(const char *filename , int timestep);

  //! write data of discrete function to file filename|timestep 
  //! with xdr methods 
  bool write_xdr( const char *filename , int timestep );

  //! write data of discrete function to file filename|timestep 
  //! with xdr methods 
  bool read_xdr( const char *filename , int timestep );

  //! write function data to file filename|timestep in ascii Format
  bool write_ascii(const char *filename, int timestep);

  //! read function data from file filename|timestep in ascii Format
  bool read_ascii(const char *filename, int timestep);

  //! write function data in pgm fromat file
  bool write_pgm(const char *filename, int timestep) ;

  //! read function data from pgm fromat file
  bool read_pgm(const char *filename, int timestep); 

  //! return name of this discrete function
  const char * name () const { return name_; }

  //! return to MemObject which holds the memory of this discrete fucntion
  MemObjectType & memObj() { return memObj_; }
  
private:  
  // name of this func
  const char * name_;

  // MemObject that manages the memory for the dofs of this function
  MemObjectType & memObj_;
  
  typedef typename DiscreteFunctionSpaceType::GridType GridType;

  //! array containing the dof of this function, see dofmanager.hh
  DofArrayType dofVec_;

  //! hold one object for addLocal and setLocal and so on 
  LocalFunctionType localFunc_;
}; // end class DFAdapt 


//**************************************************************************
//
//  --LocalFunctionAdapt
//
//! Implementation of the local functions 
//
//**************************************************************************
template < class DiscreteFunctionSpaceType > 
class LocalFunctionAdapt 
: public LocalFunctionDefault <DiscreteFunctionSpaceType ,
  LocalFunctionAdapt < DiscreteFunctionSpaceType >  >
{
  typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;
  typedef LocalFunctionAdapt < DiscreteFunctionSpaceType > MyType;
  typedef DFAdapt <DiscreteFunctionSpaceType> DiscFuncType;

  enum { dimrange = DiscreteFunctionSpaceType::DimRange };

  friend class DFAdapt <DiscreteFunctionSpaceType>;
  typedef typename DiscreteFunctionSpaceType::RangeField RangeFieldType;
  typedef typename DiscreteFunctionSpaceType::Domain DomainType;
  typedef typename DiscreteFunctionSpaceType::Range  RangeType;
  typedef typename DiscreteFunctionSpaceType::JacobianRange JacobianRangeType;

  typedef typename DiscFuncType::DofArrayType DofArrayType;
public:
  //! Constructor 
  LocalFunctionAdapt ( const DiscreteFunctionSpaceType &f , 
           DofArrayType & dofVec );

  //! Destructor 
  ~LocalFunctionAdapt ();

  //! access to dof number num, all dofs of the dof entity
  RangeFieldType & operator [] (int num);
  
  //! access to dof number num, all dofs of the dof entity
  const RangeFieldType & read (int num) const;

  //! return number of degrees of freedom 
  int numberOfDofs () const;

  //! sum over all local base functions 
  template <class EntityType> 
  void evaluate (EntityType &en, const DomainType & x, RangeType & ret) const ;
  
  //! sum over all local base functions evaluated on given quadrature point
  template <class EntityType, class QuadratureType> 
  void evaluate (EntityType &en, QuadratureType &quad, int quadPoint , RangeType & ret) const;
  
  //! sum over all local base functions evaluated on given quadrature point
  template <class EntityType, class QuadratureType> 
  void jacobian (EntityType &en, QuadratureType &quad, int quadPoint , JacobianRangeType & ret) const;
  
protected:
  //! update local function for given Entity  
  template <class EntityType > bool init ( EntityType &en ) const;

  //! needed once 
  mutable RangeType tmp_;
  mutable DomainType xtmp_;

  //! needed once 
  mutable JacobianRangeType tmpGrad_;

  //! diffVar for evaluate, is empty 
  const DiffVariable<0>::Type diffVar;

  //! number of all dofs 
  mutable int numOfDof_;

  //! for example number of corners for linear elements 
  mutable int numOfDifferentDofs_;
 
  //! the corresponding function space which provides the base function set
  const DiscreteFunctionSpaceType &fSpace_;
  
  //! Array holding pointers to the local dofs 
  mutable Array < RangeFieldType * > values_;

  //! dofVec from all levels of the discrete function 
  DofArrayType & dofVec_;

  //! the corresponding base function set 
  mutable const BaseFunctionSetType *baseFuncSet_;
  
  //! do we have the same base function set for all elements
  bool uniform_;
}; // end LocalFunctionAdapt 


//***********************************************************************
//
//  --DofIteratorAdapt
//
//***********************************************************************
template < class DofType, class DofArrayType >
class DofIteratorAdapt : public
DofIteratorDefault < DofType , DofIteratorAdapt < DofType, DofArrayType > >
{
  typedef DofIteratorAdapt<DofType,DofArrayType> MyType;
public:
  typedef DofType T;
  
  DofIteratorAdapt ( DofArrayType & dofArray , int count )
    :  dofArray_ ( dofArray ) , constArray_ (dofArray) , count_ ( count ) {};
  
  DofIteratorAdapt ( const DofArrayType & dofArray , int count )
    :  dofArray_ ( const_cast <DofArrayType &> (dofArray) ) ,
       constArray_ ( dofArray ) , 
       count_ ( count ) {};

  //! return dof
  DofType & operator *();

  //! return dof read only 
  const DofType & operator * () const;

  //! go next dof
  MyType & operator++ ();
  
  //! go next dof
  const MyType & operator++ () const;

  //! random access 
  DofType& operator[] (int i);

  //! random access read only 
  const DofType& operator [] (int i) const;

  //! compare
  bool operator == (const MyType & I ) const;

  //! compare 
  bool operator != (const MyType & I ) const; 

  //! return actual index 
  int index () const; 

  //! set dof iterator back to begin , for const and not const Iterators
  void reset () const;

  DofType * vector() { return dofArray_.vector(); }
  const DofType * vector() const { return dofArray_.vector(); }
  
private: 
  //! the array holding the dofs 
  DofArrayType &dofArray_;  
  
  //! the array holding the dofs , only const reference
  const DofArrayType &constArray_;  
  
  //! index 
  mutable int count_;
}; // end DofIteratorAdapt 

} // end namespace Dune

#include "discfuncarray/dfadapt.cc"

#endif


