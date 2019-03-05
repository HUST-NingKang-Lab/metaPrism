#include <math.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#ifndef INDEX_ENTRY_H
#define INDEX_ENTRY_H

#define delta 50

const double threshold=0.6;
using namespace std;

class basic_entry {
 protected:
  string filename;

 public:
  basic_entry() { filename = ""; }
  basic_entry(string t_filename) { filename = t_filename; }
  string getfilename() { return filename; }

};  // end basic_entry

class biomes_entry : public basic_entry {
 private:
  string id;
  // string filename;

 public:
  biomes_entry() {
    id = "";
    filename = "";
  }
  biomes_entry(string t_id, string t_filename) {
    id = t_id;
    filename = t_filename;
  }
  biomes_entry(string t_filename) { filename = t_filename; }
  void set(string t_id, string t_filename) {
    id = t_id;
    filename = t_filename;
  }
  // biomes_entry(string t_filename) { filename = t_filename; }
  void set_path(string t_filname) { filename = filename; }
  void display() {
    cout << "id:" << id << "\t"
         << "filename:" << filename << endl;
  }
  string name() { return id; }
  // string getpath() { return filename; }
};  // end

class Index_Entry : public basic_entry {
 public:
  Index_Entry() {
    this->id = 0;
    this->group = 0;
    this->keys = NULL;
    this->weight = NULL;
    this->abundance = 0;
    this->dim = 0;
    this->group = 0;
  }

  Index_Entry(unsigned int _id, unsigned int _group, string* _keys,
              float* _weight, int _dim, string _filename, float d_min) {
    this->id = _id;
    this->group = _group;
    this->dim = _dim;
    this->keys = new string[_dim];
    this->weight = new float[_dim];

    for (int i = 0; i < dim; i++) {
      this->keys[i] = _keys[i];
      this->weight[i] = _weight[i];
    }
    this->filename = _filename;

    this->abundance = this->Make_key(d_min);
  }

  Index_Entry(unsigned int _id, unsigned int _group, string key, int _dim,
              string _filename) {
    this->id = _id;
    this->group = _group;
    this->key = key;
    this->dim = _dim;
    this->filename = _filename;

    this->abundance = 0;
  }
  Index_Entry(string _filename) { this->filename = _filename; }

  int Get_id() { return this->id; }

  int Get_group() { return this->group; }

  string Get_key(int i) {
    if (i < this->dim)
      return this->keys[i];
    else
      return "";
  }

  string Get_key() { return this->key; }

  float Get_weight(int i) {
    if (i < this->dim)
      return this->weight[i];
    else
      return 0;
  }

  string Get_filename() { return this->filename; }

  float Get_abundance() { return this->abundance; }

 private:
  unsigned int id;
  unsigned int group;
  string* keys;
  float* weight;

  string key;
  float abundance;

  int dim;
  // string filename;

  float Make_key(float d_min);
};

float Index_Entry::Make_key(float d_min) {
  string temp_keys[this->dim];
  float temp_weight[this->dim];

  int key_count = 0;
  float temp_abundance = 0;

  for (int i = 0; i < this->dim; i++) {
    if (this->weight[i] >= d_min) {
      temp_keys[i] = this->keys[i];
      temp_weight[i] = this->weight[i];
      key_count++;
      temp_abundance += this->weight[i];
    }

    else
      break;
  }

  if (key_count == 0) {
    this->key = "other";
    return 0;
  }

  for (int i = 0; i < key_count - 1; i++)
    for (int j = i + 1; j < key_count; j++)
      if ((temp_keys[i] > temp_keys[j]) &&
          (abs(temp_weight[j] - temp_weight[i]) < delta)) {
        string temp_k = temp_keys[i];
        temp_keys[i] = temp_keys[j];
        temp_keys[j] = temp_k;

        float temp_w = temp_weight[i];
        temp_weight[i] = temp_weight[j];
        temp_weight[j] = temp_w;
      }

  this->key = "";

  for (int i = 0; i < key_count; i++) this->key += temp_keys[i];

  return temp_abundance;
}
#endif
vector<basic_entry*> merge_index_cluster(vector <Index_Entry*> *fir, vector<biomes_entry*> *sec){
  int fir_size=fir->size();
  int sec_size=sec->size();
  vector<Index_Entry*> :: iterator fir_iter=fir->begin();
  vector<biomes_entry*> :: iterator sec_iter=sec->begin();
  vector<basic_entry*> res;
  for(sec_iter=sec->begin();sec_iter!=sec->end();sec_iter++){
      for(fir_iter=fir->begin();fir_iter!=fir->end();fir_iter++){

        if((*sec_iter)->getfilename()==(*fir_iter)->getfilename()){
     
          fir->erase(fir_iter);

        
        }
      }
    }

for(fir_iter=fir->begin();fir_iter!=fir->end();fir_iter++){
      res.push_back(*fir_iter);
      //cout<<(*fir_iter)->getfilename()<<endl;
    }
  for(sec_iter=sec->begin();sec_iter!=sec->end();sec_iter++){
      res.push_back(*sec_iter);
      //cout<<(*sec_iter)->getfilename()<<endl;
    }
  
  return res;
  }

double comp_index_cluster(vector<Index_Entry*> fir, vector<biomes_entry*> sec){
  int fir_size=fir.size();
  int sec_size=sec.size();
  double total_size;
  int sim_count=0;

  vector<Index_Entry*> :: iterator fir_iter=fir.begin();
  vector<biomes_entry*> :: iterator sec_iter=sec.begin();
  vector<basic_entry*> merge_res;

  if(fir_size < sec_size){
    total_size=fir_size;
  }
  else{
    total_size=sec_size;
  }
  //vector<basic_entry*> merge_res(total_size);
  merge_res=merge_index_cluster(&fir,&sec);

    
  double score = total_size / merge_res.size();
  return score;
}

void clust_index(map<string,vector<Index_Entry * > * > t1,map<string,vector<biomes_entry * > * >  b1){//考虑减少一个计算重复的循环
  map<string,vector<Index_Entry * > * > :: iterator fir_iter;
  map<string,vector<biomes_entry * > * > :: iterator sec_iter;
  map<string,vector<basic_entry*> > Meta_merge;
  double identity=0;
  for(fir_iter=t1.begin();fir_iter!=t1.end();fir_iter++){
    for(sec_iter=b1.begin();sec_iter!=b1.end();sec_iter++){
          identity=comp_index_cluster(*(fir_iter->second),*(sec_iter->second));
            if(identity >= threshold){
              string newkey=fir_iter->first+sec_iter->first;
              vector<basic_entry*> merge_res=merge_index_cluster(fir_iter->second,sec_iter->second);
              Meta_merge[newkey]=merge_res;
              
            }

    }
  }  

}




int main() {
  basic_entry* array[4];
  Index_Entry a("taxa1");
  Index_Entry b("taxa2");
  biomes_entry c("biomes1");
  biomes_entry d("taxa1");
  vector <basic_entry*> res;
  vector <biomes_entry*> b1;
  vector <Index_Entry*> t1;
  b1.push_back(&c);
  b1.push_back(&d);
  t1.push_back(&a);
  t1.push_back(&b);
  cout<<comp_index_cluster(t1,b1)<<endl;
  /*res=merge_index_cluster(&t1,&b1);
  vector <basic_entry*> :: iterator iter=res.begin();
  for(;iter!=res.end();iter++)
    {
      cout<<(*iter)->getfilename()<<endl;
    }
  */
  return 0;
}