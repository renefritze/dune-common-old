#ifndef DUNE_AMIRAMESH_WRITER_HH
#define DUNE_AMIRAMESH_WRITER_HH

#include <string>
#include <dune/istl/bvector.hh>

namespace Dune {
   
    /** @ingroup IO 
     * \brief Provides file writing facilities in the AmiraMesh format.
     *
     * Use it by calling the static method write().  Its default implementation
     * only yields a warning message.  Actual functionality is provided by
     * specializations of the methods.  So far, the following ones are
     * available:
     * <ul>
     * <li> SimpleGrid<3,3>, with <code>double</code> data </li>
     * </ul>
     * \todo As DiscFuncType is supposed to know its GridType we can
     * delete the first template parameter.
     */
    template<class GridType>
    class AmiraMeshWriter {

    public:

        /** \brief Writes a grid in AmiraMesh format
         *
         * @param grid The grid objects that is to be written
         * @param filename The filename
         */
        static void writeGrid(const GridType& grid, 
                              const std::string& filename);

        /** \brief Writes a discrete function in AmiraMesh format
         *
         * @param f Function that should be written
         * @param filename The filename
         */
        template <class DiscFuncType>
        static void writeFunction(const DiscFuncType& f,
                                  const std::string& filename);

        static void writeBlockVector(const Dune::BlockVector<Dune::FieldVector<double, 3> >& f,
                                     const std::string& filename);
        AmiraMeshWriter() {}

    };

}

// The default implementation
#include "amiramesh/amirameshwriter.cc"

// the amiramesh writer for SGrid
//#include "amiramesh/amsgridwriter.cc"

#endif
