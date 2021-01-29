
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avl.h>
#include <stack.h>
#include <list.h>
#include "nq.h"


int nq_queen_compare(const void *arg1, const void *arg2);
int nq_solution_equals(const void *arg1, const void *arg2);


int nq_queen_compare(const void *arg1, const void *arg2) {
    int cmpval;
    struct NQ_QUEEN *queen1, *queen2;
    queen1 = (struct NQ_QUEEN *) arg1;
    queen2 = (struct NQ_QUEEN *) arg2;
    if ((queen1->pos_row == queen2->pos_row) && (queen1->pos_column == queen2->pos_column)) {
        cmpval = 0;
    }
    else {
        if (queen1->pos_row != queen2->pos_row) {
            cmpval = queen1->pos_row - queen2->pos_row;
        }
        else {
            cmpval = queen1->pos_column - queen2->pos_column;
        }
    }
    return cmpval;
}

int nq_solution_equals(const void *arg1, const void *arg2) {
    int cmpval;
    struct NQ_SOLUTION *sol1, *sol2;
    unsigned int itr;
    sol1 = (struct NQ_SOLUTION *) arg1;
    sol2 = (struct NQ_SOLUTION *) arg2;
    if (sol1->length != sol2->length) {
        cmpval = -1;
    }
    else {
        cmpval = 0;
        for (itr = 0; itr < sol1->length; itr += 1) {
            cmpval = nq_queen_compare((const void *) (sol1->positions + itr), (const void *) (sol2->positions + itr));
            if (cmpval != 0) {
                break;
            }
        }
    }
    return cmpval;
}

int nq_board_init(struct NQ_BOARD *board, unsigned int sideLength) {
    int retval;
    unsigned int *matrix, data_length;
    retval = -1;
    data_length = sizeof(unsigned int) * sideLength * sideLength;
    matrix = (unsigned int *) malloc(data_length);
    if (matrix == 0) {
        goto INIT_END;
    }
    memset((void *) board, 0, sizeof(struct NQ_BOARD));
    memset((void *) matrix, NQ_BOARD_SQUARE_VALUE, data_length);
    board->sideLength = sideLength;
    board->data = matrix;
    retval = 0;
    INIT_END:
    return retval;
}

void nq_board_destroy(void *board) {
    struct NQ_BOARD *board_t;
    board_t = (struct NQ_BOARD *) board;
    free((void *) board_t->data);
    return;
}

int nq_queen_init(struct NQ_QUEEN *queen, int id) {
    int retval;
    retval = 0;
    memset((void *) queen, 0, sizeof(struct NQ_QUEEN));
    queen->id = id;
    queen->pos_row = -1;
    queen->pos_column = -1;
    return retval;
}

int nq_solution_init(unsigned int n, struct NQ_QUEEN *queens, struct NQ_SOLUTION *solution) {
    int retval;
    struct NQ_QUEEN *positions;
    retval = -1;
    positions = (struct NQ_QUEEN *) malloc(sizeof(struct NQ_QUEEN) * n);
    if (positions == 0) {
        goto INIT_END;
    }
    memset((void *) solution, 0, sizeof(struct NQ_SOLUTION));
    memcpy((void *) positions, (const void *) queens, sizeof(struct NQ_QUEEN) * n);
    solution->length = n;
    solution->positions = positions;
    retval = 0;
    INIT_END:
    return retval;
}

void nq_solution_destroy(void *arg) {
    struct NQ_SOLUTION *solution;
    solution = (struct NQ_SOLUTION *) arg;
    free((void *) solution->positions);
    return;
}

int nq_context_init(unsigned int n, struct NQ_CONTEXT *context) {
    int retval;
    unsigned int itr;
    struct NQ_QUEEN *arr_queen, *sorted_queen;
    
    retval = -1;
    if (n == 0 || context == 0) {
        goto INIT_END;
    }
    
    memset((void *) context, 0, sizeof(struct NQ_CONTEXT));
    
    arr_queen = (struct NQ_QUEEN *) malloc(sizeof(struct NQ_QUEEN) * n);
    sorted_queen = (struct NQ_QUEEN *) malloc(sizeof(struct NQ_QUEEN) * n);
    
    nq_board_init(&(context->board), n);
    stack_init(&(context->stPlace), 0);
    avl_init(&(context->unqSol), nq_solution_equals, 0, 0);
    
    context->n = n;
    context->queens = arr_queen;
    for (itr = 0; itr < n; itr += 1) {
        nq_queen_init(arr_queen + itr, itr + 1);
        stack_push(&(context->stPlace), (const void *) (arr_queen + itr));
    }
    
    memcpy((void *) sorted_queen, (const void *) arr_queen, sizeof(struct NQ_QUEEN) * n);
    context->sortedQueens = sorted_queen;
    retval = 0;
    INIT_END:
    return retval;
}

void nq_context_destroy(struct NQ_CONTEXT *context) {
    free((void *) (context->queens));
    nq_board_destroy((void *) &(context->board));
    stack_destroy(&(context->stPlace));
    avl_destroy(&(context->unqSol));
    free((void *) (context->sortedQueens));
    return;
}

