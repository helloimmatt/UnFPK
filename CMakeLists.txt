cmake_minimum_required(VERSION 3.0)

project (unfpk)

file(GLOB unfpk_SRC
	"*.h"
	"*.cpp"
)

add_executable(unfpk ${unfpk_SRC})