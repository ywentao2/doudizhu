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
    if (empty) {
      if (players[i].is_landlord) {
        return 1; // Landlord wins
      } else {
        return 0; // Peasant wins
      }
    }
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
  while (!(std::cin >> t)) {
    std::cout << "Enter a valid deck size.\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  if (t > 2 || t < 1) {
    std::cout << "Can only play with one or two decks, set to 1" << std::endl;
    t = 1;
  }
restart:
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
  int highest_bet{0};
  int landlord{-1};
  bool instant_landlord = false;

  for (auto &p : players) {
    int bet = 0;
    int i = &p - &players[0] + 1;

    while (true) {
      std::cout << "Player " << i << ", enter your bet (0-3): ";

      while (!(std::cin >> bet)) {
        std::cout << "Invalid/malformed bet size, try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      }

      if (bet < 0 || bet > 3) {
        std::cout << "Please enter a bet size between 0 and 3.\n";
        continue;
      }

      if (bet <= highest_bet && bet != 0) {
        std::cout << "Bet must be higher than current highest bet of "
                  << highest_bet << ", try again.\n";
        continue;
      }

      p.bet = bet;

      if (bet > highest_bet) {
        highest_bet = bet;
        landlord = i - 1;
      }

      if (bet == 3) {
        landlord = i - 1;
        highest_bet = 3;
        instant_landlord = true;
      }

      break;
    }

    if (instant_landlord)
      break;
  }

  if (landlord == -1) {
    std::cout << "No landlord determined, restarting game.\n";
    goto restart;
  }

  for (auto &p : players)
    p.is_landlord = false;
  players[landlord].is_landlord = true;

  std::cout << "Player " << (landlord + 1) << " is the landlord!\n";

  std::rotate(players.begin(), players.begin() + landlord, players.end());
  Move prev;
  int passes{0};
  while (check_winner(players) == -1) {
    for (auto &p : players) {
      int pturn = &p - &players[0];
      std::string who =
          p.is_landlord ? "Landlord" : "Peasant " + std::to_string(pturn);

      while (true) {
        std::cout
            << who
            << "'s turn. Enter your move/use '.help' to view more options: ";
        std::string move_str;
        std::cin >> move_str;
        if (move_str == ".help") {
          std::cout << "Commands:\n";
          std::cout << "  .hand - View your hand\n";
          std::cout << "  .help - View this help message\n";
          std::cout << "  .moves - View possible moves\n";
          continue;
        }
        if (move_str == ".hand") {
          print_hand(p.hand_map);
          continue;
        }
        if (move_str == ".moves") {
          std::vector<Move> moves = filter_moves(generate_move(p), prev);
          if (moves.empty()) {
            std::cout << "No possible moves.\n";
            std::cout << who << " passes.\n";
            passes++;
            if (passes >= (int)players.size() - 1) {
              std::cout << "All other players passed.\n";
              prev = Move{};
              passes = 0;
            }
            break;
          } else {
            std::cout << "Possible moves:\n";
            for (const auto &m : moves) {
              std::string move_repr;
              for (int i = 0; i < 15; ++i) {
                move_repr.append((size_t)m.used[i], rank(i));
              }
              std::cout << "  " << move_repr << " (" << to_string(m.type)
                        << ")\n";
            }
          }
          continue;
        }
        Move move{};
        process_move(move_str, move, p);

        if (move.type == MoveType::Invalid) {
          std::cout << "Invalid move, try again.\n";
          continue;
        }

        if (move.type == MoveType::Pass) {
          if (prev.type == MoveType::Invalid || prev.type == MoveType::Pass) {
            std::cout << "You can't pass here.\n";
            continue;
          }
          std::cout << who << " passes.\n";
          passes++;
          if (passes >= (int)players.size() - 1) {
            std::cout << "All other players passed.\n";
            prev = Move{};
            passes = 0;
          }
          break;
        }

        bool prevActive =
            (prev.type != MoveType::Invalid && prev.type != MoveType::Pass);
        if (prevActive && move.type != prev.type &&
            move.type != MoveType::Bomb && move.type != MoveType::Rocket) {
          std::cout << "You can't play that.\n";
          undo(p.hand_map, move.used);
          continue;
        }

        if (!wins(move, prev)) {
          std::cout << "Doesn't beat previous move.\n";
          undo(p.hand_map, move.used);
          continue;
        }

        std::cout << who << " played: " << move_str << " ("
                  << to_string(move.type) << ")\n";

        print_hand(p.hand_map);

        prev = move;
        passes = 0;
        break;
      }

      if (check_winner(players) != -1)
        break;
    }
  }

  if (check_winner(players) == 1) {
    std::cout << "Landlord wins!\n";
  } else {
    std::cout << "Peasants win!\n";
  }
  return 0;
}