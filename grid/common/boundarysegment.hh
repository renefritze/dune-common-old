#ifndef DUNE_BOUNDARY_SEGMENT_HH
#define DUNE_BOUNDARY_SEGMENT_HH

/** \file
    \brief Base class for grid boundary segments of arbitrary geometry
 */

namespace Dune {

    template <int dimworld>
    class BoundarySegment {
    public:

        virtual ~BoundarySegment() {}

        virtual FieldVector<double,dimworld> operator()(const FieldVector<double, dimworld-1>& local) const = 0;

    };

}  // end namespace Dune

#endif
