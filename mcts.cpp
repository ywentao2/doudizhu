#include "mcts.h"
#include <cmath>
#include <utility>

MCTS::MCTS(state root_state, int root_team)
    : root_(std::make_unique<MCTSNode>(std::move(root_state))), team_(root_team),
      rng(std::random_device{}()) {
  root_->untried_moves = root_->state.legal_moves();
}

double MCTS::uct_value(const MCTSNode *child, int parent_visits,
                       double c) const {
  if (child->visits == 0) {
    return std::numeric_limits<double>::infinity();
  }
  double win_rate = child->wins / child->visits;
  double exploration =
      c * std::sqrt(std::log((double)parent_visits) / child->visits);
  return win_rate + exploration;
}

MCTSNode *MCTS::select_node(MCTSNode *node) {
  while (!node->state.end()) {
    if (!node->untried_moves.empty())
      return node;
    MCTSNode *best_child = nullptr;
    double best_uct = -std::numeric_limits<double>::infinity();
    for (const auto &child : node->children) {
      double score = uct_value(child.get(), node->visits, C);
      if (score > best_uct) {
        best_uct = score;
        best_child = child.get();
      }
    }
    if (!best_child)
      return node;
    node = best_child;
  }
  return node;
}

MCTSNode *MCTS::expand_node(MCTSNode *node) {
  if (node->untried_moves.empty())
    return node;
  std::uniform_int_distribution<size_t> dist(0, node->untried_moves.size() - 1);
  size_t index = dist(rng);
  Move move = node->untried_moves[index];
  node->untried_moves.erase(node->untried_moves.begin() + index);
  state next_state = node->state;
  next_state.step(move);
  auto child = std::make_unique<MCTSNode>(std::move(next_state), move, node);
  child->untried_moves = child->state.legal_moves();
  node->children.push_back(std::move(child));
  return node->children.back().get();
}

double MCTS::rollout(MCTSNode *node) {
    state copy = node->state;
    while (!copy.end()) {
        std::vector<Move> legal_moves = copy.legal_moves();
        if (legal_moves.empty()) {
            copy.step(Move{MoveType::Pass, '0', 0, 1, 0, {}});
            continue;
        }
        std::uniform_int_distribution<size_t> dist(0, legal_moves.size() - 1);
        size_t index = dist(rng);
        copy.step(legal_moves[index]);
    }
    int winner = copy.winner();
    return (winner == team_) ? 1.0 : 0.0;
}

void MCTS::backpropagate(MCTSNode *node, double result) {
    while (node) {
        node->visits += 1;
        node->wins += result;
        node = node->parent;
    }
}

Move MCTS::search(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        MCTSNode* leaf = select_node(root_.get());
        MCTSNode* child = expand_node(leaf);
        double result = rollout(child);
        backpropagate(child, result);
    }
    MCTSNode* best = nullptr;
    int best_visits = -1;
    for (const auto& child : root_->children) {
        if (child->visits > best_visits) {
            best_visits = child->visits;
            best = child.get();
        }
    }
    if (!best) {
        return Move{MoveType::Pass, '0', 0, 1, 0, {}};
    }
    return best->parent_move;
}

void MCTS::advance_root(Move move) {
    for (auto& child : root_->children) {
        if (child->parent_move.used == move.used && child->parent_move.type == move.type) {
            child->parent = nullptr;
            root_ = std::move(child);
            return;
        }
    }
    state next_state = root_->state;
    next_state.step(move);
    root_ = std::make_unique<MCTSNode>(std::move(next_state));
    root_->untried_moves = root_->state.legal_moves();
}

int MCTS::est_landlord(state s, int index, int iter) {
    double best_rate = -1.0;
    int best_cand = 0;
    auto rollout = [](state s, std::mt19937& rng) -> double {
        state temp = s;
        while (!temp.end()) {
        auto moves = temp.legal_moves();
        if (moves.empty()) {
            temp.step(Move{MoveType::Pass, '0', 0, 1, 0, {}});
            continue;
        }
        std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
        temp.step(moves[dist(rng)]);
    }
    int w = temp.winner();
    return (w == 1) ? 1.0 : 0.0;
    };
    for (int cand = 0; cand < s.players.size(); ++cand) {
        state temp; 
        temp.players = s.players;
        for (auto &p : temp.players) p.is_landlord = false;
        temp.players[cand].is_landlord = true;
        std::rotate(temp.players.begin(), temp.players.begin() + cand, temp.players.end());
        temp.prev = {};
        temp.turn = 0;
        temp.passes = 0;
        int wins = 0;
        for (int sims = 0; sims < iter; ++sims) {
            state sim = temp;
            double result = rollout(sim, rng);
            if (result > 0.5) wins++;
        }
        double win_rate = (double)wins / iter;
        if (win_rate > best_rate) {
            best_rate = win_rate;
            best_cand = cand;
        }
    }
    return best_cand;
}