cmake_minimum_required(VERSION 3.10)
project(footsteps C)

set(CMAKE_C_STANDARD 11)

add_executable(footsteps effects.h effects.c footsteps.c)
if (WIN32)
	target_link_libraries(footsteps OpenAL32.dll sndfile m)
else()
	target_link_libraries(footsteps openal sndfile m)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/dat DESTINATION ${CMAKE_BINARY_DIR})
