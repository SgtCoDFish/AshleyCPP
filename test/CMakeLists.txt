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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${ASHLEY_DEBUG_FLAGS}")

add_executable(AshleyCPPTest EXCLUDE_FROM_ALL  ${ASHLEY_TEST_SOURCES})
target_link_libraries(AshleyCPPTest AshleyCPP-d gtest_main gtest)