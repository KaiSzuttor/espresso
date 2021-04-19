add_library(interactions INTERFACE)
target_include_directories(
  interactions INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)
target_link_libraries(interactions INTERFACE EspressoUtils)

install(TARGETS interactions LIBRARY DESTINATION ${PYTHON_INSTDIR}/espressomd)

if(WITH_TESTS)
  add_subdirectory(tests)
endif()
