#include "pch.h"
#include "itinerary.h"
#include "Configuration.h"
#include "ExceptionHandling.h"
#include <iostream>
#include <fstream>
#include<string>



using namespace std;
using namespace Network;
using namespace DataBase;
using namespace Config;
using namespace Helpers;



//function to set the starting trip and the last one we did
void Itinerary::setInitial(CountifiedPair<string>& trip, DateTime& departureTime, bool isDepot)
{
	//keeping if it's out of the dpot or just a normal trip
	string type = isDepot ? OUT_OF_DEPOT : NORMAL;
	Trip* newTrip = new Trip(trip.getFirst(), trip.getSecond(), line, departureTime, (*new DateTime(departureTime + trip.getDuration())), type);
	//for the moment it's the initial
	this->firstTrip = newTrip;
	//and also the last one we just did
	this->lastTrip = newTrip;
	//calculate the new duration
	duration += trip.getDuration();
	if (isDepot)
		cost += trip.getDuration() * stof(Configuration::getInstance()["CV"]);
}

bool Itinerary::addTrip(Trip& trip, int wait)
{
	//updating the cost
	cost += wait * stof(Configuration::getInstance()["CA"]);

	auto pair = make_pair(lastTrip, &trip);
	sequence.insert(pair);
	numberOfTrips++;
	lastTrip = &trip;
	duration += trip.getDuration() + wait;
	durationOfWaiting += wait;
	return true;
}

void Itinerary::addHLP(CountifiedPair<string>& hlp, int wait)
{
	Trip* newTrip = new Trip(hlp.getFirst(), hlp.getSecond(), line, (*new DateTime(lastTrip->getDatearr())), (*new DateTime(lastTrip->getDatearr() + hlp.getDuration())), HLP);
	cost += wait * stof(Configuration::getInstance()["CA"]);
	cost += hlp.getDuration() * stof(Configuration::getInstance()["CV"]);

	auto pair = make_pair(lastTrip, newTrip);
	sequence.insert(pair);

	lastTrip = newTrip;

	duration += hlp.getDuration() + wait;
	numberOfHLP++;

	durationOfHLP += hlp.getDuration();

	durationOfWaiting += wait;
}

void Network::Itinerary::addError(string typeError, CountifiedPair<string>* trip, int wait)
{

	Trip* newTrip = new Trip(typeError, "", "", "", *new DateTime(1), *new DateTime(2), ERROR);
	auto pair = make_pair(lastTrip, newTrip);
	sequence.insert(pair);
	lastTrip = newTrip;
	if (typeError != INTERTRIP_NOT_EXISTING)
	{
		cost += wait * stof(Configuration::getInstance()["CA"]);
		numberOfTrips++;
		duration += trip->getDuration() + wait;
		durationOfWaiting += wait;

	}

}

void Itinerary::addOutofDepot(CountifiedPair<string>& trip, DateTime& timeDeparture)
{
	Trip* newTrip = new Trip(trip.getFirst(), trip.getSecond(), line, timeDeparture, (*new DateTime(timeDeparture + trip.getDuration())), OUT_OF_DEPOT);

	cost += trip.getDuration() * stof(Configuration::getInstance()["CV"]);

	auto pair = make_pair(lastTrip, newTrip);
	sequence.insert(pair);
	duration += trip.getDuration();
	lastTrip = newTrip;
}

void Itinerary::addToDepot(CountifiedPair<string>& trip, int wait)
{
	//creating the trip to depot from our last normal trip
	Trip* newTrip = new Trip(trip.getFirst(), trip.getSecond(), line, (*new DateTime(lastTrip->getDatearr())), (*new DateTime(lastTrip->getDatearr() + trip.getDuration())), INTO_DEPOT);
	//updating cost
	cost += trip.getDuration() * stof(Configuration::getInstance()["CV"]);
	//
	auto pair = make_pair(lastTrip, newTrip);
	sequence.insert(pair);
	//updating the final arc
	lastTrip = newTrip;

	duration += trip.getDuration() + wait;

	durationOfWaitingInDepot += wait;
}



