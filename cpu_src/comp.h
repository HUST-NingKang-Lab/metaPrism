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
    Res res[table[i].size()];
    int res_iter = 0;
    
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

float Calc_sim(float * Abd_1, float * Abd_2, int * Order_1, float * Dist_1, int * Order_2, float * Dist_2, int * Order_d,float &score1, float &score2){
      
      float Reg_1[70];
      float Reg_2[70];
      float Reg_abs[70];
      
      float total = 0;
      float total2=0;
      float total3=0;
      int root;

      //cout<<"begin to loop"<<endl;
      
      for(int i = 0; i < OrderN; i++){
              //cout<<"i:"<<i<<endl;
              
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

              //cout<<Abd_1[i]<<"\t"<<Abd_2[i]<<endl;
                                
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
              //cout<<"total:"<<total<<"\t"<<i<<endl;
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
      //total2 += abs(Reg_1[root]-Reg_2[root]);
      //cout<<"second score:"<<total2<<endl;
      //cout<<"third score:"<<100-Reg_abs[root]<<endl;
      //cout<<total<<"\t"<<100-Reg_abs[root]<<endl;
      score1=total;
      score2=100-Reg_abs[root];
      
      
      //return 100-Reg_abs[root];
      return total;
      
      }



float Calc_sim(float * Abd_1, float * Abd_2, int * Order_1, float * Dist_1, int * Order_2, float * Dist_2, int * Order_d){
      
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
      return 100-Reg_abs[root];
      
      }


void Comp_init(){
      
      string path = Check_Env();
      string id_file = path + "/treefile/id.txt";
      string order_file = path + "/treefile/order.txt";
          
      Load_id(id_file.c_str(), Id);
      Load_order(order_file.c_str(), Order_1, Dist_1, Order_2, Dist_2, Order_d);
      
      }

float Comp_sam(const char * sam1, const char * sam2){
      
      float * Abd_1 = new float[LeafN];
      float * Abd_2 = new float[LeafN];
      
      float shannon_1=Load_abd(sam1, Id, Abd_1);
      float shannon_2=Load_abd(sam2, Id, Abd_2);


      //temp add
      //map<string, unsigned int> table[7];
      //cout<<"in file "<<sam1<<endl;
      //Calc_abundant(sam1);
      //cout<<"in file "<<sam2<<endl;
      //Calc_abundant(sam2);


      float result =  Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d);
      //cout<<result<<"\t"<<abs(shannon_1-shannon_2)<<endl;

      return result;

      
      //return Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d);
      
      }

float Comp_sam(const char * sam1, const char * sam2,float &score1, float &score2){
      
      float * Abd_1 = new float[LeafN];
      float * Abd_2 = new float[LeafN];
      
      Load_abd(sam1, Id, Abd_1);
      Load_abd(sam2, Id, Abd_2);

      
      return Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);
      
      }



float Comp_sam(float * Abd_1, const char * sam2,float &score1, float &score2){
      
      //float * Abd_1 = new float[LeafN];
      float * Abd_2 = new float[LeafN];
      
      //Load_abd(sam1, Id, Abd_1);
      Load_abd(sam2, Id, Abd_2);

      
      return Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);
      
      }



