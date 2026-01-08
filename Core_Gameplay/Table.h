#ifndef TABLE_H
#define TABLE_H

#include "Deck.h"
#include "Player.h"

class Table {
    private:
        Deck deck;
        std::vector<Player> players = {};
        std::vector<uint32_t> community_cards = {};
    public:
        Table() : deck() {};
        ~Table();  

        int get_num_players() { return players.size(); }
        int get_num_community() { return community_cards.size(); }
        std::vector<uint32_t> get_community() { return community_cards; }

        void deal_flop();
        void deal_players();
};

#endif