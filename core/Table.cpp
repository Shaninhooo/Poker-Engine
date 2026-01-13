#include "Table.h"

void Table::deal_flop() {
    deck.draw(3);
};

void Table::deal_players() {
    for (int i = 0; i < 2; ++i) {
        for (auto& player : players) {
            player.addToHand(deck.draw(1)[0]);
        }
    }
};

Table::~Table() {}  