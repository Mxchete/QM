cmake_minimum_required(VERSION 3.2)
project(QM)

set(SOURCE_FILES main.cpp)

add_executable(qm ${SOURCE_FILES})
install(TARGETS qm DESTINATION ${PROJECT_SOURCE_DIR}/../bin/)