set<Trip*> Network::Itinerary::getNormalTrips()
{
	set<Trip*> trips;
	auto init = firstTrip;
	if (init)
	{
		while (init)
		{
			if (init->getType() == NORMAL)trips.insert(init);

			if (init == lastTrip)
			{
				break;
			}
			else
			{
				init = sequence.at(init);
			}
		}
	}
	return trips;
}



void Itinerary::Print_v2(ofstream& file)
{
	auto currentArc = firstTrip;
	if (currentArc)
	{

		while (currentArc)
		{
			auto type = currentArc->getType();

			if (type == NORMAL)
			{
				file << currentArc->getId();
			}
			else if (type == INTO_DEPOT)
			{
				if (currentArc == lastTrip)
				{
					file << currentArc->getSecond();
				}
				else
				{
					auto last = currentArc;
					currentArc = sequence.at(currentArc);
					long wait = currentArc->getDatedep() - last->getDatearr();
					file << "WaitInDepot(" << wait << ")";
				}
			}
			else if (type == HLP)
			{
				file << "hlp(" << currentArc->getFirst() << "," << currentArc->getSecond() << "," << currentArc->getDuration() << " min)";
			}
			else if (type == OUT_OF_DEPOT)
			{
				file << currentArc->getFirst();
			}
			else {
				ExceptionHandling::getInstance().ThrowException("unkown arc type");
			}

			if (currentArc == lastTrip)
			{
				file << endl;
				break;
			}
			else
			{
				file << " ----> ";
				currentArc = sequence.at(currentArc);
			}
		}
		file << "[ total durration : " << duration << " min " << " Total cost :" << cost << " " << "Total of HLPs : " << numberOfHLP << " Durration of HLP : " << durationOfHLP << " min P: " << getPercentOfHLPDuration() << "% Waiting Time : " << durationOfWaiting << " min P:" << getPercentOfWaitingTime() << " % Waiting in depot : " << durationOfWaitingInDepot << " min P:" << getPercentOfWaitingInDepot() << " % ]" << endl;
	}
}

void Itinerary::Print_v3(ofstream& file)
{
	auto currentArc = firstTrip;
	auto lastArc = firstTrip;

	if (currentArc)
	{
		while (currentArc)
		{
			auto type = currentArc->getType();

			if (type == NORMAL)
			{
				if (lastArc != currentArc)
				{
					if (lastArc->getType() == NORMAL)
					{
						file << "WS ";
					}
				}

				file << currentArc->getId();
			}
			else if (type == INTO_DEPOT)
			{
				if (currentArc == lastTrip)
				{
					file << ";";
				}
				else
				{
					currentArc = sequence.at(currentArc);
					file << "WD";
				}
			}
			else if (type == HLP)
			{
				file << "HLP";
			}
			else if (type == ERROR)
			{
				file << "ERROR";
			}
			else if (type != OUT_OF_DEPOT)
			{
				ExceptionHandling::getInstance().ThrowException("unkown arc type");
			}

			if (currentArc == lastTrip)
			{
				file << endl;
				break;
			}
			else
			{
				file << " ";
				lastArc = currentArc;
				currentArc = sequence.at(currentArc);
			}
		}
	}
}



string Itinerary::getLine()
{
	return line;
}

long Itinerary::getDuration() const
{
	return duration;
}

float Itinerary::getCost() const
{
	return cost;
}

int Itinerary::getNumberOfHLP() const
{
	return numberOfHLP;
}

int Network::Itinerary::getNumberofTrips() const
{
	return numberOfTrips;
}

long Itinerary::getDurationOfHLP() const
{
	return durationOfHLP;
}

long Itinerary::getDurationOfWaiting() const
{
	return durationOfWaiting;
}

long Itinerary::getDurationOfWaitingInDepot() const
{
	return durationOfWaitingInDepot;
}

float Itinerary::getPercentOfHLPDuration() const
{
	return (durationOfHLP * 1.0 / duration) * 100;
}

float Itinerary::getPercentOfWaitingTime() const
{
	return (durationOfWaiting * 1.0 / duration) * 100;
}

float Itinerary::getPercentOfWaitingInDepot() const
{
	return (durationOfWaitingInDepot * 1.0 / duration) * 100;
}

