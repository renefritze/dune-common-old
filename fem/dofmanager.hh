#ifndef DUNE_DOFMANAGER_HH
#define DUNE_DOFMANAGER_HH

//- System includes 
#include <cassert>
#include <vector> 
#include <string>
#include <list>

//- Dune includes 
#include <dune/common/dlist.hh>
#include <dune/common/stdstreams.hh>
#include <dune/common/exceptions.hh>
#include <dune/common/genericiterator.hh>
#include <dune/common/interfaces.hh>

#include <dune/fem/common/dofmapperinterface.hh>

// here are the default grid index set defined 
#include <dune/grid/common/defaultindexsets.hh>
#include <dune/fem/transfer/datacollector.hh>

namespace Dune {

// forward declaration 
template <class GridType> class DofManager;
template <class DofManagerImp> class DofManagerFactory;


//! oriented to the STL Allocator funtionality 
template <class T>
class DefaultDofAllocator {
public:
  //! allocate array of nmemb objects of type T
  static T* malloc (size_t nmemb)
  {
    T* p = new T[nmemb];
    return p;
  }

  //! release memory previously allocated with malloc member
  static void free (T* p)
  {
    delete [] p;
  }
  
  //! allocate array of nmemb objects of type T
  static T* realloc (T* oldMem, size_t oldSize , size_t nmemb)
  {
    T* p = new T[nmemb];
    std::memcpy(p,oldMem,oldSize * sizeof(T));
    DefaultDofAllocator :: free (oldMem);
    return p;
  }
};

//! allocator for simple structures like int, double and float
//! using the C malloc,free, and realloc 
struct SimpleDofAllocator 
{
  //! allocate array of nmemb objects of type T
  template <typename T>
  static T* malloc (size_t nmemb)
  {
    T* p = (T *) std::malloc(nmemb * sizeof(T));
    assert(p);
    return p;
  }

  //! release memory previously allocated with malloc member
  template <typename T>
  static void free (T* p)
  {
    assert(p);
    std::free(p);
  }
  
  //! allocate array of nmemb objects of type T
  template <typename T>
  static T* realloc (T* oldMem, size_t oldSize , size_t nmemb)
  {
    assert(oldMem);
    T * p = (T *) std::realloc(oldMem , nmemb*sizeof(T));
    assert(p);
    return p;
  }
};

template <>
class DefaultDofAllocator<double> 
{
  typedef double T;
public:
  //! allocate array of nmemb objects of type T
  static T* malloc (size_t nmemb)
  {
    return SimpleDofAllocator::malloc<T> (nmemb);
  }

  //! release memory previously allocated with malloc member
  static void free (T* p)
  {
    SimpleDofAllocator::free<T> (p);
    return ;
  }
  
  //! allocate array of nmemb objects of type T
  static T* realloc (T* oldMem, size_t oldSize , size_t nmemb)
  {
    return SimpleDofAllocator::realloc<T> (oldMem,oldSize,nmemb);
  }
};

template <>
class DefaultDofAllocator<int> 
{
  typedef int T;
public:
  //! allocate array of nmemb objects of type T
  static T* malloc (size_t nmemb)
  {
    return SimpleDofAllocator::malloc<T> (nmemb);
  }

  //! release memory previously allocated with malloc member
  static void free (T* p)
  {
    SimpleDofAllocator::free<T> (p);
    return ;
  }
  
  //! allocate array of nmemb objects of type T
  static T* realloc (T* oldMem, size_t oldSize , size_t nmemb)
  {
    return SimpleDofAllocator::realloc<T> (oldMem,oldSize,nmemb);
  }
};


template <class A, class B> 
A * convertToValueType (B * b)
{
  return const_cast<A *> (b);
}

/*! 
 DofArray is the array that a discrete functions sees. If a discrete
 function is created, then it is signed in by the function space and the
 return value is a MemObject. This MemObject contains a DofArrayMemory
 which is then as reference given to the DofArray of the DiscreteFunction. 
 The DofArray is only a wrapper class for DofArrayMemory where we dont know
 the type of the dofs only the size of one dof. 
 Therefore we have this wrapper class for cast to the right type.
*/
template <class T, class AllocatorType = DefaultDofAllocator<T> >
class DofArray 
{
private:
  typedef DofArray<T, AllocatorType> ThisType;

  // size of array 
  int size_;

  int memSize_;
 
  // pointer to mem
  T * vec_;

  // true if this class allocated the vector 
  bool myProperty_; 
  
public:
  //! definition conforming to STL  
  typedef T value_type;
  
  //! DofIterator
  typedef GenericIterator<ThisType, T> DofIteratorType;
  
  //! Const DofIterator
  typedef GenericIterator<const ThisType, const T> ConstDofIteratorType;

  //! create array of length size
  DofArray(int size) 
    : size_(size) , memSize_(size) , vec_(0) , myProperty_ (true)
  {
    vec_ = AllocatorType :: malloc (size_);
    assert( vec_ );
  }
  
  //! create array with given memory vector aof length size
  //! vector must be of the type of T
  //! only const may be casted away 
  template <class VectorPointerType> 
  DofArray(int size, VectorPointerType * vector ) 
    : size_(size) , memSize_(size) , vec_(const_cast<T *>(vector)) , myProperty_ (false)
  {
    assert( vec_ );
  }
  
  //! Destructor 
  ~DofArray() {
    if( vec_ && myProperty_ ) AllocatorType :: free ( vec_ );
  }

