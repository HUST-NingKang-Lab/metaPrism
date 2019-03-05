#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "comp.h"
#include "version.h"

#define MAX_SAM_NUM 10000
/*
int main(int argc, char * argv[]){
    
    
    cout << "The similarity is " << endl << Comp_sam(argv[1], argv[2]) << "%" << endl;
    
    //system("pause");
    
    
    return 0;
}
*/

struct Argument{
       
     int thread;
     int file_n;
     int pair_count;
     float * results;
     float * entropys;
     string * nametable;
     int (* comptable)[2];
     
     //string * tree_file;
     
     };

int printhelp(){
    
    cout << "Welcome to Meta-Storms Beta " << endl;
    cout << "Version : " << Version << endl;
    cout << "Usage : " << endl;
    cout << "comp_sam [-option] value" << endl;
    cout << "\toption : " << endl;

    cout << "\t-i two samples path for single sample comparison, conflict with -l" << endl;
    cout << "\t-l sample name list table for multi-sample comparison, confilct with - i" << endl;
    cout << "\t-o result output file, default is to output on screen" << endl;
    cout << "\t-t cpu core number, default is 1" << endl;
    cout << "\t-h help" << endl;
    
    exit(0);
    
    return 0;
    
    };

int single_comp(const char * queryfile1, const char * queryfile2){
    float entropy;
    cout << "The similarity is " << endl << Comp_sam(queryfile1, queryfile2) << "%" << endl;
    //cout << "The entropy is "<<endl<<entropy<<endl;
    //Comp_sam(queryfile1, queryfile2);
    }

void * multi_comp(void * args){
     
     int thread = ((Argument *)args)->thread;
     int file_n = ((Argument *)args)->file_n;
     int pair_count = ((Argument *)args)->pair_count;
     float * results = ((Argument *)args)->results;
     //float * entropys= ((Argument *)args)->entropys;
     string * nametable = ((Argument *)args)->nametable;
     int (* comptable)[2] = ((Argument *)args)->comptable;
     //string * tree_file = ((Argument *)args)->tree_file;
     //cout<<"compare running"<<endl;
     for (int i = 0; i < file_n; i++){ //loop each pair    
                        
                        int offset = thread * file_n + i;
                        //cout<<"offset:"<<offset<<endl;
                        if (offset >= pair_count) break;
                        results[offset] = Comp_sam((nametable[comptable[offset][0]]).c_str(), (nametable[comptable[offset][1]]).c_str());
                        //cout<<nametable[comptable[offset][0]]<<'\t'<<nametable[comptable[offset][1]]<<endl;
                        //cout<<"finished a compare"<<endl;
                        
                        }
     pthread_exit(0);
     
     
     }

int list_comp(const char * listfilename, const char * outfilename, int t_number){                          
             
             ifstream infile(listfilename, ifstream::in);
             if (!infile){
                       cerr << "Open list file error : " << listfilename << endl;
                       exit(0);
                       }        
             
             ofstream outfile(outfilename, ofstream::out);
             if (!outfile){
                           cerr << "Open output file error : " << outfilename << endl;
                           exit(0);
                           }
             
             // Get nametable
             string nametable[MAX_SAM_NUM]; //nametable
             string buffer;
             int count = 0;
             
             while(getline(infile, buffer)){
                                             if (buffer.size() == 0) continue;     
                                                  
                                             nametable[count] = buffer;
                                             
                                             count ++;
                                            //cout<<count<<endl;                 
                                             }
             
             cout << count << " Samples in total" << endl;
             
             // Get comptable
             int (* comptable)[2] = new int [count * (count - 1) / 2][2];
             int ** comptable_r = new int * [count];
             for (int i = 0; i < count; i++)
                      comptable_r[i] = new int [count];
             int pair_count = 0; //number of pairs
             
             for (int i = 0; i < count - 1; i++)
                 for (int j = i + 1; j < count; j++){
                     
                     comptable[pair_count][0] = i;
                     comptable[pair_count][1] = j;
                     
                     comptable_r[i][j] = pair_count;
                     comptable_r[j][i] = pair_count;
                     
                     pair_count ++;
                     }
                     
             //Parallel comp
             //cout << "comptable finished" << endl;
             
             if (t_number > sysconf(_SC_NPROCESSORS_CONF))
                           t_number = sysconf(_SC_NPROCESSORS_CONF);
             
             if (t_number > pair_count)
                          t_number = pair_count;
                          
             int file_n = pair_count / t_number; //count of each thread
             
             if (pair_count % t_number != 0) file_n ++;
             
             float * results = new float [pair_count]; 
             //cout<<"pair_count:"<<pair_count<<endl;
             pthread_t t[t_number];
             Argument args[t_number];
             
             cout << t_number << " threads" << endl;
             
             for (int i = 0; i < t_number; i++){ //loop of each thread
             
                    args[i].thread = i;
                    args[i].file_n = file_n;
                    args[i].pair_count = pair_count;
                    args[i].results = results;
                    args[i].nametable= nametable;
                    args[i].comptable = comptable;
                    //args[i].tree_file = tree_file;
                    
                    pthread_create(&(t[i]),NULL,multi_comp,&(args[i]));
                    //cout<<"pthread_create"<<endl;
                 
                    }                     
             
             for (int i = 0; i < t_number; i++)
                           pthread_join(t[i], NULL);  

                                                             
             //Output
             for (int i = 0; i < count; i++){
                 
                 for (int j = 0; j < count; j++ ){
                     
                     if (i == j) outfile << 100 << "\t";
                     else outfile << results[comptable_r[i][j]] << "\t";
                     
                     }
                 outfile << endl;
                 }             
             
             infile.close();
             infile.clear();
             outfile.close();
             outfile.clear();
                          
    
    }


int main(int argc, char * argv[]){
    //Para

    string queryfile1, queryfile2;
    string listfilename;
    
    string outfilename;
        
    int t_number = 1;
    
    int mode = 0; //0: single 1: multi
    
    //string tree_file;
    
    //Check_Env(tree_file);
    
    Comp_init();
        
    int i = 1;
      
      if (argc ==1) 
		printhelp();
      
      while(i<argc){
         //cout<<"argc："<<argc<<endl;
         if (argv[i][0] != '-') {
                           printf("Argument # %d Error : Arguments must start with -\n", i);
                           exit(0);
                           };           
         switch(argv[i][1]){

                            case 'i': queryfile1 = argv[i+1]; queryfile2 = argv[i+2]; i++; break;
                            case 'l': listfilename = argv[i+1]; mode = 1; break;
                            case 'o': outfilename = argv[i+1]; break;
                            case 't': t_number = atoi(argv[i+1]); break;//temp try
                            default : printf("Unrec argument %s\n", argv[i]); printhelp(); break; 
                            }
         i+=2;
         }
        
        if (mode == 0){
                 
                single_comp(queryfile1.c_str(), queryfile2.c_str());
                 
                 }
        else{
                 
                 list_comp(listfilename.c_str(), outfilename.c_str(), t_number);
            
             }
         
         
         return 0;    

}
    
