#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cmath>


/*
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
*/
//#include "index_entry.h"

//#include "meta_database.cpp"
//#include "meta_database.h"



using namespace std;


void SplitString(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
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
      cout<<(*iter)<<endl;


      /*while(iter!=v.end()-2){
        cout<<(*iter)<<endl;
        iter++;

      }*/


}





int main() {
  // Meta_Database database("12","/mnt/hgfs/vm_share/Meta-Storms-Samples/biome_example",4);
  // database.Parallel_Indexed_Query(cout,"/mnt/hgfs/vm_share/Meta-Storms-Samples/biome_example/2/65-5/classification.txt","/3",5,
  // 1, -1);
  /*
  char a;ss
  cin>>a;
  if(a=='\0')
    cout<<"True"<<endl;
  else if(a==' ')
    cout<<"Blank"<<endl;
  */

  vector<string> split_result;

  string inputline="/home/zhumo/Meta-Prism-Test/cluster4/solid_waste/SRR3087215/classification.txt";

  SplitString(inputline,split_result,"/");








  /*
  float* adb = new float[10];
  for(int i=0;i<10;i++)
  {
    adb[i]=i;
    cout<<adb[i]<<" ";
  }
  cout<<endl;
  float* adb2= new float[10];
  for(int i=0;i<10;i++)
  {
    adb2[i]=adb[i];
    cout<<adb2[i]<<" ";
  }
  cout<<endl;
  adb2[0]=10;
  cout<<"1:"<<adb[0]<<" "<<"2:"<<adb2[0]<<endl;


  */

  /*vector<int> a;
  cout<<"kidding me?"<<endl;
  int b[4]={1,2,3,4};
  a.push_back(1);
  a.push_back(2);
  a.push_back(4);
    //Grial(a, 0,sizeof(a) / sizeof(int));
  for(int i=0;i<10;i++){
  random_shuffle(b,b+3);
  cout<<b[0]<<" "<<b[1]<<" "<<b[2]<<" "<<b[3]<<endl;
  }

  */
  /*string Rfile ="temp2.txt";

  ifstream R_res(Rfile.c_str(), ifstream::in);
if (!R_res){
           cerr <<"Error: Cannot Open Database Index File : " << Rfile << endl;
           exit(0);
           }
string buffer;
int i=1;
int j=1;
int tempflag=0;
while (getline(R_res, buffer)){
  tempflag=0;
  j=1;

  while(tempflag!=-1)
  {	//cout<<buffer[tempflag]<<"\t";
          if(buffer[tempflag]=='1'||buffer[tempflag+1]=='1'){
                          cout<<i<<"\t"<<j<<endl;
                          break;
                  }
          j++;
          //cout<<buffer<<endl;
          tempflag=buffer.find('\t',tempflag+1);
          //cout<<tempflag<<endl;


  }
  i++;
  cout<<endl;
}*/
/*
  string infilename = "/mnt/hgfs/vm_share/Meta-Storms-Samples/database1/10-1/"
                      "classification.txt";
  ifstream infile(infilename.c_str(), ifstream::in);
  if (!infile) {
    cerr << "Open infile error :" << infilename << endl;
    exit(0);
    // system("pause");
  }

  string buffer;
  getline(infile, buffer); // Lable
  map<string, vector<string>> table[7];
  while (getline(infile, buffer)) {
    if (buffer.size() == 0)
      continue;
    int bio_level = 0;
    // cout<<buffer<<endl;
    int str_length = buffer.size();
    string temp = "";
    int str_iter = buffer.find_first_of('\t') + 1;
    int str_iter2 = buffer.substr(str_iter).find_first_of('\t') - 1;
    string name = buffer.substr(str_iter, str_iter2);
    // cout<<name<<endl;
    str_iter = buffer.find_last_of('\t') + 1; // To locate last of '\t'

    while ((bio_level < 7) && (str_iter < str_length)) {
      if ((buffer[str_iter] == ';') || (str_iter == str_length - 1)) {
        if (temp.find("otu_") != string::npos)
          break;
        if (temp.find("other") != string::npos)
          break;
        if (temp.find("nclassified") != string::npos)
          break;

        temp += ';';
        if (count(table[bio_level][temp].begin(), table[bio_level][temp].end(),
                  name) == 0)
          table[bio_level][temp].push_back(name);
        // cout<<temp<<endl;
        temp = "";

        bio_level++;
        str_iter++;
      } else
        temp += buffer[str_iter];

      str_iter++;
    }
  }
  int i = 4;
  map<string, vector<string>>::iterator a_iter = table[i].begin();
  for (; a_iter != table[i].end(); a_iter++) {
    cout << a_iter->first << endl;
    cout << a_iter->second.size() << endl;
  }
*/
   //string a="/mnt/hgfs/vm_share/Meta-Storms-Samples/biome_example/1/1-1";
   //string path="/mnt/hgfs/vm_share/code/txtfile/Meta-Prism-project/newindex";
   //string sql="mkdir -p "+path;
   //system(sql.c_str()); 
   //string temp="1233";
   //cout<<atoi(temp.c_str())<<endl;
   
  // int b= path.find_first_of("/");
  // cout<<path.size()<<endl;
  // int c=path.find('/',b+1);

  // cout<<newkey<<endl;
  /*vector<int> a;
  vector<int> b;
  int c=1;
  int d=2;
  a.push_back(c);
  b.push_back(c);
  a.push_back(d);
  b.push_back(d);
  if(a==b)
  {
          cout<<"done"<<endl;
  }
  map<vector<int>,int> t_map;
  t_map[a]=1;
  if(t_map.find(b)!=t_map.end()){
          cout<<"done"<<endl;
  }*/

 /* vector<int> a;
  a.push_back(1);
  a.push_back(2);
  a.push_back(4);
  //cout<<a[1]<<endl;
  vector<int> ::iterator iter=a.begin();
  cout<<count(a.begin(),a.end(),1)<<endl;
//cout<<(*iter+2)<<endl;
*/

  /*
  map<int,int>  t_table;
  
  t_table.insert(std::pair<int,int>(1,1));
  t_table.insert(std::pair<int,int>(2,2));
  map<int,int>  ::iterator map_iter=t_table.begin();
  map_iter->second=3;
  cout<<map_iter->first<<" "<<map_iter->second<<endl;
  */
  return 0;
}
