#ifndef __DUNE_NORM_HH__
#define __DUNE_NORM_HH__

namespace Dune {

    //! Abstract base for classes computing norms of discrete functions
    template <class DiscFuncType>
    class Norm {

    public:

        //! Compute the norm
        virtual double compute(const DiscFuncType& f, int level) = 0;

    };

}

#endif