bool Network::Itinerary::isEmpthy() const
{
	return firstTrip == NULL;
}

vector<string> Network::Itinerary::itineraryValid(int id) const
{
    unordered_set<string> visitedTrips;

    vector<string> errors;
    if (isEmpthy())
    {
        return errors;
    }
    bool realisable = true;
    auto current = firstTrip;
    auto lastTripInItenerary = firstTrip;

    current = sequence.at(current);


    while (current)
    {
        if (current->getType() == ERROR)
        {
            if (current->getId() == INTERTRIP_NOT_EXISTING)
            {
                string str = "Erreur dans cluster ["+to_string(id)+"]: Intertrip n'existe pas entre : [ " + lastTripInItenerary->getId() + " et  " + sequence.at(current)->getId() + " ] ";
                errors.push_back(str);
//                cout << "\tIntertrip n'existe pas entre : [ " << lastTripInItenerary->getId() << " time = ";  lastTripInItenerary->getDatearr().Print_Date(); cout << " ] et [ " << sequence.at(current)->getId() << " time = ";  sequence.at(current)->getDatedep().Print_Date(); cout << " ] " << endl;
                realisable = false;
                ExceptionHandling::getInstance().ThrowException(str);

            }
            else
            {
                //station double
                string str = "Erreur dans cluster ["+to_string(id)+"]: The Trip : "+current->getId()+" is duplicated !";
                errors.push_back(str);
//                cout  << current->getId() << endl;
                realisable = false;
                ExceptionHandling::getInstance().ThrowException(str);

            }
        }
        else {

            if (lastTripInItenerary->getSecond() != current->getFirst())
            {
                string str = "Erreur dans cluster ["+to_string(id)+"]: Station d'arrivee n'est pas la meme que station de depart entre : [ " + lastTripInItenerary->getId() + " et " + current->getId() + "]";
                errors.push_back(str);
                realisable = false;
                ExceptionHandling::getInstance().ThrowException(str);
            }
            else if (!lastTripInItenerary->Before(*current))
            {
                if (lastTripInItenerary->getType() == INTO_DEPOT)
                {
                    auto afterWD = sequence.at(current);
                    errors.push_back("Erreur dans cluster "+to_string(id)+" on ne peut pas faire le Trip  " + afterWD->getId() + " apres WD car on va arriver en retard " + current->getId());
//                    cout << "Erreur ! en peut pas faire le voyage  " << afterWD->getId() << " apres WD car on va arriver en retard " << current->getId() << endl;

                }
                else
                {
                    errors.push_back("Erreur dans cluster "+to_string(id)+" ordre des Trips est incorrect, verifier Trip avant " + current->getId());
//                    cout << "+++Erreur ! ordre des voyage incorrecte entre  " << lastTripInItenerary->getId() << " et " << current->getId() << endl;
                    ExceptionHandling::getInstance().ThrowException("Erreur dans cluster ["+to_string(id)+"] : Ordre des Trips est incorrect, verifier Trip avant " + current->getId());
                }
                realisable = false;
            }
            else if(lastTripInItenerary->getDatearr() - current->getDatedep() > stoi(Configuration::getInstance()["MAX_WAIT"]))
            {
                if (lastTripInItenerary->getType() != INTO_DEPOT)
                {
                    string str = "Erreur dans cluster ["+to_string(id)+"] : Temps d'attente entre  " + lastTripInItenerary->getId() + " et " + current->getId() + " depasse le max de temps d'attente";
                    errors.push_back(str);
//                    cout << "Erreur ! temps d'attente entre  " << lastTripInItenerary->getId() << " et " << current->getId() << " depasse le max de temps d'attente"<< endl;
                    realisable = false;
                    ExceptionHandling::getInstance().ThrowException(str);
                }
            }

        }


        if (current == lastTrip)	break;

        lastTripInItenerary = current;
        current = sequence.at(current);

    }

    return errors;
}

Trip* Network::Itinerary::getInitial()
{
	return firstTrip;
}

Trip* Network::Itinerary::getFinal()
{
	return lastTrip;
}





