cmake_minimum_required(VERSION 3.0)
project(hello)
set(CMAKE_CXX_STANDARD 11)
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/hello.cpp ${CMAKE_CURRENT_SOURCE_DIR}/PrimeChecker.cpp)

enable_testing()
add_executable(${PROJECT_NAME}-Runner TestPrimeChecker.cpp ${CMAKE_CURRENT_SOURCE_DIR}/PrimeChecker.cpp)
add_test(NAME ${PROJECT_NAME}-Runner COMMAND ${PROJECT_NAME}-Runner)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

add_custom_target(coverage
    COMMAND ${CMAKE_BINARY_DIR}/hello-Runner
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
    COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info
    COMMAND lcov --remove ${CMAKE_BINARY_DIR}/coverage/coverage.info '/usr/*' --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info
    COMMAND lcov --list ${CMAKE_BINARY_DIR}/coverage/coverage.info
    COMMAND genhtml --frames -o ${CMAKE_BINARY_DIR}/coverage-report-frames --prefix source/ ${CMAKE_BINARY_DIR}/coverage/coverage.info
    COMMAND lcov_cobertura ./coverage/coverage.info --output ${CMAKE_BINARY_DIR}/coverage/coverage.xml --base-dir ${CMAKE_BINARY_DIR} 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests and generating coverage report"
)

