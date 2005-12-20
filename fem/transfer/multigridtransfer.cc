#include <dune/common/fvector.hh>
#include <dune/istl/matrixindexset.hh>
#include <dune/disc/shapefunctions/lagrangeshapefunctions.hh>

template<class DiscFuncType>
template<class FunctionSpaceType>
void Dune::MultiGridTransfer<DiscFuncType>::setup(const FunctionSpaceType& coarseFSpace, 
                                                  const FunctionSpaceType& fineFSpace)
{
    int cL = coarseFSpace.level();
    int fL = fineFSpace.level();

    if (fL != cL+1)
        DUNE_THROW(Exception, "The two function spaces don't belong to consecutive levels!");

    typedef typename FunctionSpaceType::GridType GridType;
    const GridType& grid = coarseFSpace.grid();
    if (&grid != &(fineFSpace.grid()))
        DUNE_THROW(Exception, "The two function spaces don't belong to the same grid!");


    int rows = fineFSpace.size();
    int cols = coarseFSpace.size();

    // Make identity matrix
    MatrixBlock identity(0);
    for (int i=0; i<blocksize; i++)
        identity[i][i] = 1;

    // 
    OperatorType mat(rows, cols, OperatorType::random);

    mat = 0;

    typedef typename GridType::template Codim<0>::LevelIterator ElementIterator;
    typedef typename FunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;

    ElementIterator cIt    = grid.template lbegin<0>(cL);
    ElementIterator cEndIt = grid.template lend<0>(cL);


    // ///////////////////////////////////////////
    // Determine the indices present in the matrix
    // /////////////////////////////////////////////////
    MatrixIndexSet indices(rows, cols);

    for (; cIt != cEndIt; ++cIt) {

        const BaseFunctionSetType& coarseBaseSet = coarseFSpace.getBaseFunctionSet( *cIt );
        //const int numCoarseBaseFct = coarseBaseSet.getNumberOfBaseFunctions(); 
        const int numCoarseBaseFct = coarseBaseSet.numBaseFunctions(); 

        typedef typename GridType::template Codim<0>::Entity EntityType;
        typedef typename EntityType::HierarchicIterator HierarchicIterator;

        HierarchicIterator fIt    = cIt->hbegin(fL);
        HierarchicIterator fEndIt = cIt->hend(fL);

        for (; fIt != fEndIt; ++fIt) {

            if (fIt->level()==cIt->level())
                continue;
        
             const BaseFunctionSetType& fineBaseSet = fineFSpace.getBaseFunctionSet( *fIt );
             const int numFineBaseFct = fineBaseSet.numBaseFunctions(); 
            
            for (int i=0; i<numCoarseBaseFct; i++) {
                
                int globalCoarse = coarseFSpace.mapToGlobal(*cIt, i);
                
                for (int j=0; j<numFineBaseFct; j++) {
                    
                    int globalFine = fineFSpace.mapToGlobal(*fIt, j);
                    
                    FieldVector<double, GridType::dimension> local = cIt->geometry().local(fIt->geometry()[j]);
                    
                    // Evaluate coarse grid base function
                    typename FunctionSpaceType::RangeType value;
                    FieldVector<int, 0> diffVariable;
                    coarseBaseSet.evaluate(i, diffVariable, local, value);
                    
                    //std::cout << "value: " << value << "\n";
                    if (value[0] > 0.001) 
                        indices.add(globalFine, globalCoarse);

                }
                
                
            }
            
        }
        
    }

    indices.exportIdx(mat);

    // /////////////////////////////////////////////
    // Compute the matrix
    // /////////////////////////////////////////////
    cIt    = grid.template lbegin<0>(cL);
    for (; cIt != cEndIt; ++cIt) {

        const BaseFunctionSetType& coarseBaseSet = coarseFSpace.getBaseFunctionSet( *cIt );
        const int numCoarseBaseFct = coarseBaseSet.numBaseFunctions(); 


        typedef typename GridType::template Codim<0>::Entity EntityType;
        typedef typename EntityType::HierarchicIterator HierarchicIterator;

        HierarchicIterator fIt    = cIt->hbegin(fL);
        HierarchicIterator fEndIt = cIt->hend(fL);

        for (; fIt != fEndIt; ++fIt) {
        
            if (fIt->level()==cIt->level())
                continue;

            const BaseFunctionSetType& fineBaseSet = fineFSpace.getBaseFunctionSet( *fIt );
            const int numFineBaseFct = fineBaseSet.numBaseFunctions(); 
            
            for (int i=0; i<numCoarseBaseFct; i++) {
                
                int globalCoarse = coarseFSpace.mapToGlobal(*cIt, i);
                
                for (int j=0; j<numFineBaseFct; j++) {
                    
                    int globalFine = fineFSpace.mapToGlobal(*fIt, j);
                    
                    // Evaluate coarse grid base function at the location of the fine grid dof
                   
                    // first determine local fine grid dof position
                    FieldVector<double, GridType::dimension> local = cIt->geometry().local(fIt->geometry()[j]);
                    
                    // Evaluate coarse grid base function
                    typename FunctionSpaceType::RangeType value;
                    FieldVector<int, 0> diffVariable;
                    coarseBaseSet.evaluate(i, diffVariable, local, value);
                    
                    // The following conditional is a hack:  evaluation the coarse
                    // grid base function will often return 0.  However, we don't
                    // want explicit zero entries in our prolongation matrix.  Since
                    // the whole code works for P1-elements only anyways, we know
                    // that value can only be 0, 0.5, or 1.  Thus testing for nonzero
                    // by testing > 0.001 is safe.
                    if (value[0] > 0.001) {
                        MatrixBlock matValue = identity;
                        matValue *= value[0];
                        
                        mat[globalFine][globalCoarse] = matValue;
                    }

                }
                
                
            }
            
        }
        
    }

    matrix_ = mat;

}

