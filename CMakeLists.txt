project(DockerClientpp)
cmake_minimum_required(VERSION 2.6)

include_directories(include)

set(CMAKE_BUILD_TYPE Debug)

find_package(GTest)
find_package(Threads REQUIRED)

enable_testing()

file(GLOB TEST_SRC_FILES test/*.cpp)
file(GLOB SRC_FILES src/*.cpp)
add_executable(${PROJECT_NAME}Test ${TEST_SRC_FILES} ${SRC_FILES})

target_compile_features(${PROJECT_NAME}Test PRIVATE cxx_range_for)
target_link_libraries(${PROJECT_NAME}Test
  ${GTEST_BOTH_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  )

add_test(test ${PROJECT_NAME}Test)
