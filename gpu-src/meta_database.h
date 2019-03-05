#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <time.h>

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>


#include <stdlib.h>
#include <unistd.h>

#include "index_entry.h"
//#include "tree_class.h"
#include "cfn.h"
#include "comp.h"
#include "gpu_compare16.cu"
//#include "gpu_compare1.0.cu"

#ifndef META_DATABASE_H
#define META_DATABASE_H

#define Buffer_Size 1000
#define i_min 15
#define q_min 1
#define q_max 20

#define dom_t 60 //change

using namespace std;

class Meta_Result{
      private:
        float m_value;
        float n_value;
        Index_Entry * entry;
        basic_entry * b_entry;
        bool be_filter;
        //to be delete
        float shannon;

      
      public:
             
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
       
class Meta_Database{ 
      public:
      Meta_Database(){
                      this->Entry_count = 0;
                      this->Entry_number = 0;
                      this->Key_dim = 0;
                      this->Name = "";   
                      this->Abundance = 0;    
                      //Comp_init();                                                           
                      }
       Meta_Database(string _name, int dim){
                      this->Entry_count = 0;
                      this->Entry_number = 0;
                      this->Key_dim = dim;
                      this->Name = _name; 
                      this->Abundance = 0;  
                      //Comp_init();    
                      }
        Meta_Database(string _name, string path, int dim){
                           this->Entry_count = 0;
                           this->Entry_number = 0;
                           this->Key_dim = dim;
                           this->Name = _name; 
                           this->Abundance = 0;

                           this->Make_Index(path/*,path.length()*/);
                          
                           
                           //Comp_init();   
                           }
      Meta_Database(string infilename,string biotype){
                           this->Entry_count = 0;
                           this->Entry_number = 0;
                           this->Abundance = 0;    
                           this->Load_Index(infilename,biotype);
                           //this->result_score=new float [this->Entry_count+ 3000];




                           //this->display_t_table(Meta_index);
                           //this->display_b_table(Meta_biomes);
                           //cout<<"taxon size:"<<Meta_index.size()<<endl;
                           //cout<<"biome size:"<<Meta_biomes.size()<<endl;
                           //Comp_init();   
                           }
      Meta_Database(string infilename,string biotype,int * gpulist,int count){
                           this->Entry_count = 0;
                           this->Entry_number = 0;
                           this->Abundance = 0;    
                           //gpu_step.initial_gpu(gpulist,count);
                           this->Load_Index(infilename,biotype);
                            }                       

      ~Meta_Database(){};
      
      void Insert_Index_by_Entry(Index_Entry * entry);
      void Insert_Group_by_Entry(Index_Entry * entry);
      
      void Insert_Index_by_File(string infilename, biomes_entry * b_entry,unsigned int group);
      void Insert_Index_by_File(string infilename, unsigned int group);
      
      int Make_Index(string path/*,int length*/);
      int Make_Index_Add(string path, string biotype,unsigned int group);
      
      //unsigned int Query(string infilename);
      //unsigned int Exhaustive_Query(string infilename);
      unsigned int Parallel_Indexed_Query(ostream & out, string infilename, string biotype,int n, int t_number,int group,string scoringtype, string filterflag);


      unsigned int Parallel_Exhaustive_Query(ostream & out, string infilename, int n, int t_number, int group,string scoringtype,string filterflag);
      
      int Out_Index(string outfilename);
      int Load_Index(string infilename,string biotype);
      int Merge_Index(string infilename);
      
      int Get_Entry_Count(){ return this->Entry_count; };
      int Get_Key_Dim(){ return this->Key_dim; };
      int Get_Key_Count(){ return this->Meta_index.size(); };
      
      float Get_Abundance();
      float Update_Abundance();

      void clust_index(string query_key, string biotype);
      int decide_to_clust(vector<Index_Entry *>* entry, vector<biomes_entry* >* b_entry);

      //RAM version
      unsigned int Parallel_Exhaustive_Query_RAM(ostream & out, string infilename, vector<string> &databaselist, map<string,float *> &database_map, int n, int t_number, int group,int is_gpu);
      unsigned int Parallel_Indexed_Query_RAM(ostream & out, string infilename,string biotype, vector<string> &databaselist, map<string,float *> &database_map, int n, int t_number,int group,string scoringtype, string filterflag,int is_gpu);
      unsigned int Parallel_Indexed_Query_RAM(string infilename, string query_key, string biotype, map<string,float* > &database_map, Meta_Result * results, int n, int t_number, int group, string scoringtype, string filterflag,double &comparison_time,int is_gpu);





    private:
              string Name;
              map<string, vector<Index_Entry * > * > Meta_index;
              map<string,vector<biomes_entry* > *> Meta_biomes;
              map<string,vector<basic_entry * > > Meta_merge;
              map<unsigned int, vector<Index_Entry * > * > Meta_groups;
              map<vector<string>, string> Meta_key;
              unsigned int Entry_count;
              unsigned int Entry_number; //ID
              unsigned int Key_dim;      
              int Abundance;
              
