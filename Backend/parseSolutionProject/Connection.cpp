#include "pch.h"
#include "Connection.h"
#include "Configuration.h"

using namespace Network;
using namespace Config;

Connection::Connection(string id, Trip* dep, Trip* arr ,long duration , string type, double cost) : CountifiedPair(id, dep, arr, duration, type)
{
	float ca = stof(Configuration::getInstance()["CA"]);
	float cv = stof(Configuration::getInstance()["CV"]);

	if (type == NORMAL)
	{
		this->cost = ca * duration;
	}
	else if (type == HLP)
	{
		this->cost = cv * duration + ca * (arr->getDatedep() - dep->getDatearr() - duration);
	}
	else if (type == WAIT_IN_DEPOT)
	{
		this->cost = cost;
	}
}


Connection::Connection(Trip* dep, Trip* arr, long duration, string type,double cost) : CountifiedPair(dep, arr, duration, type)
{
	float ca = stof(Configuration::getInstance()["CA"]);
	float cv = stof(Configuration::getInstance()["CV"]);
	if (type == NORMAL)
	{
		this->cost = ca * duration;
	}
	else if(type == HLP)
	{
		this->cost = cv * duration + ca * (arr->getDatedep() - dep->getDatearr() - duration);
	}
	else if(type == WAIT_IN_DEPOT)
	{
		this->cost = cost;
	}
}

Network::Connection::Connection(const Connection& C) :CountifiedPair<Trip*>(C)
{
	if (C.cost == 0) {
		cout << "";
	}
	cost = C.cost;
}

double Connection::getCost()
{
	return cost;
}

