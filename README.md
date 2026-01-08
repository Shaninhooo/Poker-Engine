Poker Engine using Cactus Kev's algorithm with Perfect Hashing and Lookup Table


---

## Run Instructions

#### 1. Enter the build directory
cd build

#### 2. Tell CMake to look at the parent directory (..) for the CMakeLists.txt
cmake -DCMAKE_BUILD_TYPE=Release ..

#### 3. Build the project
make -j$(nproc)

#### 4. Run compliled file
./poker_sim

### Profiling
cmake -DCMAKE_BUILD_TYPE=Profile ..
gprof poker_sim gmon.out > analysis.txt