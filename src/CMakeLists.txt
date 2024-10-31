cmake_minimum_required(VERSION 3.5)

# Add subdirectories for each library
add_subdirectory(IO)
add_subdirectory(QM)

set(SOURCE_FILES main.cpp)

# Add executable target for the main binary
add_executable(qm ${SOURCE_FILES})

# Include directories for all libraries
target_include_directories(qm PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link statically to the libraries
target_link_libraries(qm PRIVATE QM IO)
