#ifndef DYNAMICITINERARY_H_INCLUDED
#define DYNAMICITINERARY_H_INCLUDED

#pragma once
#include "itinerary.h"
#include "Graphe.h"
#include "Solution.h"

namespace Network {
	class DynamicItinerary : public Itinerary
	{
	protected:
		Graphe& graph;
		unordered_map<Trip*, pair<Trip*, Connection>> seq;
	public:
		DynamicItinerary(Graphe& graph);
		bool addTripInPlace(Trip* trip, bool generation = false,bool assert = false,float maxCostDiff = 1000);
		pair<bool,DynamicItinerary*> RemoveTrip(Trip*trip);
		float getCost() const override;
		long getDuration() const override;
		vector<Trip*> getTripsRandom();
		DynamicItinerary& Copy(Graphe& C)const;
		void Print_v3(ofstream& file) override;
		void Print_v2(ofstream& file) override;
		DynamicItinerary& cutFrom(Trip* trip);
		void addStatistques(Connection& conn);
		void removeStatistques(Connection& conn);
		bool CanBeBefore(DynamicItinerary* itineray);
		bool tryToLink(DynamicItinerary* itineray);
        int getTimeLeft(Connection &conn);
		void Exchange(DynamicItinerary& toChange, Trip*current,Trip*toChangeTr);
		unordered_map<Trip*, pair<Trip*, Connection>>& getSequence();
        ~DynamicItinerary(){}
    };
}


#endif // DYNAMICITINERARY_H_INCLUDED
