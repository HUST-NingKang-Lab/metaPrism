#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

class biomes {
 private:
  string project_id;
  string path;

 public:
  biomes() {
    project_id = "";
    path = "";
  }
  biomes(string t_id, string t_path) {
    project_id = t_id;
    path = t_path;
  }
  void set(string t_id, string t_path) {
    project_id = t_id;
    path = t_path;
  }
  biomes(string t_path) { path = t_path; }
  void set_path(string t_path) { path = t_path; }
  void display() {
    cout << "project_id:" << project_id << "\t"
         << "path:" << path << endl;
  }
  string getproject_id() { return project_id; }
  string getpath() { return path; }
};  // end biomes

void insert_biomes(string path, map<string, vector<biomes> > &biomes_table) {
  struct dirent *ent = NULL;
  DIR *pDir;
  pDir = opendir(path.c_str());
  ent = readdir(pDir);  // read .
  ent = readdir(pDir);  // read ..
  while ((ent = readdir(pDir)) != NULL) {
    biomes b1;

    if (ent->d_type == 4) {
      cout << "d_ino:" << ent->d_ino << endl;
      cout << "d_reclen:" << ent->d_reclen << endl;
      cout << "d_type:" << ent->d_type << endl;
      cout << "d_name:" << ent->d_name << endl;
      string newpath = path + "/" + ent->d_name;
      insert_biomes(newpath);
    } else {
      string newpath = path + "/" + ent->d_name;
      b1.set_path(ent->d_name, newpath);
      // b1.set(id,newpath);

      biomes_table[path].push_back(b1);
    }
  }

  closedir(pDir);
  /*while ((ent=readdir(pDir)) != NULL)
  {

          if(ent->d_type == 4)
          {
                  string newpath=path+"/"+"ent->name";
                  view_dir(newpath);
          }
          else
          {
                  string newpath=path+"/"+"ent->name";
                  cout<<ent->d_name[0];
          }
  }*/
}

int main() {
  string path = "/mnt/hgfs/vm_share/biomes";
  map<string, vector<biomes> > biomes_table;

  insert_biomes(path, biomes_table);

  return 0;
}
