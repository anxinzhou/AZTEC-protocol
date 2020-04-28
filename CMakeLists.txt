cmake_minimum_required(VERSION 3.13)

project(anonymous-payment)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(SGX REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(EDL_SEARCH_PATHS Enclave Enclave/TrustedLibrary)

set(E_SRCS Enclave/Enclave.cpp)
set(T_SRCS Enclave/TrustedLibrary/Libcxx.cpp)
set(SGX_HW OFF)
set(SGX_MODE Debug)
message(STATUS "SGX HW ${SGX_HW}")
message(STATUS "SGX MODE ${SGX_MODE}")
if(SGX_HW AND SGX_MODE STREQUAL "Release")
    set(LDS Enclave/Enclave.lds)
else()
    set(LDS Enclave/Enclave_debug.lds)
endif()
add_trusted_library(trusted_lib SRCS ${T_SRCS} EDL Enclave/Enclave.edl EDL_SEARCH_PATHS ${EDL_SEARCH_PATHS})
add_enclave_library(enclave SRCS ${E_SRCS} TRUSTED_LIBS trusted_lib EDL Enclave/Enclave.edl EDL_SEARCH_PATHS ${EDL_SEARCH_PATHS} LDSCRIPT ${LDS})
enclave_sign(enclave KEY Enclave/Enclave_private.pem CONFIG Enclave/Enclave.config.xml)

set(SRCS App/App.cpp App/TrustedLibrary/Libcxx.cpp)

add_untrusted_executable(Cxx11SGXDemo SRCS ${SRCS} EDL Enclave/Enclave.edl EDL_SEARCH_PATHS ${EDL_SEARCH_PATHS})
add_dependencies(Cxx11SGXDemo enclave-sign)

target_link_libraries(Cxx11SGXDemo)

#set( CMAKE_CXX_FLAGS  "-L /usr/local/lib  ${CMAKE_CXX_FLAGS} -std=c++11 ")
#
#add_executable(${PROJECT_NAME}   main.cpp aztec.cpp)
#
#target_link_libraries(${PROJECT_NAME}  gmp gmpxx ff crypto)
## make install
#set(CMAKE_BUILD_TYPE  Release)
#include(GNUInstallDirs)
#set(MY_LIB aztec)
#set(MY_HEADER aztec.h)
#add_library(${MY_LIB} SHARED  aztec.cpp)
#set_target_properties(${MY_LIB} PROPERTIES PUBLIC_HEADER "${MY_HEADER}")
#target_include_directories(${MY_LIB} PUBLIC ${PROJECT_SOURCE_DIR})
#install(TARGETS ${MY_LIB}
#        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib
#        PUBLIC_HEADER DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include)