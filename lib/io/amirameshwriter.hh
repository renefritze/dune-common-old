#ifndef __AMIRAMESHWRITER_HH__
#define __AMIRAMESHWRITER_HH__

//#include <string>
#include "../../io/gridwriter.hh"

namespace Dune {

    class AmiraMeshWriter : public GridWriter {

    public:

        virtual void write(void* grid,
                           const std::string& filename) const;

        AmiraMeshWriter() {}

    };

}

#endif
