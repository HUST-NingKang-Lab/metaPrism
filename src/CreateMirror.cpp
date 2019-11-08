
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "math.h"
#include "meta_database.cpp"
#include "version.h"

int printhelp(){

    cout << "Welcome to Meta-Prism " << endl;
    cout << "Version : " << Version << endl;
    cout << "Usage : " << endl;
    cout << "CreateMirror [-option] value" << endl;
    cout << "\toption : " << endl;
    cout << "\t-d database index name" << endl;
    cout << "\t-h help" << endl;
    exit(0);

    return 0;

};

int main(int argc, char * argv[]){

    string indexname = "Default";

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
            default : printf("Unrec argument %s\n", argv[i]); printhelp(); break;
        }
        i+=2;
    }
    int pos = samplepath.find("classification.txt");

    if (pos != string::npos)
        samplepath = samplepath.substr(0, pos);

    cout << "Welcome to Metagenomic-Search Beta " << endl;

    Meta_Database database(indexname,biotype);
    if(biotype!="None"){
        biomes_entry* entry= new biomes_entry(biotype,samplepath);
        database.Insert_Index_by_File(samplepath,entry,group);

    }
    else{
        database.Insert_Index_by_File(samplepath, group);}

    database.Out_Index(newindexname);

    cout << "1 Items is added into the index, there are " << database.Get_Entry_Count() << " items in total" << endl;

    cout << "The expect accuracy is " << database.Get_Abundance() << endl;

    return 0;

}