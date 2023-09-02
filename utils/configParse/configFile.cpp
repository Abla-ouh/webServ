#include "configFile.hpp"

void	configFile::getServerContext(ifstream &in, string &line)
{
	string	key = "", value = "";
	server	serv;
	string	tmp;

	while (getline(in, line))
	{
		(*_err)++;
		tmp = line;
		_full_file += line + "\n";
		int returnValue = clean_line(line, key, value);
		if (returnValue == 1)
			continue;
		else if (returnValue == 2)
			break;
		if (line.find_first_of("{}") < line.length() && key != "location")
			throw (unvalidDirective());
		if (key == "root")
			serv.setRoot(count_argument(value, 1) ? value : "");
		else if (key == "listen")
			serv.setPort(count_argument(value, 1) && check_number(value) && check_range(value, 0, 65536) ? value : "");
		else if (key == "host")
			serv.setHost(count_argument(value, 1) && check_host(value) ? value : "");
		else if (key == "server_name")
			serv.setServerName(count_argument(value, 1) ? value : "");
		else if (key == "client_max_body_size")
			serv.setclient_max_body_size(count_argument(value, 1) && check_number(value) ? value : "");
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
		else if (key == "location" && value.find_first_of("{") < value.length())
			serv.getLocationContext(in, value.find_first_not_of(" 	{") < value.length() ? line : throw (unvalidDirective()), _err);
		else
			throw (unvalidDirective());
	}
	if (serv.getServerName().empty())
		throw(unvalidConfigFile());
	//serv.checkHostPort();
	serv.setEnv(_env);
	_server.push_back(serv);
}

// ? left
// TODO : check host range => fixed √
// TODO : check curly braces => fixed for now √
// TODO : location => get data done √
// TODO :

configFile::configFile(const string file, char **env, int *err) : _full_file(""), _env(env), _err(err)
{
	check_file(file, "read");
	check_braces(file);
	ifstream	in(file.c_str());
	string		line;

	for (; getline(in, line);)
	{
		(*_err)++;
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
	if (_server.empty())
		throw(unvalidConfigFile());
	lastCheck();
}

void	configFile::print()
{
	for (size_t i = 0; i < _server.size(); i++)
	{
		cout << YELLOW "Server " << i + 1 << " -----------------------" WHITE << "\n";
		cout << GREEN "Port: \n" WHITE;
			cout << "|" << _server[i].getPort() << "|" << "\n";

		cout << GREEN "Host's: \n" WHITE;
		cout << this->_server[i].getHost() << "\n";

		cout << GREEN "Client Max Body Size: \n" WHITE;
		cout << this->_server[i].getclient_max_body_size() << "\n";

		cout << GREEN "Server Name: \n" WHITE;
			cout << "|" << _server[i].getServerName() << "|" << "\n";

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
