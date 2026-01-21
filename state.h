#pragma once
#include "move.h"
#include "setup.h"
#include <vector>

struct state {
    std::vector<player> players;
    int passes {0};
    Move prev {};
    int last_player {-1}; 
    int turn {-1};

    int curr () const { return turn; }

    inline bool end() const {
        for (const auto &p : players) if (p.empty()) return true;
        return false;
    }

    inline int winner() const {
        for (const auto &p : players) 
            if (p.empty()) 
                return p.is_landlord ? 1 : 0;
        return -1; 
    }

    inline void reset() {
        prev = Move{};
        passes = 0;
        last_player = -1;
    }
    
    inline void apply_move(const Move& move) {
        ::apply_move(players[turn].hand_map, move.used);
    }

    std::vector<Move> legal_moves() const;
    void step(const Move& move);
};