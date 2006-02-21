#ifndef DUNE_MASSMATRIX_HH
#define DUNE_MASSMATRIX_HH

#include <dune/fem/feoperator.hh>
#include <dune/fem/feop/spmatrix.hh>
#include <dune/istl/bcrsmatrix.hh>
#include <dune/istl/matrixindexset.hh>

#include <dune/quadrature/quadraturerules.hh>

namespace Dune {
  
  /** \brief The mass matrix
   *
   * \tparam polOrd The quadrature order
   *
   * \todo Giving the quadrature order as a template parameter is
   * a hack.  It would be better to determine the optimal order automatically.
   */
  template <class DiscFunctionType, int polOrd>
  class MassMatrixFEOp :
    
    public FiniteElementOperator<DiscFunctionType,
                                 SparseRowMatrix<double>,
                                 MassMatrixFEOp<DiscFunctionType, polOrd> > {
    typedef typename DiscFunctionType::FunctionSpaceType FunctionSpaceType;
    typedef typename FiniteElementOperator<DiscFunctionType,
                                           SparseRowMatrix<double>,
                                           MassMatrixFEOp<DiscFunctionType, polOrd> >::OpMode OpMode;
    
    typedef typename FunctionSpaceType::RangeField RangeFieldType;
    typedef typename FunctionSpaceType::GridType GridType;
    typedef typename FunctionSpaceType::Range RangeType;
    
  public:

    //! Returns the actual matrix if it is assembled
    /** \todo Should this be in a base class? */
    const SparseRowMatrix<double>* getMatrix() const {
      assert(this->matrix_);
      return this->matrix_;
    }

    //! Constructor
    MassMatrixFEOp( const typename DiscFunctionType::FunctionSpace &f, OpMode opMode )://= ON_THE_FLY ) :
      FiniteElementOperator<DiscFunctionType,
                            SparseRowMatrix<double>,
                            MassMatrixFEOp<DiscFunctionType, polOrd> >( f, opMode ) 
    {}
        
    //! ???
    SparseRowMatrix<double>* newEmptyMatrix( ) const {
      return new SparseRowMatrix<double>( this->functionSpace_.size () , 
                                          this->functionSpace_.size () , 
                                          30);
    }
        
    //! ???
    template <class EntityType>
    double getLocalMatrixEntry( EntityType &entity, const int i, const int j ) const 
    {
      typedef typename FunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;
      const int dim = GridType::dimension;
      double val = 0;
      
      FieldVector<double, dim> tmp(1.0);
      
      const BaseFunctionSetType & baseSet 
        = this->functionSpace_.getBaseFunctionSet( entity );
      
      RangeType v1 (0.0);
      RangeType v2 (0.0);

      // Get quadrature rule
      const QuadratureRule<double, dim>& quad = QuadratureRules<double, dim>::rule(entity.geometry().type(), polOrd);
      
      const double vol = entity.geometry().integrationElement( tmp ); 
      for ( int pt=0; pt < quad.size(); pt++ ) 
        {      
          baseSet.eval(i,quad,pt,v1); 
          baseSet.eval(j,quad,pt,v2); 
          val += ( v1 * v2 ) * quad[pt].weight();
        }
      
      return val*vol;
    }
    
    //! ???
    template < class  EntityType, class MatrixType>
    void getLocalMatrix( EntityType &entity, const int matSize, MatrixType& mat) const 
    {
      int i,j;
      typedef typename FunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;
      const int dim = GridType::dimension;
      const BaseFunctionSetType & baseSet 
        = this->functionSpace_.getBaseFunctionSet( entity );
      
      /** \todo What's the correct type here? */
      static FieldVector<double, dim> tmp(1.0);
      const double vol = entity.geometry().integrationElement(tmp);
      
      static RangeType v[30];
      // Check magic constant. Otherwise program will fail in loop below
      assert(matSize <= 30); 

      for(i=0; i<matSize; i++) 
        for (j=0; j<=i; j++ ) 
          mat[j][i]=0.0;
      
      // Get quadrature rule
      const QuadratureRule<double, dim>& quad = QuadratureRules<double, dim>::rule(entity.geometry().type(), polOrd);
      
      for ( int pt=0; pt < quad.size(); pt++ )
        {
          for(i=0; i<matSize; i++) 
            baseSet.eval(i,quad,pt,v[i]); 
          
          for(i=0; i<matSize; i++) 
            for (j=0; j<=i; j++ ) 
              mat[j][i] += ( v[i] * v[j] ) * quad[pt].weight();
        }
      
      for(i=0; i<matSize; i++) 
        for (j=0; j<=i; j++ ) 
          mat[j][i] *= vol;
      
      for(i=0; i<matSize; i++) 
        for (j=matSize; j>i; j--) 
          mat[j][i] = mat[i][j];
      
      // * for debugging only
      //      for (int i = 0; i < matSize; ++i) {
      //assert(mat[i][i] > 0.0039 && mat[i][i] < 0.004);
      //}
    }

  protected:
    DiscFunctionType *_tmp;
  };






    /** \brief The mass matrix operator for block matrices
     *
     * \tparam polOrd The quadrature order
     *
     * This is an implementation of the mass matrix operator using the ISTL-classes
     *
     * \todo Giving the quadrature order as a template parameter is
     * a hack.  It would be better to determine the optimal order automatically.
     */
    template <class GridType, int blocksize, int polOrd>
    class MassMatrix {
        
        enum {dim = GridType::dimension};

        enum {elementOrder = 1};

        typedef typename GridType::template Codim<0>::Entity EntityType;

