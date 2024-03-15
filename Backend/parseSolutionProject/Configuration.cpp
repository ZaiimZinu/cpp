#include "pch.h"
#include "Configuration.h"
#include "StringHandling.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

using namespace Config;
using namespace Helpers;

Configuration* Configuration::instance = NULL;
string Configuration::ConfigLocation = "D:\\Canada\\parseSolutionProject\\data\\param.ini";

//"C:\\Users\\source\\repos\\AutobusNetwork\\GraphLib\\param.ini";


Config::Configuration::Configuration()
{
	read_config();
}

void Config::Configuration::read_config()
{
	ifstream stream(ConfigLocation);
	string line;
	while (getline(stream, line))
	{

		line = StringHandling::trim(line);
		if (line == "" || line[0] == '#') continue;
		if (!StringHandling::contains(line, '='))continue;

		line = StringHandling::remove_char(line, '\"');
		auto pair = StringHandling::split_string_by(line, '=');

		config.insert({ StringHandling::trim(pair[0]) , StringHandling::trim(pair[1]) });
	}
}

void Config::Configuration::initParamLoc(string loc)
{
	ConfigLocation = loc;
}

Configuration& Config::Configuration::getInstance()
{
	if (instance == NULL) instance = new Configuration();
	return *instance;
}

string Config::Configuration::operator[](string find) const
{
	return config.at(find);
}

