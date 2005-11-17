

namespace Dune {

  //- class StandardBaseFunctionSet
  template <class FunctionSpaceImp>
  int StandardBaseFunctionSet<FunctionSpaceImp>::numBaseFunctions() const 
  {
    return baseFunctions_.size();
  }

  template <class FunctionSpaceImp>
  template <int diffOrd>
  void StandardBaseFunctionSet<FunctionSpaceImp>::
  evaluate(int baseFunct,
           const FieldVector<deriType, diffOrd>& diffVar,
           const DomainType& xLocal,
           RangeType& phi) const {
    baseFunctions_[baseFunct]->evaluate(diffVar, xLocal, phi);
  }

  //- class VectorialBaseFunctionSet
  template <class FunctionSpaceImp>
  int VectorialBaseFunctionSet<FunctionSpaceImp>::numBaseFunctions() const 
  {
    return baseFunctions_.size()*FunctionSpaceImp::DimRange;
  }


  template <class FunctionSpaceImp>
  template <int diffOrd>
  void VectorialBaseFunctionSet<FunctionSpaceImp>::
  evaluate(int baseFunct,
           const FieldVector<int, diffOrd>& diffVar,
           const DomainType& xLocal,
           RangeType& phi) const 
  {
    ScalarRangeType tmp;
    baseFunctions_[util_.containedDof(baseFunct)]->evaluate(diffVar, xLocal, tmp);
    
    phi *= 0.0;
    phi[util_.component(baseFunct)] = tmp[0];
  }

  template <class FunctionSpaceImp>
  typename VectorialBaseFunctionSet<FunctionSpaceImp>::DofType
  VectorialBaseFunctionSet<FunctionSpaceImp>::
  evaluateSingle(int baseFunct, 
                 const DomainType& xLocal,
                 const RangeType& factor) const 
  {
    this->eval(util_.containedDof(baseFunct), xLocal, tmp_);
    return factor[util_.component(baseFunct)]*tmp_[0];
  }
  
  template <class FunctionSpaceImp>
  typename VectorialBaseFunctionSet<FunctionSpaceImp>::DofType
  VectorialBaseFunctionSet<FunctionSpaceImp>::
  evaluateGradientSingle(int baseFunct,
                         const DomainType& xLocal,
                         const JacobianRangeType& factor) const 
  {
    this->jacobian(util_.containedDof(baseFunct), xLocal, jTmp_);
    return jTmp_[util_.component(baseFunct)]*factor[0];
  }
  
} // end namespace Dune
