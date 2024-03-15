#ifndef LINE_H_INCLUDED
#define LINE_H_INCLUDED

#pragma once
#include<iostream>
using namespace std;

namespace DataBase {

	class Line
	{
	private:
		string id;
	public:
		long numberOfTrips;
		Line(string id, long numberOftrips);
		void setId(string id);
		string getId() const;
		bool operator<(const Line& l) const;
		bool operator=(const Line& l) const;
	};

}

#endif // LINE_H_INCLUDED
