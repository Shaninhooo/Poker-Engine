#include "../engine/Engine.h"

// The container for raw data
struct PokerState {
    std::array<uint32_t, 2> hero_hand;
    std::array<uint32_t, 5> board;
    int known_board_count;

    int players = 2;
    int pot;
    std::string raw_history;
};

// The state that the AI actually uses
// class GameState {
// private:
//     const PokerState& physical_state; // Reference to the raw data
//     int bucket;                       // Pre-calculated by Engine

// public:
//     GameState(const PokerState& ps, Engine& e) 
//         : physical_state(ps) {
//         // Translate raw cards into an AI bucket immediately
//         bucket = e.get_bucket(ps.hero_hand, ps.board);
//     }

//     // This creates the Key the CFR needs
//     std::string get_infoset_key() const {
//         return std::to_string(bucket) + ":" + physical_state.raw_history;
//     }
// };