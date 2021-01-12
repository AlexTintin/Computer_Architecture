#include "Cache.h"


Cache::Cache(int size,int assoc,int blocksize){
  m_size = (size);
  m_assoc = (assoc);
  m_blocksize = (blocksize);
  m_set = (size/(blocksize*assoc));
  m_index_bit = (log2(m_set));
  m_block_offset_bit = (log2(m_blocksize));
  m_tag_bit = (32-m_index_bit-m_block_offset_bit);
  block b;
  b.occupied =0;
  b.history  = 0;
  b.dirty=0;
  b.tag  = "0000";
  b.line="blabla";
  b.invalid=false;
  vector<block> set;
  for (int i=0; i<m_assoc;i++)
  {
    set.push_back(b);
  }
  for(int j=0;j<m_set;j++)
  {
    m_allsetstart.push_back(set);
  }
}


Cache::~Cache(){
}

vector<int> Cache::preprocess(string path){
	ifstream file(path);
	string line,newline,startofline;
	vector<int> fullhist;
	vector<string>filestring;
	int nbline(0),hist(0);
	bool safe(false);
	if (file.is_open())
  {
    while ( getline(file,line) )
    {
			safe=false;
			for(int i=0; i<(int)line.size();i++){
					startofline=line[i];
					if(startofline=="r" || startofline=="w"){
						safe=true;
						break;
					}
				}
			if(safe){
					if (nbline==0)
						line.erase(0,5);		//weird stuff for 1st line
					else
						line.erase(0,2);
					nbline++;
					filestring.push_back(line);
				}
			}
		file.close();
	}
	else cout << "Unable to open file";
int index(0),index2;
string tag,tag2;
	for(int i=0;i<nbline-1;i++){
			line = filestring[i];
      tag = this->readInput(stoi(line, nullptr, 16), &index);
			hist=0;
			//found =false;
			for (int k=i+1;k<nbline;k++)
	    {
				newline = filestring[k];
        tag2 = this->readInput(stoi(newline, nullptr, 16), &index2);
				hist+=1;
				if(tag2==tag && index==index2)
					break;
	    }
			if(hist==nbline-i-1)
				hist=nbline+1;
			fullhist.push_back(hist);
    }

		fullhist.push_back(nbline+1);
		return fullhist;
}



string Cache::readInput(int input, int* index){
  std::bitset<32> bin_input(input);
	string s = bin_input.to_string();
	string tag = s.substr(0,m_tag_bit);
  string index_str = s.substr(m_tag_bit,m_index_bit);
  if(m_index_bit==0)
    *index=0;
  else
    *index = stoi(index_str, nullptr, 2);
  return tag;
}


void Cache::setBlockOpti(int index_set,string tag,int hist,int r_or_w,int* r_miss,int* w_miss, int* w_back,int M){
  block b;
  //int oldest_block(0);
  int block_to_replace_so_far(-1);

  for(int j=0; j<m_set;j++){
  for (int i=0; i<m_assoc;i++){
    if(m_allsetstart[j][i].history!=M)
      m_allsetstart[j][i].history =m_allsetstart[j][i].history-1 ;
    }
  }
    for (int i=0; i<m_assoc;i++)
    {
      if(m_allsetstart[index_set][i].occupied == 0)
      {
        m_allsetstart[index_set][i].tag = tag;
        m_allsetstart[index_set][i].history = hist;
        m_allsetstart[index_set][i].occupied = 1;
        if (r_or_w==1){
          *w_miss+=1;
          m_allsetstart[index_set][i].dirty = 1;
        }
        else{
          *r_miss+=1;
          //m_allsetstart[index_set][i].dirty = 0;
        }  /*for(int j =0; j<m_set;j++){
		for(int i=0; i<m_assoc;++i){
			delete m_allsetstart[j][i];
		}
	}*/
        return;
      }
      else
      {
        if(m_allsetstart[index_set][i].tag==tag){
          m_allsetstart[index_set][i].history = hist;
          if (r_or_w==1){
            m_allsetstart[index_set][i].dirty = 1;
          }
          return;
        }

      }
    }
    block_to_replace_so_far=(m_allsetstart[index_set][0].history>=m_allsetstart[index_set][1].history)?0:1;
    //if(index_set==0)
      //cout<<m_allsetstart[0][0].history<< "     "<<m_allsetstart[0][1].history<<"     "<<block_to_replace_so_far<<endl;
    if (block_to_replace_so_far!=-1)
    {
      m_allsetstart[index_set][block_to_replace_so_far].tag = tag;
      m_allsetstart[index_set][block_to_replace_so_far].history = hist;
      if(m_allsetstart[index_set][block_to_replace_so_far].dirty == 1)
        *w_back+=1;
      if (r_or_w==1){
        *w_miss+=1;
        m_allsetstart[index_set][block_to_replace_so_far].dirty = 1;
      }
      else{
        *r_miss+=1;
        m_allsetstart[index_set][block_to_replace_so_far].dirty = 0;
      }
      return;
    }
  }