              //for gpu compare
              gpu_compare gpu_step;
              float * result_score;
              

              //string Path;                            
              static int match_count;


              unsigned int Get_Entry_List(Index_Entry * * list, int group);
              unsigned int Get_Entry_List(Index_Entry * * list, string query_key, int group);
              unsigned int Get_Entry_List(basic_entry ** list2,vector<basic_entry *> merge_res, int group);

              
              unsigned int Parallel_Indexed_Query(string infilename, string query_key, string biotype,Meta_Result * results, int n, int t_number, int flag,int group);
              //unsigned int Parallel_Indexed_Query(string infilename, string query_key, string biotype,Meta_Result * results, int n, int t_number, int group);             
              
              unsigned int Parallel_Query(string infilename, Index_Entry * * list, Meta_Result * results, int n, int t_number, int count, string scoringtype,string filterflag);
              unsigned int Parallel_Query(string infilename, basic_entry * * list, Meta_Result * results, int n, int t_number, int count, string scoringtype,string filterflag);
              static void * Parallel_Query_Static(void * args);
              vector<basic_entry*> real_merge_index_cluster(vector <Index_Entry*> fir, vector<biomes_entry*> sec);
              vector<basic_entry*> real_merge_index_cluster(vector <Index_Entry*> fir, vector<basic_entry*> sec);
              int merge_index_cluster(vector <Index_Entry*> *fir, vector<biomes_entry*> *sec);
              double comp_index_cluster(vector<Index_Entry*> fir, vector<biomes_entry*> sec);
              //void clust_index();
              void display_table(map<string,vector<basic_entry* >  > sample);
              void display_t_table(map<string,vector<Index_Entry* > * > sample);
              void display_b_table(map<string,vector<biomes_entry* >  *> sample);
              void query_t_table(map<string,vector<Index_Entry* > * > sample,string query_key);
              void display_samples(Index_Entry ** entry, int count);
              string make_new_key(string fir,string sec);
              string get_new_key(string fir,string sec);


              //test new score:
              //unsigned int Parallel_Indexed_Query(string infilename, string query_key, string biotype,Meta_Result * results, Meta_Result * new_results,int n, int t_number, int flag ,int group);
              unsigned int Parallel_Indexed_Query(string infilename, string query_key, string biotype,Meta_Result * results, int n, int t_number, int group,string scoringtype,string filterflag);

              //RAM version
              static void * Parallel_Query_Static_RAM(void * args);
              unsigned int Parallel_Query_RAM(string infilename, Index_Entry ** list, map<string,float* > &database_map,Meta_Result * results,int n, int t_number, int count,int group,string scoringtype,string filterflag,double &comparison_time);
              unsigned int Parallel_Query_RAM(string infilename, basic_entry ** list, map<string,float* > &database_map,Meta_Result * results,int n, int t_number, int count,int group,string scoringtype, string filterflag,double &comparison_time);

              //GPU version
              //unsigned int GPU_Parallel_Query_RAM(string infilename, basic_entry ** list, map<string,float* > &database_map,Meta_Result * results,int n, int t_number, const int count,int group,string scoringtype,string filterflag,double &comparison_time,gpu_compare * gpu_step);
              //unsigned int GPU_Parallel_Query_RAM(string infilename, Index_Entry ** list, map<string,float* > &database_map,Meta_Result * results,int n, int t_number, const int count,int group,string scoringtype,string filterflag,double &comparison_time,gpu_compare * gpu_step);
              unsigned int GPU_Parallel_Query_RAM(string infilename, basic_entry ** list, map<string,float* > &database_map,Meta_Result * results,int n, int t_number, const int count,int group,string scoringtype,string filterflag,double &comparison_time);
              unsigned int GPU_Parallel_Query_RAM(string infilename, Index_Entry ** list, map<string,float* > &database_map,Meta_Result * results,int n, int t_number, const int count,int group,string scoringtype,string filterflag,double &comparison_time);

              
              
                    };//end Meta_Database


int get_compData(CompData *cd)
{
    cout<<"begin to copy"<<endl;
    memcpy(cd->Dist_1, Dist_1, sizeof(float) * OrderN);
    memcpy(cd->Dist_2, Dist_2, sizeof(float) * OrderN);
    memcpy(cd->Order_1, Order_1, sizeof(int) * OrderN);
    memcpy(cd->Order_2, Order_2, sizeof(int) * OrderN);
    memcpy(cd->Order_d, Order_d, sizeof(int) * OrderN);
    memcpy(cd->Id, Id, sizeof(int) * OrderN);
    return 1;
}

                    
              

struct Argument{
       
       string infilename;
       Index_Entry * * list;
       basic_entry * * list2;
       vector<string> * p_list;
       map<string,float *> * p_database;
       map<string, float *> database_map;
       int thread;
       int per_number;
       int count;
       int after_filter_count;
       string filterflag;
       //string * tree_file;
       int flag;
       double comparison_time;
       Meta_Result * buffer;

       };

#endif



