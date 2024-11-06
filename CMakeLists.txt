cmake_minimum_required(VERSION 3.13)

project( RayScan  VERSION 1.0  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(asb)
target_include_directories(asb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/asb")
target_link_libraries(asb PUBLIC warthog::core)

add_executable(run
	main.cpp
	Entry.cpp
	ScenarioLoader.cpp
	Timer.cpp)
target_link_libraries(run PUBLIC asb)

add_executable(wrun
	main.cpp
	WarthogEntry.cpp
	ScenarioLoader.cpp
	Timer.cpp)
target_link_libraries(wrun PUBLIC asb)

add_subdirectory(asb)
add_subdirectory(extern/warthog-core EXCLUDE_FROM_ALL)
