
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "meta_database.h"



using namespace std;

//const double threshold =0.4;
const double newthreshold=0.15;

void display_database(map<string,float *>  temp_entry){
  cout<<"begin to display_database"<<endl;
  map<string,float *> ::iterator map_iter;
  for(map_iter=temp_entry.begin();map_iter!=temp_entry.end();map_iter++){
    cout<<map_iter->first<<endl;
    float * abd=map_iter->second;
    /*for(int i=0;i<LeafN;i++)
        {
          cout<<abd[i]<<"\t"<<i<<endl;
        }
    */
    for(int i=0;i< LeafN;i++)
    {
        if(abd[i]!=0)
          cout<<abd[i]<<endl;
          //continue;
    }
    //cout<<endl;
  }
  cout<<"end of display"<<endl;

}



void Meta_Database::Insert_Index_by_Entry(Index_Entry * entry){
                         
            if (this->Meta_index.count(entry->Get_key())==0)
                                         this->Meta_index[entry->Get_key()] = new vector<Index_Entry * >;
          
            this->Meta_index[entry->Get_key()]->push_back(entry);

            
            this->Entry_count ++;

            if (entry->Get_abundance() > dom_t)
               this->Abundance ++;

           }
           

void Meta_Database::Insert_Group_by_Entry(Index_Entry * entry){
     
           if (this->Meta_groups.count(entry->Get_group())==0)
                                                this->Meta_groups[entry->Get_group()] = new vector<Index_Entry *>;
                                              
           this->Meta_groups[entry->Get_group()]->push_back(entry);
     

     }

void Meta_Database::Insert_Index_by_File(string infilename,biomes_entry* b_entry, unsigned int group){
          
          string keys[this->Key_dim];
          float weight[this->Key_dim];
                    
          //taxon index
          Get_Key_Weight((infilename + "/classification.txt").c_str(), keys, weight, this->Key_dim);
          
          Index_Entry * entry = new Index_Entry(this->Entry_number, group, keys, weight, this->Key_dim, infilename, i_min);
          
          //biomes index
          b_entry->Set_id(this->Entry_number);
          b_entry->Set_group(group);
          if (this->Meta_biomes.count(b_entry->getbiomes()) == 0)
                                      this->Meta_biomes[b_entry->getbiomes()] = new vector<biomes_entry *>;

          Meta_biomes[b_entry->getbiomes()]->push_back(b_entry);

          
          this->Entry_number ++; 
          
          this->Insert_Index_by_Entry(entry);
          
          this->Insert_Group_by_Entry(entry);
          
          }

void Meta_Database::Insert_Index_by_File(string infilename, unsigned int group){
          
          string keys[this->Key_dim];
          float weight[this->Key_dim];
                    
          //taxon index
          Get_Key_Weight((infilename + "/classification.txt").c_str(), keys, weight, this->Key_dim);
          
          Index_Entry * entry = new Index_Entry(this->Entry_number, group, keys, weight, this->Key_dim, infilename, i_min);
          
          
          this->Entry_number ++; 
          
          this->Insert_Index_by_Entry(entry);
          
          this->Insert_Group_by_Entry(entry);
          
          }


int Meta_Database::Make_Index(string path){
    
    //if (this->Meta_index.empty())
    //   this->Meta_index.clear();
       
    //this->Entry_count = 0;

  
    //this->Entry_number = 0;
    
    //this->Path = path;

    struct dirent* ent = NULL;
    DIR *pDir;
    pDir=opendir(path.c_str());
    if (pDir == NULL){
     cerr<<"Error: Cannot Open Path : " << path << endl;
     exit(0);
     }
 
    while ((ent=readdir(pDir)) != NULL)
          if ((ent->d_type == 4) && (ent->d_name[0]!= '.')){
                           
             string inpathname = path + "/" + ent->d_name;
             
             string infilename = inpathname + "/classification.txt";
             ifstream infile(infilename.c_str(), ifstream::in); 
              
             if (infile != NULL) 
                                 {
                                 infile.close();
                                 infile.clear();
                                 //cout << inpathname << endl;//try

                                 //biomes index
                                 biomes_entry* entry= new biomes_entry(ent->d_name,path);

                                 

                                 //taxon index
                                 this->Insert_Index_by_File(inpathname,entry,0);
                                                                                                   
                                 }
             else{ 
             this->Make_Index(inpathname);

             }
           }
           
    closedir(pDir);
          
    return this->Entry_count;
    }

int Meta_Database::Make_Index_Add(string path,string biotype ,unsigned int group){

    struct dirent* ent = NULL;
    DIR *pDir;
    pDir=opendir(path.c_str());
    if (pDir == NULL){
     cerr<<"Error: Cannot Open Path : " << path << endl;
     exit(0);
     }
 
    unsigned int _entry_count = 0;
    
    while ((ent=readdir(pDir)) != NULL)
          if ((ent->d_type == 4) && (ent->d_name[0]!= '.')){
             
             string inpathname = path + "/" + ent->d_name;
             
             string infilename = inpathname + "/classification.txt";
             ifstream infile(infilename.c_str(), ifstream::in); 
              
             if (infile != NULL) 
                                 {
                                 infile.close();
                                 infile.clear();
                                 //cout << inpathname << endl;
                                  if(biotype != "None")
                                  {
                                  biomes_entry* entry= new biomes_entry(biotype,path);
                                 this->Insert_Index_by_File(inpathname,entry,group);
                                 _entry_count ++;}
                                 else
                                 {
                                  this->Insert_Index_by_File(inpathname,group);
                                  _entry_count ++;

                                 }
                                                                                                   
                                 }
             else _entry_count += this->Make_Index_Add(inpathname, biotype,group);
           }
    closedir(pDir);               
    return _entry_count;
    }


 //query part
unsigned int Meta_Database::Get_Entry_List(Index_Entry * * list, int group){
         
         unsigned int count = 0;
         
         map<string, vector<Index_Entry *> *>::iterator map_iter;
         
         for (map_iter = this->Meta_index.begin(); map_iter != this->Meta_index.end(); map_iter ++){
             
                      vector<Index_Entry *>::iterator vect_iter;
                      for (vect_iter = map_iter->second->begin(); vect_iter != map_iter->second->end(); vect_iter++)
                          if (((*vect_iter)->Get_group() == group)||(group < 0))     
                                          {
                                           list[count] = *vect_iter;                          
                                           count ++;
                                           }
                 }
          //display_samples(list,count);
         
         
        return count; 
         
        }



unsigned int Meta_Database::Get_Entry_List(Index_Entry * * list,string query_key,int group){
         
         unsigned int count = 0;
         
         if (this->Meta_index.count(query_key) == 0) return 0;
         
         vector<Index_Entry *>::iterator vect_iter;
         //vector<basic_entry *>::iterator vect_iter2;
         for (vect_iter = this->Meta_index[query_key]->begin(); vect_iter != this->Meta_index[query_key]->end(); vect_iter ++)
                        if (((*vect_iter)->Get_group() == group) || (group < 0))     
                                          {
                                           
                                           list[count] = *vect_iter;
                          
                                           count ++;
                                           }

  
         return count;
         
         }

unsigned int Meta_Database::Get_Entry_List(basic_entry ** list2, vector<basic_entry *> merge_res, int group){
         
         unsigned int count = 0;
         if (merge_res.size() == 0) return 0;
         
         vector<basic_entry *>::iterator vect_iter;
         for (vect_iter = merge_res.begin(); vect_iter != merge_res.end(); vect_iter ++)   
                                          {
                                           
                                           list2[count] = *vect_iter;
                          
                                           count ++;
                                           }

         return count;
         
         }



/*
void * Meta_Database::Parallel_Query_Static_RAM(void * args){

      string infilename = ((Argument *)args)->infilename;


     int thread = ((Argument *)args)->thread;
     int per_number = ((Argument *)args)->per_number;
     int count = ((Argument *)args)->count;
     Index_Entry * * list=((Argument *)args)->list;
 
     Meta_Result * buffer = ((Argument *)args)->buffer;
    


     vector<string> *p_list=((Argument *)args)->p_list;
     map<string,float *> *p_database=((Argument *)args)->p_database;
     //double thread_comparison_time= ((Argument *)args)->comparison_time;

     float * Abd_1=new float[LeafN];
     Load_abd(infilename.c_str(),Id,Abd_1);
     double thread_comparison_time=0;



     for (int i =0; i < per_number; i++){//Loop in each thread
                                               int sam_file =  thread * per_number + i;
                                               //cout<<"sam_file:"<<sam_file<<endl;
                                               if (sam_file >= count) break;
                                                             
                                               string entry_filename = list[sam_file]->getfilename() + "/classification.txt";
                                              
                                               
                                               float * Abd_2=new float[LeafN];
                                               
                                               Abd_2=p_database->find(entry_filename)->second;
                                               //cout<<"compare:"<<entry_filename<<endl;
                                              
                                               clock_t startTime,endTime;
                                               startTime=clock();  
                                               Comp_sam_RAM(Abd_1,Abd_2,
                                               buffer[sam_file].m_value,buffer[sam_file].n_value);
                                               endTime=clock();
                                               double single_comparison_time=double(endTime-startTime);
                                               thread_comparison_time=thread_comparison_time+single_comparison_time;



                    
                                               buffer[sam_file].b_entry = list[sam_file];


                                                                                              
                                               }
      ((Argument *)args)->comparison_time=thread_comparison_time;

       pthread_exit(0);
                                         
}


*/




