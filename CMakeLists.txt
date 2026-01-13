cmake_minimum_required(VERSION 3.10)
project(PokerEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find threading library
find_package(Threads REQUIRED)

# 1. Performance Flags (Release Mode)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto")

# 2. Profiling Flags (New Profile Mode)
# This keeps -O2 so the code is still fast, but adds -pg for gprof
set(CMAKE_CXX_FLAGS_PROFILE "-O2 -pg -g")
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-pg")

include_directories(core engine solver)

add_executable(poker_sim 
    main.cpp 
    engine/Engine.cpp 
    engine/Evaluator.cpp
)

target_link_libraries(poker_sim PRIVATE Threads::Threads)