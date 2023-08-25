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
	// setenv("REMOTE_ADDR", client.get, 1);
	setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
	setenv("REQUEST_URI", req.getMethod().c_str(), 1);
	// client.getlocation().getRoot() + '/' + req.getURI();
	setenv("SCRIPT_NAME", client.getlocation().getIndex()[0].c_str(), 1);
	setenv("SERVER_PORT", client.getServer().getPort().c_str(), 1);
	setenv("SERVER_PROTOCOL", ("HTTP/" + req.getVersion()).c_str(), 1);
	setenv("SERVER_NAME", client.getServer().getHost().c_str(), 1);
	setenv("SERVER_SOFTWARE", (client.getServer().getServerName() + req.getVersion()).c_str() , 1);
	setenv("GATEWAY_INTERFACE", "1.1", 1);
}

void cgi_handler(Request& req, Client& client, string scritpPath)
{
	int pid;
	// int status;
	// int savedfd[2];
	FILE* infd = tmpfile();
	FILE* outfd = tmpfile();
	// string	scritpPath = client.getlocation().getRoot() + '/' + req.getURI() + '/' + client.getlocation().getIndex()[0];
	// string	scritpPath = src;

	if (access(scritpPath.c_str(), R_OK))
		return (client.setStatus(500), perror(scritpPath.c_str()));
	if (access(client.getlocation().getCgiPass()[0].path.c_str(), R_OK))
		return (client.setStatus(500), perror(client.getlocation().getCgiPass()[0].path.c_str()));
	client.setCgiFd(fileno(outfd));
	setCgiEnv(req, client);

	if ((pid = fork()) == -1)
		return (client.setStatus(500), perror("fork"));
	if (!pid)
	{
		char	*tab[3] = {strdup(client.getlocation().getCgiPass()[0].path.c_str()), strdup(scritpPath.c_str()), 0};
		write(fileno(infd), req.getBody().c_str(), req.getBody().length());
		dup2(fileno(infd), STDIN_FILENO);
		dup2(fileno(outfd), STDOUT_FILENO);
		execve(tab[0], tab, client.getServer().getEnv());
		perror("execve");
		exit(1);
	}

	// if (waitpid(-1, &status, WNOHANG) == -1)
	// 	client.setStatus(500);
	// else if (waitpid(-1, &status, WNOHANG))
	// {
	// 	client.setCgiFd(fileno(outfd));
	// 	client.setState(DONE);
	// }
	// else
	// 	client.setState(WAITING_CGI);
}

// int main()
// {
// 	cgi_handler();
// 	return (0);
// }