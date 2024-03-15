#include "pch.h"
#include "GenetiqueHelper.h"
#include "DynamicItinerary.h"
#include "Random.h"
#include <algorithm>

using namespace Helpers;


unordered_set<Solution*>& Helpers::GenetiqueHelper::GeneratePopulation(Graphe& graph, int Number)
{
	//population de depart
	unordered_set<Solution*>* population = new unordered_set<Solution*>();

	for (int i = 0; i < Number; i++)
	{
		//creer une copie des trips existants
		unordered_set<Trip*> TempTrips(graph.getTripsData().GetTrips().begin(), graph.getTripsData().GetTrips().end());
		Solution* individu = new Solution(graph);
		//tant qu'on a pas utilisé tous les trips
		while (TempTrips.size() > 0)
		{
			int rand = getRandomInt(0, TempTrips.size() - 1);
			auto big = TempTrips.begin();
			//mettre l'iterator big dans la position rand
			advance(big, rand);
			//trip recoit le contenu de rand
			Trip* trip = *big;
			//retirer ce trip de TempTrip parcequ'il est utilisé
			TempTrips.erase(*big);
			//ajouter un gene a un des chromosomes
			individu->AddTrip(trip);
		}
		//ajouter cet individu a la population
		population->insert(individu);
	}
	return *population;
}



void Helpers::GenetiqueHelper::play(vector<Solution*>& players, unordered_set<Solution*>& population, float ratio)
{
	int cromIndice;
	Solution* indiv;

	//cas random > 0,8    --> on prend en vrac
	if (getRandomFloat(0, 1) > ratio) {
		cromIndice = getRandomInt(0, players.size() - 1);
		indiv = players[cromIndice];
	}
	//cas random < 0,8    --> le meilleur rank
	else {
		indiv = players[0];
		for (auto it : players)
		{
			if (it->getRank() < indiv->getRank())
			{
				indiv = it;
			}
		}
	}
	//delete pour les individus indesirables
	for (auto it : players)
	{
		if (it != indiv)
		{
			population.erase(population.find(it));
			delete it;
		}
	}
}


void Helpers::GenetiqueHelper::Tournament(unordered_set<Solution*>& Population, int TOURNAMENT_GROUP)
{
	vector<Solution*> TempPopulation(Population.begin(), Population.end());
	vector<Solution*>* Candidats = new vector<Solution*>();

	while (TempPopulation.size() > 0)
	{

		for (int i = 0; i < TOURNAMENT_GROUP && TempPopulation.size() > 0; i++) {
			int rand = getRandomInt(0, TempPopulation.size() - 1);
			Candidats->push_back(TempPopulation[rand]);
			TempPopulation.erase(TempPopulation.begin() + rand);
		}

		float ratio = stof(Configuration::getInstance()["TOURNAMENT_PERSENTAGE"]);
		play(*Candidats, Population, ratio);

		Candidats->clear();
	}
}





/**
*	cette fonction sert a ranker(classer) les individues d'une population donées
*	et retourn le meilleur qui est en position 0
*/
Solution* Helpers::GenetiqueHelper::RankPopulation(unordered_set<Solution*>& Population)
{
	vector<Solution*> copy(Population.begin(), Population.end());
	if (Population.size()) {
		sort(copy.begin(), copy.end(), [](Solution* t, Solution* t2) { return t->getCost() < t2->getCost(); });
	}

	for (size_t i = 0; i < copy.size(); i++)
	{
		(*Population.find(copy[i]))->setRank(i + 1);
	}
	return copy[0];
}



void Helpers::GenetiqueHelper::CrossOverAndMutation(unordered_set<Solution*>& Population,int PopulationNumber,float sicknessCureChance)
{
	vector<Solution*> OriginPopulation(Population.begin(), Population.end());
	// on remplis encore une fois la population de depart
	while (Population.size() < PopulationNumber)
	{
		vector<Solution*> CopyPopulation(OriginPopulation.begin(), OriginPopulation.end());

		while (CopyPopulation.size() > 1)
		{
			int rand = getRandomInt(0, CopyPopulation.size() - 1);
			Solution* indiv1 = CopyPopulation[rand];
			CopyPopulation.erase(CopyPopulation.begin() + rand);

			rand = getRandomInt(0, CopyPopulation.size() - 1);
			Solution* indiv2 = CopyPopulation[rand];
			CopyPopulation.erase(CopyPopulation.begin() + rand);

            auto vec = CrossOver(indiv1, indiv2);

			//mutation
			for (int i = 0; i < vec.size(); i++)
			{
				vec[i]->LinkClusters();
				Population.insert(vec[i]);
			}
        }
    }
}

vector<Solution*> Helpers::GenetiqueHelper::CrossOver(Solution* individu_1, Solution* individu_2)
{
	//deux Individues aleatoire sont passés en arguments
	vector<Solution*> fils;
	//on cree des copies
	Solution& indiv1 = individu_1->Copy();
	Solution& indiv2 = individu_2->Copy();

	int rand = getRandomInt(0, indiv1.size() - 1);
	int rand2 = getRandomInt(0, indiv2.size() - 1);
	//on choisie deux chromosomes(clusters) aleatoirement
	DynamicItinerary* it1 = indiv1[rand];
	DynamicItinerary* it2 = indiv2[rand2];

	//ensemble de trips non ordonnees
	auto trips1 = it1->getTripsRandom();
	auto trips2 = it2->getTripsRandom();

	//enlever de indiv2 les trips qui existent  dans trips1(chez indiv1)
	for (auto trip : trips1)
		indiv2.RemoveTrip(trip);
	//ajouter a indiv2 les trips que nous venons d'enlever (seront inséré dans un nouveau ordre)
	for (auto trip : trips1)
		indiv2.AddTripBestPlace(trip);// we can do here an exact algorithme to add the trip if the algorithm is not good enough

	//enlever de indiv1 les trips qui existent  dans trips2(chez indiv2)
	for (auto trip : trips2)
		indiv1.RemoveTrip(trip);
	//ajouter a indiv1 les trips que nous venons d'enlever (seront inséré dans un nouveau ordre)
	for (auto trip : trips2)
		indiv1.AddTripBestPlace(trip); //we can do here an exact algorithme to add the trip if the algorithm is not good enough

	fils.push_back(&indiv1);
	fils.push_back(&indiv2);
	return fils;
}
