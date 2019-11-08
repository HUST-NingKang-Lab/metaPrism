#include <iostream>
#include <algorithm>
#include <queue>
#include "index_entry.h"
#include "meta_result.cpp"
#include<algorithm>
#include"single_gpu.cu"

using namespace std;
#ifndef CALCCPP
#define CALCCPP

struct miniResult {
    float value;
    basic_entry *b_entry;
};

bool operator<(miniResult a, miniResult b) {
    return a.value < b.value;
}
bool cmp(miniResult a, miniResult b) {
    return a.value>b.value;
}
bool operator>(miniResult a, miniResult b) {
        return a.value > b.value;
}

void *calcSim(void *arg) {
    int deviceID = *((int *) arg);
    CompData CD;
    Comp_init(&CD);
    SiGPU gpu(deviceID, &CD);
    sendPara sendp;
    resultPara resultp;
    while (1) {
        sem_wait(&semse);
        pthread_mutex_lock(&sendLock);
        sendp = sendBuffer.front();
        sendp.deviceID=deviceID;
        sendBuffer.pop();
        pthread_mutex_unlock(&sendLock);
        sem_post(&semsf);
        resultp.setValue(sendp);
        cout<<endl;
         if(!sendp.EOP) {
            gpu.sendData(sendp.Abd1, sendp.Abd2, sendp.count);
            gpu.act();
            gpu.getResult(resultp.result);
        }
        sem_wait(&semrf);
        pthread_mutex_lock(&resultLock);
        resultBuffer.push(resultp);
        pthread_mutex_unlock(&resultLock);
        sem_post(&semre);
        if(sendp.EOP) {
            break;
        }
    }
    return;
}

void *process(void *arg) {
    int top_n = *(int *) arg;
    miniResult *buffer;
    resultPara resultp;
    int count;
    basic_entry **list;
    float *result;
    while (1) {
        sem_wait(&semre);
        pthread_mutex_lock(&killLock);
        pthread_mutex_lock(&resultLock);
        resultp = resultBuffer.front();
        resultBuffer.pop();
        pthread_mutex_unlock(&resultLock);
        sem_post(&semrf);
        if(resultp.EOP){
        }
        else {

            count = resultp.count;
            result = resultp.result;
            list = resultp.list;
            buffer = new miniResult[count];
            for (int i = 0; i < count; i++) {
                buffer[i].b_entry = list[i];
                buffer[i].value = result[i];
            }
            nth_element(buffer, buffer + top_n, buffer + count, cmp);
            sort(buffer, buffer + top_n, cmp);
            pthread_mutex_lock(&coutLock);
            cout <<"Query "<<resultp.Abd1->name << " calculated by GPU" << resultp.deviceID << endl;
            for (int i = 0; i < top_n; i++) {
                cout << "Match " << i + 1 << "   \t";
                cout << "Similarity: " << buffer[i].value << "%" << endl;
                cout << "Sample path: " << buffer[i].b_entry->getfilename() << endl;
            }
            pthread_mutex_unlock(&coutLock);
            delete[]buffer;
        }
        pthread_mutex_lock(&trashLock);
        trashBuffer.push(resultp);
        pthread_mutex_unlock(&trashLock);
        pthread_mutex_unlock(&killLock);
        //delete[]resultp.list;
    }
}

#endif