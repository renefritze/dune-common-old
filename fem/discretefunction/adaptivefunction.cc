
namespace Dune {

  //- AdaptiveDiscreteFunction
  // nothing here, everything in adaptivefunction.hh
  
  //- AdaptiveDiscreteFunction (specialisation)
  
  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  AdaptiveDiscreteFunction<
     CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  ~AdaptiveDiscreteFunction() 
  {
    for (int i = 0; i < subSpaces_.size(); ++i) {
      delete subSpaces_[i];
      subSpaces_[i] = 0;
    }
  }
  
  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  AdaptiveDiscreteFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  SubDiscreteFunctionType
  AdaptiveDiscreteFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  subFunction(int component) 
  {
    SubSpaceType* subSpace = new SubSpaceType(this->space(), component);
    subSpaces_.push_back(subSpace);
    
    return SubDiscreteFunctionType(std::string("Subfunction of ") + name(),
                                   *subSpace,
                                   this->memObject());
  }
  
  
  //- AdaptiveLocalFunction
  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  AdaptiveLocalFunction(const DiscreteFunctionSpaceType& spc,
                        DofStorageType& dofVec) :
    spc_(spc),
    dofVec_(dofVec),
    values_(),
    tmp_(0.0),
    tmpGrad_(0.0)
  {}
  
  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  AdaptiveLocalFunction(const ThisType& other) :
    spc_(other.spc_),
    dofVec_(other.dofVec_),
    values_(),
    tmp_(0.0),
    tmpGrad_(0.0)
  {}

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  ~AdaptiveLocalFunction() {}  

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::DofType&
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  operator[] (int num) 
  {
    return (* (values_[num]));
  }
  
  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  const AdaptiveLocalFunction<
    DiscreteFunctionSpaceImp, DofManagerImp>::DofType&
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  operator[] (int num) const 
  {
    return (* (values_[num]));
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  int AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  numberOfDofs() const
  {
    return values_.size();
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  template <class EntityType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  evaluateLocal(EntityType& en, const DomainType& x, RangeType& ret) const 
  {
    ret *= 0.0;
    const BaseFunctionSetType& bSet = spc_.getBaseFunctionSet(en);

    for (int i = 0; i < bSet.getNumberOfBaseFunctions(); ++i) {
      bSet.eval(i, x, tmp_);
      for (int l = 0; l < dimRange; ++l) {
        ret[l] += (*values_[i]) * tmp_[l];
      }
    }
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  template <class EntityType, class QuadratureType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  evaluate(EntityType& en, 
           QuadratureType& quad, 
           int quadPoint, 
           RangeType& ret) const 
  {
    evaluateLocal(en, quad.point(quadPoint), ret);
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  template <class EntityType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  jacobianLocal(EntityType& en, 
                const DomainType& x, 
                JacobianRangeType& ret) const
  {
    enum { dim = EntityType::dimension };

    ret *= 0.0;
    const BaseFunctionSetType& bSet = spc_.getBaseFunctionSet(en);
    
    for (int i = 0; i < bSet.getNumberOfBaseFunctions(); ++i) {
      tmpGrad_ *= 0.0;
      bSet.jacobian(i, x, tmpGrad_);

      for (int l = 0; l < dimRange; ++l) {
        tmpGrad_[l] *= *values_[i];
        en.geometry().jacobianInverse(x).umtv(tmpGrad_[l], ret[l]);
      }
    }    
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  template <class EntityType, class QuadratureType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  jacobian(EntityType& en, 
           QuadratureType& quad, 
           int quadPoint, 
           JacobianRangeType& ret) const
  {
    jacobianLocal(en, quad.point(quadPoint), ret);
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  template <class EntityType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp, DofManagerImp>::
  init(EntityType& en) 
  {
    int numOfDof = 
      spc_.getBaseFunctionSet(en).getNumberOfBaseFunctions();
    values_.resize(numOfDof);

    for (int i = 0; i < numOfDof; ++i) {
      values_[i] = &(this->dofVec_[spc_.mapToGlobal(en, i)]);
    }
  }

  //- AdaptiveLocalFunction (Specialisation for CombinedSpace)
  
  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  AdaptiveLocalFunction(const DiscreteFunctionSpaceType& spc,
                        DofStorageType& dofVec) :
    spc_(spc),
    dofVec_(dofVec),
    values_(),
    cTmp_(0.0),
    cTmpGrad_(0.0),
    tmp_(0.0),
    tmpGrad_(0.0)
  {}

  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  AdaptiveLocalFunction(const ThisType& other) :
    spc_(other.spc_),
    dofVec_(other.dofVec_),
    values_(),
    cTmp_(0.0),
    cTmpGrad_(0.0),
    tmp_(0.0),
    tmpGrad_(0.0)
  {}

  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  ~AdaptiveLocalFunction() {}

  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::DofType&
  AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  operator[] (int num) 
  {
    return *values_[num/N][num%N];
  }

  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  const AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::DofType&
  AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  operator[] (int num) const 
  {
    return *values_[num/N][num%N];
  }

  template <class ContainedFunctionSpaceImp, int N,
            DofStoragePolicy p, class DofManagerImp>
  int AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  numberOfDofs() const 
  {
    return values_.size()*N;
  }

  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  template <class EntityType>
  void AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  evaluateLocal(EntityType& en, 
                const DomainType& x, 
                RangeType& result) const
  {
    const BaseFunctionSetType& bSet = spc_.getBaseFunctionSet(en);
    result *= 0.0;

    assert(values_.size() == bSet.numContainedFunctions());
    for (int i = 0; i < values_.size(); ++i) {
      // Assumption: scalar contained base functions
      bSet.evaluateContained(i, x, cTmp_);
      for (int j = 0; j < N; ++j) {
        result[j] += cTmp_[0]*(*values_[i][j]);
      }
    }
  }

  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  template <class EntityType, class QuadratureType>
  void AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  evaluate(EntityType& en, 
           QuadratureType& quad, 
           int quadPoint, 
           RangeType& ret) const
  {
    evaluateLocal(en, quad.point(quadPoint), ret);
  }

  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  template <class EntityType>
  void AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  jacobianLocal(EntityType& en, 
                const DomainType& x, 
                JacobianRangeType& result) const
  {
    enum { dim = EntityType::dimension };
    result *= 0.0;

    const BaseFunctionSetType& bSet = spc_.getBaseFunctionSet(en);

    for (int i = 0; i < bSet.numContainedFunctions(); ++i) {
      cTmpGrad_ *= 0.0;
      bSet.jacobianContained(i, x, cTmpGrad_);

      for (int j = 0; j < N; ++j) {
        // Assumption: ContainedDimRange == 1
        tmpGrad_[j] = (*values_[i][j])*cTmpGrad_[0];
      }
      en.geometry().jacobianInverse(x).umtv(tmpGrad_, result);
    }

  }

  template <class ContainedFunctionSpaceImp, int N, 
            DofStoragePolicy p, class DofManagerImp>
  template<class EntityType, class QuadratureType>
  void AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  jacobian(EntityType& en, 
           QuadratureType& quad, 
           int quadPoint, 
           JacobianRangeType& ret) const 
  {
    jacobianLocal(en, quad.point(quadPoint), ret);
  }

  template <class ContainedFunctionSpaceImp, int N,
            DofStoragePolicy p, class DofManagerImp>
  void AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  assign(int dofNum, const DofVectorType& dofs) {
    for (int i = 0; i < N; ++i) {
      // Assumption: the local ordering is point based
      *values_[dofNum][i] = dofs[i];
    }
  }

  template <class ContainedFunctionSpaceImp, int N,
            DofStoragePolicy p, class DofManagerImp>
  int AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  numberOfBaseFunctions() const {
     return values_.size();
  }

  template <class ContainedFunctionSpaceImp, int N,
            DofStoragePolicy p, class DofManagerImp>
  template <class EntityType>
  void AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p>, DofManagerImp>::
  init(EntityType& en) {
    int numOfDof =
      spc_.getBaseFunctionSet(en).numContainedFunctions();
    values_.resize(numOfDof);

    for (int i = 0; i < numOfDof; ++i) {
      for (int j = 0; j < N; ++j) {
        values_[i][j] = &(dofVec_[spc_.mapToGlobal(en, i*N+j)]);
      } // end for j
    } // end for i

  }
 
} // end namespace Dune
