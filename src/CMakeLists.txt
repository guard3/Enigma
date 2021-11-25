cmake_minimum_required(VERSION 3.20.2)
project(enigma)

set(CMAKE_CXX_STANDARD 17)

add_executable(enigma main.cpp Enigma2.h Enigma2.cpp Console.h Console.cpp)

if (WIN32)
	target_sources(enigma PUBLIC Win32.h)
endif()