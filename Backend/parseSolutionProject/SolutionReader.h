#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include "AutoBusData.h"
#include "itinerary.h"

	using namespace std;
	using namespace DataBase;
	using namespace Network;

namespace Builders {

class SolutionReader
{
private:
	string file_name;
	ifstream* file;
	AutoBusData& data;
	string line;

public:
	SolutionReader(string file_name, AutoBusData& data);
	unordered_map<int,Itinerary*>& read_file();

};


}
