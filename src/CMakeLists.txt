cmake_minimum_required(VERSION 3.20)
project(enigma)

set(CMAKE_CXX_STANDARD 17)

add_executable(enigma main.cpp Enigma.h Enigma.cpp EnigmaIO.h EnigmaIO.cpp)
