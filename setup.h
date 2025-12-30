#pragma once

#include <algorithm>
#include <string>
#include <iostream>
#include <random>
#include <vector>

struct player {
    std::string hand;
};

class Setup {
public:
    Setup(int decks);
    void set_decks(int decks);
    int get_decks() const { return decks_; }
    std::vector<char> get_deck() const { return deck_; }
    static bool card_order(char a, char b); //static so it can be used without an instance/in comparison functions
private:
    int decks_ {0};
    std::vector<char> deck_;
};