#include <config.h>

#include <dune/common/mpihelper.hh>
#include <dune/common/mpiguard.hh>

int main(int argc, char** argv)
{
    Dune::MPIHelper & mpihelper = Dune::MPIHelper::instance(argc, argv);

    if (mpihelper.rank() == 0)
        std::cout << "---- default constructor" << std::endl;
    try
    {
        // at the end of this block the guard is destroyed and possible exceptions are communicated
        {
            Dune::MPIGuard guard;
            if (mpihelper.rank() > 0)
                DUNE_THROW(Dune::Exception, "Fakeproblem on process " << mpihelper.rank());
            guard.finalize();
        }
    }
    catch (Dune::Exception & e)
    {
        std::cout << "Error (rank " << mpihelper.rank() << "): "
                  << e.what() << std::endl;
    }

    mpihelper.getCollectiveCommunication().barrier();
    if (mpihelper.rank() == 0)
        std::cout << "---- guard(MPI_COMM_WORLD)" << std::endl;
    try
    {
        // at the end of this block the guard is destroyed and possible exceptions are communicated
        {
            Dune::MPIGuard guard(MPI_COMM_WORLD);
            if (mpihelper.rank() > 0)
                DUNE_THROW(Dune::Exception, "Fakeproblem on process " << mpihelper.rank());
            guard.finalize();
        }
    }
    catch (Dune::Exception & e)
    {
        std::cout << "Error (rank " << mpihelper.rank() << "): "
                  << e.what() << std::endl;
    }

    mpihelper.getCollectiveCommunication().barrier();
    if (mpihelper.rank() == 0)
        std::cout << "---- guard(MPIHelper)" << std::endl;
    try
    {
        // at the end of this block the guard is destroyed and possible exceptions are communicated
        {
            Dune::MPIGuard guard(mpihelper);
            if (mpihelper.rank() > 0)
                DUNE_THROW(Dune::Exception, "Fakeproblem on process " << mpihelper.rank());
            guard.finalize();
        }
    }
    catch (Dune::Exception & e)
    {
        std::cout << "Error (rank " << mpihelper.rank() << "): "
                  << e.what() << std::endl;
    }


    mpihelper.getCollectiveCommunication().barrier();
    if (mpihelper.rank() == 0)
        std::cout << "---- manual error" << std::endl;
    try
    {
        // at the end of this block the guard is destroyed and possible exceptions are communicated
        {
            Dune::MPIGuard guard;
            guard.finalize(mpihelper.rank() > 0);
        }
    }
    catch (Dune::Exception & e)
    {
        std::cout << "Error (rank " << mpihelper.rank() << "): "
                  << e.what() << std::endl;
    }

    mpihelper.getCollectiveCommunication().barrier();
    if (mpihelper.rank() == 0)
        std::cout << "---- done" << std::endl;
}
