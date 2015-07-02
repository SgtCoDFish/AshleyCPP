cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project("AshleyCPP")

set(dialect "-std=c++0x")
set(warnings "-Wall")
set(flags "-fmessage-length=0")
set(ASHLEY_DEBUG_FLAGS "-O0 -g3")
set(ASHLEY_RELEASE_FLAGS "-O3 -g0")

set(CMAKE_CXX_COMPILER_ARG1 "-std=c++0x")
set(CMAKE_CXX_FLAGS_BASE "${warnings} ${flags} ${debug_flags}")

include_directories("include" "test")

set(ASHLEY_CPP_SOURCES 
src/ComponentType.cpp
src/Engine.cpp
src/Entity.cpp
src/Family.cpp
src/IntervalIteratingSystem.cpp
src/IntervalSystem.cpp
src/IteratingSystem.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${ASHLEY_DEBUG_FLAGS}")
add_library(AshleyCPP-d ${ASHLEY_CPP_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${ASHLEY_RELEASE_FLAGS}")
add_library(AshleyCPP ${ASHLEY_CPP_SOURCES})

if (NOT EXCLUDE_TESTS)
find_package(GTest REQUIRED)

set (ASHLEY_TEST_SOURCES 
test/core/ComponentMapperTests.cpp 
test/core/ComponentTypeTest.cpp 
test/core/EngineTests.cpp 
test/core/EntityTests.cpp 
test/core/FamilyTests.cpp 
 
test/signals/SignalTests.cpp 
 
test/systems/IntervalIteratingSystemTests.cpp 
test/systems/IntervalSystemTests.cpp 
test/systems/IteratingSystemTests.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} -pthread ${ASHLEY_DEBUG_FLAGS}")

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(AshleyCPPTest ${ASHLEY_TEST_SOURCES})
target_link_libraries(AshleyCPPTest AshleyCPP-d ${GTEST_BOTH_LIBRARIES})
endif (NOT EXCLUDE_TESTS)

