#ifndef __GRIDWRITER_HH__
#define __GRIDWRITER_HH__

#include <string>

namespace Dune {

    class GridWriter {

    public:
        virtual void write(void* grid, 
                           const std::string& filename) const = 0;

        static GridWriter* getWriter(const std::string& format);

    protected:

        GridWriter() {}

        static GridWriter* writer;

    };

}

#endif