bool Cache::setBlockLRUwithL2(string line,int hist,int r_or_w,int* r_miss,int* w_miss, int* w_back,int* w_totL2,int* r_missL2,int* w_missL2,int* w_backL2,Cache* cac2){
  //block b;
  int index_set(0);//,index2(0);
  string tag;
  tag = this->readInput(stoi(line, nullptr, 16), &index_set);
  int oldest_block(hist);
  int block_to_replace_so_far(-1);
  //int r_totL2(0),w_totL2(0),r_missL2(0),w_missL2(0), w_backL2(0);
  for (int i=0; i<m_assoc;i++)
  {
    if(m_allsetstart[index_set][i].occupied == 0)
    {
      m_allsetstart[index_set][i].tag = tag;
      m_allsetstart[index_set][i].history = hist;
      m_allsetstart[index_set][i].occupied = 1;
      m_allsetstart[index_set][i].line = line;
      if (r_or_w==1){
        *w_miss+=1;
        //*w_back+=1;
        m_allsetstart[index_set][i].dirty = 1;
      }
      else{
        *r_miss+=1;
        //m_allsetstart[index_set][i].dirty = 0;
      }
      return true;
    }
    else
    {
      if(m_allsetstart[index_set][i].tag==tag){
        m_allsetstart[index_set][i].history = hist;
        m_allsetstart[index_set][i].line = line;
        if (r_or_w==1){
          m_allsetstart[index_set][i].dirty = 1;
        }
        return false;
      }
      if(oldest_block > m_allsetstart[index_set][i].history)
      {
        oldest_block = m_allsetstart[index_set][i].history;
        block_to_replace_so_far = i;
      }

    }
  }
  if (block_to_replace_so_far!=-1)
  {
    m_allsetstart[index_set][block_to_replace_so_far].tag = tag;
    m_allsetstart[index_set][block_to_replace_so_far].history = hist;
    if(m_allsetstart[index_set][block_to_replace_so_far].dirty == 1){
      *w_back+=1;
      cac2->setBlockLRU(m_allsetstart[index_set][block_to_replace_so_far].line,2*hist,1,r_missL2,w_missL2,w_backL2);
      *w_totL2+=1 ;
      m_allsetstart[index_set][block_to_replace_so_far].dirty = 0;
    }
    if (r_or_w==1){
      *w_miss+=1;
      m_allsetstart[index_set][block_to_replace_so_far].dirty = 1;
    }
    else{
      *r_miss+=1;
      m_allsetstart[index_set][block_to_replace_so_far].dirty = 0;
    }
    m_allsetstart[index_set][block_to_replace_so_far].line = line;
    return true;
  }
}

