#ifndef TRIP_H_INCLUDED
#define TRIP_H_INCLUDED

#pragma once
#include <iostream>
#include <fstream>
#include "DateTime.h"
#include "CountifiedPair.h"
using namespace std;

namespace DataBase {

	class Trip : public CountifiedPair<string>
	{
	private:

		DateTime& date_dep;
		DateTime& date_arr;
		string line;
	public:

		Trip(string id, string dep, string arr, string line, DateTime& date_dep, DateTime& date_arr, string type);
		Trip(string dep, string arr, string line, DateTime& date_dep, DateTime& date_arr, string type);

		DateTime& getDatedep() const;

		DateTime& getDatearr() const;

		string getLine() const;


		bool Before(const Trip& nextTrip) const
		{
			return nextTrip.getDatedep() >= this->getDatearr() ;
		}
		bool canThisBeBefore(const Trip& tr) const
		{
			return tr.date_dep >= date_arr;
		}

		~Trip() {
			cout << "";
		}
	};

}


#endif // TRIP_H_INCLUDED
