#ifndef PLAYER_H
#define PLAYER_H

#include "Table.h"

class Player {
    private:
        std::vector<uint32_t> hand;
        int chips;
    public:
        Player();
        ~Player();  

        void addToHand(uint32_t card);
}   

#endif