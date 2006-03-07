
namespace Dune {

  //- AdaptiveDiscreteFunction
  // nothing here, everything in adaptivefunction.hh
  
  
  //- AdaptiveLocalFunction
  template <class DiscreteFunctionSpaceImp>
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  AdaptiveLocalFunction(const DiscreteFunctionSpaceType& spc,
                        DofStorageType& dofVec) :
    spc_(spc),
    dofVec_(dofVec),
    values_(),
    tmp_(0.0),
    tmpGrad_(0.0),
    init_(false)
  {}
  
  template <class DiscreteFunctionSpaceImp>
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  AdaptiveLocalFunction(const ThisType& other) :
    spc_(other.spc_),
    dofVec_(other.dofVec_),
    values_(),
    tmp_(0.0),
    tmpGrad_(0.0),
    init_(false)
  {}

  template <class DiscreteFunctionSpaceImp>
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  ~AdaptiveLocalFunction() {}  

  template <class DiscreteFunctionSpaceImp>
  typename AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::DofType&
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  operator[] (int num) 
  {
    assert(init_);
    assert(num >= 0 && num < numDofs());
    // check that storage (dofVec_) and mapper are in sync:
    assert(dofVec_.size() >= spc_.size());
    return (* (values_[num]));
  }
  
  template <class DiscreteFunctionSpaceImp>
  const typename AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::DofType&
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  operator[] (int num) const 
  {
    assert(init_);
    assert(num >= 0 && num < numDofs());
    // check that storage (dofVec_) and mapper are in sync:
    if( dofVec_.size() != spc_.size() ) 
    assert(dofVec_.size() >= spc_.size());
    return (* (values_[num]));
  }

  template <class DiscreteFunctionSpaceImp>
  int AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  numberOfDofs() const
  {
    return values_.size();
  }

  template <class DiscreteFunctionSpaceImp>
  int AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  numDofs() const
  {
    return values_.size();
  }

  template <class DiscreteFunctionSpaceImp>
  template <class EntityType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  evaluate(EntityType& en, const DomainType& x, RangeType& ret) const 
  {
    evaluateLocal( en, en.geometry().local(x), ret);
  }
  
  template <class DiscreteFunctionSpaceImp>
  template <class EntityType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  evaluateLocal(EntityType& en, const DomainType& x, RangeType& ret) const 
  {
    assert(init_);
    assert(en.geometry().checkInside(x));
    ret *= 0.0;
    const BaseFunctionSetType& bSet = this->getBaseFunctionSet();

    for (int i = 0; i < this->numDofs(); ++i) 
    {
      bSet.eval(i, x, tmp_);
      for (int l = 0; l < dimRange; ++l) {
        ret[l] += (*values_[i]) * tmp_[l];
      }
    }
  }

  template <class DiscreteFunctionSpaceImp>
  template <class EntityType, class QuadratureType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  evaluate(EntityType& en, 
           QuadratureType& quad, 
           int quadPoint, 
           RangeType& ret) const 
  {
    assert(init_);
    assert(en.geometry().checkInside(quad.point(quadPoint)));
    ret *= 0.0;
    const BaseFunctionSetType& bSet = this->getBaseFunctionSet();

    for (int i = 0; i < this->numDofs(); ++i) 
    {
      bSet.eval(i, quad,quadPoint, tmp_);
      for (int l = 0; l < dimRange; ++l) {
        ret[l] += (*values_[i]) * tmp_[l];
      }
    }
    // evaluateLocal(en, quad.point(quadPoint), ret);
  }

  template <class DiscreteFunctionSpaceImp>
  template <class EntityType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  jacobianLocal(EntityType& en, 
                const DomainType& x, 
                JacobianRangeType& ret) const
  {
    assert(init_);
    enum { dim = EntityType::dimension };

    ret *= 0.0;
    const BaseFunctionSetType& bSet = this->getBaseFunctionSet();
    
    for (int i = 0; i < this->numDofs(); ++i) {
      tmpGrad_ *= 0.0;
      bSet.jacobian(i, x, tmpGrad_);

      for (int l = 0; l < dimRange; ++l) {
        tmpGrad_[l] *= *values_[i];
        // * umtv or umv?
        en.geometry().jacobianInverseTransposed(x).umv(tmpGrad_[l], ret[l]);
      }
    }    
  }

