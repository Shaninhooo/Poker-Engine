#include "Deck.h"

Deck::Deck(){
    cards = ValueTable::get_master_deck;
}; 

void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
};

std::vector<uint32_t>  Deck::draw(int n) {
    std::vector<uint32_t> hand;
    // Burn Card
    cards.pop_back();

    // Draw Cards
    for (int i = 0; i < n; ++i) {
        if (!cards.empty()) {
            hand.push_back(cards.back());
            cards.pop_back();
        }
    }

    return drawn
};

void Deck::print_deck() {
    for (auto card : cards) {
        std::cout << vt.get_rank_names(card) << " of " << vt.get_suit_names(card) << std::endl;
    }
};

void Deck::reset() {
    cards = ValueTable::get_master_deck;
};

Deck::~Deck() {}