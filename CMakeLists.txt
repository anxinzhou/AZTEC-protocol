cmake_minimum_required(VERSION 3.13)
project(anonymous-payment)
set( CMAKE_CXX_FLAGS  "-L /usr/local/lib  ${CMAKE_CXX_FLAGS} -std=c++11 ")

add_executable(${PROJECT_NAME}   main.cpp aztec.cpp)

target_link_libraries(${PROJECT_NAME}  gmp gmpxx ff crypto)
# make install
#set(CMAKE_BUILD_TYPE  Release)
include(GNUInstallDirs)
set(MY_LIB aztec)
set(MY_HEADER aztec.h)
add_library(${MY_LIB} SHARED  aztec.cpp)
set_target_properties(${MY_LIB} PROPERTIES PUBLIC_HEADER "${MY_HEADER}")
target_include_directories(${MY_LIB} PUBLIC ${PROJECT_SOURCE_DIR})
install(TARGETS ${MY_LIB}
        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib
        PUBLIC_HEADER DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include)