#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "mm.h"

cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size);

__global__ void addKernel(int *c, const int *a, const int *b)
{
    int i = threadIdx.x;
    c[i] = a[i] + b[i];
}

int cuda_main()
{
    const int arraySize = 5;
    const int a[arraySize] = { 1, 2, 3, 4, 5 };
    const int b[arraySize] = { 10, 20, 30, 40, 50 };
    int c[arraySize] = { 0 };

    // Add vectors in parallel.
    cudaError_t cudaStatus = addWithCuda(c, a, b, arraySize);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addWithCuda failed!");
        return 1;
    }

    printf("{1,2,3,4,5} + {10,20,30,40,50} = {%d,%d,%d,%d,%d}\n",
        c[0], c[1], c[2], c[3], c[4]);

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }

    return 0;
}

// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size)
{
    int *dev_a = 0;
    int *dev_b = 0;
    int *dev_c = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Allocate GPU buffers for three vectors (two input, one output)    .
    cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Copy input vectors from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    addKernel << <1, size >> >(dev_c, dev_a, dev_b);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

Error:
    cudaFree(dev_c);
    cudaFree(dev_a);
    cudaFree(dev_b);

    return cudaStatus;
}

void cuda_clear_info(cudaDeviceProp *cdp)
{
    free(cdp);
}

int cuda_get_info(int *pDeviceCount, cudaDeviceProp **pDeviceProp)
{
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    cudaDeviceProp *cdp = (cudaDeviceProp *)malloc(deviceCount * sizeof(cudaDeviceProp));
    if (cdp == NULL) {
        printf("There is not enough memory for getting device information.\n");
        return -1;
    }

    for (int i = 0; i < deviceCount; i++)
        cudaGetDeviceProperties(&cdp[i], i);

    *pDeviceCount = deviceCount;
    *pDeviceProp = cdp;

    return 0;
}

int cuda_print_info(int deviceCount, cudaDeviceProp *deviceProp)
{
    printf("Device count: %d\n\n", deviceCount);

    for (int i = 0; i < deviceCount; i++)
    {
        printf("Device name: %s\n", deviceProp[i].name);
        printf("Total global memory: %Iu\n", deviceProp[i].totalGlobalMem);
        printf("Shared memory per block: %Iu\n", deviceProp[i].sharedMemPerBlock);
        printf("Registers per block: %d\n", deviceProp[i].regsPerBlock);
        printf("Warp size: %d\n", deviceProp[i].warpSize);
        printf("Memory pitch: %Iu\n", deviceProp[i].memPitch);
        printf("Max threads per block: %d\n", deviceProp[i].maxThreadsPerBlock);

        printf("Max threads dimensions: x = %d, y = %d, z = %d\n",
            deviceProp[i].maxThreadsDim[0],
            deviceProp[i].maxThreadsDim[1],
            deviceProp[i].maxThreadsDim[2]);

        printf("Max grid size: x = %d, y = %d, z = %d\n",
            deviceProp[i].maxGridSize[0],
            deviceProp[i].maxGridSize[1],
            deviceProp[i].maxGridSize[2]);

        printf("Clock rate: %d\n", deviceProp[i].clockRate);
        printf("Total constant memory: %Iu\n", deviceProp[i].totalConstMem);
        printf("Compute capability: %d.%d\n", deviceProp[i].major, deviceProp[i].minor);
        printf("Texture alignment: %d\n", deviceProp[i].textureAlignment);
        printf("Device overlap: %d\n", deviceProp[i].deviceOverlap);
        printf("Multiprocessor count: %d\n", deviceProp[i].multiProcessorCount);

        printf("Kernel execution timeout enabled: %s\n",
            deviceProp[i].kernelExecTimeoutEnabled ? "true" : "false");
    }

    return 0;
}

int cuda_get_and_print_info()
{
    int device_count;
    cudaDeviceProp *cdp;

    if (cuda_get_info(&device_count, &cdp) != 0)
        return -1;

    if (cuda_print_info(device_count, cdp) != 0)
        return -1;

    cuda_clear_info(cdp);

    return 0;
}

cudaError_t mmWithCuda(int device, float *c, const float *a, const float *b, unsigned int size);

#define BLOCK_SIZE 16

