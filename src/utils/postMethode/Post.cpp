#include "../configParse/configFile.hpp"
#include "../../HTTPServer.hpp"
#include <sys/types.h>
#include <dirent.h>
#include "../CGI/CGI.hpp"

string	generateName()
{
	string final = "";
	srand((unsigned)time(0));
	for (int i = 0; i < 20; i++)
		final += 97 + rand() % 25;
	return (final);
}

string getIndexFromDirectory(Client& client, string directory)
{
	struct dirent	*de;
	DIR*			dir = opendir(directory.c_str());
	bool			has_index = 0;
	location		loc = client.getlocation();

	if (!dir)
	{
		perror("opendir");
		client.setStatus(500);
	}
	while ((de = readdir(dir)) != 0 && !has_index)
	{
		stringstream ss;

		ss << de->d_name;
		vector<string>::iterator itt = find(loc.getIndex().begin(), loc.getIndex().end(), ss.str());
		if (itt != loc.getIndex().end())
		{
			closedir(dir);
			return (directory + *itt);
		}
	}
	closedir(dir);
	return ("null");
}

void	dir_has_index_file(Client& client, location loc, Request req)
{
	string	index = getIndexFromDirectory(client, client.getlocation().getRoot() + '/' + req.getURI());
	cout << "**************** check if directory has index file ****************\n";
	if (index == "null" || !loc.isCgi())
		client.setStatus(403);
	else
	{
		client.setState(WAITING_CGI);
		run_cgi(client, index);
		cout << "*************** CGI EXECUTED***************\n\n";
	}
}

void	writeToNewFile(Client &client)
{
	int		rd;
	char	buffer[2048];

	memset(buffer, 0, 2048);
	rd = read(client.uploadedInFile, buffer, 2047);
	if (rd > 0)
		write(client.uploadedOutFile, buffer, rd);
	if (!rd)
	{
		if (client.uploadedOutFile != -1)
			close(client.uploadedOutFile);
		if (client.uploadedInFile != -1)
			close(client.uploadedInFile);
		client.setStatus(201);
		client.err = 0;
		client.setState(FILE_READING);
		check_errors(client, client.getStatus());
	}
	if (rd < 0)
	{
		if (client.uploadedOutFile != -1)
			close(client.uploadedOutFile);
		if (client.uploadedInFile != -1)
			close(client.uploadedInFile);
		client.setStatus(201);
		client.err = 0;
		client.setState(FILE_READING);
		check_errors(client, client.getStatus());
	}
}

void Post(Request& req, location& loc, Client &client)
{
	if (find(client.getlocation().getAllowMethodes().begin(), client.getlocation().getAllowMethodes().end(), "POST") == client.getlocation().getAllowMethodes().end())
		return (client.setStatus(405));
	cout << RED "**************** POST ****************\n" WHITE;

	// while (1)
		// cout << "haineg up\n";
	if (loc.getUploadPath()[0] == '/')
		loc.getUploadPath().erase(0, 1);
	//? location support upload
	if (loc.getUploadPath().length() > 0)
	{
		string	uploadDir = loc.getRoot() + '/' + loc.getUploadPath();
		if (access(uploadDir.c_str(), W_OK))
		{
			perror(uploadDir.c_str());
			return (client.setStatus(403));
		}
		string	random = generateName();
		string	extension = "." + req.getHeader("Content-Type").substr(req.getHeader("Content-Type").find('/') + 1);
		int file = open((uploadDir + "/" + random + extension).c_str(), O_CREAT | O_WRONLY, 0644);
		if (!file)
		{
			perror((uploadDir + "/" + random).c_str());
			return (client.setStatus(403));
		}
		client.uploadedOutFile = file;
		// ? check for empty file
		lseek(client.uploadedInFile, 0, SEEK_SET);
		char buffer[5];
		int rd = read(client.uploadedInFile, buffer, 5);
		if (!rd)
		{
			if (client.uploadedOutFile != -1)
				close(client.uploadedOutFile);
			if (client.uploadedInFile != -1)
				close(client.uploadedInFile);
			rename((uploadDir + "/" + random + extension).c_str(), ("/tmp/file_" + random).c_str());
			client.setStatus(400);
		}
		else {
			lseek(client.uploadedInFile, 0, SEEK_SET);
			client.setState(MOVING_FILE);
		}
	}
	//? location doesn't support upload
	else
	{
		if (get_resource_type((loc.getRoot() + '/' + req.getURI()).c_str(), client) == "FILE") // * is file
		{
			if (loc.isCgi())
			{
				client.setState(WAITING_CGI);
				run_cgi(client, loc.getRoot() + '/' + req.getURI());
				cout << GREEN "*************** CGI EXECUTED ***************\n\n" RESET;
			}
			else
			{
				cout << "does not have cgi\n";
				client.setStatus(403);
			}
		}
		else if (get_resource_type((loc.getRoot() + '/' + req.getURI()).c_str(), client) == "DIRE") // * is dir
		{
			if (req.getURI()[req.getURI().length() - 1] != '/')
			{
				cout << "Moved Permanetely\n";
				client.setStatus(301); // ? 301 Moved Permanetely
			}
			else
				dir_has_index_file(client, loc, req); // * uri has '/' in the end
		}
		else
		{
			cout << "404 Not Found\n";
			client.setStatus(404);
		}
	}
}
