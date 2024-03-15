#include "pch.h"
#include "Line.h"

using namespace DataBase;

Line::Line(string id, long numberOftrips)
{
	this->id = id;
	this->numberOfTrips = numberOftrips;
}

void Line::setId(string id)
{
	this->id = id;
}

string Line::getId() const
{
	return id;
}

bool Line::operator<(const Line& l) const
{
	return numberOfTrips < l.numberOfTrips;
}

bool Line::operator=(const Line& l) const
{
	return false;
}
