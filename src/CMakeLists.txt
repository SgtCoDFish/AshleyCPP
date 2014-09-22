set(ASHLEY_CPP_SOURCES 
ComponentType.cpp
Engine.cpp
Entity.cpp
Family.cpp
IntervalIteratingSystem.cpp
IntervalSystem.cpp
IteratingSystem.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${ASHLEY_DEBUG_FLAGS}")
add_library(AshleyCPP-d ${ASHLEY_CPP_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${ASHLEY_RELEASE_FLAGS}")
add_library(AshleyCPP ${ASHLEY_CPP_SOURCES})