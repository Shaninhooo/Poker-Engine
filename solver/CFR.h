#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

// Node
struct InfoSet {
    std::string key;
    std::vector<double> regret_sum;
    std::vector<double> strategy_sum;
    std::vector<double> current_strategy;

    InfoSet(int num_actions) {
        regret_sum.resize(num_actions, 0.0);
        strategy_sum.resize(num_actions, 0.0);
        current_strategy.resize(num_actions, 1.0 / num_actions);
    }
};


// Key
// STAGE : POSITION : BUCKET : ACTION_HISTORY

// STAGE: Pre-Flop, Flop, Turn, River
// POSITION: OOP (1st), IP (2nd)
// BUCKET: 0-9
// ACTION_HISTORY: C CHECK/CALL, B BET/RAISE, F FOLD

class CFR {
    private:
        std::unordered_map<std::string, InfoSet> gametree;

    public:

        void train_iteration();

        double walker(std::string history, double p1_reach, double p2_reach);
};