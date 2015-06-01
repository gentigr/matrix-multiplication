#define _CRT_SECURE_NO_DEPRECATE

#include "mm.h"

#define FROM_FILE TRUE

#define SIZE_A1 100
#define SIZE_B1 400
#define SIZE_A2 400
#define SIZE_B2 200

#define MAX_ELEMENT 1000

// MATRIX
int allocate_matrix(FLOAT ***mt, int size_a, int size_b)
{
    FLOAT **a = (FLOAT**)calloc(1, size_a * sizeof(FLOAT*));
    if (a == NULL) {
        printf("Not enough memory to allocate for matrix.\n");
        return -1;
    }

    for (int i = 0; i < size_a; i++) {
        a[i] = (FLOAT*)calloc(1, size_b * sizeof(FLOAT));
        if (a[i] == NULL) {
            for (int j = i; j > 0; j--)
                free(a[j]);
            free(a);
        }
    }

    *mt = a;
    return 0;
}

void free_matrix(FLOAT **mt, int size_a)
{
    for (int i = 0; i < size_a; i++)
        free(mt[i]);
    free(mt);
}

void generate_matrix(FLOAT **a, int size_a, int size_b)
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < size_a; i++)
    for (int j = 0; j < size_b; j++)
        a[i][j] = (rand() % MAX_ELEMENT + 1) / 10.f;
}

