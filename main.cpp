#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include "Monte_Carlo_Engine/Engine.h"
#include "Core_Gameplay/ValueTable.h"

int main() {
    ValueTable vt;
    // 1. Get the master deck to find card values
    const auto& deck = ValueTable::get_master_deck();

    // 2. Setup a test hand (e.g., Ace of Spades, Ace of Hearts)
    // Note: You'll need to know the indices in your master_deck
    std::array<uint32_t, 2> hero_hand = { deck[48], deck[49] }; 

    // 3. Setup community (e.g., Empty for pre-flop)
    std::vector<uint32_t> community = {};

    // 4. Setup the remaining deck (exclude hero cards)
    std::vector<uint32_t> unseen_deck;
    for(auto card : deck) {
        if (card != hero_hand[0] && card != hero_hand[1]) {
            unseen_deck.push_back(card);
        }
    }

    // 5. Run the engine
    // Engine(hero, community, pool, villains, iterations)
    Engine engine(hero_hand, community, unseen_deck, 3, 100000000); 

    // 1. Print Hero Hand
    std::cout << "Hero Hand: [ ";
    for (size_t i = 0; i < hero_hand.size(); ++i) {
        std::cout << vt.get_rank_name(hero_hand[i]) << " of " << vt.get_suit_name(hero_hand[i]);
        if (i < hero_hand.size() - 1) std::cout << " | "; // Separator between cards
    }
    std::cout << " ]" << std::endl;

    // 2. Print Community Cards (if any)
    if (!community.empty()) {
        std::cout << "Board:     [ ";
        for (size_t i = 0; i < community.size(); ++i) {
            std::cout << vt.get_rank_name(community[i]) << " of " << vt.get_suit_name(community[i]);
            if (i < community.size() - 1) std::cout << " | ";
        }
        std::cout << " ]" << std::endl;
    } else {
        std::cout << "Board:     [ PRE-FLOP ]" << std::endl;
    }

    std::cout << "--------------------------------------" << std::endl;
    
    std::cout << "Starting Simulation..." << std::endl;
    auto results = engine.simulate_parallel();

    std::cout << "Win Equity: " << results.first * 100.0f << "%" << std::endl;
    std::cout << "Total Wins: " << std::fixed << std::setprecision(0) << results.second << std::endl;

    return 0;
}