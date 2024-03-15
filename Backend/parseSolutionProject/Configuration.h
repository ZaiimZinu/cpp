#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#pragma once
#include<iostream>
#include<unordered_map>

using namespace std;
namespace Config
{
	class Configuration
	{
	private:
		static string ConfigLocation;
		static Configuration* instance;
		Configuration();
		Configuration(const Configuration&);
		void read_config();

		unordered_map<string, string> config;
	public:
		static void initParamLoc(string loc);
		static Configuration& getInstance();
		string operator[](string) const;
	};
}


#endif // CONFIGURATION_H_INCLUDED
