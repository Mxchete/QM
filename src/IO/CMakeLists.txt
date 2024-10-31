add_library(IO STATIC blif_reader.cpp)

target_include_directories(IO PUBLIC ${CMAKE_SOURCE_DIR}/include)
