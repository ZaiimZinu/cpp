#include "pch.h"
#include "Trip.h"
#include "ExceptionHandling.h"

using namespace DataBase;
using namespace Helpers;


Trip::Trip(string id, string dep, string arr, string line, DateTime& dateDep, DateTime& dateArr, string type) : CountifiedPair(id,dep,arr,dateArr - dateDep,type), date_dep(dateDep), date_arr(dateArr)
{
    if (dateDep > dateArr)
    {
        ExceptionHandling::getInstance().ThrowException("Trip "+ id + " has a date of departure later than the date of arrival. Please check your data");
    }

    this->line = line;
}

Trip::Trip(string dep, string arr, string line, DateTime& dateDep, DateTime& dateArr, string type) : CountifiedPair(dep,arr,dateArr - dateDep,type), date_dep(dateDep), date_arr(dateArr)
{
    if (dateDep > dateArr)
    {
        ExceptionHandling::getInstance().ThrowException("Trip " + id + " has a date of deparature later than the date of arrival. Please check your data");
    }

    this->line = line;
}


DateTime& Trip::getDatedep() const
{
    return date_dep;
}



DateTime& Trip::getDatearr() const
{
    return date_arr;
}



string Trip::getLine() const
{
    return line;
}