template<class DiscFuncType>
template<class GridType>
void Dune::MultiGridTransfer<DiscFuncType>::setup(const GridType& grid, int cL, int fL)
{
    if (fL != cL+1)
        DUNE_THROW(Exception, "The two function spaces don't belong to consecutive levels!");

    const int dim = GridType::dimension;

    const typename GridType::Traits::LevelIndexSet& coarseIndexSet = grid.levelIndexSet(cL);
    const typename GridType::Traits::LevelIndexSet& fineIndexSet   = grid.levelIndexSet(fL);
 
    int rows = grid.size(fL, dim);
    int cols = grid.size(cL, dim);

    // Make identity matrix
    MatrixBlock identity(0);
    for (int i=0; i<blocksize; i++)
        identity[i][i] = 1;

    // 
    OperatorType mat(rows, cols, OperatorType::random);

    mat = 0;

    typedef typename GridType::template Codim<0>::LevelIterator ElementIterator;

    ElementIterator cIt    = grid.template lbegin<0>(cL);
    ElementIterator cEndIt = grid.template lend<0>(cL);


    // ///////////////////////////////////////////
    // Determine the indices present in the matrix
    // /////////////////////////////////////////////////
    MatrixIndexSet indices(rows, cols);

    for (; cIt != cEndIt; ++cIt) {

        const LagrangeShapeFunctionSet<double, double, dim>& coarseBaseSet 
            = Dune::LagrangeShapeFunctions<double, double, dim>::general(cIt->geometry().type(), 1);
        const int numCoarseBaseFct = coarseBaseSet.size();

        typedef typename GridType::template Codim<0>::Entity EntityType;
        typedef typename EntityType::HierarchicIterator HierarchicIterator;

        HierarchicIterator fIt    = cIt->hbegin(fL);
        HierarchicIterator fEndIt = cIt->hend(fL);

        for (; fIt != fEndIt; ++fIt) {

            if (fIt->level()==cIt->level())
                continue;
        
            const LagrangeShapeFunctionSet<double, double, dim>& fineBaseSet 
                = Dune::LagrangeShapeFunctions<double, double, dim>::general(fIt->geometry().type(), 1);
            const int numFineBaseFct = fineBaseSet.size();

            for (int i=0; i<numCoarseBaseFct; i++) {
                
                //int globalCoarse = cIt->template subIndex<dim>(i);
                int globalCoarse = coarseIndexSet.template subIndex<dim>(*cIt,i);

                for (int j=0; j<numFineBaseFct; j++) {
                    
                    //int globalFine = fIt->template subIndex<dim>(j);
                    int globalFine = fineIndexSet.template subIndex<dim>(*fIt,j);

                    FieldVector<double, GridType::dimension> local = cIt->geometry().local(fIt->geometry()[j]);
                    
                    // Evaluate coarse grid base function
                    double value = coarseBaseSet[i].evaluateFunction(0, local);

                    if (value > 0.001) 
                        indices.add(globalFine, globalCoarse);

                }
                
                
            }
            
        }
        
    }

    indices.exportIdx(mat);

    // /////////////////////////////////////////////
    // Compute the matrix
    // /////////////////////////////////////////////
    cIt    = grid.template lbegin<0>(cL);
    for (; cIt != cEndIt; ++cIt) {

        const LagrangeShapeFunctionSet<double, double, dim>& coarseBaseSet 
            = Dune::LagrangeShapeFunctions<double, double, dim>::general(cIt->geometry().type(), 1);
        const int numCoarseBaseFct = coarseBaseSet.size();

        typedef typename GridType::template Codim<0>::Entity EntityType;
        typedef typename EntityType::HierarchicIterator HierarchicIterator;

        HierarchicIterator fIt    = cIt->hbegin(fL);
        HierarchicIterator fEndIt = cIt->hend(fL);

        for (; fIt != fEndIt; ++fIt) {
        
            if (fIt->level()==cIt->level())
                continue;

            const LagrangeShapeFunctionSet<double, double, dim>& fineBaseSet 
                = Dune::LagrangeShapeFunctions<double, double, dim>::general(fIt->geometry().type(), 1);
            const int numFineBaseFct = fineBaseSet.size();

            for (int i=0; i<numCoarseBaseFct; i++) {
                
                //int globalCoarse = cIt->template subIndex<dim>(i);
                int globalCoarse = coarseIndexSet.template subIndex<dim>(*cIt,i);
                
                for (int j=0; j<numFineBaseFct; j++) {
                    
                    //int globalFine = fIt->template subIndex<dim>(j);
                    int globalFine = fineIndexSet.template subIndex<dim>(*fIt,j);
                    
                    // Evaluate coarse grid base function at the location of the fine grid dof
                   
                    // first determine local fine grid dof position
                    FieldVector<double, GridType::dimension> local = cIt->geometry().local(fIt->geometry()[j]);
                    
                    // Evaluate coarse grid base function
                    double value = coarseBaseSet[i].evaluateFunction(0, local);
                    
                    // The following conditional is a hack:  evaluating the coarse
                    // grid base function will often return 0.  However, we don't
                    // want explicit zero entries in our prolongation matrix.  Since
                    // the whole code works for P1-elements only anyways, we know
                    // that value can only be 0, 0.5, or 1.  Thus testing for nonzero
                    // by testing > 0.001 is safe.
                    if (value > 0.001) {
                        MatrixBlock matValue = identity;
                        matValue *= value;
                        
                        mat[globalFine][globalCoarse] = matValue;
                    }

                }
                
                
            }
            
        }
        
    }

    matrix_ = mat;

}

