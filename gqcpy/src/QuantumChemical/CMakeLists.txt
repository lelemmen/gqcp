list(APPEND python_bindings_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/Spin_bindings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SpinResolved_bindings.cpp
)

set(python_bindings_sources ${python_bindings_sources} PARENT_SCOPE)
