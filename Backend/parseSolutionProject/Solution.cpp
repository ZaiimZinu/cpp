#include "pch.h"
#include "Solution.h"
#include "DynamicItinerary.h"
#include "Configuration.h"

using namespace std;
using namespace Network;
using namespace Config;

Network::Solution::Solution(Graphe& graph) :graph(graph)
{
}

Network::Solution::~Solution()
{
	for (auto it : itineraryies)
	{
		delete it;
	}
}

int Network::Solution::getRank()
{
	return rank;
}

void Network::Solution::setRank(int rank)
{
	this->rank = rank;
}

double Network::Solution::getCost()
{
	if (this->cost != 0) {
		return this->cost;
	}

	float cost = 0;

	for (auto ar : itineraryies) {
		cost += ar->getCost();
	}
	this->cost = cost;

	return cost;
}

void Network::Solution::AddTrip(Trip* trip, bool generation)
{
	numberOfTrips++;
	int maxTrips = stof(Configuration::getInstance()["MAX_TRIPS_CLUSTER"]);
	float maxDiff = graph.getAverageConn();
	maxDiff = maxDiff * stof(Configuration::getInstance()["MAX_COST_DIFF"]);
	//si on arive a l'inserer dans un itineraire deja existant
	for (auto it : itineraryies) {

		if (it->getNumberofTrips() >= maxTrips)continue;
		if (it->addTripInPlace(trip, generation, false, maxDiff)) {
			cost = 0;
			return;
		}
	}
	//si n on cree un nouveau itineraire pour cette trip
	DynamicItinerary* iti = new DynamicItinerary(graph);
	iti->addTripInPlace(trip, generation);
	//ajouter cet itineraire a la liste d'itineraires
	itineraryies.push_back(iti);
	cost = 0;
}

void Network::Solution::AddTripBestPlace(Trip* trip, bool generate, bool assert)
{
	numberOfTrips++;
	float maxDiff = graph.getAverageConn();
	maxDiff = maxDiff * stof(Configuration::getInstance()["MAX_COST_DIFF"]);
	int maxTrips = stof(Configuration::getInstance()["MAX_TRIPS_CLUSTER"]);
	DynamicItinerary* bestToAddTo = NULL;
	float costDiffrenceMin = 0;
	for (auto it : itineraryies)
	{
		if (it->getNumberofTrips() >= maxTrips)continue;

		auto costBef = it->getCost();
		if (it->addTripInPlace(trip, generate, assert, maxDiff))
		{
			auto costDiffrence = it->getCost() - costBef;
			if (bestToAddTo == NULL || costDiffrence <= costDiffrenceMin)
			{
				if (bestToAddTo)
				{
					bestToAddTo->RemoveTrip(trip);
				}

				costDiffrenceMin = costDiffrence;
				bestToAddTo = it;
			}
			else
			{
				it->RemoveTrip(trip);
			}
		}
	}
	//si nn creer un nouveu itineraire

	if (!bestToAddTo)
	{
		DynamicItinerary* iti = new DynamicItinerary(graph);
		iti->addTripInPlace(trip);
		itineraryies.push_back(iti);
	}

	cost = 0;
}

void Network::Solution::RemoveTrip(Trip* trip)
{
	for (auto i = itineraryies.begin(); i != itineraryies.end(); i++) {

		auto it = *i;
		auto ResultPair = it->RemoveTrip(trip);
		if (ResultPair.first)
		{
			if (ResultPair.second != NULL)
			{
				itineraryies.push_back(ResultPair.second);
			}
			if (it->isEmpthy())
			{
				delete it;
				itineraryies.erase(i);
			}
			numberOfTrips--;
			cost = 0;
			return;
		}
	}
}

int Network::Solution::size()
{
	return itineraryies.size();
}



DynamicItinerary* Network::Solution::operator[](int num)
{
	return itineraryies[num];
}


vector<DynamicItinerary*>& Network::Solution::getItineraryies()
{
	return itineraryies;
}

Solution& Network::Solution::Copy()
{
	Solution* newCrom = new Solution(graph);

	newCrom->numberOfTrips = numberOfTrips;
	for (auto it : itineraryies)
	{
		newCrom->itineraryies.push_back(&it->Copy(graph));
	}

	return *newCrom;
}

//try to link
void Network::Solution::LinkClusters()
{
	for (int it = 0; it < itineraryies.size() - 1; it++)
	{
		for (int it2 = it + 1; it2 < itineraryies.size(); it2++)
		{
			if (itineraryies[it]->tryToLink(itineraryies[it2]))
			{
				delete itineraryies[it2];
				itineraryies.erase(itineraryies.begin() + it2);
			}
		}
	}
}

void Network::Solution::OptimizeHLPs()
{
	int count = 0;
	for (int it = 0; it < itineraryies.size() - 1; it++)
	{
		auto& seq = itineraryies[it]->getSequence();
		auto current = itineraryies[it]->getInitial();
		auto finalAr = itineraryies[it]->getFinal();
		while (current != finalAr)
		{
			auto conn = seq.at(current).second;
			if (conn.getType() == HLP)
			{
				for (int it2 = it + 1; it2 < itineraryies.size(); it2++)
				{
					auto& seq2 = itineraryies[it2]->getSequence();
					auto current2 = itineraryies[it2]->getInitial();
					auto finalAr2 = itineraryies[it2]->getFinal();
					while (current2 != finalAr2)
					{
						auto conn2 = seq2.at(current2).second;
						if (conn2.getType() == HLP)
						{
							if (conn.getFirst()->getSecond() == conn2.getSecond()->getFirst() && conn.getSecond()->getFirst() == conn2.getFirst()->getSecond())
							{
								Connection* firstConn = graph.getNormalConnection(conn.getFirst(), conn2.getSecond());
								Connection* secConn = graph.getNormalConnection(conn2.getFirst(), conn.getSecond());
								if (firstConn && secConn)
								{
									if (firstConn->getCost() + secConn->getCost() < conn.getCost() + conn2.getCost())
									{
                                        count++;
										itineraryies[it]->Exchange(*itineraryies[it2], current, current2);
										conn = seq.at(current).second;
										finalAr = itineraryies[it]->getFinal();
										finalAr2 = itineraryies[it2]->getFinal();
                                    }
									delete firstConn;
									delete secConn;
								}
								else if (firstConn) {
									delete firstConn;
								}
								else if (secConn) {
									delete secConn;
								}
							}
						}
						current2 = seq2.at(current2).first;
					}
				}
			}
			current = seq.at(current).first;
		}
	}
	cout << count;
	cost = 0;
}






