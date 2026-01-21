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

  score += hm[index('2')] * 6;
  score += hm[index('L')] * 12;
  score += hm[index('B')] * 15;

  if (hm[index('L')] >= 1 && hm[index('B')] >= 1) score += 50;

  for (int r = 0; r < 15; ++r) {
    if (hm[r] == 4) score += 30;     
    if (hm[r] == 3) score += 10;     
    if (hm[r] == 2) score += 4;      
  }

  const int max_rank = index('A');
  for (int s = 0; s <= max_rank;) {
    if (hm[s] == 0) { ++s; continue; }
    int e = s;
    while (e <= max_rank && hm[e] >= 1) ++e;
    int len = e - s;
    if (len >= 5) score += 2 * len;  
    s = e;
  }

  return score;
}

static int choose_landlord(const std::vector<player>& players,
                                   const std::array<uint8_t, 15>& kitty_map) {
  int best = 0;
  int bestDelta = -1e9;
  int bestFinal = -1e9;

  for (int i = 0; i < (int)players.size(); ++i) {
    int before = strength_score(players[i].hand_map);

    std::array<uint8_t, 15> afterMap = players[i].hand_map;
    for (int r = 0; r < 15; ++r) afterMap[r] += kitty_map[r];

    int after = strength_score(afterMap);

    int delta = after - before;

    if (delta > bestDelta || (delta == bestDelta && after > bestFinal)) {
      bestDelta = delta;
      bestFinal = after;
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
