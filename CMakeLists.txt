#project config

cmake_minimum_required(VERSION 3.10)

project(mina-asm)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)

else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -Wall -Wextra -pthread -O3")
endif()


# add source files
file(GLOB src_files
	"src/*.h"
	"src/*.cpp"

)

#includes 

include_directories("src/headers")
include_directories("src")


add_executable(mina-asm ${src_files})