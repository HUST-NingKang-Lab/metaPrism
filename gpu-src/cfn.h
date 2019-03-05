#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

#ifndef CFN_H
#define CFN_H

using namespace std;

struct Res{
       string name;
       unsigned int count;
       };

int Read_file(const char * infilename, map<string, unsigned int> (&table)[7]){
    
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
                                      
                                      if (table[bio_level].count(temp) == 0)
                                                       table[bio_level][temp] = 1;
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
                          
    return line_count;
    }

    int Show_table(map<string, unsigned int> table[6], int line_count,
                   ostream& out) {
      string label[7] = {"½ç", "ÃÅ", "¸Ù", "Ä¿", "¿Æ", "Êô", "ÖÖ"};
      // for (int i = 0; i < 6; i++)
      {
        int i = 1;  
        float temp = 0;
        
        map<string, unsigned int>::iterator map_iter = table[i].begin();
        out << label[i] << ": " << table[i].size() << "\t";
        
        
        Res res[table[i].size()]; 
        
        int res_iter = 0;
        
        while(map_iter != table[i].end()){
                       
                       //cout << map_iter->first <<": ";
                       //cout << (float)(map_iter->second * 100 / line_count) <<"%\t"; 
                       
                       res[res_iter].name = map_iter->first;
                       res[res_iter].count = map_iter->second; 
                       res_iter ++; 
                       map_iter ++;

                      }
        for (int j = 0; j < table[i].size() - 1; j++ )
            for (int k = j+1; k < table[i].size(); k++)
              if (res[j].count < res[k].count){
                               Res temp;
                               temp = res[j];
                               res[j] = res[k];
                               res[k] = temp;
                               }
                               
        int num = 4;
        
        for (int j = 0; j < table[i].size(); j++){
            
            out << res[j].name << ": ";
            out << (float)(res[j].count * 100) / (float)line_count << "%\t";
            
            temp += (float)(res[j].count * 100) / (float)line_count;
            
            
            //if (j > num) break;
            
            }
        
        //cout << endl;
        //out << temp << endl;
        out << endl;
        
        }
    
    return 0;
    }


int Get_Key(const char * infilename, string * top, int n){
    
    map<string, unsigned int> table[7];
    
    int total = Read_file(infilename, table);
    
    if (total <2) {
              
              cerr << "Empty file :" << infilename << endl;
              return -1;
              
              }
    
    int i = 1;
    
    Res res[table[i].size()]; 
        
    int res_iter = 0;
    
    map<string, unsigned int>::iterator map_iter = table[i].begin();    
    while(map_iter != table[i].end()){
 
                       res[res_iter].name = map_iter->first;
                       res[res_iter].count = map_iter->second; 
                       res_iter ++; 
                       map_iter ++;

                      }
      for (int j = 0; j < table[i].size() - 1; j++ )
            for (int k = j+1; k < table[i].size(); k++)
              if (res[j].count < res[k].count){
                               Res temp;
                               temp = res[j];
                               res[j] = res[k];
                               res[k] = temp;
                               }
    
    int end = (table[i].size() < n)?table[i].size():n;

    for (int j = 0; j < end; j++)
        top[j] = res[j].name;
    
    if (table[i].size() < n)    
      for (int j = table[i].size(); j < n; j++)
          top[j] = "";
    
    return 0;
    }

int Get_Key_Weight(const char * infilename, string * top, float * weight, int n){
    
    map<string, unsigned int> table[7];
    
    int total = Read_file(infilename, table);
    
    if (total <2) {
              
              cerr << "Empty file :" << infilename << endl;
              //cout << "Empty file :" << infilename << endl;
              return -1;
              
              }

    
    int i = 1;//affect the level of key origin is 1
    
    Res res[table[i].size()]; 
        
    int res_iter = 0;
    
    map<string, unsigned int>::iterator map_iter = table[i].begin();    
    while(map_iter != table[i].end()){
 
                       res[res_iter].name = map_iter->first;
                       res[res_iter].count = map_iter->second; 
                       res_iter ++;
                       map_iter ++;

                      }
      for (int j = 0; j < table[i].size() - 1; j++ )
            for (int k = j+1; k < table[i].size(); k++)
              if (res[j].count < res[k].count){
                               Res temp;
                               temp = res[j];
                               res[j] = res[k];
                               res[k] = temp;
                               }
    
    int end = (table[i].size() < n)?table[i].size():n;

    for (int j = 0; j < end; j++){
        top[j] = res[j].name;
        weight[j] = ((float)(res[j].count) * 100.0 / (float)total );
        }


    
    if (table[i].size() < n)    
      for (int j = table[i].size(); j < n; j++)
          {
            top[j] = "";
          weight[j] = 0;
         }
         
    return 0;
    }

#endif

/*
int main(int argc, char * argv[]){
    
    map<string, unsigned int> table[6];
    
    int line_count;
    
    line_count = Read_file(argv[1], table);
    
    cout << line_count << endl;
    
    Show_table(table, line_count,cout);
    
    system("pause");
    
    return 0;
    
    }
*/