void * Meta_Database::Parallel_Query_Static_RAM(void * args){

  string infilename = ((Argument *)args)->infilename;


     int thread = ((Argument *)args)->thread;
     int per_number = ((Argument *)args)->per_number;
     int count = ((Argument *)args)->count;
     //Index_Entry * * list=((Argument *)args)->list;
     int flag = ((Argument *)args)->flag;
 
     Meta_Result * buffer = ((Argument *)args)->buffer;
    


     vector<string> *p_list=((Argument *)args)->p_list;
     map<string,float *> *p_database=((Argument *)args)->p_database;
     //double thread_comparison_time= ((Argument *)args)->comparison_time;

     float * Abd_1=new float[LeafN];
     Load_abd(infilename.c_str(),Id,Abd_1);
     double thread_comparison_time=0;


     if(flag==0){

      Index_Entry * * list=((Argument *)args)->list;

     for (int i =0; i < per_number; i++){//Loop in each thread
                                               int sam_file =  thread * per_number + i;
                                               //cout<<"sam_file:"<<sam_file<<endl;
                                               if (sam_file >= count) break;
                                                             
                                               string entry_filename = list[sam_file]->getfilename() + "/classification.txt";
                                              
                                               
                                               float * Abd_2=new float[LeafN];
                                               
                                               Abd_2=p_database->find(entry_filename)->second;
                                               

                                              
                                               clock_t startTime,endTime;
                                               startTime=clock();  
                                               Comp_sam_RAM(Abd_1,Abd_2,
                                               buffer[sam_file].m_value,buffer[sam_file].n_value);
                                               endTime=clock();
                                               double single_comparison_time=double(endTime-startTime);
                                               thread_comparison_time=thread_comparison_time+single_comparison_time;

                    
                                               buffer[sam_file].b_entry = list[sam_file];
                                                                                              
                                               }

                                  }



      else{
        basic_entry ** list=((Argument *)args)->list2;
        for (int i =0; i < per_number; i++){//Loop in each thread
                                               int sam_file =  thread * per_number + i;
                                               //cout<<"sam_file:"<<sam_file<<endl;
                                               if (sam_file >= count) break;
                                                             
                                               string entry_filename = list[sam_file]->getfilename() + "/classification.txt";
                                              
                                               
                                               float * Abd_2=new float[LeafN];
                                               
                                               Abd_2=p_database->find(entry_filename)->second;
                                               

                                              
                                               clock_t startTime,endTime;
                                               startTime=clock();  
                                               Comp_sam_RAM(Abd_1,Abd_2,
                                               buffer[sam_file].m_value,buffer[sam_file].n_value);
                                               endTime=clock();
                                               double single_comparison_time=double(endTime-startTime);
                                               thread_comparison_time=thread_comparison_time+single_comparison_time;

                    
                                               buffer[sam_file].b_entry = list[sam_file];
                                                                                              
                                               }        


      }

      
      ((Argument *)args)->comparison_time=thread_comparison_time;

       pthread_exit(0);
                                         
}


void * Meta_Database::Parallel_Query_Static(void * args){
     //time_t startTime,endTime;
     //startTime=clock();
     
     string infilename = ((Argument *)args)->infilename;
     //cout<<"query static:"<<infilename<<endl;
     int flag=((Argument *)args)->flag;
     //cout<<"flag:"<<flag<<endl;
     //basic_entry * * list;
     int thread = ((Argument *)args)->thread;
     int per_number = ((Argument *)args)->per_number;
     int count = ((Argument *)args)->count;
     int after_filter_count=((Argument *)args)->after_filter_count;
     string filter_flag = ((Argument *)args)->filterflag;
     float * Abd_1 = new float[LeafN];
     float shannon_1=Load_abd(infilename.c_str(),Id,Abd_1);





     if(flag==1){
      basic_entry * * list = ((Argument *)args)->list2;
      Meta_Result * buffer = ((Argument *)args)->buffer;
    
     for (int i =0; i < per_number; i++){//Loop in each thread
                                               //time_t startTime,endTime;
                                               //startTime=clock();
                                               int sam_file =  thread * per_number + i;
                                               if (sam_file >= count) break;
                                                         
                                               string entry_filename = list[sam_file]->getfilename() + "/classification.txt";

                                               //origin 
                                               //buffer[sam_file].m_value = Comp_sam(infilename.c_str(), entry_filename.c_str());
                                               if(filter_flag=="F"){
                                                //time_t startTime,endTime;
                                                //startTime=clock();
                                               //Comp_sam(infilename.c_str(), entry_filename.c_str(),buffer[sam_file].m_value,buffer[sam_file].n_value);
                                               // to be delete
                                               Comp_sam(Abd_1, entry_filename.c_str(),buffer[sam_file].m_value,buffer[sam_file].n_value);
                                               //buffer[sam_file].b_entry=list[sam_file]; 
                                               after_filter_count++;
                                               //endTime=clock();
                                               //cout<<"F: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
                                               //cout<<infilename<<"\t"<<entry_filename<<endl;
                                                }

                                               
                                               else{

                                                //time_t startTime,endTime;
                                                //startTime=clock();
                                               buffer[sam_file].be_filter=Filter_main(Abd_1, entry_filename.c_str(),shannon_1,buffer[sam_file].m_value,buffer[sam_file].n_value);
                                               //cout<<"shannon:"<<buffer[sam_file].shannon<<endl;

                                               if(buffer[sam_file].be_filter == false){
                                               buffer[sam_file].b_entry = list[sam_file];
                                               after_filter_count++;
                                               //cout<<"unfiltered\t";

                                                }

                                                /*else{
                                                  //cout << buffer[sam_file].m_value<<"\t"<<buffer[sam_file].n_value<<endl;
                                                  //cout<<"filtered\t";
                                                  //continue;
                                                }
                                                */

                                               //endTime=clock();
                                               //cout<<"T: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
                                               //cout<<infilename<<"\t"<<entry_filename<<endl;


                                              }


                                               //cout << (float)(j + 1) / (float)(per_number) * 100.0 << "%\tfinished" << endl;
                                                                                              
                                               }
                                                 ((Argument *)args)->count=count;
                                                 ((Argument *)args)->after_filter_count=after_filter_count;
                                                 //endTime=clock();
                                                 //cout<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
                                                 //cout<<"after_filter_count:"<<after_filter_count<<endl;                                              
                                           pthread_exit(0);

    }
     else if(flag==0){
      Index_Entry * * list=((Argument *)args)->list;
      //string * tree_file = ((Argument *)args)->tree_file; 
      Meta_Result * buffer = ((Argument *)args)->buffer;

     for (int i =0; i < per_number; i++){//Loop in each thread
                                               int sam_file =  thread * per_number + i;
                                               //cout<<"sam_file:"<<sam_file<<endl;
                                               if (sam_file >= count) break;
                                                             
                                               string entry_filename = list[sam_file]->getfilename() + "/classification.txt";
                                               //cout<<list[sam_file]->getfilename()<<endl;

                                               if(filter_flag=="F"){
                                               Comp_sam(Abd_1, entry_filename.c_str(),buffer[sam_file].m_value,buffer[sam_file].n_value);
                                               buffer[sam_file].b_entry=list[sam_file];
                                               //cout<<buffer[sam_file].m_value<<"\t"<<buffer[sam_file].n_value<<endl;

                                               after_filter_count++;
                                                }

                                               
                                               else{
                                               buffer[sam_file].be_filter=Filter_main(Abd_1, entry_filename.c_str(), shannon_1,buffer[sam_file].m_value,buffer[sam_file].n_value);


                                               if(buffer[sam_file].be_filter == false){
                                               buffer[sam_file].b_entry = list[sam_file];
                                               after_filter_count++;

                                                }
                                                else{
                                                  //cout <<  buffer[sam_file].m_value<<"\t"<<buffer[sam_file].n_value<<endl;
                                                  continue;
                                                }

                                              }


                                                                                              
                                               }

                                                ((Argument *)args)->count=count;
                                                ((Argument *)args)->after_filter_count=after_filter_count;
                                                 //cout<<"after_filter_count:"<<after_filter_count<<endl;    
                                           pthread_exit(0);
                                         }


     
     }


