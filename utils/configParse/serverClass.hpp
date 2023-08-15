#pragma once
#include "configFile.hpp"
#include "locationClass.hpp"
#include "cstring"
#include "exceptions.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define WHITE "\033[0;37m"


using namespace std;

class server {
	private:
		string				_port;
		string				_host;
		string				_server_name;
		string				_client_max_body_size;
		string				_root;
		vector<string>		_index;
		map<string, string>	_error_pages;
		vector<location>	_locations;
		string				_autoindex;
		vector<string>		_allow_methodes;
		struct addrinfo 	hint;
        struct addrinfo 	*res;
		int             	server_socket;
	public:
		server();
		~server(){};
		//?
		// ? seter's
		//?
		void			setPort(string port){_port = port;};
		void			setAllowMethodes(string allow_methodes){_allow_methodes.push_back(allow_methodes);};
		void			setHost(string host){_host = host;};
		void			setServerName(string serverName){_server_name = serverName;};
		void			setclient_max_body_size(string client_max){_client_max_body_size = client_max;};
		void			setRoot(string root){_root = root;};
		void			setErrorPage(string key, string value){
							pair<map<string, string>::iterator, bool> ret;
							ret = _error_pages.insert(std::pair<string, string>(key, value));
							if (!ret.second)
								ret.first->second = value;
						};
		//void			setLocations();
		void			setAutoIndex(string autoindex){_autoindex = autoindex;};
		void			setIndex(string path){_index.push_back(path);};
		//?
		// ? geter's
		//?
		string					getPort(){return (_port);};
		string&					getHost(){return (_host);};
		string&					getclient_max_body_size(){return (_client_max_body_size);};
		string&					getRoot(){return (_root);};
		string					getServerName(){return (_server_name);};
		vector<string>&			getAllowMethodes(){return (_allow_methodes);};
		string&					getAutoIndex(){return (_autoindex);};
		map<string, string>&	getErrorPage(){return (_error_pages);};
		vector<string>&			getIndex(){return (_index);};
		void					getLocationContext(ifstream &in, string line);
		int						getServerSocket() { return server_socket;};
		void					print();
        void    				CreateSocket(server servers);
};


int	count_argument(string value, int count);