#include "pch.h"
#include "DynamicItinerary.h"
#include "Configuration.h"
#include <algorithm>
#include <random>
#include "ExceptionHandling.h"

using namespace Config;
using namespace Network;
using namespace Helpers;

Network::DynamicItinerary::DynamicItinerary(Graphe& crom) : graph(crom)
{

}

bool Network::DynamicItinerary::addTripInPlace(Trip* trip,bool generation,bool assert , float maxCost)
{
	//Ajouter premier trip
	if (isEmpthy()) {
		firstTrip = trip;
		lastTrip = trip;
		numberOfTrips++;
        duration+= trip->getDuration();
        return true;
	}

	auto init = firstTrip;
	if (trip->canThisBeBefore(*init))
	{
		auto Cons = graph.getConnections().at(trip->getSecond());
		auto conns = Cons->find(trip->getId());
		if (conns != Cons->end()) {
			auto conn = conns->second->find(init->getId());

			if (conn != conns->second->end())
			{
				if (!assert) {
					if (maxCost <= conn->second->getCost())
					{
						return false;
					}
				}

				seq.insert({ trip , {init , *conn->second} });
				addStatistques(*conn->second);
				firstTrip = trip;
				numberOfTrips++;
                duration+= trip->getDuration();

                return true;
			}
		}

		//construct HLP if possible
		auto hlp = graph.constructHLP_or_WD(trip, init,assert);

		if (hlp == NULL)
		{
			return false;
		}

		if (!assert) {
			if (maxCost <= hlp->getCost())
			{
				delete hlp;
				return false;
			}
		}


		seq.insert({ trip , {init , *hlp} });
		addStatistques(*hlp);
		delete hlp;
		numberOfTrips++;
		firstTrip = trip;
        duration+= trip->getDuration();
        return true;
	}

	if (lastTrip->canThisBeBefore(*trip))
	{
		auto Cons = graph.getConnections().at(lastTrip->getSecond());
		auto conns = Cons->find(lastTrip->getId());
		if (conns != Cons->end()) {
			auto conn = conns->second->find(trip->getId());

			if (conn != conns->second->end())
			{

				if (!assert) {
					if (maxCost <= conn->second->getCost())
					{
						return false;
					}
				}

				seq.insert({ lastTrip , {trip ,*conn->second} });
				addStatistques(*conn->second);
				numberOfTrips++;
				lastTrip = trip;
                duration+= trip->getDuration();

                return true;
			}
		}
		//construct HLP if possible

		auto hlp = graph.constructHLP_or_WD(lastTrip, trip,assert);

		if (hlp == NULL)
		{
			return false;
		}

		seq.insert({ lastTrip , {trip , *hlp} });
		addStatistques(*hlp);
		delete hlp;

		numberOfTrips++;
		lastTrip = trip;
        duration+= trip->getDuration();

        return true;
	}

	if (generation) return false;

	if (lastTrip == firstTrip)return false;

	auto last = init;
	auto current = init;

	//test in the middle
	do
	{
		last = current;
		auto pair = seq.at(current);
		current = pair.first;

		if (trip->canThisBeBefore(*current))
		{
			if (!last->canThisBeBefore(*trip))
			{
				return false;
			}

			auto Cons = graph.getConnections().at(trip->getSecond());
			auto connsFromTrip = Cons->find(trip->getId());
			Connection* ConnFromTripToCurrent = NULL;
			bool deleteConnFromTripToCurrent = false;

			if (connsFromTrip != Cons->end())
			{
				auto connToCurrent = connsFromTrip->second->find(current->getId());

				if (connToCurrent != connsFromTrip->second->end())
				{
					ConnFromTripToCurrent = connToCurrent->second;
				}
			}

			if (ConnFromTripToCurrent == NULL)//construct HLP if possible
			{
				auto hlp = graph.constructHLP_or_WD(trip, current,assert);

				if (hlp == NULL)
				{
					return false;
				}

				ConnFromTripToCurrent = hlp;
				deleteConnFromTripToCurrent = true;
			}

			Cons = graph.getConnections().at(last->getSecond());
			auto connsFromLast = graph.getConnections().at(last->getSecond())->find(last->getId());
			Connection* ConnFromLastToTrip = NULL;
			bool deleteConnFromLastToTrip = false;
			if (connsFromLast != Cons->end()) {
				auto connToTrip = connsFromLast->second->find(trip->getId());
				if (connToTrip != connsFromLast->second->end())
				{
					ConnFromLastToTrip = connToTrip->second;
				}

			}
			//construct HLP if possible
			if (ConnFromLastToTrip == NULL)
			{
				auto hlp = graph.constructHLP_or_WD(last, trip,assert);

				if (hlp == NULL)
				{
					return false;
				}

				ConnFromLastToTrip = hlp;
				deleteConnFromLastToTrip = true;
			}

			if (!assert) {
				if (maxCost <= ConnFromTripToCurrent->getCost() + ConnFromLastToTrip->getCost() - pair.second.getCost())
				{
					if (deleteConnFromLastToTrip) {
						delete ConnFromLastToTrip;
					}

					if (deleteConnFromTripToCurrent) {
						delete ConnFromTripToCurrent;
					}

					return false;
				}
			}

			auto& pair = seq.at(last);

			removeStatistques(pair.second);
			seq.erase(last);

			seq.insert({ trip , {current , *ConnFromTripToCurrent} });
			seq.insert({ last, {trip , *ConnFromLastToTrip} });

			addStatistques(*ConnFromLastToTrip);
			addStatistques(*ConnFromTripToCurrent);

			if (deleteConnFromLastToTrip) {
				delete ConnFromLastToTrip;
			}

			if (deleteConnFromTripToCurrent) {
				delete ConnFromTripToCurrent;
			}

			numberOfTrips++;
            duration+= trip->getDuration();
			return true;
		}
	} while (current != lastTrip);

	return false;
}