// Multiply the vector f from the right to the prolongation matrix
template<class DiscFuncType>
void Dune::MultiGridTransfer<DiscFuncType>::prolong(const DiscFuncType& f, DiscFuncType& t) const
{
    if (f.size() != matrix_.M())
        DUNE_THROW(Exception, "Number of entries in the coarse grid vector is not equal "
                   << "to the number of columns of the interpolation matrix!");

    t.resize(matrix_.N());

    typedef typename DiscFuncType::Iterator      Iterator;
    typedef typename DiscFuncType::ConstIterator ConstIterator;

    typedef typename OperatorType::row_type RowType;
    typedef typename RowType::ConstIterator ColumnIterator;
    
    Iterator tIt      = t.begin();
    ConstIterator fIt = f.begin(); 

    for(int rowIdx=0; rowIdx<matrix_.N(); rowIdx++) {

        const RowType& row = matrix_[rowIdx];

        *tIt = 0.0;
        ColumnIterator cIt    = row.begin();
        ColumnIterator cEndIt = row.end();

        for(; cIt!=cEndIt; ++cIt) {

            cIt->umv(f[cIt.index()], *tIt);

        }
        
        ++tIt;
    }

}

// Multiply the vector f from the right to the transpose of the prolongation matrix
template<class DiscFuncType>
void Dune::MultiGridTransfer<DiscFuncType>::restrict(const DiscFuncType& f, DiscFuncType& t) const
{
    if (f.size() != matrix_.N())
        DUNE_THROW(Exception, "Fine grid vector has " << f.size() << " entries "
                   << "but the interpolation matrix has " << matrix_.N() << " rows!");
    
    t.resize(matrix_.M());
    t = 0;

    typedef typename DiscFuncType::Iterator      Iterator;
    typedef typename DiscFuncType::ConstIterator ConstIterator;

    typedef typename OperatorType::row_type RowType;
    typedef typename RowType::ConstIterator ColumnIterator;

    Iterator tIt      = t.begin();
    ConstIterator fIt = f.begin();
    
    for (int rowIdx=0; rowIdx<matrix_.N(); rowIdx++) {
        
        const RowType& row = matrix_[rowIdx];

        ColumnIterator cIt    = row.begin();
        ColumnIterator cEndIt = row.end();
        
        for(; cIt!=cEndIt; ++cIt) {

            cIt->umtv(f[rowIdx], t[cIt.index()]);
            
        }

    }
 
}