  DofIteratorType begin() {
    return DofIteratorType(*this, 0);
  }
  
  ConstDofIteratorType begin() const {    
    return ConstDofIteratorType(*this, 0);
  }
  
  DofIteratorType end() {
    return DofIteratorType(*this, size_);
  }
  
  ConstDofIteratorType end() const {    
    return ConstDofIteratorType(*this, size_);
  }

  //! return number of enties of array 
  int size () const { return size_; }  

  //! return reference to entry i
  T&       operator [] ( int i )       
  { 
    assert( ((i<0) || (i>=size()) ? (std::cout << std::endl << i << " i|size " << size() << 
#ifdef _ALU3DGRID_PARALLEL_
                                     //            " on p=" << __MyRank__ << 
#endif
            std::endl, 0) : 1));
    return vec_[i]; 
  }
  
  //! return reference to const entry i
  const T& operator [] ( int i ) const 
  { 
    //assert( ((i<0) || (i>=size()) ? (std::cout << std::endl << i << " i|size " << size() << std::endl, 0) : 1));
    return vec_[i]; 
  }

  //! assign arrays 
  DofArray<T>& operator= (const DofArray<T> &copy)
  {
    assert(copy.size_ >= size_);
    std::memcpy(vec_,copy.vec_, size_ * sizeof(T));
    return *this;
  }
 
  //! operator = assign all entrys with value t 
  DofArray<T>& operator= (const T t)
  {
    for(int i=0; i<size(); i++) this->operator [] (i) = t;
    return *this;
  }

  //! Comparison operator
  //! The comparison operator checks for object identity, i.e. if this and
  //! other are the same objects in memory rather than containing the same data
  bool operator==(const DofArray<T>& other) const {
    return vec_ == other.vec_;
  }

  //! return leak pointer for usage in BLAS routines 
  T* leakPointer() { return vec_; }
  //! return leak pointer for usage in BLAS routines 
  const T* leakPointer() const { return vec_; }

  //! read and write xdr 
  bool processXdr(XDR *xdrs)
  {
    if(xdrs != 0)
    {
      int len = size_;
      xdr_int( xdrs, &len );
      assert(size_ <= len);

      xdr_vector(xdrs,(char *) vec_,size_, sizeof(T) ,(xdrproc_t)xdr_double);
      return true;
    }
    else
      return false;
  } 

  void realloc ( int nsize )
  {
    assert(myProperty_);
    assert(nsize >= 0);

    if(!myProperty_) 
      DUNE_THROW(OutOfMemoryError,"Not my property to change mem size!");

    if(nsize <= memSize_) 
    {
      size_ = nsize;
      return ;
    }

    // nsize is the minimum needed size of the vector 
    // we double this size to reserve some memory and minimize
    // reallocations 
    int nMemSize = 2 * nsize; 
    vec_ = AllocatorType :: realloc (vec_,size_,nMemSize);

    size_ = nsize;
    memSize_ = nMemSize;
  }
};

//! specialisation for int 
template <>
inline bool DofArray<int>::processXdr(XDR *xdrs)
{
  typedef int T;
  if(xdrs != 0)
  {
    int len = size_;
    xdr_int( xdrs, &len );
    assert(size_ <= len);
    xdr_vector(xdrs,(char *) vec_,size_, sizeof(T) ,(xdrproc_t)xdr_int);
    return true;
  }
  else
    return false;
}

//! specialisation for double 
template <>
inline bool DofArray<double>::processXdr(XDR *xdrs)
{
  typedef double T;
  
  if(xdrs != 0)
  {
    int len = size_;
    xdr_int( xdrs, &len );
    assert( (size_ > len) ? (std::cout << size_ << " s|l " << len << "\n" ,0 ): 1);

    xdr_vector(xdrs,(char *) vec_,size_, sizeof(T) ,(xdrproc_t)xdr_double);
    return true;
  }
  else
    return false;
}
//******************************************************************
//
//  IndexSetObject
/*! The idea of the IndexSetObject is that, every MemObject has an
 *  IndexSetObject, but if two different MemObjects belong to the same
 *  funtion space, then they have the same IndexSetObject. 
 *  Furthermore the IndexSetObject is more or less a wrapper for the
 *  IndexSetInterface, but here we can store aditional infomation, for
 *  example if set has been compressed. 
 */
//******************************************************************

/*! this class is the virtual interface for the index sets added to the
  dofmanager. The derived classed are of the type IndexSetObj<IndexSet>.
  This means we don't have to inherit every index set we want to use with
  this DofManager. 
  */
class IndexSetObjectInterface 
{
public:
  virtual ~IndexSetObjectInterface () {}
  // resize of index set 
  virtual void resize () = 0; 
  // compress of index set 
  virtual bool compress () = 0;
  // returns true if set generally needs a compress 
  virtual bool needsCompress () const = 0;
  // return address of index set 
  virtual void * address () const = 0;

