#include "SolutionReader.h"
#include <sstream>
#include <algorithm>
#include "StringHandling.h"
#include "ExceptionHandling.h"
//#include "Configuration.h"
#include<unordered_set>
#include"itinerary.h"

using namespace Helpers;
Builders::SolutionReader::SolutionReader(string file_name, AutoBusData& data):data(data)
{
	this->file_name = file_name;
	this->file = new ifstream(file_name);
}

unordered_map<int,Itinerary*>& Builders::SolutionReader::read_file()
{
	unordered_set<string> visitedTrips;
	unordered_map<int, Itinerary*>* results = new unordered_map<int, Itinerary*>();
	auto& interTrips = data.getIntertrips();
	//get the current line
	while (getline(*file, line))
	{
        line = StringHandling::trim(line);
		if (line == "") {
			continue;
		}
		Itinerary* itinerary = new Itinerary();

		string clusterIdString = StringHandling::trim(StringHandling::get_string_between(line, 'r', ':'));
		int clusterId = stoi(clusterIdString);

		string TripsString = StringHandling::trim(StringHandling::get_string_between(line, ':', ';'));
		vector<char> splitBy = { ' ','\t' };
		auto TripsTokens = StringHandling::split_string_by(TripsString, splitBy);
		Trip* actualTrip=NULL;
		long wait = 0;

		for (int i=0;i<TripsTokens.size();i++)
		{
			auto token = TripsTokens[i];
			if (token[0] == 'T')
			{
				actualTrip = data.getMapTrip().at(token);

				/*if ( i >= 2 && (clusterIdString == "13")) {
                    //On doit s'assurer que deux trips dans un cluster sont possibles d'etre en sequence
                    Trip* previousTrip = data.getMapTrip().at(TripsTokens[i-2]);
                    long durationBetweenTrips = actualTrip->getDatedep() - previousTrip->getDatearr();
                    //long durationBetweenTrips = data.getMapTrip().at(TripsTokens[i-2])->getDatearr() - actualTrip->getDatedep();

                    if (durationBetweenTrips < 0) {
                        ExceptionHandling::getInstance().ThrowException("The given Trips "+TripsTokens[i-2]+" and "+token+" can not be successive in Cluster ["+clusterIdString + " ]");
                        continue;
                    }
				}*/

				if (itinerary->isEmpthy())
				{
					auto arcsDepo = interTrips.at(data.getDepot());
					//the road between the depot and the first station in our line
					auto arcDepart = arcsDepo->at(actualTrip->getFirst());
					itinerary->setInitial(*arcDepart, (*new DateTime(actualTrip->getDatedep() - arcDepart->getDuration())));
				}
				//if it's duplicated
				if (visitedTrips.find(actualTrip->getId()) != visitedTrips.end())
				{
					itinerary->addError( actualTrip->getId() ,actualTrip,wait);
                    ExceptionHandling::getInstance().ThrowException("The trip with Id: "+actualTrip->getId()+" is duplicated in Cluster ["+clusterIdString+"]");

				}
				else {
				itinerary->addTrip(*actualTrip,wait);
				visitedTrips.insert(actualTrip->getId());
				}
				wait = 0;
			}
			else if (token == "WS" || token == "ws")
			{
				//si on commence par un WS
				if (i==0)
				{
					ExceptionHandling::getInstance().ThrowException("Impossible to start with a wait in station in cluster, reference to cluster [" + clusterIdString + "]");
					continue;
				}
				if(     TripsTokens[i + 1] == "HLP" || TripsTokens[i + 1] == "hlp"
                    ||  TripsTokens[i + 1] == "WS" || TripsTokens[i + 1] == "ws"){

                    ExceptionHandling::getInstance().ThrowException("Impossible to have a sequence of "+TripsTokens[i]+" and "+TripsTokens[i + 1]+" in a given cluster: cluster[" + clusterIdString + "]");
                    cout<<endl;
				}
				wait = data.getMapTrip().at(TripsTokens[i+1])->getDatedep() - actualTrip->getDatearr();
			}
			else if (token == "HLP" || token == "hlp")
			{
				if (i == 0)
				{
					ExceptionHandling::getInstance().ThrowException("Impossible to start with an HLP in a cluster, reference to cluster [" + clusterIdString + "]");
					continue;
				}
				//if (TripsTokens[i - 1] == "HLP" || TripsTokens[i - 1] == "hlp")
				//{
				//	ExceptionHandling::getInstance().ThrowException("Impossible to have two sequential HLP in a row in cluster, reference to cluster [ " + clusterIdString + " ]");
				//}
				if(     TripsTokens[i + 1] == "HLP" || TripsTokens[i + 1] == "hlp"
                    ||  TripsTokens[i + 1] == "WS" || TripsTokens[i + 1] == "ws"){

                    ExceptionHandling::getInstance().ThrowException("Impossible to have a sequence of "+TripsTokens[i]+" and "+TripsTokens[i + 1]+" in a given cluster: cluster[ " + clusterIdString + " ]");
                     cout<<endl;
				}
                else{
                    auto TripAfterHLP = data.getMapTrip().at(TripsTokens[i + 1]);
                    auto hlp = interTrips.at(actualTrip->getSecond())->find(TripAfterHLP->getFirst());
                    if (hlp == interTrips.at(actualTrip->getSecond())->end())
                    {
                        itinerary->addError(INTERTRIP_NOT_EXISTING,NULL,0);
                    }
                    else
                    {
                        long timeLeft = TripAfterHLP->getDatedep() - (actualTrip->getDatearr() + hlp->second->getDuration());
                        wait = 0;
                        itinerary->addHLP(*hlp->second, timeLeft);
                    }
                }
			}
			else if (token == "WD" || token == "wd")
			{
				if (i == 0)
				{
					ExceptionHandling::getInstance().ThrowException("Starting with an HLP in cluster [ " + clusterIdString + " ]");
					continue;
				}
				if (TripsTokens[i - 1] == "WD" || TripsTokens[i - 1] == "WD")
				{
					ExceptionHandling::getInstance().ThrowException("Two HLP in a row in cluster [ " + clusterIdString + " ]");

				}
				auto TripAfterWD = data.getMapTrip().at(TripsTokens[i + 1]);
				auto arcBackToDepot = interTrips.at(actualTrip->getSecond())->at(data.getDepot());
				//how much time left to the next trip we chose
				long timeLeft = TripAfterWD->getDatedep() - actualTrip->getDatearr();
				//leaving the depot arc
				auto ArcOutOfDepot = interTrips.at(data.getDepot())->at(TripAfterWD->getFirst());
				//how much time we will wait in depot
				auto waitingTime = timeLeft - (arcBackToDepot->getDuration() + ArcOutOfDepot->getDuration());
				//returning to Depot and wait
				itinerary->addToDepot(*arcBackToDepot, waitingTime);
				//getting out of depot
				itinerary->addOutofDepot(*ArcOutOfDepot, (*new DateTime(TripAfterWD->getDatedep() - ArcOutOfDepot->getDuration())));
			}
			else {
				ExceptionHandling::getInstance().ThrowException("Unknown Token in cluster [ " + clusterIdString + " ]");
			}
		}
		if (!itinerary->isEmpthy())
		{
				auto arcBackToDepot = interTrips.at(actualTrip->getSecond())->at(data.getDepot());
				itinerary->addToDepot(*arcBackToDepot);

		}
		results->insert({ clusterId,itinerary });
	}
	return *results;
}
