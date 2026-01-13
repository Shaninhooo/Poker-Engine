#include "Deck.h"

Deck::Deck(){
    cards = ValueTable::get_master_deck();
}

void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

std::vector<uint32_t>  Deck::draw(int n) {
    std::vector<uint32_t> drawn;
    // Burn Card
    cards.pop_back();

    // Draw Cards
    for (int i = 0; i < n; ++i) {
        if (!cards.empty()) {
            drawn.push_back(cards.back());
            cards.pop_back();
        }
    }

    return drawn;
}

void Deck::print_deck() {
    for (auto card : cards) {
        std::cout << ValueTable::get_rank_name(card) << " of " << ValueTable::get_suit_name(card) << std::endl;
    }
}

void Deck::reset() {
    cards = ValueTable::get_master_deck();
}

Deck::~Deck() {}