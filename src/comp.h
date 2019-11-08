#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>
#include <time.h>
#include <sys/time.h>
//#include "tree_class.h"
//#include "make_tree.h"
#include "cfn.h"

#ifndef COMP_H
#define COMP_H

#define LeafN 99322
#define OrderN 99321

using namespace std;

float Dist_1[OrderN];
float Dist_2[OrderN];
     
int Order_1[OrderN];
int Order_2[OrderN];
int Order_d[OrderN];
    
int Id[LeafN];
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
CompData GlobalCompData;
string Check_Env(){
                 
                 if (getenv("MetaPrism") == NULL) {
                                          
                                          cerr << "Please set the environment variable \"MetaStorms\" to the directory" << endl;
                                          exit(0);
                                          }
                 
                 string path = getenv("MetaPrism");
                 return path;
                                 
    }


void Calc_abundant(const char * infilename){
  map<string, unsigned int> table[7];
  ifstream infile(infilename, ifstream::in);
    if (!infile){
                 cerr <<"Open infile error :" << infilename << endl;
                 exit(0);
                 //system("pause");
                 }
    
    int line_count = 0;
    string buffer;
    getline(infile, buffer); //Lable
    while(getline(infile, buffer)){
                          if (buffer.size()==0) continue;
                          int bio_level = 0;
                          
                          int str_length = buffer.size();
                          string temp = "";                           
                          int str_iter = buffer.find_last_of('\t') + 1;//To locate last of '\t'
                          
                          while ((bio_level < 7) && (str_iter < str_length)){
                                
                                if ((buffer[str_iter] == ';')||(str_iter == str_length - 1)){
                                      if (temp.find("otu_")!=string::npos) break;
                                      if (temp.find("other")!=string::npos) break;
                                      if (temp.find("nclassified")!=string::npos) break;
                                      
                                      temp += ';'; 
                                      
                                      if (table[bio_level].count(temp) == 0){
                                                       table[bio_level][temp] = 1;
                                                       //cout<<temp<<endl;
                                      }
                                      else table[bio_level][temp] ++;

                                      temp = "";
                                      
                                      bio_level ++;
                                      str_iter ++;
                                      
                                      }
                                else 
                                     temp += buffer[str_iter];
                                
                                str_iter ++;
                                }  
                                  
                          line_count ++;
                          }

    int i =1;

    map<string, unsigned int>::iterator map_iter = table[i].begin();
    while(map_iter != table[i].end()){

                       cout<<map_iter->first<<'\t'<< (float)map_iter->second / (float)line_count <<endl;
                       map_iter++;

                      }




}

/*
float Comp_tree(string * tree_file, map<string, _PerRate> * table){

    Node node;
    node.Node_calc(*tree_file, 0, table);

    //cout << node.Get_same()<<"%"<<endl;
    //cout << node.Get_evalue().value << endl;


    return node.Get_same();

    }


float Comp_sam(string * tree_file, const char * sam1, const char * sam2){


    map<string, _PerRate> table;

    Make_tree(sam1, sam2, &table);

    float result = Comp_tree(tree_file, &table);

    return result;
    }
*/

void Load_id(const char * infilename, int * id){

     ifstream infile(infilename, ifstream::in);
     if (!infile){
                  cerr << "Open id file error, please check : " << infilename << endl;
                  exit(0);

                  }

     for (int i = 0; i < LeafN; i++)
         infile >> id[i];

     infile.close();
     infile.clear();
     }

float Load_abd(const char * infilename, int * id, float * abd){

    map<int, int> hash;

    ifstream infile(infilename, ifstream::in);
    if (!infile){
                  cerr << "Cannot open file : " << infilename << endl;
                  exit(0);

                  }

    string buffer;
    int line_count = 0;
    float total_abd=0;
    float shannon=0;
    getline(infile, buffer); //label

    while(getline(infile, buffer)){

                          if (buffer.size() == 0) continue;

                          line_count ++;

                          string seq_id;
                          int current_id;

                          stringstream strin(buffer);

                          strin >> seq_id >> current_id;

                          if (hash.count(current_id) == 0)
                                                     hash[current_id] = 1;
                          else hash[current_id] ++;
                          }

    for (int i = 0; i < LeafN; i++)
        if (hash.count(id[i]) == 0)
           abd[i] = 0;
        else {abd[i] = (float)hash[id[i]] * 100 / (float)line_count;
              total_abd=total_abd+abd[i];
              float t_abd=abd[i]/100;
              shannon=shannon-t_abd*log(t_abd);
              //cout<<"shannon:"<<shannon<<endl;
              }
    infile.close();
    infile.clear();


    return shannon;
    }



void Load_order(const char * infilename, int * Order_1, float * Dist_1, int * Order_2, float * Dist_2, int * Order_d){
    ifstream infile(infilename, ifstream::in);
    if (!infile){
                  cerr << "Open order file error, please check : " << infilename << endl;
                  exit(0);

                  }

    for (int i = 0; i < OrderN; i++){

        infile >> Order_1[i] >> Dist_1[i] >> Order_2[i] >> Dist_2[i] >> Order_d[i];

        }

    infile.close();
    infile.clear();
    }


#endif
