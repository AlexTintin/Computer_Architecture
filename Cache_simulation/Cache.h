#ifndef CACHE
#define CACHE

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <bitset>
#include <stdio.h>


#define ADDRESS_BITS 32

using namespace std;

struct block{
  string tag;
  string line;
  int history;
  int occupied;
  int dirty;
  bool invalid;
};

class Cache{
public:

Cache():m_size(0),m_assoc(0),m_blocksize(0),m_set(0),m_index_bit(0),m_block_offset_bit(0),m_tag_bit(0){};
Cache(int size,int assoc,int blocksize);
~Cache();

void setSize(const int size){m_size = size;}
void setAssoc(const int assoc){m_assoc = assoc;}
void setBlock(const int blocksize){m_blocksize = blocksize;}
void setNumSet(const int set){m_set = set;}
void setNumSet(const int size, int assoc, int blocksize){m_set = size/(blocksize*assoc);}
int getSize(){return m_size;}
int getAssoc(){return m_assoc;}
int getBlock(){return m_blocksize;}
int getNumSet(){return m_set;}
vector<vector<block>> getCache(){return m_allsetstart;}
vector<int> preprocess(string path);
bool setBlockLRUwithL2(string line,int hist,int r_or_w,int* r_miss,int* w_miss, int* w_back,int* w_totL2,int* r_missL2,int* w_missL2,int* w_backL2,Cache* cac2);
bool setBlockLRUwithL2Include(string line,int hist,int hist2,int* track,int r_or_w,int* r_miss,int* w_miss, int* w_back,int* w_totL2,int* r_missL2,int* w_missL2,int* w_backL2,Cache* cac2);
void setBlockLRUInclusiveL2(string line,int hist,int r_or_w,int* r_miss,int* w_miss, int* w_back,Cache* cac1,int* trafic);
void setBlockLRUInclusive(string line,int* trafic);
void setBlockLRU(string line,int hist,int r_or_w,int* r_miss,int* w_miss,int* w_back);
void setBlockPseudoLRU(int index_set,string tag,int hist,int r_or_w,int* r_miss,int* w_miss,int* w_back,vector<int> &binarytree);
void setBlockOpti(int index_set,string tag,int hist,int r_or_w,int* r_miss,int* w_miss, int* w_back,int M);
void printCache();
vector<vector<int>> initBinaryTree();
void updateBinaryTree(vector<int> &binarytree, int index_assoc);
int replaceWithBinTree(vector<int> binarytree);
string readInput(int input,int* index);





  private:
      int m_size,m_assoc,m_blocksize,m_set,m_index_bit,m_block_offset_bit,m_tag_bit;
      vector<vector<block>> m_allsetstart;

};


#endif