  // read and write method of index sets 
  virtual void read_xdr(const char * filename, int timestep) = 0;
  virtual void write_xdr(const char * filename, int timestep) const = 0;
};

template <class IndexSetType, class EntityType> class RemoveIndicesFromSet;
template <class IndexSetType, class EntityType> class InsertIndicesToSet;

template <class IndexSetType, class EntityType>
class IndexSetObject : public IndexSetObjectInterface ,
        public LocalInlinePlus < IndexSetObject<IndexSetType,EntityType> , EntityType >
{
private:
  // the dof set stores number of dofs on entity for each codim
  IndexSetType & indexSet_;

  // insertion and removal of indices 
  InsertIndicesToSet   <IndexSetType,EntityType> insertIdxObj_;
  RemoveIndicesFromSet <IndexSetType,EntityType> removeIdxObj_;

public:  
  // Constructor of MemObject, only to call from DofManager 
  IndexSetObject ( IndexSetType & iset ) : indexSet_ (iset) 
   , insertIdxObj_(indexSet_), removeIdxObj_(indexSet_) {} 

  //! wrap resize of index set 
  void resize () 
  {
    indexSet_.resize();
  }
  
  //! wrap compress of index set 
  bool compress () 
  { 
    return indexSet_.compress(); 
  }

  //! returns whether the set needs a compress after adaptation 
  bool needsCompress () const 
  {
    return indexSet_.needsCompress (); 
  }

  //! return address of index set 
  void * address() const 
  {
    return (void *)&indexSet_;
  }

  //! call read_xdr of index set 
  virtual void read_xdr(const char * filename, int timestep)
  {
    indexSet_.read_xdr(filename,timestep); 
  }
  
  //! call write_xdr of index set 
  virtual void write_xdr(const char * filename, int timestep) const
  {
    indexSet_.write_xdr(filename,timestep);
  }
   
  //! return reference to insertObj
  InsertIndicesToSet<IndexSetType,EntityType> & insertIndexObj() 
  {
    return insertIdxObj_; 
  }
  
  //! return reference to removeObj
  RemoveIndicesFromSet<IndexSetType,EntityType> & removeIndexObj() 
  {
    return removeIdxObj_;    
  }
};

//****************************************************************
//
// MemObject 
//
//****************************************************************
//! interface of MemObjects to store for DofManager 
class MemObjectInterface 
{
public:
  virtual ~MemObjectInterface() {};
  
  //! reallocate memory 
  virtual void realloc () = 0;
  //! reallocate memory 
  virtual void realloc (int newSize) = 0;
  //! size of space, i.e. mapper.size()
  virtual int size () const = 0;
  //! additional size needed for restrict 
  virtual int additionalSizeEstimate () const = 0;
  //! new size of space, i.e. after grid adaptation
  virtual int newSize () const = 0;
  //! returns name of obj
  virtual const char * name () const  = 0;
  //! compressed the underlying dof vector 
  virtual void dofCompress () = 0;

  //! returns true if resize ids needed 
  virtual bool resizeNeeded () const = 0;
  //! returns size of memory per element
  virtual int  elementMemory() const = 0;
};


template <class MemObjectType> class CheckMemObjectResize;
template <class MemObjectType> class ResizeMemoryObjects;

/*! 
  A MemObject holds the memory for one DiscreteFunction and the
  corresponding DofArrayMemory. If a DiscreteFunction is signed in by a
  function space, then such a MemObject is created by the DofManager. 
  The MemObject also knows the DofMapper from the function space which the
  discrete function belongs to. Here we dont know the exact type of the dof
  mapper therefore the methods newSize and calcInsertPoints of the mappers
  have to be virtual. This isnt a problem because this methods should only
  be called during memory reorganizing which is only once per timestep. 
*/
template <class MapperType , class DofArrayType>
class MemObject : public MemObjectInterface
{
private:
  typedef MemObject < MapperType , DofArrayType> MemObjectType;
  
  // the dof set stores number of dofs on entity for each codim
  const MapperType & mapper_;

  // Array which the dofs are stored in 
  DofArrayType array_;

  // name of mem object, i.e. name of discrete function 
  std::string name_;

  CheckMemObjectResize < MemObjectType > checkResize_; 
  ResizeMemoryObjects  < MemObjectType > resizeMemObj_; 

public:  
  // Constructor of MemObject, only to call from DofManager 
  MemObject ( const MapperType & mapper, std::string name ) 
    : mapper_ (mapper) , array_( mapper_.size() ), name_ (name) , 
      checkResize_(*this) , resizeMemObj_(*this) {} 

  //! returns name of this vector 
  const char * name () const { return name_.c_str(); }

  //! if grid changed, then calulate new size of dofset 
  int newSize () const { return mapper_.newSize(); }  

  //! return size of underlying array 
  int size () const { return array_.size(); }

  //! return true if array needs resize 
  bool resizeNeeded () const 
  {
    return (size() < newSize());  
  }

  //! return number of dofs on one element 
  int elementMemory () const 
  {
    return mapper_.numDofs();
  }

  //! return number of entities  
  int additionalSizeEstimate () const 
  { 
    return mapper_.additionalSizeEstimate(); 
  }

  //! reallocate the memory with the new size 
  void realloc () 
  {
    array_.realloc( newSize() );
  }

  //! reallocate the memory with the new size 
  void realloc ( int nSize ) 
  {
    array_.realloc( nSize );
  }

  //! copy the dof from the rear section of the vector to the holes 
  void dofCompress () 
  {
    if( mapper_.needsCompress() )
    {
      // run over all holes and copy array vules to new place 
      const int holes = mapper_.numberOfHoles(); 
      for(int i=0; i<holes; ++i)
      {
        // copy value 
        array_[ mapper_.newIndex(i) ] 
          = array_[ mapper_.oldIndex(i) ];
      }
    }

    // store new size, which is should be smaller then actual size 
    array_.realloc ( newSize() );
  }
 
