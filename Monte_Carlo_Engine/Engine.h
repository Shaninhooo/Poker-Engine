#ifndef ENGINE_H
#define ENGINE_H

#include "Evaluator.h"
#include <future>
#include <thread>
#include <random>
#include <cstdint>


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

    // Add this helper method:
    inline uint64_t range(uint64_t n) {
        if (n == 0) return 0;
        return operator()() % n;
    }
};

class Engine {
    private:
        Evaluator eval;
        int iterations;
        int community_count;
        int n_villains;
        int cards_to_deal;
        int community_needed;

        std::array<uint32_t, 2> hero_hand; // Store locally for speed
        std::array<uint32_t, 5> known_community; 
        std::vector<uint32_t> pool; // The remaining deck


        float simulate_one_hand(std::vector<uint32_t>& local_pool, std::array<uint32_t, 5>& full_board);
    public:
        Engine(
            std::array<uint32_t, 2> hero,
            const std::vector<uint32_t>& community,
            const std::vector<uint32_t>& unseen_deck,
            int num_villains,
            int iters = 100000000
        );
        ~Engine();  
        
        std::pair<float, float> simulate_parallel();
};

#endif