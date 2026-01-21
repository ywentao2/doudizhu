#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include "setup.h"

Setup::Setup(int decks) : decks_(decks) {
    kitty.fill(0);
    std::vector<char> temp;
    temp.reserve(static_cast<size_t>(decks_ * 54)); //reserve takes size_t, cast to avoid warnings
    std::string ranks = "3456789TJQKA2";
    for (char c : ranks) {
        temp.insert(temp.end(), 4 * decks_, c);
    }
    temp.insert(temp.end(), decks_, 'L');
    temp.insert(temp.end(), decks_, 'B');
    std::shuffle(temp.begin(), temp.end(), std::mt19937{std::random_device{}()});
    std::vector<char> tempk;
    deck_.assign(temp.begin(), temp.end() - decks_ * 3);
    tempk.assign(temp.end() - decks_ * 3, temp.end());
    for (char c : tempk) {
        int r = index(c);
        if (r != -1) {
            kitty[r]++;
        }
    }
}

void Setup::set_decks(uint8_t decks) {
    decks_ = decks;
}

bool Setup::card_order(char a, char b) { //does not need static
    std::string ranks = "3456789TJQKA2LB";
    return ranks.find(a) < ranks.find(b);
}
