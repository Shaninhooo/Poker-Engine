
## Monte Carlo Engine (Simulator)

Poker Engine using Cactus Kev's algorithm with Perfect Hashing and Lookup Table.

Simulates millions of random possible hands given the community cards and the hand the hero (user) has.
Returns the total win rate from these simulations.

---

## Counterfactual Regret Minimization (Solver / Decision Maker)

Actions here are made based off of **Regret** a value that represents how much you wish you had taken a different action in the past.
In short it is the difference between the payoff of that "other decision" and what you actually got.

**The Rule:** If an action (like Bluffing) would have resulted in a better outcome than what you actually did, your "Regret" for not bluffing increases. In the next round, the AI is more likely to choose the action with the highest positive regret.


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