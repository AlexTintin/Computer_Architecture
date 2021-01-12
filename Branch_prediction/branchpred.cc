#include "branchpred.h"



void Nsmith(string file_str, int bits)
{

  string line, tag;
  int compteur = pow(2,bits-1);
  int lim = pow(2,bits-1);
  int Max = (int)(pow(2,bits)-1);
  int realval,pred,tot(0),miss(0);
  ifstream file ("../traces/"+file_str);
  if (file.is_open())
  {
    while ( getline(file,line) )
    {
      tag = line.substr(0,6);
      realval = line.substr(7,1)=="n" ? 0:1;
      pred = (compteur>=lim)? 1:0;
      compteur += (realval==1)? 1:-1;
      compteur = max(0, min(compteur, Max));
      miss+= (pred==realval)? 0:1;
      tot+=1;
    }
  file.close();
  }
  else cout << "Unable to open file";
  cout <<"OUTPUT"<<endl;
  std::cout.precision(2);
  std::cout <<std::fixed;
  cout <<"number of predictions:		"<<tot<<endl;
  cout <<"number of mispredictions:	"<<miss<<endl;
  cout <<"misprediction rate:		 "<<100.f*(double)miss/(double)tot<<"%"<<endl;
  cout <<"FINAL COUNTER CONTENT:		"<<compteur<<endl;
  return;
}

void Bimodal(string file_str, int m)
{

  string line, tag,s,index_str;
  int lim = 4;
  int Max = 7;
  int realval,pred,tot(0),miss(0),index;
  std::bitset<32> bin_input;
  vector<int> list;

  for(int i=0; i<pow(2,m);i++)
    list.push_back(4);

  ifstream file ("../traces/"+file_str);
  if (file.is_open())
  {
    while ( getline(file,line) )
    {
      tag = line.substr(0,6);
      bin_input = (std::bitset<32>)stoi(tag, nullptr, 16);
      s = bin_input.to_string();
      index_str = s.substr(30-m,m);
      index = stoi(index_str, nullptr, 2);
      realval = line.substr(7,1)=="n" ? 0:1;
      pred = (list[index]>=lim)? 1:0;
      list[index] += (realval==1)? 1:-1;
      list[index] = max(0, min(list[index], Max));
      miss+= (pred==realval)? 0:1;
      tot+=1;
    }
  file.close();
  }
  else cout << "Unable to open file";
  cout <<"OUTPUT"<<endl;
  std::cout.precision(2);
  std::cout <<std::fixed;
  cout <<"number of predictions:		"<<tot<<endl;
  cout <<"number of mispredictions:	"<<miss<<endl;
  cout<<"misprediction rate:		 "<<100.f*(double)miss/(double)tot<<"%"<<endl;
  cout <<"FINAL BIMODAL CONTENTS		"<<endl;
  for(int i=0; i<pow(2,m);i++)
    cout <<i<<"		"<<list[i] <<endl;
  return;
}



void Gshare(string file_str, int m, int n)
{

  string line, tag,s,index_str,index_str_n,global_hist,Xor_index;
  int lim = 4;
  int Max = 7;
  int realval,pred,tot(0),miss(0),index;
  std::bitset<32> bin_input;
  vector<int> list;

  for(int i=0; i<n;i++)
    global_hist.push_back('0');

  for(int i=0; i<pow(2,m);i++)
    list.push_back(4);

  ifstream file ("../traces/"+file_str);
  if (file.is_open())
  {
    while (getline(file,line) )
    {

      tag = line.substr(0,6);
      bin_input = (std::bitset<32>)stoi(tag, nullptr, 16);
      s = bin_input.to_string();
      index_str = s.substr(30-m,m);
      index_str_n = index_str.substr(m-n,n);
      Xor_index = (std::bitset<32>(index_str_n) ^ std::bitset<32>(global_hist)).to_string().substr(32-n,n);
      index_str.replace(m-n,n,Xor_index);
      index = stoi(index_str, nullptr, 2);
      realval = line.substr(7,1)=="n" ? 0:1;
      pred = (list[index]>=lim)? 1:0;
      list[index] += (realval==1)? 1:-1;
      list[index] = max(0, min(list[index], Max));
      global_hist.pop_back();
      (realval==1)? global_hist.insert(0,"1"):global_hist.insert(0,"0");
      miss+= (pred==realval)? 0:1;
      tot+=1;
    }
  file.close();
  }
  else cout << "Unable to open file";
  cout <<"OUTPUT"<<endl;
  std::cout.precision(2);
  std::cout <<std::fixed;
  cout <<"number of predictions:		"<<tot<<endl;
  cout <<"number of mispredictions:	"<<miss<<endl;
  cout <<"misprediction rate:		 "<<100.f*(double)miss/(double)tot<<"%"<<endl;
  cout <<"FINAL GSHARE CONTENTS		"<<endl;
  for(int i=0; i<pow(2,m);i++)
    cout <<i<<"		"<<list[i] <<endl;
  return;
}


