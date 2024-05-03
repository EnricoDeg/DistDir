cmake_minimum_required(VERSION 3.0)

project(distdir C)

include(cmake/FetchCMocka.cmake)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(docs)
if(ENABLE_PYTHON)
    add_subdirectory(bindings/python)
endif()
