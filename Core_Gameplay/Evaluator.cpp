#include "Evaluator.h"

std::unordered_map<long, int> Evaluator::flush_lookup;
std::unordered_map<long, int> Evaluator::unsuited_lookup;


// Helper Functions 

// Gets all combinations of poker hands
static std::vector<std::array<uint32_t, 5>> get_all_rank_sets() {
    std::vector<std::array<uint32_t, 5>> all_sets;
    
    // Get the master deck from your ValueTable
    // This deck contains the actual bit-packed uint32_t values
    auto deck = ValueTable::get_master_deck(); 

    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            for (int k = j; k < 13; ++k) {
                for (int l = k; l < 13; ++l) {
                    for (int m = l; m < 13; ++m) {
                        // We use index * 4 to always grab the 'Spades' version 
                        // of that rank from the master deck (0, 4, 8, 12... 48)
                        all_sets.push_back(std::array<uint32_t, 5>{{
                            deck[i * 4], 
                            deck[j * 4], 
                            deck[k * 4], 
                            deck[l * 4], 
                            deck[m * 4]
                        }});
                    }
                }
            }
        }
    }
    return all_sets;
}

// Helper to check if any rank appears 'target' times
static bool has_count(const std::array<int, 13>& counts, int target) {
    for (int c : counts) if (c == target) return true;
    return false;
}

// Helper to count how many pairs (exactly 2) are in the hand
static int count_pairs(const std::array<int, 13>& counts) {
    int pairs = 0;
    for (int c : counts) if (c == 2) pairs++;
    return pairs;
}

static int get_straight_high(const std::array<uint32_t, 5>& hand) {
    // OR the raw integers to combine their bitmasks (bits 16-28)
    uint32_t combined_bits = (hand[0] | hand[1] | hand[2] | hand[3] | hand[4]) >> 16;
    
    if (combined_bits == 0x100F) {
        return 3;
    } 
    
    for (int high_rank = 12; high_rank >= 4; --high_rank) {
        // This mask creates 5 consecutive '1' bits
        // e.g., if high_rank is 12 (Ace), mask is 0x1F00 (1111100000000)
        uint32_t mask = 0x1F << (high_rank - 4);
        
        if ((combined_bits & mask) == mask) {
            return high_rank;
        }
    }

    return -1; // C++ uses -1 or 0 instead of None
}

// Main Class Functions

void Evaluator::build_table() {

    if (!unsuited_lookup.empty()) return;

    std::vector<std::array<uint32_t, 5>> all_rank_sets = get_all_rank_sets();

    std::vector<std::pair<uint32_t, HandScore>> all_hands;

    for (auto hand : all_rank_sets) {
        std::array<int, 13> counts = {0};

        // Calculate hand unique prime product
        uint32_t prime_prod = 1;

        for (auto card : hand) {
            int rank = (card >> 8) & 0xF; 
            counts[rank]++;
            prime_prod *= (card & 0xFF); // Extract prime (bits 0-7)
        }

        // Skip impossible hands (5 of a kind)
        bool invalid = false;
        bool is_unique = true;
        for(int c : counts) {
            if (c > 4) { 
                invalid = true; 
                break; // Stop immediately, this hand is impossible
            }
            if (c > 1) { 
                is_unique = false; // We found a pair, trips, or quads
                // Don't break here, because we still need to check for c > 4 in other ranks
            }
        }
        if (invalid) continue;

        all_hands.push_back({prime_prod, slow_evaluator(hand, false)});

        if (is_unique) {
            // We add it to a separate list to be ranked against other flushes
            all_hands.push_back({prime_prod, slow_evaluator(hand, true)});
        }
    }

    std::sort(all_hands.begin(), all_hands.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; 
    });

    // Builds table using all hand ranks
    int current_rank = 1;
    for (size_t i = 0; i < all_hands.size(); ++i) {
        // If this hand is strictly worse than the previous one, increment the rank ID
        // (This ensures that two different King-high Straights get the SAME rank ID)
        if (i > 0 && (all_hands[i-1].second > all_hands[i].second)) {
            current_rank = i + 1;
        }

        uint32_t prime_key = all_hands[i].first;
        HandScore s = all_hands[i].second;

        // Put it in the correct map based on whether it was evaluated as a flush
        if (s.category == 9 || s.category == 6) {
            flush_lookup[prime_key] = current_rank;
        } else {
            unsuited_lookup[prime_key] = current_rank;
        }
    }
}