int nq_set_on_board(int setType, struct NQ_QUEEN *queen, struct NQ_BOARD *board) {
    int retval;
    unsigned int setValue, searchValue, rowIndex, colIndex, index;
    retval = -1;
    if (setType == NQ_QUEEN_PLACE) {
        setValue = queen->id;
        searchValue = NQ_BOARD_SQUARE_VALUE;
    }
    else if (setType == NQ_QUEEN_DISPLACE) {
        setValue = NQ_BOARD_SQUARE_VALUE;
        searchValue = queen->id;
    }
    else {
        goto SET_END;
    }
    if (queen->pos_row == -1 || queen->pos_column == -1) {
        goto SET_END;
    }
    
    // Set whole row of the respected queen's position to value
    rowIndex = queen->pos_row;
    for (colIndex = 0; colIndex < board->sideLength; colIndex += 1) {
        index = (rowIndex * board->sideLength) + colIndex;
        if (*(board->data + index) == searchValue) {
            *(board->data + index) = setValue;
        }
    }
    
    // Set whole column of the respected queen's position to value
    colIndex = queen->pos_column;
    for (rowIndex = 0; rowIndex < board->sideLength; rowIndex += 1) {
        index = (rowIndex * board->sideLength) + colIndex;
        if (*(board->data + index) == searchValue) {
            *(board->data + index) = setValue;
        }
    }
    
    // Set upper-left index positions regarding queen's position to vaue
    rowIndex = queen->pos_row;
    colIndex = queen->pos_column;
    while (rowIndex > 0 && colIndex > 0) {
        rowIndex -= 1;
        colIndex -= 1;
        index = (rowIndex * board->sideLength) + colIndex;
        if (*(board->data + index) == searchValue) {
            *(board->data + index) = setValue;
        }
    }
    
    // Set lower-right index positions regarding queen's position to vaue
    rowIndex = queen->pos_row;
    colIndex = queen->pos_column;
    while (rowIndex < board->sideLength - 1 && colIndex < board->sideLength - 1) {
        rowIndex += 1;
        colIndex += 1;
        index = (rowIndex * board->sideLength) + colIndex;
        if (*(board->data + index) == searchValue) {
            *(board->data + index) = setValue;
        }
    }
    
    // Set upper-right index positions regarding queen's position to vaue
    rowIndex = queen->pos_row;
    colIndex = queen->pos_column;
    while (rowIndex > 0 && colIndex < board->sideLength - 1) {
        rowIndex -= 1;
        colIndex += 1;
        index = (rowIndex * board->sideLength) + colIndex;
        if (*(board->data + index) == searchValue) {
            *(board->data + index) = setValue;
        }
    }
    
    // Set lower-left index positions regarding queen's position to vaue
    rowIndex = queen->pos_row;
    colIndex = queen->pos_column;
    while (rowIndex < board->sideLength - 1 && colIndex > 0) {
        rowIndex += 1;
        colIndex -= 1;
        index = (rowIndex * board->sideLength) + colIndex;
        if (*(board->data + index) == searchValue) {
            *(board->data + index) = setValue;
        }
    }
    retval = 0;
    SET_END:
    return retval;
}

int nq_solve(struct NQ_CONTEXT *context, List *solutions) {
    int retval, opval, insval;
    struct NQ_QUEEN *queen;
    unsigned int boardLength, index, value;
    Stack *stQueens;
    struct NQ_BOARD *board;
    struct NQ_SOLUTION *solution;
    
    retval = -1;
    if (context == 0 || solutions == 0) {
        goto SOLVE_END;
    }
    stQueens = &(context->stPlace);
    board = &(context->board);
    
    // If all the queens have been placed on the board (empty stack), then just return
    // Previous recursive call to this function will consider this as a success
    // And will take necessary steps to store the solution
    if (context->n < 2 || stack_size(stQueens) == 0) {
        goto SOLVE_END;
    }
    stack_pop(stQueens, (void **) &queen);
    boardLength = board->sideLength * board->sideLength;
    
    // Enemurate whole board to find an empty place where a queen can be placed
    for (index = 0; index < boardLength; index += 1) {
        value = *(board->data + index);
        if (value == NQ_BOARD_SQUARE_VALUE) {
            // We have found an empty place where a queen might be placed
            // Without threateaning itself or other queens (independent position)
            queen->pos_row = index / board->sideLength;
            queen->pos_column = index % board->sideLength;
            // Place the queen at hand into the position
            nq_set_on_board(NQ_QUEEN_PLACE, queen, board);
            // Try to place the next queen on this board
            opval = nq_solve(context, solutions);
            if (opval == -1) {
                // Next queen cannot be placed because there is no queen left
                // We have found a solution, so store a copy of it to List *solutions
                
                memcpy((void *) context->sortedQueens, (const void *) context->queens, sizeof(struct NQ_QUEEN) * context->n);
                qsort((void *) context->sortedQueens, context->n, sizeof(struct NQ_QUEEN), nq_queen_compare);
                solution = (struct NQ_SOLUTION *) malloc(sizeof(struct NQ_SOLUTION));
                nq_solution_init(context->n, context->sortedQueens, solution);
                insval = avl_insert(&(context->unqSol), (const void *) solution, 0);
                if (insval == 0) {
                    list_ins_next(solutions, list_tail(solutions), (const void *) solution);
                }
                else {
                    nq_solution_destroy((void *) solution);
                }
            }
            nq_set_on_board(NQ_QUEEN_DISPLACE, queen, board);
        }
    }
    stack_push(stQueens, (const void *) queen);
    retval = 0;
    SOLVE_END:
    return retval;
}
