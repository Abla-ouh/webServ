#pragma	once
#include "iostream"
#include "vector"
#include "map"
#include <fstream>
#include <string>
#include <algorithm>
#include "iterator"
#include "serverClass.hpp"

using namespace std;

class server;

class configFile {
	private:
		vector<server>	_server;
		string			_full_file;
	public:
		configFile(const string file);
		~configFile(){};
		void	getServerContext(ifstream &in, string &line);
		void	check_braces(string file);
		void	print();
};

int		count_argument(string value, int count);
int		check_range(string nm, int min, int max);
int		check_number(string num);
int		clean_line(string &line, string& key, string& value);
void	check_file(const string file, string method);
int		check_host(string value);
