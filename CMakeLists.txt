cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project(AshleyCPP)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message("Conifguring AshleyCPP version ${VERSION}.")

if( DEFINED WIN32 )
	if( MSVC )
		set(dialect "")
		set(warnings "")
		set(flags "/GR /EHsc")
		set(ASHLEY_DEBUG_FLAGS "/Od /Gs")
		set(ASHLEY_RELEASE_FLAGS "/Ox ")

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
		set(ASHLEY_DEBUG_FLAGS "-O0 -g3")
		set(ASHLEY_RELEASE_FLAGS "-O3 -g0")
	endif() 
else()
	set(dialect "-std=c++0x")
	set(warnings "-Wall -Wextra -Wno-unused-parameter")
	set(flags "-fPIC")
	set(ASHLEY_DEBUG_FLAGS "-O0 -g3")
	set(ASHLEY_RELEASE_FLAGS "-O3 -g0")
endif()

set(CMAKE_CXX_COMPILER_ARG1 "-std=c++0x")
set(CMAKE_CXX_FLAGS_BASE "${warnings} ${flags} ${debug_flags}")

include_directories(include test)

file(GLOB_RECURSE ASHLEY_CPP_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB ASHLEY_CPP_HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${ASHLEY_DEBUG_FLAGS}")
add_library(AshleyCPP-d ${ASHLEY_CPP_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${ASHLEY_RELEASE_FLAGS}")
add_library(AshleyCPP ${ASHLEY_CPP_SOURCES})

if (NOT EXCLUDE_TESTS)
	find_package(GTest REQUIRED)
	
	file (GLOB_RECURSE ASHLEY_TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cpp) 
	
	set(ASHLEY_TEST_FLAGS "-pthread")
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${ASHLEY_DEBUG_FLAGS} ${ASHLEY_TEST_FLAGS}")
	
	include_directories(${GTEST_INCLUDE_DIRS})
	
	add_executable(AshleyCPPTest ${ASHLEY_TEST_SOURCES})
	target_link_libraries(AshleyCPPTest AshleyCPP-d ${GTEST_BOTH_LIBRARIES})
endif (NOT EXCLUDE_TESTS)

install (TARGETS AshleyCPP-d DESTINATION lib)
install (TARGETS AshleyCPP DESTINATION lib)
install (DIRECTORY ${PROJECT_SOURCE_DIR}/include/Ashley DESTINATION include)

