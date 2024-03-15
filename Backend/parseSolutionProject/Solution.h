#ifndef SOLUTION_H_INCLUDED
#define SOLUTION_H_INCLUDED

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
#include "Graphe.h"

using namespace std;
using namespace DataBase;

namespace Network
{
	class DynamicItinerary;
	class Solution
	{
	private:
		vector<DynamicItinerary*> itineraryies;
		int rank = 0;
		Graphe& graph;
		int numberOfTrips = 0;
		float cost = 0;
	public:

		Solution(Graphe& graph);
		int getRank();
		void setRank(int rank);
		double getCost();
		void AddTrip(Trip* trip,bool generation = false);
		void AddTripBestPlace(Trip* trip,bool generate = false,bool assert = false);
		void RemoveTrip(Trip* trip);
		int size();
		DynamicItinerary* operator[](int num);
		vector<DynamicItinerary*>& getItineraryies();
		Solution& Copy();
		void LinkClusters();
		void OptimizeHLPs();
		~Solution();

	};
}




#endif // SOLUTION_H_INCLUDED
