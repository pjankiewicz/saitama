#include "defs.h"

const int MATE = 29000;
const int INFINITE = 30000;
const int DRAW = 0;

static void CheckUp(S_SEARCHINFO *info) {
    // .. check if time up, or interrupt from GUI
    if (info->timeset == TRUE && getRealTime() > info->stoptime) {
        //        printf("%ull %ull\n", getRealTime(), info->stoptime);
        info->stopped = TRUE;
    }
    ReadInput(info);
}

static void PickNextMove(int move_num, S_MOVELIST *list) {
    S_MOVE temp;
    int best_score = 0;
    int best_num = move_num;
    for (int index = move_num; index < list->count; index++) {
        if (list->moves[index].score > best_score) {
            best_score = list->moves[index].score;
            best_num = index;
        }
    }

    // swap
    temp = list->moves[move_num];
    list->moves[move_num] = list->moves[best_num];
    list->moves[best_num] = temp;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
    int index = 0;
    int index2 = 0;

    for (index = 0; index < 13; index++) {
        for (index2 = 0; index2 < BRD_SQ_NUM; index2++) {
            pos->search_history[index][index2] = 0;
        }
    }
    for (index = 0; index < 2; index++) {
        for (index2 = 0; index2 < MAXDEPTH; index2++) {
            pos->search_killers[index][index2] = 0;
        }
    }

    pos->ply = 0;

    info->starttime = getRealTime();
    info->quit = 0;
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {

    assert(CheckBoard(pos));

    if ((info->nodes & 2047) == 0) {
        CheckUp(info);
    }

    info->nodes++;

    if (IsRepetition(pos) || pos->fiftyMove >= 100) {
        return 0;
    }

    if (pos->ply > MAXDEPTH - 1) {
        return EvalPosition(pos);
    }

    int Score = EvalPosition(pos);

    if (Score >= beta) {
        return beta;
    }

    if (Score > alpha) {
        alpha = Score;
    }

    S_MOVELIST list[1];
    GenerateAllCaptures(pos, list);

    int MoveNum = 0;
    int Legal = 0;
    int OldAlpha = alpha;
    int BestMove = NOMOVE;

    int PvMove = ProbePvTable(pos);

    if (PvMove != NOMOVE) {
        for (MoveNum = 0; MoveNum < list->count; ++MoveNum) {
            if (list->moves[MoveNum].move == PvMove) {
                list->moves[MoveNum].score = 2000000;
                break;
            }
        }
    }

    for (MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        PickNextMove(MoveNum, list);

        if (!MakeMove(pos, list->moves[MoveNum].move)) {
            continue;
        }

        Legal++;
        Score = -Quiescence(-beta, -alpha, pos, info);
        TakeMove(pos);

        if (info->stopped == TRUE) {
            return 0;
        }

        if (Score > alpha) {
            if (Score >= beta) {
                if (Legal == 1) {
                    info->fhf++;
                }
                info->fh++;
                return beta;
            }
            alpha = Score;
            BestMove = list->moves[MoveNum].move;
        }
    }

    if (alpha != OldAlpha) {
        StorePvMove(pos, BestMove);
    }

    return alpha;
}

int IsRepetition(const S_BOARD *pos) {
    for (int i = pos->hisPly - pos->fiftyMove; i < pos->hisPly - 1; i++) {
        if (pos->posKey == pos->history[i].posKey) {
            return TRUE;
        }
    }
    return FALSE;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int do_null) {
    assert(CheckBoard(pos));

    if (IsRepetition(pos) || pos->fiftyMove >= 100) {
        return DRAW;
    }

    if (pos->ply > MAXDEPTH - 1) {
        return EvalPosition(pos);
    }

    int in_check = SqAttacked(pos, pos->kingSq[pos->side], pos->side ^ 1);

//    if (in_check) depth++;

    if (depth == 0) {
        return Quiescence(alpha, beta, pos, info);
    }

    if ((info->nodes & 4096) == 0) {
        CheckUp(info);
    }

    info->nodes++;



    int legal = 0;
    int old_alpha = alpha;
    int best_move = NOMOVE;
    int cur_score = -INFINITE;
    int pvmove = ProbePvTable(pos);

    S_MOVELIST moves[1];
    GenerateAllMoves(pos, moves);

    if (pvmove != NOMOVE) {
        for (int move_num = 0; move_num < moves->count; move_num++) {
            if (moves->moves[move_num].move == pvmove) {
                moves->moves[move_num].score = 2000000;
                break;
            }
        }
    }

    for (int move_num = 0; move_num < moves->count; move_num++) {
        PickNextMove(move_num, moves);
        if (!MakeMove(pos, moves->moves[move_num].move)) {
            continue;
        }
        legal++;
        cur_score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, do_null);
        TakeMove(pos);

        if (info->stopped == TRUE) {
            return 0;
        }

        if (cur_score > alpha) {
            if (cur_score >= beta) {
                if (legal == 1) {
                    info->fhf++;
                }
                info->fh++;

                if (!(moves->moves[move_num].move & MFLAGCAP)) {
                    pos->search_killers[1][pos->ply] = pos->search_killers[0][pos->ply];
                    pos->search_killers[0][pos->ply] = moves->moves[move_num].move;
                }

                return beta;
            }
            alpha = cur_score;
            best_move = moves->moves[move_num].move;
            if (!(best_move & MFLAGCAP)) {
                pos->search_history[pos->pieces[FROMSQ(best_move)]][TOSQ(best_move)] += depth;
            }
        }
    }

    if (legal == 0) {
        if (in_check) {
            return -MATE + pos->ply;
        } else {
            return DRAW;
        }
    }

    if (alpha != old_alpha) {
        StorePvMove(pos, best_move);
    }

    return alpha;
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
    // .. iterative deepening, search init
    int best_move = NOMOVE;
    int best_score = -INFINITE;
    int current_depth = 0;
    int pv_moves = 0;
    int pv_num = 0;

    ClearForSearch(pos, info);

    for (current_depth = 1; current_depth <= info->depth; current_depth++) {
        best_score = AlphaBeta(-INFINITE, INFINITE, current_depth, pos, info, TRUE);
        if (info->stopped == TRUE && current_depth > 1) {
            break;
        }
        pv_moves = GetPvLine(pos, current_depth);
        best_move = pos->pvarray[0];
        printf("info score cp %d depth %d nodes %ld time %d ", best_score, current_depth, info->nodes,
               getRealTime() - info->starttime);

        printf("pv ");
        for (pv_num = 0; pv_num < pv_moves; pv_num++) {
            printf(" %s", PrMove(pos->pvarray[pv_num]));
        }
        printf("\n");
        //        if (info->fh > 0) {
        //            printf("Ordering: %.2f\n", (double) info->fhf / info->fh);
        //        }
    }
    printf("bestmove %s\n", PrMove(best_move));
}