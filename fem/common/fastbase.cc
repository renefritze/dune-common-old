
//template <class FunctionSpaceType, class FastBaseFunctionSetImp>
//FastBaseFunctionSet<FunctionSpaceType, FastBaseFunctionSetImp >::

template <class FunctionSpaceType>
FastBaseFunctionSet<FunctionSpaceType >::
FastBaseFunctionSet( FunctionSpaceType & fuspace, int numOfBaseFct )
  : BaseFunctionSetDefault <FunctionSpaceType,FastBaseFunctionSet <FunctionSpaceType > > ( fuspace ),
  vecEvaluate_( numDiffOrd ) {
  for(int i=0; i<numDiffOrd; i++)
    evaluateQuad_[i] = DynamicType::undefined;

  baseFunctionList_.resize( numOfBaseFct );

  for(int i=0; i<numOfBaseFct; i++)
    baseFunctionList_[i] = NULL; 
  
}

template <class FunctionSpaceType> template <int diffOrd>
void FastBaseFunctionSet<FunctionSpaceType >::
evaluate( int baseFunct, const FieldVector<deriType, diffOrd> &diffVariable, const Domain & x,  Range & phi ) const 
{
  getBaseFunction( baseFunct ).evaluate( diffVariable, x, phi );
}

template <class FunctionSpaceType> template <int diffOrd, class QuadratureType>
void FastBaseFunctionSet<FunctionSpaceType >::
evaluate( int baseFunct, const FieldVector<deriType, diffOrd> &diffVariable, QuadratureType & quad, 
        int quadPoint, Range & phi ) const 
{   
  // check if cache is for the used quadrature 
  if ( quad.getIdentifier() != evaluateQuad_[ diffOrd ] ) 
  {
    const_cast<FastBaseFunctionSet<FunctionSpaceType >& > 
                      (*this).evaluateInit<diffOrd>(quad);  
  }

  int idex = index( baseFunct, diffVariable, quadPoint, 
                      quad.nop());
  //! copy the value to phi 
  phi = vecEvaluate_[diffOrd][ idex ];
}


//! if the quadrature is new, then once cache the values 
template <class FunctionSpaceType>
template <int diffOrd, class QuadratureType>
void FastBaseFunctionSet<FunctionSpaceType >::
evaluateInit( const QuadratureType &quad ) 
{
  int p = 1;
  for ( int i = 0; i < diffOrd; i++ ) { p *= DimDomain; }
  
  // resize the cache vector if nessesary 
  vecEvaluate_[ diffOrd ].resize( p * baseFunctionList_.size() * quad.nop () );
 
  // cache the basefunction evaluation 
  typename DiffVariable<diffOrd>::Type diffVariable (0);
  int count = index( 0, diffVariable, 0, quad.nop ());

  int diffCount = 0;
  // for all posible diffVariable values 
  for(int i=0; i<p; i++)
  {
    // go through all possible combinations of 0,1,2 
    if(diffOrd == 2)
    {
      diffVariable[0] = i % DimDomain;
      diffVariable[1] = diffCount;
      diffCount++; 
      if (diffCount >= DimDomain) diffCount = 0;
    }
    else 
      for(int j=0; j<diffOrd; j++) 
          diffVariable[j] = i % DimDomain;
    
    for ( int baseFunc = 0; baseFunc < this->getNumberOfBaseFunctions(); baseFunc++ ) 
    {
      for ( int quadPt = 0; quadPt < quad.nop(); quadPt++ ) 
      {
        getBaseFunction( baseFunc ).evaluate( diffVariable, quad.point( quadPt ), 
               vecEvaluate_[ diffOrd ][ count ] );
        count++;
      }
    }
  }

  // remember the cached quadrature 
  evaluateQuad_[ diffOrd ] = quad.getIdentifier();
}

