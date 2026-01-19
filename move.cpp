#include "move.h"
#include "setup.h"
#include <cctype>
#include <string>

bool valid_move(const std::array<uint8_t, 15> &hand_map,
                const std::array<uint8_t, 15> &used) {
  for (size_t i = 0; i < hand_map.size(); ++i)
    if (used[i] > hand_map[i])
      return false;
  return true;
}

void apply_move(std::array<uint8_t, 15> &hand_map,
                const std::array<uint8_t, 15> &used) {
  for (size_t i = 0; i < hand_map.size(); ++i)
    hand_map[i] -= used[i];
}

void undo(std::array<uint8_t, 15> &hand_map,
          const std::array<uint8_t, 15> &used) {
  for (size_t i = 0; i < hand_map.size(); ++i)
    hand_map[i] += used[i];
}

void process_move(const std::string &move_str, Move &move, player &p) {
  std::string temp = move_str;
  for (auto &c : temp)
    if (std::islower(c))
      c = std::toupper(c);
  move.used = {};
  if (temp == "PASS") {
    move.type = MoveType::Pass;
    return;
  }
  sort(temp.begin(), temp.end(), Setup::card_order);
  if (temp.length() == 1 && (index(temp[0]) != -1)) {
    move.type = MoveType::Single;
    move.used[index(temp[0])] = 1;
    if (valid_move(p.hand_map, move.used)) {
      apply_move(p.hand_map, move.used);
      move.length = 1;
      move.primary_rank = index(temp[0]);
    } else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else if (temp.length() == 2 && temp[0] == temp[1]) {
    move.type = MoveType::Pair;
    move.used[index(temp[0])] = 2;
    if (valid_move(p.hand_map, move.used)) {
      apply_move(p.hand_map, move.used);
      move.length = 2;
      move.primary_rank = index(temp[0]);
    } else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else if (temp.length() == 3 && temp[0] == temp[1] && temp[1] == temp[2]) {
    move.type = MoveType::Triple;
    move.used[index(temp[0])] = 3;
    if (valid_move(p.hand_map, move.used)) {
      apply_move(p.hand_map, move.used);
      move.length = 3;
      move.primary_rank = index(temp[0]);
    } else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else if (temp.length() == 4 && temp[0] == temp[1] && temp[1] == temp[2] &&
             temp[2] == temp[3]) {
    move.type = MoveType::Bomb;
    move.used[index(temp[0])] = 4;
    if (valid_move(p.hand_map, move.used)) {
      apply_move(p.hand_map, move.used);
      move.length = 4;
      move.primary_rank = index(temp[0]);
    } else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else if (temp == "LB" || temp == "BL") {
    move.type = MoveType::Rocket;
    move.used[index('L')] = 1;
    move.used[index('B')] = 1;
    if (valid_move(p.hand_map, move.used)) {
      apply_move(p.hand_map, move.used);
      move.length = 2;
      move.primary_rank = index('B');
    } else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else if (temp.length() == 4 &&
             ((temp[0] == temp[1] && temp[1] == temp[2]) ||
              (temp[1] == temp[2] && temp[2] == temp[3]))) {
    move.type = MoveType::Three_Carry_One;
    if (temp[0] == temp[1] && temp[1] == temp[2]) {
      move.used[index(temp[0])] = 3;
      move.used[index(temp[3])] = 1;
      move.length = 4;
      move.primary_rank = index(temp[0]);
    } else {
      move.used[index(temp[1])] = 3;
      move.used[index(temp[0])] = 1;
      move.length = 4;
      move.primary_rank = index(temp[1]);
    }
    if (valid_move(p.hand_map, move.used))
      apply_move(p.hand_map, move.used);
    else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else if (temp.length() == 5 && ((temp[0] == temp[1] && temp[1] == temp[2] &&
                                     temp[3] == temp[4]) ||
                                    (temp[0] == temp[1] && temp[2] == temp[3] &&
                                     temp[3] == temp[4]))) {
    move.type = MoveType::Three_Carry_Two;
    if (temp[0] == temp[1] && temp[1] == temp[2]) {
      move.used[index(temp[0])] = 3;
      move.used[index(temp[3])] = 2;
      move.length = 5;
      move.primary_rank = index(temp[0]);
    } else {
      move.used[index(temp[2])] = 3;
      move.used[index(temp[0])] = 2;
      move.length = 5;
      move.primary_rank = index(temp[2]);
    }
    if (valid_move(p.hand_map, move.used))
      apply_move(p.hand_map, move.used);
    else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else if (temp.length() == 8 && (temp[0] == temp[1] && temp[1] == temp[2]) &&
             (temp[3] == temp[4] && temp[4] == temp[5])) {
    move.type = MoveType::Double_Three_Carry_One;
    move.used[index(temp[0])] = 3;
    move.used[index(temp[3])] = 3;
    move.length = 8;
    move.primary_rank = index(temp[3]);
    if (valid_move(p.hand_map, move.used))
      apply_move(p.hand_map, move.used);
    else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else if (temp.length() >= 6) {
    std::array<uint8_t, 15> counts{};
    for (char c : temp) {
      int r = index(c);
      if (r == -1) {
        move = {};
        move.type = MoveType::Invalid;
        return;
      }
      counts[r]++;
    }
    for (int i : counts) {
      if (i == 4) {
        move = {};
        move.type = MoveType::Invalid;
        return;
      }
    }
    std::vector<int> triples;
    for (size_t i = 0; i < 15; ++i) {
      if (counts[i] == 3) {
        if (i >= index('2')) {
          move = {};
          move.type = MoveType::Invalid;
          return;
        }
        triples.push_back(i);
      }
    }
    int k = triples.size();
    if (k < 2) {
      move = {};
      move.type = MoveType::Invalid;
      return;
    }
    std::sort(triples.begin(), triples.end());
    for (size_t i = 1; i < triples.size(); ++i) {
      if (triples[i] != triples[i - 1] + 1) {
        move = {};
        move.type = MoveType::Invalid;
        return;
      }
    }
    int remaining = temp.length() - k * 3;
    if (remaining == 0) {
      move.type = MoveType::Airplane;
      for (int r : triples) {
        move.used[r] = 3;
      }
      move.length = temp.length();
      move.count = k;
      move.primary_rank = index(temp.back());
      if (valid_move(p.hand_map, move.used))
        apply_move(p.hand_map, move.used);
      else {
        move = {};
        move.type = MoveType::Invalid;
      }
      return;
    }
    if (remaining == k) {
      int singles = 0;
      for (int r = 0; r < 15; ++r) {
        if (counts[r] == 1)
          singles++;
        else if (counts[r] == 2) {
          move = {};
          move.type = MoveType::Invalid;
          return;
        }
      }
      if (singles == k) {
        move.type = MoveType::Airplane;
        for (int r : triples) {
          move.used[r] = 3;
        }
        move.length = temp.length();
        move.count = k;
        move.primary_rank = index(temp.back());
        for (int r = 0; r < 15; ++r) {
          if (counts[r] == 1)
            move.used[r] = 1;
        }
        if (valid_move(p.hand_map, move.used))
          apply_move(p.hand_map, move.used);
        else {
          move = {};
          move.type = MoveType::Invalid;
        }
      }
      return;
    }
    if (remaining == 2 * k) {
      int pairs = 0;

      for (int r = 0; r < 15; ++r) {
        if (counts[r] == 2)
          pairs++;
        else if (counts[r] == 1) {
          move = {};
          move.type = MoveType::Invalid;
          return;
        }
      }

      if (pairs != k) {
        move = {};
        move.type = MoveType::Invalid;
        return;
      }

      move.used = {};
      for (int r : triples)
        move.used[r] = 3;
      for (int r = 0; r < 15; ++r)
        if (counts[r] == 2)
          move.used[r] = 2;

      if ((int)temp.length() == 10 && k == 2) {
        move.type = MoveType::Double_Three_Carry_Two;
        move.length = temp.length();
        move.count = k;
        move.primary_rank = index(temp.back());
      } else {
        move.type = MoveType::Airplane;
        move.length = temp.length();
        move.count = k;
        move.primary_rank = index(temp.back());
      }

      if (!valid_move(p.hand_map, move.used)) {
        move = {};
        move.type = MoveType::Invalid;
        return;
      }

      apply_move(p.hand_map, move.used);
      return;
    }
  }

  else if (temp.length() >= 5) {
    for (size_t i = 0; i < temp.length() - 1; ++i) {
      int r = index(temp[i]);
      if (index(temp[i + 1]) != index(temp[i]) + 1 || r >= index('2')) {
        move = {};
        move.type = MoveType::Invalid;
        return;
      }
      move.used[index(temp[i])] = 1;
    }
    move.used[index(temp.back())] = 1;
    move.type = MoveType::Straight;
    move.length = temp.length();
    move.primary_rank = index(temp.back());

    if (valid_move(p.hand_map, move.used))
      apply_move(p.hand_map, move.used);
    else {
      move = {};
      move.type = MoveType::Invalid;
    }
    return;
  } else {
    move.type = MoveType::Invalid;
    move.used = {};
    return;
  }
}

bool wins(const Move &curr, const Move &prev) {
  if (prev.type == MoveType::Pass || prev.type == MoveType::Invalid)
    return true;
  if (curr.type == MoveType::Invalid)
    return false;
  if (curr.type == MoveType::Rocket)
    return true;
  if (prev.type == MoveType::Rocket)
    return false;
  if (curr.type == MoveType::Bomb) {
    if (prev.type == MoveType::Bomb)
      return curr.primary_rank > prev.primary_rank;
    return true;
  }
  if (prev.type == MoveType::Bomb)
    return false;

  if (curr.type != prev.type)
    return false;

  if (curr.type == MoveType::Straight) {
    return curr.count == prev.count && curr.primary_rank > prev.primary_rank;
  }

  if (curr.type == MoveType::Airplane ||
      curr.type == MoveType::Double_Three_Carry_Two) {
    return curr.count == prev.count && curr.length == prev.length &&
           curr.primary_rank > prev.primary_rank;
  }

  return curr.primary_rank > prev.primary_rank;
}
// generates all valid moves for player
std::vector<Move> generate_move(const player &p) {
  std::vector<Move> moves;
  auto push = [&](MoveType type, const std::vector<int> &tripRanks,
                  const std::vector<int> &singleRanks,
                  const std::vector<int> &pairRanks,
                  uint8_t primaryRankOverride = 255) {
    Move m{};
    m.type = type;

    for (int r : tripRanks)
      m.used[r] += 3;
    for (int r : singleRanks)
      m.used[r] += 1;
    for (int r : pairRanks)
      m.used[r] += 2;
    uint8_t len = 0;
    for (int i = 0; i < 15; ++i)
      len += m.used[i];
    m.length = len;
    if (type == MoveType::Straight)
      m.count = (uint8_t)singleRanks.size();
    else
      m.count = (uint8_t)tripRanks.size();
    if (primaryRankOverride != 255) {
      m.primary_rank = primaryRankOverride;
    } else {
      if (!tripRanks.empty())
        m.primary_rank = tripRanks.back();
      else if (!singleRanks.empty())
        m.primary_rank = singleRanks.back();
      else
        m.primary_rank = 0;
    }

    moves.push_back(m);
  };
  for (int i = 0; i < 15; ++i) {
    if (p.hand_map[i] >= 1) {
      Move temp = {};
      temp.type = MoveType::Single;
      temp.used[i] = 1;
      temp.length = 1;
      temp.primary_rank = i;
      moves.push_back(temp);
    }
    if (p.hand_map[i] >= 2) {
      Move temp = {};
      temp.type = MoveType::Pair;
      temp.used[i] = 2;
      temp.length = 2;
      temp.primary_rank = i;
      moves.push_back(temp);
    }
    if (p.hand_map[i] >= 3) {
      Move temp = {};
      temp.type = MoveType::Triple;
      temp.used[i] = 3;
      temp.length = 3;
      temp.primary_rank = i;
      moves.push_back(temp);
    }
    if (p.hand_map[i] == 4) {
      Move temp = {};
      temp.type = MoveType::Bomb;
      temp.used[i] = 4;
      temp.length = 4;
      temp.primary_rank = i;
      moves.push_back(temp);
    }
    for (int j = 0; j < 15; ++j) {
      if (i != j && p.hand_map[i] >= 3 && p.hand_map[j] >= 1) {
        Move temp = {};
        temp.type = MoveType::Three_Carry_One;
        temp.used[i] = 3;
        temp.used[j] = 1;
        temp.length = 4;
        temp.primary_rank = i;
        moves.push_back(temp);
      }
      if (i != j && p.hand_map[i] >= 3 && p.hand_map[j] >= 2) {
        Move temp = {};
        temp.type = MoveType::Three_Carry_Two;
        temp.used[i] = 3;
        temp.used[j] = 2;
        temp.length = 5;
        temp.primary_rank = i;
        moves.push_back(temp);
      }
    }
  }
  const int max_rank = index('A');
  for (int start = 0; start <= max_rank;) {
    if (p.hand_map[start] == 0) {
      ++start;
      continue;
    }
    int end = start;
    while (end <= max_rank && p.hand_map[end] >= 1)
      ++end;
    --end;
    int len = end - start + 1;
    if (len >= 5) {
      for (int l = 5; l <= len; ++l) {
        for (int s = start; s <= end - l + 1; ++s) {
          std::vector<int> seq;
          seq.reserve(len);
          for (int k = 0; k < l; ++k)
            seq.push_back(s + k);
          push(MoveType::Straight, {}, seq, {});
        }
      }
    }
    start = end + 1;
  }
  for (int start = 0; start <= max_rank;) {
    if (p.hand_map[start] < 3) {
      ++start;
      continue;
    }
    int end = start;
    while (end <= max_rank && p.hand_map[end] >= 3)
      ++end;
    --end;
    int len = end - start + 1;
    if (len >= 2) {
      for (int s = start; s <= end; ++s) {
        for (int l = 2; s + l - 1 <= end; ++l) {
          std::vector<int> tripSeq;
          tripSeq.reserve(l);
          for (int k = s; k < s + l; ++k)
            tripSeq.push_back(k);
          push(MoveType::Airplane, tripSeq, {}, {});
          std::vector<int> singleSeq;
          std::vector<int> pairSeq;
          singleSeq.reserve(15);
          pairSeq.reserve(15);
          for (int r = 0; r < 15; ++r) {
            bool isTrip = (r >= s && r < s + l);
            if (isTrip)
              continue;
            if (p.hand_map[r] >= 1)
              singleSeq.push_back(r);
            if (p.hand_map[r] >= 2)
              pairSeq.push_back(r);
          }
          if ((int)singleSeq.size() >= l) {
            std::vector<int> chosen;
            chosen.reserve(l);
            auto backtrack = [&](auto &&self, int index, int need) -> void {
              if (need == 0) {
                MoveType type = (l == 2) ? MoveType::Double_Three_Carry_One
                                         : MoveType::Airplane;
                push(type, tripSeq, chosen, {});
                return;
              }
              if (index >= (int)singleSeq.size())
                return;
              chosen.push_back(singleSeq[index]);
              self(self, index + 1, need - 1);
              chosen.pop_back();
              self(self, index + 1, need);
            };
            backtrack(backtrack, 0, l);
          }
          if ((int)pairSeq.size() >= l) {
            std::vector<int> chosenPairs;
            chosenPairs.reserve(l);
            auto backtrackPairs = [&](auto &&self, int index,
                                      int need) -> void {
              if (need == 0) {
                MoveType type = (l == 2) ? MoveType::Double_Three_Carry_Two
                                         : MoveType::Airplane;
                push(type, tripSeq, {}, chosenPairs);
                return;
              }
              if (index >= (int)pairSeq.size())
                return;
              chosenPairs.push_back(pairSeq[index]);
              self(self, index + 1, need - 1);
              chosenPairs.pop_back();
              self(self, index + 1, need);
            };
            backtrackPairs(backtrackPairs, 0, l);
          }
        }
      }
    }
    start = end + 1;
  }
  if (p.hand_map[index('L')] >= 1 && p.hand_map[index('B')] >= 1) {
    Move temp = {};
    temp.type = MoveType::Rocket;
    temp.used[index('L')] = 1;
    temp.used[index('B')] = 1;
    temp.length = 2;
    temp.primary_rank = index('B');
    moves.push_back(temp);
  }
  return moves;
}

std::vector<Move> filter_moves(const std::vector<Move> &moves,
                               const Move &prev) {
  std::vector<Move> filtered;
  for (const auto &m : moves) {
    if (wins(m, prev))
      filtered.push_back(m);
  }
  return filtered;
}