add_library(QM STATIC
prime_implicants.cpp
qm_process_handler.cpp
)

target_include_directories(QM PUBLIC ${CMAKE_SOURCE_DIR}/include)
