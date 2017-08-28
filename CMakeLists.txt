project(DockerClientpp)
cmake_minimum_required(VERSION 2.6)

include_directories("include")
include_directories("ext/libarchive/libarchive")

file(GLOB DOCKER_CLIENT_PP_SRC_FILES src/*.cpp)

find_package(Threads REQUIRED)
add_subdirectory("ext/libarchive")

set(DOCKER_CLIENT_PP_LIB_TARGET DockerClientpp)

add_library(${DOCKER_CLIENT_PP_LIB_TARGET} SHARED ${DOCKER_CLIENT_PP_SRC_FILES})
set_target_properties(${DOCKER_CLIENT_PP_LIB_TARGET}
  PROPERTIES
  CXX_STANDARD 14)
target_link_libraries(${DOCKER_CLIENT_PP_LIB_TARGET}
  archive)


if (ENABLE_TEST)
  enable_testing()

  include_directories("ext/googletest/googletest/include")
  include_directories("ext/googletest/googlemock/include")
  add_subdirectory("ext/googletest")

  file(GLOB DOCKER_CLIENT_PP_TEST_SRC_FILES test/*.cpp)

  set(CMAKE_BUILD_TYPE Debug)

  add_executable(${PROJECT_NAME}Test
    ${DOCKER_CLIENT_PP_TEST_SRC_FILES}
    ${DOCKER_CLIENT_PP_SRC_FILES})

  set_target_properties(${PROJECT_NAME}Test
    PROPERTIES
    CXX_STANDARD 14)

  target_link_libraries(${PROJECT_NAME}Test
    gmock
    ${CMAKE_THREAD_LIBS_INIT}
    archive)

  add_test(DockerClientppTest ${PROJECT_NAME}Test)
endif()
