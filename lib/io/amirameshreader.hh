#ifndef __DUNE_AMIRAMESHREADER_HH__
#define __DUNE_AMIRAMESHREADER_HH__

#include <string>
#include "../../common/array.hh"

namespace Dune {
   
    /** @ingroup IO 
     * \brief Provides file reading facilities in the AmiraMesh format.
     *
     */
    template<class GRID>
    class AmiraMeshReader {

    public:

        /** \brief The method that does the reading.
         *
         * @param grid The grid objects that is to be written
         * @param filename The filename
         */
        static void read(GRID& grid, 
                          const std::string& filename);


        AmiraMeshReader() {}

    };

}

// Default implementation
template<class GRID>
void Dune::AmiraMeshReader<GRID>::read(GRID& grid, 
                                  const std::string& filename) 
{
    printf("No AmiraMesh reading has been implemented for this grid type!\n");
}


// the amiramesh reader for UGGrid
#ifdef HAVE_UG
#include "amuggridreader.cc"
#endif

#endif
