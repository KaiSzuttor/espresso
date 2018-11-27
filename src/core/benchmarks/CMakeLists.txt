set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL ON)

function(BENCHMARK)
  cmake_parse_arguments(BENCHMARK "" "NAME" "SRC" ${ARGN})
  set(BENCHMARK_NAME BM_${BENCHMARK_NAME})
  add_executable(${BENCHMARK_NAME} ${BENCHMARK_SRC})
  target_link_libraries(${BENCHMARK_NAME} PRIVATE benchmark ${Boost_LIBRARIES})
  target_include_directories(${BENCHMARK_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/benchmark/include ${CMAKE_SOURCE_DIR}/src/core ${Boost_INCLUDE_DIRS})
  target_include_directories(${BENCHMARK_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/libs/blaze)
  target_link_libraries(${BENCHMARK_NAME} PRIVATE blaze)
endfunction(BENCHMARK)

BENCHMARK(NAME lb_mode_calculation SRC lb_mode_calculation.cpp)
