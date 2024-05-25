find_package(Python3)
find_package(MPI REQUIRED)

set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")

set(PY_OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/distdir")

configure_file(
    ${SETUP_PY_IN}
    ${SETUP_PY}
)

add_custom_command(OUTPUT "${PY_OUTPUT}"
                   COMMAND Python3::Interpreter ${SETUP_PY} build_ext --build-lib ${PROJECT_SOURCE_DIR}/lib/pydistdir
                   COMMAND ${CMAKE_COMMAND} -E touch ${PY_OUTPUT}
               )

add_custom_target(pydistdir ALL DEPENDS ${PY_OUTPUT})

#install(CODE "execute_process(COMMAND Python3::Interpreter ${SETUP_PY} install)")
