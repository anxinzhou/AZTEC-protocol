cmake_minimum_required(VERSION 3.13)
project(anonymous-payment)
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -I /usr/local/include -I /Users/anxin/ethash/include  -I /usr/local/opt/openssl/include/openssl -L /usr/local/lib -L /usr/local/opt/openssl/lib -L /Users/anxin/ethash/build/lib/keccak -lkeccak -lgmp -lgmpxx -lff -lcrypto")

add_executable(${PROJECT_NAME}   main.cpp aztec.cpp)

