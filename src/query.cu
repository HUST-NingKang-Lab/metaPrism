#include <iostream>
#include <fstream>
#include <queue>
#include <stdio.h>

#include "version.h"
#include <time.h>
#include <sys/time.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "meta_drive.cpp"
#include "meta_database.cpp"
#include "calc.cpp"
using namespace std;


int printhelp() {

    cout << "Welcome to Meta-Prism Beta " << endl;
    cout << "Version : " << Version << endl;
    cout << "Usage : " << endl;
    cout << "query_index [-option] value" << endl;
    cout << "\toption : " << endl;
    cout << "\t-l query sample list" << endl;
    cout << "\t-d database index path" << endl;
    cout << "\t-i query sample path" << endl;
    cout << "\t-o result output path, default is screen" << endl;
    cout << "\t-n hit number, default is 5" << endl;
    cout << "\t-e exhaustive search" << endl;
    cout << "\t-g to assign group number, default is off" << endl;
    cout << "\t-b biotype, default is none" << endl;
    cout << "\t-h help" << endl;
    exit(0);
    return 0;
};


int main(int argc, char *argv[]) {
    string indexname = "Default.txt";
    string queryfilename = "classification.txt";
    string outfilename = "";
    struct timeval tv_begin, tv_end;
    gettimeofday(&tv_begin, NULL);

    int t_number = 1;
    int is_index = 1;
    int r_number = 5;
    int group = -1;

    // temp
    //int gpulist[2]={0,1};


    string scroingtype = "F";
    string biotype = "none";
    string filterflag = "T";
    string filelist = "";
    //string tree_file;
    CompData* CD;
    CD=new CompData();
    Comp_init(CD);
    int i = 1;

    if (argc == 1)
        printhelp();

    while (i < argc) {
        if (argv[i][0] != '-') {
            printf("Argument # %d Error : Arguments must start with -\n", i);
            exit(0);
        };
        switch (argv[i][1]) {
            case 'd':
                indexname = argv[i + 1];
                break;
            case 'i':
                queryfilename = argv[i + 1];
                break;
            case 'e':
                is_index = 0;
                i--;
                break;
            case 'n':
                r_number = atoi(argv[i + 1]);
                break;
            case 'o':
                outfilename = argv[i + 1];
                break;
            case 'g':
                group = atoi(argv[i + 1]);
                break;
            case 'b':
                biotype = argv[i + 1];
                break;
            case 's':
                scroingtype = argv[i + 1];
                break;
            case 'f':
                filterflag = argv[i + 1];
                break;
            case 'l':
                filelist = argv[i + 1];
                break;
            default :
                printf("Unrec argument %s\n", argv[i]);
                printhelp();
                break;
        }
        i += 2;
    }
    cudaGetDeviceCount(&deviceCount);
    if(deviceCount<1) {
        printf("no cuda device\n");
        exit(0);

    }
    calcID=new pthread_t[deviceCount];
    int ret;
    sem_init(&semse,0,0);
    sem_init(&semsf,0,100);
    sem_init(&semre,0,0);
    sem_init(&semrf,0,100);
    pthread_mutex_init(&sendLock,NULL);
    pthread_mutex_init(&calcLock,NULL);
    pthread_mutex_init(&resultLock,NULL);
    pthread_mutex_init(&trashLock,NULL);
    pthread_mutex_init(&killLock,NULL);
    pthread_mutex_init(&coutLock,NULL);
    //deviceCount=1;
    {
        cout<<"i'm loader pro\n";

        drivePara para;
        para.outpath=outfilename;
        para.filelist=filelist;
        para.indexname=indexname;
        para.n=r_number;
        para.t=t_number;
        para.group=group;
        para.scroingtype=scroingtype;
        para.filterflag=filelist;
        para.is_index=is_index;
        para.biotype=biotype;
        para.CD=CD;
        //drive(para);
        int m=0;
        int *deviceIds;
        deviceIds=new int[deviceCount];
        for(m=0;m<deviceCount;m++){
            deviceIds[m]=m;
            ret=pthread_create(&(calcID[m]),
                    NULL,calcSim,
                    (void*)(deviceIds+m));
            cout<<"creat one calc thread\n";
        }
        ret=pthread_create(&processID,NULL,process,(void*)&r_number);
        cout<<"creat one process thread\n";
        drive((void*)&para);

        gettimeofday(&tv_end, NULL);
        double time_use = double(tv_end.tv_sec - tv_begin.tv_sec) * 1000000 + double(tv_end.tv_usec - tv_begin.tv_usec);
        cout << "This query step costs: " << time_use / 1000000 << " s" << endl;
    }
    /*
    fpid=fork();
    if(fpid<0)
        printf("error in fork\n");
    else if (fpid==0){
        // loader
        cout<<"i'm loader pro\n";
        abdMap *pAbdMap;
        pAbdMap=new abdMap(indexname,CD);
        Meta_Database database(indexname, biotype, pAbdMap);
        drive(outfilename, filelist, indexname, r_number, t_number, group, scroingtype, filterflag, is_index,
                    database, is_gpu,CD);
    }
    else{
        bool device =false;
        int deviceID;
        for(i=0;i<deviceCount;i++){
            fpid=fork();
            if(fpid<0) printf("error in fork\n");
            else if(fpid==0){
                deviceID=i;
                device=true;
                break;
            }
        }
        if(device== false){
            cout<<"i'm waiter pro\n";
            //waiter
            exit(0);
        }
        else {
            cout<<"i'm executer pro "<<deviceID<<endl;
            //gpu executer
            exit(0);
        }
    }
     */
    return 0;
}
