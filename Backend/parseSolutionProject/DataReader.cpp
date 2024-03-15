#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <algorithm>
#include "pch.h"
#include "DataReader.h"
#include "StringHandling.h"
#include "ExceptionHandling.h"
#include "Configuration.h"


using namespace std;
using namespace Helpers;
using namespace Builders;
using namespace Config;

DataReader::DataReader(string file_name)
{
	this->file_name = file_name;
	this->file = new ifstream(file_name);
}

AutoBusData& DataReader::read_file()
{
	auto data = new AutoBusData();
	//get the current line
	while (getline(*file, line))
	{
		//trim the line to remove the unnecessary spaces in the sides
		line = StringHandling::trim(StringHandling::remove_char(line, '\0'));
		//if this line contains "{"
		if (line.find('{') != string::npos)
		{
			//get the word that precedes the " ={ "
			line = StringHandling::trim(StringHandling::get_string_before(line, '='));

			if (line == "BusStations")
			{
				read_stations(*data);
			}
			else if (line == "BusTrips")
			{
				read_trips(*data);
			}
			else if (line == "InterTrips")
			{
				read_interTrips(*data);
				data->CheckInterTrips();
			}
			else {
				ExceptionHandling::getInstance().AssertThrowException("Unknown token when reading the data file (known ones are BusStations,BusTrips,InterTrips )");
			}

		}
	}
	file->close();
	return *data;
}

void DataReader::read_stations(AutoBusData& data)
{
	regex StationPattern("^\\w+[ ]+[01][ ]*;$");

	auto& stations = data.getStations();

	while (getline(*file, line))
	{
		line = StringHandling::trim(StringHandling::remove_char(line, '\0'));
		if (line.find('}') != string::npos) {
			return;
		}

		if (!regex_match(line, StationPattern))
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading a Station. the line doesn't match the Station format");
		}

		string stationCode = StringHandling::get_string_before(line, ' ');
		string isDepot = StringHandling::get_string_between(line, ' ', ';');

		if (stations.count(stationCode) != 0)
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading the data file found two instaces of the following station " + stationCode);
		}

		//checking if it's de depot or not
		if (isDepot == "0")
		{
			data.AddStation(stationCode);
		}
		else if (isDepot == "1")
		{
			if (data.getDepot() != "")
			{
				ExceptionHandling::getInstance().AssertThrowException("Error found when reading the data file found two depots");
			}

			data.AddStation(stationCode, true);
		}
		else
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading the type of a station (expected 1 or 0 but found " + isDepot + ")");
		}
	}
}

void DataReader::read_trips(AutoBusData& data)
{
	auto& stations = data.getStations();
	regex TripPattern("^\\w+[ ]+\\w+[ ]+\\w+[ ]+\\d{4}\\/\\d{2}\\/\\d{2}[ ]+\\d{2}:\\d{2}[ ]+\\w+[ ]+\\d{4}\\/\\d{2}\\/\\d{2}[ ]+\\d{2}:\\d{2}[ ]*;$");
	int read_line = stoi(Configuration::getInstance()["IGNORE_LINE"]);
	while (getline(*file, line))
	{
		//triming the line
		line = StringHandling::trim(StringHandling::remove_char(line, '\0'));
		if (line == "")continue;
		//if it's the end we stop
		if (line.find('}') != string::npos) return;

		if (!regex_match(line, TripPattern))
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading a trip. the line doesn't match the trip format");
		}

		vector<string> splitedTripLine = StringHandling::split_string_by(StringHandling::get_string_before(line, ';'), ' ');
		string lineNumber, idTrip, depStation, arrStation;
		DateTime* depDateTime;
		DateTime* arrDateTime;

		lineNumber = read_line ? "L_1" : splitedTripLine[0];
		idTrip = splitedTripLine[1];
		depStation = splitedTripLine[2];
		depDateTime = new DateTime(splitedTripLine[3] + " " + splitedTripLine[4]);
		arrDateTime = new DateTime(splitedTripLine[6] + " " + splitedTripLine[7]);
		arrStation = splitedTripLine[5];

		if (stations.count(depStation) == 0)
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading the trip " + idTrip + " unknown station " + depStation + " was found. ");
		}

		if (stations.count(arrStation) == 0)
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading the trip " + idTrip + " unknown station " + arrStation + " was found. ");
		}

		if (*depDateTime > *arrDateTime)
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading the trip " + idTrip + " the arrival time is earlier than the departure time ");
		}

		//constructing the trip object
		Trip* trip = new Trip(idTrip, depStation, arrStation, lineNumber, *depDateTime, *arrDateTime, "normal");
		//adding the trip to the main data
		data.add_trip(*trip);
	}
}


void DataReader::read_interTrips(AutoBusData& data)
{
	auto& stations = data.getStations();
	regex interTripPattern("^\\w+[ ]+\\w+[ ]+\\w+[ ]+\\d+[ ]*;$");
	while (getline(*file, line))
	{
		line = StringHandling::trim(StringHandling::remove_char(line, '\0'));
		if (line.find('}') != string::npos) return;

		if (!regex_match(line, interTripPattern))
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading a intertrip. the line doesn't match the intertrip format");
		}

		vector<string> splited = StringHandling::split_string_by(StringHandling::get_string_before(line, ';'), ' ');
		string id = splited[0];
		string departure = splited[1];
		string arrival = splited[2];
		long duration = stoi(splited[3]);

		if (stations.count(departure) == 0)
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading the interTrip " + id + " unknown station " + departure + " was found. ");
		}

		if (stations.count(arrival) == 0)
		{
			ExceptionHandling::getInstance().AssertThrowException("Error found when reading the trip " + id + " unknown station " + arrival + " was found. ");
		}

		CountifiedPair<string>* arc = new CountifiedPair<string>(id, departure, arrival, duration,NORMAL);
		data.AddInterTrip(*arc);
	}
}
