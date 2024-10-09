#ifndef MINIMAX_H
#define MINIMAX_H

#include "movegen.h"

int evaluate(GameState* state);
int minimax(GameState* state, int depth, int alpha, int beta, int maximizing_player, char best_move[6]);

#endif
