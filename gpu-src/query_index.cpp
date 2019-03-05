#include <iostream>
#include <fstream>
#include <stdio.h>
#include "meta_database.cpp"
#include "version.h"
#include <time.h>
#include <sys/time.h>
//#include <map>

using namespace std;

int printhelp(){
    
    cout << "Welcome to Meta-Storms Beta " << endl;
    cout << "Version : " << Version << endl;
    cout << "Usage : " << endl;
    cout << "query_index [-option] value" << endl;
    cout << "\toption : " << endl;
    cout << "\t-d database index name" << endl;
    cout << "\t-i query sample path" << endl;
    cout << "\t-o result output file, default is to output on screen" << endl;
    cout << "\t-n hit number, default is 5" << endl;
    cout << "\t-e exhaustive search" << endl;
    cout << "\t-g to assign group number, default is off" << endl;
    cout << "\t-t cpu core number, default is 1" << endl;
    cout << "\t -b biotype, default is none"<<endl;
    cout << "\t -f whether conduct a alpha-diversity filtering step, default is T"<<endl;
    cout << "\t -p use gpu to conduct parallel computation"<<endl;

    
    cout << "\t-h help" << endl;
    
    exit(0);
    
    return 0;
    
    };


struct Arg{
  vector<string> * p_databaselist;
  map<string, float *>* p_database_map;
  int thread;
  int per_number;
  int count;

};


string extractfilename(string buffer){
     if (buffer.size() == 0)
            cerr<<"Input file is empty"<<endl;
          
          unsigned int id;
          unsigned int group;
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

float load_abd(string infilename,float * abd,int * id){
    map<int, int> hash;
     
    ifstream infile(infilename.c_str(), ifstream::in);
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
    
    for (int i = 0; i < LeafN; i++){
        if (hash.count(id[i]) == 0)
           abd[i] = 0;
        else {
              abd[i] = (float)hash[id[i]] * 100 / (float)line_count;
              //cout<<abd[i]<<'\t'<<i<<endl;
              total_abd=total_abd+abd[i];
              float t_abd=abd[i]/100;
              //cout<<abd[i]<<endl;
              shannon=shannon-t_abd*log(t_abd);
              //cout<<"shannon:"<<shannon<<endl;
              }

              } 
    //cout<<"total_abd:"<<total_abd<<endl;   
    infile.close();
    infile.clear();

        
    return shannon;

}




int load_database(string indexname,map<string, float *> &database_map,vector<string> &databaselist){
    //map<string,float *> database_map;
    string nametable=indexname+"/"+"nametable.txt";
    
    ifstream in_name(nametable.c_str(),ifstream::in);
    if(!in_name){
        cerr <<"Error: Cannot Open Database Index File : " << nametable << endl;
                 exit(0);
    }
    string buffer;
    Comp_init();
    int f_count=0;
    //meta information
    getline(in_name,buffer);
    //label
    getline(in_name,buffer);
    while(getline(in_name,buffer)){
        string filename=extractfilename(buffer)+"/classification.txt";
        //cout<<filename<<endl;
        float * Abd = new float [LeafN];

        load_abd(filename,Abd,Id);
        //cout<<filename<<endl;
        database_map[filename]=Abd;
        f_count++;
        //delete [] Abd;
        //temp_database.push_back(database_map);
        //cout<<endl;

    }

    return f_count;


}


void* parallel_execution(void * args){
  vector<string>* p_databaselist=((Arg*)args)->p_databaselist;
  map<string, float* >* p_database_map=((Arg*)args)->p_database_map;
  int per_number=((Arg*)args)->per_number;
  int thread=((Arg*)args)->thread;
  int count=((Arg*)args)->count;

  for(int i=0;i<per_number;i++){
    int sam_file =  thread * per_number + i;
    if (sam_file >= count) break;
    float * Abd = new float [LeafN];
    string filename= (*(p_databaselist))[i];
    load_abd(filename,Abd,Id);
    (*p_database_map)[filename]=Abd;
    //cout<<filename<<endl;
   
  }

  pthread_exit(0);

}


int parallel_load_database(string indexname,map<string, float *> &database_map,vector<string> &databaselist,int t_number){
  struct timeval tv_begin,tv_end;
  time_t startTime,endTime;
  gettimeofday(&tv_begin,NULL);
  startTime=clock();

  string nametable=indexname+"/"+"nametable.txt";
  int count=0;  
    ifstream in_name(nametable.c_str(),ifstream::in);
    if(!in_name){
        cerr <<"Error: Cannot Open Database Index File : " << nametable << endl;
                 exit(0);
    }
    string buffer;
    Comp_init();
    int f_count=0;
    //meta information
    getline(in_name,buffer);
    //label
    getline(in_name,buffer);

    while(getline(in_name,buffer)){
      string filename=extractfilename(buffer)+"/classification.txt";
      databaselist.push_back(filename);
      count = count +1;
    }
    t_number = (t_number < count)? t_number : count;
    if (t_number == 0 || t_number > sysconf(_SC_NPROCESSORS_CONF))
                    t_number = sysconf(_SC_NPROCESSORS_CONF);
                                         
                   //Allocation tasks for threads
                        
    int per_number = count /t_number;
    int after_filter_count=0;
                  //cout<<"per_number:"<<per_number<<endl;
                  
    if (count % t_number !=0)
               per_number++;

    Arg args[t_number];
    pthread_t t[t_number];
    for(int i=0;i<t_number;i++){
      args[i].p_databaselist=&databaselist;
      args[i].p_database_map=&database_map;
      args[i].per_number=per_number;
      args[i].count=count;
      args[i].thread=i;

      pthread_create(&(t[i]),NULL, parallel_execution,&(args[i]));
    }


    for(int i=0;i<t_number;i++){
      pthread_join(t[i], NULL);
    }

    endTime=clock();
    gettimeofday(&tv_end,NULL);
    cout<<"This load_database step costs "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
    double time_use=double(tv_end.tv_sec-tv_begin.tv_sec)*1000000+double(tv_end.tv_usec-tv_begin.tv_usec);
    cout<<"This load_database step costs "<<time_use<<" μs"<<endl;

    return count;

}


void GetBiome(const string& s, string &biome, const string& c)
{
    string::size_type pos1;
    string::size_type pos2;
    vector<string> v;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
         
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));

      vector<string> :: iterator iter=v.end()-3;
      biome=(*iter);
      //cout<<biome<<endl;

}


