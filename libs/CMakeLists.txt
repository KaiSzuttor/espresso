cmake_minimum_required(VERSION 3.0)

add_library(Random123 INTERFACE)
target_include_directories(Random123 SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Random123-1.09/include)

if(WITH_BENCHMARKS)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
  add_subdirectory(benchmark)
endif(WITH_BENCHMARKS)