  //! return object that checks for resize
  CheckMemObjectResize < MemObjectType > & checkResizeObj () 
  { 
    return checkResize_; 
  }

  //! return object that makes the resize 
  ResizeMemoryObjects < MemObjectType > & resizeMemObject() 
  {
    return resizeMemObj_;  
  }

  //! return reference to array for DiscreteFunction 
  DofArrayType & getArray() { return array_; } 
};


/*! 
 * Same as MemObject but doing nothing, because the memory comes from
 * outside. This is for using packages like LAPACK and other with discrete
 * functions. 
*/
template <class MapperType , class DofArrayType, class VectorPointerType>
class DummyMemObject : public MemObjectInterface
{
private:
  typedef DummyMemObject < MapperType , DofArrayType, VectorPointerType > MemObjectType;
  
  // the dof set stores number of dofs on entity for each codim
  const MapperType & mapper_;

  // Array which the dofs are stored in 
  DofArrayType array_;

  // name of mem object, i.e. name of discrete function 
  std::string name_;

  // vector represented by array 
  VectorPointerType * vector_;

  CheckMemObjectResize < MemObjectType > checkResize_; 
  ResizeMemoryObjects  < MemObjectType > resizeMemObj_; 

public:  
  // Constructor of MemObject, only to call from DofManager 
  DummyMemObject ( const MapperType & mapper, 
                   std::string name , VectorPointerType * vector ) 
    : mapper_ (mapper) , array_( mapper_.size() , vector ), name_ (name) 
    , vector_ (vector) , checkResize_(*this) , resizeMemObj_(*this) 
    {
      assert( vector_ );
    } 

  //! returns name of this vector 
  const char * name () const { return name_.c_str(); }

  //! if grid changed, then calulate new size of dofset 
  int newSize () const { return mapper_.newSize(); }  

  //! return size of underlying array 
  int size () const { return array_.size(); }

  //! return true if array needs resize 
  bool resizeNeeded () const 
  {
    assert( (size() != newSize()) ? 
        (std::cerr << "WARNING: DummyMemObject's vector is not up to date! \n" , 0) : 1);
    return false; 
  }

  //! return number of dofs on one element 
  int elementMemory () const 
  {
    return mapper_.numDofs();
  }

  //! return number of entities  
  int additionalSizeEstimate () const 
  { 
    return mapper_.additionalSizeEstimate(); 
  }

  //! reallocate the memory with the new size 
  void realloc () 
  {
    assert( (size() != newSize()) ? 
        (std::cerr << "WARNING: DummyMemObject's may not resize vectors! \n" , 1) : 1);
  }

  //! reallocate the memory with the new size 
  void realloc ( int nSize ) 
  {
    assert( (size() != newSize()) ? 
        (std::cerr << "WARNING: DummyMemObject's may not resize vectors! \n" , 1) : 1);
  }

  //! copy the dof from the rear section of the vector to the holes 
  void dofCompress () 
  {
    assert( (size() != newSize()) ? 
        (std::cerr << "WARNING: DummyMemObject's may not compress vectors! \n" , 1) : 1);
  }
 
  //! return object that checks for resize
  CheckMemObjectResize < MemObjectType > & checkResizeObj () 
  { 
    return checkResize_; 
  }

  //! return object that makes the resize 
  ResizeMemoryObjects < MemObjectType > & resizeMemObject() 
  {
    return resizeMemObj_;  
  }

  //! return reference to array for DiscreteFunction 
  DofArrayType & getArray() { return array_; } 
};

template <class IndexSetType, class EntityType>
class RemoveIndicesFromSet 
: public LocalInlinePlus < RemoveIndicesFromSet<IndexSetType,EntityType> , EntityType >
{
private:
  // the dof set stores number of dofs on entity for each codim
  IndexSetType & indexSet_;

public:  
  // Constructor of MemObject, only to call from DofManager 
  RemoveIndicesFromSet ( IndexSetType & iset ) : indexSet_ (iset) {} 

  //! apply wraps the removeOldIndex Method of the index set 
  void apply ( EntityType & en )
  {
    indexSet_.removeOldIndex( en );
  }
};

template <class IndexSetType, class EntityType>
class InsertIndicesToSet 
: public LocalInlinePlus < InsertIndicesToSet<IndexSetType,EntityType> , EntityType >
{
private:
  // the dof set stores number of dofs on entity for each codim
  IndexSetType & indexSet_;

public:  
  // Constructor of MemObject, only to call from DofManager 
  InsertIndicesToSet ( IndexSetType & iset ) : indexSet_ (iset) {} 

  //! apply wraps the insertNewIndex method of the index set
  void apply ( EntityType & en )
  {
    indexSet_.insertNewIndex( en );
  }
};

template <class MemObjectType> 
class CheckMemObjectResize 
: public LocalInlinePlus < CheckMemObjectResize < MemObjectType > , int > 
{
private:
  // the dof set stores number of dofs on entity for each codim
  const MemObjectType & memobj_;

public:  
  // Constructor of MemObject, only to call from DofManager 
  CheckMemObjectResize ( const MemObjectType & mo ) : memobj_ (mo) {} 

  // if resize is needed the check is set to true 
  void apply ( int & check )
  {
    if( memobj_.resizeNeeded() ) check = 1; 
  }
};

// this class is the object for a single MemObject to 
template <class MemObjectType> 
class ResizeMemoryObjects  
: public LocalInlinePlus < ResizeMemoryObjects < MemObjectType > , int > 
{
private:
  // the dof set stores number of dofs on entity for each codim
  MemObjectType & memobj_;

public:  
  // Constructor of MemObject, only to call from DofManager 
  ResizeMemoryObjects ( MemObjectType & mo ) : memobj_ (mo) {} 

  // resize with own size plus chunksize 
  void apply ( int & nsize )
  {
    memobj_.realloc ( memobj_.size() + memobj_.elementMemory() * nsize );  
  }
};

// this is the dofmanagers object which is being used during restriction 
// and prolongation process for adding and removing indices to and from
// index sets which belong to functions that belong to that dofmanager
template <class DofManagerType , class RestrictProlongIndexSetType> 
class IndexSetRestrictProlong 
{
  DofManagerType & dm_;
  
