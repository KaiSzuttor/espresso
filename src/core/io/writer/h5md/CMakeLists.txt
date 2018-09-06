add_library(H5mdCore SHARED h5md_core.cpp)
target_include_directories(H5mdCore SYSTEM PUBLIC
                           ${CONAN_INCLUDE_DIRS_H5XX}
)
add_dependencies(H5mdCore EspressoConfig)

