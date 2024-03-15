#include "pch.h"
#include "Graphe.h"
#include "Configuration.h"
#include "DynamicItinerary.h"
#include<iostream>
#include<unordered_set>
#include<map>
#include<algorithm>
#include<set>
#include<vector>

using namespace DataBase;
using namespace Network;
using namespace std;
using namespace Config;

Graphe::Graphe(AutoBusData& tripsData) : tripsData(tripsData), interTrips(tripsData.getIntertrips())
{
	for (auto& it : tripsData.getStations())
	{
		AddStation(it);
	}

	depot = tripsData.getDepot();

	for (auto it : tripsData.GetTrips())
	{
		AddTrip(*it);
	}

	//ConstuctsHLPs();
}

/**
* fonction pour ajouter une station
*
*/
void Graphe::AddStation(string station, bool isDepot)
{
	if (isDepot)
		depot = station;
	//creating the pair[station,set of trips]
	auto pair = make_pair(station, new set<Trip*>());
	auto pair_inverted = make_pair(station, new set<Trip*>());
	auto pair3 = make_pair(station, new set<Connection*>());
	Graph.insert(pair);
	invertedGraph.insert(pair_inverted);
	Connections.insert({ station, new unordered_map<string,unordered_map<string,Connection*>*>() });
}

void Graphe::AddTrip(Trip& trip)
{
	int allowWD = stoi(Configuration::getInstance()["ALLOW_WD"]);
	//trip cost
	float cv = stof(Configuration::getInstance()["CV"]);
	//45 min
	int max_wait = stoi(Configuration::getInstance()["MAX_WAIT"]);
	//all the trips that lead to our trip's departure
	auto trips = invertedGraph.at(trip.getFirst());
	//connections possible to our trip departure
	auto connectionsArr = Connections.at(trip.getFirst());
	//trips from depot to other stations
	auto arcsDepo = interTrips.at(depot);

	for (auto t : *trips)
	{
		//if this current trip "t" comes before the given "trip"
		if (t->getDatearr() < trip.getDatedep())
		{
			//time difference between trip starting time and current t ending time
			auto timeDifference = trip.getDatedep() - t->getDatearr();
			//if it's lower than the 45 min
			if (timeDifference <= max_wait)
			{
				//create a connection
				auto connection = new Connection(t, &trip, timeDifference, NORMAL);
				AddConnection(connectionsArr, connection);
			}
			else if(allowWD)
			{
				auto arcBackToDepot = interTrips.at(t->getSecond())->find(depot);
				auto arcDepart = arcsDepo->find(trip.getFirst());
				//going and back from depot Arcs exists
				if (arcBackToDepot != interTrips.at(t->getSecond())->end() && arcDepart != arcsDepo->end())
				{
					long waitingTime = timeDifference - (arcBackToDepot->second->getDuration() + arcDepart->second->getDuration());
					if (waitingTime >= 0)
					{
						auto cost = cv * (arcBackToDepot->second->getDuration() + arcDepart->second->getDuration());
						auto connection = new Connection(t, &trip, waitingTime, WAIT_IN_DEPOT, cost);
						AddConnection(connectionsArr, connection);
					}
				}
			}
		}
	}

	auto trips2 = Graph.at(trip.getSecond());
	auto connectionsDep = Connections.at(trip.getSecond());

	for (auto tri : *trips2)
	{
		if (trip.getDatearr() < tri->getDatedep())
		{
			auto timeDifference = tri->getDatedep() - trip.getDatearr();
			//if it's lower than the 45 min
			if (timeDifference <= max_wait)
			{
				//create a connection
				auto connection = new Connection(&trip, tri, timeDifference, NORMAL);
				AddConnection(connectionsDep, connection);
			}
			else if (allowWD)
			{
				auto arcBackToDepot = interTrips.at(trip.getSecond())->find(depot);
				auto arcDepart = arcsDepo->find(tri->getFirst());
				//going and back from depot Arcs exists
				if (arcBackToDepot != interTrips.at(trip.getSecond())->end() && arcDepart != arcsDepo->end())
				{
					long waitingTime = timeDifference - (arcBackToDepot->second->getDuration() + arcDepart->second->getDuration());
					if (waitingTime >= 0)
					{
						auto cost = cv * (arcBackToDepot->second->getDuration() + arcDepart->second->getDuration());
						auto connection = new Connection(&trip, tri, waitingTime, WAIT_IN_DEPOT, cost);
						AddConnection(connectionsDep, connection);
					}
				}
			}
		}
	}

	//insert the trip with the departure as key
	Graph.at(trip.getFirst())->insert(&trip);
	//insert the trip with the arrival as key
	invertedGraph.at(trip.getSecond())->insert(&trip);
}

