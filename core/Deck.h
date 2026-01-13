#ifndef DECK_H
#define DECK_H

#include "ValueTable.h"
#include <random>
#include <algorithm>
#include <iostream>

class Deck {
    private:
        std::vector<uint32_t> cards;
    public:
        Deck();
        ~Deck();  
        void shuffle();
        std::vector<uint32_t> draw(int n);
        void print_deck();
        void reset();
};

#endif