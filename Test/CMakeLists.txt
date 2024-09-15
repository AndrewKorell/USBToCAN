cmake_minimum_required(VERSION 3.14)
project(tests)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED_ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD_17)
set(CMAKE_CXX_STANDARD_REQUIRED_ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_language(C CXX ASM)



include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

message(STATUS "CC set to: $ENV{CC}")
message(STATUS "CXX set to:$ENV{CXX}")

enable_testing()


add_executable(
	test_can_open
	
	test_can_open.cpp
)

add_executable(
	test_main
	test_main.cpp
)

target_link_libraries(
	test_can_open
	can_open
	GTest::gtest_main
	GTest::gtest
)

target_link_libraries(
	test_main PUBLIC
    GTest::gtest_main
	GTest::gtest
)



include(GoogleTest)
gtest_discover_tests(test_can_open)
gtest_discover_tests(test_main)
