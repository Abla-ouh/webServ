#include "configFile.hpp"

int	count_argument(string value, int count)
{
	int c = 0;
	for (size_t i = 0; i < value.length(); i++)
	{
		value.erase(0, value.find_first_not_of(" 	"));
		if (value.substr(0, value.find_first_of(" 	")).length() > 0)
			c++;
		value.erase(0, value.find_first_of(" 	"));
		i = value.find_first_not_of(" 	") - 1;
	}
	if (count == -1 && c == 0)
		throw (unvalidArgument());
	if (count != -1 && c != count)
		throw (unvalidArgument());
	return (1);
}

int	check_range(string nm, int min, int max)
{
	if (atoi(nm.c_str()) < min || atoi(nm.c_str()) > max)
		throw (unvalidRange());
	return (1);
}

int	check_number(string num)
{
	for (size_t i = 0; i < num.length(); i++)
		if (!isdigit(num[i]))
			throw (unvalidDigit());
	return (1);
}

int clean_line(string &line, string& key, string& value)
{
	line.erase(0, line.find_first_not_of(" 	"));
	if (line[0] == '#' || line.length() < 1) // ! temporary condition because of end curly braces
		return (1);
	if (line[0] == '}')
		return (2);
	key = line.substr(0, line.find_first_of(" 	"));
	value = line.substr(line.find_first_of(" 	") > line.length() ? throw (unvalidArgument()) : line.find_first_of(" 	"));
	value.erase(value.find_first_of("#") > value.length() ? value.length() : value.find_first_of("#"));
	value.erase(0, value.find_first_not_of(" 	"));
	size_t i;
	for (i = value.length() - 1; isspace(value[i]); i--);
	value.erase(i + 1);
	if (key != "location" && value.find_first_of(";") > value.length())
		throw (noSemiColonEnd());
	if (key != "location" && value[value.length() - 1] != ';')
		throw (noSemiColonEnd());//throw something else
	value.erase(value.find_last_of(";") > value.length() ? value.length() : value.find_last_of(";"));
	return (0);
}

void check_file(const string file, string method)
{
	if (method == "write")
	{
		ofstream out(file.c_str());
		if (!out)
			throw (failedOpenFile());
		
	}
	else if (method == "read")
	{
		ifstream in(file.c_str());
		if (!in)
			throw (failedOpenFile());
	}
}

int	check_host(string value)
{
	int		countOctect = 0;
	string	tmp;
	value.erase(0, value.find_first_not_of(" 	"));
	value.erase(value.find_last_not_of(" 	") + 1);
	if (value.find_first_not_of(".0123456789") < value.length() || count(value.begin(), value.end(), '.') != 3)
		throw(unvalidHost());
	while (value.length() > 0)
	{
		tmp = value.substr(0, value.find_first_of("."));
		value.erase(0, !(value.find_first_of(".") + 1) ? value.length() : value.find_first_of(".") + 1);
		if (atoi(tmp.c_str()) > 255 || atoi(tmp.c_str()) < 0)
			throw(unvalidHost());
		countOctect++;
	}
	if (countOctect != 4)
		throw(unvalidHost());
	return (1);
}
