add_library(IO STATIC pla_reader.cpp)

target_include_directories(IO PUBLIC ${CMAKE_SOURCE_DIR}/include)
