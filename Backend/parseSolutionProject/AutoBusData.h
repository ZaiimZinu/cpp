#ifndef AUTOBUSDATA_H_INCLUDED
#define AUTOBUSDATA_H_INCLUDED


#pragma once
#include<iostream>
#include<map>
#include<unordered_map>
#include<unordered_set>
#include<set>
#include"Trip.h"
#include"Line.h"
using namespace std;

namespace DataBase {

	class AutoBusData
	{
	private:
		//map with the pair [number of line] and [associated trips]
		unordered_map<string, set<Trip*>*> tripsByLines;
		unordered_map<string, unordered_map<string, CountifiedPair<string>*>*> interTrips;
		set<Trip*> trips;
		multiset<Trip> Stats_trips;
		unordered_map<string, Trip*> map_trips;
		unordered_set<string> stations;
		long totalminutes = 0;
		string depot;
	public:
		//tripsByLines getter
		unordered_map<string, set<Trip*>*>& getTripsByLines();
		//trips getter
		set<Trip*>& GetTrips();
		unordered_map<string, unordered_map<string, CountifiedPair<string>*>*>& getIntertrips();
		unordered_set<string>& getStations();
		string getDepot();

		void AddStation(string station, bool isDepot = false);
		void AddInterTrip(CountifiedPair<string>& trip);

		void print_numberOfTrips_by_ligne();
		void print_duration_of_trips();
		void print_duration_of_trips(string file);
		void print_total_trips();
		void print_min_duration_trip();
		void print_max_duration_trip();
		void print_average_duration();
		void add_trip(Trip& trip);
		void CheckInterTrips();

		unordered_map<string, Trip*>& getMapTrip();

		~AutoBusData()
		{
			for (auto it : interTrips) {
				for (auto au : *it.second) {
					delete au.second;
				}
				delete it.second;
			}

			for (auto it : tripsByLines) {
				for (auto au : *it.second) {
					delete au;
				}
				delete it.second;
			}
		}
	};

}


#endif // AUTOBUSDATA_H_INCLUDED
