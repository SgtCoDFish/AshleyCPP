set (ASHLEY_TEST_SOURCES 
core/ComponentMapperTests.cpp 
core/ComponentTypeTest.cpp 
core/EngineTests.cpp 
core/EntityTests.cpp 
core/FamilyTests.cpp 
 
signals/SignalTests.cpp 
 
systems/IntervalIteratingSystemTests.cpp 
systems/IntervalSystemTests.cpp 
systems/IteratingSystemTests.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} -pthread ${ASHLEY_DEBUG_FLAGS}")

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(AshleyCPPTest EXCLUDE_FROM_ALL  ${ASHLEY_TEST_SOURCES})
target_link_libraries(AshleyCPPTest AshleyCPP-d ${GTEST_BOTH_LIBRARIES})

