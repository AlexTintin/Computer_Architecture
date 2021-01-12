#include "Pipeline.h"

bool Pipeline::Advance_Cycle()
{

  m_cycle+=1;
  if(m_fake_ROB.empty() || m_depleted)
    return false;
  return true;

}

void Pipeline::Fetch()
{
  int bandwidth(0),k(0),p(0),q(0);
  string line;
  instruction inst;
  ifstream file ("../traces/"+m_trace_file);
  if (file.is_open())
  {

    while ( k<m_last_read_line){
      getline(file,line);
      k++;
    }
    while ( getline(file,line) && bandwidth<m_fetch_rate && (int)m_dispatch_list.size()<2*m_fetch_rate)
    {
      bandwidth++;
      inst.tag = m_last_read_line;
      m_last_read_line++;
      inst.operation_type = stoi(line.substr(7,1),nullptr, 10);

      if(line.substr(9,1)=="-"){
        inst.register_dest = -1;
        p=2;
      }
      else{
        while(line.substr(9+p,1)!=" ")
          p++;
        inst.register_dest = stoi(line.substr(9,p),nullptr, 10);
      }


      if(line.substr(9+p+1,1)=="-"){
        inst.register1 = -1;
        q=2;
      }
      else{
        while(line.substr(9+p+q+1,1)!=" ")
          q++;
        inst.register1 = stoi(line.substr(9+p+1,q),nullptr, 10);
      }

      if(line.substr(9+p+q+2,1)=="-")
        inst.register2 = -1;
      else{
        inst.register2 = stoi(line.substr(9+p+q+2,line.length()-(9+p+q+2)+1),nullptr, 10);
      }

      p=0;
      q=0;
      inst.current_state_id=0;
      inst.history[0].cycle = m_cycle;
      m_fake_ROB.push_back(inst);
      m_dispatch_list.push_back(inst);

    }
  file.close();
  }
  else cout << "Unable to open file";

}

void Pipeline::Dispatch()
{
  for(int i=0; i<(int)m_dispatch_list.size();i++)
  {

    if (m_dispatch_list[i].current_state_id == 1 && (int)m_issue_list.size()<m_sched_q_size)
    {

      m_dispatch_list[i].history[2].cycle = m_cycle;
      m_dispatch_list[i].history[1].duration = m_cycle-m_dispatch_list[i].history[1].cycle;
      m_dispatch_list[i].current_state_id = 2;

      m_dispatch_list[i].wait_on_1 = -1;
      m_dispatch_list[i].wait_on_2 = -1;

      if(m_dispatch_list[i].register1!=-1)
      {
        if(m_register_file[m_dispatch_list[i].register1][0]!=-1)
          m_dispatch_list[i].wait_on_1 = m_register_file[m_dispatch_list[i].register1][m_register_file[m_dispatch_list[i].register1].size()-1];
      }
      if(m_dispatch_list[i].register2!=-1)
      {
        if(m_register_file[m_dispatch_list[i].register2][0]!=-1)
          m_dispatch_list[i].wait_on_2 = m_register_file[m_dispatch_list[i].register2][m_register_file[m_dispatch_list[i].register2].size()-1];
      }

      if(m_dispatch_list[i].register_dest!=-1)
      {
        if(m_register_file[m_dispatch_list[i].register_dest][0]!=-1)
          m_register_file[m_dispatch_list[i].register_dest].push_back(m_dispatch_list[i].tag);
        else
          m_register_file[m_dispatch_list[i].register_dest][0] = m_dispatch_list[i].tag;
      }



      m_issue_list.push_back(m_dispatch_list[i]);
      m_dispatch_list.erase(m_dispatch_list.begin()+i);
      i--;
    }
    else if (m_dispatch_list[i].current_state_id == 0)
    {
      m_dispatch_list[i].history[1].cycle = m_cycle;
      m_dispatch_list[i].history[0].duration = m_cycle-m_dispatch_list[i].history[0].cycle;
      m_dispatch_list[i].current_state_id = 1;
    }

  }

}