int Network::DynamicItinerary::getTimeLeft(Connection& conn)
{
    if (conn.getType()==HLP){
        DateTime v( conn.getFirst()->getDatearr() + conn.getDuration());
        return conn.getSecond()->getDatedep() - v;
    }else if(conn.getType() == WAIT_IN_DEPOT){
        int dur = 0;
        auto trip = graph.getInterTrips().at(conn.getFirst()->getSecond())->at(graph.getDepot());
        dur += trip->getDuration();

        trip = graph.getInterTrips().at(graph.getDepot())->at(conn.getSecond()->getFirst());
        dur += trip->getDuration();
        return dur;
    }
    return 0;
}

void Network::DynamicItinerary::Exchange(DynamicItinerary& toExchangeWith, Trip* current, Trip* toChangeTr)
{
    DynamicItinerary& itinerary = this->cutFrom(current);
    DynamicItinerary& itinerary2 = toExchangeWith.cutFrom(toChangeTr);

	this->tryToLink(&itinerary2);
	toExchangeWith.tryToLink(&itinerary);

    delete &itinerary;
    delete &itinerary2;
}

unordered_map<Trip*, pair<Trip*, Connection>>& Network::DynamicItinerary::getSequence()
{
	return seq;
}


void Network::DynamicItinerary::addStatistques(Connection& conn)
{
    int after = getTimeLeft(conn);
	if (conn.getType() == HLP)
	{
		numberOfHLP++;
		durationOfHLP += conn.getDuration();
        durationOfWaiting += after;
	}
	else if (conn.getType() == WAIT_IN_DEPOT) {

		durationOfWaitingInDepot += conn.getDuration();
	}
	else if(conn.getType() == NORMAL)
	{
		durationOfWaiting += conn.getDuration();
	}
    duration += (conn.getDuration()+after);
	cost += conn.getCost();
}
void Network::DynamicItinerary::removeStatistques(Connection& conn)
{
    int after = getTimeLeft(conn);

    if (conn.getType() == HLP)
	{
		numberOfHLP--;
		durationOfHLP -= conn.getDuration();
        durationOfWaiting -= after;
	}
	else if (conn.getType() == WAIT_IN_DEPOT) {

		durationOfWaitingInDepot -= conn.getDuration() ;
	}
	else if(conn.getType() == NORMAL)
	{
		durationOfWaiting -= conn.getDuration();
	}
	duration -= (conn.getDuration()+after);
	cost -= conn.getCost();
}

