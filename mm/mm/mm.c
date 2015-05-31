#define _CRT_SECURE_NO_DEPRECATE

#include "mm.h"

#define FROM_FILE TRUE

#define PATH_TO_MATRIX_A "matrix_a.txt"
#define PATH_TO_MATRIX_B "matrix_b.txt"
#define PATH_TO_MATRIX_C "matrix_c.txt"

#define SIZE_A1 100
#define SIZE_B1 400
#define SIZE_A2 400
#define SIZE_B2 200

#define MAX_ELEMENT 1000

// MATRIX
int allocate_matrix(float ***mt, int size_a, int size_b)
{
    float **a = (float**)malloc(size_a * sizeof(float*));
    if (a == NULL) {
        printf("Not enough memory to allocate for matrix.\n");
        return -1;
    }

    for (int i = 0; i < size_a; i++) {
        a[i] = (float*)malloc(size_b * sizeof(float));
        if (a[i] == NULL) {
            for (int j = i; j > 0; j--)
                free(a[j]);
            free(a);
        }
    }

    *mt = a;
    return 0;
}

void free_matrix(float **mt, int size_a)
{
    for (int i = 0; i < size_a; i++)
        free(mt[i]);
    free(mt);
}

void generate_matrix(float **a, int size_a, int size_b)
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < size_a; i++)
    for (int j = 0; j < size_b; j++)
        a[i][j] = (rand() % MAX_ELEMENT + 1) / 10.f;
}

void print_matrix(float **a, int size_a, int size_b)
{
    printf("--------------------------------------------------\n\n");
    for (int i = 0; i < size_a; i++) {
        for (int j = 0; j < size_b; j++)
            printf("%.1f\t", a[i][j]);
        printf("\n");
    }
    printf("--------------------------------------------------\n\n");
}

// MATRIXES
int allocate_matrixes(float ***pa, float ***pb, float ***pc,
    int size_a1, int size_b1, int size_a2, int size_b2)
{
    float **a, **b, **c;

    if (allocate_matrix(&a, size_a1, size_b1) != 0) {
        return -1;
    }
    else if (allocate_matrix(&b, size_a2, size_b2) != 0) {
        free_matrix(a, size_a1);
        return -1;
    }
    else if (allocate_matrix(&c, size_a1, size_b2) != 0) {
        free_matrix(a, size_a1);
        free_matrix(b, size_a2);
        return -1;
    }

    *pa = a;
    *pb = b;
    *pc = c;

    return 0;
}

void free_matrixes(float **a, float **b, float **c,
    int size_a1, int size_b1, int size_a2, int size_b2)
{
    free_matrix(a, size_a1);
    free_matrix(b, size_a2);
    free_matrix(c, size_a1);
}

boolean compare_matrixes(float **a, int a_row, int a_column,
    float **b, int b_row, int b_column)
{
    if (a_row != b_row || a_column != b_column)
        return FALSE;

    for (int i = 0; i < a_row; i++)
    for (int j = 0; j < a_column; j++)
    if (a[i][j] != b[i][j])
        return FALSE;

    return TRUE;
}

int write_matrix_to_file(char *path, float **mt, int row, int column)
{
    char error_message[] = "Error occured during writing to file for " \
        "saving with path = %s\n";

    FILE *output = fopen(path, "w");
    if (output == NULL) {
        printf("Error occured during creation file for saving " \
            "with path = %s\n", path);
        return -1;
    }

    if (fprintf(output, "%d %d\n", row, column) < 2) {
        printf(error_message, path);
        fclose(output);
        return -1;
    }

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (fprintf(output, "%f ", mt[i][j]) < 1) {
                printf(error_message, path);
                fclose(output);
                return -1;
            }
        }
        if (fprintf(output, "\n") < 1) {
            printf(error_message, path);
            fclose(output);
            return -1;
        }
    }

    fclose(output);
    return 0;
}