unsigned int Meta_Database::GPU_Parallel_Query_RAM(string infilename, basic_entry ** list, map<string,float* > &database_map,
  Meta_Result * results,int n, int t_number, const int count,int group,string scoringtype,string filterflag,
  double &comparison_time){

                  t_number = (t_number < count)? t_number : count;

                  struct timeval tv_begin,tv_end;
                  gettimeofday(&tv_begin,NULL);


                              
                  if (t_number == 0 || t_number > sysconf(_SC_NPROCESSORS_CONF))
                           t_number = sysconf(_SC_NPROCESSORS_CONF);
                  CompData * cu = new CompData[1];
                  
                  //cout<<"begin to gpu step 1"<<endl;

                  //Comp_init();
                  get_compData(cu);
                  struct Abd * abd1 = new struct Abd[1];
                  struct Abd * total_abd2 =new struct Abd[count];
                  
                  //Abd abd1;

                  

                  //Abd total_abd2[count];

                  //cout<<"begin to gpu step 2"<<endl;

                  float  * result_score = new float[count];

                  

                  Meta_Result buffer[count];

                  //abd1->name=infilename;
                  //abd1->data=database_map[infilename];


                   // for gpu 1.0 version 
                  
                  abd1[0].name=infilename;
                  float t_abd[LeafN];
                  Load_abd(infilename.c_str(),Id,t_abd);

                  memcpy(abd1->data,t_abd,sizeof(float)*LeafN);
              

                  //abd1->name=infilename
                  //abd1->data=database_map[name];
                  for(int i=0;i < count; i++){
                    string name=list[i]->getfilename()+"/classification.txt";
                    

                    total_abd2[i].name=name;

                    //cout<<name<<endl;

                    if(database_map.find(name)!=database_map.end()){
                          //cout<<name<<endl;
                          memcpy(total_abd2[i].data,database_map[name],sizeof(float)*LeafN);
                        }

                    else
                        cout<<"Entry "<<name<<" not found in database"<<endl;
                    //total_abd2[i].data=database_map[name];

                  }

                  
                  
                  /*
                  // fro gpu 1.1 version
                  abd1[0].name=infilename;
                  abd1[0].data=database_map[infilename];

                  for(int i=0;i<count;i++){
                    string name=list[i]->getfilename()+"/classification.txt";
                    total_abd2[i].name=name;
                    if(database_map.find(name)!=database_map.end()){
                          //cout<<name<<endl;
                          //memcpy(total_abd2[i].data,database_map[name],sizeof(float)*LeafN);
                          //total_abd2[i]={name,&database_map[name]};
                          total_abd2[i].data=database_map[name];
                        }

                    else
                        cout<<"Entry "<<name<<" not found in database"<<endl;

                  }

                    */

                  //scoring type
                  int version;
                  if(scoringtype=="T"){
                    version=1;
                  }
                  else{
                    version=0;
                  }



                  //begin to create object
                  //cout<<"begin to gpu step 3"<<endl;
                  //struct Abd* p1=&abd1;
                  //struct Abd* p2=&total_abd2[0];

                  //initial var to calculate the running time
                


                  struct timeval SD_begin;
                  struct timeval act_begin;
                  struct timeval GR_begin,GR_end;

                  gettimeofday(&tv_begin,NULL);
                  //gpu_compare gpu_step;
                  //gpu_step.sendData(cu,p1,p2,count,version);
                  gettimeofday(&SD_begin,NULL);
                  this->gpu_step.sendData(cu,abd1,total_abd2,count,version);
                  gettimeofday(&act_begin,NULL);
                  this->gpu_step.act();
                  gettimeofday(&GR_begin,NULL);
                  this->gpu_step.getResult(result_score);
                  gettimeofday(&GR_end,NULL);
                  

                  delete [] abd1;
                  delete [] total_abd2;



                  gettimeofday(&tv_end,NULL);

                  double struct_time=double(SD_begin.tv_sec-tv_begin.tv_sec)*1000000+double(SD_begin.tv_usec-tv_begin.tv_usec);
                  double sendData_time=double(act_begin.tv_sec-SD_begin.tv_sec)*1000000+double(act_begin.tv_usec-SD_begin.tv_usec);
                  double act_time=double(GR_begin.tv_sec-act_begin.tv_sec)*1000000+double(GR_begin.tv_usec-act_begin.tv_usec);
                  double getResult_time=double(GR_end.tv_sec-GR_begin.tv_sec)*1000000+double(GR_end.tv_usec-GR_begin.tv_usec);

                  comparison_time=double(tv_end.tv_sec-tv_begin.tv_sec)*1000000+double(tv_end.tv_usec-tv_begin.tv_usec);

                  cout<<"struct_time:"<<(float)struct_time/ 1000000<<endl;
                  cout<<"sendData_time:"<<(float)sendData_time/ 1000000<<endl;
                  cout<<"act_time:"<<(float)act_time / 1000000<<endl;
                  cout<<"getResult_time:"<<(float)getResult_time / 1000000<<endl;



                  for(int i=0;i<count;i++){
                      cout<<result_score[i]<<' '<<i<<endl;

                      buffer[i].b_entry=list[i];
                      buffer[i].m_value=result_score[i];


                  }

                  
                  cout<<"finish gpu parallel computing step "<<endl;
                   for (int i = 0; i < count; i++){
                          //cout<<"buffer[i].m_value"<<buffer[i].m_value<<endl;
                      if (buffer[i].m_value > results[n-1].m_value){
                                      results[n-1] =  buffer[i];
                                      for (int j = n-1; j>0; j--)
                                          if (results[j].m_value > results[j-1].m_value){
                                                             Meta_Result temp = results[j];
                                                             results[j] = results[j-1];
                                                             results[j-1] = temp;                                                             
                                                             }
                                          else break;                                                                              
                                       }}



                  if (n < count) return n;
                  else return count;


                  //rearrange the dataset







} 


unsigned int Meta_Database::GPU_Parallel_Query_RAM(string infilename, Index_Entry ** list, map<string,float* > &database_map,
  Meta_Result * results,int n, int t_number, const int count,int group,string scoringtype,string filterflag,
  double &comparison_time){

                  t_number = (t_number < count)? t_number : count;
                              
                  if (t_number == 0 || t_number > sysconf(_SC_NPROCESSORS_CONF))
                           t_number = sysconf(_SC_NPROCESSORS_CONF);
                  CompData * cu = new CompData[1];
                  //Comp_init();
                  //cout<<"begin to gpu step 1"<<endl;
                  get_compData(cu);
                  //struct Abd * abd1 = new struct Abd[1];
                  //struct Abd * total_abd2 =new struct Abd[count];
                  //cout<<"begin to gpu step 2"<<endl;
                  struct Abd abd1;
                  struct Abd total_abd2[count];


                  //struct Abd * abd1 = new struct Abd[1];
                  //struct Abd * total_abd2 =new struct Abd[count];


                  float  * result_score = new float[count];
                  Meta_Result buffer[count];

                  //abd1->name=infilename;
                  //abd1->data=database_map[infilename];


                  //for  gpu 1.0 version
                   
                  abd1.name = infilename;
                  memcpy(abd1.data,database_map[infilename],sizeof(float)*LeafN);

                  for(int i=0;i < count; i++){
                    string name=list[i]->getfilename();
                    //cout<<name<<endl;

                    //total_abd2[i]->name=name;
                    //total_abd2[i]->data=database_map[name];

                    total_abd2[i].name=name;
                    memcpy(total_abd2[i].data,database_map[name],sizeof(float)*LeafN);

                  }
                  


                  /*
                  //for gpu 1.1 version
                  abd1[0].name=infilename;
                  abd1[0].data=database_map[infilename];

                  for(int i=0;i<count;i++){
                    string name=list[i]->getfilename()+"/classification.txt";
                    total_abd2[i].name=name;
                    if(database_map.find(name)!=database_map.end()){
                          //cout<<name<<endl;
                          //memcpy(total_abd2[i].data,database_map[name],sizeof(float)*LeafN);
                          total_abd2[i].data=database_map[name];
                        }

                    else
                        cout<<"Entry "<<name<<" not found in database"<<endl;

                  }
                  */
                  //scoring type
                  int version;
                  if(scoringtype=="T"){
                    version=1;
                  }
                  else{
                    version=0;
                  }





                  //begin to create object
                  struct Abd* p1=&abd1;
                  struct Abd* p2=&total_abd2[0];


                  //CompData * cu = new CompData[1];
                  get_compData(cu);
                  gpu_compare * gpu_step=new gpu_compare;
                  //gpu_step->sendCompData(cu);

                  //gpu_compare gpu_step;
                  gpu_step->sendData(cu,p1,p2,count,version);
                  gpu_step->act();
                  gpu_step->getResult(result_score);

                  for(int i=0;i<count;i++){
                     //cout<<result_score[i]<<endl;
                     buffer[i].entry=list[i];
                     buffer[i].m_value=result_score[i];


                  }

                  

                   for (int i = 0; i < count; i++){
                          //cout<<"buffer[i].m_value"<<buffer[i].m_value<<endl;
                      if (buffer[i].m_value > results[n-1].m_value){
                                      results[n-1] =  buffer[i];
                                      for (int j = n-1; j>0; j--)
                                          if (results[j].m_value > results[j-1].m_value){
                                                             Meta_Result temp = results[j];
                                                             results[j] = results[j-1];
                                                             results[j-1] = temp;                                                             
                                                             }
                                          else break;                                                                              
                                       }}



                  if (n < count) return n;
                  else return count;


} 