void Cache::setBlockLRU(string line,int hist,int r_or_w,int* r_miss,int* w_miss, int* w_back){
  int oldest_block(hist);
  int block_to_replace_so_far(-1);
  string tag;
  int index_set(0);
  tag = this->readInput(stoi(line, nullptr, 16), &index_set);

  for (int i=0; i<m_assoc;i++)
  {
    if(m_allsetstart[index_set][i].occupied == 0)
    {
      m_allsetstart[index_set][i].tag = tag;
      m_allsetstart[index_set][i].history = hist;
      m_allsetstart[index_set][i].occupied = 1;
      m_allsetstart[index_set][i].line = line;
      if (r_or_w==1){
        *w_miss+=1;
        m_allsetstart[index_set][i].dirty = 1;
      }
      else{
        *r_miss+=1;
        m_allsetstart[index_set][i].dirty = 0;
      }
      return;
    }
    else
    {
      if(m_allsetstart[index_set][i].tag==tag){
        m_allsetstart[index_set][i].history = hist;
        m_allsetstart[index_set][i].line = line;
        if (r_or_w==1){
          m_allsetstart[index_set][i].dirty = 1;
        }
        return;
      }
      if(oldest_block > m_allsetstart[index_set][i].history)
      {
        oldest_block = m_allsetstart[index_set][i].history;
        block_to_replace_so_far = i;
      }

    }
  }
  if (block_to_replace_so_far!=-1)
  {
    m_allsetstart[index_set][block_to_replace_so_far].tag = tag;
    m_allsetstart[index_set][block_to_replace_so_far].history = hist;
    m_allsetstart[index_set][block_to_replace_so_far].line = line;
    if(m_allsetstart[index_set][block_to_replace_so_far].dirty == 1)
      *w_back+=1;
    if (r_or_w==1){
      *w_miss+=1;
      m_allsetstart[index_set][block_to_replace_so_far].dirty = 1;
    }
    else{
      *r_miss+=1;
      m_allsetstart[index_set][block_to_replace_so_far].dirty = 0;
    }
    return;
  }
}

bool Cache::setBlockLRUwithL2Include(string line,int hist,int hist2,int* track,int r_or_w,int* r_miss,int* w_miss, int* w_back,int* w_totL2,int* r_missL2,int* w_missL2,int* w_backL2,Cache* cac2){
  //block b;
  int index_set(0);//,index2(0);
  string tag;
  tag = this->readInput(stoi(line, nullptr, 16), &index_set);
  int oldest_block(hist);
  int block_to_replace_so_far(-1);
  //int r_totL2(0),w_totL2(0),r_missL2(0),w_missL2(0), w_backL2(0);
  if(index_set==16)
    *track+=1;
  for (int i=0; i<m_assoc;i++)
  {
    if(m_allsetstart[index_set][i].occupied == 0)
    {
      m_allsetstart[index_set][i].tag = tag;
      m_allsetstart[index_set][i].history = hist;
      m_allsetstart[index_set][i].occupied = 1;
      m_allsetstart[index_set][i].line = line;
      if (r_or_w==1){
        *w_miss+=1;
        //*w_back+=1;
        m_allsetstart[index_set][i].dirty = 1;
      }
      else{
        *r_miss+=1;
        //m_allsetstart[index_set][i].dirty = 0;
      }

      return true;
    }
    else
    {
      if(m_allsetstart[index_set][i].tag==tag){
        m_allsetstart[index_set][i].history = hist;
        m_allsetstart[index_set][i].line = line;
        if (r_or_w==1){
          m_allsetstart[index_set][i].dirty = 1;
        }
        m_allsetstart[index_set][i].invalid = false;
        return false;
      }
      if(oldest_block > m_allsetstart[index_set][i].history)
      {
        oldest_block = m_allsetstart[index_set][i].history;
        block_to_replace_so_far = i;
      }

    }
  }
  for (int i=0; i<m_assoc;i++)
  {
    if(m_allsetstart[index_set][i].invalid){
      m_allsetstart[index_set][i].tag=tag;
      m_allsetstart[index_set][i].history = hist;
      m_allsetstart[index_set][i].line = line;
      m_allsetstart[index_set][i].invalid = false;
      if(m_allsetstart[index_set][i].invalid)
        cout<<"ok"<<endl;
      if (r_or_w==1){
        *w_miss+=1;
        m_allsetstart[index_set][i].dirty = 1;
      }
      else{
        *r_miss+=1;
        m_allsetstart[index_set][i].dirty = 0;
      }
      return true;
    }
  }

  if (block_to_replace_so_far!=-1)
  {
    m_allsetstart[index_set][block_to_replace_so_far].tag = tag;
    m_allsetstart[index_set][block_to_replace_so_far].history = hist;
    if(m_allsetstart[index_set][block_to_replace_so_far].dirty == 1){
      *w_back+=1;
      //tag2 = cac2->readInput(stoi(m_allsetstart[index_set][block_to_replace_so_far].line, nullptr, 16), &index2);
      cac2->setBlockLRU(m_allsetstart[index_set][block_to_replace_so_far].line,hist2,1,r_missL2,w_missL2,w_backL2);
      *w_totL2+=1 ;
      //m_allsetstart[index_set][block_to_replace_so_far].dirty = 0;
    }
    if (r_or_w==1){
      *w_miss+=1;
      m_allsetstart[index_set][block_to_replace_so_far].dirty = 1;
    }
    else{
      *r_miss+=1;
      m_allsetstart[index_set][block_to_replace_so_far].dirty = 0;
    }
    m_allsetstart[index_set][block_to_replace_so_far].line = line;
    return true;
  }
}

