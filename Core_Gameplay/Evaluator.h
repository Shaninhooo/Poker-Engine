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
        static std::unordered_map<long,int> flush_lookup;
        static std::unordered_map<long,int> unsuited_lookup;

        HandScore slow_evaluator(const std::array<uint32_t, 5>& hand, bool is_flush);
        void build_table();

    public:
        Evaluator() { build_table(); }
        ~Evaluator();

        std::pair<int, std::array<uint32_t, 5>> evaluate_player(const std::array<uint32_t, 2>& hand, const std::array<uint32_t, 5>& community);
};

#endif