  RestrictProlongIndexSetType & insert_;
  RestrictProlongIndexSetType & remove_;
public: 
  
  IndexSetRestrictProlong ( DofManagerType & dm , RestrictProlongIndexSetType & is, RestrictProlongIndexSetType & rm ) 
    : dm_(dm) , insert_(is), remove_(rm) {}

  // just for interface reasons 
  template <class EntityType>
  void calcFatherChildWeight (EntityType &father, EntityType &son) const
  {
  }

  //! restrict data to father 
  template <class EntityType>
  void restrictLocal ( EntityType & father, EntityType & son , bool initialize ) const
  {
    insert_.apply( father );
    remove_.apply( son );
    dm_.checkMemorySize();
  }  

  //! prolong data to children 
  template <class EntityType>
  void prolongLocal ( EntityType & father, EntityType & son , bool initialize ) const
  {
    remove_.apply( father );
    insert_.apply( son );
    dm_.checkMemorySize();
  }
  
};

class DofManError : public Exception {};

/*! 
 The DofManager is responsible for managing memory allocation and freeing
 for all discrete functions living on the grid the manager belongs to. 
 There is only one DofManager per grid.
 Each discrete function knows its dofmanager and can sign in. 
 If the grid is adapted, then the
 dofmanager reorganizes the memory if necessary. The DofManager holds a
 list of MemObjects which manage the memory and the corresponding
 mapper so they can determine the size of new memory. 
 Furthermore the DofManager holds an IndexSet which the DofMapper needs for
 calculating the indices in the dof vector for a given entity and local dof
 number. This IndexSet is delivered to the mapper when a function space is
 created. The default value for the IndexSet is the DefaultIndexSet class
 which is mostly a wrapper for the grid indices. 
*/
template <class GridImp> 
class DofManager 
{
public:  
  //! type of Grid this DofManager belongs to 
  typedef GridImp GridType;

private:  
  typedef DofManager<GridType> MyType;
  friend class DofManagerFactory<MyType>;
public:
  typedef typename GridObjectStreamOrDefault<
    GridType, DummyObjectStream>::ObjectStreamType ObjectStreamType;

  typedef DataCollectorInterface<GridType, ObjectStreamType> DataCollectorType;

private:  
  typedef std::list<MemObjectInterface*> ListType;
  typedef typename ListType::iterator ListIteratorType;

  typedef LocalInterface< int > MemObjectCheckType;
  
  typedef std::list<IndexSetObjectInterface * > IndexListType;
  typedef typename IndexListType::iterator IndexListIteratorType;
  typedef typename IndexListType::const_iterator ConstIndexListIteratorType;
  
  // list with MemObjects, for each DiscreteFunction we have one MemObject
  ListType memList_;

  // list of all different indexsets 
  IndexListType indexList_;

  // the dofmanager belong to one grid only 
  const GridType & grid_;

  // index set for mapping 
  mutable DataCollectorType dataInliner_;
  mutable DataCollectorType dataXtractor_;
 
  typedef typename DataCollectorType::LocalInterfaceType LocalDataCollectorType;
  mutable LocalDataCollectorType dataWriter_;
  mutable LocalDataCollectorType dataReader_;

  typedef LocalInterface<typename GridType::
        template Codim<0>::Entity> LocalIndexSetObjectsType;

  mutable LocalIndexSetObjectsType indexSets_; 

  mutable LocalIndexSetObjectsType insertIndices_;
  mutable LocalIndexSetObjectsType removeIndices_;

  mutable MemObjectCheckType checkResize_;
  mutable MemObjectCheckType resizeMemObjs_;

  // variable which stores chunk size for actual problem 
  int chunkSize_; 

  //! if chunk size if small then defaultChunkSize is used 
  const int defaultChunkSize_; 
  
public: 
  typedef IndexSetRestrictProlong< MyType , LocalIndexSetObjectsType > IndexSetRestrictProlongType;
private:
  IndexSetRestrictProlongType indexRPop_; 
  //**********************************************************
  //**********************************************************
  //! Constructor 
  DofManager (const GridType & grid) 
    : grid_(grid) 
    , chunkSize_ (100)
    , defaultChunkSize_(100) 
    , indexRPop_( *this, insertIndices_ , removeIndices_ ) 
  {}
  
  //! Desctructor, removes all MemObjects and IndexSetObjects 
  ~DofManager (); 

public:
  template <class MapperType , class DofStorageType >
  struct Traits 
  {
    typedef MemObject< MapperType, DofStorageType > MemObjectType;
  };
  
public:
  //! add new index set to the list of the indexsets of this dofmanager
  template <class IndexSetType>
  inline void addIndexSet (const GridType &grid, IndexSetType &iset); 

