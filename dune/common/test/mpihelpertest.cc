#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include<dune/common/mpihelper.hh>
#include<iostream>
int main(int argc, char** argv)
{

#ifdef MPIHELPER_PREINITIALZE
  MPI_Init(&argc, &argv);
#endif

  typedef Dune::MPIHelper Helper;
  
  {
    Helper& mpi = Helper::instance(argc, argv);
    
    Helper::MPICommunicator comm = mpi.getCommunicator();
    comm= mpi.getCommunicator();
  }
  
  {
    Helper& mpi = Helper::instance(argc, argv);

    Helper::MPICommunicator comm= mpi.getCommunicator();
    comm= mpi.getCommunicator();

#ifdef MPIHELPER_PREINITIALZE
    MPI_Finalize();
#endif
  }
  std::cout << "We are at the end!"<<std::endl;
  
}
