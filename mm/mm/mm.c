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
    
    int comp = 10;
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

int _convert_two_to_one_dimension(int size, float **mt, float **pmt)
{
    float *res = (float*)malloc(size * size * sizeof(float));
    if (res == NULL) {
        printf("There is not enough memory!\n");
        return -1;
    }

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            res[i*size + j] = mt[i][j];

    *pmt = res;

    return 0;
}

void convert_one_to_two_dimension_with(int size, FLOAT *in, FLOAT **out)
{
    for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
        out[i][j] = in[i*size + j];
}

void sum(FLOAT **from, FLOAT **to, int size)
{
    for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
        to[i][j] += from[i][j];
}

void tozerator(FLOAT **mt, int size)
{
    for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
        mt[i][j] = 0;
}

int basic_multiplication(boolean is_generate, boolean is_print,
    boolean is_compare, boolean is_save, boolean is_cuda,
    char* a_path, char* b_path, char* c_path, char* e_path,
    int size_a1, int size_b1, int size_a2, int size_b2)
{
    FLOAT **a, **b, **c;

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    printf("%d: basic_multiplication.\n", id);
    if (id != 0)
        goto one;
    
    printf("%d: basic_multiplication after goto.\n", id);
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

one:
//    int id = 0;
    if (is_cuda) {
//        if (cuda_mm(a, b, c, size_a1) != 0) {
//            printf("Troubles with CUDA multiplication.\n");
//            return -1;
//        }
    } else if (0) {
        mm(a, b, c, size_a1, size_b1, size_b2);
    } else if (0) {
        int side = 640;
        int matrix_on_side = (size_a1 / side);
        int matrix_count = matrix_on_side * matrix_on_side;

        FLOAT **pa, **pb, **pc;
        pa = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
        pb = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
        pc = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
        if (pa == NULL || pb == NULL || pc == NULL) {
            //TODO: Memory leaks in this case
            printf("There is not enough memory.\n");
            return -1;
        }

        int matrix_number = 0;
        for (int i = 0; i < matrix_on_side; i++) {
            for (int j = 0; j < matrix_on_side; j++) {
                if ((get_one_dimension_matrix(i, j, side, a, &pa[matrix_number]) != 0) ||
                    (get_one_dimension_matrix(i, j, side, b, &pb[matrix_number++]) != 0)) {
                    //TODO: Memory leaks in case of error
                    printf("Error during conversation.\n");
                    return -1;
                }
            }
        }

        FLOAT ****cca = (FLOAT****)malloc(matrix_on_side * sizeof(FLOAT***));
        FLOAT ****ccb = (FLOAT****)malloc(matrix_on_side * sizeof(FLOAT***));
        FLOAT ****ccc = (FLOAT****)malloc(matrix_on_side * sizeof(FLOAT***));
        if (cca == NULL || ccb == NULL || ccc == NULL) {
            //TODO: Memory leaks
            return -1;
        }

        for (int i = 0; i < matrix_on_side; i++) {
            cca[i] = (FLOAT***)malloc(matrix_on_side * sizeof(FLOAT**));
            ccb[i] = (FLOAT***)malloc(matrix_on_side * sizeof(FLOAT**));
            ccc[i] = (FLOAT***)malloc(matrix_on_side * sizeof(FLOAT**));
            if (cca[i] == NULL || ccb[i] == NULL || ccc[i] == NULL)
                return -1;
        }

        for (int i = 0; i < matrix_on_side; i++) {
            for (int j = 0; j < matrix_on_side; j++) {
                if ((allocate_matrix(&cca[i][j], side, side) != 0) ||
                    (allocate_matrix(&ccb[i][j], side, side) != 0) || 
                    (allocate_matrix(&ccc[i][j], side, side) != 0)) {
                    //TODO: Memory leaks
                    printf("Cannot allocate 2D matrixes.\n");
                    return -1;
                }
            }
        }

        int number = 0;
        for (int i = 0; i < matrix_on_side; i++) {
            for (int j = 0; j < matrix_on_side; j++) {
                convert_one_to_two_dimension_with(side, pa[number], cca[i][j]);
                convert_one_to_two_dimension_with(side, pb[number++], ccb[i][j]);
            }
        }

        FLOAT **tt;
        if (allocate_matrix(&tt, side, side) != 0) {
            //TODO: Memory leaks
            return -1;
        }

        for (int i = 0; i < matrix_on_side; i++) {
            for (int j = 0; j < matrix_on_side; j++) {
                for (int k = 0; k < matrix_on_side; k++) {
                    mm(cca[i][k], ccb[k][j], tt, side, side, side);
                    sum(tt, ccc[i][j], side);
                    tozerator(tt, side);
                }
            }
        }

        for (int i = 0; i < side * matrix_on_side; i++)
        for (int j = 0; j < side * matrix_on_side; j++) {
            if (i < side) {
                c[i][j] = (j < side) ? ccc[0][0][i][j] : ccc[0][1][i][j - side];
            }
            else {
                c[i][j] = (j < side) ? ccc[1][0][i - side][j] : ccc[1][1][i - side][j - side];
            }
        }
    } else {
//mpimpi
        int side = 640;//2;//640;
        int matrix_on_side = (1280 / side);//(4/*1280*/ / side);
        int matrix_count = matrix_on_side * matrix_on_side;

        int processId, processCount;
        MPI_Comm_rank(MPI_COMM_WORLD, &processId);
        MPI_Comm_size(MPI_COMM_WORLD, &processCount);
        MPI_Status status;

        id = processId;
        if (processId == 0) {
            printf("processId == 0\n");
            FLOAT **pa, **pb, **pc;
            pa = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
            pb = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
            pc = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
            if (pa == NULL || pb == NULL || pc == NULL) {
                //TODO: Memory leaks in this case
                printf("There is not enough memory.\n");
                return -1;
            }

            int matrix_number = 0;
            for (int i = 0; i < matrix_on_side; i++) {
                for (int j = 0; j < matrix_on_side; j++) {
                    if ((get_one_dimension_matrix(i, j, side, a, &pa[matrix_number]) != 0) ||
                        (get_one_dimension_matrix(i, j, side, b, &pb[matrix_number++]) != 0)) {
                        //TODO: Memory leaks in case of error
                        printf("Error during conversation.\n");
                        return -1;
                    }
                }
            }

            int mtype = 1;
//            float *ms = (float*)malloc(20*sizeof(float));
//            for (int i = 0; i < 20; i++)
//                ms[i] = i;
            printf("0 matrix_count = %d, side = %d\n", matrix_count, side);
            for (int destination = 1; destination < processCount; destination++) {
                for (int i = 0; i < matrix_count; i++) {
                    //printf("============== 0 SEND: %f %f %f %f\n", ms[i*matrix_count], ms[i*matrix_count+1], ms[i*matrix_count+2], ms[i*matrix_count+3]);
                    //MPI_Send(&ms[i*matrix_count], 4, MPI_FLOAT, destination, mtype, MPI_COMM_WORLD);
                    MPI_Send(pa[i], side * side, MPI_FLOAT, destination, mtype, MPI_COMM_WORLD);
//                    printf("============== 0 SEND: pa %d: %f %f %f %f\n", i, pa[i][0], pa[i][1], pa[i][2], pa[i][3]);
                    MPI_Send(pb[i], side * side, MPI_FLOAT, destination, mtype, MPI_COMM_WORLD);
//                    printf("============== 0 SEND: pb %d: %f %f %f %f\n", i, pb[i][0], pb[i][1], pb[i][2], pb[i][3]);
                }
            }

            printf("0 side = %d, matrix_count = %d\n", side, matrix_count);
            for(int i = 0; i < matrix_count; i++)
                pc[i] = (FLOAT *)malloc(side * side * sizeof(FLOAT));

            int source = 1;
            mtype = 2;
            for (int i = 0; i < matrix_count; i++) {
                MPI_Recv(pc[i], side * side, MPI_FLOAT, source, mtype, MPI_COMM_WORLD, &status);
//                printf("============== 0 RECEIVE: pc %d: %f %f %f %f\n", i, pc[i][0], pc[i][1], pc[i][2], pc[i][3]);
                //TODO: Verify status
            }

            FLOAT ****ccc = (FLOAT****)malloc(matrix_on_side * sizeof(FLOAT***));
            if (ccc == NULL)
                return -1;

            for (int i = 0; i < matrix_on_side; i++) {
                ccc[i] = (FLOAT***)malloc(matrix_on_side * sizeof(FLOAT**));
                if (ccc[i] == NULL)
                    return -1;
            }

            for (int i = 0; i < matrix_on_side; i++) {
                for (int j = 0; j < matrix_on_side; j++) {
                    if (allocate_matrix(&ccc[i][j], side, side) != 0) {
                        //TODO: Memory leaks
                        printf("Cannot allocate 2D matrixes.\n");
                        return -1;
                    }
                }
            }

            int number = 0;
            for (int i = 0; i < matrix_on_side; i++) {
                for (int j = 0; j < matrix_on_side; j++) {
                    convert_one_to_two_dimension_with(side, pc[number++], ccc[i][j]);
                }
            }

            for (int i = 0; i < side * matrix_on_side; i++)
            for (int j = 0; j < side * matrix_on_side; j++) {
                if (i < side) {
                    c[i][j] = (j < side) ? ccc[0][0][i][j] : ccc[0][1][i][j - side];
                } else {
                    c[i][j] = (j < side) ? ccc[1][0][i - side][j] : ccc[1][1][i - side][j - side];
                }
            }

        } else if (processId > 0) {
            printf("1 ============== 1 processId > 0\n");
            FLOAT **pa, **pb, **pc;
            pa = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
            pb = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
            pc = (FLOAT **)calloc(matrix_count, sizeof(FLOAT*));
            printf("1 ============== 2 processId > 0\n");
            if (pa == NULL || pb == NULL || pc == NULL) {
                //TODO: Memory leaks in this case
                printf("There is not enough memory.\n");
                return -1;
            }
            printf("1 matrix_count %d ============== 3 processId > 0\n", matrix_count);

            int source = 0, mtype = 1;

            float *ms = (float*)malloc(20*sizeof(float));
            for (int i = 0; i < matrix_count; i++) {
                //TODO: Memory verifying
//                printf("============== 0 in for ac MPI_Recv\n");
                pa[i] = (FLOAT *)calloc(side * side, sizeof(FLOAT));
                pb[i] = (FLOAT *)calloc(side * side, sizeof(FLOAT));
                pc[i] = (FLOAT *)calloc(side * side, sizeof(FLOAT));

//                printf("============== 1 in for ac MPI_Recv: %d\n", i);
                int j = 0;
                //MPI_Recv(&ms[i*matrix_count], 4, MPI_FLOAT, source, mtype, MPI_COMM_WORLD, &status);
                //printf("============== 1 RECEIVE: %f %f %f %f\n", ms[i*matrix_count], ms[i*matrix_count+1], ms[i*matrix_count+2], ms[i*matrix_count+3]);
                MPI_Recv(pa[i], side * side, MPI_FLOAT, source, mtype, MPI_COMM_WORLD, &status);
//                printf("============== 1 RECEIVE: pa %d: %f %f %f %f\n", i, pa[i][0], pa[i][1], pa[i][2], pa[i][3]);
                MPI_Recv(pb[i], side * side, MPI_FLOAT, source, mtype, MPI_COMM_WORLD, &status);
//                printf("============== 1 RECEIVE: pb %d: %f %f %f %f\n", i, pb[i][0], pb[i][1], pb[i][2], pb[i][3]);
            }

            FLOAT ****cca = (FLOAT****)malloc(matrix_on_side * sizeof(FLOAT***));
            FLOAT ****ccb = (FLOAT****)malloc(matrix_on_side * sizeof(FLOAT***));
            FLOAT ****ccc = (FLOAT****)malloc(matrix_on_side * sizeof(FLOAT***));
            if (cca == NULL || ccb == NULL || ccc == NULL) {
                //TODO: Memory leaks
                return -1;
            }

            for (int i = 0; i < matrix_on_side; i++) {
                cca[i] = (FLOAT***)malloc(matrix_on_side * sizeof(FLOAT**));
                ccb[i] = (FLOAT***)malloc(matrix_on_side * sizeof(FLOAT**));
                ccc[i] = (FLOAT***)malloc(matrix_on_side * sizeof(FLOAT**));
                if (cca[i] == NULL || ccb[i] == NULL || ccc[i] == NULL)
                    return -1;
            }

            for (int i = 0; i < matrix_on_side; i++) {
                for (int j = 0; j < matrix_on_side; j++) {
                    if ((allocate_matrix(&cca[i][j], side, side) != 0) ||
                        (allocate_matrix(&ccb[i][j], side, side) != 0) || 
                        (allocate_matrix(&ccc[i][j], side, side) != 0)) {
                        //TODO: Memory leaks
                        printf("Cannot allocate 2D matrixes.\n");
                        return -1;
                    }
                }
            }

            int number = 0;
            for (int i = 0; i < matrix_on_side; i++) {
                for (int j = 0; j < matrix_on_side; j++) {
                    convert_one_to_two_dimension_with(side, pa[number], cca[i][j]);
                    convert_one_to_two_dimension_with(side, pb[number++], ccb[i][j]);
                }
            }

            FLOAT **tt;
            if (allocate_matrix(&tt, side, side) != 0) {
                //TODO: Memory leaks
                return -1;
            }

            for (int i = 0; i < matrix_on_side; i++) {
                for (int j = 0; j < matrix_on_side; j++) {
                    for (int k = 0; k < matrix_on_side; k++) {
                        mm(cca[i][k], ccb[k][j], tt, side, side, side);
                        sum(tt, ccc[i][j], side);
                        tozerator(tt, side);
                    }
                }
            }

            int destination = 0;
            int number1 = 0;
            mtype = 2;
            for (int i = 0; i < matrix_on_side; i++)
            for (int j = 0; j < matrix_on_side; j++) {
                _convert_two_to_one_dimension(side, ccc[i][j], &pc[number1]);
                MPI_Send(pc[number1], side * side, MPI_FLOAT, destination, mtype, MPI_COMM_WORLD);
//                printf("============== 1 SEND: pc %d: %f %f %f %f\n", number1, pc[number1][0], pc[number1][1], pc[number1][2], pc[number1][3]);
                number1++;
            }

//        for (int i = 0; i < side * matrix_on_side; i++)
//        for (int j = 0; j < side * matrix_on_side; j++) {
//            if (i < side) {
//                c[i][j] = (j < side) ? ccc[0][0][i][j] : ccc[0][1][i][j - side];
//            }
//            else {
//                c[i][j] = (j < side) ? ccc[1][0][i - side][j] : ccc[1][1][i - side][j - side];
//            }
//        }
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
if (id == 0) {
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
}
//    free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);

    return 0;
}

int main(int argc, char **argv)
{
    double start, end;
    MPI_Init(&argc, &argv);

    int processId, processCount;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
 
    printf("%d: This is a native C program. (%d)\n", processId, processCount);

#if 0
    if ((generate_matrix_to_file(PATH_TO_MATRIX_A, 4, 4) != 0) ||
        generate_matrix_to_file(PATH_TO_MATRIX_B, 1280, 1280) != 0) {
        return -1;
    }
#endif

    //cuda_get_and_print_info();
    //system("pause");
    //return 0;

//    start = GetTickCount();

    if (basic_multiplication(FALSE, FALSE, TRUE, FALSE, FALSE,
        PATH_TO_MATRIX_A, PATH_TO_MATRIX_B, PATH_TO_MATRIX_D, PATH_TO_MATRIX_C,
        SIZE_A1, SIZE_B1, SIZE_A2, SIZE_B2) != 0) {
        printf("Error during multiplication.\n");
        system("pause");
        return -1;
    }
//    end = GetTickCount();
//    printf("Time of valuation: %f ms\n", end - start);

    MPI_Finalize();
//    system("pause");
    return 0;
}