  //! add new index set to the list of the indexsets of this dofmanager
  template <class IndexSetType>
  inline bool checkIndexSetExists (const IndexSetType &iset) const; 

  //! add dofset to dof manager 
  //! this method should be called at signIn of DiscreteFucntion, and there
  //! we know our DofStorage which is the actual DofArray  
  template <class DofStorageType, class MapperType >
  std::pair<MemObjectInterface*, DofStorageType*>
  addDofSet(const DofStorageType* ds, const MapperType& mapper, std::string name);

  //! add dofset to dof manager 
  //! this method should be called at signIn of DiscreteFucntion, and there
  //! we know our DofStorage which is the actual DofArray  
  template <class DofStorageType, class MapperType , class VectorPointerType >
  std::pair<MemObjectInterface*, DofStorageType*>
  addDummyDofSet(const DofStorageType* ds, const MapperType& mapper, std::string name, VectorPointerType * vec );

  //! remove MemObject, is called from DiscreteFucntionSpace at sign out of
  //! DiscreteFunction 
  bool removeDofSet (const MemObjectInterface & obj);

  //! returns the index set restrinction and prolongation operator
  IndexSetRestrictProlongType & indexSetRPop () 
  {
    return indexRPop_;
  }
   
  //! this method resizes the memory before restriction is done 
  void resizeForRestrict () 
  {
    ListIteratorType it    = memList_.begin();
    ListIteratorType endit = memList_.end();

    for( ; it != endit ; ++it)
    {
      int addSize = (*it)->additionalSizeEstimate();
      chunkSize_ = std::max( addSize , chunkSize_ );
      (*it)->realloc ( (*it)->size() + addSize );
    }
  }
  
  //! check if there is still enough memory 
  void checkMemorySize () 
  {
    // here it is necessary that this is fast, therefore we use 
    // the combined objects technique 
    assert(chunkSize_ > 0);
    int check = 0;
    checkResize_.apply( check );
    if( check ) resizeMemObjs_.apply ( chunkSize_ );
  }
 
  //! resize the memory and set chunk size to nsize 
  void reserveMemory (int nsize) 
  {
    // remember the chunksize 
    chunkSize_ = std::max(nsize, defaultChunkSize_ );
    assert( chunkSize_ > 0 );
    resizeMemObjs_.apply ( chunkSize_ );
  }

  //! resize indexsets memory due to what the mapper has as new size 
  void resize()
  {
    IndexListIteratorType endit = indexList_.end();
    for(IndexListIteratorType it = indexList_.begin(); it != endit; ++it)
    {
      (*it)->resize(); 
    }
    resizeDofMem();
  }

  //! inserts index to all index sets added to dof manager
  template <class EntityType>
  void insertNewIndex (EntityType & en )
  {
    insertIndices_.apply( en );
  }
          
  //! removes index to all index sets added to dof manager
  template <class EntityType>
  void removeOldIndex (EntityType & en )
  {
    removeIndices_.apply( en );
  }
 
private:
  //! resize the MemObject if necessary 
  void resizeDofMem()
  {
    ListIteratorType it    = memList_.begin();
    ListIteratorType endit = memList_.end();

    for( ; it != endit ; ++it)
    {
      int size  = (*it)->size();
      int nSize = (*it)->newSize();
      chunkSize_ = std::max( std::abs(nSize - size) , chunkSize_ );
      (*it)->realloc ( nSize );
    }
  }

public:
  //! compress all data that is hold by this dofmanager 
  void dofCompress() 
  {
    // compress indexsets first 
    {
      IndexListIteratorType endit  = indexList_.end();
      for(IndexListIteratorType it = indexList_.begin(); it != endit; ++it)
      {
        // reset compressed so the next time compress of index set is called 
        (*it)->compress(); 
      }
    }

    // compress all data now 
    {
      ListIteratorType endit  = memList_.end();
      for(ListIteratorType it = memList_.begin(); it != endit ; ++it)
      {
        // if correponding index was not compressed yet, this is called in
        // the MemObject dofCompress, if index has not changes, nothing happens  
        // if IndexSet actual needs  no compress, nothing happens to the
        // data either 
        // also data is resized, which means the vector is getting shorter
        (*it)->dofCompress () ;
      }
    }
  }

  //! add data handler for data inlining to dof manager
  template <class DataCollType>
  void addDataInliner ( DataCollType & d)
  {
    dataInliner_ += d;
  }

  //! add data handler for data xtracting to dof manager
  template <class DataCollType>
  void addDataXtractor ( DataCollType & d)
  {
    dataXtractor_ += d;
  }

  //! add data handler for data write to dof manager
  template <class DataCollType>
  void addDataWriter ( DataCollType & d)
  {
    dataWriter_ += d;
  }

  //! add data handler for data read to dof manager
  template <class DataCollType>
  void addDataReader ( DataCollType & d)
  {
    dataReader_ += d;
  }

  //! packs all data of this entity en and all child entities  
  template <class ObjectStreamType, class EntityType>
  void inlineData ( ObjectStreamType & str, EntityType & en )
  {
    dataInliner_.apply(str,en);
  }

  //! unpacks all data of this entity en and all child entities  
  template <class ObjectStreamType, class EntityType>
  void scatter ( ObjectStreamType & str, EntityType & en )
  {
    std::pair < ObjectStreamType * , const EntityType * > p (&str,&en);
    dataWriter_.apply( p );
  }

