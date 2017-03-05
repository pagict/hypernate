cmake_minimum_required(VERSION 3.7)
project(hypernate)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp connection.cpp connection.h configuration_keys.h persistent_object.cpp persistent_object.h plain_table.h school.h hyper_column.cpp hyper_column.h hyper_table.cpp hyper_table.h)
add_executable(hypernate ${SOURCE_FILES})
target_link_libraries(hypernate mysqlcppconn)