HandScore Evaluator::slow_evaluator(const std::array<uint32_t, 5>& hand, bool is_flush) {
    std::array<int, 13> counts = {0};
    for (uint32_t card : hand) {
        counts[(card >> 8) & 0xF]++; // Extract rank bits
    }

    // Prepare sorted_ranks (similar to your Python counts.keys() logic)
    std::vector<int> sorted_ranks;
    for (int r = 12; r >= 0; --r) {
        if (counts[r] > 0) sorted_ranks.push_back(r);
    }
    // Sort by frequency first, then rank value
    std::sort(sorted_ranks.begin(), sorted_ranks.end(), [&](int a, int b) {
        if (counts[a] != counts[b]) return counts[a] > counts[b];
        return a > b;
    });

    int straight_high = get_straight_high(hand); // Your is_straight logic
    bool is_straight = (straight_high != -1);

    // 1. Straight Flush
    if (is_straight && is_flush) return {9, {straight_high}};

    // 2. Four of a Kind
    if (has_count(counts, 4)) return {8, {sorted_ranks[0], sorted_ranks[1]}};

    // 3. Full House
    if (has_count(counts, 3) && has_count(counts, 2)) 
        return {7, {sorted_ranks[0], sorted_ranks[1]}};

    // 4. Flush
    if (is_flush) return {6, sorted_ranks};

    // 5. Straight
    if (is_straight) return {5, {straight_high}};

    // 6. Three of a Kind
    if (has_count(counts, 3)) return {4, {sorted_ranks[0], sorted_ranks[1], sorted_ranks[2]}};

    // 7. Two Pair
    if (count_pairs(counts) == 2) return {3, {sorted_ranks[0], sorted_ranks[1], sorted_ranks[2]}};

    // 8. One Pair
    if (count_pairs(counts) == 1) return {2, {sorted_ranks[0], sorted_ranks[1], sorted_ranks[2], sorted_ranks[3]}};

    // 9. High Card
    return {1, sorted_ranks};
}



std::pair<int, std::array<uint32_t, 5>> Evaluator::evaluate_player(const std::array<uint32_t, 2>& hand, const std::array<uint32_t, 5>& community) 
{
    // Combine cards on the STACK (Zero cost)
    uint32_t c[7] = { hand[0], hand[1], community[0], community[1], community[2], community[3], community[4] };

    int best_rank = 99999;
    std::array<uint32_t, 5> best_hand;

    // 21 Combinations (7 choose 5) performed in-place
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            for (int k = j + 1; k < 5; k++) {
                for (int l = k + 1; l < 6; l++) {
                    for (int m = l + 1; m < 7; m++) {
                        
                        // Current 5-card subset
                        uint32_t c1 = c[i], c2 = c[j], c3 = c[k], c4 = c[l], c5 = c[m];

                        // 1. Prime Product
                        uint32_t p = (c1 & 0xFF) * (c2 & 0xFF) * (c3 & 0xFF) * (c4 & 0xFF) * (c5 & 0xFF);

                        // 2. Flush Check
                        bool flush = (c1 & c2 & c3 & c4 & c5 & 0xF0000000) != 0;

                        // 3. O(1) Lookup
                        int rank = flush ? flush_lookup[p] : unsuited_lookup[p];

                        if (rank < best_rank) {
                            best_rank = rank;
                            best_hand = {c1, c2, c3, c4, c5};
                            if (best_rank == 1) return {1, best_hand}; // Royal Flush shortcut
                        }
                    }
                }
            }
        }
    }
    return {best_rank, best_hand};
}


// Destructor 
Evaluator::~Evaluator() {}