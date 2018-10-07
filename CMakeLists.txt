cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project(AshleyCPP CXX)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)
set(VERSION_PATCH 0)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message("Conifguring ${CMAKE_PROJECT_NAME} version ${VERSION}.")

if( DEFINED WIN32 )
	if( MSVC )
		set(dialect "")
		set(warnings "")
		set(flags "/GR /EHsc")

		#/Od: Disable optimisation
		#/Ox: Enable all optimisations
		#/Gx: Check buffer security
		#/EHsc: Enables c++ exceptions
		#/GR: Enables RTTI
	else () 
		# MinGW
		set(dialect "-std=c++0x")
		set(warnings "-Wall -Wextra -Wno-unused-parameter")
		set(flags "-mwindows")
	endif()
else()
	set(dialect "-std=c++0x")
	set(warnings "-Wall -Wextra -Wno-unused-parameter")
	set(flags "-fPIC")
endif()

set(CMAKE_CXX_FLAGS_BASE "${CMAKE_CXX_FLAGS_BASE} ${dialect} ${warnings} ${flags}")

include_directories("include")

file(GLOB_RECURSE ASHLEY_CPP_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB ASHLEY_CPP_HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp)

set(ASHLEY_LIB_NAME ${CMAKE_PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE}")
add_library(${ASHLEY_LIB_NAME} ${ASHLEY_CPP_SOURCES})

if (NOT EXCLUDE_TESTS)
	if ( NOT MSVC )
		set(ASHLEY_TEST_FLAGS "-pthread")
	else ()
		set(ASHLEY_TEST_FLAGS "")
	endif ()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASHLEY_TEST_FLAGS}")

	add_subdirectory("test")

	include_directories("test" "test/googletest/include")
	file (GLOB_RECURSE ASHLEY_TEST_SOURCES
		${PROJECT_SOURCE_DIR}/test/core/*.cpp
		${PROJECT_SOURCE_DIR}/test/signals/*.cpp
		${PROJECT_SOURCE_DIR}/test/systems/*.cpp)

	add_executable(AshleyCPPTest ${ASHLEY_TEST_SOURCES})
	target_link_libraries(AshleyCPPTest ${ASHLEY_LIB_NAME} gtest_main gtest)
endif (NOT EXCLUDE_TESTS)

install (TARGETS ${ASHLEY_LIB_NAME} DESTINATION lib)
install (DIRECTORY ${PROJECT_SOURCE_DIR}/include/Ashley DESTINATION include)
