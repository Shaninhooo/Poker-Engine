#include "ValueTable.h"



class Deck {
    private:
        std::vector<uint32_t> cards;
    public:
        Deck();
        ~Deck();  
        void shuffle();
        std::vector<uint32_t>  draw(int n);
        void print_deck();
        void reset();
}

