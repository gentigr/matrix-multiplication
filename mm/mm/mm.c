#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <Windows.h>

#define FROM_FILE TRUE

#define PATH_TO_MATRIX_A "matrix_a.txt"
#define PATH_TO_MATRIX_B "matrix_b.txt"
#define PATH_TO_MATRIX_C "matrix_c.txt"

#define SIZE_A1 100
#define SIZE_B1 400
#define SIZE_A2 400
#define SIZE_B2 200

#define MAX_ELEMENT 1000

void generate_matrix(float **a, int size_a, int size_b)
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < size_a; i++) {
		for (int j = 0; j < size_b; j++) {
			a[i][j] = (float)((rand() % MAX_ELEMENT + 1) / 10.);
		}
	}
}

int allocate_matrix(float ***mt, int size_a, int size_b)
{
	int i, j;
	float **a;
	a = (float**)malloc(size_a * sizeof(float*));
	if (a == NULL) {
		return -1;
	}
	for (i = 0; i < size_a; i++) {
		a[i] = (float*)malloc(size_b * sizeof(float));
		if (a[i] == NULL) {
			for (j = i; j > 0; j--)
				free(a[j]);
			free(a);
		}
	}

	*mt = a;
	return 0;
}

void free_matrix(float **mt, int size_a)
{
	for (int i = 0; i < size_a; i++) {
		free(mt[i]);
	}
	free(mt);
}

void print_matrix(float **a, int size_a, int size_b)
{
	printf("--------------------------------------------------\n\n");

	for (int i = 0; i < size_a; i++) {
		for (int j = 0; j < size_b; j++)
			printf("%.0f\t", a[i][j]);
		printf("\n");
	}

	printf("--------------------------------------------------\n\n");
}

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

int generate_matrix_to_file(char *path, int row, int column)
{
	float **mt;
	int res = 0;
	if (allocate_matrix(&mt, row, column) != 0)
		return -1;

	generate_matrix(mt, row, column);

	FILE *output = fopen(path, "w");
	if (output == NULL) {
		printf("error during file creation with path = %s\n", path);
		free_matrix(mt, row);
		return -1;
	}

	if (fprintf(output, "%d %d\n", row, column) <= 0) {
		res = -1;
		goto cleanup;
	}

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			if (fprintf(output, "%f ", mt[i][j]) <= 0) {
				res = -1;
				goto cleanup;
			}
		}
		if (fprintf(output, "\n") <= 0) {
			res = -1;
			goto cleanup;
		}
	}

cleanup:
	fclose(output);
	free_matrix(mt, row);

	if (res < 0) {
		printf("error during output matrix to file\n");
		remove(path);
		return -1;
	}

	return 0;
}

boolean compare_matrixes(float **a, int a_row, int a_column,
                         float **b, int b_row, int b_column)
{
    if (a_row != b_row || a_column != b_column)
        return FALSE;

    for (int i = 0; i < a_row; i++) {
        for (int j = 0; j < a_column; j++) {
            if (a[i][j] != b[i][j])
                return FALSE;
        }
    }

    return TRUE;
}

int write_matrix_to_file(char *path, float **mt, int row, int column)
{
    FILE *output = fopen(path, "w");
    char error_message[] = "Error occured during writing to file for " \
                           "saving with path = %s\n";
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
	FILE *input = fopen(path, "r");

	if (fscanf(input, "%d %d\n", &row, &column) < 2) {
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

int multiply_from_file_matrixes(float **a, float **b, float ***pc,
	int size_a1, int size_b1, int size_a2, int size_b2,
	boolean is_print)
{
	float **c;

	if (size_b1 != size_a2) {
		printf("Incorrect rows/column length\n");
		return -1;
	}

	if (allocate_matrix(&c, size_a1, size_b2) != 0) {
		return -1;
	}

	if (is_print) {
		print_matrix(a, size_a1, size_b1);
		print_matrix(b, size_a2, size_b2);
	}

	mm(a, b, c, size_a1, size_b1, size_b2);

	if (is_print) {
		print_matrix(c, size_a1, size_b2);
	}

	*pc = c;

	return 0;
}

int multiply_generated_matrixes(int size_a1, int size_b1, int size_a2, int size_b2, boolean is_print)
{
	float **a, **b, **c;

	if (size_b1 != size_a2) {
		printf("Incorrect rows/column length\n");
		return -1;
	}

	if (allocate_matrixes(&a, &b, &c, size_a1, size_b1, size_a2, size_b2) != 0)
		return -1;

	generate_matrix(a, size_a1, size_b1);
	generate_matrix(b, size_a2, size_b2);

	if (is_print) {
		print_matrix(a, size_a1, size_b1);
		print_matrix(b, size_a2, size_b2);
	}

	mm(a, b, c, size_a1, size_b1, size_b2);

	if (is_print) {
		print_matrix(c, size_a1, size_b2);
	}

	free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);

	return 0;
}

int main()
{
	double start, end;
	int res;
	printf("This is a native C program.\n");

#if 0
	if ((generate_matrix_to_file(PATH_TO_MATRIX_A, 3, 4) != 0) ||
		generate_matrix_to_file(PATH_TO_MATRIX_B, 4, 5) != 0) {
		return -1;
	}
#endif

	start = GetTickCount();
	if (FROM_FILE) {
		float **a, **b, **c;
		int size_a1, size_b1, size_a2, size_b2;
		if ((read_matrix_from_file(PATH_TO_MATRIX_A, &a, &size_a1, &size_b1) != 0) ||
			(read_matrix_from_file(PATH_TO_MATRIX_B, &b, &size_a2, &size_b2) != 0)) {
			return -1;
		}
		res = multiply_from_file_matrixes(a, b, &c, size_a1, size_b1, size_a2, size_b2, TRUE);
        if (res == 0) {
            float **correct_matrix;
            if ((read_matrix_from_file(PATH_TO_MATRIX_C, &correct_matrix, &size_a1, &size_b2) != 0)) {
                res = -1;
            } else {
                if (compare_matrixes(correct_matrix, size_a1, size_b2, c, size_a1, size_b2) != TRUE) {
                    printf("The matrixes are not equal.\n");
                } else {
                    printf("The matrixes are equal.\n");
                    if (write_matrix_to_file(PATH_TO_MATRIX_C, c, size_a1, size_b2) != 0) {
                        printf("Troubles with saving result to file\n");
                        free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);
                        return -1;
                    }
                }
            }
        }
		free_matrixes(a, b, c, size_a1, size_b1, size_a2, size_b2);
	} else {
		res = multiply_generated_matrixes(SIZE_A1, SIZE_B1, SIZE_A2, SIZE_B2, FALSE);
	}
	if (res != 0) {
		printf("Error during multiplication\n");
		return -1;
	}
	end = GetTickCount();
	printf("Time of valuation: %f ms\n", end - start);

	system("pause");
	return 0;
}