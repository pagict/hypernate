#cmake_minimun_required(VERSION 3.7)

#find_package(gtest REQUIRED)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)

find_library(GTEST_LIB gtest /tmp/googletest/googletest/build)
add_executable(test_hypernate test_hypernate.cpp)
include_directories(/tmp/googletest/googletest/include)
target_link_libraries(test_hypernate ${GTEST_LIB} hypernate)
