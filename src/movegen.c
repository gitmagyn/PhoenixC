#include "movegen.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#define EMPTY 0

void init_board(GameState* state) {
    int initial[BOARD_SIZE] = {
        -4, -2, -3, -5, -6, -3, -2, -4,
        -1, -1, -1, -1, -1, -1, -1, -1,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,
         1,  1,  1,  1,  1,  1,  1,  1,
         4,  2,  3,  5,  6,  3,  2,  4
    };
    memcpy(state->board, initial, sizeof(initial));
    state->side_to_move = 0;
}

void parse_fen(GameState* state, const char* fen) {
    memset(state->board, 0, sizeof(state->board));

    int i = 0, sq = 0;
    while (fen[i] && sq < 64) {
        char c = fen[i];
        if (isdigit(c)) {
            sq += c - '0';
        } else if (c == '/') {
        } else {
            switch (c) {
                case 'p': state->board[sq] = -1; break;
                case 'r': state->board[sq] = -4; break;
                case 'n': state->board[sq] = -2; break;
                case 'b': state->board[sq] = -3; break;
                case 'q': state->board[sq] = -5; break;
                case 'k': state->board[sq] = -6; break;
                case 'P': state->board[sq] = 1; break;
                case 'R': state->board[sq] = 4; break;
                case 'N': state->board[sq] = 2; break;
                case 'B': state->board[sq] = 3; break;
                case 'Q': state->board[sq] = 5; break;
                case 'K': state->board[sq] = 6; break;
            }
            sq++;
        }
        i++;
    }

    while (fen[i] && fen[i] != ' ') i++;
    if (fen[i] == ' ') i++;
    state->side_to_move = (fen[i] == 'w') ? 0 : 1;
}

int is_square_empty(GameState* state, int sq) {
    return state->board[sq] == 0;
}

int is_square_opponent(GameState* state, int sq) {
    int piece = state->board[sq];
    if (state->side_to_move == 0) {
        return piece < 0;
    } else {
        return piece > 0;
    }
}

int is_within_board(int sq) {
    return sq >= 0 && sq < 64;
}

void add_move(char moves[MAX_MOVES][6], int* move_count, int from, int to) {
    snprintf(moves[*move_count], 6, "%c%d%c%d", 'a' + (from % 8), 8 - (from / 8), 'a' + (to % 8), 8 - (to / 8));
    (*move_count)++;
}

void make_move(GameState* state, const char* move) {
    int from = (move[1] - '1') * 8 + (move[0] - 'a');
    int to = (move[3] - '1') * 8 + (move[2] - 'a');
    state->board[to] = state->board[from];
    state->board[from] = EMPTY;
}

void generate_pawn_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count) {
    int direction = state->side_to_move == 0 ? -8 : 8;
    int start_rank = state->side_to_move == 0 ? 6 : 1;
    for (int i = 0; i < 64; i++) {
        if ((state->side_to_move == 0 && state->board[i] == 1) || (state->side_to_move == 1 && state->board[i] == -1)) {
            int forward = i + direction;
            if (is_within_board(forward) && is_square_empty(state, forward)) {
                add_move(moves, move_count, i, forward);
                if (i / 8 == start_rank) {
                    int double_forward = i + 2 * direction;
                    if (is_within_board(double_forward) && is_square_empty(state, double_forward)) {
                        add_move(moves, move_count, i, double_forward);
                    }
                }
            }
        }
    }
}

void generate_knight_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count) {
    int knight_offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    for (int i = 0; i < 64; i++) {
        if ((state->side_to_move == 0 && state->board[i] == 2) || (state->side_to_move == 1 && state->board[i] == -2)) {
            for (int j = 0; j < 8; j++) {
                int to = i + knight_offsets[j];
                if (is_within_board(to) && abs((i % 8) - (to % 8)) <= 2 && 
                    (is_square_empty(state, to) || is_square_opponent(state, to))) {
                    add_move(moves, move_count, i, to);
                }
            }
        }
    }
}

void generate_bishop_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count) {
    int directions[] = {-9, -7, 7, 9};
    for (int i = 0; i < 64; i++) {
        if ((state->side_to_move == 0 && state->board[i] == 3) || (state->side_to_move == 1 && state->board[i] == -3)) {
            for (int j = 0; j < 4; j++) {
                int to = i;
                while (1) {
                    int from_rank = i / 8;
                    int from_file = i % 8;

                    to += directions[j];
                    int to_rank = to / 8;
                    int to_file = to % 8;

                    if (!is_within_board(to) || abs(from_rank - to_rank) != abs(from_file - to_file)) break;

                    if (is_square_empty(state, to)) {
                        add_move(moves, move_count, i, to);
                    } else if (is_square_opponent(state, to)) {
                        add_move(moves, move_count, i, to);
                        break;
                    } else {
                        break;
                    }
                }
            }
        }
    }
}

void generate_rook_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count) {
    int directions[] = {-8, -1, 1, 8};
    for (int i = 0; i < 64; i++) {
        if ((state->side_to_move == 0 && state->board[i] == 4) || (state->side_to_move == 1 && state->board[i] == -4)) {
            for (int j = 0; j < 4; j++) {
                int to = i;
                while (1) {
                    to += directions[j];
                    if (!is_within_board(to) || (directions[j] == -1 && to % 8 == 7) || (directions[j] == 1 && to % 8 == 0)) break;
                    if (!is_square_empty(state, to)) {
                        if (is_square_opponent(state, to)) add_move(moves, move_count, i, to);
                        break;
                    }
                    add_move(moves, move_count, i, to);
                }
            }
        }
    }
}

void generate_queen_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count) {
    int directions[] = {-9, -7, 7, 9, -8, -1, 1, 8};
    for (int i = 0; i < 64; i++) {
        if ((state->side_to_move == 0 && state->board[i] == 5) || (state->side_to_move == 1 && state->board[i] == -5)) {
            for (int j = 0; j < 8; j++) {
                int to = i;
                while (1) {
                    to += directions[j];
                    if (!is_within_board(to) || 
                        (directions[j] == -1 && to % 8 == 7) || 
                        (directions[j] == 1 && to % 8 == 0)) break;
                    if (!is_square_empty(state, to)) {
                        if (is_square_opponent(state, to)) {
                            add_move(moves, move_count, i, to);
                        }
                        break;
                    }
                    add_move(moves, move_count, i, to);
                }
            }
        }
    }
}

void generate_king_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count) {
    int king_offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    for (int i = 0; i < 64; i++) {
        if ((state->side_to_move == 0 && state->board[i] == 6) || (state->side_to_move == 1 && state->board[i] == -6)) {
            for (int j = 0; j < 8; j++) {
                int to = i + king_offsets[j];
                if (is_within_board(to) && (is_square_empty(state, to) || is_square_opponent(state, to))) {
                    add_move(moves, move_count, i, to);
                }
            }
        }
    }
}

int generate_moves(GameState* state, char moves[MAX_MOVES][6]) {
    int move_count = 0;
    generate_pawn_moves(state, moves, &move_count);
    generate_knight_moves(state, moves, &move_count);
    generate_bishop_moves(state, moves, &move_count);
    generate_rook_moves(state, moves, &move_count);
    generate_queen_moves(state, moves, &move_count);
    generate_king_moves(state, moves, &move_count);
    return move_count;
}