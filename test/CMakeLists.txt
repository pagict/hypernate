#cmake_minimun_required(VERSION 3.7)

#find_package(gtest REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

enable_testing()
add_executable(test_hypernate test_hypernate.cpp)
target_link_libraries(test_hypernate gtest hypernate)
