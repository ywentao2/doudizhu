#pragma once
#include <memory>
#include <vector>
#include <random>
#include "move.h"
#include "state.h"

#define C 1.4142 // sqrt(2) for initial exploration constant

struct MCTSNode {
    state state;
    Move parent_move;
    MCTSNode* parent {nullptr};
    std::vector<std::unique_ptr<MCTSNode>> children;
    std::vector<Move> untried_moves;
    int visits {0};
    double wins {0.0};
    explicit MCTSNode(struct state&& state, Move move = {}, MCTSNode* parent = nullptr)
        : state(std::move(state)), parent_move(move), parent(parent) {}
};

class MCTS {
public:
    explicit MCTS(state root_state, int team);
    Move search(int iterations);
    void advance_root(Move move);
    int est_landlord(state s, int index, int iter);
    int root_visits() const { return root_->visits; }

private:
    std::unique_ptr<MCTSNode> root_;
    int team_;
    std::mt19937 rng;

    MCTSNode* select_node(MCTSNode* node);
    MCTSNode* expand_node(MCTSNode* node);
    double rollout(MCTSNode* node);
    void backpropagate(MCTSNode* node, double result);

    double uct_value(const MCTSNode* child, int parent_visits, double c) const;
};