#include <stdio.h>
#include <string.h>
#include "movegen.h"
#include "minimax.h"

// PST
const int PAWN_PST[BOARD_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 
    -1, -1, -2, -2, -2, -2, -1, -1, 
    1, 0, 0, 0, 0, 0, 0, 1, 
    0.5, 0.5, 0.5, 1, 1, 0.5, 0.5, 0.5, 
    0, 0.5, 1, 1.5, 1.5, 1, 0.5, 0, 
    0.3, 1, 2, 2.5, 2.5, 2, 1, 0.3, 
    1, 2, 3, 4, 4, 3, 2, 1, 
    0, 0, 0, 0, 0, 0, 0, 0
};

const int KNIGHT_PST[BOARD_SIZE] = {
    -5, -4, -3, -3, -3, -3, -4, -5, 
    -4, -2, 0, 0.5, 0.5, 0, -2, -4, 
    -3, 0, 1, 1.5, 1.5, 1, 0, -3, 
    -3, 0.5, 1.5, 1, 1, 1.5, 0.5, -3, 
    -3, 0.5, 1.5, 1, 1, 1.5, 0.5, -3, 
    -3, 0.5, 1.5, 1.5, 1.5, 1, 0.5, -3, 
    -4, -2, 0, 0.5, 0.5, 0, -2, -4, 
    -5, -4, -3, -3, -3, -3, -4, -5
};

const int BISHOP_PST[BOARD_SIZE] = {
    -2, -1, -1, -1, -1, -1, -1, -2, 
    -1, 0, 0, 0, 0, 0, 0, -1, 
    -0.5, 0, 0.5, 1, 1, 0.5, 0, -0.5, 
    -0.5, 0.5, 1, 1.5, 1.5, 1, 0.5, -0.5, 
    -0.5, 0.5, 1, 1.5, 1.5, 1, 0.5, -0.5, 
    -0.5, 0, 0.5, 1, 1, 0.5, 0, -0.5, 
    -1, 0, 0, 0, 0, 0, 0, -1, 
    -2, -1, -1, -1, -1, -1, -1, -2
};

// ADICIONAR PARA TORRE, DAMA E REI
// EXEMPLO PARA TESTES

const int ROOK_PST[BOARD_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 
    0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 
    0, 0, 0, 0.5, 0.5, 0, 0, 0, 
    0, 0, 0, 0.5, 0.5, 0, 0, 0, 
    0, 0, 0, 0.5, 0.5, 0, 0, 0, 
    0, 0, 0, 0.5, 0.5, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 
    0, 0, 0, 0, 0, 0, 0, 0
};

const int QUEEN_PST[BOARD_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 
    0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 
    0, 0, 0, 0.5, 0.5, 0, 0, 0, 
    0, 0, 0, 0.5, 0.5, 0, 0, 0, 
    0, 0, 0, 0.5, 0.5, 0, 0, 0, 
    0, 0, 0, 0.5, 0.5, 0, 0, 0, 
    0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 
    0, 0, 0, 0, 0, 0, 0, 0
};

// MIDGAME
const int KING_PST[BOARD_SIZE] = {
    -3, -4, -4, -5, -5, -4, -4, -3, 
    -3, -4, -4, -5, -5, -4, -4, -3, 
    -3, -4, -4, -5, -5, -4, -4, -3, 
    -3, -4, -4, -5, -5, -4, -4, -3, 
    -2, -3, -3, -4, -4, -3, -3, -2, 
    -1, -2, -2, -2, -2, -2, -2, -1, 
    1, 2, 2, 2, 2, 2, 2, 1, 
    2, 3, 1, 0, 0, 1, 3, 2
};

void uci_position_fen(GameState* state, const char* fen) {
    parse_fen(state, fen);
}

void uci_position(GameState* state, const char* input) {
    if (strstr(input, "startpos")) {
        init_board(state);
    } else if (strstr(input, "fen")) {
        const char* fen = strstr(input, "fen") + 4;
        uci_position_fen(state, fen);
    }

    const char* moves_str = strstr(input, "moves");
    if (moves_str) {
        moves_str += 6;
        char move[6];
        while (sscanf(moves_str, "%s", move) == 1) {
            make_move(state, move);
            moves_str += strlen(move) + 1;
        }
    }
}

void uci_go(GameState* state) {
    char moves[MAX_MOVES][6];
    int num_moves = generate_moves(state, moves);

    if (num_moves > 0) {
        char best_move[6];
        int alpha = -10000;
        int beta = 10000;
        int depth = 4;

        int best_eval = -10000;
        for (int i = 0; i < num_moves; i++) {
            GameState new_state = *state;
            make_move(&new_state, moves[i]);

            int eval = minimax(&new_state, depth - 1, alpha, beta, 0, best_move);
            if (eval > best_eval) {
                best_eval = eval;
                strcpy(best_move, moves[i]);
            }
        }

        make_move(state, best_move);

        printf("bestmove %s\n", best_move);
    } else {
        printf("bestmove (none)\n");
    }
}

void uci_loop(GameState* state) {
    char input[256];
    while (1) {
        if (!fgets(input, sizeof(input), stdin)) {
            continue;
        }
        if (strncmp(input, "uci", 3) == 0) {
            printf("id name Phoenix\n");
            printf("id author Magyn\n");
            printf("uciok\n");
            fflush(stdout);
        } else if (strncmp(input, "isready", 7) == 0) {
            printf("readyok\n");
            fflush(stdout);
        } else if (strncmp(input, "position", 8) == 0) {
            uci_position(state, input);
        } else if (strncmp(input, "ucinewgame", 10) == 0) {
            printf("id name Phoenix\n");
            printf("id author Magyn\n");
            printf("uciok\n");
        } else if (strncmp(input, "stop", 4) == 0) {
            printf("search stopped\n");
        } else if (strncmp(input, "go", 2) == 0) {
            uci_go(state);
            fflush(stdout);
        } else if (strncmp(input, "mov", 3) == 0) {
            char moves[MAX_MOVES][6];
            int num_moves = generate_moves(state, moves);
            printf("Legal moves:\n");
            for (int i = 0; i < num_moves; i++) {
                printf("%s\n", moves[i]);
            }
        } else if (strncmp(input, "quit", 4) == 0) {
            break;
        }
    }
}

int main() {
    GameState state;
    init_board(&state);
    uci_loop(&state);
    return 0;
}