namespace Dune 
{

#define EPS 1.0E-15
 
template <class T>
T SparseRowMatrix<T>::staticZero = T(0);
 
/*****************************/
/*  Constructor(s)           */
/*****************************/
template <class T>
SparseRowMatrix<T>::SparseRowMatrix()
{
//  values_ = NULL;
//  col_ = NULL;
  dim_[0] = 0;
  dim_[1] = 0;
  nz_ = 0;
}

template <class T>
SparseRowMatrix<T>::~SparseRowMatrix()
{
}

/***********************************/
/*  Construct from storage vectors */
/***********************************/

template <class T> 
SparseRowMatrix<T>::SparseRowMatrix(int rows, int cols, int nz)
{
  dim_[0] = rows;
  dim_[1] = cols;
  nz_ = nz;
  
  values_.resize(dim_[0]*nz_);
  col_.resize(dim_[0]*nz_);

  col_.set(-1);
}

/***************************
 * Copy constructor
 ***************************/
template <class T> 
SparseRowMatrix<T>::SparseRowMatrix(const SparseRowMatrix<T>& other)
{
    (*this) = other;
}

/******************************/
/* Iterator creation          */
/******************************/

// Return iterator refering to first nonzero element in row
template <class T>
typename SparseRowMatrix<T>::ColumnIterator SparseRowMatrix<T>::rbegin (int row)
{
    ColumnIterator it;
    it.mat = this;
    it.base = row*nz_;
    it.offset = 0;

    // Search for the first actual entry
    while (it.offset<nz_ && col_[it.base+it.offset]==-1)
        it.offset++;

    return it;
}
    
// Return iterator refering to one past the last nonzero element of row
template <class T>
typename SparseRowMatrix<T>::ColumnIterator SparseRowMatrix<T>::rend (int row)
{
    ColumnIterator it;
    it.mat = this;
    it.base = row*nz_;
    it.offset = nz_;

    return it;
}

/***************************/
/* Assignment operator...  (deep copy) */
/***************************/

template <class T>
SparseRowMatrix<T>& SparseRowMatrix<T>::operator=(const SparseRowMatrix<T>& other)
{
    // copy field sizes
    dim_[0] = other.dim_[0];
    dim_[1] = other.dim_[1];
    nz_     = other.nz_;

    // copy data
    values_ = other.values_;
    col_    = other.col_;
    
    return *this;
}


/***************************/
/* resize()                */
/***************************/

template <class T> 
void SparseRowMatrix<T>::resize(int M, int N, int nz)
{
    // set field sizes
    dim_[0] = M;
    dim_[1] = N;
    nz_     = nz;

    // resize data fields
    values_.resize(dim_[0]*nz_);
    col_.resize(dim_[0]*nz_);
}

template <class T> 
void SparseRowMatrix<T>::resize(int M, int N)
{
    // set field sizes
    dim_[0] = M;
    dim_[1] = N;

    // resize data fields
    values_.resize(dim_[0]*nz_);
    col_.resize(dim_[0]*nz_);
}

template <class T>
void SparseRowMatrix<T>::clear()
{
    col_.set(-1);
}

/*********************/
/*   Array access    */
/*********************/

template <class T> 
const T& SparseRowMatrix<T>::operator()(int row, int col) const
{
  for (int i=0; i<nz_; i++)
  { 
    if(col_[row*nz_ +i] == col)
    {
      return values_[row*nz_ +i];
    }
  }
  return staticZero;
}

template <class T> 
int SparseRowMatrix<T>::colIndex(int row, int col)
{
    assert(nz_ > 0);
  int whichCol = -1;
  for(int i=0; i<nz_; i++)
  {
    int thisCol = col_[row*nz_ +i];
    if(thisCol == -1) 
        whichCol = i;
    if(col == thisCol) 
        return i;
  }
  return whichCol;
}

template <class T> 
void SparseRowMatrix<T>::set(int row, int col, const T& val)
{
  if(ABS(val) < EPS)
    return;

  int whichCol = colIndex(row,col);
  if(whichCol < 0)
  {
      std::cerr << "Error in SparseRowMatrix::set: Entry could neither be found "
                << "nor newly allocated!\n";
  }
  else
  {
    values_[row*nz_ + whichCol] = val; 
    col_[row*nz_ + whichCol] = col;
  }
}

template <class T> 
void SparseRowMatrix<T>::add(int row, int col, const T& val)
{
  int whichCol = colIndex(row,col);
  if(whichCol < 0)
  {
    std::cerr << "Error in SparseRowMatrix::add: Entry could neither be found "
              << "nor newly allocated!\n";
  }
  else
  {
    values_[row*nz_ + whichCol] += val; 
    col_[row*nz_ + whichCol] = col;
  }
}

template <class T> 
void SparseRowMatrix<T>::multScalar(int row, int col, const T& val)
{
  int whichCol = colIndex(row,col);
  if(whichCol < 0)
  {
    std::cerr << "Error in SparseRowMatrix::multScalar: Entry could neither be found "
              << "nor newly allocated!\n";
  }
  else
  {
    values_[row*nz_ + whichCol] *= val; 
    col_[row*nz_ + whichCol] = col;
  }
}

template<class T>
const SparseRowMatrix<T>& SparseRowMatrix<T>::operator*=(const T& val)
{
    for(int i=0; i<dim_[0]*nz_; i++) { 
        values_[i] *= val;
    }
    return (*this);
}

/***************************************/
/*  Matrix-MV_Vector multiplication    */
/***************************************/
template <class T> template <class VECtype>
void SparseRowMatrix<T>::mult(VECtype &ret, const VECtype &x) const
{
    ret.resize(rows());
    
    for(int row=0; row<dim_[0]; row++) {
        
        T& sum = ret[row];
        sum = 0.0;
        for(int col=0; col<nz_; col++)
            {
                int thisCol = row*nz_ + col;
                int realCol = col_[ thisCol ];
                if ( realCol < 0 ) continue;
                sum += values_[thisCol] * x[ realCol ];
            }
        }
}

template <class T>
SimpleVector<T> SparseRowMatrix<T>::operator*(const SimpleVector<T>& v) const
{
    SimpleVector<T> result;
    mult(result, v);
    return result;
}

template <class T>
SparseRowMatrix<T> SparseRowMatrix<T>::applyFromLeftAndRightTo(const SparseRowMatrix<T>& A) const
{
    assert(A.rows() == A.cols());
    
    SparseRowMatrix<T> result(rows(), rows(), A.numNonZeros());
    
    for (int i=0; i<rows(); i++)
        for (int j=0; j<rows(); j++) {
            
            T sum = T(0);
            for (int k=0; k<numNonZeros(); k++) {
                
                int kCol = col_[i*nz_ + k];
                if (kCol >= 0) {
                    for (int l=0; l<numNonZeros(); l++) {
                        
                        int lCol = col_[j*nz_ + l];
                        if (lCol >= 0)
                            sum += values_[i*nz_ + k] * A(kCol, lCol) * values_[j*nz_ + l];
                    }
                    
                }
             
            }   
             
            result.set(i, j, sum);
                
        }
    
    
    
    return result;
}

/***************************************/
/*  Matrix-MV_Vector multiplication    */
/***************************************/
#if 0
template <class T> template <class DiscFuncType>
void SparseRowMatrix<T>::apply(const DiscFuncType &f, DiscFuncType &ret) const 
{
  typedef typename DiscFuncType::DofIteratorType DofIteratorType;  
  int level = f.getFunctionSpace().getGrid().maxlevel();

  //! we assume that the dimension of the functionspace of f is the same as
  //! the size of the matrix 
  DofIteratorType ret_it = ret.dbegin( level ); 
  DofIteratorType f_it = f.dbegin( level ); 

  for(int row=0; row<dim_[0]; row++)
  {
    (*ret_it) = 0.0;
    
    //! DofIteratorType schould be the same 
    for(int col=0; col<nz_; col++)
    {
      int thisCol = row*nz_ + col;
      int realCol = col_[thisCol];
      
      if( realCol < 0 ) continue;        
      (*ret_it) += values_[thisCol] * (f_it[realCol]);
    }

    ++ret_it;
  } 

  return; 
}
#endif

template <class T> template <class DiscFType , class DiscFuncType>
void SparseRowMatrix<T>::apply(const DiscFType &f, DiscFuncType &ret) const 
{
  typedef typename DiscFType::DofIteratorType DofFItType;  
  typedef typename DiscFuncType::DofIteratorType DofIteratorType;  
  int level = f.getFunctionSpace().getGrid().maxlevel();

  //! we assume that the dimension of the functionspace of f is the same as
  //! the size of the matrix 
  DofIteratorType ret_it = ret.dbegin( level ); 
  DofFItType f_it = f.dbegin( level ); 

  for(int row=0; row<dim_[0]; row++)
  {
    (*ret_it) = 0.0;
    
    //! DofIteratorType schould be the same 
    for(int col=0; col<nz_; col++)
    {
      int thisCol = row*nz_ + col;
      int realCol = col_[thisCol];
      
      if( realCol < 0 ) continue;        
      (*ret_it) += values_[thisCol] * (f_it[realCol]);
    }

    ++ret_it;
  } 

  return; 
}


// apply transposed matrix 
template <class T> template <class DiscFuncType>
void SparseRowMatrix<T>::apply_t(const DiscFuncType &f, DiscFuncType &ret) const 
{
  typedef typename DiscFuncType::DofIteratorType DofIteratorType;  
  int level = f.getFunctionSpace().getGrid().maxlevel();

  //! we assume that the dimension of the functionspace of f is the same as
  //! the size of the matrix 
  DofIteratorType ret_it = ret.dbegin( level ); 
  DofIteratorType f_it = const_cast<DiscFuncType &>(f).dbegin( level ); 

  for(int row=0; row<dim_[0]; row++)
  {
    (*ret_it) = 0.0;
    
    //! DofIteratorType should be the same 
    for(int col=0; col<nz_; col++)
    {
      int thisCol = col * nz_ + row;
      int realCol = col_[thisCol];
      
      if( realCol < 0 ) continue;        
      (*ret_it) += values_[thisCol] * (f_it[realCol]);
    }

    ++ret_it;
  } 

}

// apply to transposed matrix to a SimpleVector 
template <class T>
void SparseRowMatrix<T>::apply_t(const SimpleVector<T> &f, SimpleVector<T> &ret) const 
{
    assert(f.size() == rows());

    ret.resize(cols());
    ret.set(0);

    for (int row=0; row<rows(); row++) {
 
        for (int col=0; col<nz_; col++) {

            if (col_[row*nz_+col] >= 0)
                ret[col_[row*nz_+col]] += f[row]*values_[row*nz_+col];

        }
    }

}


template <class T> 
void SparseRowMatrix<T>::print(std::ostream& s) const
{
  for(int row=0; row<dim_[0]; row++)
  {
    for(int col=0; col<dim_[1]; col++)
    {
      s << (*this)(row,col) << " ";
    }
    s << "\n";
  }
}

template <class T> 
void SparseRowMatrix<T>::printReal(std::ostream& s) const
{
  for(int row=0; row<dim_[0]; row++)
  {
    for(int col=0; col<nz_; col++)
    {
        s << "(" << values_[row*nz_ + col] << ", " << col_[row*nz_+col] << ") ";
    }
    s << "\n";
  }
}

template <class T> 
void SparseRowMatrix<T>::kroneckerKill(int row, int col) 
{
  unitRow(row);
  unitCol(col);
} 

template <class T> 
void SparseRowMatrix<T>::unitRow(int row) 
{
  for(int i=1; i<nz_; i++) 
  {
    values_[row*nz_ + i] = 0.0;
    col_[row*nz_ + i] = -1; 
  }
  values_[row*nz_] = 1.0;
  col_[row*nz_] = row;
} 

template <class T> 
void SparseRowMatrix<T>::unitCol(int col) 
{
  for(int i=0; i<dim_[0]; i++) 
    if (i != col) 
        set(i,col,0.0); 
    else 
        set(col,col,1.0); 
} 

} // end namespace Dune


