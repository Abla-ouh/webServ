#include "../../HTTPServer.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <errno.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>

using namespace std;


void	setCgiEnv(Request req, Client client)
{
	
	setenv("CONTENT_LENGTH", req.getHeader("Content-Length").c_str(), 1);
	setenv("CONTENT_TYPE", req.getHeader("Content-Type").c_str(), 1);
	setenv("AUTH_TYPE", "1.1", 1);
	setenv("PAHT_INFO", client.getlocation().getPath().c_str(), 1);
	setenv("PAHT_TRANSLATED", (client.getlocation().getRoot() + '/' + client.getlocation().getPath()).c_str(), 1);
	setenv("QUERY_STRING", req.getQuery().c_str(), 1);
	setenv("REMOTE_ADDR", client.get, 1);
	setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
	setenv("REQUEST_URI", req.getHeader("").c_str(), 1);
	setenv("SCRIPT_NAME", req.getHeader("").c_str(), 1);
	setenv("SERVER_PORT", client.getServer().getPort().c_str(), 1);
	setenv("SERVER_PROTOCOL", ("HTTP/" + req.getVersion()).c_str(), 1);

	setenv("SERVER_NAME", client.getServer().getHost().c_str(), 1);
	setenv("SERVER_SOFTWARE", (client.getServer().getServerName() + req.getVersion()).c_str() , 1);
	setenv("GATEWAY_INTERFACE", "1.1", 1);
}

void cgi_handler()
{
	// setCgiEnv(req, client);
	int pid;
	int pipedfd[2];
	int status;
	int savedfd[2];
	string file = "cgi.php";
	ofstream outFile("out");

	savedfd[0] = dup(STDIN_FILENO);
	savedfd[1] = dup(STDOUT_FILENO);
	if (pipe(pipedfd) == -1)
	{
		perror("pipe");
		return;
	}
	dup2(pipedfd[1], STDOUT_FILENO);
	dup2(pipedfd[0], STDIN_FILENO);
	if ((pid = fork()) == -1)
	{
		perror("fork");
		// return (client.setStatus(500));
		return;
	}
	if (!pid)
	{
		char *tab[3] = {strdup("/usr/bin/php-cgi"), strdup(file.c_str()), 0};
		// close(pipedfd[0]);
		execve(tab[0], tab, 0);
		perror("execve");
	}
	waitpid(-1, &status, 0);
	close(pipedfd[1]);
	dup2(savedfd[1], STDOUT_FILENO);
	string line;
	while (getline(cin, line))
		cout << line << "\n";
	dup2(savedfd[0], STDIN_FILENO);
}

int main()
{
	cgi_handler();
	return (0);
}