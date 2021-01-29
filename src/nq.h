
#ifndef NQ_H_INCLUDED
#define NQ_H_INCLUDED

#include <avl.h>
#include <stack.h>
#include <list.h>

#define NQ_QUEEN_PLACE              0
#define NQ_QUEEN_DISPLACE           1
#define NQ_BOARD_SQUARE_VALUE       0


struct NQ_QUEEN {
    unsigned int id;
    int pos_row;
    int pos_column;
};

struct NQ_BOARD {
    unsigned int sideLength;
    unsigned int *data;
};

struct NQ_SOLUTION {
    struct NQ_QUEEN *positions;
    unsigned int length;
};

struct NQ_CONTEXT {
    unsigned int n;
    struct NQ_BOARD board;
    Stack stPlace;
    struct NQ_QUEEN *queens;
    AvlTree unqSol;
    struct NQ_QUEEN *sortedQueens;
};



int nq_board_init(struct NQ_BOARD *board, unsigned int sideLength);

void nq_board_destroy(void *board);

int nq_queen_init(struct NQ_QUEEN *queen, int id);

int nq_solution_init(unsigned int n, struct NQ_QUEEN *queens, struct NQ_SOLUTION *solution);

void nq_solution_destroy(void *solution);

int nq_context_init(unsigned int n, struct NQ_CONTEXT *context);

void nq_context_destroy(struct NQ_CONTEXT *context);

int nq_set_on_board(int setType, struct NQ_QUEEN *queen, struct NQ_BOARD *board);

int nq_solve(struct NQ_CONTEXT *context, List *solutions);




#endif // NQ_H_INCLUDED
