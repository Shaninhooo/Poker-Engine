#include "Engine.h"

static thread_local FastRNG rng = { 0x123456789ULL };

// This helper sits inside your Engine class
std::vector<uint32_t> Engine::create_master_pool(const PokerState& ps) {
    const std::vector<uint32_t>& full_deck = ValueTable::get_master_deck();
    std::vector<uint32_t> pool;
    pool.reserve(52); 

    for (uint32_t card : full_deck) {
        bool is_used = false;

        // Check against Hero cards
        for (uint32_t h : ps.hero_hand) {
            if (card == h) { is_used = true; break; }
        }
        if (is_used) continue;

        // Check against Board cards
        for (int i = 0; i < ps.known_board_count; ++i) {
            if (card == ps.board[i]) { is_used = true; break; }
        }

        if (!is_used) {
            pool.push_back(card);
        }
    }
    
    return pool;
}


float Engine::simulate_one_hand(const PokerState& ps, const std::vector<uint32_t>& master_pool) {
    const int pool_size = static_cast<int>(master_pool.size());
    
    // Safety check for the Master Pool
    if (pool_size < 2 || master_pool.empty()) return 0.0f;

    int community_needed = 5 - ps.known_board_count;
    int total_needed = community_needed + 2; 

    // Safety check: Do we actually have enough cards to deal?
    if (pool_size < total_needed) return 0.0f;

    uint32_t local_pool[52]; 
    // memcpy is safe here because we checked pool_size
    std::memcpy(local_pool, master_pool.data(), pool_size * sizeof(uint32_t));
    
    // Shuffle
    for (int i = 0; i < total_needed; ++i) {
        uint64_t r = rng.range(pool_size - i);
        int j = i + static_cast<int>(r);
        std::swap(local_pool[i], local_pool[j]);
    }

    // Build Board
    std::array<uint32_t, 5> full_board = ps.board;
    for (int i = 0; i < community_needed; ++i) {
        full_board[ps.known_board_count + i] = local_pool[i];
    }
       
    // Evaluate
    auto hero_res = eval->evaluate_player(ps.hero_hand, full_board);
    
    // Villain hand comes AFTER board cards in the shuffled section
    std::array<uint32_t, 2> v_hand = { local_pool[community_needed], local_pool[community_needed + 1] };
    auto v_res = eval->evaluate_player(v_hand, full_board);

    if (v_res.first < hero_res.first) return 0.0f; 
    if (v_res.first == hero_res.first) return 0.5f;
    return 1.0f;
}


std::pair<float, float> Engine::simulate_parallel(const PokerState& ps) {

    // Number of iterations
    const int total_iterations = 100'000'000;
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 1; // Safety check

    int iters_per_thread = total_iterations / num_threads;
    std::vector<std::future<float>> tasks;

    // Initialising Master Pool
    std::vector<uint32_t> master_pool = create_master_pool(ps);

    // Creating threads
    for (int t = 0; t < num_threads; ++t) {
        // We launch a "Task" that represents millions of iterations
        tasks.push_back(std::async(std::launch::async, [this, iters_per_thread, &ps, master_pool, t]() {
            rng.state = 0x123456789ULL + (uint64_t)t + (uint64_t)std::time(nullptr);

            float local_wins = 0;
            for (int i = 0; i < iters_per_thread; ++i) {
                local_wins += this->simulate_one_hand(ps, master_pool);
            }
            return local_wins;
        }));
    }

    // Combine results
    float total_wins = 0;
    for (auto& t : tasks) total_wins += t.get();

    long long actual_total_sims = (long long)iters_per_thread * num_threads;
    float win_rate = total_wins / (float)actual_total_sims;
    
    return { win_rate, total_wins };
}

Engine::~Engine() {}