#ifndef __DUNE_ITERATIONSTEP_HH__
#define __DUNE_ITERATIONSTEP_HH__

#include <dune/solver/common/numproc.hh>

namespace Dune {

    template<class OP_TYPE, class VECTOR_TYPE>
    class IterationStep : public NumProc
    {
    public:

        IterationStep() {}

        IterationStep(OP_TYPE& mat, VECTOR_TYPE& x, VECTOR_TYPE& rhs) {
            mat_ = &mat;
            x_   = &x;
            rhs_ = &rhs;
        }

        void setProblem(OP_TYPE& mat, VECTOR_TYPE& x, VECTOR_TYPE& rhs) {
            mat_ = &mat;
            x_   = &x;
            rhs_ = &rhs;
        }

        virtual void iterate() = 0;

        virtual VECTOR_TYPE getSol() = 0;

        virtual OP_TYPE* getMatrix() {return mat_;}

        VECTOR_TYPE* x_;

        VECTOR_TYPE* rhs_;

        OP_TYPE* mat_;

    };

}

#endif
