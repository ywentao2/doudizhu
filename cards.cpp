#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include "setup.h"


int main() {
    int t = 0;
    std::cout << "One or two decks?" << std::endl;
    std::cin >> t;
    if (t > 2 || t < 1) {
        std::cout << "Can only play with one or two decks, set to 1" << std::endl;
        t = 1;
    } 
    Setup setup{t};
    std::vector<player> players((setup.get_decks() == 1 ? 3 : 4));
    std::vector<char> deck = setup.get_deck();
    while (!deck.empty()) {
        for (auto &p : players) {
            p.hand += deck.back();
            deck.pop_back();
        }
    }
    for (auto &p : players) {
        int i = &p - &players[0] + 1; //pointer arithmetic to get player number
        std::sort(p.hand.begin(), p.hand.end(), Setup::card_order); //class member function called with class name since static
        std::cout << "Player " << i << " hand: " << p.hand << std::endl;
    }
    return 0;
}