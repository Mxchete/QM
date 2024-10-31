add_library(libIO STATIC blif_reader.cpp)

target_include_directories(libIO PUBLIC ${CMAKE_SOURCE_DIR}/include)