// Multiply the vector f from the right to the transpose of the prolongation matrix
template<class DiscFuncType>
void Dune::MultiGridTransfer<DiscFuncType>::restrict(const Dune::BitField& f, Dune::BitField& t) const
{
    if (f.size() != (unsigned int)matrix_.N())
        DUNE_THROW(Exception, "Fine grid bitfield has " << f.size() << " entries "
                   << "but the interpolation matrix has " << matrix_.N() << " rows!");
    
    t.resize(matrix_.M());
    t.unsetAll();

    typedef typename OperatorType::row_type RowType;
    typedef typename RowType::ConstIterator ColumnIterator;

    for (int rowIdx=0; rowIdx<matrix_.N(); rowIdx++) {
    
        if (!f[rowIdx])
            continue;

        const RowType& row = matrix_[rowIdx];

        ColumnIterator cIt    = row.begin();
        ColumnIterator cEndIt = row.end();
        
        for(; cIt!=cEndIt; ++cIt)
            t[cIt.index()] = true;

    }
 
}


template<class DiscFuncType>
void Dune::MultiGridTransfer<DiscFuncType>::
galerkinRestrict(const OperatorType& fineMat, OperatorType& coarseMat) const
{
  // ////////////////////////
  // Nonsymmetric case
  // ////////////////////////
  typedef typename OperatorType::row_type RowType;
  typedef typename RowType::Iterator ColumnIterator;
  typedef typename RowType::ConstIterator ConstColumnIterator;

  // Clear coarse matrix
  coarseMat = 0;

  // Loop over all rows of the stiffness matrix
  for (int v=0; v<fineMat.N(); v++) {
      
      const RowType& row = fineMat[v];

      // Loop over all columns of the stiffness matrix
      ConstColumnIterator m    = row.begin();
      ConstColumnIterator mEnd = row.end();
        
      for (; m!=mEnd; ++m) {

          int w = m.index();

          // Loop over all coarse grid vectors iv that have v in their support
          ConstColumnIterator im    = matrix_[v].begin();
          ConstColumnIterator imEnd = matrix_[v].end();
          for (; im!=imEnd; ++im) {

              int iv = im.index();

              // Loop over all coarse grid vectors jv that have w in their support
              ConstColumnIterator jm = matrix_[w].begin();
              ConstColumnIterator jmEnd = matrix_[w].end();
              
              for (; jm!=jmEnd; ++jm) {
                  
                  int jv = jm.index();
                  
                  MatrixBlock& cm = coarseMat[iv][jv];

                  // Compute cm = im^T * m * jm
                  for (int i=0; i<blocksize; i++) {
                      
                      for (int j=0; j<blocksize; j++) {
                          
                          for (int k=0; k<blocksize; k++) {
                              
                              for (int l=0; l<blocksize; l++) 
                                  cm[i][j] += (*im)[k][i] * (*m)[k][l] * (*jm)[l][j];
                              
                          }
                          
                      }
                      
                  }

              }

          }

      }

  }
  
}


// Set Occupation of Galerkin restricted coarse stiffness matrix
template<class DiscFuncType>
void Dune::MultiGridTransfer<DiscFuncType>::
galerkinRestrictSetOccupation(const OperatorType& fineMat, OperatorType& coarseMat) const
{
    // ////////////////////////
    // Nonsymmetric case
    // ////////////////////////
	typedef typename OperatorType::row_type RowType;
    typedef typename RowType::ConstIterator ConstColumnIterator;
    
	
	// Create index set
    Dune::MatrixIndexSet indices(matrix_.M(), matrix_.M());

    // Loop over all rows of the fine matrix
    for (int v=0; v<fineMat.N(); v++) {
        
        const RowType& row = fineMat[v];
        
        // Loop over all columns of the fine matrix
        ConstColumnIterator m    = row.begin();
        ConstColumnIterator mEnd = row.end();
        
        for (; m!=mEnd; ++m) {
            
            int w = m.index();
            
            // Loop over all coarse grid vectors iv that have v in their support
            ConstColumnIterator im    = matrix_[v].begin();
            ConstColumnIterator imEnd = matrix_[v].end();
            for (; im!=imEnd; ++im) {
                
                int iv = im.index();
                
                // Loop over all coarse grid vectors jv that have w in their support
                ConstColumnIterator jm    = matrix_[w].begin();
                ConstColumnIterator jmEnd = matrix_[w].end();
                
                for (; jm!=jmEnd; ++jm) 
                    indices.add(iv, jm.index());
                
            }
            
        }
        
    }

    indices.exportIdx(coarseMat);
}







