//
// Created by Luofan on 2019-03-16.
//
#include <iostream>
#include "comp.h"
#include <fstream>
#include <string>
using namespace std;
char PATH[50]="/home/kangkai";
char PATH2[50]="/home/zhumo/Meta-Prism-Test/cluster";
char PATH3[50]="/data2/public/metaPrismOut";
int main(){
    char cbuff[50]:
    char cb2[100];
    float *abd;
    abd = new float[LeafN];
    string str;
    sprintf(cbuff,"%s/%s",PATH,"treefile/id.txt");
    Load_id(cbuff, Id);
    sprintf(cbuff,"%s/%s",PATH,"treefile/order.txt");
    Load_order(cbuff, Order_1, Dist_1, Order_2, Dist_2, Order_d);
    ifstream infile;
    infile.open("/home/kangkai/treefile/files.txt");
    ofstream fout;
    sprintf(cbuff,"%s/%s",PATH3,"DATA.txt");
    fout.(cbuff);
    while(!infile.eof()){
        getline(infile,str);
        cb2=sprintf("%s/%s%s",PATH2,str.c_str(),"classification.txt");
        Load_abd(cb2,Id,abd);
        fout<<str<<endl;
        for(int i=0;i<LeafN;i++)
            fout<<abd[i]<<' ';
    }
    fout.close();
    return 0;
}

