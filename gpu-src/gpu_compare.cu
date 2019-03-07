#include <iostream>
#include <map>
#include <string>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdlib.h>
#include <stdio.h>
//#include <stdio.h>
using namespace std;

#ifndef LeafN
#define LeafN 99322
#endif

#ifndef OrderN
#define OrderN 99321
#endif

struct Abd
{
    string name;
    float *data[LeafN];
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





int get_compData(CompData *cd)
{
    cout<<"begin to copy"<<endl;
    memcpy(cd->Dist_1, Dist_1, sizeof(float) * OrderN);
    memcpy(cd->Dist_2, Dist_2, sizeof(float) * OrderN);
    memcpy(cd->Order_1, Order_1, sizeof(int) * OrderN);
    memcpy(cd->Order_2, Order_2, sizeof(int) * OrderN);
    memcpy(cd->Order_d, Order_d, sizeof(int) * OrderN);
    memcpy(cd->Id, Id, sizeof(int) * OrderN);
    cout<<"finish copy"<<endl;
    return 1;
}





#ifndef GPUC
#define GPUC

__global__ void gpu_Calc_sim(struct CompData *cd, struct Abd *v_Abd_1, struct Abd *v_Abd_2, float *results,int *table);

class gpu_compare
{
public:
    gpu_compare();
    int sendData(  CompData * cd,   Abd * abd1,  Abd * abd2,  int count,  int version = 2);
    //please malloc or new memory space for these data, and don't free these memory untill you retrive the results
    //count means the number of Abd2
    //default to use second generation compare algorithm
    int act();
    int getResult(  float * putResult);//please malloc or new the memory before!
    /*
    error table  0 for all right, 1 for no enough memory
    2 for error state for example, you ask the gpu_compare to act befor send data to it
    */

private:
    int getInfo();
    int maxMem, deviceID, countCore,state,count;
    int allAbd,maxAbd;
    map<string,int> gpuMem;
    cudaDeviceProp deviceProp;
    size_t freeMem, totalMem;
    CompData *compData;
      CompData *g_compData;
    Abd *abd1, *abd2;
      Abd *g_abd1,* g_abd2;
    float *g_results;
    int version;
    int *table,*g_table;
};

int gpu_compare::getInfo()
{
    cudaGetDeviceProperties(&this->deviceProp, this->deviceID);
    this->maxMem = this->deviceProp.totalGlobalMem;
    this->countCore = this->deviceProp.multiProcessorCount;
    cudaMemGetInfo(&this->freeMem, &this->totalMem);
    cout<<"countCore:"<<this->countCore<<endl;
    cout<<"maxMem:"<<this->maxMem<<endl;
    cout<<"freeMem:"<<this->freeMem<<endl;
    cout<<"totalMem:"<<this->totalMem<<endl;
    return 0;
}

gpu_compare::gpu_compare()
{
    cudaChooseDevice(&this->deviceID, &this->deviceProp);
    cudaMalloc((void **)&this->g_abd1,sizeof(Abd));
    cudaMalloc((void **)&this->g_compData,sizeof(CompData));
    this->getInfo();
    this->allAbd=0;
    this->maxAbd=(int) 0.9*(this->freeMem)/(sizeof(Abd)+ 2* sizeof(int)+sizeof(float));// sum of Abd2, query table, result
    cudaMalloc((void **)&this->g_abd2, sizeof(Abd)*this->maxAbd);
    this->state=1;
    cout<<"finish initiate"<<endl;
}

/*gpu_compare::gpu_compare()
{
    cudaChooseDevice(&this->deviceID, &this->deviceProp);
    cudaGetDeviceProperties(&this->deviceProp, this->deviceID);
    this->maxMem = this->deviceProp.totalGlobalMem;
    this->countCore = this->deviceProp.multiProcessorCount;
    cudaMemGetInfo(&freeMem, &totalMem);
    cout<<"freeMem:"<<freeMem<<endl;
    cout<<"totalMem:"<<totalMem<<endl;
    this->state=1;
}*/


/*
int gpu_compare::sendData(  CompData *cd,   Abd *abd1,   Abd *abd2,  int count,   int version )
{
    
    //1 for no enough memory
    //2 for state error
    
    Abd * gAbd;
    int index;
    string stringName,stringBuffer;
    std::map<string,int> ::iterator miter;

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
    this->getInfo();
    //send memory from main mamory to GPU memory
    cudaMemcpy(this->g_compData,cd,sizeof(CompData),cudaMemcpyHostToDevice);
    cudaMemcpy(this->g_abd1,abd1,sizeof(Abd),cudaMemcpyHostToDevice);
    cudaMalloc((void **)&this->g_table,sizeof(int)*count);
    cudaMalloc((void **)&this->g_results,sizeof(float)*count);
    this->table = new int[count];
    if(this->allAbd+count>=this->maxAbd)
    {
        int i=this->allAbd+count-this->maxAbd;
        for(miter=this->gpuMem.begin();i>=0;i--)
        {
            stringBuffer=miter->first;
            miter++;
            this->gpuMem.erase(stringBuffer);
        }
    }
    for (int i = 0; i <count ; ++i) {
        stringName= abd2[count].name;
        miter = this->gpuMem.find(stringName);
        if (miter!=gpuMem.end())
            this->table[i]=miter->second;
        else
        {
            if(this->allAbd< this-> maxAbd)
                index=this->allAbd;
            this->table[i]=index;
            gpuMem.insert(map<string,int>::value_type(stringName,index));
            gAbd=this->g_abd2+index;
            cudaMemcpy(gAbd,abd2+count, sizeof(Abd),cudaMemcpyHostToDevice);
        }
    }
    cudaMemcpy(this->g_table,this->table, sizeof(int)*count,cudaMemcpyDeviceToHost);

    this->state=2;
    return 0;
} */

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
        cout<<"count:"<<count<<endl;
        cout<< "totalMem:"<<totalMem<<endl;
        cout<<"inputMem:"<<sizeof(float)*(LeafN*(count+1)+count)*1.1<<endl;
        return 1;
    }
    else
    {
        // malloc the memory
        cudaMalloc((void **)&this->g_abd1,sizeof(Abd));
        cudaMalloc((void **)&this->g_abd2,sizeof(Abd)*count);
        cudaMalloc((void **)&this->g_compData,sizeof(CompData));
        cudaMalloc((void **)&this->g_results,sizeof(float)*count);
        cout<<"finish allocate to gpu"<<endl;
   }
    //send memory from main mamory to GPU memory
    cudaMemcpy(this->g_compData,cd,sizeof(CompData),cudaMemcpyHostToDevice);
    cudaMemcpy(this->g_abd1,abd1,sizeof(Abd),cudaMemcpyHostToDevice);
    cudaMemcpy(this->g_abd2,abd2,sizeof(Abd)*count,cudaMemcpyHostToDevice);
    cout<<"finish copy to gpu"<<endl;

    /* 
    for(int i=1;i<count;i++){
        printf("%s",this->abd2[i].name.c_str());
        for(int j=0;j<100;j++){
            printf("%f ",this->abd2[i].data[j]);
        }
        //cout<<this->g_abd2[i].name<<endl;
    }
    */

    this->state=2;
    return 0;
}