void Cache::setBlockLRUInclusiveL2(string line,int hist,int r_or_w,int* r_miss,int* w_miss, int* w_back,Cache* cac1,int* trafic){
  int oldest_block(hist);
  int block_to_replace_so_far(-1);
  int index_set(0);
  string tag;
  tag = this->readInput(stoi(line, nullptr, 16), &index_set);

  for (int i=0; i<m_assoc;i++)
  {
    if(m_allsetstart[index_set][i].occupied == 0)
    {
      m_allsetstart[index_set][i].tag = tag;
      m_allsetstart[index_set][i].history = hist;
      m_allsetstart[index_set][i].occupied = 1;
      m_allsetstart[index_set][i].line = line;
      *r_miss+=1;
        //m_allsetstart[index_set][i].dirty = 0;
      return;
    }
    else
    {
      if(m_allsetstart[index_set][i].tag==tag){
        m_allsetstart[index_set][i].history = hist;
        m_allsetstart[index_set][i].line = line;
        return;
      }
      if(oldest_block > m_allsetstart[index_set][i].history)
      {
        oldest_block = m_allsetstart[index_set][i].history;
        block_to_replace_so_far = i;
      }

    }
  }
  if (block_to_replace_so_far!=-1)
  {
    m_allsetstart[index_set][block_to_replace_so_far].tag = tag;
    m_allsetstart[index_set][block_to_replace_so_far].history = hist;
    if(m_allsetstart[index_set][block_to_replace_so_far].dirty == 1){
      *w_back+=1;
      //return false;
    }
    *r_miss+=1;
    m_allsetstart[index_set][block_to_replace_so_far].dirty = 0;
    cac1->setBlockLRUInclusive(m_allsetstart[index_set][block_to_replace_so_far].line,trafic);

    m_allsetstart[index_set][block_to_replace_so_far].line = line;
    return ;
  }
}

void Cache::setBlockLRUInclusive(string line,int* trafic){
  int index_set(0);
  string tag;
  tag = this->readInput(stoi(line, nullptr, 16), &index_set);

  for (int i=0; i<m_assoc;i++)
  {
      if(m_allsetstart[index_set][i].tag==tag){
        m_allsetstart[index_set][i].invalid=true;
        m_allsetstart[index_set][i].tag="bla";
        if(  m_allsetstart[index_set][i].dirty==1)
                  *trafic+=1;
        return;
      }
}
return;
}