__global__ void mmKernel(float * a, float * b, int n, float * c)
{
    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;
    int aBegin = n * BLOCK_SIZE * by;
    int aEnd = aBegin + n - 1;
    int bBegin = BLOCK_SIZE * bx;
    int aStep = BLOCK_SIZE, bStep = BLOCK_SIZE * n;
    float sum = 0.0f;
    for (int ia = aBegin, ib = bBegin; ia <= aEnd; ia += aStep, ib += bStep)
    {
        __shared__ float as[BLOCK_SIZE][BLOCK_SIZE];
        __shared__ float bs[BLOCK_SIZE][BLOCK_SIZE];
        as[ty][tx] = a[ia + n * ty + tx];
        bs[ty][tx] = b[ib + n * ty + tx];
        __syncthreads();
        for (int k = 0; k < BLOCK_SIZE; k++)
            sum += as[ty][k] * bs[k][tx];
        __syncthreads();
    }
    c[n * BLOCK_SIZE * by + BLOCK_SIZE * bx + n * ty + tx] = sum;
}

int convert_two_to_one_dimension(int size, float **mt, float **pmt)
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

int convert_one_to_two_dimension(int size, float *in, float **out)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            out[i][j] = in[i*size + j];

    return 0;
}

int cuda_mm(float **pa, float **pb, float **pc, unsigned int size)
{
    float *a, *b, *c;

    //TODO: Memory leaks
    if ((convert_two_to_one_dimension(size, pa, &a) != 0) ||
        (convert_two_to_one_dimension(size, pb, &b) != 0) ||
        (convert_two_to_one_dimension(size, pc, &c) != 0)) {
        return -1;
    }

    // Add vectors in parallel.
    cudaError_t cudaStatus = mmWithCuda(0, c, a, b, size);
    if (cudaStatus != cudaSuccess) {
        printf("mmWithCuda failed!");
        return 1;
    }

    if (convert_one_to_two_dimension(size, c, pc) != 0) {
        printf("Smth go wrong during convertation!\n");
        return -1;
    }

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        printf("cudaDeviceReset failed!");
        return 1;
    }

    free(a);
    free(b);
    free(c);

    return 0;
}

cudaError_t mmWithCuda(int device, float *c, const float *a, const float *b, unsigned int size)
{
    float * adev, *bdev, *cdev;
    dim3 threads(BLOCK_SIZE, BLOCK_SIZE);
    dim3 blocks(size / threads.x, size / threads.y);
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system
    cudaStatus = cudaSetDevice(device);
    if (cudaStatus != cudaSuccess) {
        printf("cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        return cudaStatus;
    }
    
    // Allocate GPU buffers for three matrixes
    cudaStatus = cudaMalloc((void**)&adev, size * size * sizeof (float));
    if (cudaStatus != cudaSuccess) {
        printf("cudaMalloc failed!");
        return cudaStatus;
    }

    cudaStatus = cudaMalloc((void**)&bdev, size * size * sizeof (float));
    if (cudaStatus != cudaSuccess) {
        printf("cudaMalloc failed!");
        cudaFree(adev);
        return cudaStatus;
    }

    cudaStatus = cudaMalloc((void**)&cdev, size * size * sizeof (float));
    if (cudaStatus != cudaSuccess) {
        printf("cudaMalloc failed!");
        cudaFree(adev);
        cudaFree(bdev);
        return cudaStatus;
    }

    // Copy input matrixes from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(adev, a, size * size * sizeof (float), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        printf("cudaMemcpy failed!");
        cudaFree(adev);
        cudaFree(bdev);
        cudaFree(cdev);
        return cudaStatus;
    }

    cudaStatus = cudaMemcpy(bdev, b, size * size * sizeof (float), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        printf("cudaMemcpy failed!");
        cudaFree(adev);
        cudaFree(bdev);
        cudaFree(cdev);
        return cudaStatus;
    }

    // Launch a kernel on the GPU
    mmKernel << <blocks, threads >> > (adev, bdev, size, cdev);
    cudaThreadSynchronize();

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        printf("mmKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        cudaFree(adev);
        cudaFree(bdev);
        cudaFree(cdev);
        return cudaStatus;
    }

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        printf("cudaDeviceSynchronize returned error code %d after launching mmKernel!\n", cudaStatus);
        cudaFree(adev);
        cudaFree(bdev);
        cudaFree(cdev);
        return cudaStatus;
    }

    // Copy output matrix from GPU buffer to host memory
    cudaStatus = cudaMemcpy(c, cdev, size * size * sizeof(float), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        printf("cudaMemcpy failed!");
    }

    cudaFree(adev);
    cudaFree(bdev);
    cudaFree(cdev);

    return cudaStatus;
}