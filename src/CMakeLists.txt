# Find MPI library
find_package(MPI REQUIRED COMPONENTS C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DERROR_CHECK")

set(SOURCE_EXE  check.c
	        quicksort.c
                mergesort.c
                group.c
                idxlist.c
                bucket.c
                map.c
                exchange.c)

add_library(distdir SHARED ${SOURCE_EXE})

install (
  TARGETS distdir
  LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib
  ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib)

set(HEADERS_C distdir.h)
install(FILES ${HEADERS_C} DESTINATION ${PROJECT_SOURCE_DIR}/include)