void Hybrid(string file_str, int k, int m1, int n,int m2)
{
  //comon
  string line, tag,s;
  int tot(0),miss(0),lim(4),Max(7),lim_H(2),Max_H(3),realval;
  std::bitset<32> bin_input;

  //Binomial
  string index_str_Bin;
  int pred_Bin,index_Bin;
  vector<int> list_Bin;

  //Gshare
  string index_str_G,index_str_n,global_hist,Xor_index;
  int pred_G,index_G;
  vector<int> list_G;

  //Hybrid
  string index_str;
  int index,pred;
  vector<int> list;

  for(int i=0; i<pow(2,k);i++)
    list.push_back(1);

  for(int i=0; i<n;i++)
    global_hist.push_back('0');

  for(int i=0; i<pow(2,m1);i++)
    list_G.push_back(4);

  for(int i=0; i<pow(2,m2);i++)
    list_Bin.push_back(4);


  ifstream file ("../traces/"+file_str);
  if (file.is_open())
  {
    while (getline(file,line) )
    {

      //common
      tag = line.substr(0,6);
      bin_input = (std::bitset<32>)stoi(tag, nullptr, 16);
      s = bin_input.to_string();
      realval = line.substr(7,1)=="n" ? 0:1;

      //Bimodal(file_str, m2);
      index_str_Bin = s.substr(30-m2,m2);
      index_Bin = stoi(index_str_Bin, nullptr, 2);
      pred_Bin = (list_Bin[index_Bin]>=lim)? 1:0;


      //Gshare(file_str,m1,n);
      index_str_G = s.substr(30-m1,m1);
      index_str_n = index_str_G.substr(m1-n,n);
      Xor_index = (std::bitset<32>(index_str_n) ^ std::bitset<32>(global_hist)).to_string().substr(32-n,n);
      index_str_G.replace(m1-n,n,Xor_index);
      index_G = stoi(index_str_G, nullptr, 2);
      pred_G = (list_G[index_G]>=lim)? 1:0;

      //Hybrid
      index_str = s.substr(30-k,k);
      index = stoi(index_str, nullptr, 2);
      pred = (list[index]>=lim_H)? pred_G:pred_Bin;


      if(list[index]>=lim_H)
      {
        list_G[index_G] += (realval==1)? 1:-1;
        list_G[index_G] = max(0, min(list_G[index_G], Max));
      }
      else
      {
        list_Bin[index_Bin] += (realval==1)? 1:-1;
        list_Bin[index_Bin] = max(0, min(list_Bin[index_Bin], Max));
      }

      global_hist.pop_back();
      (realval==1)? global_hist.insert(0,"1"):global_hist.insert(0,"0");

      if(realval==pred_G && realval==pred_Bin)
        list[index] += 0;
      else if(realval==pred_G && realval!=pred_Bin)
        list[index] += 1;
      else if(realval!=pred_G && realval==pred_Bin)
        list[index] +=-1;

      list[index] = max(0, min(list[index], Max_H));

      miss+= (pred==realval)? 0:1;
      tot+=1;
    }
  file.close();
  }
  else cout << "Unable to open file";
  cout <<"OUTPUT"<<endl;
  std::cout.precision(2);
  std::cout <<std::fixed;
  cout <<"number of predictions:		"<<tot<<endl;
  cout <<"number of mispredictions:	"<<miss<<endl;
  cout <<"misprediction rate:		 "<<100.f*(double)miss/(double)tot<<"%"<<endl;
  cout <<"FINAL CHOOSER CONTENTS		"<<endl;
  for(int i=0; i<pow(2,k);i++)
    cout <<i<<"		"<<list[i] <<endl;
  cout <<"FINAL GSHARE CONTENTS		"<<endl;
  for(int i=0; i<pow(2,m1);i++)
    cout <<i<<"		"<<list_G[i] <<endl;
  cout <<"FINAL BIMODAL CONTENTS		"<<endl;
  for(int i=0; i<pow(2,m2);i++)
    cout <<i<<"		"<<list_Bin[i] <<endl;
  return;
}