void Pipeline::Issue()
{
  int bandwidth(0);
  bool ready;
  for(int i=0; i<(int)m_issue_list.size();i++)
  {
    ready=true;
    if(m_issue_list[i].wait_on_1!=-1){
      for (int j=0; j<(int)m_fake_ROB.size();j++)
      {
        if(m_fake_ROB[j].tag == m_issue_list[i].wait_on_1){
          ready = false;
          break;
        }
      }
    }
    if(m_issue_list[i].wait_on_2!=-1){
      for (int j=0; j<(int)m_fake_ROB.size();j++)
      {
        if(m_fake_ROB[j].tag == m_issue_list[i].wait_on_2){
          ready = false;
          break;
        }
      }
    }


    if ((m_issue_list[i].register1==-1||m_register_file[m_issue_list[i].register1][0]==-1|| ready)
      && (m_issue_list[i].register2==-1||m_register_file[m_issue_list[i].register2][0]==-1|| ready)
      && bandwidth<m_fetch_rate+1)
    {
      bandwidth++;
      m_issue_list[i].history[3].cycle = m_cycle;
      m_issue_list[i].history[2].duration = m_cycle-m_issue_list[i].history[2].cycle;
      m_issue_list[i].current_state_id = 3;
      m_issue_list[i].execution_time = 0;


      m_execute_list.push_back(m_issue_list[i]);
      m_issue_list.erase(m_issue_list.begin()+i);
      i--;
    }

  }

}

void Pipeline::Execute()
{
  for(int i=0; i<(int)m_execute_list.size();i++)
  {

    if (m_execute_list[i].operation_type==0)
    {
      m_execute_list[i].history[4].cycle = m_cycle;
      m_execute_list[i].history[3].duration = m_cycle-m_execute_list[i].history[3].cycle;
      m_execute_list[i].current_state_id = 4;
      m_execute_list[i].history[4].duration = 1;


      for (int j=0; j<(int)m_fake_ROB.size();j++)
      {
        if(m_fake_ROB[j].tag == m_execute_list[i].tag){
          m_fake_ROB.erase(m_fake_ROB.begin()+j);
          break;
        }
      }
      if (m_execute_list[i].register_dest!=-1)
      {
        if(m_register_file[m_execute_list[i].register_dest].size()>1)
        /*  for(int j=0; j<m_register_file[m_execute_list[i].register_dest].size();j++)
          {
            if(m_register_file[m_execute_list[i].register_dest][j] == m_execute_list[i].tag){
              m_register_file[m_execute_list[i].register_dest].erase(m_register_file[m_execute_list[i].register_dest].begin()+j);
              break;
            }
          }
        }*/
          m_register_file[m_execute_list[i].register_dest].erase(m_register_file[m_execute_list[i].register_dest].begin());
        /*m_register_file[m_execute_list[i].register_dest].erase(m_register_file[m_execute_list[i].register_dest].begin()
      +(*find(m_register_file[m_execute_list[i].register_dest].begin(), m_register_file[m_execute_list[i].register_dest].end(), m_execute_list[i].tag)));*/
        else
          m_register_file[m_execute_list[i].register_dest][0]=-1;
        }
      m_final_list.push_back(m_execute_list[i]);
      m_execute_list.erase(m_execute_list.begin()+i);
      i--;

    }
    else if (m_execute_list[i].operation_type==1 && m_execute_list[i].execution_time==1)
    {
      m_execute_list[i].history[4].cycle = m_cycle;
      m_execute_list[i].history[3].duration = m_cycle-m_execute_list[i].history[3].cycle;
      m_execute_list[i].current_state_id = 4;
      m_execute_list[i].history[4].duration = 1;


      for (int j=0; j<(int)m_fake_ROB.size();j++)
      {
        if(m_fake_ROB[j].tag == m_execute_list[i].tag){
          m_fake_ROB.erase(m_fake_ROB.begin()+j);
          break;
        }
      }
      if (m_execute_list[i].register_dest!=-1)
      {
        if(m_register_file[m_execute_list[i].register_dest].size()>1)
          /*  for(int j=0; j<m_register_file[m_execute_list[i].register_dest].size();j++)
            {
              if(m_register_file[m_execute_list[i].register_dest][j] == m_execute_list[i].tag){
                m_register_file[m_execute_list[i].register_dest].erase(m_register_file[m_execute_list[i].register_dest].begin()+j);
                break;
              }
            }
          }*/
            m_register_file[m_execute_list[i].register_dest].erase(m_register_file[m_execute_list[i].register_dest].begin());
        else
            m_register_file[m_execute_list[i].register_dest][0]=-1;
      }
      m_final_list.push_back(m_execute_list[i]);
      m_execute_list.erase(m_execute_list.begin()+i);
      i--;
    }
    else if (m_execute_list[i].operation_type==2 && m_execute_list[i].execution_time==4)
    {
      m_execute_list[i].history[4].cycle = m_cycle;
      m_execute_list[i].history[3].duration = m_cycle-m_execute_list[i].history[3].cycle;
      m_execute_list[i].current_state_id = 4;
      m_execute_list[i].history[4].duration = 1;


      for (int j=0; j<(int)m_fake_ROB.size();j++)
      {
        if(m_fake_ROB[j].tag == m_execute_list[i].tag){
          m_fake_ROB.erase(m_fake_ROB.begin()+j);
          break;
        }
      }
      if (m_execute_list[i].register_dest!=-1)
      {
        if(m_register_file[m_execute_list[i].register_dest].size()>1)
          /*  for(int j=0; j<m_register_file[m_execute_list[i].register_dest].size();j++)
            {
              if(m_register_file[m_execute_list[i].register_dest][j] == m_execute_list[i].tag){
                m_register_file[m_execute_list[i].register_dest].erase(m_register_file[m_execute_list[i].register_dest].begin()+j);
                break;
              }
            }
          }*/
            m_register_file[m_execute_list[i].register_dest].erase(m_register_file[m_execute_list[i].register_dest].begin());
        else
            m_register_file[m_execute_list[i].register_dest][0]=-1;
      }
      m_final_list.push_back(m_execute_list[i]);
      m_execute_list.erase(m_execute_list.begin()+i);
      i--;
    }
    else
      m_execute_list[i].execution_time++;
  }

}
bool order_inst_list(instruction i,instruction j) { return (i.tag<j.tag); }

