#ifndef __DUNE_LAPLACE_HH__
#define __DUNE_LAPLACE_HH__

#include <dune/fem/feoperator.hh>
#include <dune/fem/feop/spmatrix.hh>

#include <dune/quadrature/fixedorder.hh>

namespace Dune 
{

    /** \brief The Laplace operator
     */
    template <class DiscFunctionType, class TensorType, int polOrd>
    class LaplaceFEOp : 
        public FiniteElementOperator<DiscFunctionType,
                                     SparseRowMatrix<double>,
                                     LaplaceFEOp<DiscFunctionType,TensorType, polOrd> > {
        
        //! The corresponding function space type
        typedef typename DiscFunctionType::FunctionSpaceType FunctionSpaceType;

        //! The grid
        typedef typename FunctionSpaceType::GridType GridType;

        //! The grid's dimension
        enum { dim = GridType::dimension };

        //! ???
        typedef typename FunctionSpaceType::JacobianRange JacobianRange;

        //! ???
        typedef typename FunctionSpaceType::RangeField RangeFieldType;
        
        //! ???
        typedef typename FiniteElementOperator<DiscFunctionType,
                                               SparseRowMatrix<double>,
                                               LaplaceFEOp<DiscFunctionType, TensorType, polOrd> >::OpMode OpMode;
        
        
        mutable JacobianRange grad;
        mutable JacobianRange othGrad;
        mutable JacobianRange mygrad[4];
        
    public:

        //! ???
        FixedOrderQuad < typename FunctionSpaceType::RangeField, typename
                   FunctionSpaceType::Domain , polOrd > quad;
        
        //! ???
        DiscFunctionType *stiffFunktion_;

        //! ???
        TensorType *stiffTensor_;
        
        //! ???
        LaplaceFEOp( const typename DiscFunctionType::FunctionSpace &f, OpMode opMode ): 
            FiniteElementOperator<DiscFunctionType,SparseRowMatrix<double>,LaplaceFEOp<DiscFunctionType,TensorType, polOrd> >( f, opMode ) , 
            quad ( *(f.getGrid().template lbegin<0> (0))), stiffFunktion_(NULL), stiffTensor_(NULL)
        {
        }
        
        //! ???
        LaplaceFEOp( const DiscFunctionType &stiff, const typename DiscFunctionType::FunctionSpace &f, OpMode opMode ): 
            FiniteElementOperator<DiscFunctionType,SparseRowMatrix<double>,LaplaceFEOp<DiscFunctionType,TensorType, polOrd> >( f, opMode ) ,
            quad ( *(f.getGrid().template lbegin<0> (0))), stiffFunktion_(&stiff), stiffTensor_(NULL)
        { 
        }
        
        //! ???
        LaplaceFEOp( TensorType &stiff, const typename DiscFunctionType::FunctionSpace &f, OpMode opMode ): 
            FiniteElementOperator<DiscFunctionType,SparseRowMatrix<double>,LaplaceFEOp<DiscFunctionType,TensorType, polOrd> >( f, opMode ) ,
            quad ( *(f.getGrid().template lbegin<0> (0))), stiffFunktion_(NULL), stiffTensor_(&stiff)
        { 
        }
        
        //! Returns the actual matrix if it is assembled
        const SparseRowMatrix<double>* getMatrix() const {
            assert(this->matrix_);
            return this->matrix_;
        }
        
        //! Creates a new empty matrix
        SparseRowMatrix<double>* newEmptyMatrix( ) const 
        {
            return new SparseRowMatrix<double>( this->functionSpace_.size () , 
                                                this->functionSpace_.size () , 
                                                15 * dim);
        }
        
        //! Prepares the local operator before calling apply()
        void prepareGlobal ( const DiscFunctionType &arg, DiscFunctionType &dest ) 
        {
            this->arg_  = &arg.argument();
            this->dest_ = &dest.destination();
            assert(this->arg_ != 0); assert(this->dest_ != 0);
            dest.clear();
        }
        
        //! ???
        template <class EntityType>
        double getLocalMatrixEntry( EntityType &entity, const int i, const int j ) const 
        {
            enum { dim = GridType::dimension };
            typedef typename FunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;
            
            const BaseFunctionSetType & baseSet = this->functionSpace_.getBaseFunctionSet( entity );
            
            double val = 0.;
            for ( int pt=0; pt < quad.nop(); pt++ ) 
                {      
                    baseSet.jacobian(i,quad,pt,grad); 
                    
                    // calc Jacobian inverse before volume is evaluated 
                    const FieldMatrix<double,dim,dim>& inv = entity.geometry().jacobianInverse(quad.point(pt));
                    const double vol = entity.geometry().integrationElement(quad.point(pt)); 
            
                    // multiply with transpose of jacobian inverse 
                    //grad[0] = inv.mult_t (grad[pt]);
                    JacobianRange tmp(0);
                    inv.umtv(grad[0], tmp[0]);
                    grad[0] = tmp[0];
                    
                    if( i != j ) 
                        {
                            baseSet.jacobian(j,quad,pt,othGrad); 
                            
                            // multiply with transpose of jacobian inverse 
                            //othGrad[0] = inv.mult_t(othGrad[0]);
                            JacobianRange tmp(0);
                            inv.umtv(othGrad[0], tmp[0]);
                            othGrad[0] = tmp[0];

                            val += ( grad[0] * othGrad[0] ) * quad.weight( pt ) * vol;
                        }
                    else 
                        {
                            val += ( grad[0] * grad[0] ) * quad.weight( pt ) * vol;
                        }
                }

            return val;
        }
        
        //! ???
        template < class  EntityType, class MatrixType>
        void getLocalMatrix( EntityType &entity, const int matSize, MatrixType& mat) const {
            enum { dim = GridType::dimension };

            typedef typename FunctionSpaceType::BaseFunctionSetType BaseFunctionSetType;
            
            const BaseFunctionSetType & baseSet = this->functionSpace_.getBaseFunctionSet( entity );
            
            int i,j;
            
            for(i=0; i<matSize; i++) 
                for (j=0; j<=i; j++ ) 
                    mat[j][i]=0.0;
            
            for ( int pt=0; pt < quad.nop(); pt++ ) {

                // calc Jacobian inverse before volume is evaluated 
                const FieldMatrix<double,dim,dim>& inv = entity.geometry().jacobianInverse(quad.point(pt));
                const double vol = entity.geometry().integrationElement(quad.point(pt));

                for(i=0; i<matSize; i++) {
      
                    baseSet.jacobian(i,quad,pt,mygrad[i]); 
      
                    // multiply with transpose of jacobian inverse 
                    //mygrad[i][0] = inv.mult_t (mygrad[i][0]);
                    JacobianRange tmp(0);
                    inv.umtv(mygrad[i][0], tmp[0]);
                    mygrad[i][0] = tmp[0];
                }
                    
                typename FunctionSpaceType::Range ret;
                    
                if(stiffTensor_){
                    stiffTensor_->evaluate(entity.geometry().global(quad.point(pt)),ret);
                    ret[0] *= quad.weight( pt );
                    for(i=0; i<matSize; i++) 
                        for (j=0; j<=i; j++ ) 
                            mat[j][i] += ( mygrad[i][0] * mygrad[j][0] ) * ret[0] * vol;
                }
                else{
                    for(i=0; i<matSize; i++) 
                        for (j=0; j<=i; j++ ) 
                            mat[j][i] += ( mygrad[i][0] * mygrad[j][0] ) * quad.weight( pt ) * vol;
                }
                
                
                
            }
            
            for(i=0; i<matSize; i++) 
                for (j=matSize; j>i; j--) 
                    mat[j][i] = mat[i][j];
            
        }
        
    }; // end class
    
} // end namespace 

#endif

