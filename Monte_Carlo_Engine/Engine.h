#ifndef ENGINE_H
#define ENGINE_H

#include "Evaluator.h"
#include <future>
#include <thread>
#include <random>

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

        float simulate_one_hand(std::vector<uint32_t>& local_pool, std::mt19937& rng);

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