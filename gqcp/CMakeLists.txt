# Entry point for the CMake configuration of the C++ library
add_library(gqcp SHARED "")

target_link_libraries(gqcp PUBLIC gqcp_version)

target_compile_options(gqcp PUBLIC -m64 -pipe)
target_compile_options(gqcp PUBLIC "$<IF:$<CONFIG:Debug>,-g,-O2>")
target_compile_options(gqcp PUBLIC "$<$<CXX_COMPILER_ID:GNU>:-pthread>")
target_link_options(gqcp PUBLIC "$<$<CXX_COMPILER_ID:GNU>:-pthread>")
target_compile_features(gqcp PUBLIC cxx_std_14)

if (OPTIMIZE_FOR_NATIVE)
    target_compile_options(gqcp PRIVATE "$<IF:$<CXX_COMPILER_ID:Intel>,-xHost,-march=native>")
endif()


# Process the version.hpp-file
# Get the long git SHA1 (https://stackoverflow.com/a/21028226/7930415)
execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --always --abbrev=40 --dirty
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_SHA1
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)


add_subdirectory(src)


target_include_directories(gqcp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/gqcp/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/gqcp/include>
)

target_link_libraries(gqcp
    PUBLIC
        Eigen3::Eigen
        Boost::boost
        Int2::Int2
        cint::cint
        MKL::MKL
)
 
# Add rt library on Linux environments
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_library(LIB_RT rt)
    target_link_libraries(gqcp PUBLIC ${LIB_RT})
endif()

target_compile_options(gqcp PUBLIC -DEIGEN_USE_MKL_ALL -DMKL_LP64)
