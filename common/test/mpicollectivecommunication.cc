#include "config.h"

#include<dune/common/mpihelper.hh>

#if HAVE_MPI 
#include<dune/common/mpicollectivecommunication.hh>
#endif

#include<iostream>
int main(int argc, char** argv)
{
  typedef Dune::MPIHelper Helper;
  typedef Helper::MPICommunicator MPIComm;
  
#if HAVE_MPI 
  {
    Helper& mpi = Helper::instance(argc, argv);
    
    
    Dune::CollectiveCommunication<MPIComm> comm(mpi.getCommunicator());

    enum { length = 5 };
    double values[5]; 
    for(int i=0; i<length; ++i) values[i] = 1.0; 
    
    double * commBuff = ((double *) &values[0]);
    // calculate global sum 
    comm.sum( commBuff , length );

    double val[length]; 
    for(int i=0; i<length; ++i) val[i] = 1.0;
    // calculate global sum by calling sum for each component 
    for(int i=0; i<length; ++i) 
    {
      // this method works 
      val[i] = comm.sum( val[i] );
    }

    // result from above should be size of job 
    double sum = mpi.size();
    for(int i=0; i<length; ++i) 
    {
      assert( std::abs( values[i] - sum ) < 1e-8 );
      assert( std::abs( val[i]    - sum ) < 1e-8 );
    }
  }
  
  std::cout << "We are at the end!"<<std::endl;
#else 
  std::cout << "WARNING: test Dune::CollectiveCommunication<MPI_Comm> disabled because MPI not available! " << std::endl;
#endif
  return 0;
}