unsigned int Meta_Database::Parallel_Query_RAM(string infilename, Index_Entry ** list, map<string,float* > &database_map,Meta_Result * results,int n, int t_number, int count,int group,string scoringtype,string filterflag,double &comparison_time){
                  //cout<<"n:"<<n<<"  t_number:"<<t_number<<"  count:"<<count<<endl; 

                  t_number = (t_number < count)? t_number : count;
                              
                  if (t_number == 0 || t_number > sysconf(_SC_NPROCESSORS_CONF))
                           t_number = sysconf(_SC_NPROCESSORS_CONF);
                                         
                   //Allocation tasks for threads
                        
                  int per_number = count /t_number;
                  int after_filter_count=0;
                 
                  //cout<<"per_number:"<<per_number<<endl;
                  
                  if (count % t_number !=0)
                            per_number++;
                        
                  //Meta_Result * pmap = (Meta_Result *)map(NULL, sizeof(Meta_Result) * count, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,-1,0);
                  
                  Meta_Result buffer[count];
                  
                  Argument args[t_number];
                  pthread_t t[t_number];
                  //cout<<"fine1"<<endl;
                  //cout<<"input into threads"<<endl;
                  //display_database(database_map);

                  for (int i = 0; i< t_number; i++){//Loop each thread        
                                           //cout<< "Process " << i << " starts" << endl;
                                           
                                           args[i].infilename = infilename;
                                           //args[i].p_list = &databselist;
                                           args[i].p_database=&database_map;
                                           args[i].thread = i;
                                           args[i].per_number = per_number;
                                           args[i].count = count;
                                           args[i].buffer = buffer;
                                           args[i].flag=0;
                                           args[i].after_filter_count=0;
                                           args[i].list=list;
                                           args[i].database_map=database_map;
                                           args[i].comparison_time=0;

                                           //args[i].tree_file = &Tree_file;
                                           pthread_create(&(t[i]),NULL, Parallel_Query_Static_RAM, &(args[i]));
                                          
                                           //cout<<"fine3"<<endl;
                                           }


          
                  
                  for (int i = 0; i < t_number; i++){
                          pthread_join(t[i], NULL); 
                          after_filter_count=after_filter_count+args[i].after_filter_count;
                          comparison_time=comparison_time+args[i].comparison_time;

                        }



                //cout<<"count:"<<count<<endl;
                //cout<<"after_filter_count:"<<after_filter_count<<endl;
                  /*for(int i = 0; i < count; i++){
                    cout<<buffer[i].b_entry->getfilename()<<endl;
                    cout<<buffer[i].m_value<<endl;
                  }*/

                  for (int i = 0; i < count; i++){
                          //cout<<"buffer[i].m_value"<<buffer[i].m_value<<endl;
                      if (buffer[i].m_value > results[n-1].m_value){
                                      results[n-1] =  buffer[i];
                                      for (int j = n-1; j>0; j--)
                                          if (results[j].m_value > results[j-1].m_value){
                                                             Meta_Result temp = results[j];
                                                             results[j] = results[j-1];
                                                             results[j-1] = temp;                                                             
                                                             }
                                          else break;                                                                              
                                       }}

                
                  if (n < count) return n;
                  else return count;                     
         
         }



unsigned int Meta_Database::Parallel_Query_RAM(string infilename, basic_entry ** list, map<string,float* > &database_map,Meta_Result * results,int n, int t_number, int count,int group,string scoringtype, string filterflag,double &comparison_time){
                  //cout<<"n:"<<n<<"  t_number:"<<t_number<<"  count:"<<count<<endl; 

                  t_number = (t_number < count)? t_number : count;
                              
                  if (t_number == 0 || t_number > sysconf(_SC_NPROCESSORS_CONF))
                           t_number = sysconf(_SC_NPROCESSORS_CONF);
                                         
                   //Allocation tasks for threads
                        
                  int per_number = count /t_number;
                  int after_filter_count=0;
                 
                  //cout<<"per_number:"<<per_number<<endl;
                  
                  if (count % t_number !=0)
                            per_number++;
                        
                  //Meta_Result * pmap = (Meta_Result *)mmap(NULL, sizeof(Meta_Result) * count, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,-1,0);
                  
                  Meta_Result buffer[count];
                  
                  Argument args[t_number];
                  pthread_t t[t_number];
                  //cout<<"fine1"<<endl;
                  //cout<<"input into threads"<<endl;
                  //display_database(database_map);

                  for (int i = 0; i< t_number; i++){//Loop each thread        
                                           //cout<< "Process " << i << " starts" << endl;
                                           
                                           args[i].infilename = infilename;
                                           //args[i].p_list = &databselist;
                                           args[i].p_database=&database_map;
                                           args[i].thread = i;
                                           args[i].per_number = per_number;
                                           args[i].count = count;
                                           args[i].buffer = buffer;
                                           args[i].flag=1;
                                           args[i].after_filter_count=0;
                                           args[i].list2=list;
                                           args[i].database_map=database_map;
                                           args[i].comparison_time=0;

                                           //args[i].tree_file = &Tree_file;
                                           pthread_create(&(t[i]),NULL, Parallel_Query_Static_RAM, &(args[i]));
                                          
                                           }


          
                  
                  for (int i = 0; i < t_number; i++){
                          pthread_join(t[i], NULL); 
                          after_filter_count=after_filter_count+args[i].after_filter_count;
                          comparison_time=comparison_time+args[i].comparison_time;

                        }



                //cout<<"count:"<<count<<endl;
                //cout<<"after_filter_count:"<<after_filter_count<<endl;
                  /*for(int i = 0; i < count; i++){
                    cout<<buffer[i].b_entry->getfilename()<<endl;
                    cout<<buffer[i].m_value<<endl;
                  }*/

                  for (int i = 0; i < count; i++){
                          //cout<<"buffer[i].m_value"<<buffer[i].m_value<<endl;
                      if (buffer[i].m_value > results[n-1].m_value){
                                      results[n-1] =  buffer[i];
                                      for (int j = n-1; j>0; j--)
                                          if (results[j].m_value > results[j-1].m_value){
                                                             Meta_Result temp = results[j];
                                                             results[j] = results[j-1];
                                                             results[j-1] = temp;                                                             
                                                             }
                                          else break;                                                                              
                                       }}

                
                  if (n < count) return n;
                  else return count;                     
         
         }




    
unsigned int Meta_Database::Parallel_Query(string infilename, Index_Entry * * list, Meta_Result * results, int n, int t_number, int count, string scoringtype,string filterflag){
                  //cout<<"n:"<<n<<"  t_number:"<<t_number<<"  count:"<<count<<endl;                                
                  t_number = (t_number < count)? t_number : count;
                              
                  if (t_number == 0 || t_number > sysconf(_SC_NPROCESSORS_CONF))
                           t_number = sysconf(_SC_NPROCESSORS_CONF);
                                         
                   //Allocation tasks for threads
                        
                  int per_number = count /t_number;
                  int after_filter_count=0;
                  //cout<<"per_number:"<<per_number<<endl;
                  
                  if (count % t_number !=0)
                            per_number++;
                        
                  //Meta_Result * pmap = (Meta_Result *)mmap(NULL, sizeof(Meta_Result) * count, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,-1,0);
                  
                  Meta_Result buffer[count];
                  
                  Argument args[t_number];
                  pthread_t t[t_number];
                  //cout<<"fine1"<<endl;

                  for (int i = 0; i< t_number; i++){//Loop each thread        
                                           //cout<< "Process " << i << " starts" << endl;
                                           
                                           args[i].infilename = infilename;
                                           args[i].list = list;
                                           args[i].thread = i;
                                           args[i].per_number = per_number;
                                           args[i].count = count;
                                           args[i].buffer = buffer;
                                           args[i].flag=0;
                                           args[i].after_filter_count=0;
                                           args[i].filterflag=filterflag;
                                           //args[i].tree_file = &Tree_file;
                                          //cout<<"fine2"<<endl;
                                           pthread_create(&(t[i]),NULL, Parallel_Query_Static,&(args[i]));
                                          
                                           //cout<<"fine3"<<endl;
                                           }


          
                  
                  for (int i = 0; i < t_number; i++){
                          pthread_join(t[i], NULL); 
                          after_filter_count=after_filter_count+args[i].after_filter_count;

                        }



                //cout<<"count:"<<count<<endl;
                //cout<<"after_filter_count:"<<after_filter_count<<endl;
                  /*for(int i = 0; i < count; i++){
                    cout<<buffer[i].b_entry->getfilename()<<endl;
                    cout<<buffer[i].m_value<<endl;
                  }*/



                if(scoringtype=="F"){

                  for (int i = 0; i < count; i++){
                          //cout<<"buffer[i].m_value"<<buffer[i].m_value<<endl;
                      if (buffer[i].m_value > results[n-1].m_value){
                                      results[n-1] =  buffer[i];
                                      for (int j = n-1; j>0; j--)
                                          if (results[j].m_value > results[j-1].m_value){
                                                             Meta_Result temp = results[j];
                                                             results[j] = results[j-1];
                                                             results[j-1] = temp;                                                             
                                                             }
                                          else break;                                                                              
                                       }}

                    } 

                else if(scoringtype=="T"){
                    for (int i = 0; i < count; i++){
                      if (buffer[i].n_value > results[n-1].n_value){
                                      results[n-1] =  buffer[i];
                                      for (int j = n-1; j>0; j--)
                                          if (results[j].n_value > results[j-1].n_value){
                                                             Meta_Result temp = results[j];
                                                             results[j] = results[j-1];
                                                             results[j-1] = temp;                                                             
                                                             }
                                          else break;                                                                              
                                       }}

                }





                  
                  if (n < count) return n;
                  else return count;                     
         
         }




       
