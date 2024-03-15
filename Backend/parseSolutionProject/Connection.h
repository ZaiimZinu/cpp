#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#pragma once
#include<iostream>
#include"CountifiedPair.h"
#include "Trip.h"

using namespace std;
using namespace DataBase;

namespace Network
{
	class Connection : public CountifiedPair<Trip*>
	{
		double cost = 0;
	public:
		Connection(Trip* dep, Trip* arr, long duration, string type, double cost = 0);
		Connection(const Connection&C);
		Connection(string id, Trip* dep, Trip* arr, long duration, string type, double cost = 0);
		double getCost();

		~Connection() {
		}
	};
}




#endif // CONNECTION_H_INCLUDED