  template <class DiscreteFunctionSpaceImp>
  template <class EntityType, class QuadratureType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::
  jacobian(EntityType& en, 
           QuadratureType& quad, 
           int quadPoint, 
           JacobianRangeType& ret) const
  {
    assert(init_);
    enum { dim = EntityType::dimension };

    ret *= 0.0;
    const BaseFunctionSetType& bSet = this->getBaseFunctionSet();
    
    typedef FieldMatrix<DofType, dim, dim> JacobianInverseType;
    const JacobianInverseType& jti = 
      en.geometry().jacobianInverseTransposed(quad.point(quadPoint));

    JacobianRangeType tmp(0.0);

    for (int i = 0; i < this->numDofs(); ++i) 
    {
      bSet.jacobian(i, quad,quadPoint, tmpGrad_);
      tmpGrad_ *= *values_[i];
      tmp += tmpGrad_;
    }    
    for (int l = 0; l < dimRange; ++l) 
      jti.umv(tmp[l],ret[l]);
  }

  template <class DiscreteFunctionSpaceImp>
  const typename
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::BaseFunctionSetType& 
  AdaptiveLocalFunction<DiscreteFunctionSpaceImp >::getBaseFunctionSet() const {
    assert(baseSet_);
    return *baseSet_;
  }
  

  template <class DiscreteFunctionSpaceImp>
  template <class EntityType>
  void AdaptiveLocalFunction<DiscreteFunctionSpaceImp>::
  init(const EntityType& en) 
  {
    baseSet_ = &spc_.getBaseFunctionSet(en);

    int numOfDof = baseSet_->numBaseFunctions();
    values_.resize(numOfDof);

    for (int i = 0; i < numOfDof; ++i) {
      values_[i] = &(this->dofVec_[spc_.mapToGlobal(en, i)]);
    }

    init_ = true;
  }
  //- AdaptiveDiscreteFunction (specialisation)
  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  AdaptiveDiscreteFunction<
     CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  ~AdaptiveDiscreteFunction() 
  {
    for (unsigned int i = 0; i < subSpaces_.size(); ++i) {
      delete subSpaces_[i];
      subSpaces_[i] = 0;
    }
  }
  
  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  typename AdaptiveDiscreteFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p> >::SubDiscreteFunctionType
  AdaptiveDiscreteFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  subFunction(int component) 
  {
    SubSpaceType* subSpace = new SubSpaceType(this->space(), component);
    subSpaces_.push_back(subSpace);
    

    return SubDiscreteFunctionType(std::string("Subfunction of ")+this->name(),
                                   *subSpace,
                                   this->dofStorage());
  }