pair<bool, DynamicItinerary*> Network::DynamicItinerary::RemoveTrip(Trip* trip)
{
	if (isEmpthy())
	{
		return { false, NULL };
	}

	if (trip->canThisBeBefore(*firstTrip) || lastTrip->canThisBeBefore(*trip))
	{
		return { false, NULL };
	}

	if (trip == firstTrip)
	{
		if (firstTrip == lastTrip)
		{
			firstTrip = NULL;
			lastTrip = NULL;
		}
		else
		{
			auto pair = seq.at(firstTrip);
			removeStatistques(pair.second);
			seq.erase(firstTrip);
			firstTrip = pair.first;
		}
        duration-= trip->getDuration();
        numberOfTrips--;
        return { true, NULL };
	}

	if (firstTrip == lastTrip)
	{
		return { false,NULL };
	}

	auto last = firstTrip;
	auto current = firstTrip;

	//test in the middle
	do
	{
		last = current;
		current = seq.at(current).first;

		if (trip == current)
		{

			auto pairLast = seq.at(last);
			if (current == lastTrip)
			{
				removeStatistques(pairLast.second);
				seq.erase(last);
				lastTrip = last;
			}
			else
			{
				auto pairAfter = seq.at(current);
				auto CONN = graph.getConnections().at(last->getSecond());
				auto conns = CONN->find(last->getId());
				if (conns != CONN->end())
				{
					auto conn = conns->second->find(pairAfter.first->getId());
					if (conn != conns->second->end())
					{
						removeStatistques(pairLast.second);
						seq.erase(last);
						seq.insert({ last , {pairAfter.first , *conn->second} });
						addStatistques(*conn->second);
						removeStatistques(pairAfter.second);
						seq.erase(current);
                        duration-= trip->getDuration();
                        numberOfTrips--;
                        return { true,NULL };
					}
				}
				//generate HLP or WD if possible
				//if it's not possible, we split the itinerary
				auto hlp = graph.constructHLP_or_WD(last, pairAfter.first, true);
				removeStatistques(pairLast.second);
				if (hlp == NULL)
				{
					auto& newItin = this->cutFrom(last);
                    duration-= trip->getDuration();
                    numberOfTrips--;
                    return { true,&newItin };
				}
				seq.erase(last);
				removeStatistques(pairAfter.second);
				seq.erase(current);
				seq.insert({ last , {pairAfter.first , *hlp} });
				addStatistques(*hlp);
				delete hlp;
			}
            duration-= trip->getDuration();
            numberOfTrips--;
            return { true, NULL };
		}

	} while (current != lastTrip);

	return { false, NULL };
}

float Network::DynamicItinerary::getCost() const
{
	float cost = this->cost;
	auto trip = graph.getInterTrips().at(graph.getDepot())->at(firstTrip->getFirst());
	cost += trip->getDuration() * stof(Configuration::getInstance()["CV"]);

	trip = graph.getInterTrips().at(lastTrip->getSecond())->at(graph.getDepot());
	cost += trip->getDuration() * stof(Configuration::getInstance()["CV"]);

	return cost;
}

vector<Trip*> Network::DynamicItinerary::getTripsRandom()
{
	if (isEmpthy())
	{
		return vector<Trip*>();
	}

	vector<Trip*>vec;
	auto current = firstTrip;
	vec.push_back(current);

	while (current != lastTrip)
	{
		current = seq.at(current).first;
		vec.push_back(current);
	}

	auto rng = std::default_random_engine{};
	std::shuffle(vec.begin(), vec.end(), rng);

	return vec;
}

DynamicItinerary& Network::DynamicItinerary::Copy(Graphe& C) const
{
	DynamicItinerary* newIt = new DynamicItinerary(C);

	newIt->cost = cost;
	newIt->duration = duration;
	newIt->durationOfHLP = durationOfHLP;
	newIt->durationOfWaiting = durationOfWaiting;
	newIt->durationOfWaitingInDepot = durationOfWaitingInDepot;

	newIt->numberOfTrips = numberOfTrips;

	newIt->firstTrip = firstTrip;
	newIt->lastTrip = lastTrip;

	newIt->seq = seq;

	return *newIt;
}


