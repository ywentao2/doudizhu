#include "move.h"
#include "setup.h"
#include <cctype>

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
    } 
    else {
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
  }
  else if (temp.length() >= 6) {
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
      }
      else {
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
  } 
  else {
    move.type = MoveType::Invalid;
    move.used = {};
    return;
  }
}

bool wins(const Move& curr, const Move& prev) {
    if (prev.type == MoveType::Pass || prev.type == MoveType::Invalid) return true;
    if (curr.type == MoveType::Invalid) return false;
    if (curr.type == MoveType::Rocket) return true;
    if (prev.type == MoveType::Rocket) return false;
        if (curr.type == MoveType::Bomb) {
        if (prev.type == MoveType::Bomb) return curr.primary_rank > prev.primary_rank;
        return true;
    }
    if (prev.type == MoveType::Bomb) return false;

    if (curr.type != prev.type) return false;

    if (curr.type == MoveType::Straight) {
        return curr.count == prev.count && curr.primary_rank > prev.primary_rank;
    }

    if (curr.type == MoveType::Airplane || curr.type == MoveType::Double_Three_Carry_Two) {
        return curr.count == prev.count
            && curr.length == prev.length
            && curr.primary_rank > prev.primary_rank;
    }

    return curr.primary_rank > prev.primary_rank;
}