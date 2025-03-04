#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

int sudoku[SIZE][SIZE]; // 9x9 Sudoku grid
int results[11] = {0};  // Results array (0 = invalid, 1 = valid)

// Structure for passing parameters to threads
typedef struct {
    int row;
    int col;
} parameters;

// Function to check if a row contains numbers 1-9 exactly once
void* check_row(void* param) {
    parameters* data = (parameters*) param;
    int row = data->row;
    int seen[SIZE + 1] = {0};

    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[row][i];
        if (seen[num]) {
            pthread_exit(NULL);
        }
        seen[num] = 1;
    }
    results[0] = 1;
    pthread_exit(NULL);
}

// Function to check if a column contains numbers 1-9 exactly once
void* check_column(void* param) {
    parameters* data = (parameters*) param;
    int col = data->col;
    int seen[SIZE + 1] = {0};

    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[i][col];
        if (seen[num]) {
            pthread_exit(NULL);
        }
        seen[num] = 1;
    }
    results[1] = 1;
    pthread_exit(NULL);
}

// Function to check if a 3x3 subgrid contains numbers 1-9 exactly once
void* check_subgrid(void* param) {
    parameters* data = (parameters*) param;
    int row = data->row;
    int col = data->col;
    int seen[SIZE + 1] = {0};

    for (int i = row; i < row + 3; i++) {
        for (int j = col; j < col + 3; j++) {
            int num = sudoku[i][j];
            if (seen[num]) {
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    results[2 + (row / 3) * 3 + (col / 3)] = 1;
    pthread_exit(NULL);
}

// Function to read Sudoku grid from a file
int read_sudoku_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Unable to open the file\n");
        return 0;
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (fscanf(file, "%d", &sudoku[i][j]) != 1) {
                fclose(file);
                return 0;
            }
        }
    }

    fclose(file);
    return 1;
}

// Function to print Sudoku grid in the expected format
void print_sudoku_grid() {
    printf("Sudoku Puzzle Solution is:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: ./sudoku <sudoku_grid>\n");
        return 1;
    }

    if (!read_sudoku_from_file(argv[1])) {
        return 1;
    }

    print_sudoku_grid(); // Ensure the grid prints before validation

    pthread_t threads[11];
    parameters row_param = {0, 0};
    parameters col_param = {0, 0};

    // Create threads for row and column validation
    pthread_create(&threads[0], NULL, check_row, &row_param);
    pthread_create(&threads[1], NULL, check_column, &col_param);

    // Create threads for each 3x3 subgrid
    int thread_idx = 2;
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            parameters* grid_param = (parameters*) malloc(sizeof(parameters));
            grid_param->row = i;
            grid_param->col = j;
            pthread_create(&threads[thread_idx], NULL, check_subgrid, grid_param);
            thread_idx++;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < 11; i++) {
        pthread_join(threads[i], NULL);
    }

    // Check results array to determine validity
    int valid = 1;
    for (int i = 0; i < 11; i++) {
        if (results[i] == 0) {
            valid = 0;
            break;
        }
    }

    if (valid) {
        printf("Sudoku puzzle is valid\n");
    } else {
        printf("Sudoku puzzle is invalid\n");
    }

    return 0;
}