unsigned int Meta_Database::Parallel_Query(string infilename, basic_entry * * list, Meta_Result * results, int n, int t_number, int count, string scoringtype, string filterflag){
                                        
                  t_number = (t_number < count)? t_number : count;
                  
                              
                  if (t_number == 0 || t_number > sysconf(_SC_NPROCESSORS_CONF))
                           t_number = sysconf(_SC_NPROCESSORS_CONF);
                                         
                   //Allocation tasks for threads
                  


                  int per_number = count /t_number;
                  int after_filter_count=0;
                  
                  if (count % t_number !=0)
                            per_number++;
                        
                  //Meta_Result * pmap = (Meta_Result *)mmap(NULL, sizeof(Meta_Result) * count, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,-1,0);
                  
                  Meta_Result buffer[count];
                  
                  Argument args[t_number];
                  pthread_t t[t_number];
                  //cout<<"fine2"<<endl;                                     
                  for (int i = 0; i< t_number; i++){//Loop each thread        
                                           //cout<< "Process " << i << " starts" << endl;
                                           
                                           args[i].infilename = infilename;
                                           args[i].list2 = list;
                                           args[i].thread = i;
                                           args[i].per_number = per_number;
                                           args[i].count = count;
                                           args[i].buffer = buffer;
                                           args[i].after_filter_count=0;
                                           args[i].flag=1;
                                           args[i].filterflag=filterflag;
                                           //args[i].tree_file = &Tree_file;
                                            
                                           pthread_create(&(t[i]),NULL, Parallel_Query_Static,&(args[i]));

                                           }

                  for (int i = 0; i < t_number; i++){
                           pthread_join(t[i], NULL); 
                           after_filter_count=after_filter_count+args[i].after_filter_count;
                  }

                  //cout<<"count:"<<count<<endl;
                  //cout<<"after_filter_count:"<<after_filter_count<<endl;


                  
                if(scoringtype=="F"){

                  for (int i = 0; i < count; i++){
                      if (buffer[i].m_value > results[n-1].m_value){
                                      results[n-1] =  buffer[i];
                                      for (int j = n-1; j>0; j--)
                                          if (results[j].m_value > results[j-1].m_value){
                                                             Meta_Result temp = results[j];
                                                             results[j] = results[j-1];
                                                             results[j-1] = temp;                                                             
                                                             }
                                          else break;                                                                              
                                       }}

                    } 

                else if(scoringtype=="T"){
                    for (int i = 0; i < count; i++){
                      if (buffer[i].n_value > results[n-1].n_value){
                                      results[n-1] =  buffer[i];
                                      for (int j = n-1; j>0; j--)
                                          if (results[j].n_value > results[j-1].n_value){
                                                             Meta_Result temp = results[j];
                                                             results[j] = results[j-1];
                                                             results[j-1] = temp;                                                             
                                                             }
                                          else break;                                                                              
                                       }}

                }

                 //cout<<"N:"<<n<<endl;
                 //cout<<"COUNT"<<count<<endl;

                  
                  if (n < count) return n;
                  else return count;                     
         
         }

        


 unsigned int Meta_Database::Parallel_Indexed_Query(string infilename, string query_key, string biotype,Meta_Result * results, int n, int t_number, int group, string scoringtype, string filterflag){

                  unsigned int count = 0;                                 
                  
                  if (this->Meta_index.count(query_key) == 0) return 0;
                
                  else  count = this->Meta_index[query_key]->size();

                  //To get the list of entry                                              
                  
                    Index_Entry * list[count];

                    count=this->Get_Entry_List(list,query_key,group);

                    //cout<<"count:"<<count<<endl;

                    //display_samples(list,count);

                    n = this->Parallel_Query(infilename, list, results, n, t_number, count, scoringtype, filterflag);

                    return n;
                  }
 
                          
                 
                        
unsigned int Meta_Database::Parallel_Indexed_Query(ostream & out, string infilename, string biotype,int n, int t_number, int group,string scoringtype, string filterflag){
                        
                        string keys[this->Key_dim];
                        
                        float weight[this->Key_dim];

                        vector<string> key_set;
                        vector<int> key_count;
                        Get_Key_Weight(infilename.c_str(), keys, weight, this->Key_dim);
                                                                        
                        Meta_Result results[n];
                        //Meta_Result new_results[n];
                                     
                        int n_count = 0;
                        
                   
                        for (int i = this->Key_dim; i>0; i--)
                            if (weight[i-1] >= q_min){
                                          
                                          string temp_keys[i];
                                          float temp_weight[i];
                                                                                   
                                          for (int j = 0; j < i; j++){
                                              
                                              temp_keys[j] = keys[j];
                                              temp_weight[j] = weight[j];

                                              }

                                          
                                          for (int j = 0; j < i-1; j++)
                                              for (int k = j+1; k < i; k++)
                                                  if ((temp_keys[j] > temp_keys[k])&&(abs(temp_weight[j] - temp_weight[k]) < delta)){
                                                                 
                                                                 string temp_k = temp_keys[j];
                                                                 temp_keys[j] = temp_keys[k];
                                                                 temp_keys[k] = temp_k;
                                                     
                                                                 
                                                                 float temp_w = temp_weight[j];
                                                                 temp_weight[j] = temp_weight[k];
                                                                 temp_weight[k] = temp_w;

                                                                 }

                                  


                                          string query_key = "";
                                          
                                          for (int j = 0; j < i; j++)
                                              query_key += temp_keys[j];//a change

                                       

                                          //cout<<query_key<<endl;
                                          //clust_index(query_key,biotype);
                                          key_set.push_back(query_key);

                                          //cout<<Meta_index.count(query_key)<<endl;
                                          key_count.push_back(Meta_index.count(query_key));
                                          
                                          
                                          //cout << i << "\t" << n_count << endl;
                                          
                                          if (weight[i-1] >= q_max) break;
                                          }
                                          //temp 
                                          //vector<int> :: iterator it2 = key_count.begin();
                                          int length = key_count.size();

                                        
                                          for(int j=0 ; j < length ;j++)
                                            for(int k=j+1; k < length;k++)
                                              if(key_count[j] < key_count[k])
                                                {
                                                  int tempcount=key_count[j];
                                                  key_count[j]=key_count[k];
                                                  key_count[k]=tempcount;

                                                  string tempkey=key_set[j];
                                                  key_set[j]=key_set[k];
                                                  key_set[k]=tempkey;
                                                 }


                                          int flag =0;
                                          if(Meta_biomes.count(biotype)==0)
                                              {flag = 0;}
                                          else
                                          {
                  
                                            flag = decide_to_clust(Meta_index[key_set[0]],Meta_biomes[biotype]);
                                          }
                                  
                                      
                                          if(flag == 1){
                                            vector <basic_entry *> merge_res = real_merge_index_cluster(*(Meta_index[key_set[0]]),*(Meta_biomes[biotype]));
                                  
                                            for(int i=1;i<length;i++){
                                              if(Meta_index.count(key_set[i])!=0){
                                              merge_res=real_merge_index_cluster(*(Meta_index[key_set[i]]),merge_res);
                                              }
                                              else
                                                  continue;
                                


                                            }

                                             unsigned int count=merge_res.size();


                                             basic_entry * list[count];
                  
                                             count = this->Get_Entry_List(list, merge_res , group);

                 
                                             n_count = this->Parallel_Query(infilename, list, results, n, t_number, count,scoringtype, filterflag);
                                                                                      

                                          }
                                          else{     
                                          for(int i=0 ; i < length ;i++ )
                                          {
                                            
                                            n_count += this->Parallel_Indexed_Query(infilename,key_set[i],biotype,results, n, t_number, group,scoringtype, filterflag);
                                          }
                                        }
                                        

                                          




                 
                      //cout<< "n_count: "<< n_count<<endl;

                      
                        if (n_count == 0){
                                                            
                                          out << "#Meta-Storms" << endl;
                                          out << "#Not hits" << endl;
                            
                                          return 0; 
                                         }
                        //cout<<"n:" <<n<<"\t"<<"n_count:"<<n_count<<endl;                                                 
                        n = (n < n_count) ? n : n_count;
                        //cout<<"n:" <<n<<"\t"<<"n_count:"<<n_count<<"\t"<<"result_size:"<<sizeof(results)/sizeof(results[0])<<endl;  
                        
                        for (int i = 0; i < n; i++){
                           
                           out << "#Meta-Storm" << endl;
                           out << "Match " << i+1 << endl;
                           out << "Similarity: " << results[i].m_value << "%" << endl;
                           //out << "New-Score: " << results[i].n_value << "%" << endl;
                           //out << "Sample Group: " << results[i].b_entry->Get_group() << endl;
                           out << "Sample path: " << results[i].b_entry->getfilename() << endl;                           
                           
                           }
                           cout<<endl;

                           
                           
                       
                           
                        return n;}


