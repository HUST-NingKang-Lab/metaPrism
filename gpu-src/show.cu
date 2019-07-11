#include<stdio.h>
#include<iostream>
#include <unistd.h>
using namespace std;
void printDeviceProp(const cudaDeviceProp &prop)
{
    printf("Device Name : %s.\n", prop.name);
    printf("totalGlobalMem : %d.\n", prop.totalGlobalMem);
    printf("sharedMemPerBlock : %d.\n", prop.sharedMemPerBlock);
    printf("regsPerBlock : %d.\n", prop.regsPerBlock);
    printf("warpSize : %d.\n", prop.warpSize);
    printf("memPitch : %d.\n", prop.memPitch);
    printf("maxThreadsPerBlock : %d.\n", prop.maxThreadsPerBlock);    printf("maxThreadsDim[0 - 2] : %d %d %d.\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
    printf("maxGridSize[0 - 2] : %d %d %d.\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
    printf("totalConstMem : %d.\n", prop.totalConstMem);
    printf("major.minor : %d.%d.\n", prop.major, prop.minor);
    printf("clockRate : %d.\n", prop.clockRate);
    printf("textureAlignment : %d.\n", prop.textureAlignment);
    printf("deviceOverlap : %d.\n", prop.deviceOverlap);
    printf("multiProcessorCount : %d.\n", prop.multiProcessorCount);
}
void getGPUInfo()
{
    int count,i;
    cudaDeviceProp deviceProp;
    cudaGetDeviceCount(&count);
    for(i=0;i<count;i++)
    {
        cudaGetDeviceProperties(&deviceProp,i);
        cout<<"GPU ID "<<i<<"\nGPU name is:"<<deviceProp.name<<"\nGPU total global memory is"
            <<deviceProp.totalGlobalMem/1024/1024<<" (MB)\n";
    }
    return;
}

int main()
{
        int i,dID;
        cudaDeviceProp deviceProp;
        cudaGetDeviceCount(&i);
        getGPUInfo();
        cudaDeviceReset();
        return 0;
}