void DynamicItinerary::Print_v2(ofstream& file)
{
	auto currentArc = firstTrip;
	if (currentArc)
	{
		while (currentArc)
		{
			auto pair = seq.at(currentArc);
			auto type = pair.second.getType();
			file << currentArc->getId();

			if (type == WAIT_IN_DEPOT)
			{
				file << " ----> ";
				file << "WaitInDepot(" << pair.second.getDuration() << ")";
			}
			else if (type == HLP)
			{
				file << " ----> ";
				file << "hlp(" << pair.second.getFirst() << "," << pair.second.getSecond() << "," << pair.second.getDuration() << " min)";
			}
			else if (type != NORMAL) {
				ExceptionHandling::getInstance().ThrowException("unkown Connection type");
			}

			if (currentArc == lastTrip)
			{
				file << " ----> ";
				file << currentArc->getId();
				break;
			}
			else
			{
				file << " ----> ";
				currentArc = pair.first;
			}
		}
		file << "[ total durration : " << duration << " min " << " Total cost :" << cost << " " << "Total of HLPs : " << numberOfHLP << " Durration of HLP : " << durationOfHLP << " min P: " << getPercentOfHLPDuration() << "% Waiting Time : " << durationOfWaiting << " min P:" << getPercentOfWaitingTime() << " % Waiting in depot : " << durationOfWaitingInDepot << " min P:" << getPercentOfWaitingInDepot() << " % ]" << endl;

	}
}

DynamicItinerary& Network::DynamicItinerary::cutFrom(Trip* trip)
{
	DynamicItinerary* dynamItin = new DynamicItinerary(graph);
	auto lastTr = trip;
	auto current = seq.at(lastTr).first;
	auto& conn = seq.at(lastTr).second;
	removeStatistques(conn);
	seq.erase(lastTr);
	while (current)
	{
		dynamItin->addTripInPlace(current);
		numberOfTrips--;
		duration -= current->getDuration();

		if (current == lastTrip) break;
		auto last = current;
		auto pair = seq.at(current);
		current = pair.first;
		removeStatistques(pair.second);
		seq.erase(last);
	}
	lastTrip = lastTr;
	return *dynamItin;
}

void DynamicItinerary::Print_v3(ofstream& file)
{
	auto currentArc = firstTrip;
	auto lastArc = firstTrip;

	if (currentArc)
	{
		while (currentArc)
		{
			file << currentArc->getId();
			if (currentArc == lastTrip)
			{
				break;
			}
			auto pair = seq.at(currentArc);
			auto type = pair.second.getType();

			if (type == WAIT_IN_DEPOT)
			{
				file << " WD ";
			}
			else if (type == HLP)
			{
				file << " HLP ";
			}
			else if (type == NORMAL)
			{
				file << " WS ";
			}
			else
			{
				ExceptionHandling::getInstance().ThrowException("unkown Connection type");
			}
			currentArc = pair.first;
		}
	}
	file << " ;"<<endl;
}

bool DynamicItinerary::CanBeBefore(DynamicItinerary* itineray)
{
	return this->lastTrip->canThisBeBefore(*(itineray->firstTrip));
}


bool DynamicItinerary::tryToLink(DynamicItinerary* itineray)
{
	if (!CanBeBefore(itineray)) {
		return false;
    }
    if (!addTripInPlace(itineray->firstTrip))
	{
        return false;
    }
	auto current = itineray->firstTrip;

	while (current != itineray->lastTrip)
	{
		current = itineray->seq.at(current).first;
		addTripInPlace(current);
	}

	return true;
}


long DynamicItinerary::getDuration() const {
    long dur = duration;
    auto trip = graph.getInterTrips().at(graph.getDepot())->at(firstTrip->getFirst());
    dur += trip->getDuration();

    trip = graph.getInterTrips().at(lastTrip->getSecond())->at(graph.getDepot());
    dur += trip->getDuration();
    return dur;
}
