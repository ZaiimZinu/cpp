#ifndef DATA_READER_H_INCLUDED
#define DATA_READER_H_INCLUDED

#pragma once
#include <string>
#include "Graphe.h"
#include "AutoBusData.h"

using namespace std;
using namespace DataBase;
namespace Builders {

	class DataReader
	{
	private:
		string file_name;
		ifstream* file;
		void read_stations(AutoBusData&);
		void read_trips(AutoBusData&);
		void read_interTrips(AutoBusData&);
		string line;

	public:
		DataReader(string file_name);
		AutoBusData& read_file();

	};

}


#endif // DATA_READER_H_INCLUDED
