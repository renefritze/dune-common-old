#ifndef __DUNE_AMIRAMESHREADER_HH__
#define __DUNE_AMIRAMESHREADER_HH__

#include <string>
#include "../../common/array.hh"

namespace Dune {
   
    /** @ingroup IO 
     * \brief Provides file reading facilities in the AmiraMesh format.
     *
     */
    template<class GridType>
    class AmiraMeshReader {

    public:

        /** \brief The method that does the reading.
         *
         * @param grid The grid objects that is to be written
         * @param filename The filename
         */
        static void read(GridType& grid, 
                          const std::string& filename);


        AmiraMeshReader() {}

    };

}

// Default implementation
template<class GridType>
void Dune::AmiraMeshReader<GridType>::read(GridType& grid, 
                                           const std::string& filename) 
{
    /** \todo We should somehow output the grid type name here */
    DUNE_THROW(IOError, "No AmiraMesh reading has been implemented for this grid type!");
}


// the amiramesh reader for UGGrid
#ifdef HAVE_UG
#include "amiramesh/amuggridreader.cc"
#endif

#endif
