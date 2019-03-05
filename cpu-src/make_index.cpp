#include <iostream>
#include "meta_database.cpp"
#include <time.h>
#include "version.h"

using namespace std;

int printhelp(){
    
    cout << "Welcome to Meta-Storms Beta " << endl;
    cout << "Version : " << Version << endl;
    cout << "Usage : " << endl;
    cout << "make_index [-option] value" << endl;
    cout << "\toption : " << endl;
    cout << "\t-i samples input path, must be the full absolute path" << endl;
    cout << "\t-o index output path" << endl;
    cout << "\t-n database name" << endl;
    //cout << "\t-l index level, default is 3" << endl;
    cout << "\t-h help" << endl;
    
    exit(0);
    return 0;
    };

int main(int argc, char * argv[]){
    time_t startTime,endTime;
    startTime=clock();

    //Para
    string databasename = "Default";
    string databasepath = "DefaultPath";
    string indexname = "Default";
    int indexlevel = 4;
    
    //string tree_file;
    Check_Env();
    
      int i = 1;
      
      if (argc ==1) 
		printhelp();
      
      while(i<argc){
         if (argv[i][0] != '-') {
                           printf("Argument # %d Error : Arguments must start with -\n", i);
                           exit(0);
                           };           
         switch(argv[i][1]){
                            case 'n': databasename = argv[i+1]; break;
                            case 'i': databasepath = argv[i+1]; break;
                            case 'o': indexname = argv[i+1]; break;
                            case 'l': indexlevel = atoi(argv[i+1]); break;

                            default : printf("Unrec argument %s\n", argv[i]); printhelp(); break; 
                            }
         i+=2;
         } 
    
    
     
    cout << "Welcome to Meta-Storms Beta " << endl;
    Meta_Database database(databasename, databasepath, indexlevel);
    //cout << "begin to output index"<<endl;
    database.Out_Index(indexname);
    
    cout << database.Get_Entry_Count() <<" Entries are added into the index"<< endl;
    
    cout << "The expect accuracy is " << database.Get_Abundance() << endl;
    endTime=clock();
    cout<<"This query step costs "<<double(endTime- startTime) / CLOCKS_PER_SEC <<" s"<<endl;
    
    
    return 0;
}
