#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <random>
#include <vector>
#include <array>
#include <cstdint>

inline constexpr std::array<char, 15> IDX_TO_CARD = {
    '3','4','5','6','7','8','9','T','J','Q','K','A','2','L','B'
};

inline int index(char c) {
    for (int i = 0; i < 15; ++i) {
        if (IDX_TO_CARD[i] == c) return i;
    }
    return -1;
}

inline char rank(int idx) {
    if (idx < 0 || idx >= 15) return '0';
    return IDX_TO_CARD[idx];
}

struct player {
    std::string hand;
    bool is_landlord {false};
    std::array<uint8_t, 15> hand_map; //enables random access to counts of each card rank
};

class Setup {
public:
    Setup(int decks);
    void set_decks(uint8_t decks);
    uint8_t get_decks() const { return decks_; }
    std::vector<char> get_deck() const { return deck_; }
    static bool card_order(char a, char b); //static so it can be used without an instance/in comparison functions
private:
    uint8_t decks_ {0};
    std::vector<char> deck_;
};