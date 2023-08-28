#pragma once
#include "../../HTTPServer.hpp"

class CGI
{
	private:
		map<string, string> _env;
		Client				_client;
	public:
		CGI(){};
		~CGI(){};
		void	setCgiEnv(Request req, Client &client, string scriptName, string scritpPath);
		void	cgi_executor(Client& client, string scritpPath, string requestFile, string interpreter);
		char	**getCgiEnv();
};
