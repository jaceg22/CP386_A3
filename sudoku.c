#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

int sudoku[SIZE][SIZE];
int results[11] = {0}; // Stores validation results for threads

// Struct to pass row and column parameters
typedef struct {
    int row;
    int col;
} parameters;

void *check_row(void *param) {
    int row_flags[SIZE] = {0};
    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[((parameters *)param)->row][i];
        if (row_flags[num - 1] == 1) pthread_exit(NULL);
        row_flags[num - 1] = 1;
    }
    results[0] = 1; // Mark row check as valid
    pthread_exit(NULL);
}

void *check_column(void *param) {
    int col_flags[SIZE] = {0};
    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[i][((parameters *)param)->col];
        if (col_flags[num - 1] == 1) pthread_exit(NULL);
        col_flags[num - 1] = 1;
    }
    results[1] = 1; // Mark column check as valid
    pthread_exit(NULL);
}

void *check_subgrid(void *param) {
    int grid_flags[SIZE] = {0};
    int start_row = ((parameters *)param)->row;
    int start_col = ((parameters *)param)->col;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = sudoku[start_row + i][start_col + j];
            if (grid_flags[num - 1] == 1) pthread_exit(NULL);
            grid_flags[num - 1] = 1;
        }
    }
    results[2 + start_row / 3 * 3 + start_col / 3] = 1; // Mark sub-grid check as valid
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <sudoku_file>\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Unable to open the file\n");
        return 1;
    }
    
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            fscanf(file, "%d", &sudoku[i][j]);
    fclose(file);
    
    pthread_t threads[11];
    parameters row_param = {0, 0};
    parameters col_param = {0, 0};
    pthread_create(&threads[0], NULL, check_row, &row_param);
    pthread_create(&threads[1], NULL, check_column, &col_param);
    
    int index = 2;
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            parameters *param = malloc(sizeof(parameters));
            param->row = i;
            param->col = j;
            pthread_create(&threads[index++], NULL, check_subgrid, param);
        }
    }
    
    for (int i = 0; i < 11; i++)
        pthread_join(threads[i], NULL);
    
    int valid = 1;
    for (int i = 0; i < 11; i++) {
        if (results[i] == 0) {
            valid = 0;
            break;
        }
    }
    
    printf("Sudoku puzzle is %s\n", valid ? "valid" : "invalid");
    return 0;
}