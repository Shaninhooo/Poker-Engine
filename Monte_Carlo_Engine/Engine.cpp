#include "Engine.h"

Engine::Engine(
    std::array<uint32_t, 2> hero,
    const std::vector<uint32_t>& community,
    const std::vector<uint32_t>& unseen_deck,
    int num_villains,
    int iters
) : iterations(iters), 
    n_villains(num_villains), 
    hero_hand(hero), 
    pool(unseen_deck),
    eval() 
{
    this->community_count = static_cast<int>(community.size());
    this->community_needed = 5 - community_count;

    int cards_for_villains = num_villains * 2;
    this->cards_to_deal = cards_for_villains + community_needed;

    // Safely copy the vector into the fixed-size array
    // This ensures we don't overflow if the community has 0, 3, 4, or 5 cards
    this->known_community.fill(0);
    for (int i = 0; i < community_count; ++i) {
        known_community[i] = community[i];
    }
}


float Engine::simulate_one_hand(std::vector<uint32_t>& local_pool, std::mt19937& rng) {

    // Number of unseen cards left for community
    int pool_size = local_pool.size();

    // Shuffling
    for (int i = 0; i < cards_to_deal; ++i) {
        int remaining_range = pool_size - i;
        int j = i + (rng() % remaining_range); // significantly faster than dist(rng)
        std::swap(local_pool[i], local_pool[j]);
    }

    // Fill Community Cards  
    std::array<uint32_t, 5> full_board = known_community;
    for (int i = 0; i < community_needed; ++i) {
        full_board[community_count + i] = local_pool[i];
    }
       
    // 1. Get Hero's rank once
    auto hero_eval = eval.evaluate_player(hero_hand, full_board);;
    int hero_rank = hero_eval.first;

    int ties = 0;
    int offset = community_needed; // Start picking villain cards after the board cards
    
    for (int v = 0; v < this->n_villains; ++v) {
        // Constructing a temporary array on the fly is faster than filling 'all_villain_hands'
        std::array<uint32_t, 2> v_hand = { local_pool[offset], local_pool[offset + 1] };
        int v_rank = eval.evaluate_player(v_hand, full_board).first;

        if (v_rank < hero_rank) return 0.0f; // Villain wins, EXIT EARLY
        if (v_rank == hero_rank) ties++;
        
        offset += 2;
    }

    if (ties > 0) return 1.0f / (ties + 1);
    return 1.0f;
};

std::pair<float, float> Engine::simulate_parallel() {
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 1; // Safety check

    int iterations_per_thread = iterations / num_threads;

    std::vector<std::future<float>> tasks;

    for (int t = 0; t < num_threads; ++t) {
        // We launch a "Task" that represents millions of iterations
        tasks.push_back(std::async(std::launch::async, [this, iterations_per_thread]() {
            float local_wins = 0;
            
            // 1. Thread-Local Randomness 
            std::mt19937 rng(std::random_device{}());
            const std::array<uint32_t, 2> local_hero = this->hero_hand;

            // 2. Thread-Local Deck Scratchpad (Prevents memory conflicts)
            std::vector<uint32_t> local_pool = this->pool; 

            for (int i = 0; i < iterations_per_thread; ++i) {
                local_wins += this->simulate_one_hand(local_pool, rng);
            }
            return local_wins;
        }));
    }

    // Combine results
    float total_wins = 0;
    for (auto& t : tasks) total_wins += t.get();

    // Fix: Use the actual total iterations for the percentage
    float win_percentage = total_wins / (float)this->iterations;
    return { win_percentage, total_wins };
}

Engine::~Engine() {}