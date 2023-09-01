#pragma	once
#include "iostream"
#include "vector"
#include "map"
#include <fstream>
#include <string>
#include <algorithm>
#include "iterator"
#include <sys/types.h>
#include <dirent.h>
#include "serverClass.hpp"

using namespace std;

class server;

class configFile {
	private:
		vector<server>	_server;
		string			_full_file;
		char**			_env;
	public:
		configFile(const string file, char **env);
		~configFile(){};
		vector<server>&	getServers(){return (_server);};
		void	getServerContext(ifstream &in, string &line);
		void	check_braces(string file);
		void	lastCheck();
		void	print();
};

int				count_argument(string value, int count);
int				check_range(string nm, int min, int max);
int				check_number(string num);
int				clean_line(string &line, string& key, string& value);
void			check_file(const string file, string method);
int				check_host(string value);
void			get_allow_methodes(string& value, vector<string>& vec);
void			get_multiple_args(string& value, vector<string>& vec);
int				get_return(string& keystring, string& value, string& final, ifstream& in, string& line, string& red);
