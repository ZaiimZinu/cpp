#ifndef GRAPHE_H_INCLUDED
#define GRAPHE_H_INCLUDED

#pragma once
#include<iostream>
#include<map>
#include<unordered_map>
#include<unordered_set>
#include<set>
#include<vector>
#include "Trip.h"
#include "itinerary.h"
#include "AutoBusData.h"
#include "Connection.h"

using namespace std;
using namespace DataBase;

namespace Network{

	class DynamicItinerary;
	class Graphe
	{
	private :
		unordered_map<string, set<Trip*>*> Graph;
		unordered_map<string, set<Trip*>*> invertedGraph;
		//station arrival of trip1 -> trip1 -> trip2 -> connection(trip1,trip2)
		unordered_map<string, unordered_map<string, unordered_map<string,Connection*>*>*> Connections;
		unordered_map<string, unordered_map<string, CountifiedPair<string>*>*>& interTrips;
		string depot;
		AutoBusData& tripsData;
		int connectionCountNORMAL = 0;
		float connCostNORMAL = 0;
	public:
		Graphe(AutoBusData& tripsData);
		void AddStation(string station,bool isDepot = false);
		void AddTrip(Trip& trip);
		int NumberOfUncovered(string line,vector<Itinerary*>& it);
		AutoBusData& getTripsData();
		unordered_map<string, unordered_map<string, unordered_map<string, Connection*>*>*>& getConnections();
		unordered_map<string, unordered_map<string, CountifiedPair<string>*>*>& getInterTrips();
		string getDepot();
		Connection* getNormalConnection(Trip* Dep, Trip* Arr);
		Connection* constructHLP_or_WD(Trip* Dep, Trip* Arr, bool assert=false);
		vector<DynamicItinerary*>& get_itineraries_heuristique();
		vector<DynamicItinerary*>& get_itineraries_GA();
		void AddConnection(unordered_map<string, unordered_map<string, Connection*>*>* connectionsArr, Connection* connection);
		float getAverageConn();


    };

}


#endif // GRAPHE_H_INCLUDED
