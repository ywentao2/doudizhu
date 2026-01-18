#include "move.h"
#include "setup.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

int check_winner(const std::vector<player> &players) {
  for (int i = 0; i < (int)players.size(); ++i) {
    bool empty = true;
    for (uint8_t x : players[i].hand_map) {
      if (x != 0) {
        empty = false;
        break;
      }
    }
    if (empty)
      return i + 1;
  }
  return -1;
}

void print_hand(const std::array<uint8_t, 15> &hand_map) {
  std::string hand;
  for (int i = 0; i < 15; ++i) {
    hand.append((size_t)hand_map[i], rank(i));
  }
  std::cout << "Hand: " << hand << std::endl;
}

std::string to_string(MoveType t) {
  switch (t) {
  case MoveType::Single:
    return "Single";
  case MoveType::Pair:
    return "Pair";
  case MoveType::Triple:
    return "Triple";
  case MoveType::Bomb:
    return "Bomb";
  case MoveType::Rocket:
    return "Rocket";
  case MoveType::Three_Carry_One:
    return "Three Carry One";
  case MoveType::Three_Carry_Two:
    return "Three Carry Two";
  case MoveType::Straight:
    return "Straight";
  case MoveType::Airplane:
    return "Airplane";
  case MoveType::Invalid:
    return "Invalid";
  case MoveType::Double_Three_Carry_One:
    return "Double Three Carry One";
  case MoveType::Double_Three_Carry_Two:
    return "Double Three Carry Two";
  case MoveType::Pass:
    return "Pass";
  default:
    return "Unknown";
  }
}

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
    std::fill(p.hand_map.begin(), p.hand_map.end(), 0);
    for (char c : p.hand) {
      int idx = index(c);
      if (idx != -1) {
        p.hand_map[idx]++;
      }
    }
    int i = &p - &players[0] + 1; // pointer arithmetic to get player number
    std::sort(p.hand.begin(), p.hand.end(),
              Setup::card_order); // class member function called with class
    std::cout << "Player " << i << " hand: " << p.hand << std::endl;
    std::cout << std::endl;
  }
  Move prev;
  int passes {0};
  while (check_winner(players) == -1) {
    for (auto &p : players) {
      int pturn = &p - &players[0] + 1;

      while (true) { // ✅ retry loop for THIS player
        std::cout << "Player " << pturn << "'s turn. Enter your move: ";
        std::string move_str;
        std::cin >> move_str;

        Move move{};
        process_move(move_str, move, p);

        if (move.type == MoveType::Invalid) {
          std::cout << "Invalid move, try again.\n";
          continue; // ✅ same player retries
        }

        if (move.type == MoveType::Pass) {
          // only allow pass if there's an active prev move
          if (prev.type == MoveType::Invalid || prev.type == MoveType::Pass) {
            std::cout << "You can't pass on a fresh trick.\n";
            continue;
          }
          std::cout << "Player " << pturn << " passes.\n";
          passes++;
          if (passes >= (int)players.size() - 1) {
            prev = Move{};
            passes = 0;
          }
          break; // ✅ ends THIS player turn only
        }

        // if move doesn't beat prev, undo and retry
        if (!wins(move, prev)) {
          std::cout << "Doesn't beat previous move, try again.\n";
          undo(p.hand_map, move.used); // because process_move already applied
          continue;
        }

        std::cout << "Player " << pturn << " played: " << move_str << " ("
                  << to_string(move.type) << ")\n";

        print_hand(p.hand_map);

        prev = move; // update table move
        break;       // ✅ successful play ends turn
      }

      if (check_winner(players) != -1)
        break;
    }
  }

  std::cout << "Player " << check_winner(players) << " wins!\n";
  return 0;
}