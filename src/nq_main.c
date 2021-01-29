
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <list.h>
#include <stack.h>
#include "nq.h"

#define NQ_OUTPUT_FILENAME          "nq_solution.txt"

void print_solutions(unsigned int n, List *solutions);
void nq_solution_destroy_e(void *arg);


int main() {
    unsigned int n;
    struct NQ_CONTEXT context;
    List solutions;
    char inputBuffer[200];
    
    printf("Welcome to N-QUEEN problem solver program\n");
    printf("Enter n(>=2) : ");
    gets(inputBuffer);
    sscanf(inputBuffer, "%u", &n);
    printf("\nYour n = %u\n", n);
    
    list_init(&solutions, nq_solution_destroy_e);
    nq_context_init(n, &context);
    printf("Processing...\n");
    nq_solve(&context, &solutions);
    
    print_solutions(n, &solutions);
    printf("Solutions have been stored to %s\nPress any key to exit...", NQ_OUTPUT_FILENAME);
    getche();
    
    nq_context_destroy(&context);
    list_destroy(&solutions);
    return 0;
}

void nq_solution_destroy_e(void *arg) {
    nq_solution_destroy(arg);
    free(arg);
    return;
}

void print_solutions(unsigned int n, List *solutions) {
    ListElem *pListElem;
    struct NQ_SOLUTION *solve;
    struct NQ_QUEEN *queens;
    unsigned int itr, index;
    FILE *fpOut;
    char *matrix, *mrow;
    
    matrix = 0;
    fpOut = 0;
    matrix = (char *) malloc(n * n);
    if (matrix == 0) {
        goto PRINT_END;
    }
    fpOut = fopen(NQ_OUTPUT_FILENAME, "w");
    if (fpOut == 0) {
        goto PRINT_END;
    }
    fprintf(fpOut, "%u solutions found for n = %u.\r\n", list_size(solutions), n);
    pListElem = list_head(solutions);
    while (pListElem != 0) {
        solve = (struct NQ_SOLUTION *) list_data(pListElem);
        queens = solve->positions;
        memset((void *) matrix, '0', n * n);
        for (itr = 0; itr < n; itr += 1) {
            index = ((queens + itr)->pos_row) * n + (queens + itr)->pos_column;
            *(matrix + index) = 'l';
        }
        fprintf(fpOut, "\r\nSolution\r\n");
        for (itr = 0; itr < n; itr += 1) {
            fprintf(fpOut, "{%u,%u}, ", (queens + itr)->pos_row, (queens + itr)->pos_column);
        }
        fprintf(fpOut, "\r\n");
        for (itr = 0; itr < n; itr += 1) {
            mrow = matrix + (itr * n);
            for (index = 0; index < n; index += 1) {
                fwrite((const void *) (mrow + index), 1, 1, fpOut);
                fwrite((const void *) " ", 1, 1, fpOut);
            }
            fprintf(fpOut, "\r\n");
        }
        fprintf(fpOut, "\r\n");
        fflush(fpOut);
        pListElem = list_next(pListElem);
    }
    PRINT_END:
    free((void *) matrix);
    fclose(fpOut);
    return;
}
