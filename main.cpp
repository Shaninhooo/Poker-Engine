#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include "engine/Engine.h"
#include "core/ValueTable.h"

int main() {
    // 1. Setup Dependencies
    ValueTable vt;
    const auto& deck = ValueTable::get_master_deck();

    // 2. Initialize PokerState (The "Snapshot" of the hand)
    PokerState ps;
    
    // Let's give Hero Pocket Aces (usually the last cards in the deck)
    ps.hero_hand = { deck[48], deck[51] }; 

    // Let's simulate a Flop: 8s, 9s, 10s
    ps.board[0] = deck[25]; 
    ps.board[1] = deck[29];
    ps.board[2] = deck[33];
    ps.known_board_count = 3; 

    ps.pot = 100;
    ps.raw_history = "rc"; // raise-call

    // 3. Initialize the Engine with Rules and Logic
    // Passing the evaluator and config once
    Engine engine{}; 

    // 4. Print Table State for debugging
    std::cout << "--- Poker Simulation Test ---" << std::endl;
    std::cout << "Hero Hand: [ ";
    for (uint32_t card : ps.hero_hand) {
        std::cout << vt.get_rank_name(card) << " of " << vt.get_suit_name(card) << " ";
    }
    std::cout << "]" << std::endl;

    std::cout << "Board:     [ ";
    for (int i = 0; i < ps.known_board_count; ++i) {
        std::cout << vt.get_rank_name(ps.board[i]) << " of " << vt.get_suit_name(ps.board[i]) << " ";
    }
    std::cout << "]" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    
    // 5. Run Parallel Simulation
    std::cout << "Starting 100,000,000 iterations..." << std::endl;
    
    // Start Timer
    auto start = std::chrono::high_resolution_clock::now();
    
    auto results = engine.simulate_parallel(ps);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // 6. Output Results
    std::cout << "Simulation Complete in " << elapsed.count() << " seconds." << std::endl;
    std::cout << "Win Equity: " << std::fixed << std::setprecision(2) << results.first * 100.0f << "%" << std::endl;
    std::cout << "Total Wins: " << std::setprecision(0) << results.second << std::endl;
    std::cout << "Speed:      " << (100.0 / elapsed.count()) << " million hands/sec" << std::endl;

    return 0;
}