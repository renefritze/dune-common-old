#ifndef __DUNE_DISCFUNCARRAY_CC__
#define __DUNE_DISCFUNCARRAY_CC__

namespace Dune 
{

// Constructor makeing empty discrete function  
template<class DiscreteFunctionSpaceType >
inline DiscFuncArray< DiscreteFunctionSpaceType >::
DiscFuncArray(DiscreteFunctionSpaceType & f) : 
 DiscreteFunctionDefaultType ( f )  
 , built_ ( false ) , freeLocalFunc_ (NULL)  
 , allLevels_ (false) , level_ (-1), levOcu_ (0) {}
  
// Constructor makeing discrete function  
template<class DiscreteFunctionSpaceType >
inline DiscFuncArray< DiscreteFunctionSpaceType >::
DiscFuncArray(DiscreteFunctionSpaceType & f, 
    int level , int codim , bool allLevel )  
: DiscreteFunctionDefaultType ( f ) , level_ ( level ) ,
    allLevels_ ( allLevel ) , freeLocalFunc_ (NULL) 
{
  if(allLevels_)
    levOcu_ = level_+1;
  else
   levOcu_ = 1;
  
  getMemory();
}

template<class DiscreteFunctionSpaceType >
inline DiscFuncArray< DiscreteFunctionSpaceType >::
DiscFuncArray(const DiscFuncArray <DiscreteFunctionSpaceType> & df ) :
 DiscreteFunctionDefaultType ( df.functionSpace_ ) 
{
  built_ = df.built_; 
  allLevels_ = df.allLevels_;
  levOcu_ = df.levOcu_;
  level_ = df.level_;

  dofVec_ = df.dofVec_;
  freeLocalFunc_ = NULL;
} 

    
// Desctructor 
template<class DiscreteFunctionSpaceType >
inline DiscFuncArray< DiscreteFunctionSpaceType >::
~DiscFuncArray() 
{
  if(freeLocalFunc_) delete freeLocalFunc_;
}


template<class DiscreteFunctionSpaceType >
inline void DiscFuncArray< DiscreteFunctionSpaceType >::setLevel ( RangeFieldType x, int level )
{
  if(!allLevels_ && level != level_)
  {
    std::cout << "Level not set! \n";
    return;
  }
  int size = dofVec_[level].size();
  Array < RangeFieldType > &vec = dofVec_[level];
  for(int i=0; i<size; i++)
    vec[i] = x; 
}

template<class DiscreteFunctionSpaceType >
inline void DiscFuncArray< DiscreteFunctionSpaceType >::set ( RangeFieldType x )
{
  if(allLevels_)
  {
    for(int l=0; l<level_; l++)
    {
      int size = dofVec_[l].size();
      Array < RangeFieldType > &vec = dofVec_[l];
      for(int i=0; i<size; i++)
        vec[i] = x; 
    }
  }
 
  int size = dofVec_[level_].size();
  Array < RangeFieldType > &vec = dofVec_[level_];
  for(int i=0; i<size; i++)
    vec[i] = x; 
}  

template<class DiscreteFunctionSpaceType >
inline void DiscFuncArray< DiscreteFunctionSpaceType >::clearLevel (int level )
{
  set(0.0,level);
}

template<class DiscreteFunctionSpaceType >
inline void DiscFuncArray< DiscreteFunctionSpaceType >::clear ()
{
  set ( 0.0 ); 
}

template<class DiscreteFunctionSpaceType >
inline void DiscFuncArray< DiscreteFunctionSpaceType >::print(std::ostream &s, int level )
{
  RangeFieldType sum = 0.;
  int numLevel = const_cast<GridType &> (functionSpace_.getGrid()).maxlevel();
  DofIteratorType enddof = dend ( numLevel );
  for(DofIteratorType itdof = dbegin ( numLevel ); itdof != enddof; ++itdof) 
  {
//    s << (*itdof) << " DofValue \n";
    sum += ABS(*itdof);
  } 
  s << "sum = " << sum << "\n";
}
//*************************************************************************
//  Interface Methods 
//*************************************************************************
template<class DiscreteFunctionSpaceType > template<class GridIteratorType> 
inline LocalFunctionArrayIterator<DiscFuncArray<DiscreteFunctionSpaceType>,GridIteratorType> 
DiscFuncArray< DiscreteFunctionSpaceType >::
localFunction ( GridIteratorType &it )
{
  LocalFunctionArrayIterator<DiscreteFunctionType,GridIteratorType>
      tmp ( *this , it );
  return tmp;
}

template<class DiscreteFunctionSpaceType > 
inline typename DiscFuncArray<DiscreteFunctionSpaceType>::LocalFunctionType * 
DiscFuncArray< DiscreteFunctionSpaceType >::getLocalFunction ()
{
  if(!freeLocalFunc_)
  {
    LocalFunctionType *lf = new LocalFunctionType (functionSpace_,dofVec_);
    return lf; 
  }
  else 
  {
    LocalFunctionType *lf = freeLocalFunc_;
    freeLocalFunc_ = lf->getNext();
    return lf; 
  }
}

template<class DiscreteFunctionSpaceType > 
inline void DiscFuncArray< DiscreteFunctionSpaceType >::
freeLocalFunction ( typename DiscFuncArray<DiscreteFunctionSpaceType>::LocalFunctionType *lf)  
{
  lf->setNext(freeLocalFunc_);
  freeLocalFunc_ = lf;  
}


template<class DiscreteFunctionSpaceType > 
inline typename DiscFuncArray<DiscreteFunctionSpaceType>::DofIteratorType 
DiscFuncArray< DiscreteFunctionSpaceType >::dbegin ( int level )
{
  DofIteratorType tmp ( dofVec_ [level] , 0 );     
  return tmp;
}

template<class DiscreteFunctionSpaceType > 
inline typename DiscFuncArray<DiscreteFunctionSpaceType>::DofIteratorType 
DiscFuncArray< DiscreteFunctionSpaceType >::dend ( int level )
{
  DofIteratorType tmp ( dofVec_ [ level ] , dofVec_[ level ].size() );     
  return tmp;
}


//**************************************************************************
//  Read and Write Methods 
//**************************************************************************
template<class DiscreteFunctionSpaceType >
inline bool DiscFuncArray< DiscreteFunctionSpaceType >::
write_xdr( const char *filename , int timestep )
{
  FILE  *file;
  XDR   xdrs;
  const char *path = NULL;

  const char * fn  = genFilename(path,filename, timestep);
  file = fopen(fn, "wb");
  if (!file)
  { 
    printf( "\aERROR in DiscFuncArray::write_xdr(..): couldnot open <%s>!\n", filename);
    fflush(stderr);
    return false;
  }

  xdrstdio_create(&xdrs, file, XDR_ENCODE);   

  int allHelp = (allLevels_ == true) ? 1 : 0; 
  // write allLevels to file , xdr_bool didnt work 
  xdr_int (&xdrs, &allHelp);
  xdr_int (&xdrs, &level_);
  
  if(allLevels_)
  {
    for(int lev=0; lev<level_; lev++)
      dofVec_[lev].processXdr(&xdrs);
    
  }

  int lev = level_;
  dofVec_[lev].processXdr(&xdrs);

  xdr_destroy(&xdrs);
  fclose(file);
  
  return true;
}

template<class DiscreteFunctionSpaceType >
inline bool DiscFuncArray< DiscreteFunctionSpaceType >::
read_xdr( const char *filename , int timestep )
{
  FILE   *file;
  XDR     xdrs;
  const char *path = NULL;

  const char * fn  = genFilename(path,filename, timestep);
  std::cout << "Reading <" << fn << "> \n";
  file = fopen(fn, "rb");
  if(!file)
  { 
    printf( "\aERROR in DiscFuncArray::read_xdr(..): couldnot open <%s>!\n", filename);
    fflush(stderr);
    return(false);
  }

  // read xdr 
  xdrstdio_create(&xdrs, file, XDR_DECODE);     

  int allHelp; 
  // write allLevels to file  
  xdr_int (&xdrs, &allHelp);
  allLevels_ = (allHelp == 0) ? false : true; 
 
  // read max level on which function lives 
  xdr_int (&xdrs, &level_);
  
  if(allLevels_)
  {
    levOcu_ = level_+1;
    getMemory();

    for(int lev=0; lev<=level_; lev++)
    {
      int length = functionSpace_.size( lev );
      dofVec_[lev].processXdr(&xdrs);
      if(length != dofVec_[lev].size())
      {
        std::cerr << "DiscFuncArray::read_xdr: ERROR wrong length! \n";
        abort();
      }
    }
  }
  else 
  {
    levOcu_ = 1;
    getMemory();

    int lev = level_;
    int length = functionSpace_.size( lev );
    dofVec_[lev].processXdr(&xdrs);
    if(length != dofVec_[lev].size())
    {
      std::cerr << "DiscFuncArray::read_xdr: ERROR wrong length! \n";
      abort();
    }
  }
  
  xdr_destroy(&xdrs);
  fclose(file);
  return true;
}

template<class DiscreteFunctionSpaceType >
inline bool DiscFuncArray< DiscreteFunctionSpaceType >::
write_ascii( const char *filename , int timestep )
{
  const char * path=NULL;
  const char * fn = genFilename(path,filename,timestep); 
  std::fstream outfile( fn , std::ios::out );
  outfile << allLevels_ <<  "\n";
  if(allLevels_)
  {
    outfile << level_ << "\n"; 
    for(int lev=0; lev<level_; lev++)
    {
      int length = functionSpace_.size( lev );
      outfile << length << "\n";
      DofIteratorType enddof = dend ( lev );
      for(DofIteratorType itdof = dbegin ( lev );itdof != enddof; ++itdof) 
      {
        outfile << (*itdof) << " ";
      }
      outfile << "\n";
    }
  }
  {
    int lev = level_;
    int length = functionSpace_.size( lev );
    outfile << length << "\n";
    DofIteratorType enddof = dend ( lev );
    for(DofIteratorType itdof = dbegin ( lev );itdof != enddof; ++itdof) 
    {
      outfile << (*itdof) << " ";
    }
    outfile << "\n";
  }

  outfile.close();
  return true;
}


template<class DiscreteFunctionSpaceType >
inline bool DiscFuncArray< DiscreteFunctionSpaceType >::
read_ascii( const char *filename , int timestep )
{
  const char * path=NULL;
  const char * fn = genFilename(path,filename,timestep); 
  FILE *infile=NULL;
  infile = fopen( fn, "r" );
  if(!infile)
  {
    std::cerr << "Couldnt open file! "<< fn << "\n";
    abort();
  }
  fscanf(infile,"%d \n",&allLevels_); 
  std::cout << "Got allLevels = " << allLevels_ << "\n";
  if(allLevels_)
  {
    fscanf(infile,"%d \n",&level_); 
    levOcu_ = level_+1;
    getMemory();
    std::cout << "Got Levels = " << level_ << "\n";
    for(int lev=0; lev<=level_; lev++)
    {
      int length; 
      fscanf(infile,"%d \n",&length); 
      std::cout << "Got Size of Level = "<< length << " " << lev << "\n";
      std::cout << functionSpace_.size( lev ) << "\n";
      if(length != functionSpace_.size( lev )) 
      {
        std::cerr << "ERROR: wrong number of dofs stored in file!\n"; 
        abort();
      }
      DofIteratorType enddof = dend ( lev );
      for(DofIteratorType itdof = dbegin ( lev );itdof != enddof; ++itdof) 
      {
        fscanf(infile,"%le \n",& (*itdof)); 
      }
    }
  }
  else 
  {
    levOcu_ = 1;
    getMemory();
    int lev = level_;
    int length;
    fscanf(infile,"%d \n",&length); 
    if(length != functionSpace_.size( lev )) 
    {
      std::cerr << "ERROR: wrong number of dofs stored in file!\n"; 
      abort();
    }
    DofIteratorType enddof = dend ( lev );
    for(DofIteratorType itdof = dbegin ( lev );itdof != enddof; ++itdof) 
    {
      fscanf(infile,"%le \n",& (*itdof)); 
    }
  }
  fclose(infile);
  return true;
}

template<class DiscreteFunctionSpaceType >
inline bool DiscFuncArray< DiscreteFunctionSpaceType >::
write_pgm( const char *filename , int timestep )
{
  const char * path=NULL;
  const char * fn = genFilename(path,filename,timestep); 
  std::ofstream out( fn );

  enum { dim = GridType::dimension };
  
  int danz = 129; 
  /*
  int danz = functionSpace_.getGrid().size(level_, dim );
  danz = (int) pow (( double ) danz, (double) (1.0/dim) );
  std::cout << danz << " Danz!\n";
  */
  
  out << "P2\n " << danz << " " << danz <<"\n255\n";
  DofIteratorType enddof = dend ( level_ );
  for(DofIteratorType itdof = dbegin ( level_ ); itdof != enddof; ++itdof) {
    out << (int)((*itdof)*255.) << "\n";
  }
  out.close();
}

template<class DiscreteFunctionSpaceType >
inline bool DiscFuncArray< DiscreteFunctionSpaceType >::
read_pgm( const char *filename , int timestep )
{
  FILE *in;
  int v;

  // get the hole memory 
  level_ = functionSpace_.getGrid().maxlevel();
  allLevels_ = true;
  levOcu_ = level_+1;
  
  getMemory();
  const char * path=NULL;
  const char * fn = genFilename(path,filename,timestep); 
  in = fopen( fn, "r" );
  fscanf( in, "P2\n%d %d\n%d\n", &v, &v, &v );
  DofIteratorType enddof = dend ( level_ );
  for(DofIteratorType itdof = dbegin ( level_ ); itdof != enddof; ++itdof) {
    fscanf( in, "%d", &v );
    (*itdof) = ((double)v)/255.;
  } 
  fclose( in );
}

template<class DiscreteFunctionSpaceType >
inline bool DiscFuncArray< DiscreteFunctionSpaceType >::
write_USPM( const char *filename , int timestep )
{
  // USPM
  std::fstream out( filename , std::ios::out );
  //ElementType eltype = triangle;
  //out << eltype << " 1 1\n"; 
  int length = functionSpace_.size( level );
  out << length << " 1 1\n";

  DofIteratorType enddof = dend ( level );
  for(DofIteratorType itdof = dbegin ( level );
      itdof != enddof; ++itdof)
  {
    out << (*itdof)  << "\n";
  }

  out.close();
  std::cout << "Written Dof to file `" << filename << "' !\n";
}

template<class DiscreteFunctionSpaceType >
inline void DiscFuncArray< DiscreteFunctionSpaceType >::
addScaled( const DiscFuncArray<DiscreteFunctionSpaceType> &g, 
           const RangeFieldType &scalar )
{
  int level = functionSpace_.getGrid().maxlevel();
  int length = dofVec_[level].size();

  Array<RangeFieldType> &v = dofVec_[level];
  const Array<RangeFieldType> &gvec = g.dofVec_[level];
  
  for(int i=0; i<length; i++)
    v[i] += scalar*gvec[i];
}

template<class DiscreteFunctionSpaceType >
template<class GridIteratorType>
inline void DiscFuncArray< DiscreteFunctionSpaceType >::
addScaledLocal( GridIteratorType &it , 
    const DiscFuncArray<DiscreteFunctionSpaceType> &g, const RangeFieldType &scalar )
{
  typedef typename Traits<GridIteratorType>::LocalFunctionIteratorType LFIterType;
  LFIterType dest = localFunction( it );
  LFIterType arg  = const_cast<DiscFuncArray<DiscreteFunctionSpaceType> &> 
                        (g).localFunction( it );

  int length = dest->numberOfDofs();
  if(scalar == 1.)
  {
    for(int i=0; i<length; i++)
      (*dest)[i] += (*arg)[i];
  }
  else if ( scalar == -1. )
  {
    for(int i=0; i<length; i++)
      (*dest)[i] -= (*arg)[i];
  }
  else 
  {
    for(int i=0; i<length; i++)
      (*dest)[i] += scalar * (*arg)[i];
  }
}

template<class DiscreteFunctionSpaceType >
template<class GridIteratorType>
inline void DiscFuncArray< DiscreteFunctionSpaceType >::
addLocal( GridIteratorType &it , 
 const DiscFuncArray<DiscreteFunctionSpaceType> &g)
{
  typedef typename Traits<GridIteratorType>::LocalFunctionIteratorType LFIterType;
  LFIterType dest = localFunction( it );
  LFIterType arg  = const_cast<DiscFuncArray<DiscreteFunctionSpaceType> &> 
                        (g).localFunction( it );

  int length = dest->numberOfDofs();
  for(int i=0; i<length; i++)
    (*dest)[i] += (*arg)[i];
}

template<class DiscreteFunctionSpaceType >
template<class GridIteratorType>
inline void DiscFuncArray< DiscreteFunctionSpaceType >::
substractLocal( GridIteratorType &it , 
 const DiscFuncArray<DiscreteFunctionSpaceType> &g)
{
  typedef typename Traits<GridIteratorType>::LocalFunctionIteratorType LFIterType;
  LFIterType dest = localFunction( it );
  LFIterType arg  = const_cast<DiscFuncArray<DiscreteFunctionSpaceType> &> 
                        (g).localFunction( it );

  int length = dest->numberOfDofs();
  for(int i=0; i<length; i++)
    (*dest)[i] += (*arg)[i];
}

template<class DiscreteFunctionSpaceType >
template<class GridIteratorType>
inline void DiscFuncArray< DiscreteFunctionSpaceType >::
setLocal( GridIteratorType &it , const RangeFieldType & scalar )
{
  typedef typename Traits<GridIteratorType>::LocalFunctionIteratorType LFIterType;
  LFIterType dest = localFunction( it );
  int length = dest->numberOfDofs();
  for(int i=0; i<length; i++)
    (*dest)[i] = scalar;
}
//**********************************************************************
//  --LocalFunctionArray 
//**********************************************************************
template<class DiscreteFunctionSpaceType >
inline LocalFunctionArray < DiscreteFunctionSpaceType >::
LocalFunctionArray( const DiscreteFunctionSpaceType &f , 
              std::vector < Array < RangeFieldType > > & dofVec )
 : fSpace_ ( f ), dofVec_ ( dofVec )  , next_ (NULL)
 , baseFuncSet_ (NULL)
 , uniform_(true)
{}
      
template<class DiscreteFunctionSpaceType >
inline LocalFunctionArray < DiscreteFunctionSpaceType >::~LocalFunctionArray() 
{
  if(next_) delete next_;
}

template<class DiscreteFunctionSpaceType >
inline LocalFunctionArray < DiscreteFunctionSpaceType >::RangeFieldType & 
LocalFunctionArray < DiscreteFunctionSpaceType >::operator [] (int num) 
{
  return (* (values_[num]));
}

template<class DiscreteFunctionSpaceType >
inline int LocalFunctionArray < DiscreteFunctionSpaceType >::
numberOfDofs () const 
{
  return numOfDof_;
}

// hier noch evaluate mit Quadrature Regel einbauen 
template<class DiscreteFunctionSpaceType > template <class EntityType> 
inline void LocalFunctionArray < DiscreteFunctionSpaceType >::
evaluate (EntityType &en, const DomainType & x, RangeType & ret) 
{
  ret = 0.0;
  if(numOfDifferentDofs_ > 1) // i.e. polynom order > 0 
  {
    for(int i=0; i<numOfDifferentDofs_; i++)
    {
      baseFuncSet_->eval( i*dimrange , en.geometry().local(x) ,tmp); 
      for(int l=0; l<dimrange; l++)
      {
        int num = i*dimrange + l;
        ret(l) += (* (values_[num])) * tmp(l);
      }
    }
  }
  else 
  {
    for(int l=0; l<dimrange; l++)
    {
      ret(l) = (* (values_[ l ]));
    }
  }
}

// hier noch evaluate mit Quadrature Regel einbauen 
template<class DiscreteFunctionSpaceType > 
template <class EntityType, class QuadratureType> 
inline void LocalFunctionArray < DiscreteFunctionSpaceType >::
evaluate (EntityType &en, QuadratureType &quad, int quadPoint, RangeType & ret) 
{
  ret = 0.0;
  if(numOfDifferentDofs_ > 1) // i.e. polynom order > 0 
  {
    for(int i=0; i<numOfDifferentDofs_; i++)
    {
      // evaluate base function 
      baseFuncSet_->eval( i*dimrange , quad , quadPoint ,tmp); 

      for(int l=0; l<dimrange; l++)
      {
        int num = i*dimrange + l;
        ret(l) += (* (values_[num])) * tmp(l);
      }
    }
  }
  else 
  {
    // if we have only one base we won't evaluate that
    for(int l=0; l<dimrange; l++)
    {
      ret(l) = (* (values_[ l ]));
    }
  }
}

template<class DiscreteFunctionSpaceType >
inline LocalFunctionArray < DiscreteFunctionSpaceType > * 
LocalFunctionArray < DiscreteFunctionSpaceType >::getNext () const  
{
  return next_;
}

template<class DiscreteFunctionSpaceType >
inline void LocalFunctionArray < DiscreteFunctionSpaceType >::
setNext (LocalFunctionArray < DiscreteFunctionSpaceType > *n)   
{
  next_ = n;
}

template<class DiscreteFunctionSpaceType > template <class EntityType> 
inline bool LocalFunctionArray < DiscreteFunctionSpaceType >::
init (EntityType &en )
{
  if((!uniform_) || (!baseFuncSet_))
  {
    baseFuncSet_ = & ( fSpace_.getBaseFunctionSet(en) );
    numOfDof_ = baseFuncSet_->getNumberOfBaseFunctions();
    numOfDifferentDofs_ = baseFuncSet_->getNumberOfDiffBaseFuncs();

    if(numOfDof_ > values_.size())
      values_.resize( numOfDof_ );
  }

  for(int i=0; i<numOfDof_; i++)
    values_ [i] = &((dofVec_[ en.level() ])[fSpace_.mapToGlobal ( en , i)]);
  return true;
} 

//**********************************************************************
//
//  DofIteratorArray 
//
//**********************************************************************
template <class DofType>
inline DofType& DofIteratorArray<DofType>::operator *()
{
  return dofArray_ [ count_ ];
}

template <class DofType>
inline DofIteratorArray<DofType>& DofIteratorArray<DofType>::operator ++()
{
  count_++;
  return (*this);      
}

template <class DofType>
inline DofIteratorArray<DofType>& DofIteratorArray<DofType>::operator ++(int i)
{
  count_ += i;
  return (*this);
}

template <class DofType>
inline DofType& DofIteratorArray<DofType>::operator [](int i)
{
  return dofArray_[i];
}

template <class DofType>
inline bool DofIteratorArray<DofType>::
operator ==(const DofIteratorArray<DofType> & I) const
{
  return count_ == I.count_;
}

template <class DofType>
inline bool DofIteratorArray<DofType>::
operator !=(const DofIteratorArray<DofType> & I) const
{
  return count_ != I.count_;
}

template <class DofType>
inline int DofIteratorArray<DofType>::index() const
{
  return count_;
}

template <class DofType>
inline void DofIteratorArray<DofType>::reset() 
{
  count_ = 0;
}
//**********************************************************************
//
//  LocalFunctionArrayIterator 
//
//**********************************************************************
template <class DiscFuncType , class GridIteratorType >
inline LocalFunctionArrayIterator<DiscFuncType, GridIteratorType>::
LocalFunctionArrayIterator (DiscFuncType &df , GridIteratorType & it ) : 
it_ ( it ) , df_ ( df ) , built_ ( false ) , lf_ ( df_.getLocalFunction() ) {}
      
template <class DiscFuncType , class GridIteratorType >
inline LocalFunctionArrayIterator<DiscFuncType, GridIteratorType>::
LocalFunctionArrayIterator (const LocalFunctionArrayIteratorType & copy) 
 : it_ ( copy.it_ ) , df_ ( copy.df_ ) , built_ (false) 
 , lf_ ( df_.getLocalFunction() ) {}
      
template <class DiscFuncType , class GridIteratorType >
inline LocalFunctionArrayIterator<DiscFuncType, GridIteratorType>::
~LocalFunctionArrayIterator ()
{
  df_.freeLocalFunction( lf_ ); 
} 
      
template <class DiscFuncType , class GridIteratorType >
inline typename LocalFunctionArrayIterator<DiscFuncType, GridIteratorType>::
LocalFunctionType & LocalFunctionArrayIterator<DiscFuncType,GridIteratorType>::
operator *()
{
  if(!built_) lf_->init ( *it_ );
  return (*lf_);
}

template <class DiscFuncType , class GridIteratorType >
inline typename LocalFunctionArrayIterator<DiscFuncType, GridIteratorType>::
LocalFunctionType * LocalFunctionArrayIterator<DiscFuncType,GridIteratorType>::
operator ->()
{
  if(!built_) lf_->init ( *it_ );
  return lf_;
}

template <class DiscFuncType , class GridIteratorType >
inline LocalFunctionArrayIterator<DiscFuncType, GridIteratorType> &
LocalFunctionArrayIterator<DiscFuncType,GridIteratorType>::operator ++()
{
  ++it_;
  built_ = false;
  return (*this);
}

template <class DiscFuncType , class GridIteratorType >
inline LocalFunctionArrayIterator<DiscFuncType, GridIteratorType> &
LocalFunctionArrayIterator<DiscFuncType,GridIteratorType>::operator ++(int i)
{
  ++it_.operator ++(i);
  built_ = false;
  return (*this);
}

template <class DiscFuncType , class GridIteratorType >
inline bool LocalFunctionArrayIterator<DiscFuncType,GridIteratorType>::
operator ==(const LocalFunctionArrayIteratorType & I) const
{
   return it_ == I.it_;
}

template <class DiscFuncType , class GridIteratorType >
inline bool LocalFunctionArrayIterator<DiscFuncType,GridIteratorType>::
operator !=(const LocalFunctionArrayIteratorType & I) const
{
   return it_ != I.it_;
}

template <class DiscFuncType , class GridIteratorType >
inline int LocalFunctionArrayIterator<DiscFuncType,GridIteratorType>::
index() const
{
  return it_->index();
}

} // end namespace 

#endif
