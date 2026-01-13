#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <array>
#include <cstdint>
#include "ValueTable.h"

struct HandScore {
    int category; // 9 = Straight Flush, 1 = High Card
    std::vector<int> kickers;

    // This allows you to do: if (hand1 > hand2)
    bool operator>(const HandScore& other) const {
        if (category != other.category) return category > other.category;
        return kickers > other.kickers; // std::vector compares element by element automatically
    }
};

class Evaluator {
    private:
        std::vector<uint16_t> fast_lut_flush;
        std::vector<uint16_t> fast_lut_unsuited;

        HandScore slow_evaluator(const std::array<uint32_t, 5>& hand, bool is_flush);
        void build_table(std::unordered_map<uint64_t, int>& f_map, std::unordered_map<uint64_t, int>& n_map);
        void gen_fast_lut(const std::unordered_map<uint64_t, int>& f_map, const std::unordered_map<uint64_t, int>& n_map);

    public:
        Evaluator();
        ~Evaluator();

        // 4. The ultra-fast evaluation
        inline int evaluate_fast(uint64_t prime_product, bool is_flush) const {
            if (is_flush) {
                return fast_lut_flush[prime_product];      // Instant jump 1
            } else {
                return fast_lut_unsuited[prime_product];  // Instant jump 2
            }
        }
        std::pair<int, std::array<uint32_t, 5>> evaluate_player(const std::array<uint32_t, 2>& hand, const std::array<uint32_t, 5>& community) const;
};

#endif