unsigned int Meta_Database::Parallel_Exhaustive_Query(ostream & out, string infilename, int n, int t_number, int group, string scoringtype, string filterflag){
                  
                  unsigned int count = 0;


                  
                  if (group < 0) count = this->Entry_count;
                  else if (this->Meta_groups.count(group) > 0)
                       count = this->Meta_groups[group]->size();

                  if (count == 0) {
                            
                            out << "#Meta-Storms" << endl;
                            out << "#Not hits" << endl;
                            
                            return 0; 
                            }                                              
                  //To get the list of entry
                  Index_Entry * list[count];
                                    
                  this->Get_Entry_List(list, group);
                  
                  Meta_Result results[n];                  
                  
                  n = this->Parallel_Query(infilename, list, results, n, t_number, count,scoringtype, filterflag);
                  
                  //cout<<"current n:"<<n<<endl;
                  for (int i = 0; i < n; i++){
                           
                           out << "#Meta-Storm" << endl;
                           out << "Match " << i+1 << endl;
                           out << "Similarity: " << results[i].m_value << "%" << endl;
                           //out << "New-Score: " << results[i].n_value<<"%"<<endl;
                           //out << "Sample Group: " << results[i].entry->Get_group() << endl;
                           out << "Sample path: " << results[i].b_entry->getfilename() << endl;                           
                           
                                                                                               
                            }
          out<<endl;
         return n; 
}








unsigned int Meta_Database::Parallel_Exhaustive_Query_RAM(ostream & out, string infilename, vector<string> &databaselist, map<string,float *> &database_map, int n, int t_number, int group,int is_gpu){
                  
                  clock_t startTime,endTime;
                  struct timeval tv_begin,tv_end;
                  gettimeofday(&tv_begin,NULL);
                  startTime=clock();

                  unsigned int count = 0;

                  double comparison_time=0;
                  double query_time=0;
                  
                  if (group < 0) count = this->Entry_count;
                  else if (this->Meta_groups.count(group) > 0)
                       count = this->Meta_groups[group]->size();

                  if (count == 0) {
                            
                            out << "#Meta-Storms" << endl;
                            out << "#Not hits" << endl;
                            
                            return 0; 
                            }                                              
                  //To get the list of entry
                  Index_Entry * list[count];
                  
                  this->Get_Entry_List(list, group);
                  
                  Meta_Result results[n];

                  if(is_gpu ==0)
                        n = this->Parallel_Query_RAM(infilename, list, database_map,results, n, t_number,count,group,"F","F", comparison_time);
                  else{
                        n = this->GPU_Parallel_Query_RAM(infilename, list, database_map,results, n, t_number,count,group,"F","F",comparison_time);
                        }
                  endTime=clock();

                  gettimeofday(&tv_end,NULL);
                  double time_use=double(tv_end.tv_sec-tv_begin.tv_sec)*1000000+double(tv_end.tv_usec-tv_begin.tv_usec);


                  cout<<"Query entry:"<<infilename<<"\t";
                  cout<<time_use<<" us"<<endl;


                  //cout<<double(endTime-startTime)  / CLOCKS_PER_SEC <<"\t"<<comparison_time  / CLOCKS_PER_SEC <<endl;
                  
                  //cout<<"current n:"<<n<<endl;
                  for (int i = 0; i < n; i++){
                           
                           out << "#Meta-Storm" << endl;
                           out << "Match " << i+1 << endl;
                           out << "Similarity: " << results[i].m_value << "%" << endl;
                           out << "New-Score: " << results[i].n_value<<"%"<<endl;
                           //out << "Sample Group: " << results[i].entry->Get_group() << endl;
                           out << "Sample path: " << results[i].b_entry->getfilename() << endl;                           
                           
                                                                                               
                            }
          out<<endl;
         return n; 
}


 unsigned int Meta_Database::Parallel_Indexed_Query_RAM(string infilename, string query_key, string biotype, map<string,float* > &database_map, Meta_Result * results, int n, int t_number, int group, string scoringtype, string filterflag,double &comparison_time,int is_gpu){

                  unsigned int count = 0;
                  
                  if (this->Meta_index.count(query_key) == 0) return 0;
                
                  else  count = this->Meta_index[query_key]->size();

                  //To get the list of entry                                              
                  
                    Index_Entry * list[count];

                    count=this->Get_Entry_List(list,query_key,group);

                    n = this->Parallel_Query_RAM(infilename, list, database_map,results, n, t_number, count, group,scoringtype, filterflag,comparison_time);

                    return n;
                  }



unsigned int Meta_Database::Parallel_Indexed_Query_RAM(ostream & out, string infilename, string biotype,vector<string> &databaselist,map<string,float *> &database_map, int n, int t_number, int group,string scoringtype,string filterflag,int is_gpu){
                        
                        string keys[this->Key_dim];
                        
                        float weight[this->Key_dim];

                        vector<string> key_set;
                        vector<int> key_count;
                        double comparison_time=0;
                        Get_Key_Weight(infilename.c_str(), keys, weight, this->Key_dim);
                                                                        
                        Meta_Result results[n];
                        //Meta_Result new_results[n];
                                     
                        int n_count = 0;
                        
                   
                        for (int i = this->Key_dim; i>0; i--)
                            if (weight[i-1] >= q_min){
                                          
                                          string temp_keys[i];
                                          float temp_weight[i];
                                                                                   
                                          for (int j = 0; j < i; j++){
                                              
                                              temp_keys[j] = keys[j];
                                              temp_weight[j] = weight[j];

                                              }

                                          
                                          for (int j = 0; j < i-1; j++)
                                              for (int k = j+1; k < i; k++)
                                                  if ((temp_keys[j] > temp_keys[k])&&(abs(temp_weight[j] - temp_weight[k]) < delta)){
                                                                 
                                                                 string temp_k = temp_keys[j];
                                                                 temp_keys[j] = temp_keys[k];
                                                                 temp_keys[k] = temp_k;
                                                     
                                                                 
                                                                 float temp_w = temp_weight[j];
                                                                 temp_weight[j] = temp_weight[k];
                                                                 temp_weight[k] = temp_w;

                                                                 
                                                                 }

                                  


                                          string query_key = "";
                                          
                                          for (int j = 0; j < i; j++)
                                              query_key += temp_keys[j];//a change

                                       

                                          //cout<<query_key<<endl;
                                          //clust_index(query_key,biotype);
                                          key_set.push_back(query_key);

                                          //cout<<Meta_index.count(query_key)<<endl;
                                          key_count.push_back(Meta_index.count(query_key));
                                          
                                          
                                          //cout << i << "\t" << n_count << endl;
                                          
                                          if (weight[i-1] >= q_max) break;
                                          }
                                          //temp 
                                          //vector<int> :: iterator it2 = key_count.begin();
                                          int length = key_count.size();

                                        
                                          for(int j=0 ; j < length ;j++)
                                            for(int k=j+1; k < length;k++)
                                              if(key_count[j] < key_count[k])
                                                {
                                                  int tempcount=key_count[j];
                                                  key_count[j]=key_count[k];
                                                  key_count[k]=tempcount;

                                                  string tempkey=key_set[j];
                                                  key_set[j]=key_set[k];
                                                  key_set[k]=tempkey;
                                                 }


                                          int flag =0;
                                          if(Meta_biomes.count(biotype)==0)
                                              {flag = 0;}
                                          else
                                          {
                  
                                            flag = decide_to_clust(Meta_index[key_set[0]],Meta_biomes[biotype]);
                                          }
                                  
                                      
                                          if(flag == 1){
                                            vector <basic_entry *> merge_res = real_merge_index_cluster(*(Meta_index[key_set[0]]),*(Meta_biomes[biotype]));
                                  
                                            for(int i=1;i<length;i++){
                                              if(Meta_index.count(key_set[i])!=0){
                                              merge_res=real_merge_index_cluster(*(Meta_index[key_set[i]]),merge_res);
                                              }
                                              else
                                                  continue;
                                


                                            }

                                             unsigned int count=merge_res.size();


                                             basic_entry * list[count];
                  
                                             count = this->Get_Entry_List(list, merge_res , group);

                                             //n_count = this->Parallel_Query_RAM(infilename, list,database_map ,results, n, t_number, count,group,scoringtype, filterflag,comparison_time);
                                              if(is_gpu ==0)
                                                       n_count = this->Parallel_Query_RAM(infilename, list, database_map,results, n, t_number,count,group,scoringtype,filterflag, comparison_time);
                                              else{
                                                       n_count = this->GPU_Parallel_Query_RAM(infilename, list, database_map,results, n, t_number,count,group,scoringtype,filterflag,comparison_time);

                                                   }                                   

                                          }
                                          else{     
                                          for(int i=0 ; i < length ;i++ )
                                          {
                                            
                                            n_count += this->Parallel_Indexed_Query_RAM(infilename,key_set[i],biotype,database_map,results, n, t_number, group,scoringtype, filterflag,comparison_time,is_gpu);
                                          }
                                        }

                      
                        if (n_count == 0){
                                                            
                                          out << "#Meta-Storms" << endl;
                                          out << "#Not hits" << endl;
                            
                                          return 0; 
                                         }
                        //cout<<"n:" <<n<<"\t"<<"n_count:"<<n_count<<endl;                                                 
                        n = (n < n_count) ? n : n_count;
                        cout<<"Query entry:"<<infilename<<'\t'<<comparison_time / float(1000000)<<"s"<<endl;
                        cout<<"Query entry:"<<infilename<<'\t'<<comparison_time <<" μs"<<endl;

                        //cout<<"n:" <<n<<"\t"<<"n_count:"<<n_count<<"\t"<<"result_size:"<<sizeof(results)/sizeof(results[0])<<endl;  
                        
                        for (int i = 0; i < n; i++){
                           
                           out << "#Meta-Storm" << endl;
                           out << "Match " << i+1 << endl;
                           out << "Similarity: " << results[i].m_value << "%" << endl;
                           out << "New-Score: " << results[i].n_value << "%" << endl;
                           //out << "Sample Group: " << results[i].b_entry->Get_group() << endl;
                           out << "Sample path: " << results[i].b_entry->getfilename() << endl;                           
                           
                           }
                           //cout<<endl;

                           
                           
                       
                           
                        return n;}







