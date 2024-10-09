#include "minimax.h"
#include <string.h>

int evaluate(GameState* state) {
    int score = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        int piece = state->board[i];
        switch (piece) {
            case 1: score += PAWN_VALUE + PAWN_PST[i]; break;
            case 2: score += KNIGHT_VALUE + KNIGHT_PST[i]; break;
            case 3: score += BISHOP_VALUE + BISHOP_PST[i]; break;
            case 4: score += ROOK_VALUE + ROOK_PST[i]; break;
            case 5: score += QUEEN_VALUE + QUEEN_PST[i]; break;
            case 6: score += KING_VALUE + KING_PST[i]; break;
            case -1: score -= PAWN_VALUE + PAWN_PST[63 - i]; break;
            case -2: score -= KNIGHT_VALUE + KNIGHT_PST[63 - i]; break;
            case -3: score -= BISHOP_VALUE + BISHOP_PST[63 - i]; break;
            case -4: score -= ROOK_VALUE + ROOK_PST[63 - i]; break;
            case -5: score -= QUEEN_VALUE + QUEEN_PST[63 - i]; break;
            case -6: score -= KING_VALUE + KING_PST[63 - i]; break;
        }
    }
    return state->side_to_move == 0 ? score : -score;
}

void apply_move(GameState* state, const char move[6], GameState* new_state) {
    memcpy(new_state, state, sizeof(GameState));

    int from = (8 - (move[1] - '0')) * 8 + (move[0] - 'a');
    int to = (8 - (move[3] - '0')) * 8 + (move[2] - 'a');
    new_state->board[to] = new_state->board[from];
    new_state->board[from] = 0;
    new_state->side_to_move = 1 - new_state->side_to_move;
}

int minimax(GameState* state, int depth, int alpha, int beta, int maximizing_player, char best_move[6]) {
    if (depth == 0) {
        return evaluate(state);
    }

    char moves[MAX_MOVES][6];
    int num_moves = generate_moves(state, moves);

    if (num_moves == 0) {
        return evaluate(state);
    }

    if (maximizing_player) {
        int max_eval = -100000;
        for (int i = 0; i < num_moves; i++) {
            GameState new_state;
            apply_move(state, moves[i], &new_state);
            int eval = minimax(&new_state, depth - 1, alpha, beta, 0, best_move);
            if (eval > max_eval) {
                max_eval = eval;
                if (depth == 3) {
                    strcpy(best_move, moves[i]);
                }
            }
            alpha = (alpha > eval) ? alpha : eval;
            if (beta <= alpha) {
                break;
            }
        }
        return max_eval;
    } else {
        int min_eval = 100000;
        for (int i = 0; i < num_moves; i++) {
            GameState new_state;
            apply_move(state, moves[i], &new_state);
            int eval = minimax(&new_state, depth - 1, alpha, beta, 1, best_move);
            if (eval < min_eval) {
                min_eval = eval;
            }
            beta = (beta < eval) ? beta : eval;
            if (beta <= alpha) {
                break;
            }
        }
        return min_eval;
    }
}
