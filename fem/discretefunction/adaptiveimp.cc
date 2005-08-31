

namespace Dune {
  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  AdaptiveFunctionImplementation(std::string name,
                                 const DiscreteFunctionSpaceType& spc) :
    spc_(spc),
    name_(name),
    dm_(DofManagerFactory<DofManagerType>::getDofManager(spc.grid())),
    memObj_(dm_.addDofSet(&dofVec_, spc.mapper(), name_)),
    dofVec_(memObj_.getArray())
  {}

  
  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  std::string
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  name() const {
    return name_;
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  ~AdaptiveFunctionImplementation() {
    bool removed = dm_.removeDofSet(memObj_);

    assert(removed);
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::DofIteratorType
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::dbegin() {
    return dofVec_.begin();
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::DofIteratorType
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::dend() {
    return dofVec_.end();
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::ConstDofIteratorType
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::dbegin() const {
    return dofVec_.begin();
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::ConstDofIteratorType
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::dend() const{
    return dofVec_.end();
  }
  
  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::LocalFunctionType
  AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  newLocalFunction() {
    return LocalFunctionType(spc_, dofVec_);
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  template <class EntityType>
  void AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  localFunction(const EntityType& en, LocalFunctionType& lf) {
    lf.init(en);
  }

  //- Read/write methods
  template<class DiscreteFunctionSpaceImp, class DofManagerImp>
  bool AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  write_xdr(std::string fn)
  {
    FILE  *file;
    XDR   xdrs;
    file = fopen(fn.c_str(), "wb");
    if (!file)
      { 
        printf( "\aERROR in AdaptiveDiscreteFunction::write_xdr(..): could not open <%s>!\n", fn.c_str());
        fflush(stderr);
        return false;
      }

    xdrstdio_create(&xdrs, file, XDR_ENCODE);   
    dofVec_.processXdr(&xdrs);

    xdr_destroy(&xdrs);
    fclose(file);
  
    return true;
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  inline bool AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  read_xdr(std::string fn)
  {
    FILE   *file;
    XDR     xdrs;
    std::cout << "Reading <" << fn << "> \n";
    file = fopen(fn.c_str() , "rb");
    if(!file)
      { 
        printf( "\aERROR in AdaptiveDiscreteFunction::read_xdr(..): could not open <%s>!\n", fn.c_str());
        fflush(stderr);
        return(false);
      }

    // read xdr 
    xdrstdio_create(&xdrs, file, XDR_DECODE);     
    dofVec_.processXdr(&xdrs);
  
    xdr_destroy(&xdrs);
    fclose(file);
    return true;
  }

  template <class DiscreteFunctionSpaceImp, class DofManagerImp>
  inline bool AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  write_ascii(std::string fn)
  {
    std::fstream outfile( fn.c_str() , std::ios::out );
    if (!outfile)
      { 
        printf( "\aERROR in AdaptiveDiscreteFunction::write_ascii(..): could not open <%s>!\n", fn.c_str());
        fflush(stderr);
        return false;
      }

    {
      int length = spc_.size();
      outfile << length << "\n";
      DofIteratorType enddof = dend ( );
      for(DofIteratorType itdof = dbegin ( );itdof != enddof; ++itdof) 
        {
          outfile << (*itdof) << " ";
        }
      outfile << "\n";
    }

    outfile.close();
    return true;
  }


  template<class DiscreteFunctionSpaceImp, class DofManagerImp>
  inline bool AdaptiveFunctionImplementation<DiscreteFunctionSpaceImp, DofManagerImp>::
  read_ascii(std::string fn)
  {
    FILE *infile=0;
    infile = fopen( fn.c_str(), "r" );
    assert(infile != 0); 
    {
      int length;
      fscanf(infile,"%d \n",&length); 
      assert(length == spc_.size( )); 

      DofIteratorType enddof = dend ( );
      for(DofIteratorType itdof = dbegin ( );itdof != enddof; ++itdof) 
        {
          fscanf(infile,"%le \n",& (*itdof)); 
        }
    }
    fclose(infile);
    return true;
  }

} // end namespace Dune
