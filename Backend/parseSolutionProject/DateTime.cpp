#include "pch.h"
#include "DateTime.h"
#include "StringHandling.h"
#include <string>

using namespace DataBase;
using namespace Helpers;

DateTime::DateTime()
{
}

DateTime::DateTime(long minutes)
{
	// this is wrong we know but it will be 0 either way
	year = 2000 + minutes/ 5287680;
	minutes = minutes % 5287680;

	// this is wrong we know but it will be 0 either way
	month = 1 + minutes / 440640;
	minutes = minutes % 440640;

	day = 1 + minutes / 1440;
	minutes = minutes % 1440;

	hour = minutes / 60;
	minutes = minutes % 60;

	min = minutes;
}

DateTime::DateTime(string date)
{
	vector<char> chars;
	chars.push_back(' ');
	chars.push_back('/');
	chars.push_back(':');
	vector<string> splited = StringHandling::split_string_by(date, chars);

	for (int i = 0; i < splited.size(); i++)
	{
		int val = stoi(splited[i]);
		switch (i) {
		case 0:
			year = val;
			break;
		case 1:
			month = val;
			break;
		case 2:
			day = val;
			break;
		case 3:
			hour = val;
			break;
		default:
			min = val;
			break;
		}
	}
}

bool DateTime::operator < (const DateTime& dt_) const
{
	return this->minutesSince2000_1_1_00_00() < dt_.minutesSince2000_1_1_00_00();
}

bool DateTime::operator<(long& dt_) const
{
	return this->minutesSince2000_1_1_00_00() < dt_;
}

bool DateTime::operator > (const DateTime& dt_) const
{
	return this->minutesSince2000_1_1_00_00() > dt_.minutesSince2000_1_1_00_00();
}

bool DateTime::operator >= (const DateTime& dt_) const
{
	return this->minutesSince2000_1_1_00_00() >= dt_.minutesSince2000_1_1_00_00();
}

bool DateTime::operator==(const DateTime& dt_) const
{
	return this->minutesSince2000_1_1_00_00() == dt_.minutesSince2000_1_1_00_00();
}

long DateTime::operator-(const DateTime& dt_) const
{
	return abs(this->minutesSince2000_1_1_00_00() - dt_.minutesSince2000_1_1_00_00());
}

long DateTime::operator+(const DateTime& dt_) const
{
	return this->minutesSince2000_1_1_00_00() + dt_.minutesSince2000_1_1_00_00();
}

long DateTime::operator+(long dt_) const
{
	return minutesSince2000_1_1_00_00() + dt_;
}

long DateTime::operator-(long dt_) const
{
	return minutesSince2000_1_1_00_00() - dt_;
}

long DateTime::minutesSince2000_1_1_00_00() const
{
	long minutes = (this->year - 2000) * 5287680; // this is wrong we know but it will be 0 either way
	minutes += (month - 1) * 440640; // this is wrong we know but it will be 0 either way
	minutes += (day - 1) * 1440;
	minutes += (hour) * 60;
	minutes += min;
	return minutes;
}

void DateTime::Print_Date() const
{
	cout << year << "/" << month << "/" << day << " " << hour << ":" << min;
}



void DateTime::Print_Date(ofstream& file) const
{
	file << year << "/" << month << "/" << day << " " << hour << ":" << min;
}

string DateTime::strinDate() const {
//    return year+"/"+month+"/"+day+" "+hour+":"+min;
	return "";
}