int read_matrix_from_file(char *path, float ***pmt, int *prow, int *pcolumn)
{
    int row, column;
    float **mt;
    char error_message[] = "Error occured during reading from file with path = %s\n";

    if (path == NULL || pmt == NULL || prow == NULL || pcolumn == NULL) {
        printf("Incorrect input value.\n");
        return -1;
    }

    FILE *input = fopen(path, "r");
    if (input == NULL) {
        printf(error_message, path);
        return -1;
    }

    if (fscanf(input, "%d %d", &row, &column) < 2) {
        printf(error_message, path);
        fclose(input);
        return -1;
    }

    if (allocate_matrix(&mt, row, column) != 0) {
        fclose(input);
        return -1;
    }

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (fscanf(input, "%f", &mt[i][j]) < 1) {
                printf(error_message, path);
                fclose(input);
                free_matrix(mt, row);
                *pmt = NULL;
                return -1;
            }
        }
    }

    fclose(input);
    *pmt = mt;
    *prow = row;
    *pcolumn = column;

    return 0;
}

// MULTIPLICATION
void mm(float **a, float **b, float **c, int size_a, int size_b, int size_c)
{
    float sum = 0;

    for (int i = 0; i < size_a; i++) {
        for (int j = 0; j < size_c; j++) {
            for (int k = 0; k < size_b; k++) {
                sum = sum + a[i][k] * b[k][j];
            }
            c[i][j] = sum;
            sum = 0;
        }
    }
}

int generate_matrix_to_file(char *path, int row, int column)
{
    float **mt;
    if (allocate_matrix(&mt, row, column) != 0)
        return -1;

    generate_matrix(mt, row, column);
    if (write_matrix_to_file(path, mt, row, column) != 0)
        return -1;

    free_matrix(mt, row);
    return 0;
}

int basic_multiplication(boolean is_generate, boolean is_print,
    boolean is_compare, boolean is_save,
    char* a_path, char* b_path, char* c_path, char* e_path,
    int size_a1, int size_b1, int size_a2, int size_b2)
{
    float **a, **b, **c;

    if (is_generate) {
        if (allocate_matrixes(&a, &b, &c, size_a1, size_b1, size_a2, size_b2) != 0)
            return -1;
        generate_matrix(a, size_a1, size_b1);
        generate_matrix(b, size_a2, size_b2);
    }
    else {
        if ((read_matrix_from_file(a_path, &a, &size_a1, &size_b1) != 0) ||
            (read_matrix_from_file(b_path, &b, &size_a2, &size_b2) != 0)) {
            return -1;
        }
        if (allocate_matrix(&c, size_a1, size_b2) != 0) {
            free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);
            return -1;
        }
    }

    if (size_b1 != size_a2) {
        printf("Incorrect rows/column size.\n");
        free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);
        return -1;
    }

    mm(a, b, c, size_a1, size_b1, size_b2);

    if (is_save) {
        if (write_matrix_to_file(c_path, c, size_a1, size_b2) != 0) {
            printf("Troubles with saving result to file.\n");
            return -1;
        }
    }

    if (is_print) {
        print_matrix(a, size_a1, size_b1);
        print_matrix(b, size_a2, size_b2);
        print_matrix(c, size_a1, size_b2);
    }

    if (is_compare) {
        float **correct_matrix;
        if ((read_matrix_from_file(e_path, &correct_matrix, &size_a1, &size_b2) != 0)) {
            printf("Troubles with reading matrix from file to verify correctness.\n");
        }
        else {
            if (compare_matrixes(correct_matrix, size_a1, size_b2, c, size_a1, size_b2) != TRUE) {
                printf("The matrixes are not equal.\n");
            }
            else {
                printf("The matrixes are equal.\n");
            }
        }
    }

    free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);

    return 0;
}

//#include "kernel.cu"

int main()
{
    double start, end;
    printf("This is a native C program.\n");

#if 0
    if ((generate_matrix_to_file(PATH_TO_MATRIX_A, 3, 4) != 0) ||
        generate_matrix_to_file(PATH_TO_MATRIX_B, 4, 5) != 0) {
        return -1;
    }
#endif

    start = GetTickCount();
    if (basic_multiplication(FALSE, FALSE, TRUE, TRUE,
        PATH_TO_MATRIX_A, PATH_TO_MATRIX_B, PATH_TO_MATRIX_C, PATH_TO_MATRIX_C,
        SIZE_A1, SIZE_B1, SIZE_A2, SIZE_B2) != 0) {
        printf("Error during multiplication.\n");
        system("pause");
        return -1;
    }
    end = GetTickCount();
    printf("Time of valuation: %f ms\n", end - start);

    int k = cuda_main();
    int l = cuda_get_and_print_info();
    system("pause");
    return 0;
}