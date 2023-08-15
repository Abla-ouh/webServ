#include "configFile.hpp"

void	configFile::getServerContext(ifstream &in, string &line)
{
	string	key = "", value = "";
	server	serv;
	string	tmp;

	while (getline(in, line))
	{
		tmp = line;
		_full_file += line + "\n";
		int returnValue = clean_line(line, key, value);
		if (returnValue == 1)
			continue;
		else if (returnValue == 2)
			break;
		if (line.find_first_of("{}") < line.length() && key != "location")
			throw(unvalidDirective());
		if (key == "listen")
			serv.setPort(count_argument(value, 1) && check_number(value) && check_range(value, 0, 65536) ? value : "");
		else if (key == "host")
			serv.setHost(count_argument(value, 1) && check_host(value) ? value : "");
		else if (key == "server_name")
			get_multiple_args(value, serv.getServerName());
		else if (key == "allow_methods")
			get_allow_methodes(value, serv.getAllowMethodes());
		else if (key == "client_max_body_size")
			serv.setclient_max_body_size(count_argument(value, 1) && check_number(value) ? value : "");
		else if (key == "root")
			serv.setRoot(count_argument(value, 1) ? value : "");
		else if (key == "index")
			get_multiple_args(value, serv.getIndex());
		else if (key == "error_page")
		{
			count_argument(value, 2);
			string key = value.substr(0, value.find_first_of(" 	"));
			check_number(key);
			value.erase(0, value.find_first_of(" 	"));
			value.erase(0, value.find_first_not_of(" 	"));
			string val = value.substr(0, value.find_first_of(" 	"));
			serv.setErrorPage(key, val);
		}
		else if (key == "autoindex")
			serv.setAutoIndex(count_argument(value, 1) && (value == "on" || value == "off") ? value : throw(unvalidDirective()));
		else if (key == "location" && value.find_first_of("{") < value.length())
			serv.getLocationContext(in, value.find_first_not_of(" 	{") < value.length() ? line : throw (unvalidDirective()));
		else
			throw (unvalidDirective());
	}
	_server.push_back(serv);
}

// ? left
// TODO : check host range => fixed √
// TODO : check curly braces => fixed for now √
// TODO : location => get data done √
// TODO : remove semi colon in end of line

configFile::configFile(const string file) : _full_file("")
{
	check_file(file, "read");
	check_braces(file);
	ifstream	in(file.c_str());
	string		line;

	for (; getline(in, line);)
	{
		line.erase(0, line.find_first_not_of(" 	"));
		if (line[0] == '#' || line.length() < 1)
			continue;
		_full_file += line + "\n";
		line.erase(remove(line.begin(), line.end(), ' '), line.end());
		line.erase(remove(line.begin(), line.end(), '	'), line.end());
		if (line == "server{")
			getServerContext(in, line);
		else
			throw (unvalidDirective());
	}
}

void	configFile::print()
{
	for (size_t i = 0; i < _server.size(); i++)
	{
		cout << YELLOW "Server " << i + 1 << " -----------------------" WHITE << "\n";
		cout << GREEN "Port's: \n" WHITE;
		vector<string> &vec = this->_server[i].getPort();
		for (size_t i = 0; i < vec.size(); i++)
			cout << "|" << vec[i] << "|" << "\n";

		cout << GREEN "Host's: \n" WHITE;
		cout << this->_server[i].getHost() << "\n";

		cout << GREEN "Client Max Body Size: \n" WHITE;
		cout << this->_server[i].getclient_max_body_size() << "\n";

		cout << GREEN "Root: \n" WHITE;
		cout << this->_server[i].getRoot() << "\n";

		cout << GREEN "Index's: \n" WHITE;
		vector<string> &vec2 = this->_server[i].getIndex();
		for (size_t i = 0; i < vec2.size(); i++)
			cout << "|" << vec2[i] << "|" << "\n";

		cout << GREEN "Server Name's: \n" WHITE;
		vector<string> &vec3 = this->_server[i].getServerName();
		for (size_t i = 0; i < vec3.size(); i++)
			cout << "|" << vec3[i] << "|" << "\n";

		cout << GREEN "Allowed Methods: \n" WHITE;
		vector<string> &vec4 = this->_server[i].getAllowMethodes();
		for (size_t i = 0; i < vec4.size(); i++)
			cout << "|" << vec4[i] << "|" << "\n";

		cout << GREEN "autoindex: \n" WHITE;
		cout << this->_server[i].getAutoIndex() << "\n";

		cout << GREEN "Error Page's: \n" WHITE;
		map<string, string> &mp = this->_server[i].getErrorPage();
		map<string, string>::iterator itt = mp.begin();
		for (; itt != mp.end(); itt++)
			cout << "|" << (*itt).first << "|"  << (*itt).second << "|" << "\n";
		_server[i].print();
		cout << BLUE "-----------------------" WHITE << "\n";
	}
}

// ! fix for now !
void	configFile::check_braces(string file)
{
	check_file(file, "read");
	ifstream	in(file.c_str());
	string		line;

	while (getline(in, line))
		_full_file += line + "\n";
	int c = 0;
	int d = 0;
	for (size_t i = 0; i < _full_file.length(); i++)
	{
		if (_full_file[i] == '{')
			c++;
		else if (_full_file[i] == '}')
			d++;
	}
	if (c != d)
		throw(curlyBracesError());
}
