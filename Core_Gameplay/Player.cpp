#include "Player.h"

Player::Player(int chips) {
    hand.reserve(2)
    chips = chips
};

void addToHand(uint32_t card) {
    hand.push_back(card);
}; 

Player::~Player() {}