void Graphe::AddConnection(unordered_map<string, unordered_map<string, Connection*>*>* connectionsArr, Connection* connection)
{
	auto conns = connectionsArr->find(connection->getFirst()->getId());
	if (conns == connectionsArr->end())
	{
		auto pair = make_pair(connection->getFirst()->getId(), new unordered_map<string, Connection*>());
		pair.second->insert({ connection->getSecond()->getId() ,connection });
		connectionsArr->insert(pair);
	}
	else
	{
		conns->second->insert({ connection->getSecond()->getId(),connection });
	}
	if (connection->getType() == NORMAL) {
		connCostNORMAL += connection->getCost();
		connectionCountNORMAL++;
	}
}




int Graphe::NumberOfUncovered(string line, vector<Itinerary*>& it)
{
	set<Trip*>* trips;

	if (line == "")
	{
		trips = &tripsData.GetTrips();
	}
	else
	{
		trips = tripsData.getTripsByLines().at(line);
	}

	set<Trip*> myset;
	for (auto iti : it)
	{
		auto set = iti->getNormalTrips();
		for (auto itirat : set) {
			myset.insert(itirat);
		}
	}
	return trips->size() - myset.size();
}

AutoBusData& Network::Graphe::getTripsData()
{
	return tripsData;
}



unordered_map<string, unordered_map<string, unordered_map<string, Connection*>*>*>& Network::Graphe::getConnections()
{
	return Connections;
}

unordered_map<string, unordered_map<string, CountifiedPair<string>*>*>& Network::Graphe::getInterTrips()
{
	return interTrips;
}

string Network::Graphe::getDepot()
{
	return depot;
}

Connection* Network::Graphe::getNormalConnection(Trip* Dep, Trip* Arr)
{
	auto Cons = Connections.at(Dep->getSecond());
	auto conns = Cons->find(Dep->getId());
	if (conns != Cons->end()) {
		auto conn = conns->second->find(Arr->getId());
		if (conn != conns->second->end())
		{
			return new Connection(*conn->second);
		}
	}

	return NULL;
}


vector<DynamicItinerary*>& Graphe::get_itineraries_heuristique()
{
	auto Indiv = new Solution(*this);

	vector<Trip*> tripsInLine(tripsData.GetTrips().begin(),tripsData.GetTrips().end());

	sort(tripsInLine.begin(), tripsInLine.end(),[](Trip* t, Trip* t2) {
            if( t->getDatedep() < t2->getDatedep()) return true;
            if(t->getDatedep() > t2->getDatedep()) return false;
            return t->getDatearr() < t2->getDatearr();
		}
	);

	for (int i = 0; i < tripsInLine.size(); i++)
	{
		Indiv->AddTripBestPlace(tripsInLine[i],true);
	}
	Indiv->LinkClusters();
	Indiv->OptimizeHLPs();

	return Indiv->getItineraryies();
}


//Assert  : force it to make a connection (WD if possible)
Connection* Network::Graphe::constructHLP_or_WD(Trip* Dep, Trip* Arr, bool assert)
{
	auto inters = interTrips.at(Dep->getSecond());
	auto inter = inters->find(Arr->getFirst());
	//trip cost
	float cv = stof(Configuration::getInstance()["CV"]);
	//45 min
	int max_wait = stoi(Configuration::getInstance()["MAX_WAIT"]);
	long timeLeft;
	if (inter != inters->end())
	{

		auto hlp = inter->second;
		timeLeft = Arr->getDatedep() - (Dep->getDatearr() + hlp->getDuration());

		if (timeLeft > 0 && timeLeft <= max_wait)
		{
			return new Connection(Dep, Arr, hlp->getDuration(), HLP);
		}
	}

	if (!stoi(Configuration::getInstance()["ALLOW_WD"])) return NULL;

	if (!assert) return NULL;
	//the arc that takes us to the depot
	auto arcBackToDepot = interTrips.at(Dep->getSecond())->find(depot);
	//how much time left to the next trip we chose
	timeLeft = Arr->getDatedep() - Dep->getDatearr();
	//leaving the depot arc
	auto arcDepart = interTrips.at(depot)->find(Arr->getFirst());
	//going and back from depot Arcs exists
	if (arcBackToDepot != interTrips.at(Dep->getSecond())->end() && arcDepart != interTrips.at(depot)->end())
	{
		//how much time we will wait in depot
		int waitingTime = timeLeft - (arcBackToDepot->second->getDuration() + arcDepart->second->getDuration());
		if (waitingTime > 0)
		{
			auto cost = cv * (arcBackToDepot->second->getDuration() + arcDepart->second->getDuration());
			auto connection = new Connection(Dep, Arr, waitingTime, WAIT_IN_DEPOT, cost);
		}
	}
	return NULL;
}


float Graphe::getAverageConn()
{
	return connCostNORMAL / connectionCountNORMAL;
}
