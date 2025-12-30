#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include "setup.h"

Setup::Setup(int decks) : decks_(decks) {
    deck_.reserve(static_cast<size_t>(decks_ * 54)); //reserve takes size_t, cast to avoid warnings
    std::string ranks = "3456789TJQKA2";
    for (char c : ranks) {
        deck_.insert(deck_.end(), 4 * decks_, c);
    }
    deck_.insert(deck_.end(), decks_, 'L');
    deck_.insert(deck_.end(), decks_, 'B');
    std::shuffle(deck_.begin(), deck_.end(), std::mt19937{std::random_device{}()});
}

void Setup::set_decks(int decks) {
    decks_ = decks;
}

bool Setup::card_order(char a, char b) { //does not need static
    std::string ranks = "3456789TJQKA2LB";
    return ranks.find(a) < ranks.find(b);
}
