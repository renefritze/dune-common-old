#ifndef DUNE_SOLVER_HH
#define DUNE_SOLVER_HH

#include <dune/solver/common/numproc.hh>

namespace Dune {

    /** \brief The base class for all sorts of solver algorithms */
    class Solver : public NumProc
    {
    public:

        /** \brief Virtual destructor */
        virtual ~Solver() {}

        /** \brief Do the necessary preprocessing */
        virtual void preprocess();

        /** \brief Derived classes overload this with the actual
         * solution algorithm */
        virtual void solve() = 0;

        /** \brief The requested tolerance of the solver */
        double tolerance_;

    };

}

void Dune::Solver::preprocess()
{
    // Default: Do nothing
}

#endif
