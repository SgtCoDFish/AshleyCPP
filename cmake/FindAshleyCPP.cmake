# AshleyCPP_INCLUDE_DIR
# AshleyCPP_DEBUG_LIBRARY
# AshleyCPP_LIBRARY
# AshleyCPP_FOUND

find_path(AshleyCPP_INCLUDE_DIR NAMES AshleyCPP/AshleyCPPCommon.hpp
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
endif(AshleyCPP_DEBUG_LIBRARY OR AshleyCPP_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AshleyCPP DEFAULT_MSG AshleyCPP_DEBUG_LIBRARY AshleyCPP_LIBRARY AshleyCPP_INCLUDE_DIR)

mark_as_advanced(AshleyCPP_INCLUDE_DIR AshleyCPP_DEBUG_LIBRARY AshleyCPP_LIBRARY)
