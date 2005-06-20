#ifndef __DUNE_DATACOLLECTOR_HH__
#define __DUNE_DATACOLLECTOR_HH__

#include <vector>
#include <dune/fem/common/objpointer.hh>

namespace Dune{

/** @defgroup DataCollectors DataCollectors 
    @ingroup DataCollector
    
  @{
 */

/*! This could be seen as a hack, but is not
  With this method we define the class CombineRestProl which is only 
  for combining of local grid operations without using virtual methods.
  The interface of the two defined methods of the class (restrictLocal and
  prolongLocal) is given by the implementation (see below ) and 
  has to be the same for all local operators you want to combine 
*/

template <class LocalOp, class ParamType> class LocalInlinePlus;

#define PARAM_CLASSNAME CombinedLocalDataCollect 
#define PARAM_INHERIT LocalInlinePlus  
#define PARAM_FUNC_1 apply 
#include <dune/fem/common/combine.hh>

template <class ParamT>
class LocalInterface : public ObjPointerStorage
{
public:
  typedef LocalInterface<ParamT> MyType;
  struct Traits 
  {
    typedef ParamT ParamType;
  };

private:
  typedef ParamT ParamType;
  typedef void FuncType(MyType &, ParamType & p); 
  typedef typename std::pair < MyType * ,  FuncType * > PairType;
  typedef typename std::vector < PairType > ListType;
  
  template <class OpType> 
  struct AddToWrapper 
  {
    //! applyWrapper knows the real type of Op 
    static void applyWrapper(MyType & m, ParamType & p )
    {
      static_cast<OpType &> (m).apply(p); 
    }

    //! store pointer to Op /as a pointer to interface class 
    //! and store a pointer to applyWrapper as a function pointer
    static void addToList (ListType & vec , const OpType & op )
    {
      PairType p( const_cast<OpType *> (&op) , applyWrapper);
      vec.push_back(p);
    }
  };

  // copy list of op to this class 
  static void copyList (ListType & vec , const MyType & op )
  {
    const ListType & ve = op.vec_;
    if(ve.size() > 0)
    {   
      ListType tmp ( vec );
      vec.resize( vec.size() + ve.size() );
    
      // copy list to new vector 
      for(unsigned int i=0; i<tmp.size(); i++)
        vec[i] = tmp[i];
      for(unsigned int i=tmp.size(); i<vec.size(); i++)
        vec[i] = ve[i-tmp.size()];
    }
  }
  
public:
  LocalInterface () : vec_ (0) {}
  
  template <class OpType>
  LocalInterface (const OpType & op)
  {
    AddToWrapper<OpType>::addToList(vec_,op);
  }

  LocalInterface (const MyType & op)
  {
    copyList(vec_,op);
  }

  virtual ~LocalInterface() {};

  //! for all pointer to local operators call the func pointer 
  void apply ( ParamType & p ) const 
  {
    for(unsigned int i=0; i<vec_.size(); i++)
    {
      // vec_[i].second contains the pointer to the function that makes the
      // correct cast to the real type of vec_[i].first 
      (*vec_[i].second)( *(vec_[i].first) , p );
    }
  }
    
  template <class OpType>
  MyType & operator + (const OpType & op)
  {
    AddToWrapper<OpType>::addToList(vec_,op);
    return *this;
  }
  
  MyType & operator + (const MyType & op)
  {
    copyList(vec_,op);
    return *this;
  }
  
  template <class OpType>
  MyType & operator += (const OpType & op)
  {
    AddToWrapper<OpType>::addToList(vec_,op);
    return *this;
  }
  
  MyType & operator += (const MyType & op)
  {
    copyList(vec_,op);
    return *this;
  }
  
  template <class OpType>
  MyType & operator = (const OpType & op) 
  {
    //std::cout << "operator = of Lint \n";
    vec_.resize(0);
    AddToWrapper<OpType>::addToList(vec_,op);
    return *this;
  }
 
private:  
  mutable ListType vec_;
};

template <class LocalOp, class ParamT>
class LocalInlinePlus : public LocalInterface<ParamT> 
{
public:
  //! when overload this class ParamType  
  //! and LocalInterfaceType must be redefined 
  struct Traits 
  {
    typedef ParamT ParamType;
    typedef LocalInterface<ParamType> LocalInterfaceType;
  };
  