int load_input(string filelist, vector<string> &inputlist, vector<string> & namelist,vector<string> &biomelist){
      string buffer;
      ifstream in_list(filelist.c_str(),ifstream::in);
      while(getline(in_list,buffer)){
        
        //cout<<buffer<<endl;
        int iter_1=buffer.find_last_of('/');
        int iter_2=buffer.substr(0,iter_1).find_last_of('/')+1;

        string biome;
        GetBiome(buffer,biome,"/");



        inputlist.push_back(buffer);
        biomelist.push_back(biome);
        namelist.push_back(buffer.substr(iter_2,iter_1-iter_2));

      }

}



//int single_query()


int multi_query(string outpath,string filelist,string indexname,int n, int t, int group,string scroingtype,string filterflag, int is_index,Meta_Database &database,int is_gpu){
    
    vector<string> inputlist;
    vector<string> namelist;
    vector<string> databaselist;
    vector<string> biomelist;
    map<string,float *> database_map;


    load_database(indexname,database_map,databaselist);
    parallel_load_database(indexname,database_map,databaselist,t);



    load_input(filelist,inputlist,namelist,biomelist);

    vector<string> :: iterator input_iter;
    vector<string> :: iterator output_iter=namelist.begin();
    vector<string> :: iterator biome_iter=biomelist.begin();
    for(input_iter=inputlist.begin();input_iter!=inputlist.end();input_iter++){
      //cout<<"begin to query"<<endl;
      //cout<<*(input_iter)<<endl;
    if(outpath.size()>0){
       string outfilename=outpath+"/"+*(output_iter);
       ofstream out(outfilename.c_str(), ofstream::out);




       if (!out){
              cerr << "Open output file error : " << outfilename << endl;
              exit(0);
                  }
       if(is_index==0){    
              database.Parallel_Exhaustive_Query_RAM(out, *(input_iter),databaselist,database_map,n,t,group,is_gpu);
              
              }

      else{

        database.Parallel_Indexed_Query_RAM(out,*(input_iter),*(biome_iter),databaselist,database_map,n,t,group,scroingtype,filterflag,is_gpu);
      }








    }
    else{

      if(is_index==0){
      database.Parallel_Exhaustive_Query_RAM(cout, *(input_iter),databaselist,database_map,n,t,group,is_gpu);
    }

      else{
        database.Parallel_Indexed_Query_RAM(cout, *(input_iter),*(biome_iter),databaselist,database_map,n,t,group,scroingtype,filterflag,is_gpu);
      }
    }

    biome_iter++;
    output_iter++;
  }

}



