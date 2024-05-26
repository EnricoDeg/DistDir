cmake_minimum_required(VERSION 3.0)

if(ENABLE_CUDA)
    set(LANG_CUDA CUDA)
endif()

if(ENABLE_CXX)
    set(LANG_CXX CXX)
endif()

if(ENABLE_FORTRAN)
    set(LANG_FC Fortran)
endif()

project(distdir C ${LANG_CXX} ${LANG_CUDA} ${LANG_FC})

if(ENABLE_CUDA)
    find_package(CUDA QUIET REQUIRED)
endif()

include(cmake/FetchCMocka.cmake)

add_subdirectory(src)
add_subdirectory(examples)

if(ENABLE_CXX)
    add_subdirectory(bindings/C++)
    add_subdirectory(bindings/C++/examples)
endif()

if(ENABLE_FORTRAN)
    add_subdirectory(bindings/Fortran/examples)
endif()

add_subdirectory(tests)
add_subdirectory(docs)

if(ENABLE_PYTHON)
    add_subdirectory(bindings/python)
endif()
