#include "pch.h"
#include "ExceptionHandling.h"
#include "Configuration.h"
#include <iostream>

using namespace Config;
using namespace Helpers;

ExceptionHandling* ExceptionHandling::instance = NULL;

Helpers::ExceptionHandling::ExceptionHandling()
{
	Configuration& config = Configuration::getInstance();

	if (config["EXCEPTION_MODE"] == "log")
	{
		fileLocation = config["EXCEPTION_LOCATION"] + "Exception_Execution_" + __DATE__ + ".log";
	}
	else if (config["EXCEPTION_MODE"] == "throw")
	{
		throwEx = true;
	}
	else
	{
		throw runtime_error("Error in Config: unkown EXCEPTION_MODE ( known ones are log or throw )");
	}
}


ExceptionHandling& ExceptionHandling::getInstance()
{
	if (instance == NULL) instance = new ExceptionHandling();
	return *instance;
}

void Helpers::ExceptionHandling::ThrowException(string exception)
{
	if (throwEx)
	{
		AssertThrowException(exception);
	}
	else
	{
		ofstream file(fileLocation, ofstream::app);
		file << exception << endl;
		file.close();
	}
}

void Helpers::ExceptionHandling::AssertThrowException(string excep)
{
	cout << excep << endl;
	throw runtime_error(excep);
	exit(1);
}
