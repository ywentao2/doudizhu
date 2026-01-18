#pragma once
#include <cstdint>
#include <array>
#include "setup.h"


enum class MoveType {
    Invalid,
    Single,
    Pair,
    Triple,
    Three_Carry_One,
    Three_Carry_Two,
    Double_Three_Carry_One,
    Double_Three_Carry_Two,
    Straight,
    Airplane,
    Bomb,
    Rocket,
    Pass
};

struct Move {
    MoveType type {MoveType::Invalid};
    char rank {'0'};
    uint8_t length {0}; 
    uint8_t count {1};  
    uint8_t primary_rank {static_cast<uint8_t>(-1)}; //for comparing moves
    std::array<uint8_t, 15> used {};
};

bool valid_move(const std::array<uint8_t, 15>& hand_map, const std::array<uint8_t, 15>& used);
void apply_move(std::array<uint8_t, 15>& hand_map, const std::array<uint8_t, 15>& used);
void undo(std::array<uint8_t, 15>& hand_map, const std::array<uint8_t, 15>& used);
void process_move(const std::string& move_str, Move& move, player& p);
bool wins(const Move& curr, const Move& prev);