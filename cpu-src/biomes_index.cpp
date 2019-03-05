#include <iostream>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <unistd.h>  
#include <string>
#include <dirent.h>
#include <vector>
#include <map>
#include <fstream>
using namespace std; 


class biomes{
 private:
  string project_id;
  string path;

 public:
  biomes() {
    project_id = "";
    path = "";
		}
		biomes(string t_id,string t_path){
			project_id=t_id;
			path=t_path;
		}
		void set(string t_id,string t_path)
		{	project_id=t_id;
			path=t_path;
		}
		void display()
		{	cout<<"project_id:"<<project_id<<"\t"<<"path:"<<path<<endl;
			}	
		string getproject_id()
		{
			return project_id;
		}
		string getpath()
		{
			return path;
		
		}
};//end biomes

void read_file(string infilename,map<string,vector<biomes> > &biomes_table)
{
	ifstream infile;
	infile.open(infilename.c_str());
	string s1,s2,s_id;
	biomes b1; 
	int iter;
	int iter_id;
	int count=0;
	int length;
	string temp;
	//map
	map<string, unsigned int> b_table;
	//map<string,vector<biomes> > biomes_table;
	getline(infile,s1);
	while(!infile.eof())
	{
		getline(infile,s1);
		iter=s1.find_last_of('\t');
		iter_id=s1.find_first_of('\t');
		length=s1.size();
		//cout<<s1<<endl;
		s2=s1.substr(iter+1,length-1);
		s_id=s1.substr(0,iter_id);
		b1.set(s2,s_id);
		//cout<<s_id<<endl; 
		//map<string,string>::iterator it =biomes_table.find(s2);
		biomes_table[s2].push_back(b1);
		/*if(it!=b_table.end())
		{
			b_table[s2];
		}
		else
		{
			b_table[s2]=1;
		}*/
	}
	/*for(map<string,vector<biomes> >::iterator it=biomes_table.begin();it!=biomes_table.end();it++)
	{
		cout<<"name:"<<it->first<<endl;
		int t=it->second.size();
		for(int i=0;i<t;i++)
		{
			cout<<it->second[i].getpath()<<"\t";
		}
		cout<<endl;
	}*/
	
}


int main()
{
	map<string,vector<biomes> > biomes_table;
	read_file("new_biomes.txt",biomes_table);
	//a.push_back(b);
	//a[0].display();
	for(map<string,vector<biomes> >::iterator it=biomes_table.begin();it!=biomes_table.end();it++)
	{
		cout<<"name:"<<it->first<<endl;
		int t=it->second.size();
		for(int i=0;i<t;i++)
		{
			cout<<it->second[i].getpath()<<"\t";
		}
		cout<<endl;
	}
	
	return 0;
}