void print_matrix(FLOAT **a, int size_a, int size_b)
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
int allocate_matrixes(FLOAT ***pa, FLOAT ***pb, FLOAT ***pc,
    int size_a1, int size_b1, int size_a2, int size_b2)
{
    FLOAT **a, **b, **c;

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

void free_matrixes(FLOAT **a, FLOAT **b, FLOAT **c,
    int size_a1, int size_b1, int size_a2, int size_b2)
{
    free_matrix(a, size_a1);
    free_matrix(b, size_a2);
    free_matrix(c, size_a1);
}

boolean compare_matrixes(FLOAT **a, int a_row, int a_column,
    FLOAT **b, int b_row, int b_column)
{
    if (a_row != b_row || a_column != b_column)
        return FALSE;
    
    int comp = 12;
    for (int i = 0; i < a_row; i++)
    for (int j = 0; j < a_column; j++)
    if (!(((a[i][j] + comp) > b[i][j]) && ((a[i][j] - comp) < b[i][j])))
        return FALSE;
 //   if (a[i][j] != b[i][j])
 //       return FALSE;

    return TRUE;
}

int write_matrix_to_file(char *path, FLOAT **mt, int row, int column)
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

int read_matrix_from_file(char *path, FLOAT ***pmt, int *prow, int *pcolumn)
{
    int row, column;
    FLOAT **mt;
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
void mm(FLOAT **a, FLOAT **b, FLOAT **c, int size_a, int size_b, int size_c)
{
    FLOAT sum = 0;

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
    FLOAT **mt;
    if (allocate_matrix(&mt, row, column) != 0)
        return -1;

    generate_matrix(mt, row, column);
    if (write_matrix_to_file(path, mt, row, column) != 0)
        return -1;

    free_matrix(mt, row);
    return 0;
}

int get_one_dimension_matrix(int posX, int posY, int size, FLOAT **in, FLOAT **out)
{
    FLOAT *res = (FLOAT*)malloc(size * size * sizeof(FLOAT));
    if (res == NULL) {
        printf("There is not enough memory!\n");
        return -1;
    }

    int k = 0;
    for (int i = posX * size; i < posX * size + size; i++)
    for (int j = posY * size; j < posY * size + size; j++)
        res[k++] = in[i][j];

    *out = res;
    return 0;
}

int convert_one_to_two_dimension_with(int size, FLOAT *in, FLOAT **out)
{
    for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
        out[i][j] = in[i*size + j];

    return 0;
}

void sum(FLOAT **from, FLOAT **to, int size)
{
    for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
        to[i][j] += from[i][j];
}

int basic_multiplication(boolean is_generate, boolean is_print,
    boolean is_compare, boolean is_save, boolean is_cuda,
    char* a_path, char* b_path, char* c_path, char* e_path,
    int size_a1, int size_b1, int size_a2, int size_b2)
{
    FLOAT **a, **b, **c;

    if (is_generate) {
        if (allocate_matrixes(&a, &b, &c, size_a1, size_b1, size_a2, size_b2) != 0)
            return -1;
        generate_matrix(a, size_a1, size_b1);
        generate_matrix(b, size_a2, size_b2);
        printf("Matrixes are generated.\n");
    } else {
        if ((read_matrix_from_file(a_path, &a, &size_a1, &size_b1) != 0) ||
            (read_matrix_from_file(b_path, &b, &size_a2, &size_b2) != 0)) {
            return -1;
        }
        if (allocate_matrix(&c, size_a1, size_b2) != 0) {
            free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);
            return -1;
        }
        printf("Matrixes are readed.\n");
    }

    if (size_b1 != size_a2) {
        printf("Incorrect rows/column size.\n");
        free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);
        return -1;
    }

    if (is_cuda) {
        if (cuda_mm(a, b, c, size_a1) != 0) {
            printf("Troubles with CUDA multiplication.\n");
            return -1;
        }
    } else if (0) {
        mm(a, b, c, size_a1, size_b1, size_b2);
    } else if (1) {
        int size = 2;
        FLOAT *pa[4], *pb[4], *pc[4];
        if ((get_one_dimension_matrix(0, 0, size_a1 / 2, a, &pa[0]) != 0) ||
            (get_one_dimension_matrix(0, 1, size_a1 / 2, a, &pa[1]) != 0) ||
            (get_one_dimension_matrix(1, 0, size_a1 / 2, a, &pa[2]) != 0) ||
            (get_one_dimension_matrix(1, 1, size_a1 / 2, a, &pa[3]) != 0)) {
            printf("A convertion failed\n");
        }
        if ((get_one_dimension_matrix(0, 0, size_a1 / 2, b, &pb[0]) != 0) ||
            (get_one_dimension_matrix(0, 1, size_a1 / 2, b, &pb[1]) != 0) ||
            (get_one_dimension_matrix(1, 0, size_a1 / 2, b, &pb[2]) != 0) ||
            (get_one_dimension_matrix(1, 1, size_a1 / 2, b, &pb[3]) != 0)) {
            printf("B convertion failed\n");
        }
        const int line = 640;// size_a1 / 2;
        FLOAT **ca[4], **cb[4], **cc[4];
        allocate_matrix(&ca[0], line, line);        allocate_matrix(&cb[0], line, line);        allocate_matrix(&cc[0], line, line);
        allocate_matrix(&ca[1], line, line);        allocate_matrix(&cb[1], line, line);        allocate_matrix(&cc[1], line, line);
        allocate_matrix(&ca[2], line, line);        allocate_matrix(&cb[2], line, line);        allocate_matrix(&cc[2], line, line);
        allocate_matrix(&ca[3], line, line);        allocate_matrix(&cb[3], line, line);        allocate_matrix(&cc[3], line, line);

        //FLOAT ca[4][640][640] = { 0 };// , ca1[640][640], ca2[640][640], ca3[640][640];
        //FLOAT cb[4][640][640] = { 0 };// , cb1[640][640], cb2[640][640], cb3[640][640];
        //FLOAT cc[4][640][640] = { 0 };// , cc1[640][640], cc2[640][640], cc3[640][640];
        convert_one_to_two_dimension_with(size_a1 / 2, pa[0], ca[0]);
        convert_one_to_two_dimension_with(size_a1 / 2, pa[1], ca[1]);
        convert_one_to_two_dimension_with(size_a1 / 2, pa[2], ca[2]);
        convert_one_to_two_dimension_with(size_a1 / 2, pa[3], ca[3]);

        convert_one_to_two_dimension_with(size_a1 / 2, pb[0], cb[0]);
        convert_one_to_two_dimension_with(size_a1 / 2, pb[1], cb[1]);
        convert_one_to_two_dimension_with(size_a1 / 2, pb[2], cb[2]);
        convert_one_to_two_dimension_with(size_a1 / 2, pb[3], cb[3]);

        FLOAT **tmp[8];
        for (int i = 0; i < 8; i++)
            allocate_matrix(&tmp[i], line, line);
//        FLOAT tmp0[640][640], tmp1[640][640], tmp2[640][640], tmp3[640][640], tmp4[640][640], tmp5[640][640], tmp6[640][640], tmp7[640][640];

        mm(ca[0], cb[0], tmp[0], line, line, line);
        mm(ca[1], cb[2], tmp[1], line, line, line);
        mm(ca[0], cb[1], tmp[2], line, line, line);
        mm(ca[1], cb[3], tmp[3], line, line, line);
        mm(ca[2], cb[0], tmp[4], line, line, line);
        mm(ca[3], cb[2], tmp[5], line, line, line);
        mm(ca[2], cb[1], tmp[6], line, line, line);
        mm(ca[3], cb[3], tmp[7], line, line, line);

        sum(tmp[0], cc[0], line);
        sum(tmp[2], cc[1], line);
        sum(tmp[4], cc[2], line);
        sum(tmp[6], cc[3], line);

        sum(tmp[1], cc[0], line);
        sum(tmp[3], cc[1], line);
        sum(tmp[5], cc[2], line);
        sum(tmp[7], cc[3], line);

        for (int i = 0; i < line * 2; i++)
        for (int j = 0; j < line * 2; j++) {
            if (i < line) {
                c[i][j] = (j < line) ? cc[0][i][j] : cc[1][i][j - line];
            } else {
                c[i][j] = (j < line) ? cc[2][i - line][j] : cc[3][i - line][j - line];
            }
        }
    } else {
        int size = 2;
        FLOAT *pa[4], *pb[4], *pc[4];
        if ((get_one_dimension_matrix(0, 0, size_a1 / 2, a, &pa[0]) != 0) ||
            (get_one_dimension_matrix(0, 1, size_a1 / 2, a, &pa[1]) != 0) ||
            (get_one_dimension_matrix(1, 0, size_a1 / 2, a, &pa[2]) != 0) ||
            (get_one_dimension_matrix(1, 1, size_a1 / 2, a, &pa[3]) != 0)) {
            printf("A convertion failed\n");
        }
        if ((get_one_dimension_matrix(0, 0, size_a1 / 2, b, &pb[0]) != 0) ||
            (get_one_dimension_matrix(0, 1, size_a1 / 2, b, &pb[1]) != 0) ||
            (get_one_dimension_matrix(1, 0, size_a1 / 2, b, &pb[2]) != 0) ||
            (get_one_dimension_matrix(1, 1, size_a1 / 2, b, &pb[3]) != 0)) {
            printf("B convertion failed\n");
        }
        const int line = 2;// 640;// size_a1 / 2;
        FLOAT **ca[4], **cb[4], **cc[4];
        allocate_matrix(&ca[0], line, line);        allocate_matrix(&cb[0], line, line);        allocate_matrix(&cc[0], line, line);
        allocate_matrix(&ca[1], line, line);        allocate_matrix(&cb[1], line, line);        allocate_matrix(&cc[1], line, line);
        allocate_matrix(&ca[2], line, line);        allocate_matrix(&cb[2], line, line);        allocate_matrix(&cc[2], line, line);
        allocate_matrix(&ca[3], line, line);        allocate_matrix(&cb[3], line, line);        allocate_matrix(&cc[3], line, line);

        //FLOAT ca[4][640][640] = { 0 };// , ca1[640][640], ca2[640][640], ca3[640][640];
        //FLOAT cb[4][640][640] = { 0 };// , cb1[640][640], cb2[640][640], cb3[640][640];
        //FLOAT cc[4][640][640] = { 0 };// , cc1[640][640], cc2[640][640], cc3[640][640];
        convert_one_to_two_dimension_with(size_a1 / 2, pa[0], ca[0]);
        convert_one_to_two_dimension_with(size_a1 / 2, pa[1], ca[1]);
        convert_one_to_two_dimension_with(size_a1 / 2, pa[2], ca[2]);
        convert_one_to_two_dimension_with(size_a1 / 2, pa[3], ca[3]);

        convert_one_to_two_dimension_with(size_a1 / 2, pb[0], cb[0]);
        convert_one_to_two_dimension_with(size_a1 / 2, pb[1], cb[1]);
        convert_one_to_two_dimension_with(size_a1 / 2, pb[2], cb[2]);
        convert_one_to_two_dimension_with(size_a1 / 2, pb[3], cb[3]);

        FLOAT **tmp[8];
        for (int i = 0; i < 8; i++)
            allocate_matrix(&tmp[i], line, line);
        //        FLOAT tmp0[640][640], tmp1[640][640], tmp2[640][640], tmp3[640][640], tmp4[640][640], tmp5[640][640], tmp6[640][640], tmp7[640][640];

        mm(ca[0], cb[0], tmp[0], line, line, line);
        mm(ca[1], cb[2], tmp[1], line, line, line);
        mm(ca[0], cb[1], tmp[2], line, line, line);
        mm(ca[1], cb[3], tmp[3], line, line, line);
        mm(ca[2], cb[0], tmp[4], line, line, line);
        mm(ca[3], cb[2], tmp[5], line, line, line);
        mm(ca[2], cb[1], tmp[6], line, line, line);
        mm(ca[3], cb[3], tmp[7], line, line, line);

        sum(tmp[0], cc[0], line);
        sum(tmp[2], cc[1], line);
        sum(tmp[4], cc[2], line);
        sum(tmp[6], cc[3], line);

        sum(tmp[1], cc[0], line);
        sum(tmp[3], cc[1], line);
        sum(tmp[5], cc[2], line);
        sum(tmp[7], cc[3], line);

        for (int i = 0; i < line * 2; i++)
        for (int j = 0; j < line * 2; j++) {
            if (i < line) {
                c[i][j] = (j < line) ? cc[0][i][j] : cc[1][i][j - line];
            } else {
                c[i][j] = (j < line) ? cc[2][i - line][j] : cc[3][i - line][j - line];
            }
        }
    }

    if (is_save) {
        printf("Start saving to file procedure.\n");
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
        printf("Start comparing the result with etalon.\n");
        FLOAT **correct_matrix;
        if ((read_matrix_from_file(e_path, &correct_matrix, &size_a1, &size_b2) != 0)) {
            printf("Troubles with reading matrix from file to verify correctness.\n");
        } else {
            if (compare_matrixes(correct_matrix, size_a1, size_b2, c, size_a1, size_b2) != TRUE) {
                printf("The matrixes are not equal.\n");
            } else {
                printf("The matrixes are equal.\n");
            }
        }
    }

    free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);

    return 0;
}

int main()
{
    double start, end;
    printf("This is a native C program.\n");

#if 0
    if ((generate_matrix_to_file(PATH_TO_MATRIX_A, 4, 4) != 0) ||
        generate_matrix_to_file(PATH_TO_MATRIX_B, 4, 4) != 0) {
        return -1;
    }
#endif

    //cuda_get_and_print_info();
    //system("pause");
    //return 0;

    start = GetTickCount();
    if (basic_multiplication(FALSE, FALSE, TRUE, FALSE, FALSE,
        PATH_TO_MATRIX_A, PATH_TO_MATRIX_B, PATH_TO_MATRIX_D, PATH_TO_MATRIX_C,
        SIZE_A1, SIZE_B1, SIZE_A2, SIZE_B2) != 0) {
        printf("Error during multiplication.\n");
        system("pause");
        return -1;
    }
    end = GetTickCount();
    printf("Time of valuation: %f ms\n", end - start);

    system("pause");
    return 0;
}