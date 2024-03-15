#ifndef ITINERARY_H_INCLUDED
#define ITINERARY_H_INCLUDED

#pragma once
#include<iostream>
#include<map>
#include<unordered_map>
#include<unordered_set>
#include<set>
#include <fstream>
#include "Trip.h"
#include<vector>

#define NORMAL "normal"
#define HLP "hlp"
#define OUT_OF_DEPOT "Out of Depot"
#define INTO_DEPOT "Into Depot"
#define ERROR "Error"
#define INTERTRIP_NOT_EXISTING "Intertrip does not exist"

using namespace std;
using namespace DataBase;


namespace Network {

	class Itinerary
	{
	protected:
		unordered_map<Trip*, Trip*> sequence;
		Trip* firstTrip = NULL;
		Trip* lastTrip = NULL;
		long duration = 0;
		float cost = 1000;
		int numberOfHLP = 0;
		int numberOfTrips = 0;
		long durationOfHLP = 0;
		long durationOfWaiting = 0;
		long durationOfWaitingInDepot = 0;
		string line;
	public:
		void setInitial(CountifiedPair<string>& init, DateTime& departureTime, bool isDepot = true); // initial trip
		bool addTrip(Trip& trip, int wait = 0); // wait before starting trip (duration of the connection)
		void addHLP(CountifiedPair<string>& hlp, int wait = 0);// wait after doing the HLP
		void addError(string typeError, CountifiedPair<string>* trip, int wait = 0);//Add an inconvenient trip
		void addOutofDepot(CountifiedPair<string>& hlp, DateTime& timeDeparture);
		void addToDepot(CountifiedPair<string>& hlp, int wait = 0); // wait in depot
		set<Trip*> getNormalTrips();
		string getLine();
		void virtual Print_v2(ofstream& file);
		void virtual Print_v3(ofstream& file);
		long virtual getDuration() const;
		float virtual getCost() const;
		int getNumberOfHLP() const;
		int getNumberofTrips() const;
		long getDurationOfHLP() const;
		long getDurationOfWaiting() const;
		long getDurationOfWaitingInDepot() const;
		float getPercentOfHLPDuration() const;
		float getPercentOfWaitingTime() const;
		float getPercentOfWaitingInDepot() const;
		bool isEmpthy() const;
        vector<string> itineraryValid(int cluster_id) const;
		Trip* getInitial();
		Trip* getFinal();
        ~Itinerary(){}

	};

}

#endif // ITINERARY_H_INCLUDED
