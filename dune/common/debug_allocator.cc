#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "debug_allocator.hh"

#include <iostream>
#include <unistd.h>
#include <cstdlib>

namespace Dune
{
    namespace DebugMemory
    {
        // system constant for page size
        const AllocationManager::difference_type AllocationManager::page_size = getpagesize();

        // implement member functions
        void AllocationManager::allocation_error(const char* msg)
        {
            std::cerr << "Abort - Memory Corruption: " << msg << std::endl;
            std::abort();
        }

        // global instance of AllocationManager
        AllocationManager alloc_man;
        
    } // end namespace DebugMemory
} // end namespace Dune

