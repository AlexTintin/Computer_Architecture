#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include "Pipeline.h"


using namespace std;

int main(int argc, char *argv[]) {
	vector<string> arguments;
	for (unsigned int i = 0; i < (unsigned int)argc; i++)
        arguments.push_back(argv[i]);
	int k(0);
	Pipeline pip(stoi(arguments[1]),stoi(arguments[2]),arguments[3]);
	do {
		k++;
		pip.Execute();
		pip.Issue();
		pip.Dispatch();
		pip.Fetch();
	} while(pip.Advance_Cycle());
	pip.Print();
	cout<<"number of instructions = "<<pip.get_last_read_line()<<endl;
	cout<<"number of cycles       = "<<k<<endl;
	cout << fixed;
	cout << setprecision(5);
	cout<<"IPC                    = "<<pip.get_last_read_line()/(float)k<<endl;
	return 0;

}
