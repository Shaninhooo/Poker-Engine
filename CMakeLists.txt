cmake_minimum_required(VERSION 3.10)
project(PokerEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find threading library
find_package(Threads REQUIRED)

# Performance Flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto")

# Include the directories so the compiler can find your .h files
include_directories(Core_Gameplay Monte_Carlo_Engine)

# Define the executable and the paths to your source files
# I'm assuming your main.cpp is in the root or one of these folders
add_executable(poker_sim 
    main.cpp 
    Monte_Carlo_Engine/Engine.cpp 
    Core_Gameplay/Evaluator.cpp
)

target_link_libraries(poker_sim PRIVATE Threads::Threads)