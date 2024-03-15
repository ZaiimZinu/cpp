#ifndef STRINGHANDLING_H_INCLUDED
#define STRINGHANDLING_H_INCLUDED

#pragma once
#include <string>
#include <vector>
using namespace std;

namespace Helpers {

class StringHandling
{
public :
	static bool contains(const string&,char);
	static string remove_char(const string& str, char rem);
	static string ltrim(const std::string& s);
	static string rtrim(const std::string& s);
	static string trim(const std::string& s);
	static string get_string_before(string const& s, char before);
	static string get_string_after(string const& s, char before);
	static string get_string_between(string const& s, char after, char before);
	static vector<string> split_string_by(string const& s, vector<char>& split);
	static vector<string> split_string_by(string const& s, char split);
};

}


#endif // STRINGHANDLING_H_INCLUDED
