#include "sim.h"
#include "setup.h"
#include "mcts.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>

static void build_hand_map(player &p) {
  std::fill(p.hand_map.begin(), p.hand_map.end(), 0);
  for (char c : p.hand) {
    int idx = index(c);
    if (idx != -1) p.hand_map[idx]++;
  }
}

static int strength_score(const std::array<uint8_t, 15>& hm) {
  int score = 0;

  // Control cards (high power)
  score += hm[index('2')] * 20;
  score += hm[index('A')] * 15;
  score += hm[index('K')] * 12;
  
  // Jokers are extremely valuable
  score += hm[index('L')] * 25;
  score += hm[index('B')] * 30;
  
  // Rocket is game-winning
  if (hm[index('L')] >= 1 && hm[index('B')] >= 1) {
    score += 100;
  }

  // Bombs are very strong
  for (int r = 0; r < 14; ++r) {
    if (hm[r] == 4) score += 80;
  }
  
  // Triples are good building blocks
  for (int r = 0; r < 14; ++r) {
    if (hm[r] == 3) score += 25;
  }
  
  // Pairs are useful
  for (int r = 0; r < 14; ++r) {
    if (hm[r] == 2) score += 10;
  }
  
  // Count singles (actually a weakness if too many odd cards)
  int singles = 0;
  for (int r = 0; r < 13; ++r) {
    if (hm[r] == 1) singles++;
  }
  score -= singles * 3;  // Penalize unmatched cards

  // Straight potential (consecutive cards)
  const int max_rank = index('A');
  int consecutive = 0;
  for (int r = 0; r <= max_rank; ++r) {
    if (hm[r] >= 1) {
      consecutive++;
      if (consecutive >= 5) score += 15;
    } else {
      consecutive = 0;
    }
  }

  // Triple sequences (airplane potential)
  consecutive = 0;
  for (int r = 0; r <= max_rank; ++r) {
    if (hm[r] >= 3) {
      consecutive++;
      if (consecutive >= 2) score += 30;
    } else {
      consecutive = 0;
    }
  }

  return score;
}

static int choose_landlord(const std::vector<player>& players,
                           const std::array<uint8_t, 15>& kitty_map) {
  int best = 0;
  int best_final_score = -1;

  for (int i = 0; i < (int)players.size(); ++i) {
    // Calculate what this player's hand would be WITH the kitty
    std::array<uint8_t, 15> final_hand = players[i].hand_map;
    for (int r = 0; r < 15; ++r) {
      final_hand[r] += kitty_map[r];
    }
    
    int final_score = strength_score(final_hand);
    
    if (final_score > best_final_score) {
      best_final_score = final_score;
      best = i;
    }
  }

  return best;
}

static state make_start_state(int decks) {
  Setup setup{decks};
  std::vector<player> players((setup.get_decks() == 1 ? 3 : 4));
  auto deckvec = setup.get_deck();

  while (!deckvec.empty()) {
    for (auto &p : players) {
      p.hand += deckvec.back();
      deckvec.pop_back();
    }
  }

  for (auto &p : players) build_hand_map(p);

  int landlord = choose_landlord(players, setup.get_kitty());

  for (auto &p : players) p.is_landlord = false;
  players[landlord].is_landlord = true;

  auto kitty_map = setup.get_kitty();
  for (int r = 0; r < 15; ++r)
    players[landlord].hand_map[r] += kitty_map[r];

  std::rotate(players.begin(), players.begin() + landlord, players.end());

  state s;
  s.players = players;
  s.turn = 0;
  s.prev = Move{};
  s.passes = 0;
  s.last_player = -1;
  return s;
}

simulation_result run(int num_games, int decks, int mcts_iters_per_move) {
  simulation_result stats {};
  stats.games = num_games;
  for (int g = 0; g < num_games; ++g) {
    state s = make_start_state(decks);

    while (!s.end()) {
      int team = s.players[s.turn].is_landlord ? 1 : 0;
      MCTS mcts(s, team);
      Move best = mcts.search(mcts_iters_per_move);
      s.step(best);
    }
    std::cout << "Game " << g + 1 << " completed." << std::endl;
    int w = s.winner();
    if (w == 1) stats.landlord_wins++;
    else if (w == 0) stats.peasant_wins++;
  }

  return stats;
}