  template <class B> 
  CombinedLocalDataCollect<LocalOp,B> & operator + (const B & b) 
  {
    std::cout << "operator + of LocalInlinePlus \n";
    typedef CombinedLocalDataCollect<LocalOp,B> CombinedType;
    CombinedType * combo = new CombinedType ( asImp() , b );
    saveObjPointer( combo );   
    return *combo;
  }
  LocalOp & asImp() { return static_cast<LocalOp &> (*this); }
};

class DummyObjectStream
{ 
  public:
    class EOFException {} ;
    template <class T>
    void readObject (T &) {}
    void readObject (int) {}
    void readObject (double) {}
    template <class T>
    void writeObject (T &) {};
    void writeObject (int) {} ;
    void writeObject (double) {};
};

/*! Combination of different DataCollectors

 This Class is the result of a combination of different
 AdaptationOperators. It is the same principle as with Mapping and
 DiscreteOperatorImp. 
*/ 
template <class GridType, class ObjectStreamImp = DummyObjectStream >
class DataCollectorInterface 
{
  typedef ObjectStreamImp  ObjectStreamType;
  typedef typename GridType::template codim<0>::Entity EntityType;

  typedef DataCollectorInterface<GridType,ObjectStreamImp> MyType;
  typedef std::pair < ObjectStreamType * , 
          const typename GridType:: template codim<0>::Entity  * >  DataCollectorParamType;
  
public:
  typedef LocalInterface<DataCollectorParamType> LocalInterfaceType;

  struct Traits 
  {
    typedef LocalInterface<DataCollectorParamType> LocalInterfaceType;
  };
  
  DataCollectorInterface () : dc_ (0) {}
  
  //! all adaptation operators have this method which adapts the
  //! corresponding grid and organizes the restriction prolongation process
  //! of the underlying function spaces
  virtual void apply (ObjectStreamType &str,EntityType & en) const 
  {
    //std::cout << "apply on interface class \n";
    if(dc_) (*dc_).apply(str,en);  
    else 
    {
      std::cerr << "WARNING: apply: did nothing! \n";
    }
  };
  
  virtual const LocalInterfaceType & getLocalInterfaceOp () const 
  {
    if(dc_) 
      return dc_->getLocalInterfaceOp ();
    else 
    {
      std::cerr << "No LocalInterfaceOperator \n";
      assert(false);
      return *(new LocalInterfaceType());
    }
  };
  
  virtual LocalInterfaceType & getLocalInterfaceOp ()
  {
    if(dc_) 
      return dc_->getLocalInterfaceOp ();
    else 
    {
      std::cerr << "No LocalInterfaceOperator \n";
      assert(false);
      return *(new LocalInterfaceType());
    }
  };

  //! Assignement operator
  template <class OpType>
  MyType & operator += (const OpType & dc)
  {
    if(dc_)
    {
      std::cout << "Operator += with OpType \n";
      dc_ = dcConv_;
      MyType * tmp = const_cast<OpType &> (dc).convert();
      dc_ = &(*dc_).operator += (*tmp);
    }
    else 
    {
      dc_     = const_cast<OpType *> (&dc);
      dcConv_ = const_cast<OpType &> (dc).convert();
    }
    return (*this);
  }
  
  //! Assignement operator
  virtual MyType & operator += (const MyType & dc)
  {
    if(dc_)
    {
      std::cout << "Operator += with MyType \n";
      dc_ = dcConv_;
      dc_ = &(*dc_).operator += (dc);
    }
    else 
    {
      dc_ = const_cast<MyType *> (&dc);
      dcConv_ = const_cast<MyType *> (&dc);
    }
    return (*this);
  }
  
  //! Assignement operator
  template <class OpType>
  MyType & operator = (const OpType & dc)
  {
    std::cout << "Store operator \n";
    dc_     = const_cast<OpType *> (&dc);
    dcConv_ = const_cast<OpType &> (dc).convert();
    return (*this);
  }

  //! Assignement operator
  MyType & operator = (const MyType & dc)
  {
    std::cout << "No need to do this, use += \n";
    dc_     = const_cast<MyType *> (dc.dc_);
    dcConv_ = const_cast<MyType *> (dc.dcConv_);
    return (*this);
  }

private: 
  MyType *dc_;
  MyType *dcConv_;
};


//! empty data collector 
template <class GridType>
class DummyDataCollector 
{
  typedef DummyDataCollector<GridType> MyType;
public:

  typedef std::pair < int * , int * > DataCollectorParamType;
  typedef LocalInterface<DataCollectorParamType> LocalInterfaceType;
  //! all adaptation operators have this method which adapts the
  //! corresponding grid and organizes the restriction prolongation process
  //! of the underlying function spaces
  void apply (int , int ) const 
  {
    std::cerr << "WARNING: apply: did nothing! \n";
  };
  
  //! Assignement operator
  template <class OpType>
  MyType & operator += (const OpType & dc)
  {
    return (*this);
  }
  
  //! Assignement operator
  template <class OpType>
  MyType & operator = (const OpType & dc)
  {
    return (*this);
  }
};

/** \brief The DataCollector is an example for a grid walk done while load
 * balancing moves entities from one processor to another. 
 * The Communicator or grid should call the inlineData (write Data to
 * ObjectStream) and the xtractData (read Data from Stream) and provide the
 * macro level Entity<codim =0> and the ObjectStream. This Operator then
 * does the hierarhic walk and calls its local pack operators which know
 * the discrete functions to pack to the stream. 
 */
template <class GridType, class LocalDataCollectImp, class DofManagerType >
class DataCollector
: public DataCollectorInterface<GridType, typename GridType::ObjectStreamType>
, public ObjPointerStorage 
{  
  typedef typename GridType::template codim<0>::Entity EntityType;
  typedef DataCollector<EntityType,LocalDataCollectImp,DofManagerType> MyType;
  typedef typename GridType::ObjectStreamType ObjectStreamType;

  typedef typename std::pair < ObjectStreamType * , const EntityType * > ParamType;
  typedef LocalInterface<ParamType> LocalInterfaceType;
  
  friend class DataCollectorInterface<GridType,ObjectStreamType>; 
  typedef DataCollectorInterface<GridType,ObjectStreamType> DataCollectorInterfaceType;
  
public:
  //! create DiscreteOperator with a LocalOperator 
  DataCollector (GridType & grid, DofManagerType & dm, LocalDataCollectImp & ldc, bool read ) :
    grid_(grid) , dm_ ( dm ), ldc_ (ldc) , read_(read) {}

  //! Desctructor 
  virtual ~DataCollector () {}

  //! operator + (combine this operator) and return new Object 
  template <class LocalDataCollectType> 
  DataCollector<GridType,
  CombinedLocalDataCollect <LocalDataCollectImp,LocalDataCollectType>, DofManagerType> & 
  operator + (const DataCollector<GridType,LocalDataCollectType,DofManagerType> &op)
  {
    typedef DataCollector<GridType,LocalDataCollectType,DofManagerType> CopyType;
    typedef CombinedLocalDataCollect <LocalDataCollectImp,LocalDataCollectType> COType;
     
    COType *newLDCOp = new COType ( ldc_  , const_cast<CopyType &> (op).getLocalOp() );
    typedef DataCollector < GridType, COType, DofManagerType > OPType;
   
    OPType *dcOp = new OPType ( grid_ , dm_ , *newLDCOp , read_ );    

    // memorize this new generated object because is represents this
    // operator and is deleted if this operator is deleted
    saveObjPointer( dcOp , newLDCOp );
   
    return *dcOp;
  }

  //! oeprator += combine and return this Object 
  template <class LocalDataCollectType> 
  DataCollector<GridType,LocalInterface<ParamType>, DofManagerType> & 
  operator += (const DataCollector<GridType,LocalDataCollectType,DofManagerType> &op)
  {
    typedef DataCollector<GridType,LocalDataCollectType,DofManagerType> CopyType;
    typedef LocalInterface<ParamType> COType;
     
    COType *newLDCOp = new COType ( ldc_ + op.getLocalOp() );
    typedef DataCollector < GridType, COType, DofManagerType > OPType;
   
    OPType *dcOp = new OPType ( grid_ , dm_ , *newLDCOp , read_ );    

    // memorize this new generated object because is represents this
    // operator and is deleted if this operator is deleted
    saveObjPointer( dcOp , newLDCOp );
   
    return *dcOp;
  }
 
  //! operator += combine and return InterfaceType
  DataCollectorInterfaceType & 
  operator += (const DataCollectorInterfaceType &op)
  {
    std::cout << "operator += with Interface Type \n";
    typedef LocalInterface<ParamType> COType;
    typedef DataCollector<GridType,COType,DofManagerType> CopyType;
     
    COType *newLDCOp = new COType ( ldc_ + op.getLocalInterfaceOp() );
    typedef DataCollector < GridType, COType, DofManagerType > OPType;
   
    OPType *dcOp = new OPType ( grid_ , dm_ , *newLDCOp , read_ );    

    // memorize this new generated object because is represents this
    // operator and is deleted if this operator is deleted
    saveObjPointer( dcOp , newLDCOp );
   
    return *dcOp;
  }
 
  //! return reference to loacl Operator 
  const LocalDataCollectImp & getLocalOp () const 
  {
    return ldc_;
  }
 
  //! return reference to loacl Operator 
  LocalDataCollectImp & getLocalOp ()  
  {
    return ldc_;
  }

  const LocalInterfaceType & getLocalInterfaceOp () const 
  {
    std::cout << "getLocalInter \n";
    return ldc_;
  }
  
  LocalInterfaceType & getLocalInterfaceOp ()  
  {
    std::cout << "getLocalInter \n";
    return ldc_;
  }

  //! apply, if this operator is in write status the inlineData is called
  //! else xtractData is called 
  void apply (ObjectStreamType & str, EntityType & en) const 
  {
    //std::cout << "apply on imp class \n";
    if(!read_) 
      inlineData(str,en);
    else 
      xtractData(str,en);
  }

  //! write all data of all entities blowe this Entity to the stream 
  void inlineData (ObjectStreamType & str, EntityType & en) const 
  {
    //std::cout << "DataCollector Inline data\n";
    str.writeObject( grid_.maxlevel() );
    goDown(str,en,grid_.maxlevel());
  }

  //! read all data of all entities blowe this Entity from the stream 
  void xtractData (ObjectStreamType & str, EntityType & en) const 
  {
    //std::cout << "DataCollector xtract data\n";
    int mxlvl = 0;
    str.readObject( mxlvl );
    mxlvl = std::max( mxlvl , grid_.maxlevel() );

    // dont needed anymore, because here the grid was 
    // adapted before 
    goDown(str,en,mxlvl);
  }
  
private:
  DataCollector<GridType,LocalInterface<ParamType>, DofManagerType> * convert ()  
  {
    typedef LocalInterface<ParamType> COType;
     
    COType *newLDCOp = new COType ( ldc_ );
    typedef DataCollector < GridType, COType, DofManagerType > OPType;
   
    OPType *dcOp = new OPType ( grid_ , dm_ , *newLDCOp , read_ );    

    // memorize this new generated object because is represents this
    // operator and is deleted if this operator is deleted
    saveObjPointer( dcOp , newLDCOp );
   
    return dcOp;
  }
 
  void goDown (ObjectStreamType & str, EntityType & en, int mxlvl) const 
  {
    ParamType p( &str , &en );
    
    ldc_.apply( p );
    {
      typedef typename EntityType::HierarchicIterator HierItType;
      HierItType endit = en.hend(mxlvl);
      for(HierItType it = en.hbegin(mxlvl); 
          it != endit; ++it )
      {
        //if((*it).isLeaf())
        {
          p.second = it.operator -> ();
          ldc_.apply( p );
        }
      }
    }
  }

  //! corresponding grid 
  mutable GridType & grid_;

  //! DofManager corresponding to grid
  mutable DofManagerType & dm_;
  
  //! Restriction and Prolongation Operator 
  mutable LocalDataCollectImp & ldc_;

  //! if true inline else xtract
  bool read_;
};


//***********************************************************************

