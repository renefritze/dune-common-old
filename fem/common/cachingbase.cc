
//template <class FunctionSpaceType, class FastBaseFunctionSetImp>
//CachingBaseFunctionSet<FunctionSpaceType, CachingBaseFunctionSetImp >::

template <class FunctionSpaceType>
CachingBaseFunctionSet<FunctionSpaceType>::
CachingBaseFunctionSet(FunctionSpaceType& fuspace,
                       ElementType elType,
                       int nBaseFnc) :
  BaseFunctionSetDefault<FunctionSpaceType,
                         CachingBaseFunctionSet<FunctionSpaceType> > (fuspace),
  numOfBaseFct_(nBaseFnc)
{
  // Create the necessary base functions
  baseFunctionList_.resize(numOfBaseFct_);
  for (int i = 0; i < numOfBaseFct_; ++i) {
    baseFunctionList_[i] = fuspace.getBaseFunction(elType, i);
  }
}

template <class FunctionSpaceType>
CachingBaseFunctionSet<FunctionSpaceType>::
~CachingBaseFunctionSet() {
  for (int i = 0; i < numOfBaseFct_; ++i) {
    delete baseFunctionList_[i];
  }
}

template <class FunctionSpaceType> 
template <int diffOrd>
void CachingBaseFunctionSet<FunctionSpaceType >::
evaluate(int baseFunct, 
         const FieldVector<deriType, diffOrd> &diffVariable, 
         const Domain & x,
         Range & phi) const 
{
  assert(baseFunct < numOfBaseFct_);
  baseFunctionList_[baseFunct]->evaluate(diffVariable, x, phi);
}

template <class FunctionSpaceType> 
template <int diffOrd, class QuadratureType>
void CachingBaseFunctionSet<FunctionSpaceType >::
evaluate(int baseFunct, 
         const FieldVector<deriType, diffOrd> &diffVariable, 
         QuadratureType & quad, 
         int quadPoint, 
         Range & phi ) const 
{   
  switch (diffOrd) {
  case 0:
    phi = values(baseFunct, quad)[quadPoint];
    break;
  case 1:
    phi = extractGradientComp(gradients(baseFunct, quad)[quadPoint],
                              diffVariable[0]);
    break;
  default:
    DUNE_THROW(NotImplemented, "Sorry, only derivatives up to first order allowed");
  }
}

template <class FunctionSpaceType>
template <class QuadratureType>
const std::vector<FunctionSpaceType::Range>&
CachingBaseFunctionSet<FunctionSpaceType >::
values(int baseFunct, const QuadratureType& quad) const {
  ConstRangeMapIterator it = vals_.find(quad.getIdentifier());

  assert(it != vals_.end()); // Forgot to initialise with quadrature, eh?
  assert(baseFunct < numOfBaseFct_);

  return it->second[baseFunct];
}

template <class FunctionSpaceType>
template <class QuadratureType>
const std::vector<FunctionSpaceType::JacobianRange>&
CachingBaseFunctionSet<FunctionSpaceType >::
gradients(int baseFunct, const QuadratureType& quad) const {
  ConstJacobianMapIterator it = grads_.find(quad.getIdentifier());
  
  assert(it != grads_.end());
  assert(baseFunct < numOfBaseFct_);
  
  return it->second[baseFunct];
}


//! if the quadrature is new, then once cache the values 
template <class FunctionSpaceType>
template <class QuadratureType>
void CachingBaseFunctionSet<FunctionSpaceType >::
registerQuadrature( const QuadratureType &quad ) 
{
  // check if quadrature is already registered
  int identifier = quad.getIdentifier();
  if (vals_.find(identifier) == vals_.end()) {

    // Initialise values and gradients
    int nBaseFct = getNumberOfBaseFunctions();
    int nQuadPts = quad.nop();
    Range tmp(0.0);
    
    // Get iterators to the right map entries
    std::pair<RangeMapIterator, bool> rp =
      vals_.insert(std::make_pair(identifier,
                                  std::vector<std::vector<Range> >(nBaseFct,
                                                                   std::vector<Range>(nQuadPts, tmp))));
 
    std::pair<JacobianMapIterator, bool> jp =
      grads_.insert(std::make_pair(identifier,
                                   std::vector<std::vector<JacobianRange> >(nBaseFct,
                                                                            std::vector<JacobianRange>(nQuadPts, JacobianRange()))));
    
    // Iterate over all base functions and initialise values at all quadrature points
    for (int i = 0; i < nBaseFct; ++i) {
      for (int j = 0; j < nQuadPts; ++j) {
        // Range initialisation
        this->eval(i, quad.point(j), rp.first->second[i][j]);
        // Jacobian range initialisation
        this->jacobian(i, quad.point(j), jp.first->second[i][j]);
      }
    }
  } // end if
}

template <class FunctionSpaceType>
const FunctionSpaceType::Range
CachingBaseFunctionSet<FunctionSpaceType>::
extractGradientComp(const JacobianRange& jr, int idx) const {
  Range result;
  for (int i = 0; i < DimRange; ++i) {
     result[i] = jr(idx, i);
  }
  return result;
}