/***************************************************************************/
/***************************************************************************/
/*  The following functions are the deprecated ones for DiscFuncArrays     */
/***************************************************************************/
/***************************************************************************/

template<class DiscFuncType>
void Dune::MultiGridTransfer<DiscFuncType>::prolongDFA(const DiscFuncType& f, DiscFuncType& t) const
{
    assert(t.getFunctionSpace().size() == matrix_.N());
    assert(f.getFunctionSpace().size() == matrix_.M());

    typedef typename DiscFuncType::DofIteratorType           Iterator;  
    typedef typename DiscFuncType::ConstDofIteratorType      ConstIterator;  

    typedef typename OperatorType::row_type RowType;
    typedef typename RowType::ConstIterator ConstColumnIterator;
    
    Iterator tIt = t.dbegin(); 
    ConstIterator fIt = f.dbegin(); 

    for(int row=0; row<matrix_.N(); row++) {

        *tIt = 0.0;
        
        ConstColumnIterator cIt    = matrix_[row].begin();
        ConstColumnIterator cEndIt = matrix_[row].end();
        
        for(; cIt!=cEndIt; ++cIt)
            *tIt += *cIt * fIt[cIt.index()];
        
        ++tIt;
    } 
}

template<class DiscFuncType>
void Dune::MultiGridTransfer<DiscFuncType>::restrictDFA(const DiscFuncType& f, DiscFuncType& t) const
{
    assert(f.getFunctionSpace().size() == matrix_.N());
    assert(t.getFunctionSpace().size() == matrix_.M());
    
    typedef typename DiscFuncType::DofIteratorType           IteratorType;  
    typedef typename DiscFuncType::ConstDofIteratorType      ConstIteratorType;  
    typedef typename OperatorType::row_type RowType;
    typedef typename RowType::ConstIterator ConstColumnIterator;

    t.clear();
    
    IteratorType tIt = t.dbegin(); 
    ConstIteratorType fIt = f.dbegin(); 

    
    for (int row=0; row<matrix_.N(); row++) {
        
        ConstColumnIterator cIt    = matrix_[row].begin();
        ConstColumnIterator cEndIt = matrix_[row].end();

        for(; cIt!=cEndIt; ++cIt) 
            tIt[cIt.index()] += fIt[row] * (*cIt);

    }
    
}


template<class DiscFuncType>
Dune::SparseRowMatrix<double> Dune::MultiGridTransfer<DiscFuncType>::
galerkinRestrict(const Dune::SparseRowMatrix<double>& fineMat) const
{
    typedef typename OperatorType::row_type RowType;
    //typedef typename RowType::Iterator ColumnIterator;
    typedef typename RowType::ConstIterator ConstColumnIterator;

    typedef typename SparseRowMatrix<double>::ColumnIterator ColumnIterator;

    SparseRowMatrix<double> result(matrix_.M(), matrix_.M() , fineMat.numNonZeros());
    
    for (int rowIdx=0; rowIdx<fineMat.rows(); rowIdx++) {

        ColumnIterator cIt    = const_cast<SparseRowMatrix<double>&>(fineMat).template rbegin(rowIdx);
        ColumnIterator cEndIt = const_cast<SparseRowMatrix<double>&>(fineMat).template rend(rowIdx);

        for(; cIt!=cEndIt; ++cIt) {

            double mvalue = *cIt;

            const RowType& row = matrix_[rowIdx];

//             ColumnIterator tciIt    = const_cast<SparseRowMatrix<double>&>(matrix_).template rbegin(rowIdx);
//             ColumnIterator tciEndIt = const_cast<SparseRowMatrix<double>&>(matrix_).template rend(rowIdx);
            ConstColumnIterator tciIt    = row.begin();
            ConstColumnIterator tciEndIt = row.end();

            for (; tciIt!=tciEndIt; ++tciIt) {

                double fac = mvalue * ((*tciIt)[0][0]);

                //ColumnIterator tcjIt    = const_cast<SparseRowMatrix<double>&>(matrix_).template rbegin(cIt.col());
                //ColumnIterator tcjEndIt = const_cast<SparseRowMatrix<double>&>(matrix_).template rend(cIt.col());
                ConstColumnIterator tcjIt    = matrix_[cIt.col()].begin();
                ConstColumnIterator tcjEndIt = matrix_[cIt.col()].end();

                for (; tcjIt!=tcjEndIt; ++tcjIt)
                    result.add( tcjIt.index(), tciIt.index(), fac * ((*tcjIt)));

            }

        }


    }

    return result;
}
