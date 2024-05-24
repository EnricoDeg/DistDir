set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

if(CMAKE_C_COMPILER_ID STREQUAL "NVHPC")
    if(ENABLE_CUDA)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -acc=verystrict -Minfo=accel,inline -gpu=cc80 -DCUDA")
    endif()
endif()

add_executable(example_basic1cpp example_basic1.cpp)
target_link_libraries (example_basic1cpp distdir)
target_include_directories(example_basic1cpp PRIVATE ${PROJECT_SOURCE_DIR})
if(ENABLE_CUDA)
    set_property(TARGET example_basic1cpp PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

install (TARGETS
  example_basic1cpp # executables
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bindings/C++/examples)