  //! packs all data of this entity to message buffer 
  template <class ObjectStreamType, class EntityType>
  void gather ( ObjectStreamType & str, EntityType & en )
  {
    std::pair < ObjectStreamType * , const EntityType * > p (&str,&en);
    dataReader_.apply( p );
  }
  
  //! unpacks all data of this entity from message buffer 
  template <class ObjectStreamType, class EntityType>
  void xtractData ( ObjectStreamType & str, EntityType & en )
  {
    // here the elements already have been created that means we can 
    // all resize and the memory is adapted
    //resize();
    dataXtractor_.apply(str,en);
  }

private:
  //! only called from DofManagerFactory 
  //********************************************************
  // read-write Interface for index set 
  //********************************************************
  //! writes all underlying index sets to a file 
  bool write(const std::string filename, int timestep);
  //! reads all underlying index sets from a file 
  bool read(const std::string filename, int timestep);

  bool write_xdr(const std::string filename, int timestep);
  bool read_xdr( const std::string filename, int timestep);
}; // end class DofManager

//***************************************************************************
//
//  inline implemenations 
//
//***************************************************************************

template <class GridType>
inline DofManager<GridType>::~DofManager () 
{
  if(memList_.size() > 0)
  {
    while( memList_.rbegin() != memList_.rend())
    {
      MemObjectInterface * mobj = (* memList_.rbegin() );
      memList_.pop_back();
      
      // alloc new mem an copy old mem 
      dverb << "Removing '" << mobj->name() << "' from DofManager!\n";  
      if(mobj) delete mobj;
    }
  }

  if(indexList_.size() > 0)
  {
    while ( indexList_.rbegin() != indexList_.rend()) 
    {
      IndexSetObjectInterface * iobj = (* indexList_.rbegin() );
      indexList_.pop_back();
      if(iobj) delete iobj;
    }
  }
}

template <class GridType>
inline bool DofManager<GridType>::
removeDofSet (const MemObjectInterface & obj)
{
  bool removed = false;

  ListIteratorType endit = memList_.end();
  for( ListIteratorType it = memList_.begin();
       it != endit ; ++it)
  {
    if(*it == &obj)
    {
      // alloc new mem and copy old mem 
      MemObjectInterface * mobj = (*it);
      memList_.erase( it );  
      dverb << "Removing '" << obj.name() << "' from DofManager!\n";
      if(mobj) delete mobj;
      removed = true;
      break;
    }
  }
  return removed;
}


template <class GridType>
template <class IndexSetType>
inline void DofManager<GridType>::
addIndexSet (const GridType &grid, IndexSetType &iset)
{
  if(&grid_ != &grid)
    DUNE_THROW(DofManError,"DofManager can only be used for one grid! \n");

  typedef typename GridType::template Codim<0>::Entity EntityType;
  typedef IndexSetObject< IndexSetType, EntityType > IndexSetObjectType;
  
  IndexSetObjectType * indexSet = 0;
  
  IndexListIteratorType endit = indexList_.end();
  for(IndexListIteratorType it = indexList_.begin(); it != endit; ++it)
  {
    if( (*it)->address() == &iset )
    {
      indexSet = static_cast<IndexSetObjectType *> ((*it));
      break;
    }
  }
  
  if(!indexSet) 
  { 
    indexSet = new IndexSetObjectType ( iset );
      
    IndexSetObjectInterface * iobj = indexSet;
    indexList_.push_back( iobj );
    indexSets_ += *indexSet;

    insertIndices_ += (*indexSet).insertIndexObj();
    removeIndices_ += (*indexSet).removeIndexObj();
  }
  return ; 
}

template <class GridType>
template <class IndexSetType>
inline bool DofManager<GridType>::
checkIndexSetExists (const IndexSetType &iset) const
{
  ConstIndexListIteratorType endit  = indexList_.end();
  for(ConstIndexListIteratorType it = indexList_.begin(); it != endit; ++it)
  {
    if( (*it)->address() == &iset ) 
    {
      return true; 
    }
  }
  return false;
}


template <class GridType>
template <class DofStorageType, class MapperType >
std::pair<MemObjectInterface*, DofStorageType*>
DofManager<GridType>::
addDofSet(const DofStorageType * ds, const MapperType & mapper, std::string name)
{
  assert( name.c_str() != 0);
  dverb << "Adding '" << name << "' to DofManager! \n";

  typedef MemObject<MapperType,DofStorageType> MemObjectType; 
  MemObjectType * obj = new MemObjectType ( mapper, name ); 
  memList_.push_back( obj );    

  // add the special object to the checkResize list object 
  checkResize_ += (*obj).checkResizeObj(); 
  
  // the same for the resize call  
  resizeMemObjs_ += (*obj).resizeMemObject();

  return std::pair<
    MemObjectInterface*, DofStorageType*>(obj, & (obj->getArray()) );
}

template <class GridType>
template <class DofStorageType, class MapperType , class VectorPointerType >
std::pair<MemObjectInterface*, DofStorageType*>
DofManager<GridType>::
addDummyDofSet(const DofStorageType * ds, const MapperType & mapper, std::string name, VectorPointerType * vector )
{
  assert( name.c_str() != 0);
  dverb << "Adding '" << name << "' to DofManager! \n";

  typedef DummyMemObject<MapperType,DofStorageType,VectorPointerType> MemObjectType; 
  MemObjectType * obj = new MemObjectType ( mapper, name , vector ); 
  memList_.push_back( obj );    

  // obj is not inserted in resize lists because mem is coming from outside 
  return std::pair<
    MemObjectInterface*, DofStorageType*>(obj, & (obj->getArray()) );
}


template <class GridType>
inline bool DofManager<GridType>::
write(const std::string filename, int timestep)
{
  return write_xdr(filename,timestep);
}
template <class GridType>
inline bool DofManager<GridType>::
read(const std::string filename , int timestep)
{
  return read_xdr(filename,timestep);  
}

template <class GridType>
inline bool DofManager<GridType>::
write_xdr(const std::string filename , int timestep)
{
  int count = 0;
  IndexListIteratorType endit = indexList_.end();
  for(IndexListIteratorType it = indexList_.begin(); it != endit; ++it)
  {
    std::string newFilename (filename);
    newFilename += "_"; 
    char tmp[256]; 
    sprintf(tmp,"%d",count);
    newFilename += tmp;
    newFilename += "_"; 
    (*it)->write_xdr(newFilename.c_str(),timestep); 
    count ++;
  }
  return true;
}

template <class GridType>
inline bool DofManager<GridType>::
read_xdr(const std::string filename , int timestep)
{
  int count = 0;
  IndexListIteratorType endit = indexList_.end();
  for(IndexListIteratorType it = indexList_.begin(); it != endit; ++it)
  {
    std::string newFilename (filename);
    newFilename += "_"; 
    char tmp[256]; 
    sprintf(tmp,"%d",count);
    newFilename += tmp;
    newFilename += "_"; 
    std::string fnstr = genFilename("",newFilename.c_str(), timestep);
    FILE * testfile = fopen(fnstr.c_str(),"r");
    if( testfile )
    {
      fclose( testfile );
      (*it)->read_xdr(newFilename.c_str(),timestep); 
      count ++;
    }
    else 
    {
      std::cout << "WARNING: Skipping " << fnstr << " in DofManager::read_xdr! \n";
    }
  }
  return true;
}

//! DofManagerFactory guarantees that only one instance of a dofmanager 
//! per grid is generated. If getDofManager is called with a grid for which
//! already a mamager exists, then the reference to this manager is returned. 
template <class DofManagerImp>
class DofManagerFactory 
{
  typedef DofManagerImp DofManagerType;
  typedef typename DofManagerType :: GridType GridType; 
  typedef DoubleLinkedList < std::pair < const GridType * , DofManagerType * > > ListType;
  typedef typename ListType::Iterator ListIteratorType;

