#include <dune/common/exceptions.hh>

namespace Dune {
    // static member of Dune::Exception
    ExceptionHook * Exception::_hook = 0;
}
