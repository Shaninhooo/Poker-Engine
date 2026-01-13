#ifndef ENGINE_H
#define ENGINE_H

#include "Evaluator.h"
#include "../solver/PokerState.h"
#include <future>
#include <thread>
#include <random>
#include <cstring>
#include <cstdint>

using EvalPtr = std::shared_ptr<const Evaluator>;

// A lightning-fast RNG
struct FastRNG {
    uint64_t state;
    
    // The core algorithm
    inline uint64_t operator()() {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return state;
    }

    inline uint64_t range(uint64_t n) {
        if (n == 0) return 0;
        return operator()() % n;
    }
};

class Engine {
    private:
        EvalPtr eval;

        float simulate_one_hand(const PokerState& ps, const std::vector<uint32_t>& master_pool);
    public:
        Engine() : eval(std::make_shared<Evaluator>()) {}
        Engine(EvalPtr e) : eval(std::move(e)) {}
        ~Engine();  
        

        std::vector<uint32_t> create_master_pool(const PokerState& ps);

        std::pair<float, float> simulate_parallel(const PokerState& ps);

        int get_bucket(const PokerState& ps);
};

#endif