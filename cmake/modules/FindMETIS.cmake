# Accepts the following variables:
#
# METIS_DIR: Prefix where ParMETIS is installed.
# METIS_LIB_NAME: Name of the METIS library (default: metis).
# METIS_LIBRARY: Full path of the METIS library.

# Sets the following variables:
#
# METIS_LIBRARY: Full path of the METIS library.
# METIS_FOUND: True if ParMETIS was found.
# METIS_LIBRARIES: List of all libraries needed for linking with METIS,
# 
# Provides the following macros:
#
# find_package(METIS)
#
# Searches for METIS (See above)
#
#
# add_dune_metis_flags(TARGETS)
#
# Adds the necessary flags to comile and link TARGETS with ParMETIS support.
#

# function to add the metis flags to a target
function(add_dune_metis_flags _targets)
  if(METIS_FOUND)
    foreach(_target ${_targets})
      target_link_libraries(${_target} ${METIS_LIBRARY})
    endforeach(_target ${_targets})
    set_property(TARGET ${_targets} APPEND PROPERTIES 
      COMPILE_DIRECTORIES ${METIS_INCLUDE_DIR})
  endif(METIS_FOUND)
endfunction(add_dune_metis_flags _targets)

# search metis header
find_path(METIS_INCLUDE_DIR metis.h
  PATHS ${METIS_DIR} 
  PATH_SUFFIXES metis include include/metis Lib
  NO_DEFAULT_PATH
  DOC "Include directory of metis")
find_path(METIS_INCLUDE_DIR metis.h
  PATH_SUFFIXES metis include include/metis Lib)

set(METIS_LIBRARY METIS_LIBRARY-NOTFOUND CACHE FILEPATH "Full path of the METIS library")

# check metis header
include(CMakePushCheckState)
cmake_push_check_state() # Save variables
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${METIS_INCLUDE_DIR})
check_include_file(metis.h METIS_FOUND)

# search metis library
if(NOT METIS_LIB_NAME)
  set(METIS_LIB_NAME metis)
endif(NOT METIS_LIB_NAME)

find_library(METIS_LIBRARY ${METIS_LIB_NAME} 
  PATHS ${METIS_DIR} 
  PATH_SUFFIXES lib
  NO_DEFAULT_PATH)
find_library(METIS_LIBRARY ${METIS_LIB_NAME}
  PATH_SUFFIXES lib
)

# behave like a CMake module is supposed to behave
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  "METIS"
  DEFAULT_MSG
  METIS_INCLUDE_DIR
  METIS_LIBRARY
)

# check metis library
if(METIS_LIBRARY)
  list(APPEND CMAKE_REQUIRED_LIBRARIES ${METIS_LIBRARIY})
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${METIS_LIBRARY})
  include(CheckSymbolExists)
  check_symbol_exists(METIS_PartMeshDual metis.h METIS_FOUND)
endif(METIS_LIBRARY)
cmake_pop_check_state()

mark_as_advanced(METIS_INCLUDE_DIR METIS_LIBRARIES METIS_LIB_NAME)

# if both headers and library are found, store results
if(METIS_FOUND)
  set(METIS_INCLUDE_DIRS ${METIS_INCLUDE_DIR})
  set(METIS_LIBRARIES ${METIS_LIBRARY})
  # log result
  file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
    "Determing location of METIS succeded:\n"
    "Include directory: ${METIS_INCLUDE_DIRS}\n"
    "Library directory: ${METIS_LIBRARIES}\n\n")
else(METIS_FOUND)
  # log errornous result
  file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
    "Determing location of METIS failed:\n"
    "Include directory: ${METIS_INCLUDE_DIRS}\n"
    "Library directory: ${METIS_LIBRARIES}\n\n")
endif(METIS_FOUND)
