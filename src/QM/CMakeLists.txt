add_library(libQM STATIC
boolean_function.cpp
literal_map.cpp
)

target_include_directories(libQM PUBLIC ${CMAKE_SOURCE_DIR}/include)