float Comp_sam(const char * sam1, const char * sam2,float &score1, float &score2,float &shannon){
      
      float * Abd_1 = new float[LeafN];
      float * Abd_2 = new float[LeafN];
      
      float shannon_1 = Load_abd(sam1, Id, Abd_1);
      float shannon_2 = Load_abd(sam2, Id, Abd_2);
      //Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);
      //cout<<score1<<"\t"<<score2<<endl;
      float result=Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);
      //cout<<"entropy:"<<abs(shannon_1- shannon_2)<<endl;
      //cout<<"similarity:"<<score1<<endl;
      cout << abs(shannon_1- shannon_2) <<"\t"<< 1 <<endl;
      //cout<<score1 <<"\t"<< 2 <<endl;

      
      return result;
      //return Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);
      
      }

  float Comp_sam_RAM(float * Abd_1, float *Abd_2, float &score1, float &score2){
      
      //float * Abd_1 = new floa[LeafN];
      //float * Abd_2 = new float[LeafN];
      
      //float shannon_1 = Load_abd(sam1, Id, Abd_1);
      //float shannon_2 = Load_abd(sam2, Id, Abd_2);
      //Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);
      //cout<<score1<<"\t"<<score2<<endl;
      //cout<<"begin to calculate"<<endl;
      struct timeval tv_begin, tv_end;
      time_t startTime,endTime;
      gettimeofday(&tv_begin,NULL);
      startTime=clock();

      float result=Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);
      endTime=clock();
      gettimeofday(&tv_end,NULL);

      //cout<<"pair-wise comparison costs:"<<double(endTime-startTime) << "\t" <<CLOCKS_PER_SEC<<endl;
      double time_use=double(tv_end.tv_sec-tv_begin.tv_sec)*1000000+double(tv_end.tv_usec-tv_begin.tv_usec);
      //cout<<"pair-wise comparison costs:"<<time_use<<endl;
      //cout<<"entropy:"<<abs(shannon_1- shannon_2)<<endl;
      //cout<<"similarity:"<<score1<<endl;
      //cout << abs(shannon_1- shannon_2) <<"\t"<< 1 <<endl;
      //cout<<score1 <<"\t"<< 2 <<endl;

      
      return result;
      //return Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);
      
      }











bool Filter_main(const char * sam1, const char * sam2,float &score1,float &score2){

      //time_t startTime,secTime,endTime;
      //startTime=clock();


      float * Abd_1 = new float[LeafN];
      float * Abd_2 = new float[LeafN];
      
      float shannon_1=Load_abd(sam1, Id, Abd_1);
      float shannon_2=Load_abd(sam2, Id, Abd_2);
      //secTime=clock();


      if(abs(shannon_1-shannon_2) < 1.5){
        //time_t startTime,endTime;
        //startTime=clock();

        Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);

        //endTime=clock();
        //cout<<"F: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
        //cout<<sam1<<"\t"<<sam2<<endl;
        return false;
        }
      else{
        //time_t startTime,endTime;
        //startTime=clock();
        score1=0;
        score2=0;
        //endTime=clock();
        //cout<<"Load: "<<double(secTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
        //cout<<"Total: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
        //cout<<sam1<<"\t"<<sam2<<endl        return true;
      }



}


bool Filter_main(float * Abd_1, const char * sam2,float shannon_1,float &score1,float &score2){

      //time_t startTime,secTime,endTime;
      //startTime=clock();


      //float * Abd_1 = new float[LeafN];
      float * Abd_2 = new float[LeafN];
      
      //float shannon_1=Load_abd(sam1, Id, Abd_1);
      float shannon_2=Load_abd(sam2, Id, Abd_2);
      //secTime=clock();


      if(abs(shannon_1-shannon_2) < 1.5){
        //time_t startTime,endTime;
        //startTime=clock();

        Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);

        //endTime=clock();
        //cout<<"F: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
        //cout<<sam1<<"\t"<<sam2<<endl;
        return false;
        }
      else{
        //time_t startTime,endTime;
        //startTime=clock();
        score1=0;
        score2=0;
        //endTime=clock();
        //cout<<"Load: "<<double(secTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
        //cout<<"Total: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
        //cout<<sam1<<"\t"<<sam2<<endl        return true;
      }



}

bool Filter_main(const char * sam1, const char * sam2,float &score1,float &score2,float &shannon){
      float * Abd_1 = new float[LeafN];
      float * Abd_2 = new float[LeafN];
      
      float shannon_1=Load_abd(sam1, Id, Abd_1);
      float shannon_2=Load_abd(sam2, Id, Abd_2);


      if(abs(shannon_1-shannon_2) < 1.5){
        //time_t startTime,endTime;
        //startTime=clock();

        Calc_sim(Abd_1, Abd_2, Order_1, Dist_1, Order_2, Dist_2, Order_d,score1,score2);

        //endTime=clock();
        //cout<<"F: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
        //cout<<sam1<<"\t"<<sam2<<endl;
        shannon=abs(shannon_1-shannon_2);
        return false;
        }
      else{
        //time_t startTime,endTime;
        //startTime=clock();
        score1=0;
        score2=0;
        //endTime=clock();
        //cout<<"T: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
        //cout<<sam1<<"\t"<<sam2<<endl;
        shannon=abs(shannon_1-shannon_2);
        
        return true;
      }





}


#endif
