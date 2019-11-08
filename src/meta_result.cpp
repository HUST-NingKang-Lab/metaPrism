#include "comp.h"
#include <iostream>
#include<sys/types.h>
#include<sys/ipc.h>
#include <memory>
#include<semaphore.h>
#include<unistd.h>
#include<signal.h>
#include<pthread.h>
#include "index_entry.h"
using namespace std;

#ifndef LeafN
#define LeafN 99322
#endif

#ifndef OrderN
#define OrderN 99321
#endif
#ifndef WarpSize
#define WarpSize 32
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

#ifndef METARESULT
#define METARESULT
int deviceCount;
class sendPara{
public:
    int count;
    int deviceID;
    Abd* Abd1;
    Abd* Abd2;
    float* result;
    basic_entry** list;
    bool EOP;
    sendPara(){
        Abd1=NULL;
        result=NULL;
        Abd2=NULL;
        list=NULL;
    }
};

class resultPara{
public:
    int count;
    float* result;
    Abd* Abd1;
    Abd* Abd2;
    int deviceID;
    basic_entry** list;
    bool EOP;
    resultPara(){EOP= false;}
    void setValue(sendPara a){
        deviceID=a.deviceID;
        result=a.result;
        count=a.count;
        Abd1=a.Abd1;
        Abd2=a.Abd2;
        list=a.list;
        EOP=a.EOP;
        return;
    }
};
sem_t semsf,semse,semrf,semre;//send|result full|empty
pthread_mutex_t sendLock,resultLock,calcLock,trashLock,killLock,coutLock;
queue<sendPara> sendBuffer;
queue<resultPara> resultBuffer,trashBuffer;
pthread_t driveID,*calcID,processID;
class Meta_Result{
public:
    float m_value;
    float n_value;
    Index_Entry * entry;
    basic_entry * b_entry;
    bool be_filter;
    //to be delete
    float shannon;

    friend class Meta_Database;

    Meta_Result(){
        this->m_value =0;
        this->n_value=0;
        entry = 0;
        b_entry=0;
        be_filter=false;
        //to be delete
        shannon=0;
    }
    Meta_Result(float _value, Index_Entry * _entry ){
        m_value = _value;
        entry = _entry;
        be_filter=false;
        //b_entry=0;
    }

    Meta_Result(float _value, basic_entry * _entry ){
        m_value = _value;
        b_entry = _entry;
        be_filter= false;
        //entry=0;
    }
};//end Meta_Result

#endif
