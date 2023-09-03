#include "serverClass.hpp"

server::server()
{
	this->setErrorPage("200", "error_pages/200.html");
	this->setErrorPage("201", "error_pages/201.html");
	this->setErrorPage("501", "error_pages/501.html");
	this->setErrorPage("400", "error_pages/400.html");
	this->setErrorPage("403", "error_pages/403.html");
	this->setErrorPage("404", "error_pages/404.html");
	this->setErrorPage("405", "error_pages/405.html");
	this->setErrorPage("413", "error_pages/413.html");
	this->setErrorPage("414", "error_pages/414.html");
	this->setErrorPage("301", "error_pages/301.html");
	this->setErrorPage("500", "error_pages/500.html");
	this->setErrorPage("505", "error_pages/505.html");
	this->setErrorPage("204", "error_pages/204.html");
	_port = "8080";
	_host = "127.0.0.1";
	_client_max_body_size = "1000000";
	_root = "./";
	//_server_name;
	//_virtual_servers;
	//_err;
	//_locations;
	//_error_pages;
	//_index;
}


void	server::getLocationContext(ifstream &in, string line, int *err)
{
	location	loc;
	string		key = "", value = "";
	line.erase(0, line.find_first_not_of("	 "));
	line.erase(line.find_first_not_of(" 	"), line.find_first_of("	 "));
	line.erase(remove(line.begin(), line.end(), '{'), line.end());
	count_argument(line, 1);
	line.erase(remove(line.begin(), line.end(), ' '), line.end());
	line.erase(remove(line.begin(), line.end(), '	'), line.end());
	loc.setPath(line);
	if (loc.getPath()[0] == '=')
	{
		loc.setEqual("true");
		loc.getPath().erase(0, 1);
	}
	loc.setRoot(_root);
	while (getline(in, line))
	{
		(*err)++;
		int	returnValue = clean_line(line, key, value);
		if (returnValue == 1)
			continue;
		else if (returnValue == 2)
			break;
		if (key == "allow_methods")
			get_allow_methodes(value, loc.getAllowMethodes());
		else if (key == "return" && count_argument(value, 1))
		{
			if (get_return(key, value, loc.getReturn(), in, line, loc.getRedirection()))
				break;
		}
		else if (key == "root")
			loc.setRoot(count_argument(value, 1) ? value : "");
		else if (key == "index")
			get_multiple_args(value, loc.getIndex());
		else if (key == "autoindex")
			loc.setAutoIndex(count_argument(value, 1) && (value == "off" || value == "on")? value : throw (unvalidDirective()));
		else if (key == "upload_pass")
			loc.setUploadsPath(count_argument(value, 1) ? value : "");
		else if (key == "cgi_pass")
		{
			count_argument(value, 2);
			string	ext;
			string	path;

			ext = value.substr(0, value.find_first_of("	 "));
			value.erase(0, value.find_first_of("	 "));
			value.erase(remove(value.begin(), value.end(), ' '), value.end());
			value.erase(remove(value.begin(), value.end(), '	'), value.end());
			path = value;
			loc.setCgiPass(ext, path);
			loc.setHasCgi(1);
		}
		else if (key == "cgi_path")
			loc.setCgiPath(count_argument(value, 1) ? value : "");
		else
			throw(unvalidDirective());
	}
	this->_locations.push_back(loc);
}

void server::print()
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		cout << RED "Location " << i + 1 << " -----------------------" WHITE << "\n";

		cout << GREEN "Path: \n" WHITE;
		cout << "|" + _locations[i].getPath() + "|" << "\n";

		cout << GREEN "redirection: \n" WHITE;
		cout << _locations[i].getRedirection() << "\n";

		cout << GREEN "Equal occurence: \n" WHITE;
		cout << _locations[i].getEqual() << "\n";

		cout << GREEN "allowed methode: \n" WHITE;
		vector<string> &vec2 = _locations[i].getAllowMethodes();
		for (size_t i = 0; i < vec2.size(); i++)
			cout << "|" << vec2[i] << "|" << "\n";

		cout << GREEN "Root: \n" WHITE;
		cout << _locations[i].getRoot() << "\n";

		cout << GREEN "Index's: \n" WHITE;
		vector<string> &vec3 = _locations[i].getIndex();
		for (size_t i = 0; i < vec3.size(); i++)
			cout << "|" << vec3[i] << "|" << "\n";

		cout << GREEN "autoindex: \n" WHITE;
		cout << _locations[i].getAutoIndex() << "\n";

		cout << GREEN "upload path: \n" WHITE;
		cout << _locations[i].getUploadPath() << "\n";

		cout << GREEN "return: \n" WHITE;
		cout << _locations[i].getReturn() << "\n";

		cout << GREEN "CGI Pass: \n" WHITE;
		map<string, string>	obj = _locations[i].getCgiPass();
		for (map<string, string>::iterator itt = obj.begin(); itt != obj.end(); itt++)
			cout << "|lang = " << itt->first << "|Path = " << itt->second << "\n";

		cout << GREEN "CGI Path: \n" WHITE;
		cout << _locations[i].getCgiPath() << "\n";
	}
}

// void	server::checkHostPort()
// {
// 	memset(&hint, 0, sizeof(hint));
    
//     hint.ai_family = AF_INET;
//     hint.ai_socktype = SOCK_STREAM;
//     if (getaddrinfo(this->getHost().c_str(), this->getPort().c_str(), &hint, &res))
//     {
//         // freeaddrinfo(res); // sigfault when free res
// 		throw runtime_error("ERROR : Can't resolve hostname\n");
//     }
// 	freeaddrinfo(res);
// }

void server::CreateSocket(server servers)
{
    memset(&hint, 0, sizeof(hint));
    
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    int yes = 1;

    if (getaddrinfo(servers.getHost().c_str(), servers.getPort().c_str(), &hint, &res))
    {
		//cout << getaddrinfo(servers.getServerName().c_str(), servers.getPort().c_str(), &hint, &res) << std::endl;
		//throw runtime_error("ERROR : Can't resolve hostname");
		//exit(1);
        std::cerr << "getaddrinfo() failed" << std::endl;
        freeaddrinfo(res);
        return;
    }

    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	std::cout << "server_socket: " << server_socket << std::endl;
    if (server_socket == -1) {
        perror("socket");
        freeaddrinfo(res);
        return;
    }

    // if ((server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
    // {
    //     std::cerr << "socket() failed" << std::endl;
    //     exit(1);
    // }
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Failed to set socket to non-blocking mode" << std::endl;
        close(server_socket);
        freeaddrinfo(res);
        return;
    }

	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    {
        std::cout << "setsocket failed" << std::endl;
        close(server_socket);
        freeaddrinfo(res);
        return;
    }
    if (bind(server_socket, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("bind");
        close(server_socket);
        freeaddrinfo(res);
        return;
    }
	
    freeaddrinfo(res);
    
    if (listen(server_socket, 400) == -1)
    {
        std::cerr << "listen() failed" << std::endl;
        close(server_socket);
        return;
    }
}
