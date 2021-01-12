#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include "branchpred.h"


using namespace std;

int main(int argc, char *argv[]) {
	string file;
	vector<string> arguments;
	cout<<"COMMAND"<<endl;
	for (unsigned int i = 0; i < (unsigned int)argc; i++)
        arguments.push_back(argv[i]);
	if(arguments[1]=="smith")
	{
		cout<<arguments[0]<<" "<<arguments[1]<<" "<<arguments[2]<<" "<<arguments[3]<<endl;
		Nsmith(arguments[3], stoi(arguments[2]));
	}
	else if(arguments[1]=="bimodal")
	{
		cout<<arguments[0]<<" "<<arguments[1]<<" "<<arguments[2]<<" "<<arguments[3]<<endl;
		Bimodal(arguments[3], stoi(arguments[2]));
	}
	else if(arguments[1]=="gshare")
	{
		cout<<arguments[0]<<" "<<arguments[1]<<" "<<arguments[2]<<" "<<arguments[3]<<
		" "<<arguments[4]<<endl;
		Gshare(arguments[4], stoi(arguments[2]),stoi(arguments[3]));
	}
	else if(arguments[1]=="hybrid")
	{
		cout<<arguments[0]<<" "<<arguments[1]<<" "<<arguments[2]<<" "<<arguments[3]<<
		" "<<arguments[4]<<" "<<arguments[5]<<" "<<arguments[6]<<endl;
		Hybrid(arguments[6], stoi(arguments[2]),stoi(arguments[3]),stoi(arguments[4]),stoi(arguments[5]));
	}

  return 0;
}
