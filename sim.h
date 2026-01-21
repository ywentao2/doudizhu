#pragma once
#include <iostream>

struct simulation_result {
    int games;
    int landlord_wins {0};
    int peasant_wins {0};
};

simulation_result run(int num_games, int decks, int iters_per_move);