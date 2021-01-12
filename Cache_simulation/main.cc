#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include "world.h"
#include "Cache.h"


using namespace std;

int main(int argc, char *argv[]) {
	int block,L1_size,L1_assoc,L2_size,L2_assoc,replacement,inclusion;
	string trace_file;
	vector<string> arguments;
	for (unsigned int i = 0; i < (unsigned int)argc; i++)
        arguments.push_back(argv[i]);
	block = stoi(arguments[1]);
	L1_size =stoi(arguments[2]);
	L1_assoc = stoi(arguments[3]);
	L2_size = stoi(arguments[4]);
	L2_assoc = stoi(arguments[5]);
	replacement =stoi(arguments[6]);
	inclusion = stoi(arguments[7]);
	trace_file = (arguments[8]);;
	string line,tag,tag2;
	string startofline;
	bool needL2;

  ifstream file ("../traces/"+trace_file);
  if (file.is_open())
  {
		vector<int> fullhist;
		vector<vector<int>> binarytree;
		int hist(1),hist2(1),track(0);
		Cache cac(L1_size,L1_assoc,block);
		Cache cac2;
		int r_tot2(0),w_tot2(0),r_miss2(0),w_miss2(0), w_back2(0),index2(0),trafic(0);
		if(L2_size!=0){
			Cache cac3(L2_size,L2_assoc,block);
			cac2=cac3;
		}
		bool safe(false);
		if (replacement==2)
			 fullhist =	cac.preprocess("../traces/"+trace_file);
		if (replacement==1){
			 binarytree = cac.initBinaryTree();
			 //if(L2_size==0)
			 	//binarytree2 = cac2.initBinaryTree();
		}

		int index(0);
		int r_or_w; //0 for r, 1 for w
		int r_tot(0),w_tot(0),r_miss(0),w_miss(0), w_back(0);

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
				if (hist==1){
					r_or_w = line.substr(3,1)=="r" ? 0:1;
					line.substr(3,1)=="r" ? r_tot++:w_tot++;
					line.erase(0,5);		//weird stuff for 1st line
				}
				else{
					r_or_w = line.substr(0,1)=="r" ? 0:1;
					line.substr(0,1)=="r" ? r_tot++:w_tot++;
					line.erase(0,2);
				}
				if(L2_size==0){
					if (replacement==0)
						cac.setBlockLRU(line,hist,r_or_w,&r_miss,&w_miss,& w_back);
					if(replacement==1){
						tag = cac.readInput(stoi(line, nullptr, 16), &index);
						cac.setBlockPseudoLRU(index,tag,hist,r_or_w,&r_miss,&w_miss,& w_back,binarytree[index]);
					}
					if(replacement==2){
						tag = cac.readInput(stoi(line, nullptr, 16), &index);
						cac.setBlockOpti(index,tag,fullhist[hist-1],r_or_w,&r_miss,&w_miss,& w_back,(int)fullhist.size()+1);
					}
				}
				else{
					if(inclusion==0){
							needL2 = cac.setBlockLRUwithL2(line,hist,r_or_w,&r_miss,&w_miss,& w_back,&w_tot2,&r_miss2,&w_miss2, &w_back2,&cac2);
							if(needL2){
								r_tot2+=1;
								cac2.setBlockLRU(line,2*hist+1,0,&r_miss2,&w_miss2,& w_back2);
							}
						}
					else if (inclusion==1){
						needL2 = cac.setBlockLRUwithL2Include(line,hist,hist2,&track,r_or_w,&r_miss,&w_miss,& w_back,&w_tot2,&r_miss2,&w_miss2, &w_back2,&cac2);
						if(needL2){
							r_tot2+=1;
							hist2++;
							cac2.setBlockLRUInclusiveL2(line,hist2,r_or_w,&r_miss2,&w_miss2,& w_back2,&cac,&trafic);
						}
					}
				}
				hist2++;
				hist++;
			}
    }
    file.close();
		cout <<"===== Simulator configuration ====="<<endl;
		cout <<"BLOCKSIZE:             "<<block<<endl;
		cout <<"L1_SIZE:               "<<L1_size<<endl;
		cout <<"L1_ASSOC:              "<<L1_assoc<<endl;
		cout <<"L2_SIZE:               "<<L2_size<<endl;
		cout <<"L2_ASSOC:              "<<L2_assoc<<endl;
		cout <<"REPLACEMENT POLICY:    "<<((replacement==0)?"LRU":(replacement==1)?"Pseudo-LRU":"Optimal")<<endl;
		cout <<"INCLUSION PROPERTY:    "<<((inclusion==0)?"non-inclusive":"inclusive")<<endl;
		cout <<"trace_file:            "<<trace_file<<endl;

		cout <<"===== L1 contents ====="<<endl;
		cac.printCache();
		if(L2_size!=0){
			cout <<"===== L2 contents ====="<<endl;
			cac2.printCache();
		}
		cout <<"===== Simulation results (raw) ====="<<endl;
		cout <<std::dec<<"a. number of L1 reads:        "<< r_tot<<endl;
		cout <<"b. number of L1 read misses:  "<< r_miss<<endl;
		cout <<"c. number of L1 writes:       "<< w_tot<<endl;
		cout <<"d. number of L1 write misses: "<< w_miss<<endl;
		cout <<"e. L1 miss rate:              "<< left << setfill('0') << setw(8) <<(double)((w_miss+r_miss)/(float)(r_tot+w_tot))<<endl;
		cout <<"f. number of L1 writebacks:   "<< w_back<<endl;
		cout <<"g. number of L2 reads:        "<< r_tot2<<endl;
		cout <<"h. number of L2 read misses:  "<< r_miss2<<endl;
		cout <<"i. number of L2 writes:       "<<w_tot2<<endl;
		cout <<"j. number of L2 write misses: "<<w_miss2<<endl;
		cout <<"k. L2 miss rate:              "<< left << setfill('0') << ((r_tot2==0)?setw(0):setw(8)) <<((r_tot2==0)?0:(double)((r_miss2)/(float)(r_tot2)))<<endl;
		cout <<"l. number of L2 writebacks:   "<<w_back2<<endl;
		cout <<"m. total memory traffic:      "<<((r_tot2==0)?(r_miss+w_miss+w_back):(r_miss2+w_miss2+w_back2+trafic))<<endl;
  }

  else cout << "Unable to open file";
  return 0;
}
