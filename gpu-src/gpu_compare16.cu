#include <iostream>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdlib.h>
#include <sys/time.h>


using namespace std;

#ifndef LeafN
#define LeafN 99322
#endif

#ifndef OrderN
#define OrderN 99321
#endif

struct Abd
{
    //char name[20];
    string name;
    float data[LeafN];
};
struct CompData
{
    float Dist_1[OrderN];
    float Dist_2[OrderN];
    int Order_1[OrderN];
    int Order_2[OrderN];
    int Order_d[OrderN];
    int Id[LeafN];
};







#ifndef GPUC
#define GPUC

__global__ void gpu_Calc_sim(CompData *cd, Abd *v_Abd_1, Abd *v_Abd_2, float *results, int count);

class gpu_compare
{
public:
    gpu_compare();
    int sendData(CompData * cd, Abd * abd1, Abd * abd2,int count, int version = 2);
    //please malloc or new memory space for these data, and don't free these memory untill you retrive the results
    //count means the number of Abd2
    //default to use second generation compare algorithm
    int act();
    int getResult(float * putResult);
    /*
    error table  0 for all right, 1 for no enough memory
    2 for error state for example, you ask the gpu_compare to act befor send data to it
    */

private:
    int maxMem, deviceID, countCore,state,count;
    cudaDeviceProp deviceProp;
    size_t freeMem, totalMem;
    CompData *compData, *g_compData;
    Abd *abd1, *abd2, *g_abd1, *g_abd2;
    float *results, *g_results;
    int version,compFlag;
};

gpu_compare::gpu_compare()
{
    cudaChooseDevice(&this->deviceID, &this->deviceProp);
    cudaGetDeviceProperties(&this->deviceProp, this->deviceID);
    this->maxMem = this->deviceProp.totalGlobalMem;
    this->countCore = this->deviceProp.multiProcessorCount;
    cudaMemGetInfo(&freeMem, &totalMem);
    //cout<<"the best block size is : "<<this->bestBlockSize<<endl;
	//cout<<"the min Grid size is : "<<this->minGridSize<<endl;
    if(1)
    {
        cout<<"now using "<<this->deviceProp.name<<" device id:"<<this->deviceID<<endl;
        cout<<"count of SM is "<<this->deviceProp.multiProcessorCount<<endl;
        cout<<"warp size is (threads per SM) "<<this->deviceProp.warpSize<<endl;

    }
    this->compFlag=1;
    this->state=1;
}
int gpu_compare::sendData(CompData *cd, Abd *abd1, Abd *abd2,int count, int version )
{
    /*
    1 for no enough memory
    2 for state error
    */
    if (this->state!=1)
    {
        cout<<"gpu_compare is not in right state!\n";
        return 2;
    }
    this->count=count;
    this->compData = cd;
    this->abd1 = abd1;
    this->abd2 = abd2;
    this->version = version;
	
    if (totalMem<sizeof(float)*(LeafN*(count+1)+count)*1.1)
    {
        cout<<"no enough gpu memory, will update later to support it\n";
        return 1;
    }
    else
    {
        // malloc the memory
        cudaMalloc((void **)&this->g_abd1,sizeof(Abd));
        cudaMalloc((void **)&this->g_abd2,sizeof(Abd)*count);
        cudaMalloc((void **)&this->g_results,sizeof(float)*count);
        if(this->compFlag) {
            cudaMalloc((void **) &this->g_compData, sizeof(CompData));
            compFlag=0;
        }
    }
    //send memory from main mamory to GPU memory
    cudaMemcpy(this->g_compData,cd,sizeof(CompData),cudaMemcpyHostToDevice);
    cudaMemcpy(this->g_abd1,abd1,sizeof(Abd),cudaMemcpyHostToDevice);
    cudaMemcpy(this->g_abd2,abd2,sizeof(Abd)*count,cudaMemcpyHostToDevice);

    this->state=2;
    return 0;
}

int gpu_compare::act()
{
    int wrapSize,i,blocks,wrapCount;
    if(this->state!=2)
    {
        cout<<"gpu_compare state error\n";
        return 2;
    }
    blocks=this->count/1+1;
    gpu_Calc_sim<<<blocks, 16>>>(this->g_compData, this->g_abd1, this->g_abd2, this->g_results,this->count);
    this->state=3;
    return 0;
}

int gpu_compare::getResult(float *putResult)
{
    if(this->state!=3)
    {
        cout<<"gpu_compare state error\n";
        return 2;
    }
    this->results=putResult;
    //this->results=new float[count];
    cudaMemcpy(this->results, this->g_results, sizeof(float) * this->count,cudaMemcpyDeviceToHost);
    /*for(int i=0;i<count;i++)
    {
        cout<<i<<endl;
        printf("the similarity is: %f",putResult[i]);
    }*/
    this-> state=4;
    //putResult=this->results;

    cudaFree(this->g_abd1);
    cudaFree(this->g_abd2);
    cudaFree(this->g_results);

    this->state=1;
    return 0;
}
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
    float Reg_abs[70];

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

        float abs_1;
        float abs_2;

        if (order_1 >= 0){

            c1_1 = Abd_1[order_1];
            c1_2 = Abd_2[order_1];
            abs_1=abs(Abd_1[order_1]- Abd_2[order_1]) * 0.5;
        }
        else {
            c1_1 = Reg_1[order_1 + 70];
            c1_2 = Reg_2[order_1 + 70];
            abs_1=Reg_abs[order_1 + 70];
        }

        if (order_2 >= 0){

            c2_1 = Abd_1[order_2];
            c2_2 = Abd_2[order_2];
            abs_2=abs(Abd_1[order_2]-Abd_2[order_2]) * 0.5;

        }
        else {
            c2_1 = Reg_1[order_2 + 70];
            c2_2 = Reg_2[order_2 + 70];
            abs_2 = Reg_abs[order_2 + 70];
        }
        //min
        float min_1 = (c1_1 < c1_2)?c1_1:c1_2;
        float min_2 = (c2_1 < c2_2)?c2_1:c2_2;

        total += min_1;
        total2 += abs(c1_1-c1_2);


        total += min_2;
        total2 += abs(c2_1-c2_2);


        /*if(abs(c2_1-c2_2) !=0  || abs(c1_1-c1_2) !=0)
        {
        cout<<c1_1<<"-"<<c1_2<<"="<<c1_1-c1_2<<" "<<abs(c1_1-c1_2)<<endl;
        cout<<c2_1<<"-"<<c2_2<<"="<<c2_1-c2_2<<" "<<abs(c2_1-c2_2)<<endl;
        cout<<"total2:"<<total2<<endl;
        }
        */



        //reduce
        Reg_1[order_d] = (c1_1 - min_1) * dist_1 + (c2_1 - min_2) * dist_2;
        Reg_2[order_d] = (c1_2 - min_1) * dist_1 + (c2_2 - min_2) * dist_2;
        Reg_abs[order_d]= abs_1*dist_1 + abs_2*dist_2;

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

