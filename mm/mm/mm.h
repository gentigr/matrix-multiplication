#ifndef __MM_H__
#define __MM_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
    int cuda_main();
    int cuda_get_and_print_info();
    int cuda_mm(float **pa, float **pb, float **pc, unsigned int size);
}
#endif
#endif /* __MM_H__ */