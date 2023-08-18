#include "serverClass.hpp"

server::server()
{
	_port = "8080";
	_host = "127.0.0.1";
	//_client_max_body_size;
	_root = "./";
	//_index_page;
	//_server_name;
	//_virtual_servers;
	//_err;
	//_locations;
	//_error_pages;
	//_index;
}


void	server::getLocationContext(ifstream &in, string line)
{
	location	loc;
	string		key = "", value = "";
	line.erase(0, line.find_first_not_of("	 "));
	line.erase(line.find_first_not_of(" 	"), line.find_first_of("	 "));
	loc.setPath(line.substr(line.find_first_not_of(" 	"), line.find_first_of("{") - 1));
	if (loc.getPath()[0] == '=')
	{
		loc.setEqual("true");
		loc.getPath().erase(0, 1);
	}
	while (getline(in, line))
	{
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
		else if (key == "client_max_body_size")
			loc.setClient_max_body_size(count_argument(value, 1) && check_number(value) ? value : "");
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
			cgi obj;

			obj.lang = value.substr(0, value.find_first_of("	 "));
			value.erase(0, value.find_first_of("	 "));
			value.erase(remove(value.begin(), value.end(), ' '), value.end());
			value.erase(remove(value.begin(), value.end(), '	'), value.end());
			obj.path = value;
			loc.setCgiPass(obj);
			loc.setHasCgi(true);
		}
		else if (key == "cgi_path")
			loc.setCgiPath(count_argument(value, 1) ? value : "");
		else if (key == "cgi_ext")
			loc.setCgiExt(count_argument(value, 1) ? value : "");
		else
		{
			cout << line << "\n";
			throw (unvalidDirective());
		}
	}
	this->_locations.push_back(loc);
}

void server::print()
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		cout << RED "Location " << i + 1 << " -----------------------" WHITE << "\n";

		cout << GREEN "Path: \n" WHITE;
		cout << _locations[i].getPath() << "\n";

		cout << GREEN "redirection: \n" WHITE;
		cout << _locations[i].getRedirection() << "\n";

		cout << GREEN "Equal occurence: \n" WHITE;
		cout << _locations[i].getEqual() << "\n";

		cout << GREEN "allowed methode: \n" WHITE;
		vector<string> &vec2 = _locations[i].getAllowMethodes();
		for (size_t i = 0; i < vec2.size(); i++)
			cout << "|" << vec2[i] << "|" << "\n";

		cout << GREEN "Client Max Body Size: \n" WHITE;
		cout << _locations[i].getClient_max_body_size() << "\n";

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
		vector<cgi>	obj = _locations[i].getCgiPass();
		for (size_t i = 0; i < obj.size(); i++)
			cout << "|lang = " << obj[i].lang << "|Path = " << obj[i].path << "\n";

		cout << GREEN "CGI Path: \n" WHITE;
		cout << _locations[i].getCgiPath() << "\n";

		cout << GREEN "CGI extension: \n" WHITE;
		cout << _locations[i].getCgiExt() << "\n";
	}
}

void	server::checkHostPort()
{
	memset(&hint, 0, sizeof(hint));
    
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(this->getServerName().c_str(), this->getPort().c_str(), &hint, &res))
    {
        // freeaddrinfo(res); // sigfault when free res
		throw runtime_error("ERROR : Can't resolve hostname\n");
    }
	freeaddrinfo(res);
}

void server::CreateSocket(server servers)
{
    memset(&hint, 0, sizeof(hint));
    
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    int yes = 1;

    if (getaddrinfo(servers.getServerName().c_str(), servers.getPort().c_str(), &hint, &res))
    {
		//cout << getaddrinfo(servers.getServerName().c_str(), servers.getPort().c_str(), &hint, &res) << std::endl;
		//throw runtime_error("ERROR : Can't resolve hostname");
		//exit(1);
        std::cerr << "getaddrinfo() failed" << std::endl;
        freeaddrinfo(res);
        return;
    }

	//cout<< "test" << endl;
    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
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
