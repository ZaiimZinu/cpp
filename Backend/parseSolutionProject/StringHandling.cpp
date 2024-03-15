#include "pch.h"
#include "StringHandling.h"
#include <iostream>
#include<algorithm>

using namespace std;
using namespace Helpers;
const std::string WHITESPACE = " \n\r\t\f\vþÿ";

bool Helpers::StringHandling::contains(const string& str, char find)
{
	return str.find(find) != string::npos;
}

string StringHandling::remove_char(const string& str, char rem)
{
	string my_str = str;
	my_str.erase(remove(my_str.begin(), my_str.end(), rem), my_str.end());
	return my_str;
}

string StringHandling::ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

string StringHandling::rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string StringHandling::trim(const std::string& s) {
	return rtrim(ltrim(s));
}

string StringHandling::get_string_before(string const& s, char before)
{
	string::size_type pos = s.find(before);

	if (pos != std::string::npos)
	{
		return s.substr(0, pos);
	}
	else
	{
		return s;
	}
}
string StringHandling::get_string_after(string const& s, char after)
{
	string::size_type pos = s.find(after);

	if (pos != std::string::npos)
	{
		return s.substr(pos+1,s.length()-1);
	}
	else
	{
		return s;
	}
}

string StringHandling::get_string_between(string const& s,char after , char before )
{
	return get_string_before(get_string_after(s, after), before);
}

vector<string> StringHandling::split_string_by(string const& s, vector<char>& split)
{
	vector<string> strings;
	string buffer = "";
	for (int i = 0; i < s.length(); i++)
	{
		if (find(split.begin(), split.end(), s[i]) == split.end())
		{
			buffer += s[i];
		}
		else
		{
			if (buffer != "") {
				strings.push_back(buffer);
				buffer = "";
			}
		}
	}
	if(buffer!= "")strings.push_back(buffer);

	return strings;
}

vector<string> StringHandling::split_string_by(string const& s, char split)
{
	vector<char> vec;
	vec.push_back(split);
	return split_string_by(s, vec);
}
