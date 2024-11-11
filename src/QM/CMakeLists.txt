add_library(QM STATIC
# boolean_function.cpp
minterm_map.cpp
qm_process_handler.cpp
)

target_include_directories(QM PUBLIC ${CMAKE_SOURCE_DIR}/include)
