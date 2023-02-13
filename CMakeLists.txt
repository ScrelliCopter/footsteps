cmake_minimum_required(VERSION 3.1)
project(footsteps C)

set(CMAKE_C_STANDARD 11)

add_executable(footsteps effects.h effects.c footsteps.c)
if (WIN32)
	target_link_libraries(footsteps OpenAL32.dll sndfile m)
elseif (APPLE)
	find_library(OPENAL soft_oal REQUIRED)
	find_library(SNDFILE sndfile REQUIRED)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(sndfile REQUIRED IMPORTED_TARGET sndfile)
	target_link_libraries(footsteps ${OPENAL} PkgConfig::sndfile m)
	target_include_directories(footsteps PRIVATE ${OPENAL}/Headers)

	set_target_properties(footsteps PROPERTIES
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_GUI_IDENTIFIER com.a-dinosaur.footsteps
		MACOSX_BUNDLE_NAME footsteps
		MACOSX_BUNDLE_VERSION 0.1
		MACOSX_BUNDLE_SHORT_VERSION_STRING 0.1)
		#MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/yea.plist.in)
else()
	target_link_libraries(footsteps openal sndfile m)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/dat DESTINATION ${CMAKE_BINARY_DIR})
