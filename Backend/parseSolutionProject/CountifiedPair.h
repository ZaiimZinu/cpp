#ifndef COUNTIFIEDPAIR_H_INCLUDED
#define COUNTIFIEDPAIR_H_INCLUDED

#pragma once
#include <iostream>
#include <string>
#include <fstream>

#define NORMAL "normal"
#define HLP "hlp"
#define OUT_OF_DEPOT "Out of Depot"
#define INTO_DEPOT "Into Depot"
#define WAIT_IN_DEPOT "wait in depot"

using namespace std;

namespace DataBase {

	template <class T>
	class CountifiedPair
	{
	protected:
		static int count;
		string id;
		T first;
		T second;
		long duration = 0;
		string type;
	public:
		CountifiedPair(T dep, T arr, long duration,string type);
		CountifiedPair(string id, T dep, T arr, long duration,string type);

		string getId() const;
		void setId(string id);

		T getFirst() const;
		void setFirst(T first);

		T getSecond() const;
		void setSecond(T second);

		long getDuration() const;
		void setDuration(long duration);

		void Print();
		void Print(ofstream& file);

		string getType()
		{
			return type;
		}

		CountifiedPair(const CountifiedPair<T>& C)
		{
			id = C.id;
			first = C.first;
			second = C.second;
			duration = C.duration;
			type = C.type;
		}

		bool operator<(const CountifiedPair&) const;
		bool operator==(const CountifiedPair&) const;
		bool operator>(const CountifiedPair& ) const;

		~CountifiedPair()
		{
		}
	};

	template<class T>
	int CountifiedPair<T>::count = 0;

	template<class T>
	CountifiedPair<T>::CountifiedPair(string id, T dep, T arr, long duration,string type)
	{
		this->id = id;
		this->first = dep;
		this->second = arr;
		this->duration = duration;
		this->type = type;
	}

	template<class T>
	CountifiedPair<T>::CountifiedPair(T dep, T arr, long duration,string type)
	{
		id = to_string(count++);
		this->first = dep;
		this->second = arr;
		this->duration = duration;
		this->type = type;
	}

	template<class T>
	string CountifiedPair<T>::getId() const
	{
		return id;
	}

	template<class T>
	void CountifiedPair<T>::setId(string id)
	{
		this->id = id;
	}

	template<class T>
	T CountifiedPair<T>::getFirst() const
	{
		return first;
	}

	template<class T>
	void CountifiedPair<T>::setFirst(T first)
	{
		this->first = first;
	}

	template<class T>
	T CountifiedPair<T>::getSecond() const
	{
		return second;
	}

	template<class T>
	void CountifiedPair<T>::setSecond(T second)
	{
		this->second = second;
	}

	template<class T>
	long CountifiedPair<T>::getDuration() const
	{
		return duration;
	}

	template<class T>
	void CountifiedPair<T>::setDuration(long duration)
	{
		this->duration = duration;
	}

	template<class T>
	bool CountifiedPair<T>::operator<(const CountifiedPair<T>& x) const
	{
		return duration < x.duration;
	}

	template<class T>
	bool CountifiedPair<T>::operator>(const CountifiedPair<T>& x) const
	{
		return duration > x.duration;
	}

	template<class T>
	bool CountifiedPair<T>::operator==(const CountifiedPair<T>& x) const
	{
		return id == x.id;
	}
	//printing CountifiedPair details
	template<class T>
	void CountifiedPair<T>::Print()
	{
		cout << "ID: "<< id << " Departure: " << first << " Arrival: " << second << " Duration: " << duration << " min";
	}

	template<class T>
	void CountifiedPair<T>::Print(ofstream& file)
	{
		file << "ID: "<< id << " Departure: " << first << " Arrival: " << second << " Duration: " << duration << " min";
	}
}




#endif // COUNTIFIEDPAIR_H_INCLUDED
