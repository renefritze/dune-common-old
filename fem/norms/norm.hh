#ifndef DUNE_NORM_HH
#define DUNE_NORM_HH

namespace Dune {

    //! Abstract base for classes computing norms of discrete functions
    template <class DiscFuncType>
    class Norm {

    public:

        //! Compute the norm
        virtual double compute(const DiscFuncType& f) = 0;

    };

}

#endif
