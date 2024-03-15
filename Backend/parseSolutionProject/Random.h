#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#pragma once
#include<iostream>
#include<set>
#include<time.h>
using namespace std;

namespace Helpers
{
	int getRandomInt(int min, int max)
	{
		srand(time(0));
		int randNum = rand() % (max - min + 1) + min;
		return randNum;
	}

	float getRandomFloat(float min, float max)
	{
		srand(time(0));
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = max - min;
		float r = random * diff;
		return min + r;
	}
}

#endif // RANDOM_H_INCLUDED
