cmake_minimum_required(VERSION 3.14)
project(Battleships)

set(CMAKE_CXX_STANDARD 20)

# -------------------------------------------------------
# Main game executable
# -------------------------------------------------------
add_executable(Battleships main.cpp
        Game.cpp
        Player.h
)

# -------------------------------------------------------
# Google Test (fetched automatically — no install needed)
# -------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent GoogleTest from overriding compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(BattleshipsTests TestCases.cpp)
target_compile_definitions(BattleshipsTests PRIVATE TESTING)
target_link_libraries(BattleshipsTests PRIVATE GTest::gtest_main)

include(GoogleTestHelpers OPTIONAL)
include(GoogleTest)
gtest_discover_tests(BattleshipsTests)