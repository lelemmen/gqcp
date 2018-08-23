# In this CMake file, we will include the headers and link to the necessary libraries


# Include this project's headers
target_include_directories(${LIBRARY_NAME} PRIVATE ${PROJECT_INCLUDE_FOLDER})

# Include the boost headers (dynamic bitset)
target_include_directories(${LIBRARY_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# Include Eigen
target_link_libraries(${LIBRARY_NAME} PUBLIC Eigen3::Eigen)

