#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
using namespace std;
#ifndef LeafN
#define LeafN 99322
#endif
#ifndef ABD
#define ABD
struct Abd
{
    //char name[20];
    string name;
    float data[LeafN];
};
#endif

#ifndef OrderN
#define OrderN 99321
#endif
#ifndef WarpSize
#define WarpSize 32
#endif
#ifndef CMPDATA
#define CMPDATA
struct CompData
{
    float Dist_1[OrderN];
    float Dist_2[OrderN];
    int Order_1[OrderN];
    int Order_2[OrderN];
    int Order_d[OrderN];
    int Id[LeafN];
};
#endif
__global__ void gpu_Calc_sim(CompData *cd, Abd *v_Abd_1, Abd *v_Abd_2, float *results,int count);


#ifndef gpu
#define gpu
class GPU{
public:
    int count,id,computSum;
    int alloc;
    int offset;
    cudaDeviceProp deviceProp;
    void init(int i);
    void reset();
    int sendData(CompData *, Abd *, Abd *);
    int act();
    int getResult(float *putResult);
private:
    bool compFlag;
    struct Abd *abd1,*g_abd1,*abd2,*g_abd2;
    struct CompData *compData,*g_compData;
    float *results,*g_results;
};
void GPU::init(int i){
    this->id=i;
    cudaGetDeviceProperties(&(this->deviceProp),id);
    computSum=deviceProp.warpSize*deviceProp.multiProcessorCount;
    alloc=offset=0;
    compFlag= true;
    return;
}
void GPU::reset(){
    alloc=offset=0;
    cudaSetDevice(id);
    cudaFree(g_abd1);
    cudaFree(g_abd2);
    cudaFree(g_results);
    abd1=abd2=g_abd1=g_abd2=0;g_results=0;
}
int GPU::sendData(CompData *cd, Abd *first_abd, Abd *second_abd){
    cudaSetDevice(id);
    alloc=alloc+offset>count?count-offset:alloc;
    if (deviceProp.totalGlobalMem<sizeof(float)*(LeafN*(alloc+1)+alloc)*1.1)
        return 1;
    abd1=first_abd;
    abd2=second_abd+offset;
    cudaMalloc((void **)&this->g_abd1,sizeof(Abd));
    cudaMalloc((void **)&this->g_abd2,sizeof(Abd)*alloc);
    cudaMalloc((void **)&this->g_results,sizeof(float)*alloc);
    if(this->compFlag) {
        cudaMalloc((void **) &this->g_compData, sizeof(CompData));
        cudaMemcpy(this->g_compData,cd,sizeof(CompData),cudaMemcpyHostToDevice);
        compFlag= false;
    }
    cudaMemcpy(this->g_abd1,abd1,sizeof(Abd),cudaMemcpyHostToDevice);
    cudaMemcpy(this->g_abd2,abd2,sizeof(Abd)*alloc,cudaMemcpyHostToDevice);
    return 0;
}
int GPU::act(){
    cudaSetDevice(id);
    int blocks;
    blocks=alloc/WarpSize+1;
    gpu_Calc_sim<<<blocks, WarpSize>>>(g_compData, g_abd1, g_abd2, g_results,alloc);
    return 0;
}
int GPU::getResult(float * putResult){
    putResult+=offset;
    cudaSetDevice(id);
    cudaMemcpy(putResult, this->g_results, sizeof(float) * this->alloc,cudaMemcpyDeviceToHost);
    return 0;
}
#endif
#ifndef calc
#define calc
__global__ void gpu_Calc_sim(CompData *cd, Abd *v_Abd_1, Abd *v_Abd_2, float *results,int count)
{
    //process memory data
    float *Dist_1;
    float *Dist_2;
    int *Order_1;
    int *Order_2;
    int *Order_d;
    int *Id;
    Dist_1 = cd->Dist_1;
    Dist_2 = cd->Dist_2;
    Order_1 = cd->Order_1;
    Order_2 = cd->Order_2;
    Order_d = cd->Order_d;
    Id = cd->Id;
    //change offset of each parameters

    int offset;
    offset=blockIdx.x*blockDim.x+threadIdx.x;
    if(offset>=count)
        return ;
    //const Meta_Result * buffer=(Meta_Result * )v_buffer+blockIdx.x;// don't know wether it's used
    v_Abd_2=(Abd *)v_Abd_2 +offset;
    const float *Abd_2 = v_Abd_2->data;
    const float *Abd_1 = v_Abd_1->data;

    //start origin data
    float Reg_1[70];
    float Reg_2[70];

    float total = 0;
    float total2=0;
    float total3=0;
    int root;

    for(int i = 0; i < OrderN; i++){
        //cout<<"i:"<<endl;

        int order_1 = Order_1[i];
        int order_2 = Order_2[i];
        int order_d = Order_d[i] + 70;

        float dist_1 = 1- Dist_1[i];
        float dist_2 = 1- Dist_2[i];

        float c1_1;
        float c1_2;

        float c2_1;
        float c2_2;


        if (order_1 >= 0){

            c1_1 = Abd_1[order_1];
            c1_2 = Abd_2[order_1];
        }
        else {
            c1_1 = Reg_1[order_1 + 70];
            c1_2 = Reg_2[order_1 + 70];
        }

        if (order_2 >= 0){

            c2_1 = Abd_1[order_2];
            c2_2 = Abd_2[order_2];

        }
        else {
            c2_1 = Reg_1[order_2 + 70];
            c2_2 = Reg_2[order_2 + 70];
        }
        //min
        float min_1 = (c1_1 < c1_2)?c1_1:c1_2;
        float min_2 = (c2_1 < c2_2)?c2_1:c2_2;

        total += min_1;
        total += min_2;

        //reduce
        Reg_1[order_d] = (c1_1 - min_1) * dist_1 + (c2_1 - min_2) * dist_2;
        Reg_2[order_d] = (c1_2 - min_1) * dist_1 + (c2_2 - min_2) * dist_2;
        root = order_d;
    }
    total += (Reg_1[root] < Reg_2[root])?Reg_1[root]:Reg_2[root];
    //cout<<"total:"<<total<<endl;
    //total2 += abs(Reg_1[root]-Reg_2[root]);
    //cout<<"second score:"<<total2<<endl;
    //cout<<"third score:"<<100-Reg_abs[root]<<endl;
    //cout<<total<<"\t"<<100-Reg_abs[root]<<endl;

    //return total;

    results[offset]=total;
    //100-Reg_abs[root];
    return;
}
#endif