void Pipeline::Print()
{
  sort(m_final_list.begin(), m_final_list.end(), order_inst_list);
  for(int i=0; i<(int)m_final_list.size();i++)
  {
    cout<<m_final_list[i].tag<<" fu{"<<m_final_list[i].operation_type<<"} src{"<<
    m_final_list[i].register1<<","<<m_final_list[i].register2<<"} dst{"<<
    m_final_list[i].register_dest<<
    "} IF{"<<m_final_list[i].history[0].cycle<<","<<m_final_list[i].history[0].duration<<
    "} ID{"<<m_final_list[i].history[1].cycle<<","<<m_final_list[i].history[1].duration<<
    "} IS{"<<m_final_list[i].history[2].cycle<<","<<m_final_list[i].history[2].duration<<
    "} EX{"<<m_final_list[i].history[3].cycle<<","<<m_final_list[i].history[3].duration<<
    "} WB{"<<m_final_list[i].history[4].cycle<<","<<m_final_list[i].history[4].duration<<
    "}"<<endl;

}
}

void Pipeline::print_queues()
{
  for(int i=0; i<(int)m_dispatch_list.size();i++)
  {
    cout<<"fu  "<<m_dispatch_list[i].tag<<"   state:  "<<m_dispatch_list[i].current_state_id<<endl;
  }
  for(int i=0; i<(int)m_issue_list.size();i++)
  {
    cout<<"fu  "<<m_issue_list[i].tag<<"  state:  "<<m_issue_list[i].current_state_id<<endl;
  }
  for(int i=0; i<(int)m_execute_list.size();i++)
  {
    cout<<"fu  "<<m_execute_list[i].tag<<"  state:  "<<m_execute_list[i].current_state_id<<endl;
  }
  for(int i=0; i<(int)m_final_list.size();i++)
  {
    cout<<"fu  "<<m_final_list[i].tag<<"  state:  "<<m_final_list[i].current_state_id<<endl;
  }
  cout<<endl;
}
