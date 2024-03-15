#ifndef GENETIQUEHELPER_H_INCLUDED
#define GENETIQUEHELPER_H_INCLUDED

#pragma once
#include<vector>
#include<unordered_set>
#include"Solution.h"
#include "Configuration.h"

using namespace Network;
using namespace std;
using namespace Config;

namespace Helpers
{
	class GenetiqueHelper
	{
	public:
		static unordered_set<Solution*>& GeneratePopulation(Graphe& graph, int PopulationNumber);
		static Solution* RankPopulation(unordered_set<Solution*>& Population);
		static void Tournament(unordered_set<Solution*>& Population, int TOURNAMENT_GROUP);
		static void CrossOverAndMutation(unordered_set<Solution*>& Population, int PopulationNumber, float sicknessCureChance);
		static vector<Solution*> CrossOver(Solution* crom1,Solution*crom2);
		static void play(vector<Solution*>& players, unordered_set<Solution*>& population, float ratio);

	};

}

#endif // GENETIQUEHELPER_H_INCLUDED
