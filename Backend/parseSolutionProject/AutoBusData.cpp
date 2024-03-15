#include "pch.h"
#include "AutoBusData.h"
#include <fstream>
#include <iostream>
#include "ExceptionHandling.h"

using namespace DataBase;
using namespace Helpers;


unordered_map<string, set<Trip*>*>& AutoBusData::getTripsByLines()
{
	return tripsByLines;
}

set<Trip*>& AutoBusData::GetTrips()
{
	return trips;
}

unordered_map<string, unordered_map<string, CountifiedPair<string>*>*>& AutoBusData::getIntertrips()
{
	return interTrips;
}

unordered_set<string>& AutoBusData::getStations()
{
	return stations;
}

string AutoBusData::getDepot()
{
	return depot;
}

void AutoBusData::AddStation(string station, bool isDepot)
{
	if (isDepot)
		depot = station;
	stations.insert(station);
	//creating the pair[station,set of inter trips]
	interTrips.insert({ station, new unordered_map<string,CountifiedPair<string>*>() });
}

void AutoBusData::AddInterTrip(CountifiedPair<string>& trip)
{
	interTrips.at(trip.getFirst())->insert({ trip.getSecond(),&trip});
}


void AutoBusData::print_numberOfTrips_by_ligne()
{
	multiset<Line> lines;

	for (auto& a : tripsByLines)
	{
		Line line(a.first, a.second->size());
		lines.insert(line);
	}

	cout << "Number of trips by ligne : " << endl;
	for (auto& line : lines)
	{
		cout << "line : " << line.getId() << " number of trips : " << line.numberOfTrips << endl;
	}
}

void AutoBusData::print_duration_of_trips()
{
	cout << "Duration of trips : " << endl;

	for (auto trip : Stats_trips)
	{
		cout << "trip : " << trip.getId() << " duration : " << trip.getDuration() << " min" << endl;
	}
}

void AutoBusData::print_duration_of_trips(string out)
{
	ofstream file(out);

	file << "Trip, Duration" << endl;

	for (auto& trip : Stats_trips)
	{
		file << trip.getId() << "," << trip.getDuration() << " min" << endl;
	}
}

void AutoBusData::print_total_trips()
{
	cout << "Total of trips is :   "<< this->trips.size() << " trips" << endl;
}

void AutoBusData::print_min_duration_trip()
{
	cout << "the trip with the minimum duration :  " << Stats_trips.begin()->getId() << " duration = "<< Stats_trips.begin()->getDuration() << " min" << endl;
}

void AutoBusData::print_max_duration_trip()
{
	//geting the last trip
	auto end = Stats_trips.end();
	end--;

	cout << "the trip with the maximum duration :  " << end->getId() << " duration = " << end->getDuration() << " min" << endl;
}

void AutoBusData::print_average_duration()
{
	double average = (totalminutes * 1.0) / trips.size();

	cout << "average duration of trips :  " << average << " min" << endl;
}

/**
* this function allows to add a trip to the map:tripsByLines
* - if the line of this trip is not allready in the map as a key
*   we create the pair <line number,set of trips> then we put it in the map tripsByLines
* - if the line already exists as key in tripsByLines we add this trip to the associated set
*/
void AutoBusData::add_trip(Trip& trip)
{
	//testing if both stations are known
	if (stations.find(trip.getFirst()) == stations.end())
	{
		ExceptionHandling::getInstance().ThrowException("Unknow station "+ trip.getFirst() + " in Trip "+ trip.getId()+ ". Please check your data");
		//in case the exception mode is log we correct the data
		stations.insert(trip.getFirst());
		interTrips.insert({ trip.getFirst(), new unordered_map<string,CountifiedPair<string>*>() });
	}

	if (stations.find(trip.getSecond()) == stations.end())
	{
		ExceptionHandling::getInstance().ThrowException("Unknow station "+ trip.getSecond() + " in Trip "+ trip.getId() + ". Please check your data");
		//in case the exception mode is log we correct the data
		stations.insert(trip.getSecond());
		interTrips.insert({ trip.getSecond() , new unordered_map<string,CountifiedPair<string>*>() });
	}

	//adding the duration of the trip to the total minutes of all trips
	totalminutes += trip.getDuration();
	//insertion of the trip
	trips.insert(&trip);
	Stats_trips.insert(trip);

	set<Trip*>* newtrips = {};
	//if this trip is not in tripsByLines
	if (tripsByLines.find(trip.getLine()) == tripsByLines.end()) {
		//creating the new set of trips
		newtrips = new set<Trip*>();
		//creating the pair <line number, set of trips>
		auto pair = make_pair(trip.getLine(), newtrips);
		//inserting the new pair
		tripsByLines.insert(pair);
	}
	else //existing line number
	{
		//adding the trip to the associated set
		newtrips = tripsByLines.at(trip.getLine());
	}
	//both cases at the end we add it to the pointer of the set
	newtrips->insert(&trip);

	map_trips.insert({ trip.getId(),&trip });
}

void DataBase::AutoBusData::CheckInterTrips()
{
	for (auto it : interTrips)
	{
		if (it.second->size() != stations.size() - 1)
		{
			//ExceptionHandling::getInstance().ThrowException("Some inter trips are missing for the station " + it.first +". Please check your data");
		}
	}
}

unordered_map<string, Trip*>& DataBase::AutoBusData::getMapTrip()
{
	return this->map_trips;
}
