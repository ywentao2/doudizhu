#include "state.h"
#include "move.h"

std::vector<Move> state::legal_moves() const {
    const auto &p = players[turn];
    return filter_moves(::generate_move(p), prev);
}

void state::step(const Move &move) {
  if (move.type == MoveType::Pass) {
    ++passes;
    if (passes >= (int)players.size() - 1) {
      reset();
    }
  } else {
    ::apply_move(players[turn].hand_map, move.used);
    prev = move;
    passes = 0;
    last_player = turn;
  }
  turn = (turn + 1) % players.size();
}