cmake_minimum_required(VERSION 3.2.0)

project(generate-ycm-conf)


if(MSVC)

add_definitions(/std:c++17 /permissive-)

else()

add_definitions(-std=c++17)

endif(MSVC)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(MINI_TEST)
find_package(boost)

include_directories( "${CMAKE_SOURCE_DIR}/include")

if(MINI_TEST_FOUND AND BOOST_FOUND)

	add_executable(gyc gyc/main.cpp)

	target_include_directories("gyc" PRIVATE ${MINI_TEST_INCLUDE_DIR} ${BOOST_INCLUDE_DIR})
	target_link_directories("gyc" PRIVATE ${BOOST_LIBRARYS_DIR})
	if(MSVC)
		if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
			target_link_libraries("gyc" libboost_filesystem-vc142-mt-x64-1_71.lib)
		else()
			target_link_libraries("gyc" libboost_filesystem-vc142-mt-gd-x64-1_71.lib)
		endif()
	else()
		target_link_libraries("gyc" libboost_filesystem.so)
	endif()

endif()
