#ifndef __MM_H__
#define __MM_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <Windows.h>

#define FLOAT float

#if 0
#define PATH_TO_MATRIX_A "matrix_a_4.txt"
#define PATH_TO_MATRIX_B "matrix_b_4.txt"
#define PATH_TO_MATRIX_C "matrix_c_4.txt"
#define PATH_TO_MATRIX_D "matrix_d_4.txt"
#define STREAM_COUNT     1
#elif 0
#define PATH_TO_MATRIX_A "matrix_a_256.txt"
#define PATH_TO_MATRIX_B "matrix_b_256.txt"
#define PATH_TO_MATRIX_C "matrix_c_256.txt"
#elif 1
#define PATH_TO_MATRIX_A "matrix_a_1280.txt"
#define PATH_TO_MATRIX_B "matrix_b_1280.txt"
#define PATH_TO_MATRIX_C "matrix_c_1280.txt"
#define PATH_TO_MATRIX_D "matrix_d_1280.txt"
#define STREAM_COUNT     (2560/256)
#elif 0
#define PATH_TO_MATRIX_A "matrix_a_2560.txt"
#define PATH_TO_MATRIX_B "matrix_b_2560.txt"
#define PATH_TO_MATRIX_C "matrix_c_2560.txt"
#define STREAM_COUNT     (2560/256)
#elif 0
#define PATH_TO_MATRIX_A "matrix_a_10000.txt"
#define PATH_TO_MATRIX_B "matrix_b_10000.txt"
#define PATH_TO_MATRIX_C "matrix_c_10000.txt"
#endif

#ifdef __cplusplus
extern "C" {
    int cuda_main();
    int cuda_get_and_print_info();
    int cuda_mm(float **pa, float **pb, float **pc, unsigned int size);
}
#endif
#endif /* __MM_H__ */