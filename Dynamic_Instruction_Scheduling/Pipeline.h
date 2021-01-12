#ifndef PIPELINE
#define PIPELINE

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <bitset>
#include <stdio.h>
#include <iomanip>
#include <algorithm>


using namespace std;

struct state{
  int cycle;
  int duration;
};

struct instruction{
  int tag;
  int current_state_id;
  int operation_type;
  int register1,register2,register_dest;
  int execution_time;
  int wait_on_1,wait_on_2;
  state history[5];

};

class Pipeline{
public:

Pipeline(int S,int N,string file):m_sched_q_size(S),m_fetch_rate(N),m_cycle(0),
m_last_read_line(0),m_depleted(false),m_trace_file(file){
  for(int i=0;i<128;i++)
    m_register_file[i].push_back(-1);
};
void print_queues();
int get_last_read_line(){return m_last_read_line;}
void Print();
void Execute();
void Issue();
void Dispatch();
void Fetch();
bool Advance_Cycle();

~Pipeline(){};


  private:
      int m_sched_q_size,m_fetch_rate,m_cycle,m_last_read_line;
      bool m_depleted;
      string m_trace_file;
      vector<instruction> m_dispatch_list,m_issue_list,m_execute_list,m_fake_ROB,m_final_list;
      vector<int> m_register_file[128];
};


#endif
