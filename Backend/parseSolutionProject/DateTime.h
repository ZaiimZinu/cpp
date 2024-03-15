#ifndef DATETIME_H_INCLUDED
#define DATETIME_H_INCLUDED

#pragma once
#include <iostream>
#include <fstream>
using namespace std;

namespace DataBase {

class DateTime
{
	private:
		int year;
		int month;
		int day;
		int hour;
		int min;

	public:
		DateTime();
		DateTime(string date);
		DateTime(long date);
		bool operator<(const DateTime& dt_) const;
		bool operator<(long& dt_) const;
		bool operator>(const DateTime& dt_) const;
		bool operator>=(const DateTime& dt_) const;
		bool operator==(const DateTime& dt_) const;
		long operator-(const DateTime& dt_) const;
		long operator+(const DateTime& dt_) const;
		long operator+(long dt_) const;
		long operator-(long dt_) const;
		long minutesSince2000_1_1_00_00() const;
		void Print_Date() const;
		void Print_Date(ofstream& stream) const;
        string strinDate() const;
};
}



#endif // DATETIME_H_INCLUDED
