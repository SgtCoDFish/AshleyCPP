cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project("AshleyCPP")

set(dialect "-std=c++0x")
set(warnings "-Wall")
set(flags "-fmessage-length=0")
set(ASHLEY_DEBUG_FLAGS "-O0 -g3")
set(ASHLEY_RELEASE_FLAGS "-O3")

set(CMAKE_CXX_COMPILER_ARG1 "-std=c++0x")
set(CMAKE_CXX_FLAGS_BASE "${warnings} ${flags} ${debug_flags}")

include_directories("include" "test")

add_subdirectory("src")
add_subdirectory("test")

