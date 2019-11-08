//
// Created by 康凯 on 2019-09-04.
//
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

#ifndef ABDMAP
#define ABDMAP

string extractfilename(string buffer) {
    if (buffer.size() == 0)
        cerr << "Input file is empty" << endl;

    string filename;

    int begin = 0;
    int end = buffer.find('\t', begin);

    id = atoi(buffer.substr(begin, end - begin).c_str());

    begin = end + 1;
    end = buffer.find('\t', begin);

    group = atoi(buffer.substr(begin, end - begin).c_str());

    begin = end + 1;
    end = buffer.find('\t', begin);

    filename = buffer.substr(begin, end - begin);
    return filename;
}
class abdMap {
public:
    abdMap(string indexName, CompData *CD);

    float *operator[](string name);
    bool find(string name){
        return (nameSet.count(name)>0);
    }

private:
    string indexName;
    set <string> nameSet;
    unordered_map<string, float *> floatMap;

    float* loadAbd(string infilename);

    CompData *CD;
};
abdMap::abdMap(string indexName, CompData *CD) {
    this->indexName = indexName;
    this->CD = CD;
    string nametable = indexName + "/" + "nametable.txt";
    ifstream in_name(nametable.c_str(), ifstream::in);
    if (!in_name) {
        cerr << "Error: Cannot Open Database Index File : " << nametable << endl;
        exit(0);
    }
    string buffer;
    Comp_init();
    getline(in_name, buffer);
    getline(in_name, buffer);
    while (getline(in_name, buffer)) {
        string filename = extractfilename(buffer) + "/classification.txt";
        //cout<<filename<<endl;
        nameSet.insert(filename);
    }
}

float* abdMap::operator[](string name) {
    if (floatMap.count(name) > 0){
        //cout<<"i have\n";
        return floatMap[name];}
    else {
         float *a =loadAbd(name);
         floatMap[name] =a;
        //cout<<"loading\n";
        return floatMap[name];
    }
}

float* abdMap::loadAbd(string infilename) {
    int* id = this->CD->Id;

    unordered_map<int, int> hash;

    ifstream infile(infilename.c_str(), ifstream::in);
    if (!infile) {
        cerr << "Cannot open file : " << infilename << endl;
        exit(0);
    }
    float *abd = new float[LeafN];

    string buffer;
    int line_count = 0;
    getline(infile, buffer); //label


    while (getline(infile, buffer)) {

        if (buffer.size() == 0) continue;
        line_count++;

        string seq_id;
        int current_id;

        stringstream strin(buffer);

        strin >> seq_id >> current_id;

        if (hash.count(current_id) == 0)
            hash[current_id] = 1;
        else hash[current_id]++;
    }

    for (int i = 0; i < LeafN; i++) {
        if (hash.count(id[i]) == 0)
            abd[i] = 0;
        else {
            abd[i] = (float) hash[id[i]] * 100 / (float) line_count;
        }
    }
    //cout<<"total_abd:"<<total_abd<<endl;
    infile.close();
    infile.clear();

    return abd;
}



#endif