#ifndef EXCEPTIONHANDLING_H_INCLUDED
#define EXCEPTIONHANDLING_H_INCLUDED

#pragma once
#include <iostream>
#include <fstream>
using namespace std;

namespace Helpers {

	class ExceptionHandling
	{
	private:
		static ExceptionHandling* instance;
		ExceptionHandling();
		ExceptionHandling(const ExceptionHandling&);
		string fileLocation;
		bool throwEx = false;
	public:
		static ExceptionHandling& getInstance();
		void ThrowException(string exception);
		void AssertThrowException(string exception);
	};
}

#endif // EXCEPTIONHANDLING_H_INCLUDED