void Cache::updateBinaryTree(vector<int> &binarytree, int index_assoc){
    int layers = (int)log2(m_assoc);
    int left(0);
    for (int i=0; i<layers;i++)
    {

      if(((float)(index_assoc+1)/(float)(m_assoc/(float)(i+1)))>0.5){
          binarytree[2*i-left]=0;
          index_assoc = (int)((index_assoc+1)/2.f)-1;
      }
      else{
          binarytree[2*i-left]=1;
          left+=1;
        }
    }
    //binarytree[0] = index_assoc;
}

vector<vector<int>> Cache::initBinaryTree(){
    vector<vector<int>> binarytree;
    vector<int> treeset;
    int numnodes = pow(2,m_assoc-1)-1;
    for (int i=0; i<numnodes;i++)
    {
      treeset.push_back(0);
      for (int i=0; i<m_set;i++)
        binarytree.push_back(treeset);
    }

    return binarytree;
}

int Cache::replaceWithBinTree(vector<int> binarytree){
  int layers = (int)log2(m_assoc);
  int index(0);
  for (int i=0; i<layers;i++)
  {
    index += (binarytree[i+index]==0)? 0:(int)m_assoc/(2.f*(i+1));
  }
  return index;
  //return (binarytree[0]==0) 1:0;

}

void Cache::setBlockPseudoLRU(int index_set,string tag,int hist,int r_or_w,int* r_miss,int* w_miss, int* w_back, vector<int> &binarytree){
  block b;
  int index_to_replace(0);
  for (int i=0; i<m_assoc;i++)
  {
    if(m_allsetstart[index_set][i].occupied == 0)
    {
      updateBinaryTree(binarytree,i);
      m_allsetstart[index_set][i].tag = tag;
      //m_allsetstart[index_set][i].history = hist;
      m_allsetstart[index_set][i].occupied = 1;
      if (r_or_w==1){
        *w_miss+=1;
        m_allsetstart[index_set][i].dirty = 1;
      }
      else{
        *r_miss+=1;
        //m_allsetstart[index_set][i].dirty = 0;
      }
      return;
    }

    else
    {
      if(m_allsetstart[index_set][i].tag==tag){
        updateBinaryTree(binarytree,i);
        //cout<<binarytree[0]<<endl;
        if (r_or_w==1){
          m_allsetstart[index_set][i].dirty = 1;
        }
        return;
      }
    }
  }
    //cout<<binarytree[0]<<endl;
    index_to_replace = replaceWithBinTree(binarytree);
    //binarytree[0] = (binarytree[0]==0) ? 1:0;
    updateBinaryTree(binarytree,index_to_replace);
    m_allsetstart[index_set][index_to_replace].tag = tag;
    //m_allsetstart[index_set][index_to_replace].history = hist;
    if(m_allsetstart[index_set][index_to_replace].dirty == 1)
      *w_back+=1;
    if (r_or_w==1){
      *w_miss+=1;
      m_allsetstart[index_set][index_to_replace].dirty = 1;
    }
    else{
      *r_miss+=1;
      m_allsetstart[index_set][index_to_replace].dirty = 0;
    }
    return;
  }



void Cache::printCache(){
  for(int j=0;j<m_set;j++)
  {
    cout<<std::dec<<"Set     "<<j<<":	";
    for (int i=0; i<m_assoc;i++)
    {
      cout<<std::hex<< std::stoi(m_allsetstart[j][i].tag,nullptr,2);
      if (m_allsetstart[j][i].dirty==1)
        cout<<" D 	";
      else
        cout<<"   	";
    }
    cout<<endl;

  }


}
