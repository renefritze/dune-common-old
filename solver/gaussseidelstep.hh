#ifndef __DUNE_GAUSS_SEIDEL_STEP_HH__
#define __DUNE_GAUSS_SEIDEL_STEP_HH__

#include <dune/solver/common/iterationstep.hh>

namespace Dune {

    template<class OperatorType, class DiscFuncType>
    class GaussSeidelStep : public IterationStep<OperatorType, DiscFuncType>
    {
    public:
        
        //! Default constructor.  Doesn't init anything
        GaussSeidelStep() {}

        //! Constructor with a linear problem
        GaussSeidelStep(OperatorType& mat, DiscFuncType& x, DiscFuncType& rhs)
            : IterationStep<OperatorType,DiscFuncType>(mat, x, rhs)
        {}

        //! Perform one iteration
        virtual void iterate();

        virtual DiscFuncType getSol();

        double residual(int index) const;

    };

}  // namespace Dune

#include "common/gaussseidelstep.cc"

#endif