int gpu_compare::act()
{
    if(this->state!=2)
    {
        cout<<"gpu_compare state error\n";
        return 2;
    }

    /*for(int i=1;i<count;i++){
        cout<<g_abd2[i].name<<endl;
    }*/

    cout<<"begin to calculate"<<endl;

    gpu_Calc_sim<<<count, 1>>>(this->g_compData, this->g_abd1, this->g_abd2, this->g_results, this->g_table);


    for(int i=0;i<count;i++){
        printf("%s\n",g_abd2[i].name.c_str());
    }
    this->state=3;
    return 0;
}
int gpu_compare::getResult(  float *putResult)
{
    if(this->state!=3)
    {
        cout<<"gpu_compare state error\n";
        return 2;
    }

    cudaMemcpy(/*this->*/putResult, this->g_results, sizeof(float) * count,cudaMemcpyDeviceToHost);
    this-> state=4;
    return 0;
}
__global__ void gpu_Calc_sim(CompData *cd, Abd *v_Abd_1, Abd *v_Abd_2, float *results,int * table)
{
    //process memory data
    float *Dist_1;
    float *Dist_2;
    int *Order_1;
    int *Order_2;
    int *Order_d;
    int *Id;
    int offset;
    Dist_1 = cd->Dist_1;
    Dist_2 = cd->Dist_2;
    Order_1 = cd->Order_1;
    Order_2 = cd->Order_2;
    Order_d = cd->Order_d;
    Id = cd->Id;
    //change offset of each parameters

    //  Meta_Result * buffer=(Meta_Result * )v_buffer+blockIdx.x;// don't know wether it's used
    offset=table[blockIdx.x];
    v_Abd_2=(Abd *)v_Abd_2 +offset;
      float *Abd_2 = (float *) &(v_Abd_2->data);
      float *Abd_1 = (float *) &(v_Abd_1->data);

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
            //cout<<"c1_1："<<c1_1<<endl;
            c1_2 = Abd_2[order_1];
            //cout<<"c1_2: "<<c1_2<<endl;
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
     //results[blockIdx.x]=100-Reg_abs[root];
     results[blockIdx.x]=total;

    //return total;

}


#endif
