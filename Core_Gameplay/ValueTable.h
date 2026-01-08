#ifndef VALUE_TABLE_H
#define VALUE_TABLE_H

#include <vector>
#include <string>
#include <cstdint>
#include <array>

class ValueTable {
private:
    inline static const std::vector<std::string> rank_names = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "JACK", "QUEEN", "KING", "ACE"};
    inline static const std::vector<std::string> suit_names = {"HEARTS", "SPADES", "DIAMONDS", "CLUBS"};

  // Define these once so they are identical everywhere
    static constexpr uint32_t HEARTS   = 0x1000;
    static constexpr uint32_t SPADES   = 0x2000;
    static constexpr uint32_t DIAMONDS = 0x4000;
    static constexpr uint32_t CLUBS    = 0x8000;
public:
    // Constructor/Destructor defined right here
    ValueTable() {}
    ~ValueTable() {}

    static inline std::vector<uint32_t>& get_master_deck() {
        static std::vector<uint32_t> internal_deck = []() {
            std::vector<uint32_t> deck;
            deck.reserve(52);
            std::array<int, 13> primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};
            
            // Re-order the rank bits to use bits 16-28. 
            // We use bit 16 for '2' up to bit 28 for 'Ace'.
            // Because Ace uses bit 28, our suits MUST START at bit 29.
            uint32_t suits[4] = { HEARTS, SPADES, DIAMONDS, CLUBS };

            for (int r = 0; r < 13; ++r) {
                for (int s = 0; s < 4; ++s) {
                    // We use (1U << r) but start it at bit 16
                    uint32_t rank_bit = (1U << (r + 16));
                    
                    // CRITICAL FIX: We must ensure suits don't overlap with (1U << 28)
                    // Let's use bits 12-15 for suits instead, which are empty!
                    uint32_t suit_bit = (1U << (s + 12)); 

                    uint32_t card = rank_bit | suit_bit | (r << 8) | primes[r];
                    deck.push_back(card);
                }
            }
            return deck;
        }();
        return internal_deck;
    }

    // Name helpers defined right here
    static inline std::string get_suit_name(uint32_t card) {
        // Shift right by 12 to bring the suit bits (12-15) to the front
        uint32_t suit_val = (card >> 12) & 0xF;

        switch (suit_val) {
            case 1:  return "HEARTS";   // 0x1000 >> 12 = 1
            case 2:  return "SPADES";   // 0x2000 >> 12 = 2
            case 4:  return "DIAMONDS"; // 0x4000 >> 12 = 4
            case 8:  return "CLUBS";    // 0x8000 >> 12 = 8
            default: 
                return "UNKNOWN(" + std::to_string(suit_val) + ")";
        }
    }

    static inline std::string get_rank_name(uint32_t card) {
        int rank_index = (card >> 8) & 0xF;
        if (rank_index >= 0 && rank_index < 13) return rank_names[rank_index];
        return "??";
    }
};

#endif