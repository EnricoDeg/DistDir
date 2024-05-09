cmake_minimum_required(VERSION 3.0)

if(ENABLE_CUDA)
    set(LANG_CUDA CUDA)
endif()

project(distdir C ${LANG_CUDA})

if(ENABLE_CUDA)
    find_package(CUDA QUIET REQUIRED)
endif()

include(cmake/FetchCMocka.cmake)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(docs)
if(ENABLE_PYTHON)
    add_subdirectory(bindings/python)
endif()
