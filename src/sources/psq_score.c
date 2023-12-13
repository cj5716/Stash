/*
**    Stash, a UCI chess playing engine developed from scratch
**    Copyright (C) 2019-2023 Morgan Houppin
**
**    Stash is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    Stash is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "psq_score.h"
#include "types.h"

// clang-format off

scorepair_t PsqScore[PIECE_NB][SQUARE_NB];
const score_t PieceScores[PHASE_NB][PIECE_NB] = {
    {
        0, PAWN_MG_SCORE, KNIGHT_MG_SCORE, BISHOP_MG_SCORE, ROOK_MG_SCORE, QUEEN_MG_SCORE, 0, 0,
        0, PAWN_MG_SCORE, KNIGHT_MG_SCORE, BISHOP_MG_SCORE, ROOK_MG_SCORE, QUEEN_MG_SCORE, 0, 0
    },
    {
        0, PAWN_EG_SCORE, KNIGHT_EG_SCORE, BISHOP_EG_SCORE, ROOK_EG_SCORE, QUEEN_EG_SCORE, 0, 0,
        0, PAWN_EG_SCORE, KNIGHT_EG_SCORE, BISHOP_EG_SCORE, ROOK_EG_SCORE, QUEEN_EG_SCORE, 0, 0
    }
};

#define S SPAIR

// Square-based Pawn scoring for evaluation
const scorepair_t PawnSQT[48] = {
    S(-32, 16), S(-28, 16), S(-39,  8), S(-23, -4), S(-23,  7), S( 14, 15), S( 14,  7), S(-16,-22),
    S(-30, 10), S(-42, 13), S(-22,  1), S(-20, -2), S(-15,  6), S(-18,  8), S( -3,-10), S(-15,-19),
    S(-33, 15), S(-33,  9), S(-16,-16), S(-10,-27), S( -1,-20), S( -4,-12), S( -8,-13), S(-26,-16),
    S(-23, 35), S(-28, 18), S(-19, -4), S(  2,-29), S( 17,-20), S( 24,-19), S( -4, -1), S(-11,  2),
    S(-16, 53), S(-16, 39), S(  7,  6), S( 24,-29), S( 30,-26), S( 99, -3), S( 45, 20), S( 10, 22),
    S( 82, 20), S( 63, 21), S( 63, -1), S( 86,-40), S( 96,-39), S( 43,-19), S(-76, 23), S(-66, 24)
};

// Square-based piece scoring for evaluation, using a file symmetry
const scorepair_t KnightSQT[32] = {
    S( -51, -44), S(  -0, -58), S( -17, -26), S(  -6,  -8),
    S(  -8, -27), S( -10,  -9), S(   3, -26), S(   9, -10),
    S(  -2, -37), S(   8, -15), S(  15, -10), S(  24,  14),
    S(   8,   7), S(  30,  15), S(  23,  26), S(  27,  34),
    S(  19,  22), S(  19,  21), S(  43,  25), S(  24,  41),
    S( -25,  13), S(  18,  19), S(  39,  27), S(  47,  28),
    S(   6, -11), S( -15,   6), S(  54,  -0), S(  53,  15),
    S(-163, -58), S(-110,   6), S(-102,  25), S(  30,   7)
};

const scorepair_t BishopSQT[32] = {
    S(  13, -51), S(  26, -27), S(   1, -20), S(  -7, -13),
    S(  17, -33), S(  20, -35), S(  22, -13), S(   5,  -2),
    S(  14, -17), S(  22,  -3), S(  13, -11), S(  15,  22),
    S(  15, -30), S(  20,   7), S(  14,  28), S(  34,  39),
    S(   0,  -7), S(  18,  21), S(  36,  25), S(  39,  44),
    S(  21,  -5), S(  25,  24), S(  38,   2), S(  38,  18),
    S( -56,   0), S( -49,  -7), S(  -9,  17), S(  -6,  10),
    S( -59, -16), S( -43,  10), S(-137,  17), S( -99,  15)
};

const scorepair_t RookSQT[32] = {
    S( -19, -43), S( -16, -34), S(  -8, -30), S(  -4, -33),
    S( -50, -41), S( -28, -39), S( -16, -34), S( -16, -34),
    S( -39, -25), S( -17, -21), S( -29, -15), S( -25, -18),
    S( -36,  -3), S( -23,   4), S( -25,   4), S( -13,  -7),
    S( -17,  18), S(  -3,  24), S(  17,  15), S(  27,  16),
    S(  -5,  27), S(  27,  22), S(  36,  25), S(  56,  14),
    S(  12,  31), S(   5,  33), S(  47,  31), S(  59,  31),
    S(  24,  26), S(  34,  29), S(  17,  27), S(  25,  23)
};

const scorepair_t QueenSQT[32] = {
    S(   8, -80), S(   4, -90), S(  11,-106), S(  17, -74),
    S(   6, -75), S(  15, -78), S(  19, -72), S(  16, -53),
    S(   8, -47), S(  13, -31), S(   8,  -8), S(  -1,  -7),
    S(   2,  -7), S(  15,  -2), S(  -3,  25), S(  -2,  41),
    S(  13,  -3), S(  -1,  43), S(   4,  41), S(  -5,  61),
    S(   5,   6), S(   3,  35), S(  -0,  71), S(   1,  66),
    S( -12,  18), S( -48,  48), S(  -3,  64), S( -18,  88),
    S( -13,  29), S( -24,  41), S( -17,  60), S( -19,  67)
};

const scorepair_t KingSQT[32] = {
    S(  29,-103), S(  46, -54), S( -35, -40), S( -95, -27),
    S(  37, -54), S(  21, -21), S(  -7,  -7), S( -30,  -3),
    S( -61, -38), S(   3, -17), S( -17,   4), S( -19,  13),
    S(-114, -32), S( -23,   4), S( -20,  20), S( -28,  29),
    S( -68,  -2), S(  18,  42), S(   8,  49), S( -17,  51),
    S( -26,  29), S(  56,  76), S(  44,  79), S(  33,  61),
    S( -38,  -1), S(  16,  75), S(  42,  68), S(  37,  55),
    S(  27,-240), S( 106, -24), S(  77,   4), S(  16,  17)
};

#undef S

// clang-format on

static void psq_score_init_piece(const scorepair_t *table, piece_t piece)
{
    const scorepair_t pieceValue =
        create_scorepair(PieceScores[MIDGAME][piece], PieceScores[ENDGAME][piece]);

    for (square_t square = SQ_A1; square <= SQ_H8; ++square)
    {
        file_t queensideFile = imin(sq_file(square), sq_file(square) ^ 7);
        scorepair_t psqEntry = pieceValue + table[sq_rank(square) * 4 + queensideFile];

        PsqScore[piece][square] = psqEntry;
        PsqScore[opposite_piece(piece)][opposite_sq(square)] = -psqEntry;
    }
}

static void psq_score_init_pawn(void)
{
    const scorepair_t pieceValue = create_scorepair(PAWN_MG_SCORE, PAWN_EG_SCORE);

    for (square_t square = SQ_A1; square <= SQ_H8; ++square)
    {
        if (sq_rank(square) == RANK_1 || sq_rank(square) == RANK_8)
        {
            PsqScore[WHITE_PAWN][square] = 0;
            PsqScore[BLACK_PAWN][opposite_sq(square)] = 0;
        }
        else
        {
            scorepair_t psqEntry = pieceValue + PawnSQT[square - SQ_A2];

            PsqScore[WHITE_PAWN][square] = psqEntry;
            PsqScore[BLACK_PAWN][opposite_sq(square)] = -psqEntry;
        }
    }
}

void psq_score_init(void)
{
    psq_score_init_pawn();
    psq_score_init_piece(KnightSQT, KNIGHT);
    psq_score_init_piece(BishopSQT, BISHOP);
    psq_score_init_piece(RookSQT, ROOK);
    psq_score_init_piece(QueenSQT, QUEEN);
    psq_score_init_piece(KingSQT, KING);
}
