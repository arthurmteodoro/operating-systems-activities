#include <stdio.h>
#include <stdlib.h>

typedef int(*FUNC_PTR)(double**, int, int);

int check_diagonal_matrix(double** matrix, int row, int col) {
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            if (i != j)
                if (matrix[i][j] != 0)
                    return 0;
        }
    }
    return 1;
}

int check_triangular_lower_matrix(double** matrix, int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (j < i) {
                if (matrix[i][j] != 0)
                    return 0;
            }
        }
    }
    return 1;
}

int check_triangular_upper_matrix(double** matrix, int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (i < j) {
                if (matrix[i][j] != 0)
                    return 0;
            }
        }
    }
    return 1;
}

double** create_matrix(int row, int col) {
    double* mat = (double*) malloc(sizeof(double)*(row*col));
    for(int i = 0; i < (row*col); i++) {
        mat[i] = i;
    }
    double **rows_lookup = (double**) malloc(sizeof(double*)*row);
    for (int i = 0; i < row; i++) {
        rows_lookup[i] = &mat[i*col];
    }

    return rows_lookup;
}

void destroy_matrix(double** matrix) {
    free(matrix[0]);
    free(matrix);
}

void insert_values_matrix(double** matrix, int row, int col) {
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {
            double value;
            printf("Insert value of M[%d][%d]: ", i, j);
            scanf("%lf", &value);
            matrix[i][j] = value;
        }
    }
}

void calculate(FUNC_PTR func, double** matrix, int row, int col) {
    int res = func(matrix, row, col);

    if(res)
        printf("Matrix is of selected type");
    else
        printf("Not of selected type");
}

void print_matrix(double** matrix, int row, int col) {
    double* mat = matrix[0];
    for(int i = 0; i < row*col; i++) {
        if (i % col == 0) printf("\n");
        printf("%f ", mat[i]);
    }
}

int main() {
    int matrix_rows = 0;
    int matrix_cols = 0;
    double** matrix;

    printf("Please insert number of rows: ");
    scanf("%d", &matrix_rows);
    printf("Please insert number of cols: ");
    scanf("%d", &matrix_cols);

    matrix = create_matrix(matrix_rows, matrix_cols);
    insert_values_matrix(matrix, matrix_rows, matrix_cols);

    FUNC_PTR functions[3];
    functions[0] = check_diagonal_matrix;
    functions[1] = check_triangular_lower_matrix;
    functions[2] = check_triangular_upper_matrix;

    int option;
    printf("1. Check if matrix is diagonal\n");
    printf("2. Check if matrix is triangular lower\n");
    printf("3. Check if matrix is triangular upper\n");
    printf("Please, select the option: ");
    scanf("%d", &option);

    calculate(functions[option-1], matrix, matrix_rows, matrix_cols);

    print_matrix(matrix, matrix_rows, matrix_cols);

    destroy_matrix(matrix);
    return 0;
}