int main(int argc, char * argv[]){
    struct timeval tv_begin,tv_end;
    time_t startTime,endTime;
    gettimeofday(&tv_begin,NULL);
    startTime=clock();
    //Para
    string indexname = "Default.txt";
    string queryfilename = "classification.txt";
    string outfilename = "";
    
    int t_number = 1;
    int is_index = 1;
    int r_number = 5;
    int group = -1;
    int is_gpu=0;

    // temp 
    int gpulist[2]={0,1};
    int count=2;




    string scroingtype="F";
    string biotype="none";
    string filterflag="T";
    string filelist="";
    //string tree_file;
    Comp_init();
    int mode=0;  
    int i = 1;
      
    if (argc ==1) 
		      printhelp();
      
    while(i<argc){
         if (argv[i][0] != '-') {
                           printf("Argument # %d Error : Arguments must start with -\n", i);
                           exit(0);
                           };           
         switch(argv[i][1]){
                            case 'd': indexname = argv[i+1]; break;
                            case 'i': queryfilename = argv[i+1]; break;
                            case 't': t_number = atoi(argv[i+1]); break;
                            case 'e': is_index = 0; i--; break;
                            case 'n': r_number = atoi(argv[i+1]); break;
                            case 'o': outfilename = argv[i+1]; break;
                            case 'g': group = atoi(argv[i+1]); break;
                            case 'b': biotype = argv[i+1];break;
                            case 's': scroingtype=argv[i+1];break;
                            case 'f': filterflag=argv[i+1];break;
                            case 'l': mode=1;filelist=argv[i+1];break;
                            case 'c': is_gpu=1;break;
                            //case 'm': 
                            default : printf("Unrec argument %s\n", argv[i]); printhelp(); break; 
                            }
         i+=2;
         } 
    
    cout << "Welcome to Meta-Storms Beta " << endl;
    
    /*if(is_gpu==0)
      Meta_Database database(indexname,biotype);
    else
    */
    Meta_Database database(indexname,biotype,gpulist,count);
    

    if(mode == 0){
    //file
    if (outfilename.size() > 0){                           
                           ofstream out(outfilename.c_str(), ofstream::out);
                           if (!out){
                                     cerr << "Open output file error : " << outfilename << endl;
                                     exit(0);
                                     }                                                       
                           
                           if (is_index == 1)
                              database.Parallel_Indexed_Query(out, queryfilename,biotype, r_number, t_number, group,scroingtype,filterflag);
                           else
                              database.Parallel_Exhaustive_Query(out, queryfilename, r_number, t_number, group, scroingtype,filterflag);
                           
                           out.close();
                           out.clear();
                           
                           } 
    //cout
    else {
        //cout<<queryfilename<<'\t'<<biotype<<endl;
         if (is_index == 1)
              database.Parallel_Indexed_Query(cout, queryfilename, biotype,r_number, t_number, group,scroingtype, filterflag);
         else
              database.Parallel_Exhaustive_Query(cout, queryfilename, r_number, t_number, group, scroingtype, filterflag);
         }

    endTime=clock();
    gettimeofday(&tv_end,NULL);
    double time_use=double(tv_end.tv_sec-tv_begin.tv_sec)*1000000+double(tv_end.tv_usec-tv_begin.tv_usec);
    cout<<"This query step costs: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<"s"<<endl;
    cout<<"This query step costs: "<<time_use / 1000000 <<" s"<<endl;

         
    }


    else{
       

  
          multi_query(outfilename,filelist,indexname,r_number,t_number,group,scroingtype,filterflag,is_index,database,is_gpu);
          endTime=clock();
          gettimeofday(&tv_end,NULL);
          double time_use=double(tv_end.tv_sec-tv_begin.tv_sec)*1000000+double(tv_end.tv_usec-tv_begin.tv_usec);
 
          cout<<"This query step costs: "<<time_use / 1000000<<" s"<<endl;
          cout<<"This query step costs: "<<double(endTime- startTime) / CLOCKS_PER_SEC <<"s"<<endl;

          //out.close();
          //out.clear();

        



    }
    
    return 0;
}