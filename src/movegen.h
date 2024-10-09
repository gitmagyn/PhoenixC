#ifndef MOVEGEN_H
#define MOVEGEN_H

#define MAX_MOVES 256
#define BOARD_SIZE 64

#define PAWN_VALUE 10
#define KNIGHT_VALUE 30
#define BISHOP_VALUE 35
#define ROOK_VALUE 50
#define QUEEN_VALUE 90
#define KING_VALUE 10000

extern const int PAWN_PST[BOARD_SIZE];
extern const int KNIGHT_PST[BOARD_SIZE];
extern const int BISHOP_PST[BOARD_SIZE];
extern const int ROOK_PST[BOARD_SIZE];
extern const int QUEEN_PST[BOARD_SIZE];
extern const int KING_PST[BOARD_SIZE];

typedef struct {
    int board[BOARD_SIZE];
    int side_to_move;
} GameState;

void init_board(GameState* state);
void parse_fen(GameState* state, const char* fen);
int generate_moves(GameState* state, char moves[MAX_MOVES][6]);

void generate_pawn_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count);
void generate_knight_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count);
void generate_bishop_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count);
void generate_rook_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count);
void generate_queen_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count);
void generate_king_moves(GameState* state, char moves[MAX_MOVES][6], int* move_count);

void make_move(GameState* state, const char* move);

#endif
