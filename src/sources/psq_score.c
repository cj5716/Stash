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
    S(-32, 17), S(-28, 16), S(-39,  8), S(-23, -4), S(-23,  7), S( 14, 15), S( 15,  7), S(-16,-22),
    S(-30, 10), S(-42, 13), S(-22,  1), S(-19, -2), S(-15,  6), S(-18,  7), S( -3,-10), S(-15,-19),
    S(-33, 15), S(-33,  9), S(-16,-16), S(-10,-27), S( -1,-20), S( -4,-12), S( -8,-12), S(-26,-16),
    S(-23, 35), S(-28, 18), S(-19, -4), S(  2,-29), S( 17,-20), S( 24,-19), S( -4, -1), S(-11,  3),
    S(-16, 53), S(-16, 39), S(  7,  6), S( 24,-29), S( 30,-26), S( 98, -3), S( 45, 19), S( 10, 22),
    S( 81, 19), S( 63, 21), S( 64, -1), S( 86,-40), S( 98,-40), S( 45,-19), S(-79, 23), S(-66, 24)
};

// Square-based piece scoring for evaluation, using a file symmetry
const scorepair_t KnightSQT[32] = {
    S( -51, -46), S(  -0, -58), S( -17, -27), S(  -6,  -9),
    S(  -8, -27), S( -11, -10), S(   3, -26), S(   9, -11),
    S(  -2, -37), S(   8, -16), S(  14, -10), S(  23,  14),
    S(   8,   6), S(  30,  15), S(  23,  26), S(  27,  34),
    S(  19,  21), S(  19,  20), S(  43,  25), S(  24,  41),
    S( -24,  12), S(  18,  18), S(  39,  27), S(  47,  28),
    S(   6, -11), S( -14,   6), S(  54,  -0), S(  54,  14),
    S(-162, -58), S(-109,   6), S(-102,  25), S(  29,   7)
};

const scorepair_t BishopSQT[32] = {
    S(  12, -50), S(  26, -27), S(   1, -20), S(  -6, -13),
    S(  17, -33), S(  20, -35), S(  22, -13), S(   5,  -2),
    S(  14, -17), S(  22,  -4), S(  13, -11), S(  15,  22),
    S(  15, -30), S(  20,   7), S(  14,  27), S(  34,  39),
    S(   0,  -8), S(  18,  21), S(  36,  24), S(  39,  43),
    S(  21,  -5), S(  25,  23), S(  38,   2), S(  38,  18),
    S( -56,  -1), S( -49,  -7), S(  -9,  16), S(  -6,  10),
    S( -58, -15), S( -43,   9), S(-137,  17), S(-100,  15)
};

const scorepair_t RookSQT[32] = {
    S( -19, -43), S( -16, -34), S(  -8, -30), S(  -4, -33),
    S( -50, -41), S( -28, -39), S( -16, -34), S( -16, -35),
    S( -39, -25), S( -17, -21), S( -29, -16), S( -25, -19),
    S( -36,  -3), S( -23,   4), S( -25,   4), S( -13,  -7),
    S( -17,  18), S(  -3,  24), S(  18,  15), S(  28,  15),
    S(  -5,  26), S(  27,  21), S(  36,  25), S(  56,  14),
    S(  12,  31), S(   6,  32), S(  48,  30), S(  59,  31),
    S(  24,  25), S(  34,  28), S(  18,  26), S(  26,  22)
};

const scorepair_t QueenSQT[32] = {
    S(   9, -82), S(   5, -91), S(  11,-107), S(  17, -75),
    S(   6, -76), S(  15, -79), S(  19, -73), S(  16, -54),
    S(   9, -49), S(  13, -32), S(   8,  -9), S(  -1,  -8),
    S(   3,  -8), S(  15,  -3), S(  -3,  24), S(  -2,  40),
    S(  13,  -4), S(  -1,  42), S(   4,  41), S(  -5,  60),
    S(   5,   5), S(   4,  34), S(  -1,  70), S(  -0,  66),
    S( -12,  18), S( -48,  48), S(  -4,  64), S( -20,  89),
    S( -14,  29), S( -26,  41), S( -18,  59), S( -21,  66)
};

const scorepair_t KingSQT[32] = {
    S(  29,-101), S(  46, -53), S( -36, -39), S( -96, -27),
    S(  37, -53), S(  21, -20), S(  -7,  -7), S( -30,  -3),
    S( -62, -37), S(   3, -16), S( -18,   5), S( -20,  14),
    S(-113, -31), S( -22,   4), S( -21,  20), S( -30,  30),
    S( -67,  -2), S(  18,  42), S(   7,  49), S( -19,  51),
    S( -25,  29), S(  58,  76), S(  43,  79), S(  32,  61),
    S( -38,  -1), S(  17,  74), S(  43,  67), S(  37,  55),
    S(  26,-242), S( 105, -27), S(  77,   1), S(  17,  16)
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