int Meta_Database::Out_Index(string outfilename){
    
    mkdir(outfilename.c_str(), 0755);
    //string sql="mkdir "+outfilename;
    //system(sql.c_str());
    string indexfile = outfilename + "/index.txt";
    string nametable = outfilename + "/nametable.txt";
    string biomeindex = outfilename+"/biomeindex.txt";//get new key
    string biomenametable = outfilename+"/bionametable.txt";// mergeresult
    
    ofstream out_index(indexfile.c_str(), ofstream::out);
    if (!out_index){
                  cerr << "Error : Cannot Open Index File : " << indexfile << endl;
                  exit(0);
                  }
    
    ofstream out_name(nametable.c_str(), ofstream::out);
    if (!out_name){
                  cerr << "Error : Cannot Open Name Table : " << nametable << endl;
                  exit(0);
                  }
    ofstream out_biomeindex(biomeindex.c_str(), ofstream::out);
    if (!out_biomeindex){
                  cerr << "Error : Cannot Open Name Table : " << biomeindex << endl;
                  exit(0);
                  }
    ofstream out_biometable(biomenametable.c_str(), ofstream::out);
    if (!out_biometable){
                  cerr << "Error : Cannot Open Name Table : " << biomenametable << endl;
                  exit(0);
                  }

        
    map<string, vector<Index_Entry *> *>::iterator miter = this->Meta_index.begin();
    //map<string,vector<basic_entry*> >:: iterator merge_iter=this->Meta_merge.begin();
    map<vector<string>, string> ::iterator map_iter=this->Meta_key.begin();
    map<string,vector<biomes_entry *> *> :: iterator biter = this->Meta_biomes.begin();
    
    out_name << "Database : " << this->Name << "\t" << "Total Entry Count : " << this->Entry_count << "\t"  << "Total Key Count : " << this->Meta_index.size() << "\t" << "Key Dim : " << this->Key_dim << "\t"<< "Total Group Count : " << this->Meta_groups.size() << endl;
    out_name << "Databasae ID\tGroup ID\tPath\tTop phyla abundence" << endl; 
    //out_biometable << "Database : " << this->Name << "\t" << "Total Entry Count : " << this->Entry_count << "\t"  << "Total Key Count : " << this->Meta_biomes.size() << "\t" << "Key Dim : " << this->Key_dim << "\t"<< "Total Group Count : " << this->Meta_groups.size() << endl;
    out_biometable << "Databasae ID\tGroup\tPath\tBiomes" << endl; 
    
    while(miter!=this->Meta_index.end()){
                                         out_index << ">"<< miter->first << endl;
                                         vector<Index_Entry *>::iterator viter;
                                         for (viter = miter->second->begin(); viter != miter->second->end(); viter ++){
                                             out_index << (*viter)->Get_id() << "\t";
                                             out_name << (*viter)->Get_id() << "\t" << (*viter)->Get_group() << "\t" << (*viter)->getfilename();  
                                             for (int i = 0; i< this->Key_dim; i++)
                                                 out_name << "\t" << (*viter)->Get_key(i) << "\t" << (*viter)->Get_weight(i);                                       
                                             out_name << endl;
                                             }
                                         out_index << endl;
                                         
                                         miter++;
                                         }
    while(biter != this->Meta_biomes.end()){
      out_biomeindex << ">" << biter->first <<endl;
      vector<biomes_entry *> :: iterator viter;
      for(viter= biter->second->begin(); viter != biter->second->end(); viter++){
        out_biomeindex <<  (*viter)->getid() << "\t";
        out_biometable << (*viter)->getid()  << "\t" <<(*viter)->Get_group()<<"\t"<<(*viter)->getfilename() <<"\t"<< (*viter)->getbiomes()<<endl;

      }
      out_biometable<<endl;
      biter++;
    }
    
    //out_mergeindex<< "Database : " << this->Name << "\t" << "Total Entry Count : " << this->Entry_count << "\t"  << "Total Key Count : " << this->Meta_index.size() << "\t" << "Key Dim : " << this->Key_dim << "\t"<< "Total Group Count : " << this->Meta_groups.size() << endl;
     



//out_map<< "Database : " << this->Name << "\t" << "Total Entry Count : " << this->Entry_count << "\t"  << "Total Key Count : " << this->Meta_index.size() << "\t" << "Key Dim : " << this->Key_dim << "\t"<< "Total Group Count : " << this->Meta_groups.size() << endl;
  //while()
    
    out_index.close();
    out_index.clear();
    out_name.close();
    out_name.clear();
    out_biomeindex.close();
    out_biomeindex.clear();
    out_biometable.close();
    out_biometable.clear();

    return this->Entry_count;
    
    }




int Meta_Database::Load_Index(string infilename,string biotype){
    
    string indexfile = infilename + "/index.txt";
    string nametable = infilename + "/nametable.txt";
    string biomeindex = infilename + "/biomeindex.txt";
    string biomenametable = infilename + "/bionametable.txt";
    
    ifstream in_index(indexfile.c_str(), ifstream::in);
    if (!in_index){
                 cerr <<"Error: Cannot Open Database Index File : " << indexfile << endl;
                 exit(0);
                 }
    ifstream in_name(nametable.c_str(), ifstream::in);
    if (!in_name){
                 cerr <<"Error: Cannot Open Database Name Table : " << nametable << endl;
                 exit(0);
                 }
    ifstream in_biomeindex(biomeindex.c_str(), ifstream::in);
    if (!in_biomeindex){
                 cerr <<"Error: Cannot Open Database Name Table : " << biomeindex << endl;
                 exit(0);
                 }
    ifstream in_biometable(biomenametable.c_str(), ifstream::in);
    if (!in_biometable){
                 cerr <<"Error: Cannot Open Database Name Table : " << biomenametable << endl;
                 exit(0);
                 }

    if (this->Meta_index.empty())
       this->Meta_index.clear();
    if (this->Meta_biomes.empty())
       this->Meta_biomes.clear();
  
    map <unsigned int, Index_Entry *> entry_map;
    
    string buffer;
    
    //Nametable
    
    //Label
    getline(in_name, buffer);
    
    //DB Name;
    int begin = buffer.find("Database : ") + 11;
    int end =buffer.find("\t");
    this->Name = buffer.substr(begin, end - begin);
    //Key dim;
    begin = buffer.find("Key Dim : ") + 10;
    end = buffer.size();
    this->Key_dim = atoi((buffer.substr(begin, end - begin)).c_str());
    //Entry_count;
    this->Entry_count = 0;
    //Abundance
    this->Abundance = 0;
    
    //Label
    //getline(in_name, buffer);
    
    while (getline(in_name, buffer)){
          
          if (buffer.size() == 0) continue;
          
          unsigned int id;
          unsigned int group;
          string filename;
          
          string keys[this->Key_dim];
          float weight[this->Key_dim];
          
          int begin = 0;
          int end = buffer.find('\t', begin);
          
          id = atoi(buffer.substr(begin, end - begin).c_str());
          
          begin = end + 1;
          end = buffer.find('\t', begin);
          
          group = atoi(buffer.substr(begin, end - begin).c_str());
          
          begin = end + 1;
          end = buffer.find('\t', begin);
          
          filename = buffer.substr(begin, end - begin);
          //cout<<filename<<endl;
          

          
          for (int i = 0; i < this->Key_dim; i++){
              
              begin = end + 1;
              end = buffer.find('\t', begin);
              keys[i] = buffer.substr(begin, end - begin);
              
              begin = end + 1;
              end = buffer.find('\t', begin);
              weight[i] = atof(buffer.substr(begin, end - begin).c_str());
              
              }
          
          Index_Entry * entry = new Index_Entry(id, group, keys, weight, this->Key_dim, filename, i_min);
          
          entry_map[id] = entry;
          
          if (id >= this->Entry_number)
             this->Entry_number = id + 1;
          
          

          }


    //bionametable
    getline(in_biometable, buffer);
    while(getline(in_biometable,buffer)){

          unsigned int id;
          unsigned int group;
          string filename;
          string biomes;
          
          if (buffer.size() == 0) continue;


          int begin=0;
          int end = buffer.find('\t', begin);
          
          id = atoi(buffer.substr(begin, end - begin).c_str());
          
          begin = end + 1;
          end = buffer.find('\t', begin);
          
          group = atoi(buffer.substr(begin, end - begin).c_str());
          
          begin = end + 1;
          end = buffer.find('\t', begin);
          
          filename = buffer.substr(begin, end - begin);

          begin = end +1;
          end=buffer.find('\t', begin);
          biomes = buffer.substr(begin,end - begin);

          //cout<<id<<"\t"<<group<<"\t"<<filename<<"\t"<<biomes<<endl;
          biomes_entry* b_entry= new biomes_entry(id,group,biomes,filename);
          //cout<<b_entry->getbiomes()<<'\t'<<"?"<<endl;
          if (this->Meta_biomes.count(b_entry->getbiomes()) == 0){
                                      this->Meta_biomes[b_entry->getbiomes()] = new vector<biomes_entry *>;
                                      //cout<<b_entry->getbiomes()<<endl;
                                      //cout<<b_entry->getfilename()<<endl;
                                    }

          Meta_biomes[b_entry->getbiomes()]->push_back(b_entry);
          
    }

    //Index
  
    //getline(in_index, buffer);
    
    while (getline(in_index, buffer)){
          
          if (buffer.size() == 0) ;
          if (buffer[0] == '>'){
                         string key = buffer.substr(1, buffer.size()-1); 
                         getline(in_index, buffer);
                         istringstream strin(buffer);
                         unsigned int id;
                         while (strin >> id){
                               if (entry_map.count(id) == 0){
                                                       
                                                       cerr << "Error : Cannot find Sample ID : " << id << endl;
                                                       exit(0);
                                                       
                                                       }

                               this->Insert_Index_by_Entry(entry_map[id]);
                               this->Insert_Group_by_Entry(entry_map[id]);
                               }
                               }
                         }

  
    in_index.close();
    in_index.clear();
    in_name.close();
    in_name.clear();
    in_biometable.close();
    in_biometable.clear();
    in_biomeindex.close();
    in_biomeindex.clear();
    return this->Entry_count;
    }

