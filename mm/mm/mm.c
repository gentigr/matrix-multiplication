#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <Windows.h>

#define SIZE_A1 100
#define SIZE_B1 400
#define SIZE_A2 400
#define SIZE_B2 200

#define MAX_ELEMENT 10

void generate_matrix(float **a, int size_a, int size_b)
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < size_a; i++) {
		for (int j = 0; j < size_b; j++) {
			a[i][j] = rand() % MAX_ELEMENT + 1;
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

int basic_multiplication(int size_a1, int size_b1, int size_a2, int size_b2, boolean is_print)
{
	float **a, **b, **c;
	int i, j;

	if (size_b1 != size_a2) {
		printf("Incorrect rows/column length\n");
		return -1;
	}

	if (allocate_matrix(&a, size_a1, size_b1) != 0) {
		return -1;
	} else if (allocate_matrix(&b, size_a2, size_b2) != 0) {
		free_matrix(a, size_a1);
		return -1;
	} else if (allocate_matrix(&c, size_a1, size_b2) != 0) {
		free(a, size_a1);
		free(b, size_a2);
		return -1;
	}

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

	free_matrix(a, size_a1);
	free_matrix(b, size_a2);
	free_matrix(c, size_a1);

	return 0;
}

int main()
{
	double start, end;
	printf("This is a native C program.\n");

	start = GetTickCount();
	if (basic_multiplication(SIZE_A1, SIZE_B1, SIZE_A2, SIZE_B2, FALSE) != 0) {
		printf("Error during multiplication\n");
		return -1;
	}
	end = GetTickCount();
	printf("Time of valuation: %f ms\n", end - start);

	system("pause");
	return 0;
}