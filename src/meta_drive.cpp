#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include "version.h"
#include<vector>
#include<set>
#include<unordered_map>
#include <time.h>
#include <sys/time.h>

using namespace std;
#ifndef LeafN
#define LeafN 99322
#endif

#ifndef OrderN
#define OrderN 99321
#endif

#ifndef CMPDATA
#define CMPDATA
struct CompData {
    float Dist_1[OrderN];
    float Dist_2[OrderN];
    int Order_1[OrderN];
    int Order_2[OrderN];
    int Order_d[OrderN];
    int Id[LeafN];
};
#endif
#ifndef META_DRIVE
#define META_DRIVE
struct drivePara {
    string outpath;
    string filelist;
    string indexname;
    int n;
    int t;
    int group;
    string scroingtype;
    string biotype;
    string filterflag;
    int is_index;
    int is_gpu;
    CompData *CD;
};

#include "meta_database.cpp"

void GetBiome(const string &s, string &biome, const string &c) {
    string::size_type pos1;

    string::size_type pos2;
    vector <string> v;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));

    vector<string>::iterator iter = v.end() - 3;
    biome = (*iter);
    //cout<<biome<<endl;

}

int load_input(string filelist, vector <string> &inputlist, vector <string> &namelist, vector <string> &biomelist) {
    string buffer;
    ifstream in_list(filelist.c_str(), ifstream::in);
    while (getline(in_list, buffer)) {

        //cout<<buffer<<endl;
        int iter_1 = buffer.find_last_of('/');
        int iter_2 = buffer.substr(0, iter_1).find_last_of('/') + 1;

        string biome;
        GetBiome(buffer, biome, "/");


        inputlist.push_back(buffer);
        biomelist.push_back(biome);
        namelist.push_back(buffer.substr(iter_2, iter_1 - iter_2));

    }
    return 0;
}

//int drive(string outpath, string filelist, string indexname, int n, int t, int group, string scroingtype,
//string filterflag, int is_index, Meta_Database &database, int is_gpu,CompData *CD) {
void drive(void *arg) {
    drivePara *pointer = (drivePara *) arg;
    drivePara &para = *pointer;
    string outpath = para.outpath;
    string filelist = para.filelist;
    string indexname = para.indexname;
    int n = para.n;
    int t = para.t;
    int group = para.group;
    string scroingtype = para.scroingtype;
    string filterflag = para.filterflag;
    int is_index = para.is_index;
    int is_gpu = para.is_gpu;
    CompData *CD = para.CD;
    vector <string> inputlist;
    vector <string> namelist;
    vector <string> databaselist;
    vector <string> biomelist;
    load_input(filelist, inputlist, namelist, biomelist);
    abdMap *pAbdMap;
    pAbdMap = new abdMap(indexname, CD);
    Meta_Database database(indexname, para.biotype, pAbdMap);
    map<string, float *> database_map;

    vector<string>::iterator input_iter;
    vector<string>::iterator output_iter = namelist.begin();
    vector<string>::iterator biome_iter = biomelist.begin();
    for (input_iter = inputlist.begin(); input_iter != inputlist.end(); input_iter++) {
        if (outpath.size() > 0) {
            string outfilename = outpath + "/" + *(output_iter);
            ofstream out(outfilename.c_str(), ofstream::out);

            if (!out) {
                cerr << "Open output file error : " << outfilename << endl;
                exit(0);
            }
            if (is_index == 0) {
                database.Parallel_Exhaustive_Query_RAM(out, *(input_iter), databaselist, database_map, n, t, group,
                                                       scroingtype, is_gpu);

            } else {

                database.Parallel_Indexed_Query_RAM(out, *(input_iter), *(biome_iter), databaselist, database_map, n, t,
                                                    group, scroingtype, filterflag, is_gpu);
            }


        } else {

            if (is_index == 0) {
                database.Parallel_Exhaustive_Query_RAM(cout, *(input_iter), databaselist, database_map, n, t, group,
                                                       scroingtype, is_gpu);
            } else {
                database.Parallel_Indexed_Query_RAM(cout, *(input_iter), *(biome_iter), databaselist, database_map, n,
                                                    t, group, scroingtype, filterflag, is_gpu);
                //database.Parallel_Indexed_Query_RAM(cout, *(input_iter),"none",databaselist,database_map,n,t,group,scroingtype,filterflag,is_gpu);
            }
        }

        biome_iter++;
        output_iter++;
    }
    for (int i = 0; i <= deviceCount; i++) {
        struct sendPara send_p;
        send_p.EOP = true;
        sem_wait(&semsf);
        pthread_mutex_lock(&sendLock);
        sendBuffer.push(send_p);
        pthread_mutex_unlock(&sendLock);
        sem_post(&semse);
    }
    for (int i = 0; i < deviceCount; i++) {
        pthread_join(calcID[i], NULL);
        pthread_mutex_lock(&coutLock);
        cout << "GPU thread " << i << " stopped\n";
        pthread_mutex_unlock(&coutLock);
    }
    pthread_mutex_lock(&killLock);
    suc = pthread_cancel(processID);
    pthread_mutex_lock(&coutLock);
    cout << "process thread stopped\n";
    pthread_mutex_unlock(&coutLock);
    pthread_mutex_unlock(&killLock);
    while (trashBuffer.size() > 0) {
        resultPara trash = trashBuffer.front();
        delete[]trash.Abd1;
        delete[]trash.Abd2;
        delete[]trash.result;
        delete[]trash.list;
        trashBuffer.pop();
    }
    return;
}

#endif

