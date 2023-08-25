#include "../configParse/configFile.hpp"
#include "../../HTTPServer.hpp"
#include <sys/types.h>
#include <dirent.h>

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
		client.setStatus(201); // ? run cgi
		cgi_handler(req, client, index);
		cout << "CGI EXECUTED***************\n\n";
	}
}


// ! test body

string b = "<!DOCTYPE html>\n\
<html>\n\
<head>\n\
    <title>Hello, World!</title>\n\
</head>\n\
<body>\n\
    <h1>Hello, World!</h1>\n\
    <p>This is a simple HTML page.</p>\n\
</body>\n\
</html>";

void Post(Request& req, location& loc, Client &client)
{
	string body = b;
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
		ofstream file((uploadDir + "/" + random).c_str());
		if (!file)
		{
			perror((uploadDir + "/" + random).c_str());
			return (client.setStatus(403));
		}
		file.write(body.c_str(), body.length());
		client.setStatus(201);
	}
	//? location doesn't support upload
	else if (req.getURI().length() > 0)
	{
		if (get_resource_type((loc.getRoot() + '/' + req.getURI()).c_str(), client) == "FILE") // * is file
		{
			if (loc.isCgi())
			{
				client.setStatus(201); // ? run cgi
				cgi_handler(req, client, loc.getRoot() + '/' + req.getURI());
				cout << "*************** CGI EXECUTED ***************\n\n";
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
	else
	{
		cout << "404 Not Found\n";
		client.setStatus(404);
	}
}