float Meta_Database::Get_Abundance(){
      //cout<<this->Abundance<<"\t"<<this->Entry_count;
      return (float) this->Abundance / (float) this->Entry_count;
      
      }

float Meta_Database::Update_Abundance(){
    
    unsigned int temp_abundance = 0;
    
    unsigned int count = this->Entry_count;
    
    Index_Entry * list[count];
    
    this->Get_Entry_List(list, -1);
    
    for (int i = 0; i < count; i++)
        if (list[i]->Get_abundance() >= dom_t ) temp_abundance ++;
        //else cout << list[i]->Get_abundance() << endl;
    
    this->Abundance = temp_abundance;
    
    return (float)temp_abundance / (float)count;
    
    }

vector<basic_entry*> Meta_Database::real_merge_index_cluster(vector <Index_Entry*> fir, vector<basic_entry*> sec){
  vector<Index_Entry*> :: iterator fir_iter=fir.begin();
  vector<basic_entry*> :: iterator sec_iter=sec.begin();
  vector<basic_entry*> res;
  int flag;
  //cout<<"fine"<<endl;
  while(fir_iter!=fir.end())
  {
    res.push_back(*fir_iter);
    fir_iter++;
  }
  
  while(sec_iter!=sec.end())
  {
    flag=0;
    fir_iter=fir.begin();
    while(fir_iter!=fir.end())
    {

      if((*fir_iter)->getfilename()==(*sec_iter)->getfilename())
      {
          flag=1;
          fir_iter++;
          continue;
      }
      fir_iter++;
    }
    if(flag==0)
    {
      res.push_back(*sec_iter);
    }
    sec_iter++;
  }

  return res;

}


vector<basic_entry*> Meta_Database::real_merge_index_cluster(vector <Index_Entry*> fir, vector<biomes_entry*> sec){
  vector<Index_Entry*> :: iterator fir_iter=fir.begin();
  vector<biomes_entry*> :: iterator sec_iter=sec.begin();
  vector<basic_entry*> res;
  int flag;
  //cout<<"fine"<<endl;
  while(fir_iter!=fir.end())
  {
    res.push_back(*fir_iter);
    fir_iter++;
  }
  
  while(sec_iter!=sec.end())
  {
    flag=0;
    fir_iter=fir.begin();
    while(fir_iter!=fir.end())
    {

      if((*fir_iter)->getfilename()==(*sec_iter)->getfilename())
      {
          flag=1;
          fir_iter++;
          continue;
      }
      fir_iter++;
    }
    if(flag==0)
    {
      res.push_back(*sec_iter);
    }
    sec_iter++;
  }

  return res;

}


int Meta_Database::merge_index_cluster(vector <Index_Entry*> *fir, vector<biomes_entry*> *sec){

  vector<Index_Entry*> :: iterator fir_iter=fir->begin();
  vector<biomes_entry*> :: iterator sec_iter=sec->begin();

  vector<basic_entry*> res;
  int _count=0;
  for(sec_iter=sec->begin();sec_iter!=sec->end();sec_iter++){
      for(fir_iter=fir->begin();fir_iter!=fir->end();fir_iter++){
          
        if((*sec_iter)->getfilename()==(*fir_iter)->getfilename()){

          _count++;
          continue;
        }
        else{
         // res.push_back(*fir_iter);
            continue;          
    
        }
        
      }
      //res.push_back(*sec_iter);
      //cout<<(*sec_iter)->getfilename()<<endl;
    }

  
  return _count;
  }


void  Meta_Database::display_table(map<string,vector<basic_entry* > > sample)
{
    map<string,vector<basic_entry*> > :: iterator map_iter;
    for(map_iter=sample.begin();map_iter != sample.end();map_iter++){
       vector<basic_entry*> :: iterator vector_iter;
       //cout<<"fine"<<endl;
       cout<<"key:"<<map_iter->first<<endl;
      for(vector_iter= map_iter->second.begin() ;vector_iter!= map_iter->second.end() ;vector_iter ++){

        
        cout<<"path:"<<(*vector_iter)->getfilename()<<endl;
        //cout<<"id:"<< vector_iter[i]->getname() <<endl;
      }
    } 
}



int Meta_Database::decide_to_clust(vector<Index_Entry *>* entry, vector<biomes_entry* >* b_entry){
  //cout<<entry->size()<<endl;
  //cout<<b_entry->size()<<endl;
  int t_length= entry->size();
  int b_length= b_entry->size();
  //cout<<"length of taxon index:"<<t_length<<endl;
  //cout<<"length of biome index:"<<b_length<<endl;
  

  int m_length=merge_index_cluster(entry,b_entry);
  //cout<<"length of whole database:"<<m_length<<endl;
  
  //cout<<m_length<<endl;

  float fir_fraction = (float) t_length / this->Entry_count;
  float sec_fraction = (float) t_length / m_length;
  //cout<<"fir faction："<<fir_fraction<<endl;
  //cout<<"sec fraction："<< sec_fraction<<endl;
  //cout<<endl;
  if(fir_fraction < newthreshold)
      return 1;
  else if(m_length < 10 || sec_fraction > 0.6)
      return 1;
  else
    return 0;

    

}
    

void Meta_Database::display_b_table(map<string,vector<biomes_entry* > * > sample)
{
    map<string,vector<biomes_entry*>*> :: iterator map_iter;
   
    for(map_iter=sample.begin();map_iter != sample.end();map_iter++){
       vector<biomes_entry*> :: iterator vector_iter;
       cout<<"key:"<<map_iter->first<<endl;
      for(vector_iter= map_iter->second->begin() ;vector_iter!= map_iter->second->end() ;vector_iter ++){

        
        //cout<<"id:"<<(*vector_iter)->getname()<<endl;
        cout<<"path:"<<(*vector_iter)->getfilename()<<endl;
        //cout<<"id:"<< vector_iter[i]->getname() <<endl;
      }
    } 
}

void Meta_Database::display_t_table(map<string,vector<Index_Entry* > * > sample)
{
    map<string,vector<Index_Entry*>*> :: iterator map_iter;
    //cout<<"size"<<sample.size()<<endl;
    for(map_iter=sample.begin();map_iter != sample.end();map_iter++){
       vector<Index_Entry*> :: iterator vector_iter;
       cout<<"key:"<<map_iter->first<<endl;
      for(vector_iter= map_iter->second->begin() ;vector_iter!= map_iter->second->end() ;vector_iter ++){

        
        cout<<"path:"<<(*vector_iter)->getfilename()<<endl;
        //cout<<"id:"<< vector_iter[i]->getname() <<endl;
      }
    } 
}

void Meta_Database::display_samples(Index_Entry ** entry, int count){
  for(int i=0;i<count;i++)
  {
    cout<<entry[i]->getfilename()<<endl;
  }

  cout<<"end of display"<<endl;
}



