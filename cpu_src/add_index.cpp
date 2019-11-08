#include <iostream>
#include <fstream>
#include <stdio.h>
#include "math.h"
#include "meta_database.cpp"
#include "version.h"

int printhelp(){
    
    cout << "Welcome to Meta-Storms " << endl;
    cout << "Version : " << Version << endl;
    cout << "Usage : " << endl;
    cout << "add_index [-option] value" << endl;
    cout << "\toption : " << endl;
    cout << "\t-i samples input path, must be the full absolute path" << endl;
    cout << "\t-d database index name" << endl;
    cout << "\t-o index output path" << endl;
    cout << "\t-g to assign group, defaults is 0" << endl; 
    cout << "\t-h help" << endl;
    exit(0);
    
    return 0;
    
    };

int main(int argc, char * argv[]){
    
     string indexname = "Default";
     
     string newindexname = "New";
     string databasepath = "Path";
     string biotype="None";
     
     int group = 0;
     
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
                            case 'd': indexname = argv[i+1]; break;
                            case 'i': databasepath = argv[i+1]; break;
                            case 'o': newindexname = argv[i+1]; break;
                            case 'g': group = abs(atoi(argv[i+1])); break;
                            case 'b': biotype = argv[i+1];break; 
                            default : printf("Unrec argument %s\n", argv[i]); printhelp(); break; 
                            }
         i+=2;
         } 
    
    cout << "Welcome to Meta-Storms Beta " << endl;
    
    Meta_Database database(indexname,biotype);
    int add_count=database.Make_Index_Add(databasepath, biotype, group);
    //database.clust_index();
    database.Out_Index(newindexname);
    
    cout << add_count << " Items are added into the index, there are " << database.Get_Entry_Count() << " items in total" << endl; 
    
    cout << "The expect accuracy is " << database.Get_Abundance() << endl;
    
    return 0;
    
    }