  //! list that store pairs of grid/dofmanager pointers 
  //! singleton grid list 
  inline static ListType & gridList() 
  {
    //! list that store pairs of grid/dofmanager pointers 
    static ListType gridList_; 
    return gridList_; 
  }
  
public:  
  //! return reference to the DofManager for the given grid. 
  //! If the object does not exist, then it is created first.
  inline static DofManagerType & getDofManager (const GridType & grid) 
  {
    // search list for dof manager 
    DofManagerType * dm = getDmFromList(grid);

    // if not exists, create it 
    if(!dm)
    {
      dm = new DofManagerType ( grid );
      assert( dm );
      std::pair < const GridType * , DofManagerType * > tmp ( & grid , dm );
      gridList().insert_after( gridList().rbegin() , tmp ); 
    }

    return *dm; 
  } 

  //! delete the dof manager that belong to the given grid 
  inline static void deleteDofManager (DofManagerType & dm ) 
  {
    ListIteratorType endit = gridList().end();
    for(ListIteratorType it = gridList().begin(); it!=endit; ++it)
    {
      if( (*it).second == (& dm ))
      {
        gridList().erase( it );
        DofManagerType * tmp = & dm ;
        std::cout << "Deleting dm = " << tmp << "\n";
        if( tmp ) delete tmp;
        return;
      }
    }
    std::cerr << "DofManager could not deleted, because is not in list anymore! \n";
  }

  //! writes DofManager of corresponding grid, when DofManager exists 
  inline static bool 
  writeDofManager(const GridType & grid, const std::string filename, int timestep)
  {
    DofManagerType * dm = getDmFromList(grid); 
    if(dm) return dm->write(filename,timestep);
    return false;
  }

  //! reads DofManager of corresponding grid, when DofManager exists 
  inline static bool 
  readDofManager(const GridType & grid, const std::string filename, int timestep)
  {
    DofManagerType * dm = getDmFromList(grid); 
    if(dm) return dm->read(filename,timestep);
    return false;
  }

private: 
  // return pointer to dof manager for given grid 
  inline static DofManagerType * getDmFromList(const GridType &grid)
  {
    ListIteratorType endit = gridList().end();
    for(ListIteratorType it = gridList().begin(); it!=endit; ++it)
    {
      if( (*it).first == & grid )
      {
        return ((*it).second);
      }
    }
    return 0;
  }
  
  // constructor 
  DofManagerFactory () {};  

public:  
  // destructor 
  ~DofManagerFactory () 
  { 
    while (gridList().rbegin() != gridList().rend())
    {
      ListIteratorType it = gridList().rbegin();
      DofManagerType * tmp = (*it);
      gridList().erase( it );
      std::cout << "Deleting dm = " << tmp << "\n";
      if( tmp ) delete tmp;
    }
  }
};

} // end namespace Dune 
#endif
