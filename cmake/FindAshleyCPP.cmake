# AshleyCPP_INCLUDE_DIR
# AshleyCPP_DEBUG_LIBRARY
# AshleyCPP_LIBRARY
# AshleyCPP_FOUND

include(FindPackageHandleStandardArgs)

find_path(AshleyCPP_INCLUDE_DIR NAMES Ashley/Ashley.hpp
          DOC "The AshleyCPP include directory"
)

find_library(AshleyCPP_DEBUG_LIBRARY NAMES AshleyCPP-d
          DOC "The AshleyCPP debug library"
)

find_library(AshleyCPP_LIBRARY NAMES AshleyCPP
          DOC "The AshleyCPP release library"
)

if(AshleyCPP_DEBUG_LIBRARY OR AshleyCPP_LIBRARY)
  set(AshleyCPP_FOUND TRUE)
else()
  set(AshleyCPP_FOUND FALSE)
endif()

if (AshleyCPP_DEBUG_LIBRARY AND AshleyCPP_LIBRARY)
    message("Found AshleyCPP:\n\tDebug   (${AshleyCPP_DEBUG_LIBRARY})\n\tRelease (${AshleyCPP_LIBRARY})")
    find_package_handle_standard_args(AshleyCPP DEFAULT_MSG AshleyCPP_DEBUG_LIBRARY AshleyCPP_LIBRARY AshleyCPP_INCLUDE_DIR)
    mark_as_advanced(AshleyCPP_INCLUDE_DIR AshleyCPP_DEBUG_LIBRARY AshleyCPP_LIBRARY)
elseif (AshleyCPP_DEBUG_LIBRARY)
    message("Found AshleyCPP: Debug only (${AshleyCPP_DEBUG_LIBRARY})")
    find_package_handle_standard_args(AshleyCPP DEFAULT_MSG AshleyCPP_DEBUG_LIBRARY AshleyCPP_INCLUDE_DIR)
    mark_as_advanced(AshleyCPP_INCLUDE_DIR AshleyCPP_DEBUG_LIBRARY)
elseif (AshleyCPP_LIBRARY)
    message("Found AshleyCPP: Release only (${AshleyCPP_LIBRARY})")
    find_package_handle_standard_args(AshleyCPP DEFAULT_MSG AshleyCPP_LIBRARY AshleyCPP_INCLUDE_DIR)
    mark_as_advanced(AshleyCPP_INCLUDE_DIR AshleyCPP_LIBRARY)
endif ()