        //!
        typedef FieldMatrix<double, blocksize, blocksize> MatrixBlock;

public:

        /** \todo Does actually belong into the base class */
        BCRSMatrix<MatrixBlock>* matrix_;
        
        /** \todo Does actually belong into the base class */
        const GridType* grid_;
        
        //! ???
        MassMatrix(const GridType &grid) : 
            grid_(&grid)
        {}
        
        //! Returns the actual matrix if it is assembled
        const BCRSMatrix<MatrixBlock>* getMatrix() const {
            assert(this->matrix_);
            return this->matrix_;
        }
        
        void getNeighborsPerVertex(MatrixIndexSet& nb) {
            
            int i, j;
            const typename GridType::Traits::LevelIndexSet& indexSet = grid_->levelIndexSet(grid_->maxLevel());
            int n = indexSet.size(dim);
            
            nb.resize(n, n);
            
            typedef typename GridType::template Codim<0>::LevelIterator LevelIterator;
            LevelIterator it = grid_->template lbegin<0>( grid_->maxLevel() );
            LevelIterator endit = grid_->template lend<0> ( grid_->maxLevel() );

            for (; it!=endit; ++it) {

                for (i=0; i<it->template count<dim>(); i++) {

                    for (j=0; j<it->template count<dim>(); j++) {

                        int iIdx = indexSet.template subIndex<dim>(*it, i);
                        int jIdx = indexSet.template subIndex<dim>(*it, j);

                        nb.add(iIdx, jIdx);

                    }

                }

            }

        }

        void assembleMatrix() {
            
            typedef typename GridType::template Codim<0>::LevelIterator LevelIterator;
            
            const typename GridType::Traits::LevelIndexSet& indexSet = grid_->levelIndexSet(grid_->maxLevel());
            int n = indexSet.size(GridType::dimension);
            
            MatrixIndexSet neighborsPerVertex;
            getNeighborsPerVertex(neighborsPerVertex);
            
            matrix_ = new BCRSMatrix<MatrixBlock>(n, n, BCRSMatrix<MatrixBlock>::random);
            
            neighborsPerVertex.exportIdx(*matrix_);
            (*matrix_) = 0;
            
            LevelIterator it = grid_->template lbegin<0>( grid_->maxLevel() );
            LevelIterator endit = grid_->template lend<0> ( grid_->maxLevel() );
            enum {maxnumOfBaseFct = 30};
            
            Matrix<MatrixBlock> mat;
            
            for( ; it != endit; ++it ) {
                
                //                 const BaseFunctionSetType & baseSet = functionSpace_.getBaseFunctionSet( *it );
                //                 const int numOfBaseFct = baseSet.numBaseFunctions();  
                const LagrangeShapeFunctionSet<double, double, dim> & baseSet 
                    = Dune::LagrangeShapeFunctions<double, double, dim>::general(it->geometry().type(), elementOrder);

            const int numOfBaseFct = baseSet.size();
                //printf("%d base functions on element\n", numOfBaseFct);
                mat.resize(numOfBaseFct, numOfBaseFct);
                // setup matrix 
                getLocalMatrix( *it, numOfBaseFct, mat);
                
                for(int i=0; i<numOfBaseFct; i++) { 
                    
                    //int row = functionSpace_.mapToGlobal( *it , i );
                    int row = indexSet.template subIndex<dim>(*it,i);
                    for (int j=0; j<numOfBaseFct; j++ ) {
                        
                        //int col = functionSpace_.mapToGlobal( *it , j );    
                        int col = indexSet.template subIndex<dim>(*it,j);
                        (*matrix_)[row][col] += mat[i][j];
                        
                    }
                }
            }
            
        }
        
        //! ???
        template < class  EntityType, class MatrixType>
        void getLocalMatrix( EntityType &entity, const int matSize, MatrixType& mat) const 
        {
            int i,j;
            const LagrangeShapeFunctionSet<double, double, dim> & baseSet 
                = Dune::LagrangeShapeFunctions<double, double, dim>::general(entity.geometry().type(), elementOrder);            
            // Clear local scalar matrix
            Matrix<double> scalarMat(matSize, matSize);
            for(i=0; i<matSize; i++) 
                for (j=0; j<=i; j++ ) 
                    scalarMat[i][j]=0.0;

            // Get quadrature rule
            const QuadratureRule<double, dim>& quad = QuadratureRules<double, dim>::rule(entity.geometry().type(), polOrd);
            
            for (unsigned int pt=0; pt < quad.size(); pt++ ) {

                // Get position of the quadrature point
                const FieldVector<double,dim>& quadPos = quad[pt].position();

                // The factor in the integral transformation formula
                const double integrationElement = entity.geometry().integrationElement(quadPos);
                
                double v[matSize];
                for(i=0; i<matSize; i++) 
                    v[i] = baseSet[i].evaluateFunction(0,quadPos); 
                
                for(i=0; i<matSize; i++) 
                    for (int j=0; j<=i; j++ ) 
                        scalarMat[i][j] += ( v[i] * v[j] ) * quad[pt].weight() * integrationElement;
            }

            // Clear local matrix
            for(i=0; i<matSize; i++) 
                for (j=0; j<matSize; j++)
                    mat[i][j] = 0;

            // Turn scalar triangular matrix into symmetric block matrix
            for(i=0; i<matSize; i++) 
                for (j=0; j<=i; j++) 
                    for (int k=0; k<blocksize; k++)
                        mat[i][j][k][k] = mat[j][i][k][k] = scalarMat[i][j];
            
        }
        
        
    }; // end class
    



} // namespace Dune

#endif