    /** \brief ???
     * \todo Please doc me!
     */
template <class DiscreteFunctionType>
class DataInliner : 
public LocalInlinePlus < DataInliner< DiscreteFunctionType > , 
  typename std::pair < typename DiscreteFunctionType::FunctionSpaceType::GridType::ObjectStreamType * , 
      const typename DiscreteFunctionType::FunctionSpaceType::GridType::template codim<0>::Entity * > > 
{
  typedef LocalInlinePlus < DataInliner< DiscreteFunctionType > ,
    typename std::pair < typename  DiscreteFunctionType::FunctionSpaceType::GridType::ObjectStreamType * ,
          const typename  DiscreteFunctionType::FunctionSpaceType::GridType::template codim<0>::Entity * > >  ChefType;
public:
  typedef typename DiscreteFunctionType::FunctionSpaceType::GridType::ObjectStreamType ObjectStreamType;
  typedef typename DiscreteFunctionType::FunctionSpaceType::GridType::template codim<0>::Entity EntityType;
  typedef typename ChefType::Traits::ParamType ParamType;

  typedef DataInliner<DiscreteFunctionType> MyType;

  typedef LocalInterface<ParamType> LocalInterfaceType;
    
  typedef typename DiscreteFunctionType::LocalFunctionType LocalFunctionType;

  typedef typename DiscreteFunctionType::RangeFieldType RangeFieldType;
  typedef typename DiscreteFunctionType::DomainType DomainType;
  
public:  
  DataInliner ( DiscreteFunctionType & df , bool leaf = true ) 
    : df_ (df) , lf_ (df.newLocalFunction() ) , leaf_(leaf) {}

  //! store data to stream  
  //template <class ObjectStreamType, class EntityType>
  void apply ( ParamType & p ) const 
  {
    assert( p.first && p.second );
    EntityType & en = const_cast<EntityType &> (*(p.second));
    
    if(leaf_) if(!en.isLeaf()) return; 
    
    df_.localFunction( en ,  lf_ );
    for(int l=0; l<lf_.numberOfDofs(); l++)
    {
      (*p.first).writeObject( lf_[l] );
    }
  }

private:
  mutable DiscreteFunctionType & df_;
  mutable LocalFunctionType lf_;
  
  // true if only leaf data is transferd 
  bool leaf_;
};


/*
template <class ObjectStreamType , class LocalFunctionType> 
struct GatherScatterCopy 
{
  static void gather ( ObjectStreamType & os, LocalFunctionType & lf )
  {
    std::cout << "Gather data \n";
    for(int l=0; l<lf.numberOfDofs(); l++)
    {
      os.readObject( lf[l] );
    }
  }
};

*/

template <class DiscreteFunctionType>
class DataXtractor : 
public LocalInlinePlus < DataXtractor< DiscreteFunctionType > , 
  typename std::pair < typename DiscreteFunctionType::FunctionSpaceType::GridType::ObjectStreamType * , 
      const typename DiscreteFunctionType::FunctionSpaceType::GridType::template codim<0>::Entity * > > 
{
  typedef LocalInlinePlus < DataInliner< DiscreteFunctionType > ,
    typename std::pair < typename  DiscreteFunctionType::FunctionSpaceType::GridType::ObjectStreamType * ,
          const typename  DiscreteFunctionType::FunctionSpaceType::GridType::template codim<0>::Entity * > >  ChefType;
public:
  typedef typename DiscreteFunctionType::FunctionSpaceType::GridType::ObjectStreamType ObjectStreamType;
  typedef typename DiscreteFunctionType::FunctionSpaceType::GridType::template codim<0>::Entity EntityType;
  typedef typename ChefType::Traits::ParamType ParamType;

  typedef DataXtractor<DiscreteFunctionType> MyType;

  typedef LocalInterface<ParamType> LocalInterfaceType;
    
  typedef typename DiscreteFunctionType::LocalFunctionType LocalFunctionType;

  typedef typename DiscreteFunctionType::RangeFieldType RangeFieldType;
  typedef typename DiscreteFunctionType::DomainType DomainType;

  //typedef void GatherFunctionType( ObjectStreamType & , LocalFunctionType & );
  //typedef void ScatterFunctionType( ObjectStreamType & , LocalFunctionType & );
  
public:  
  DataXtractor ( DiscreteFunctionType & df , bool leaf = true) 
    : df_ (df) , lf_ (df.newLocalFunction() ) , leaf_(leaf) {}

  //! store data to stream  
  //template <class ObjectStreamType, class EntityType>
  void apply ( ParamType & p ) const 
  {
    assert( p.first && p.second );
    EntityType & en = const_cast<EntityType &> (*(p.second));

    if(leaf_) if(!en.isLeaf()) return;
    
    df_.localFunction( en , lf_ );
      //assert( gatherFunc_ );
      //gatherFunc_( (*(p.first)) , lf_ ) ;
    for(int l=0; l<lf_.numberOfDofs(); l++)
    {
      (*(p.first)).readObject( lf_[l] );
    }
  }

  /*
  template <template <class,class> class GatherScatterType>
  void setGatherScatter ()
  { 
    gatherFunc_ = GatherScatterType<ObjectStreamType,LocalFunctionType>::gather;
    std::cout << "Set gather Function " << gatherFunc_ << "\n";
  }
  */
  
private:
  mutable DiscreteFunctionType & df_;
  mutable LocalFunctionType lf_;
  
  // true if only leaf data is transferd 
  bool leaf_;

  //GatherFunctionType * gatherFunc_;
};


/** @} end documentation group */

}

#endif