  //- AdaptiveLocalFunction (Specialisation for CombinedSpace)
  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  AdaptiveLocalFunction(const DiscreteFunctionSpaceType& spc,
                        DofStorageType& dofVec) :
    spc_(spc),
    dofVec_(dofVec),
    values_(),
    cTmp_(0.0),
    cTmpGradRef_(0.0),
    cTmpGradReal_(0.0),
    tmp_(0.0)
  {}

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  AdaptiveLocalFunction(const ThisType& other) :
    spc_(other.spc_),
    dofVec_(other.dofVec_),
    values_(),
    cTmp_(0.0),
    cTmpGradRef_(0.0),
    cTmpGradReal_(0.0),
    tmp_(0.0)
  {}

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  ~AdaptiveLocalFunction() {}

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  typename AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p> >::DofType&
  AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  operator[] (int num) 
  {
    assert(num >= 0 && num < numDofs());
    return *values_[num/N][static_cast<SizeType>(num%N)];
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  const typename AdaptiveLocalFunction<
    CombinedSpace<ContainedFunctionSpaceImp, N, p> >::DofType&
  AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  operator[] (int num) const 
  {
    assert(num >= 0 && num < numDofs());
    return *values_[num/N][static_cast<SizeType>(num%N)];
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  int AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  numberOfDofs() const 
  {
    return values_.size()*N;
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  int AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  numDofs() const 
  {
    return values_.size()*N;
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  template <class EntityType>
  void AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  evaluate(EntityType& en, const DomainType& x, RangeType& ret) const 
  {
    evaluateLocal( en, en.geometry().local(x), ret);
  }
  
  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  template <class EntityType>
  void AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  evaluateLocal(EntityType& en, 
                const DomainType& x, 
                RangeType& result) const
  {
    assert(en.geometry().checkInside(x));

    const BaseFunctionSetType& bSet = this->getBaseFunctionSet();
    result *= 0.0;

    assert(static_cast<int>(values_.size()) == bSet.numDifferentBaseFunctions());
    for (unsigned int i = 0; i < values_.size(); ++i) {
      // Assumption: scalar contained base functions
      bSet.evaluateScalar(i, x, cTmp_);
      for (SizeType j = 0; j < N; ++j) {
        result[j] += cTmp_[0]*(*values_[i][j]);
      }
    }
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  template <class EntityType, class QuadratureType>
  void AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  evaluate(EntityType& en, 
           QuadratureType& quad, 
           int quadPoint, 
           RangeType& ret) const
  {
    evaluateLocal(en, quad.point(quadPoint), ret);
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  template <class EntityType>
  void AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  jacobianLocal(EntityType& en, 
                const DomainType& x, 
                JacobianRangeType& result) const
  {
    enum { dim = EntityType::dimension };
    //typedef FieldMatrix<DofType, RangeType::size, RangeType::size> JacobianInverseType;
    typedef FieldMatrix<DofType, dim, dim> JacobianInverseType;
    result *= 0.0;

    const BaseFunctionSetType& bSet = this->getBaseFunctionSet();
    const JacobianInverseType& jInv = 
      en.geometry().jacobianInverseTransposed(x);

    for (int i = 0; i < bSet.numDifferentBaseFunctions(); ++i) {
      //cTmpGradRef_ *= 0.0;
      cTmpGradReal_ *= 0.0;
      bSet.jacobianScalar(i, x, cTmpGradRef_);
      jInv.umv(cTmpGradRef_[0], cTmpGradReal_[0]);

      for (SizeType j = 0; j < N; ++j) {
        // Assumption: ContainedDimRange == 1
        //cTmpGrad_[0] *= *values_[i][j];
        result[j].axpy(*values_[i][j], cTmpGradReal_[0]);
      }
    }

  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  template<class EntityType, class QuadratureType>
  void AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  jacobian(EntityType& en, 
           QuadratureType& quad, 
           int quadPoint, 
           JacobianRangeType& ret) const 
  {
    jacobianLocal(en, quad.point(quadPoint), ret);
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  void AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  assign(int dofNum, const RangeType& dofs) {
    for (SizeType i = 0; i < N; ++i) {
      // Assumption: the local ordering is point based
      *values_[dofNum][i] = dofs[i];
    }
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  int AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  numDifferentBaseFunctions() const {
     return values_.size();
  }

  template <class ContainedFunctionSpaceImp, int N, DofStoragePolicy p>
  template <class EntityType>
  void AdaptiveLocalFunction<CombinedSpace<ContainedFunctionSpaceImp, N, p> >::
  init(const EntityType& en) {
    baseSet_ = &spc_.getBaseFunctionSet(en);
    int numOfDof =
      baseSet_.numDifferentBaseFunctions();
    values_.resize(numOfDof);

    for (int i = 0; i < numOfDof; ++i) {
      for (SizeType j = 0; j < N; ++j) {
        values_[i][j] = &(dofVec_[spc_.mapToGlobal(en, i*N+j)]);
      } // end for j
    } // end for i

  }
  
} // end namespace Dune
